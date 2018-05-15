/*
 *  com.c: UART serial com interface
 *
 *  Combination of tty command console and message handling functions.
 *  Serial port is used in Asynchronous Canonical mode
 *
 * ------
 * THIS SOFTWARE IS PROVIDED BY EOS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL EOS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Embedded Ocean Systems (EOS), 2015
 *
 *----------------------------------------------------------------
*/

THIS IS UNFINSHED

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <sys/statfs.h>

#include "wispr.h"
#include "com.h"
#include "rtc.h"
#include "log.h"

static int tty_fd = 0;

static struct termios new_tio;
static struct termios old_tio;	// will be used to save old port settings
static struct sigaction tty_saio;	// set the serial interrupt handler
static speed_t tty_baudrate;
static char tty_device[32];

//  Mutex to lock buffer while reading or writing.
pthread_mutex_t com_mutex = PTHREAD_MUTEX_INITIALIZER;

extern int verbose_level;

#define RTC_DEVICE  "/dev/rtc0"

// flag set in the signal handler to indicate that a message 
// has been received or sent
static int com_message_received = 0;
static int com_message_sent = 0;

//---------------------------------------------------------------------
/* Interupt function is called whenever there is new message 
 * to be read from the serial port (using cannonical mode).
*/
void com_signal_handler(int status)
{
  if(com_message_sent > 0) {
    // decrement the message write flag
    if(verbose_level > 3) 
      log_printf( "com_signal_handler: %d sent\n", com_message_sent);
    com_message_sent--;	
  } else {
    // increment the message read flag
    com_message_received++;	  
    if(verbose_level > 3) 
       log_printf( "com_signal_handler: %d received\n", com_message_received);
  }  
}

//---------------------------------------------------------------------
/* Open com port
 * Serial port is used in Asynchronous Canonical mode
 */

int com_open (char *device, speed_t baudrate)
{
  int fd;

  // open tty console
  fd = open (device, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd < 0) {
    log_printf( "com_open: unable to open serial port (%s)\n",
	     device);
    return (0);
  }

  tcgetattr (fd, &old_tio);	// save current port settings
  tcgetattr (fd, &new_tio);		// get current port settings
  cfsetispeed (&new_tio, baudrate);
  
  //new_tio.c_cflag &= ~PARENB;	// no parity
  //new_tio.c_cflag &= ~CSTOPB;	// one stop bit
  //new_tio.c_cflag &= ~CSIZE;	/* Mask the character size bits */
  //new_tio.c_cflag |= CS8;		/* Select 8 data bits */
  //new_tio.c_cflag &= ~CRTSCTS;	// disable hdwr flow cntrl
  //new_tio.c_lflag |= (ICANON);	// canonical input
  //new_tio.c_lflag &= ~(ECHO | ECHOE);	// canonical input
  ////new_tio.c_lflag |= (ICANON | ECHO | ECHOE); // canonical input
  //new_tio.c_lflag |= (ISIG);	// enable input signal
  ////new_tio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // raw input
  ////new_tio.c_iflag |= (INPCK | ISTRIP); // parity checks
  ////new_tio.c_iflag |= (ICRNL); // map CR to NL
  //new_tio.c_iflag &= ~(IXON | IXOFF | IXANY);	// disable soft flow cntrl
  //new_tio.c_oflag &= ~OPOST;	// raw output
  //new_tio.c_cc[VMIN] = 1;   // use-vtim, raw input onlye
  //new_tio.c_cc[VTIME] = 0;  // time to wait until exiting read

  new_tio.c_cflag = CS8 | CLOCAL | CREAD;
  new_tio.c_iflag = IGNPAR | ICRNL;                                           
  new_tio.c_oflag = 0;                                                        
  new_tio.c_lflag = ICANON;                                                   
  new_tio.c_cc[VMIN] = 0; 
  new_tio.c_cc[VTIME] = 0;

  /* now clean the modem line and activate the settings for the port */
  tcflush (fd, TCIFLUSH);	// flush old data
  tcsetattr (fd, TCSANOW, &new_tio);	// apply new settings
  tcflush(fd, TCIOFLUSH);

  /* install the signal handler before making the device asynchronous */
  tty_saio.sa_handler = com_signal_handler;
  sigemptyset (&tty_saio.sa_mask);	// clear existing settings
  tty_saio.sa_flags = 0;
  tty_saio.sa_restorer = NULL;
  sigaction (SIGIO, &tty_saio, NULL);

  /* allow the process to receive SIGIO */
  fcntl (fd, F_SETOWN, getpid());
  /* Make the file descriptor asynchronous */
  fcntl (fd, F_SETFL, FASYNC | FNDELAY);  

  /* Make the file descriptor non-blocking */
  fcntl (fd, F_SETFL, FNDELAY);  

  // save local variables
  tty_fd = fd;
  strncpy (tty_device, device, 32);
  tty_baudrate = baudrate;

  return (fd);
}

