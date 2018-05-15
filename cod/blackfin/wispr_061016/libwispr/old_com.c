/*
 *  com.c: serial com interface
 *
 *  Combination of tty command console and message handling functions
 *  For docs on how to set serial port see 
 *    http://www.easysw.com/~mike/serial/serial.html
 *
 * Commands strings start with a $ and end with a *.
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
 * Embedded Ocean Systems (EOS), 2014
*/

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

//#define DEBUG

static int tty_fd;
static int tty_buffer_len;
static int tty_buffer_full;
static char tty_buffer[COM_BUFSIZE];
static struct termios tio;
static struct termios otio;	// will be used to save old port settings
static struct sigaction saio;	// set the serial interrupt handler
static speed_t tty_baudrate;
static char tty_device[32];

static int tty_buffer_locked = 0;  // semiphor to lock buffer while reading or writing.

// interupt signal handler
void com_signal_handler (int status);

extern int verbose_level;

#define RTC_DEVICE  "/dev/rtc0"

//---------------------------------------------------------
/* Open com port
 * Serial port is used in cannonical mode
*/

int com_open (char *device, speed_t baudrate)
{
  int fd;

  // open tty console
  fd = open (device, O_RDWR | O_NOCTTY | O_NONBLOCK);
  if (fd < 0) {
    log_printf( "com_open: unable to open serial port (%s)\n",
	     device);
    return (0);
  }

  tcgetattr (fd, &tio);		// get current port settings
  tcgetattr (fd, &otio);	// save current port settings
  cfsetispeed (&tio, baudrate);
  tio.c_cflag &= ~PARENB;	// no parity
  tio.c_cflag &= ~CSTOPB;	// one stop bit
  tio.c_cflag &= ~CSIZE;	/* Mask the character size bits */
  tio.c_cflag |= CS8;		/* Select 8 data bits */
  tio.c_cflag &= ~CRTSCTS;	// disable hdwr flow cntrl
  tio.c_lflag |= (ICANON);	// canonical input
  tio.c_lflag &= ~(ECHO | ECHOE);	// canonical input
  //tio.c_lflag |= (ICANON | ECHO | ECHOE); // canonical input
  tio.c_lflag |= (ISIG);	// enable input signal
  //tio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // raw input
  //tio.c_iflag |= (INPCK | ISTRIP); // parity checks
  //tio.c_iflag |= (ICRNL); // map CR to NL
  tio.c_iflag &= ~(IXON | IXOFF | IXANY);	// disable soft flow cntrl
  tio.c_oflag &= ~OPOST;	// raw output
  //tio.c_cc[VMIN] = 0;   // use-vtim, raw input onlye
  //tio.c_cc[VTIME] = 1;  // time to wait until exiting read

  /* now clean the modem line and activate the settings for the port */
  tcflush (fd, TCIFLUSH);	// flush old data
  tcsetattr (fd, TCSANOW, &tio);	// apply new settings
  //tcflush(fd, TCIOFLUSH);

  /* allow the process to receive SIGIO */
  fcntl (fd, F_SETOWN, getpid ());
  /* Make the file descriptor asynchronous */
  fcntl (fd, F_SETFL, FASYNC);

  /* install the signal handler before making the device asynchronous */
  saio.sa_handler = com_signal_handler;
  sigemptyset (&saio.sa_mask);	// clear existing settings
  saio.sa_flags = 0;
  saio.sa_restorer = NULL;
  sigaction (SIGIO, &saio, NULL);

  // save local variables
  tty_fd = fd;
  strncpy (tty_device, device, 32);
  bzero (tty_buffer, COM_BUFSIZE);	// Initialize to all zeros
  tty_buffer_full = 0;
  tty_baudrate = baudrate;

  tty_buffer_locked = 0;

  return (fd);
}


//---------------------------------------------------------
/* Interupt function is called whenever there is new data 
 * to be read off the serial port.
 * The interupt routine reads max buffer size from serial port device 
 * (cannonical mode) and puts the message into a buffer (tty_buffer).
 * It should execute quickly, so the data processing is not done here.
 * Call com_read_msg to get the next message in the buffer.
*/
void com_signal_handler (int status)
{
  char buf[COM_MSGSIZE];
  int n, m;

  // read port
  n = read (tty_fd, buf, COM_MSGSIZE);

  // return if buffer is locked
  if(tty_buffer_locked) {
	  if(n > 0) log_printf("com_signal_handler: missed message %s\n", str);
	  else log_printf("com_signal_handler: unknown signal\n");
	  return(0);
  }
	  
  if (n > 0) {
    tty_buffer_locked = 1;   // lock tty_buffer
    if (buf[0] == '$') {	// it's a valid message
      //buf[n-1] = '*'; //  add terminating char
      buf[n - 1] = 0x00;	// replace <CR>
      m = strlen (tty_buffer);
      //log_printf( "com sig: %d, %d, %s\n", m, n, buf);
      if ((m + n) < COM_BUFSIZE - 1) {
	    // append new data to the buffer of unprocessed data
	    strcat (tty_buffer, buf);
	    tty_buffer_len += n;
	    tty_buffer_full = 0;
      }
      else {
	    // buffer full
	    tty_buffer_full = 1;
	    log_printf("com_signal_handler: buffer full\n");
      }
      //log_printf("com sig: %d, buffer %s\n", 
      //   strlen(tty_buffer), tty_buffer);
    }
    tty_buffer_locked = 0;  // unlock tty_buffer
  }
}

