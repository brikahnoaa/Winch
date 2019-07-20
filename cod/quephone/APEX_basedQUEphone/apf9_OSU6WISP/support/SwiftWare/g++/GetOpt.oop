/* Getopt for GNU. 
   Copyright (C) 1987, 1989, 1992 Free Software Foundation, Inc.
   (Modified by Douglas C. Schmidt for use with GNU G++.)

This file is part of the GNU C++ Library.  This library is free
software; you can redistribute it and/or modify it under the terms of
the GNU Library General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.  This library is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/* This version of `getopt' appears to the caller like standard Unix `getopt'
   but it behaves differently for the user, since it allows the user
   to intersperse the options with the other arguments.

   As `getopt' works, it permutes the elements of `argv' so that,
   when it is done, all the options precede everything else.  Thus
   all application programs are extended to handle flexible argument order.

   Setting the environment variable _POSIX_OPTION_ORDER disables permutation.
   Then the behavior is completely standard.

   GNU application programs can use a third alternative mode in which
   they can distinguish the relative order of options and other arguments.
*/

#ifndef GETOPT_OOP
#define GETOPT_OOP

#include <std.h>
#include <stdio.h>

class GetOpt
{
private:
  /* The next char to be scanned in the option-element
     in which the last option character we returned was found.
     This allows us to pick up the scan where we left off.
        
     If this is zero, or a null string, it means resume the scan
     by advancing to the next ARGV-element.  */
  
  static char *nextchar;
  
  
  /* Describe how to deal with options that follow non-option ARGV-elements.
    
    UNSPECIFIED means the caller did not specify anything;
    the default is then REQUIRE_ORDER if the environment variable
    _OPTIONS_FIRST is defined, PERMUTE otherwise.
      
    REQUIRE_ORDER means don't recognize them as options.
    Stop option processing when the first non-option is seen.
    This is what Unix does.
            
    PERMUTE is the default.  We permute the contents of `argv' as we scan,
    so that eventually all the options are at the end.  This allows options
    to be given in any order, even with programs that were not written to
    expect this.
        
    RETURN_IN_ORDER is an option available to programs that were written
    to expect options and other ARGV-elements in any order and that care about
    the ordering of the two.  We describe each non-option ARGV-element
    as if it were the argument of an option with character code zero.
    Using `-' as the first character of the list of option characters
    requests this mode of operation.
                    
    The special argument `--' forces an end of option-scanning regardless
    of the value of `ordering'.  In the case of RETURN_IN_ORDER, only
    `--' can cause `getopt' to return EOF with `optind' != ARGC.  */
  
   enum OrderingEnum { REQUIRE_ORDER, PERMUTE, RETURN_IN_ORDER };
   OrderingEnum ordering;

  /* Handle permutation of arguments.  */
  
  /* Describe the part of ARGV that contains non-options that have
     been skipped.  `first_nonopt' is the index in ARGV of the first of them;
     `last_nonopt' is the index after the last of them.  */
  
  static int first_nonopt;
  static int last_nonopt;
  
  void exchange (char **argv);
public:
  /* For communication from `getopt' to the caller.
     When `getopt' finds an option that takes an argument,
     the argument value is returned here.
     Also, when `ordering' is RETURN_IN_ORDER,
     each non-option ARGV-element is returned here.  */
  
  char *optarg;
  
  /* Index in ARGV of the next element to be scanned.
     This is used for communication to and from the caller
     and for communication between successive calls to `getopt'.
     On entry to `getopt', zero means this is the first call; initialize.
          
     When `getopt' returns EOF, this is the index of the first of the
     non-option elements that the caller should itself scan.
              
     Otherwise, `optind' communicates from one call to the next
     how much of ARGV has been scanned so far.  */
  
  int optind;

  /* Callers store zero here to inhibit the error message
     for unrecognized options.  */
  
  int opterr;
  
  int    nargc;
  char **nargv;
  const char  *noptstring;
  
  GetOpt (int argc, char **argv, const char *optstring);
  int operator () (void);

  void reset(void) {first_nonopt = last_nonopt = optind = 1; optarg = nextchar = 0;}
};

#ifdef GETOPT_CPP

char* GetOpt::nextchar = 0;
int GetOpt::first_nonopt = 0;
int GetOpt::last_nonopt = 0;

GetOpt::GetOpt (int argc, char **argv, const char *optstring)
 :opterr (1), nargc (argc), nargv (argv), noptstring (optstring)
{
  /* Initialize the internal data when the first call is made.
     Start processing options with ARGV-element 1 (since ARGV-element 0
     is the program name); the sequence of previously skipped
     non-option ARGV-elements is empty.  */

  first_nonopt = last_nonopt = optind = 1;
  optarg = nextchar = 0;

  /* Determine how to handle the ordering of options and nonoptions.  */

  if (optstring[0] == '-')
    ordering = RETURN_IN_ORDER;
  else if (getenv ("_POSIX_OPTION_ORDER") != 0)
    ordering = REQUIRE_ORDER;
  else
    ordering = PERMUTE;
}

void
GetOpt::exchange (char **argv)
{
  int nonopts_size
    = (last_nonopt - first_nonopt) * sizeof (char *);
  char **temp = (char **) alloca (nonopts_size);

  /* Interchange the two blocks of data in argv.  */

  memcpy (temp, &argv[first_nonopt], nonopts_size);
  memcpy (&argv[first_nonopt], &argv[last_nonopt],
         (optind - last_nonopt) * sizeof (char *));
  memcpy (&argv[first_nonopt + optind - last_nonopt], temp,
         nonopts_size);

  /* Update records for the slots the non-options now occupy.  */

  first_nonopt += (optind - last_nonopt);
  last_nonopt = optind;
}

