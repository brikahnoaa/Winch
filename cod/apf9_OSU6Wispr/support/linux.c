#ifndef LINUX_H
#define LINUX_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: linux.c,v 1.2 2003/07/19 22:40:08 swift Exp swift $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: linux.c,v $
 * Revision 1.2  2003/07/19 22:40:08  swift
 * Add definitions of conio.
 *
 * Revision 1.1  2003/07/03 22:45:25  swift
 * Initial revision
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <time.h>
#include <serial.h>

/* function prototypes */
time_t itimer(void);
int kbdflush(void);
int kbdhit(void);
int SetBaud(int);
int Com1Enable(int);
int Com1RtsAssert(void);
int Com1RtsClear(void);
int Com1Cts(void);
int Com1Disable(void);
int ConioEnable(void);
int CtdAssertModePin(void);
int CtdClearModePin(void);
int CtdAssertWakePin(void);
int CtdClearWakePin(void);
int CtdActiveIo(time_t timeout);
int CtdEnableIo(void);
int CtdDisableIo(void);
int CtdInitiateSample(char *buf, int size, time_t timeout);
int CtdAssertTxPin(void);
int CtdClearTxPin(void);
int CtdPSample(char *buf, int size);
int CtdPtSample(char *buf, int size);
int CtdPtsSample(char *buf, int size, time_t timeout);
int Cts(int com);
void ftrace(const char *function_name, const char *msg);
void _fassert(int line, const char *file, const char *expr);
void ModemDisable(void);
void ModemEnable(unsigned int BaudRate);
void Wait(unsigned int millisec);

/* external declarations for serial ports */
extern struct SerialPort ctdio; 
extern unsigned short int CtdPort;

/* external declarations for generic serial ports */
extern const struct SerialPort com[4]; 

extern time_t RefTime;

#endif /* LINUX_H */

#include <sys/types.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <logger.h>
#include <serial.h>
#include <assert.h>

int isatty(int desc);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);

static int cd(int com);
static int fd(int com);
static int FlushIO(int fd);
static int FlushRXBuffer(int fd);
static int FlushTXBuffer(int fd);
static int rs232_open(int com);
static int rts(unsigned char com, unsigned char state);
static int cts(unsigned char com);
static int dtr(unsigned char com, unsigned char state);
static int dsr(unsigned char com);

static int getb(unsigned char *byte,int com);
static int iflush(int com);
static int obytes(int com);
static int oflush(int com);
static int ioflush(int com);
static int putb(unsigned char byte,int com);

static int cd1(void) {return cd(1);}
static int getb1(unsigned char *byte) {return getb(byte,1);}
static int iflush1(void) {return iflush(1);}
static int putb1(unsigned char byte) {return putb(byte,1);}
static int oflush1(void) {return oflush(1);}
static int ioflush1(void) {return ioflush(1);}
static int obytes1(void) {return obytes(1);}
static int rts1(int state) {return rts(1,state);}
static int cts1(void) {return cts(1);}
static int dtr1(int state) {return dtr(1,state);}
static int dsr1(void) {return dsr(1);}

static int cd2(void) {return cd(2);}
static int getb2(unsigned char *byte) {return getb(byte,2);}
static int iflush2(void) {return iflush(2);}
static int putb2(unsigned char byte) {return putb(byte,2);}
static int oflush2(void) {return oflush(2);}
static int ioflush2(void) {return ioflush(2);}
static int obytes2(void) {return obytes(2);}
static int rts2(int state) {return rts(2,state);}
static int cts2(void) {return cts(2);}
static int dtr2(int state) {return dtr(2,state);}
static int dsr2(void) {return dsr(2);}

static int cd3(void) {return cd(3);}
static int getb3(unsigned char *byte) {return getb(byte,3);}
static int iflush3(void) {return iflush(3);}
static int putb3(unsigned char byte) {return putb(byte,3);}
static int oflush3(void) {return oflush(3);}
static int ioflush3(void) {return ioflush(3);}
static int obytes3(void) {return obytes(3);}
static int rts3(int state) {return rts(3,state);}
static int cts3(void) {return cts(3);}
static int dtr3(int state) {return dtr(3,state);}
static int dsr3(void) {return dsr(3);}

