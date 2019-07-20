/****************************************/
/*     member of utils library          */
/****************************************/
#ifndef SWIFTWARE_OOP
#define SWIFTWARE_OOP

using namespace std;

#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <defs.p>

extern const _Ios_Fmtflags IOSFLAGS;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: swiftware.cpp,v 1.2 2008/07/14 16:59:25 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: swiftware.cpp,v $
 * Revision 1.2  2008/07/14 16:59:25  swift
 * Modifications for use with Fedora.
 *
 * Revision 1.1  2006/11/18 17:22:31  swift
 * Added new functionality to library.
 *
 * Revision 1.5  2006/11/11 18:34:34  swift
 * Modifications to conform to current C++ standards.
 *
 * Revision 1.4  2001/06/02 07:34:47  swift
 * Fixes required by RedHat 7.1 - mostly minor fix-ups to prevent compiler warnings.
 *
 * Revision 1.3  1999/06/07 17:36:37  swift
 * Removed obsolete pragma directive.
 *
 * Revision 1.2  1999/01/16 06:12:08  swift
 * Misc modifications.
 *
 * Revision 1.3  1996/06/14 15:25:34  swift
 * Modifications to eliminate compiler warnings with gcc -Wall ...
 *
 * Revision 1.2  1996/01/01 22:43:00  swift
 * Merge header & source files.
 *
 * Revision 1.1  1995/06/30 18:01:11  swift
 * Initial revision
 *
 *========================================================================*/
/* Abstract base class for SwiftWare C++ libraries                        */
/*========================================================================*/
class SwiftWare
{
   // declare a structure nested within SwiftWare
   struct State
   {
      const char   *prefix;
      char          indent[100]; 
      _Ios_Fmtflags iosformat;
      ostream      *dest;   
      State        *state;
      
      // constructor
      State(State *previous) {state=previous;}
   };

   // member data
   private:

      // define class fingerprint
      static char const *fingerprint;

   protected:
   
      // declare pointer to current State structure
      State *state;

      // declare pointer to indentation string
      const char *indent;

   // member functions
   protected:

      // function to write class header to core file
      const char *HeadCore
      (
         ostream    &dest,
         const char *id,
         const char *prefix,
         const char *fingerprint,
         _Ios_Fmtflags iosflags = IOSFLAGS
      );

      // function to write class trailer to core file
      void TailCore(void);

   public:

      virtual ~SwiftWare() {}
      
      // function to dump core to ostream
      virtual int DumpCore
      (
         ostream    &dest   = cout,  // output stream for core dump
         const char *id     = "",    // class object name
         const char *prefix = ""     // prefix string for beginning each line
      ) = 0;

      // function to return class fingerprint
      virtual const char *get_fingerprint(void) {return fingerprint;}

      // function to read core from istream
      virtual int ReadCore(istream &source);
};

#endif // SWIFTWARE_OOP
#ifdef SWIFTWARE_CPP

// initialize fingerprint for the SwiftWare class
const char *SwiftWare::fingerprint="SwiftWare";

/*------------------------------------------------------------------------*/
/* function to read core from istream                                     */
/*------------------------------------------------------------------------*/
int SwiftWare::ReadCore(istream &/*source*/)
{
   message("warning in SwiftWare::ReadCore()...Core file not read.\n"
           "\t(overload virtual SwiftWare::ReadCore() in a derived class.)\n");

   return 0;
}

/*------------------------------------------------------------------------*/
/* function to write class header to CoreDump file                        */
/*------------------------------------------------------------------------*/
const char *SwiftWare::HeadCore
(
   ostream    &dest,    
   const char *id,      
   const char *prefix,  
   const char *fingerprint,
   _Ios_Fmtflags iosflags
)
{
   // allocate a new State structure
   state = new State(state);
   
   // make sure allocation was successfull
   validate(state,"state","SwiftWare::HeadCore()");

   // save dest ostream
   state->dest = &dest;

   // save prefix
   state->prefix = prefix;

   // make indent string
   strcpy(state->indent,prefix); strcat(state->indent,"   "); indent=state->indent;

   // save and set ios format flags
   state->iosformat = dest.flags(iosflags);

   // print class identifier
   dest << prefix << fingerprint << "::" << id << endl << prefix << '{' << endl;

   // print class fingerprint
   dest << indent << "const static char *fingerprint = \"" << fingerprint << "\"" << endl;

   return indent;
}

/*------------------------------------------------------------------------*/
/* function to write class trailer to CoreDump file                       */
/*------------------------------------------------------------------------*/
void SwiftWare::TailCore()
{
   // close class report with a brace
   (*state->dest) << state->prefix << "}\n";

   // reset iosformat to original value
   state->dest->flags(state->iosformat);

   // save pointer to current state and reset to previous state
   State *tmp = state; state = tmp->state;

   // return state space back to heap
   delete tmp;

   // reset indent pointer
   indent = state->indent;
}

#endif // SWIFTWARE_CPP