//---------------------------------------------------------------------
int com_close ()
{
  // restore the old port settings before quitting
  //tcflush(tty_fd, TCIFLUSH);      // flush data
  tcsetattr (tty_fd, TCSANOW, &(old_tio));
  close (tty_fd);

  // destroy the mutex
  //pthread_mutex_destroy(&com_mutex);
  
  return 0;
}

//---------------------------------------------------------------------
void com_reset ()
{
  if(tty_fd <= 0) return;

  com_close(tty_fd);
  tty_fd = com_open(tty_device, tty_baudrate);
  
  if(verbose_level > 2) fprintf(stdout, "com_reset\n");
 
}

//---------------------------------------------------------------------
/*
* Read the next message from the serial port.
* The message buffer (msg) must be at least COM_MESSAGE_SIZE bytes 
* Read will only return a full line of input. A line is by default terminated 
* by a NL (ASCII LF), an end of file, or an end of line character. 
* A CR will not terminate a line with the default settings.
* This read function will strip the prefix and suffix char from the read message.
* Example;
*  nrd = 37 bytes
*  tmp = $GPS,1420070460,19.000000,19.000000*n
*  msg = GPS,1420070460,19.000000,19.000000
*        0123456789012345678901234567890123456
*/
int com_read_msg (char *msg)
{
  int nrd = 0;
  char tmp[COM_MESSAGE_SIZE];  // input buffer
  int len;
  char *head, *tail;
  
  // clear message buffers
  memset(tmp, 0, COM_MESSAGE_SIZE);  
  len = strlen(msg);
  if(len > COM_MESSAGE_SIZE) len = COM_MESSAGE_SIZE;
  memset(msg, 0, len);  

  // not sure why this doesn't work, it would be better
  //fd_set readfs;    /* file descriptor set */  
  //FD_ZERO(&readfs);  
  //FD_SET(tty_fd, &readfs);  /* set testing for source */  
  //struct timeval Timeout;
  //Timeout.tv_usec = 0;  /* milliseconds */
  //Timeout.tv_sec  = 0;  /* seconds */ 
  //int ret = select(tty_fd, &readfs, NULL, NULL, &Timeout);
  //if(ret < 0) {
  //	  fprintf(stdout, "select error %d\n", ret);
  //	  perror("select");
  //}
  //if(FD_ISSET(tty_fd, &readfs)) {
  
//  if(com_message_received) {
	  
	// read the message from the port
	nrd = read (tty_fd, tmp, COM_MESSAGE_SIZE);

	if(nrd <= 0) fprintf(stdout, "nrd = %d\n", nrd);

	// check if it's a valid message 
	if(nrd > 0) {
	   //log_printf( "com_read_msg: %d, %s", nrd, tmp);
	   // find start, end, and size of the msg in buffer
       head = strchr(tmp, COM_MESSAGE_PREFIX);	// find start of msg
       tail = strchr(tmp, COM_MESSAGE_SUFFIX);	// find end of msg
       len = (int)(tail - head - 1);	// size of message
       if ((len > 0) && (len < COM_MESSAGE_SIZE)) {
          // copy message into msg buffer, skipping the prefix char
          strncpy (msg, head + 1, len);
		  // terminate with NULL, overwriting the suffix char
          msg[len] = 0x00;	
	   }
	}
	
	// signal that message has been read
//	com_message_received--;
  
//  }

  // return the length of the message, 
  // which will be 0 if it's not valid
  nrd = strlen(msg);
  if((verbose_level > 2) && (nrd > 0)) { 
	log_printf( "com_read_msg: %s, %d bytes, %d waiting\n", msg, nrd, com_message_received);
  }
  return (nrd);
}

//---------------------------------------------------------------------
int com_write_msg (char *msg)
{
  int len, nwrt;
  char obuf[COM_MESSAGE_SIZE];  // output buffer

  len = strlen(msg);  // length of the message

  // check to make sure message is not too long to fix in output buffer
  if(len > (COM_MESSAGE_SIZE - 4)) len = COM_MESSAGE_SIZE - 4;

  // copy msg into transmit buffer
  strncpy (obuf + 1, msg, len);  

  // Add prefix, suffix, and a NL to the end and null terminate
  obuf[0] = COM_MESSAGE_PREFIX;  // add prefix
  obuf[len + 1] = COM_MESSAGE_SUFFIX; // add suffix
  obuf[len + 2] = 0x0a;	// <LF> newline 
  obuf[len + 3] = 0x00;	// null terminate the string
  
  // set flag to tell the SIGIO interrupt that a write was made
  com_message_sent++;

  nwrt = write (tty_fd, obuf, strlen (obuf));
  if (nwrt < 0) {
    log_printf( "com_write_msg: error %d, %s", nwrt, obuf);
    return 0;
  }
  
  if(verbose_level > 2) log_printf( "com_write_msg: %d, %s", nwrt, obuf);
  //tcflush(tty_fd, TCIFLUSH);      // flush data

  return (nwrt);
}