static int cd4(void) {return cd(4);}
static int getb4(unsigned char *byte) {return getb(byte,4);}
static int iflush4(void) {return iflush(4);}
static int putb4(unsigned char byte) {return putb(byte,4);}
static int oflush4(void) {return oflush(4);}
static int ioflush4(void) {return ioflush(4);}
static int obytes4(void) {return obytes(4);}
static int rts4(int state) {return rts(4,state);}
static int cts4(void) {return cts(4);}
static int dtr4(int state) {return dtr(4,state);}
static int dsr4(void) {return dsr(4);}

/* assign the serial ports */
const struct SerialPort com[4]={
   {getb1,putb1,iflush1,ioflush1,oflush1,obytes1,cd1,rts1,cts1,dtr1,dsr1},
   {getb2,putb2,iflush2,ioflush2,oflush2,obytes2,cd2,rts2,cts2,dtr2,dsr2},
   {getb3,putb3,iflush3,ioflush3,oflush3,obytes3,cd3,rts3,cts3,dtr3,dsr3},
   {getb4,putb4,iflush4,ioflush4,oflush4,obytes4,cd4,rts4,cts4,dtr4,dsr4},
};

const struct SerialPort modem={getb1,putb1,iflush1,ioflush1,oflush1,obytes1,cd1};

const struct SerialPort CtdIo={getb1,putb1,iflush1,ioflush1,oflush1,obytes1,cd1};

unsigned short int CtdPort=1;
struct SerialPort ctdio={getb1,putb1,iflush1,ioflush1,oflush1,obytes1,cd1};
 

time_t RefTime=0L;
time_t itimer(void) {return (time_t)((RefTime)?difftime(time(NULL),RefTime):-1);}
int Com1RtsAssert(void) {return com[0].rts(1);}
int Com1RtsClear(void)  {return com[0].rts(0);}
int Com1Cts(void)       {return com[0].cts();}

