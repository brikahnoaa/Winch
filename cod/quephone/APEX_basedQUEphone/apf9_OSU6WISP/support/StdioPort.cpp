#ifndef STDIOPORT_OOP
#define STDIOPORT_OOP

#include <defs.p>
#include <termios.h>
 
extern "C"
{
   #include "serial.h"
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: StdioPort.cpp,v 1.1 2005/08/06 17:21:57 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: StdioPort.cpp,v $
 * Revision 1.1  2005/08/06 17:21:57  swift
 * Support utilities for remote host.
 *
 * Revision 1.1  2002/05/07 00:11:43  swift
 * Initial revision
 *
 *========================================================================*/
/* class to emulate a SerialPort with stdio                               */
/*========================================================================*/
/**
   This class adapts standard unix IO channels for use with functions that
   expect a SerialPort object.  It was written to allow the embeddable
   SwiftWare xmodem Tx and Rx modules to be used to transfer files within a
   unix environment.  See the comment section of serial.c for details of the
   behavior of a SerialPort object.

   written by Dana Swift
*/
class StdioPort
{
   // data members
   private:

      // define object to store initial termios configuration of stdin
      termios itrm;

      // define an object to store the stderr stream pointer
      FILE *stderr;

   public:

      // define a serial port object
      SerialPort port;
      
   // function members
   public:

      /// constructor to set noncanonical nonblocking IO on stdin
      StdioPort(void);

      /// destructor to reset canonical blocking IO on stdin
      ~StdioPort(void);
};

#ifdef STDIOPORT_CPP

#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <regex.h>
#include <libgen.h>
#include <string>
#include <GetOpt.oop>

extern "C"
{
   #include "Rx.h"
   #include "logger.h"
   #include "xmodem.h"
         
   /// function to read a byte from the stdin
   static int getb(unsigned char *byte);

   /// function to flush the input buffer
   static int iflush(void);

   /// function to flush the input and output buffers
   static int ioflush(void);
   
   /// function to return the number of bytes
   static int obytes(void);
      
   /// function to flush the output buffer
   static int oflush(void);
      
   /// function to write a byte from the stdin
   static int putb(unsigned char byte);
}

/*------------------------------------------------------------------------*/
/* constructor to set noncanonical nonblocking IO on stdin                */
/*------------------------------------------------------------------------*/
StdioPort::StdioPort(void)
{
   // assign the pointer to the SerialPort::getb() function
   port.getb = ::getb;

   // assign the pointer to the SerialPort::putb() function
   port.putb = ::putb;

   // assign the pointer to the SerialPort::iflush() function
   port.iflush = ::iflush;

   // assign the pointer to the SerialPort::ioflush() function
   port.ioflush = ::ioflush;

   // assign the pointer to the SerialPort::obytes() function
   port.obytes = ::obytes;

   // assign the pointer to the SerialPort::oflush() function
   port.oflush = ::oflush;

   // assign the pointer to the SerialPort::oflush() function
   port.cd=NULL;

   // store a pointer to the stderr stream
   stderr = ::stderr;

   // point the stderr stream to /dev/null
   open_file("/dev/null","w",::stderr);
   
   // get the terminal attributes for stdin
   if (tcgetattr(fileno(stdin),&itrm)==-1)
   {
      LogEntry("StdioPort::StdioPort()","%s\n",strerror(errno));
      exit(0);
   }

   // copy stdin's attributes and convert to raw input mode
   termios trm=itrm; cfmakeraw(&trm); trm.c_cc[VMIN]=0; trm.c_cc[VTIME]=0;

   // set raw input mode for stdin
   if (tcsetattr(fileno(stdin),TCSANOW,&trm)==-1)
   {
      LogEntry("StdioPort::StdioPort()","%s\n",strerror(errno));
      exit(0);
   }
   
   // flush stdin
   if (tcflush(fileno(stdin),TCIOFLUSH)<0)
   {
      LogEntry("StdioPort::StdioPort()","stdin: %s\n",strerror(errno));
   }
   
   // flush stdout
   else if (tcflush(fileno(stdout),TCIOFLUSH)<0)
   {
      LogEntry("StdioPort::StdioPort()","stdout: %s\n",strerror(errno));
   }
}

/*------------------------------------------------------------------------*/
/* destructor to reset canonical blocking IO on stdin                     */
/*------------------------------------------------------------------------*/
StdioPort::~StdioPort(void)
{
   // reset stdin's terminal attributes to their original state
   if (tcsetattr(fileno(stdin),TCSANOW,&itrm)==-1)
   {
      LogEntry("StdioPort::~StdioPort()","stdin: %s\n",strerror(errno));
      exit(0);
   }

   // restore the pointer to stderr
   ::stderr=stderr;
}

extern "C" {
   
   /*------------------------------------------------------------------------*/
   /* function to read a byte from the stdin                                 */
   /*------------------------------------------------------------------------*/
   static int getb(unsigned char *byte)
   {
      // initialize the function's return value
      int status = -1;

      // validate the function parameter
      if (!byte) LogEntry("StdioPort.cpp:getb()","NULL pointer.\n");

      else
      {
         // read a byte from stdin
         status = read(fileno(stdin),byte,1);

         // check for pathological conditions
         if (status==-1)
         {
            // check to see if no byte was available
            if (errno==EAGAIN) status=0;

            // log the pathological condition
            else {LogEntry("StdioPort.cpp:getb()","%s\n",strerror(errno));}
         }
      }

      return status;
   }

   /*------------------------------------------------------------------------*/
   /* function to flush the input buffer                                     */
   /*------------------------------------------------------------------------*/
   static int iflush(void)
   {
      // flush the input buffer
      int status = tcflush(fileno(stdin),TCIFLUSH);

      // check for pathological conditions
      if (status==-1) {LogEntry("StdioPort.cpp:iflush()","%s\n",strerror(errno));}

      // tcflush() returns zero on success
      else if (!status) status=1;

      return status;
   }
   
   /*------------------------------------------------------------------------*/
   /* function to flush the input buffer                                     */
   /*------------------------------------------------------------------------*/
   static int ioflush(void)
   {
      // flush the input buffer
      int status = (iflush() && oflush()) ? 1 : 0;

      // check for pathological conditions
      if (!status) {LogEntry("StdioPort.cpp:ioflush()","%s\n",strerror(errno));}

      return status;
   }

   /*------------------------------------------------------------------------*/
   /* function to return the number of bytes waiting in the output queue     */
   /*------------------------------------------------------------------------*/
   static int obytes(void)
   {
      int n=-1;

      // use ioctl() to determine the number of bytes waiting in the output queue
      if (ioctl(fileno(stdout),TIOCOUTQ,&n)) n=-1; 

      return n;
   }

   /*------------------------------------------------------------------------*/
   /* function to flush the output buffer                                    */
   /*------------------------------------------------------------------------*/
   static int oflush(void)
   {
      // flush the input buffer
      int status = tcflush(fileno(stdout),TCIOFLUSH);
   
      // check for pathological conditions
      if (status==-1) {LogEntry("StdioPort.cpp:oflush()","%s\n",strerror(errno));}
 
      // tcflush() returns zero on success
      else if (!status) status=1;
   
      return status;
   }
 
   /*------------------------------------------------------------------------*/
   /* function to write a byte to stdout                                     */
   /*------------------------------------------------------------------------*/
   static int putb(unsigned char byte)
   {
      int i,status;
     
      for (status=-1,i=0; i<25 && status<=0; i++)
      {
         // write read a byte from stdin
         status = write(fileno(stdout),&byte,1);
      }

      // make a log entry if more than 1 try was needed
      if (i>1 && debuglevel>=3) LogEntry("StdioPort.cpp:putb()","Write required %d attempts.\n",i);
  
      // check for pathological conditions
      if (status==-1) {LogEntry("StdioPort.cpp:putb()","%s\n",strerror(errno));}

      return status;
   }
}

#endif // STDIOPORT_CPP
#endif // STDIOPORT_OOP