//---------------------------------------------------------
int com_close ()
{
  // restore the old port settings before quitting
  //tcflush(tty_fd, TCIFLUSH);      // flush data
  tcsetattr (tty_fd, TCSANOW, &(otio));
  close (tty_fd);
  return 0;
}

//---------------------------------------------------------
/*
* Copy the next message in the tty_buffer into 'msg'.
* Commands strings in the tty_buffer start with a '$' and end with a '*'.
* This function will strip off the '$' and '*' from the orginal message in the, 
* and copy the chars between the '$' and '*' into 'msg'.
* Then it shifts the rest of the messages in tty_buffer forward.
* Removes any junk in the buffer (text without a $ prefix).
*/
int com_read_msg (char *msg, int msglen)
{
  int n, m;
  char tmp[COM_BUFSIZE], *start, *tail;

  // return if buffer is locked
  if(tty_buffer_locked) return(0);

  // return if no msg in buffer
  n = strlen (tty_buffer);
  if (n <= 0) return (0);

  // make a copy
  strcpy (tmp, tty_buffer);

  // find start, end, and size of first msg in buffer
  start = strchr (tmp, '$');	// find start of msg
  tail = strchr (tmp, '*');	// find end of msg
  m = (int) (tail - start - 1);	// size of message, less '$' and '*' chars
  if (m <= 0) return (0);

  // copy first message into msg buffer, skip '$' char
  strncpy (msg, start + 1, m);

  // terminate msg
  msg[m] = 0x00;

  // copy the rest back onto buffer
  strcpy (tty_buffer, tail + 1);

  if(verbose_level > 1) log_printf( "com_read_msg: %s\n", msg);
  //log_printf( "start=%d, tail=%d, m=%d\n", start, tail, m);

  return (m);
}


//---------------------------------------------------------
void com_reset ()
{
	if(tty_fd <= 0) return;

	com_close(tty_fd);
	tty_fd = com_open(tty_device, tty_baudrate);
    
	//fprintf(stdout, "com_reset\n");

}

//---------------------------------------------------------
int com_write_msg (char *msg, int n)
{
  int len, m;
  char obuf[COM_MSGSIZE];
  obuf[0] = '$';
  strncpy (obuf + 1, msg, n);
  obuf[n + 1] = '*';
  //obuf[n+2] = 0x0d;   // <CR> after msg 
  obuf[n + 2] = 0x0a;	// <LF> newline 
  obuf[n + 3] = 0x00;	// terminate the string
  len = strlen (obuf);
  //len = n+2; 
  m = write (tty_fd, obuf, len);
  if(verbose_level > 1) log_printf( "com_write_msg: %d, %s", m, obuf);
  if (m < 0) {
    log_printf( "com_write_msg: error%d, %s", m, obuf);
    return 0;
  }
  //tcflush(tty_fd, TCIFLUSH);      // flush data

  return (m);
}

//---------------------------------------------------------
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

//---------------------------------------------------------
// Add your own message parsing here
//
int com_parse_msg (char *buf, int len, com_t * com)
{
  char msg[6], attr[128], tmstr[32];
  float lat, lon;
  time_t sec = 0;

  // remember that there is no '$' at the front of buf
  msg[0] = buf[0];
  msg[1] = buf[1];
  msg[2] = buf[2];
  msg[3] = 0;

  buf[len + 1] = 0;
  strcpy (attr, &buf[4]);

  if (verbose_level) {
    log_printf("com_parse_msg: %s,%s\n", msg, attr);
  }

  // Add special user commands here
  // For example ...

  if (strncmp (msg, "EXI", 3) == 0) {  // Exit command  
    com->state = COM_EXIT;
  }
  if (strncmp (msg, "RUN", 3) == 0) {  // Run command
    com->state = COM_RUN;
  }
  if (strncmp (msg, "PAU", 3) == 0) {  // Pause command
    com->state = COM_PAUSE;
  }
  if (strncmp (msg, "RES", 3) == 0) {  // Reset command
    com->state = COM_RESET;
  }
  if (strncmp (msg, "SLP", 3) == 0) {  // Sleep command
    com->state = COM_SLEEP;
  }

  // GPS message
  if (strncmp (msg, "GPS", 3) == 0) {
    sscanf (attr, "%ld,%f,%f", &sec, &lon, &lat);
    com->gps.time = sec;
    com->gps.lon = lon;
    com->gps.lat = lat;
    rtc_set_time(RTC_DEVICE, sec); // set RTC time
    set_system_time(sec, 0);	// set linux system time here
    com->mode = COM_GPS;	// new gps update
    if(verbose_level) {
      ctime_r(&sec, tmstr);
      log_printf("com_parse_msg: gps, %s", tmstr);
      log_printf("com_parse_msg: gps, lat=%f, lon=%f\n", lat, lon);
    }
  }

  // set time, if new time avaiable
  if (sec > 0) {
    com->time = sec;
    com->mode = COM_TIME;	// new time value
  }

  if (verbose_level) {
    log_printf("com_parse_msg: mode=0x%x, state=%d\n",
      com->mode, com->state);
  }

  return (com->state);
}

//---------------------------------------------------------


// ------------------------------------------------------------------------

pthread_mutex_t com_mutex = PTHREAD_MUTEX_INITIALIZER;

void *com_handle_msg (void *arg)
{
  com_t *com = (com_t *) arg;

  int nrd;
  char device[32];
  int fd;
  char msg[COM_MSGSIZE];
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
    nrd = com_read_msg (msg, COM_MSGSIZE);

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


