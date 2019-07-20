#ifndef FIFO_H
#define FIFO_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: fifo.c,v 1.10 2007/04/24 01:43:29 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* Copyright University of Washington.   Written by Dana Swift.
 *
 * This software was developed at the University of Washington using funds
 * generously provided by the US Office of Naval Research, the National
 * Science Foundation, and NOAA.
 *  
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at
 * your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
/** RCS log of revisions to the C source code.
 *
 * \begin{verbatim}
 * $Log: fifo.c,v $
 * Revision 1.10  2007/04/24 01:43:29  swift
 * Added acknowledgement and funding attribution.
 *
 * Revision 1.9  2006/04/21 13:45:40  swift
 * Changed copyright attribute.
 *
 * Revision 1.8  2005/10/25 17:01:48  swift
 * Fixed a bug in push() that used 'int' rather than 'long int'.
 *
 * Revision 1.7  2005/02/22 21:47:43  swift
 * Made some integers volatile.
 *
 * Revision 1.6  2004/12/29 23:06:50  swift
 * Modified LogEntry() to use strings stored in the CODE segment.  This saves
 * lots of space in the DATA segment and significantly speeds code start-up.
 *
 * Revision 1.5  2003/11/25 00:33:29  swift
 * Changed type of 'head' and 'tail' to be long int to avoid integer overflows.
 *
 * Revision 1.4  2003/11/20 18:59:42  swift
 * Added GNU Lesser General Public License to source file.
 *
 * Revision 1.3  2003/11/12 22:23:20  swift
 * Modified Fifo structure to put the buffer in the far ram.
 *
 * Revision 1.2  2003/07/03 22:44:03  swift
 * Major revisions.  This version is not yet stable.
 *
 * Revision 1.1  2003/06/21 23:20:03  swift
 * Initial revision
 * \end{verbatim}
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define fifoChangeLog "$RCSfile: fifo.c,v $  $Revision: 1.10 $ $Date: 2007/04/24 01:43:29 $"

#include <stdlib.h>

/* define a FIFO structure */
struct Fifo
{
      /* define a constant pointer to the fifo buffer */
      unsigned far char* const buf;

      /* object to record the size of the fifo buffer */
      const size_t size;

      /* object to record the number of bytes in the fifo queue */
      volatile size_t length;

      /* object to record the number of bytes lost to buffer overflow */
      volatile size_t BufOverflowCount;
      
      /* define the head and tail of the fifo */
      volatile long int head, tail;
};
 
/* function prototypes */
int del(struct Fifo *fifo);
int flush(struct Fifo *fifo);
int push(struct Fifo *fifo, unsigned char byte);
int pop(struct Fifo *fifo, unsigned char *byte);

#endif /* FIFO_H */

#include <logger.h>

