/****************************************/
/*     member of utils library          */
/****************************************/
#include <defs.p>

/* declare a pointer for logging memory requests */
static FILE *MemReqLogFile=NULL;

/* functions to get & set file pointer to log file for memory requests */
void set_MemReqLog(FILE *ptr) {MemReqLogFile=ptr;}
FILE *get_ReqLogFile(void) {return MemReqLogFile;}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: memreq.c,v 1.1.1.1 2003/11/12 23:22:58 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: memreq.c,v $
 * Revision 1.1.1.1  2003/11/12 23:22:58  swift
 * Apf9 firmware for a 260ml Apex with SBE41 CTD.
 *
 * Revision 1.2  1997/03/01 17:01:27  swift
 * Add debugging code & logging facility.
 *
 * Revision 1.1  1996/06/15 15:50:53  swift
 * Initial revision
 *
 *------------------------------------------------------------------------*/
/* function to allocate or reallocate memory from the heap                */
/*------------------------------------------------------------------------*/
/*
   This function fills requests for heap-space allocations.  It has 3 modes,
   an allocation mode, a re-allocation mode, and a mode to return previously
   allocated space back to the heap.  No garbage collection is attempted.  

      Input:
         ptr.......A pointer that determines whether to allocate or
                   reallocate space from the heap.  If (ptr) is NULL then
                   fresh space is allocated using the C standard library's
                   calloc() function.  If (ptr) is non-NULL then space is
                   re-allocated from the heap using the C standard library's
                   realloc() function.
         n.........The number of objects to allocate.  If (n) is zero and
                   (ptr) is non-NULL then the C standard library function
                   free() is called to return the previously allocated space
                   back to the heap.  If (ptr) is NULL then the free()
                   function is not called.  
         size......The size of each object.
         funcname..The name of the calling function.  This is used to
                   provide information in the event that the heap request fails.

      Output:
         This function returns a pointer to the allocated heap space.  If
         (n) is zero then this function returns NULL.  If the allocation
         request fails then the SwiftWare library function swifterr() is
         called which prints a diagnostic message is printed and exits the
         program. 

   written by Dana Swift, November 1995
*/
void *MemReq(void *ptr,int n,int size,char *funcname)
{
   void *tmp;

   /* manage requests for returning memory to heap */
   if (!n) 
   {
      /* check for log the request */
      if (MemReqLogFile) fprintf(MemReqLogFile,"MemReq() release request for (%p) by %s.%s\n",
                                 ptr,funcname, (ptr)?"":" (ignored.)");
      
      /* release the memory if (ptr) is valid */
      if (ptr) free(ptr); return(NULL);
   }

   /* allocate or reallocate space from the heap */ 
   tmp = (!ptr) ? calloc(n,size) : realloc (ptr,n*size);

   /* check for log request */
   if (MemReqLogFile)
   {
      /* log the request statistics */
      fprintf(MemReqLogFile,"MemReq() %sallocate request: %32s %5d %4d %6d 0x%-8x 0x%-8x\n",
              (ptr)?"re":"  ",funcname,n,size,n*size,(unsigned)tmp,(unsigned)ptr);

      /* make sure the allocation was successful */
      if (!tmp) fprintf(MemReqLogFile,"error in MemReq()...Memory %sallocation request from "
                        "%s failed.\n\tRequest was for %d objects each of size %d bytes "
                        "(%d bytes).\n",((ptr)?"re-":""),funcname,n,size,n*size);

      /* flush the file buffer */
      fflush(MemReqLogFile);
   }
   
   /* make sure the allocation was successful */
   if (!tmp) swifterr("error in MemReq()...Memory %sallocation request from %s failed.\n"
                      "\tRequest was for %d objects each of size %d bytes (%d bytes).\n",
                      ((ptr)?"re-":""),funcname,n,size,n*size); 
 
   return (tmp);
}
