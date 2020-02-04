/****************************************/
/*     member of utils library          */
/****************************************/
#include <defs.p>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: status.c,v 1.1.1.1 2003/11/12 23:22:58 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: status.c,v $
 * Revision 1.1.1.1  2003/11/12 23:22:58  swift
 * Apf9 firmware for a 260ml Apex with SBE41 CTD.
 *
 * Revision 1.2  2001/05/13 10:38:45  navo
 * Modifications for porting from UW to NAVO.
 *
 * Revision 1.1  1996/06/15 15:51:39  swift
 * Initial revision
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*========================================================================*/
/*   procedure to set individual status bits                              */
/*========================================================================*/
void set_bit(int *status,int bit)
{
   int one=1;

   /* set bit high */
   if (bit>0 && bit<=16) {*status = *status | (one<<(bit-1));}
   
   /* set all bits low and reinitialize status_ array */
   else if (bit==0) {*status = 0;}
   
   /* set bit low */
   else if (bit>=-16 && bit<0) {*status = *status & (~(one << ( (bit*=-1) - 1)) );}

   else {swifterr("set_bit(%d): bit operation out of range [-16 to 16]\n",bit);}
}

/*========================================================================*/
/*   function to determine if integer bit is high of low                  */
/*========================================================================*/
int get_bit(int status,int bit)
{
   int one=1;

   if (bit<=0 || bit>16) {swifterr("bit_set(%d): bit operation out of range [1 to 16]\n",bit);}

   if ( status & (one<<(bit-1)) ) return(1);
   else return(0);
}