//---------------------------------------------------------------------
int com_write_raw (char *msg, int len)
{
  int m;
//  char obuf[COM_MSGSIZE];
//  strncpy (obuf, msg, len);
  m = write (tty_fd, msg, len);
  //log_printf("tty_write_msg: %d, %s\n", m, obuf);
  if (m < 0) {
    log_printf( "com_write_msg: error %d", m);
    return 0;
  }
  //tcflush(tty_fd, TCIFLUSH);      // flush data

  return (m);
}

//---------------------------------------------------------------------
// Add your own message parsing here
//
int com_parse_msg (char *msg, int len, com_t * com)
{
  char msg_id[6], msg_args[COM_MESSAGE_SIZE-4], tmstr[32];
  float lat, lon;
  time_t sec = 0;

  // message identifiers are always 3 chars long 
  // including the '$' at the front
  msg_id[0] = msg[0];
  msg_id[1] = msg[1];
  msg_id[2] = msg[2];
  msg_id[3] = 0;

  if (verbose_level) {
    log_printf("com_parse_msg: %s\n", msg);
  }

  // Add special user commands here
  // For example ...

  if (strncmp (msg_id, "EXI", 4) == 0) {  // Exit command  
    com->state = COM_EXIT;
  }
  if (strncmp (msg_id, "RUN", 4) == 0) {  // Run command
    com->state = COM_RUN;
  }
  if (strncmp (msg_id, "PAU", 4) == 0) {  // Pause command
    com->state = COM_PAUSE;
  }
  if (strncmp (msg_id, "RES", 4) == 0) {  // Reset command
    com->state = COM_RESET;
  }
  if (strncmp (msg_id, "SLP", 4) == 0) {  // Sleep command
    com->state = COM_SLEEP;
  }

  // GPS message
  if (strncmp (msg_id, "GPS", 4) == 0) {
    strcpy (msg_args, &msg[4]);  // copy args
    sscanf (msg_args, "%ld,%f,%f", &sec, &lon, &lat);
    com->gps.time = sec;
    com->gps.lon = lon;
    com->gps.lat = lat;
    rtc_set_time(RTC_DEVICE, sec); // set RTC time
    set_system_time(sec, 0);	// set linux system time here
    com->mode = COM_GPS;	// new gps update
    if(verbose_level) {
      ctime_r(&sec, tmstr);
	  tmstr[strlen(tmstr)-1] = 0;  // get rid of NL
      log_printf("GPS: %s, lat=%f, lon=%f\n", tmstr, lat, lon);
    }
  }

  if (strncmp (msg_id, "DFP", 4) == 0) {
    com->mode = COM_STAT;
  }
  
  // set time, if new time avaiable
  if (sec > 0) {
    com->time = sec;
    com->mode = COM_TIME;	// new time value
  }

  if (verbose_level > 2) {
    log_printf("com_parse_msg: mode=0x%x, state=%d\n",
      com->mode, com->state);
  }

  return (com->state);
}

//---------------------------------------------------------------------


void *com_handle_msg (void *arg)
{
  com_t *com = (com_t *) arg;

  int nrd;
  char device[32];
  int fd;
  char msg[COM_MESSAGE_SIZE];
  int state, mode;

  strcpy (device, "/dev/ttyBF1");
  
  fd = com_open (device, B9600);
  if (fd < 0) {
    //perror ("com_open");
    log_printf( "com_handle_msg: error opening tty device\n");
    return (0);
    //exit(EXIT_FAILURE);
  }

  state = COM_RUN;
  mode = 0;
  pthread_mutex_lock (&com_mutex);
  com->state = state;
  com->mode = mode;
  pthread_mutex_unlock (&com_mutex);

  while (state > 0) {

    // check for serial port actions
    nrd = com_read_msg (msg);

    // parse msg , if available
    pthread_mutex_lock (&com_mutex);
    if (nrd > 0) {
      state = com_parse_msg (msg, nrd, com);
    }
    state = com->state;
    mode = com->mode;
    pthread_mutex_unlock (&com_mutex);

    // actions
    if (state == COM_EXIT) break;
    else sleep (1);

  }

  com_close ();

  return (NULL);
}