/* Scan elements of ARGV (whose length is ARGC) for option characters
   given in OPTSTRING.

   If an element of ARGV starts with '-', and is not exactly "-" or "--",
   then it is an option element.  The characters of this element
   (aside from the initial '-') are option characters.  If `getopt'
   is called repeatedly, it returns successively each of theoption characters
   from each of the option elements.

   If `getopt' finds another option character, it returns that character,
   updating `optind' and `nextchar' so that the next call to `getopt' can
   resume the scan with the following option character or ARGV-element.

   If there are no more option characters, `getopt' returns `EOF'.
   Then `optind' is the index in ARGV of the first ARGV-element
   that is not an option.  (The ARGV-elements have been permuted
   so that those that are not options now come last.)

   OPTSTRING is a string containing the legitimate option characters.
   A colon in OPTSTRING means that the previous character is an option
   that wants an argument.  The argument is taken from the rest of the
   current ARGV-element, or from the following ARGV-element,
   and returned in `optarg'.

   If an option character is seen that is not listed in OPTSTRING,
   return '?' after printing an error message.  If you set `opterr' to
   zero, the error message is suppressed but we still return '?'.

   If a char in OPTSTRING is followed by a colon, that means it wants an arg,
   so the following text in the same ARGV-element, or the text of the following
   ARGV-element, is returned in `optarg.  Two colons mean an option that
   wants an optional arg; if there is text in the current ARGV-element,
   it is returned in `optarg'.

   If OPTSTRING starts with `-', it requests a different method of handling the
   non-option ARGV-elements.  See the comments about RETURN_IN_ORDER, above.  */

int
GetOpt::operator () (void)
{
  if (nextchar == 0 || *nextchar == 0)
    {
      if (ordering == PERMUTE)
        {
          /* If we have just processed some options following some non-options,
             exchange them so that the options come first.  */

          if (first_nonopt != last_nonopt && last_nonopt != optind)
            exchange (nargv);
          else if (last_nonopt != optind)
            first_nonopt = optind;

          /* Now skip any additional non-options
             and extend the range of non-options previously skipped.  */

          while (optind < nargc
                 && (nargv[optind][0] != '-'
                     || nargv[optind][1] == 0))
            optind++;
          last_nonopt = optind;
        }

      /* Special ARGV-element `--' means premature end of options.
         Skip it like a null option,
         then exchange with previous non-options as if it were an option,
         then skip everything else like a non-option.  */

      if (optind != nargc && !strcmp (nargv[optind], "--"))
        {
          optind++;

          if (first_nonopt != last_nonopt && last_nonopt != optind)
            exchange (nargv);
          else if (first_nonopt == last_nonopt)
            first_nonopt = optind;
          last_nonopt = nargc;

          optind = nargc;
        }

      /* If we have done all the ARGV-elements, stop the scan
         and back over any non-options that we skipped and permuted.  */

      if (optind == nargc)
        {
          /* Set the next-arg-index to point at the non-options
             that we previously skipped, so the caller will digest them.  */
          if (first_nonopt != last_nonopt)
            optind = first_nonopt;
          return EOF;
        }
	 
      /* If we have come to a non-option and did not permute it,
         either stop the scan or describe it to the caller and pass it by.  */

      if (nargv[optind][0] != '-' || nargv[optind][1] == 0)
        {
          if (ordering == REQUIRE_ORDER)
            return EOF;
          optarg = nargv[optind++];
          return 0;
        }

      /* We have found another option-ARGV-element.
         Start decoding its characters.  */

      nextchar = nargv[optind] + 1;
    }

  /* Look at and handle the next option-character.  */

  {
    char c = *nextchar++;
    char *temp = (char *) strchr (noptstring, c);

    /* Increment `optind' when we start to process its last character.  */
    if (*nextchar == 0)
      optind++;

    if (temp == 0 || c == ':')
      {
        if (opterr != 0)
          {
            if (c < 040 || c >= 0177)
              fprintf (stderr, "%s: unrecognized option, character code 0%o\n",
                       nargv[0], c);
            else
              fprintf (stderr, "%s: unrecognized option `-%c'\n",
                       nargv[0], c);
          }
        return '?';
      }
    if (temp[1] == ':')
      {
        if (temp[2] == ':')
          {
            /* This is an option that accepts an argument optionally.  */
            if (*nextchar != 0)
              {
                optarg = nextchar;
                optind++;
              }
            else
              optarg = 0;
            nextchar = 0;
          }
        else
          {
            /* This is an option that requires an argument.  */
            if (*nextchar != 0)
              {
                optarg = nextchar;
                /* If we end this ARGV-element by taking the rest as an arg,
                   we must advance to the next element now.  */
                optind++;
              }
            else if (optind == nargc)
              {
                if (opterr != 0)
                  fprintf (stderr, "%s: no argument for `-%c' option\n",
                           nargv[0], c);
                c = '?';
              }
            else
              /* We already incremented `optind' once;
                 increment it again when taking next ARGV-elt as argument.  */
              optarg = nargv[optind++];
            nextchar = 0;
          }
      }
    return c;
  }
}

#endif // GETOPT_CPP
#endif // GETOPT_OOP