/*------------------------------------------------------------------------*/
/* function to delete the head of the fifo queue                          */
/*------------------------------------------------------------------------*/
/**
   This function deletes the last byte in the FIFO queue - it undoes the
   last push stored in the queue.

      \begin{verbatim}
      input:
         fifo....The Fifo object from which to delete.

      output:
         This function will return a positive value if successful.  A zero
         will be returned if the fifo queue is empty.  If the Fifo object is
         invalid then this function will return a negative value.
      \end{verbatim}
*/
int del(struct Fifo *fifo)
{
   /* define the logging signature */
   static cc FuncName[] = "del()";
   
   int status=-1;

   /* assert that the Fifo object is not null */
   if (!fifo || !fifo->buf)
   {
      /* create the message */
      static cc msg[]="NULL pointer to fifo.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }
   
   /* validate the Fifo's head */
   else if (fifo->head<0 || fifo->head>=fifo->size)
   {
      /* create the message */
      static cc msg[]="Corrupt fifo: invalid head.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }
   
   /* validate the Fifo's tail */
   else if (fifo->tail<0 || fifo->tail>=fifo->size)
   {
      /* create the message */
      static cc msg[]="Corrupt fifo: invalid tail.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   else
   {
      /* reinitialize the return value */
      status=0;
      
      /* ignore the delete request if the fifo is empty */
      if (fifo->head != fifo->tail) 
      {
         /* compute the new position of the fifo's head-pointer */
         fifo->head = (fifo->head>0) ? fifo->head-1 : fifo->size-1;

         /* terminate the buffer with a NULL */
         fifo->buf[fifo->head]=0; --fifo->length;

         /* indicate success */
         status=1; 
      }
   
      /* reset the overflow counter - always more room after a delete request */
      fifo->BufOverflowCount=0;
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to flush the Fifo queue                                       */
/*------------------------------------------------------------------------*/
/**
   This function flushes the FIFO queue.

      \begin{verbatim}
      input:
         fifo...The Fifo object to be flushed.

      output:
         This function returns a positive number on success and zero on
         failure. If the Fifo object is invalid then this function will
         return a negative value.
      \end{verbatim}
*/
int flush(struct Fifo *fifo)
{
   /* define the logging signature */
   static cc FuncName[] = "flush()";
   
   int status=-1;
  
   /* validate the Fifo object */
   if (!fifo)
   {
      /* create the message */
      static cc msg[]="NULL pointer to fifo.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   else
   {
      /* reinitialize return value */
      status=1;
      
      /* reset the fifo pointers and overflow counter */
      fifo->head=0; fifo->tail=0; fifo->length=0; fifo->BufOverflowCount=0;
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to push a byte into the FIFO queue                            */
/*------------------------------------------------------------------------*/
/**
   This function pushes one byte into a Fifo queue.  

      \begin{verbatim}
      input:
         fifo....The FIFO into which 'byte' will be pushed.
         byte....This is the byte to be pushed into the fifo.

      output:
         This function returns a positive value on success and zero on
         failure.  If the Fifo object is invalid then this function will
         return a negative value.
      \end{verbatim}
*/
int push(struct Fifo *fifo, unsigned char byte)
{
   /* define the logging signature */
   static cc FuncName[] = "push()";
   
   int status=-1;

   /* assert that the Fifo object is not null */
   if (!fifo || !fifo->buf)
   {
      /* create the message */
      static cc msg[]="NULL pointer to fifo.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }
   
   /* validate the Fifo's head */
   else if (fifo->head<0 || fifo->head>=fifo->size)
   {
      /* create the message */
      static cc msg[]="Corrupt fifo: invalid head.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }
   
   /* validate the Fifo's tail */
   else if (fifo->tail<0 || fifo->tail>=fifo->size)
   {
      /* create the message */
      static cc msg[]="Corrupt fifo: invalid tail.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   else
   {
      /* compute where the head should look next */
      long int next = ((fifo->head+1)>=fifo->size) ? 0 : (fifo->head+1);

      /* check if the fifo buffer is full */
      if (next != fifo->tail)
      {
         /* insert the byte in the fifo buffer and increment the head pointer */
         fifo->buf[fifo->head]=byte; fifo->head=next; ++fifo->length;

         /* indicate success */
         status=1;
      }
      else
      {
         /* terminate the buffer and increment the fifo overflow counter */
         fifo->BufOverflowCount++; fifo->buf[fifo->head]=0;

         /* indicate failure */
         status=0;
      }
   }
   
   return status;
}

/*------------------------------------------------------------------------*/
/* function to pop a byte from the FIFO queue                             */
/*------------------------------------------------------------------------*/
/**
   This function pops one byte from a Fifo queue.  

      \begin{verbatim}
      input:
         fifo....The FIFO from which 'byte' will be popped.

      output:
          byte....This will contain the popped byte.
          
         This function returns a positive value on success and zero on
         failure.  If the Fifo object is invalid then this function will
         return a negative value.
      \end{verbatim}
*/
int pop(struct Fifo *fifo, unsigned char *byte)
{
   /* define the logging signature */
   static cc FuncName[] = "pop()";
   
   int status=-1;

   /* initialize the return value */
   if (byte) *byte=0;
   
   /* make sure that the pointer is valid */
   if (!byte)
   {
      /* create the message */
      static cc msg[]="NULL pointer to byte.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   /* assert that the Fifo object is not null */
   else if (!fifo || !fifo->buf)
   {
      /* create the message */
      static cc msg[]="NULL pointer to fifo.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }
   
   /* validate the Fifo's head */
   else if (fifo->head<0 || fifo->head>=fifo->size)
   {
      /* create the message */
      static cc msg[]="Corrupt fifo: invalid head.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }
   
   /* validate the Fifo's tail */
   else if (fifo->tail<0 || fifo->tail>=fifo->size)
   {
      /* create the message */
      static cc msg[]="Corrupt fifo: invalid tail.\n";

      /* log the message */
      LogEntry(FuncName,msg);
   }

   else
   {
      /* reinitialize the function's return value */
      status=1;
            
      /* check if the fifo buffer is empty */
      if (fifo->tail != fifo->head)
      {
         /* read the next byte from the fifo buffer */
         *byte = fifo->buf[fifo->tail]; --fifo->length;

         /* increment the tail - wrap around if necessary */
         if ((++fifo->tail)>=fifo->size) fifo->tail=0;
      }

      /* fifo buffer is empty - reinitialize both pointers */
      else {fifo->head=0; fifo->tail=0; fifo->length=0; status=0;}

      /* reset the buffer overflow counter */
      fifo->BufOverflowCount=0;
   }
   
   return status;
}