int SetBaud(int baud) {return 1;}
int ConioEnable(void) {return 1;}
int CtdActiveIo(time_t timeout) {timeout=0; return 0;}
int CtdEnableIo(void) {return 1;}
int CtdDisableIo(void) {return 1;}
int CtdAssertTxPin(void)  {Wait(100); return 1;}
int CtdClearTxPin(void)  {tcsendbreak(fd(CtdPort),1000); return 1;}
void ModemDisable(void) {return;}
void ModemEnable(unsigned int BaudRate) {return;}
void Wait(unsigned int millisec) {usleep(millisec*1000);}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
static int rts(unsigned char com, unsigned char state)
{
   int status=1;
   const int iopin=TIOCM_RTS;

   if (fd(com)<0)
   {
      LogEntry("rts()","Unable to connect to Com%d.\n",com);
      status=0;
   }

   else switch (state)
   {
      case 0: 
      {
         if (ioctl(fd(com),TIOCMBIC,&iopin))
         {
            LogEntry("rts()","Attempt to clear RTS on Com%d failed.\n",com);
            status=0;
         }
         break;
      }

      default:
      {
         if (ioctl(fd(com),TIOCMBIS,&iopin))
         {
            LogEntry("rts()","Attempt to assert DTR on Com%d failed.\n",com);
            status=0;
         }
      }
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
static int cts(unsigned char com)
{
   int status=-1;

   if (fd(com)<0)
   {
      LogEntry("cts()","Unable to connect to Com%d.\n",com);
   }
   else if (ioctl(fd(com),TIOCMGET,&status))
   {
      LogEntry("cts()","Unable to determine state of CTS pin for Com%d.\n",com);
      status=0;
   }
   else {status=(status&TIOCM_CTS) ? 1 : 0;}
   
   return status;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
static int dtr(unsigned char com, unsigned char state)
{
   int status=1;
   const int iopin=TIOCM_DTR;

   if (fd(com)<0)
   {
      LogEntry("dtr()","Unable to connect to Com%d.\n",com);
      status=0;
   }

   else switch (state)
   {
      case 0: 
      {
         if (ioctl(fd(com),TIOCMBIC,&iopin))
         {
            LogEntry("rts()","Attempt to clear DTR on Com%d failed.\n",com);
            status=0;
         }
         break;
      }

      default:
      {
         if (ioctl(fd(com),TIOCMBIS,&iopin))
         {
            LogEntry("rts()","Attempt to assert DTR on Com%d failed.\n",com);
            status=0;
         }
      }
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
static int dsr(unsigned char com)
{
   int status=-1;

   if (fd(com)<0)
   {
      LogEntry("dsr()","Unable to connect to Com%d.\n",com);
   }
   else if (ioctl(fd(com),TIOCMGET,&status))
   {
      LogEntry("dsr()","Unable to determine state of DSR pin for Com%d.\n",com);
      status=0;
   }
   else {status=(status&TIOCM_DSR) ? 1 : 0;}
   
   return status;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
int Com1Disable(void) 
{
   int status=1;
   const int dtr=TIOCM_DTR, rts=TIOCM_RTS;
   
   if (ioctl(fd(1),TIOCMBIC,&dtr))
   {
      LogEntry("Com1Disable()","Attempt to clear the DTR signal failed.\n");
      status=0;
   }
   
   else if (ioctl(fd(1),TIOCMBIC,&rts))
   {
      LogEntry("Com1Disable()","Attempt to clear the RTS signal failed.\n");
      status=0;
   }

   return status;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
int Com1Enable(int speed)
{
   int status=1;
   const int dtr=TIOCM_DTR, rts=TIOCM_RTS;
   
   if (ioctl(fd(1),TIOCMBIC,&rts))
   {
      LogEntry("Com1Enable()","Attempt to clear the RTS signal failed.\n");
      status=0;
   }
   
   else if (ioctl(fd(1),TIOCMBIS,&dtr))
   {
      LogEntry("Com1Enable()","Attempt to assert the DTR signal failed.\n");
      status=0;
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to initiate a P-only sample using hardware controls           */
/*------------------------------------------------------------------------*/
int CtdPSample(char *buf, int size)
{
   /* initialize the return value */
   int status=-1;
      
   /* initialize the communications timeout period */
   const time_t timeout=3;

   /* assert that buffer-flushing services exist on ctdio */
   assert(ctdio.ioflush());

   /* Tx pin needs to be low to get a p-only sample */
   CtdClearModePin(); Wait(10);
      
   /* assert the wake pin to initiate a wake-up cycle */   
   CtdAssertWakePin(); Wait(50); CtdClearWakePin();

   /* pause and flush Rx buffer */
   Wait(50); ctdio.ioflush(); CtdClearTxPin();
 
   /* read the string from the serial port */
   status=pgets(&ctdio,buf,size,timeout,"\r\n");
 
   return status;
}

/*------------------------------------------------------------------------*/
/* function to initiate a P-only sample using hardware controls           */
/*------------------------------------------------------------------------*/
int CtdPtSample(char *buf, int size)
{
   /* initialize the return value */
   int status=-1;
      
   /* initialize the communications timeout period */
   const time_t timeout=5;

   /* assert that buffer-flushing services exist on ctdio */
   assert(ctdio.ioflush());

   /* Tx pin needs to be low to get a p-only sample */
   CtdAssertTxPin(); CtdClearModePin(); Wait(10);
      
   /* assert the wake pin to initiate a wake-up cycle */   
   CtdAssertWakePin(); Wait(100); CtdClearWakePin(); 

   /* pause and flush Rx buffer */
   Wait(50); ctdio.ioflush();
   
   /* read the string from the serial port */
   status=pgets(&ctdio,buf,size,timeout,"\r\n");
 
   return status;
}

/*------------------------------------------------------------------------*/
/* function to initiate a full PTS sample using hardware controls         */
/*------------------------------------------------------------------------*/
int CtdPtsSample(char *buf, int size, time_t timeout)
{
   /* initialize the return value */
   int status=-1;

   /* assert that buffer-flushing services exist on ctdio and conio */
   assert(ctdio.ioflush());

   /* set Tx pin low and mode pin high for PTS sample */
   CtdClearTxPin(); CtdAssertModePin(); Wait(10);
   
   /* assert the wake pin to initiate a wake-up cycle */   
   CtdAssertWakePin(); Wait(100); CtdClearWakePin(); 

   /* enable IO on the CTD serial port and flush Rx buffer */
   Wait(50); ctdio.ioflush();
   
   /* read the string from the serial port */
   status=pgets(&ctdio,buf,size,timeout,"\r\n");

   /* clear the mode pin */
   CtdClearModePin();
  
   return status;
}


/*------------------------------------------------------------------------*/
/* function to assert the CTD PTS/FP mode-select pin                      */
/*------------------------------------------------------------------------*/
/**
   This function asserts the CTD PTS/FP mode-select pin.  This function
   returns a positive value which has no meaning at this time.
*/
int CtdAssertModePin(void)
{
   int status=1;
   const int iopin=TIOCM_DTR;
   
   if (ioctl(fd(CtdPort),TIOCMBIC,&iopin))
   {
      LogEntry("CtdAssertModePin()","Attempt to assert the CTD's mode-select pin failed.\n");
      status=0;
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to clear the CTD PTS/FP mode-select pin                      */
/*------------------------------------------------------------------------*/
/**
   This function clears the CTD PTS/FP mode-select pin.  This function
   returns a positive value which has no meaning at this time.
*/
int CtdClearModePin(void)
{
   int status=1;
   const int iopin=TIOCM_DTR;
   
   if (ioctl(fd(CtdPort),TIOCMBIS,&iopin))
   {
      LogEntry("CtdAssertModePin()","Attempt to clear the CTD's mode-select pin failed.\n");
      status=0;
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to assert the CTD PTS/FP mode-select pin                      */
/*------------------------------------------------------------------------*/
/**
   This function asserts the CTD PTS/FP mode-select pin.  This function
   returns a positive value which has no meaning at this time.
*/
int CtdAssertWakePin(void)
{
   int status=1;
   const int iopin=TIOCM_RTS;
   
   if (ioctl(fd(CtdPort),TIOCMBIC,&iopin))
   {
      LogEntry("CtdAssertWakePin()","Attempt to assert the CTD's power-control pin failed.\n");
      status=0;
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to clear the CTD PTS/FP mode-select pin                      */
/*------------------------------------------------------------------------*/
/**
   This function clears the CTD PTS/FP mode-select pin.  This function
   returns a positive value which has no meaning at this time.
*/
int CtdClearWakePin(void)
{
   int status=1;
   const int iopin=TIOCM_RTS;
   
   if (ioctl(fd(CtdPort),TIOCMBIS,&iopin))
   {
      LogEntry("CtdAssertWakePin()","Attempt to clear the CTD's power-control pin failed.\n");
      status=0;
   }

   return status;
}

/*------------------------------------------------------------------------*/
/* function to initiate a PTS or P-only sample using hardware controls    */
/*------------------------------------------------------------------------*/
/**
   This function initiates a sample by the CTD.  The mode-select line is
   used to determine whether a full PTS sample is taken or else a P-only
   sample.  

      \begin{verbatim}
      output:
         This function returns a positive value on success and zero on
         failure.  A negative value indicates an exceptional error.
      \end{verbatim}
*/
int CtdInitiateSample(char *buf, int size, time_t timeout)
{
   /* initialize the return value */
   int status=-1;

   /* assert that buffer-flushing services exist on ctdio and conio */
   assert(ctdio.ioflush());
   
   /* assert the wake pin to initiate a wake-up cycle */   
   CtdAssertWakePin();

   /* pause for 50 milliseconds */
   usleep(100000);
   
   /* clear the wake pin to initiate a wake-up cycle */   
   CtdClearWakePin();

   /* enable IO on the CTD serial port */
   ctdio.ioflush(); 

   /* read the string from the serial port */
   status=pgets(&ctdio,buf,size,timeout,"\r\n");

   /* sometimes disabling/enabling interrupts generates stray bytes */
   ctdio.ioflush();

   return status;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
static int cd(int com)
{
   int status=-1;

   if (fd(com)<0)
   {
      LogEntry("cd()","Unable to connect to Com%d.\n",com);
   }
   else if (ioctl(fd(com),TIOCMGET,&status))
   {
      LogEntry("cd()","Unable to determine state of CD pin for Com%d.\n",com);
      status=0;
   }
   else {status=(status&TIOCM_CD) ? 1 : 0;}
   
   return status;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
int Cts(int com)
{
   int status=-1;

   if (fd(com)<0)
   {
      LogEntry("Cts()","Unable to connect to Com%d.\n",com);
   }
   else if (ioctl(fd(com),TIOCMGET,&status))
   {
      LogEntry("Cts()","Unable to determine state of CTS pin for Com%d.\n",com);
      status=0;
   }
   else {status=(status&TIOCM_CTS) ? 1 : 0;}
   
   return status;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
static int fd(int com)
{
   static int fd1=-1, fd2=-1, fd3=-1, fd4=-1;
   int *fd=NULL;
   
   switch (com)
   {
      case 1: {fd=&fd1; break;}
      case 2: {fd=&fd2; break;}
      case 3: {fd=&fd3; break;}
      case 4: {fd=&fd4; break;}
      default: {LogEntry("rs232()","Nonexistent port: Com%d\n",com);}
   }

   /* open the com port */
   if ((*fd)<0) *fd=rs232_open(com);

   return *fd;
}

/*------------------------------------------------------------------------*/
/* function to flush the input and output buffers                         */
/*------------------------------------------------------------------------*/
/**
   This function flushes both the input and output queues of the serial port.
*/
static int FlushIO(int fd)
{
   /* send command to flush the receive \& transmit buffers */
   if (tcflush(fd,TCIOFLUSH)<0)
   {
      LogEntry("FlushIO()","%s\n",strerror(errno));
      return 0;
   }

   return 1;
}

/*------------------------------------------------------------------------*/
/* function to flush the input buffer                                     */
/*------------------------------------------------------------------------*/
/**
   This function flushes the input queue of the serial port.
*/
static int FlushRXBuffer(int fd)
{
   /* send command to flush the receive buffer */
   if (tcflush(fd,TCIFLUSH)<0)
   {
      LogEntry("FlushRXBuffer()","%s\n",strerror(errno));
      return 0;
   }

   return 1;
}

/*------------------------------------------------------------------------*/
/* function to flush the output buffer                                    */
/*------------------------------------------------------------------------*/
/**
   This function flushes the output queue of the serial port.
*/
static int FlushTXBuffer(int fd) 
{
   /* send command to flush the transmit buffer */
   if (tcflush(fd,TCOFLUSH)<0)
   {
      LogEntry("FlushTXBuffer()","%s\n",strerror(errno));
      return 0;
   }

   return 1;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
void ftrace(const char *function_name, const char *msg)
{
   printf("Trace %s: %s\n",function_name,msg);
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
void _fassert(int line, const char *file, const char *expr)
{
   printf("Assertion failed: %s line %d: \"%s\"\n\n",file,line,expr);
   abort();
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
int getb(unsigned char *byte,int com)
{
   int status=-1;

   /* validate the function argument */
   if (!byte) {LogEntry("getb()","NULL pointer to function parameter.\n");}
      
   else
   {
      /* initialize the functions return values */
      status=0; *byte=0; 

      /* read a byte from the serial port */
      if (fd(com)>=0) {status=read(fd(com),byte,1);}

      /* warn the user */
      else LogEntry("getb()","Unable to connect to Com%d.\n",com);
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
static int iflush(int com)
{
   int status = -1;
   
   if (fd(com)>=0) {status=FlushRXBuffer(fd(com));}
   else LogEntry("iflush()","Unable to connect to Com%d.\n",com);

   return status;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
static int ioflush(int com)
{
   int status = -1;
   
   if (fd(com)>=0) {status=FlushIO(fd(com));}
   else LogEntry("ioflush()","Unable to connect to Com%d.\n",com);

   return status;
}

/* flush the keyboard buffer */
int kbdflush(void)
{
   int status = (tcflush(fileno(stdin),TCIFLUSH)>=0) ? 1 : 0;

   return status;
}

/*------------------------------------------------------------------------*/
/* function to detect keyboard hits and to return the typed character     */
/*------------------------------------------------------------------------*/
/**
   This function is designed to detect keyboard hits on machines running a
   BSD compatible version of UNIX.  It is non-blocking so that if no key was
   hit then the function returns without waiting for input. If a key was hit
   then this function returns the ascii code of the corresponding character.

   written by Dana Swift
*/
int kbdhit(void)
{
   int kbdhit=0;
   struct termios old,new;
   char byte;
   
   // get the file descriptor for the stdin stream
   int fd = fileno(stdin);

   // make sure that STDIN is attached to the keyboard
   if (!isatty(fd)) LogEntry("kbdhit()","STDIN is not attached to keyboard.\n");
      
   // get the terminal attributes
   if (tcgetattr(fd,&old)==-1) LogEntry("kbdhit()","%s\n",strerror(errno));

   // copy the attributes and convert to raw input mode
   new=old; new.c_lflag &= ~(ICANON|ECHO); new.c_cc[VMIN]=0; new.c_cc[VTIME]=0;

   // set raw input mode
   if (tcsetattr(fd,TCSANOW,&new)==-1) LogEntry("kbdhit()","%s\n",strerror(errno));

   // check for input from the keyboard
   kbdhit = read(fd,&byte,1); 

   // reset the terminal attributes to what they were
   if (tcsetattr(fd,TCSANOW,&old)==-1) LogEntry("kbdhit()","%s\n",strerror(errno));
      
   return (kbdhit) ? byte : 0;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
static int obytes(int com)
{
   int n=-1;

   // use ioctl() to determine the number of bytes waiting in the output queue
   if (fd(com)>=0) {if (ioctl(fd(com),TIOCOUTQ,&n)) n=-1;}
   else LogEntry("obytes()","Unable to connect to Com%d.\n",com);
   
   return n;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
static int oflush(int com)
{
   int status = -1;
   
   if (fd(com)>=0) {status=FlushTXBuffer(fd(com));}
   else LogEntry("oflush()","Unable to connect to Com%d.\n",com);

   return status;
}

/*------------------------------------------------------------------------*/
/* function to write a byte to a serial port                              */
/*------------------------------------------------------------------------*/
static int putb(unsigned char byte,int com)
{
   int status=-1;

   if (fd(com)>=0) {byte&=0xff; status=write(fd(com),&byte,1);}
   else LogEntry("putb()","Unable to connect to Com%d.\n",com);
   
   return status;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
static int rs232_open(int com)
{
   int fd=-1;
   int status=1;
   char *port;
   struct termios ioport;
   int baud;
   
   // select the serial port
   switch (com)
   {
     case  1: {port = "/dev/com1"; baud=B9600; break;}
     case  2: {port = "/dev/com2"; baud=B9600; break;}
     case  3: {port = "/dev/com3"; baud=B9600; break;}
     case  4: {port = "/dev/com4"; baud=B9600; break;}
     default:
     {
        LogEntry("rs232_open()","Unknown port(%d).\n",com);
        status = 0;
     }
   }

   // open a connection to the serial port
   if (status && (fd=open(port,O_RDWR|O_NONBLOCK))<0)
   {
      // print error LogEntry
      LogEntry("rs232_open()","Attempt to open port \"%s\" failed.  %s\n",
          port,strerror(errno));

      status = 0;
   }

   // set exclusive-use mode for the serial port
   else if (ioctl(fd,TIOCEXCL))
   {
      LogEntry("rs232_open()","Unable to open in exclusive-use mode.\n");

      status = 0;
   }

   // set asychronous communications mode for the serial port
   else if (fcntl(fd,F_SETFL,FASYNC)<0)
   {
      LogEntry("rs232_open()","Unable to open serial port in asynchronous mode.\n");

      status = 0;
   }
   else
   {
      // get termio parms
      tcgetattr(fd,&ioport);

      // set the flags that control the serial port behavior
      ioport.c_oflag &= ~( OPOST | ONLCR );
      ioport.c_cflag &= ~( HUPCL | CSTOPB | PARENB | CSIZE | CRTSCTS | CS8 | CS7 );
      ioport.c_lflag &= ~( ICANON | ECHO | ECHONL | ISIG | IEXTEN | FLUSHO );
      ioport.c_lflag |=  ( NOFLSH );
      ioport.c_iflag &= ~( INPCK   | IGNPAR | PARMRK | ISTRIP | IGNBRK | BRKINT |
                           IGNCR   | ICRNL  | INLCR  | IXOFF  | IXON   | IXANY  |
                           IMAXBEL );
      
      switch (com)
      {
         case  1: {ioport.c_cflag |=  ( CLOCAL | CREAD | CS7 | PARENB ); break;}
         case  2: {ioport.c_cflag |=  ( CLOCAL | CREAD | CS8 | CSTOPB ); break;}
         case  3: {ioport.c_cflag |=  ( CLOCAL | CREAD | CS8 ); break;}
         case  4: {ioport.c_cflag |=  ( CLOCAL | CREAD | CS8 ); break;}
      }
   
      // initialize input mode read() transfer parameters
      ioport.c_cc[VMIN]=0; ioport.c_cc[VTIME]=0;

      // set the speed 
      cfsetspeed(&ioport,baud);

      // flush the IO buffers
      if (!FlushIO(fd))
      {
         LogEntry("rs232_open()","Unable to flush io-buffer: %s\n",strerror(errno));

         status = 0;
      }

      // reprogram the serial port
      else if (tcsetattr(fd,TCSANOW,&ioport)<0)
      {
         LogEntry("rs232_open()","tcsetattr() failed: %s\n",strerror(errno));

         status = 0;
      }
   }
   
   return (status<=0) ? -1 : fd; 
}
