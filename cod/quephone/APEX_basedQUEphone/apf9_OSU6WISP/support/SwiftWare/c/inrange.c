/****************************************/
/*       member of utils library        */
/****************************************/
#include <defs.p>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: inrange.c,v 1.1 2006/11/18 17:20:24 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: inrange.c,v $
 * Revision 1.1  2006/11/18 17:20:24  swift
 * Added new functionality to library.
 *
 * Revision 1.1  1996/06/15 15:49:59  swift
 * Initial revision
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*========================================================================*/
/* function to determine if a number is within a semiclosed range         */
/*========================================================================*/
/*
  This function determines whether a number (x) is in the semi-closed range
  of [a,b).  The function returns true if (x) is within the range and false
  otherwise.

  written by Dana Swift, March 1993
*/
int inRange(double a,double x,double b)
{
   return ( ((x-a)*(x-b)<=0 && x!=b) ? 1 : 0);
}

/*========================================================================*/
/* function to determine if a number is within a closed interval          */
/*========================================================================*/
/*
  This function determines whether a number (x) is in the closed interval
  of [a,b].  The function returns true if (x) is within the range and false
  otherwise.

  written by Dana Swift, October 1995
*/
int inCRange(double a,double x,double b)
{
   return ( ((x-a)*(x-b)<=0) ? 1 : 0);
}
