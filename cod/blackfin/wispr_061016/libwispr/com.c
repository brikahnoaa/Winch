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

//static int tty_fd = 0;
//static struct termios new_tio;
//static struct termios old_tio;	// will be used to save old port settings
//static speed_t tty_baudrate;
//static char tty_device[32];

//static struct sigaction tty_saio;	// set the serial interrupt handler

//  Mutex to lock buffer while reading or writing.
//pthread_mutex_t com_mutex = PTHREAD_MUTEX_INITIALIZER;

extern int verbose_level;

#define RTC_DEVICE  "/dev/rtc0"


//---------------------------------------------------------------------
/* Open com port
 * Serial port is used in Asynchronous Canonical mode
 */
int com_open (com_t *com, char *device, speed_t baudrate)
{
   
  // open tty device as non-blocking
  int fd = open (device, O_RDWR | O_NOCTTY | O_NDELAY);
  com->fd = fd;
  if (fd < 0) {
    log_printf( "com_open: unable to open serial port (%s)\n", device);
    return (fd);
  }

  tcgetattr (fd, &com->old_tio);	// save current port settings
  tcgetattr (fd, &com->tio);	// get current port settings
  
  // serial port settings: see Serial-Programming-HOWTO.txt for details
  cfsetispeed (&com->tio, baudrate);
  com->tio.c_cflag = CS8 | CLOCAL | CREAD;
  com->tio.c_iflag = IGNPAR | ICRNL;                                           
  com->tio.c_oflag = 0;                                                        
  com->tio.c_lflag = ICANON;                                                   
  com->tio.c_cc[VMIN] = 0; 
  com->tio.c_cc[VTIME] = 0;

  /* Make the file descriptor non-blocking */
  fcntl (fd, F_SETFL, FNDELAY);  

  /* now clean the modem line and activate the settings for the port */
  tcflush (fd, TCIFLUSH);	// flush old data
  tcsetattr (fd, TCSANOW, &com->tio);	// apply new settings
  tcflush(fd, TCIOFLUSH);

  // save local variables
  com->fd = fd;
  strncpy (com->device, device, 32);
  com->baudrate = baudrate;

  return (fd);
}

//---------------------------------------------------------------------
int com_close (com_t *com)
{
  if(com->fd < 0) return 0;
  // restore the old port settings before quitting
  tcflush(com->fd, TCIFLUSH);      // flush data
  tcsetattr (com->fd, TCSANOW, &(com->old_tio));
  close (com->fd);  
  return 0;
}

//---------------------------------------------------------------------
void com_reset (com_t *com)
{
  if(com->fd < 0) return;
  com_close(com);
  com->fd = com_open(com, com->device, com->baudrate);
  if(verbose_level > 2) fprintf(stdout, "com_reset\n");
 
}

//---------------------------------------------------------------------
/*
* Read the next message from the serial port.
* The input message buffer (msg) must be at least COM_MESSAGE_SIZE bytes 
* In canonical mode read() will only return a full line of input. 
* A line is by default terminated by a NL (ASCII LF), an end of file, 
* or an end of line character. 
* A CR will not terminate a line with the default settings.
* This read function will strip the prefix and suffix char from the read message.
* Example;
*  nrd = 37 bytes
*  tmp = $GPS,1420070460,19.000000,19.000000*n
*  msg = GPS,1420070460,19.000000,19.000000
*        0123456789012345678901234567890123456
*/
int com_read_msg (com_t *com, char *msg)
{
  int nrd = 0;
  char tmp[COM_MESSAGE_SIZE];  // input buffer
  int len;
  char *head, *tail;
  int fd = com->fd;
  if(fd <= 0) return 0;

  // clear message buffers
  memset(tmp, 0, COM_MESSAGE_SIZE);  
  len = strlen(msg);
  if(len > COM_MESSAGE_SIZE) len = COM_MESSAGE_SIZE;
  memset(msg, 0, len);  
  
  // read the message from the port
  // because it's non-blocking, it will return -1 if nothing is available
  nrd = read (fd, tmp, COM_MESSAGE_SIZE);

  // otherwise something was read, so
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
	
  // return the length of the message, 
  // which will be 0 if it's not valid
  nrd = strlen(msg);
  if((verbose_level > 2) && (nrd > 0)) { 
	log_printf( "com_read_msg: %s, %d bytes\n", msg, nrd);
  }
  return (nrd);
}

//---------------------------------------------------------------------
int com_write_msg (com_t *com, char *msg)
{
  int len, nwrt;
  char obuf[COM_MESSAGE_SIZE];  // output buffer

  int fd = com->fd;
  if(fd <= 0) return 0;

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
  
  nwrt = write (fd, obuf, strlen (obuf));
  if (nwrt < 0) {
    log_printf( "com_write_msg: error %d, %s", nwrt, obuf);
    return 0;
  }
  
  if(verbose_level > 2) log_printf( "com_write_msg: %d, %s", nwrt, obuf);
  //tcflush(tty_fd, TCIFLUSH);      // flush data

  return (nwrt);
}

//---------------------------------------------------------------------
int com_write_raw (com_t *com, char *msg, int len)
{
  int m;
  int fd = com->fd;
  if(fd <= 0) return 0;
//  char obuf[COM_MSGSIZE];
//  strncpy (obuf, msg, len);
  m = write (fd, msg, len);
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
int com_parse_msg (com_t *com, char *msg, int len)
{
  char msg_id[6], msg_args[COM_MESSAGE_SIZE-4], tmstr[32];
  float lat, lon;
  int gain;
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
	//log_printf("post rtc_set_time\n");
    set_system_time(sec, 0);	// set linux system time here
	//log_printf("post set_system_time\n");

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
  if(strncmp(msg_id, "TFP", 4) ==0){
	com->mode = COM_STAT2;
	}
    
  if (strncmp(msg_id, "NGN", 4) == 0) {
	com->mode = COM_GAIN;
	strcpy(msg_args, &msg[4]);
	sscanf(msg_args, "%d", &gain);
	com->gain = gain;
	
	if(verbose_level){
		log_printf("GAIN: %d", gain);
	}
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

