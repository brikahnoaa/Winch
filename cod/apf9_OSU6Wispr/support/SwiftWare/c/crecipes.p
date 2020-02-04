#ifndef SWIFTWARE
#define SWIFTWARE

#undef  PI
#define PI     (3.14159265358979323846)
#define ERROR  (32767)
#define DECAY  (0.36787944)

#define open_file(FNAME,MODE,FP) if ((FP=fopen(FNAME,MODE))==0) \
{ \
   swifterr("\nUnable to open \"%s\" using mode \"%s\".\n",FNAME,MODE); \
}

#define check_cmd_line(arg,cmd_string) \
{ \
   cmd_stderr(&argc,argv); \
   if (argc<arg+1) swifterr("usage:%s %s\n",argv[0],cmd_string); \
}

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C"
{
#endif

   #include <signature.h>
   
   /* declare structure for modeling metacommands */
   struct MetaCmd
   {
      #define MAXCMDLEN (25)
      #define MAXDESCRIPTIONLEN (71)

      char cmd[MAXCMDLEN+1];
      char description[MAXDESCRIPTIONLEN+1];
      int  len;
      char *arg;

      #undef MAXCMDLEN
      #undef MAXDESCRIPTIONLEN
   };

   /* declare a structure to represent a point and a line segment in 3-space */
   struct Point {double x,y,z;};
   struct LineSegment {struct Point p1,p2;};

   /* this is a general purpose scratch buffer (used by extract and others) */
   #define MAXSTRLEN (4096)
   extern char scrbuf[MAXSTRLEN+1];

   /* This allows stuff which ordinarily goes to stderr to be put elsewhere */
   /* if I'm gone and want to log error messages */
   extern FILE *swiftware_stderr_ptr;  
   #define swiftware_stderr get_stderr()

   /* define macros */
   #define lower_case(chr) ( (chr>='A' && chr<='Z') ? chr+32 : chr )
   #define n_(i,j,col_dim) ( ((i)*(col_dim)) + (j) )
   #define upper_case(chr) ( (chr>='a' && chr<='z') ? chr-32 : chr )
   #define fswap(a,b) {double tmp=(a); (a)=(b); (b)=tmp;}

   /* a macro to test if fatal status bits are set */
   #define fatal(status) (0xFF00 & (status))

   /* declare functon prototypes */
   void     cmd_stderr(int *argc,char *argv[]);
   char    *copy(char source[],int index,int nchr,char dest[]);
   int      dfindbin(double *x,long npnt,double xtrg,long *ndx);
   void     dmachar(int *ibeta,int *it,int *irnd,int *ngrd,int *machep,
                    int *negep,int *iexp,int *minexp,int *maxexp,double *eps,
                    double *epsneg,double *xmin,double *xmax);
   void     dmatmul_(double lft[],double rt[],double prd[],int lmr,int lmc,int rmc,
                     int lcd,int rcd,int pcd);
   void     doLeader(char *leader,FILE *dest);
   int      dsolve(double a[],double x[],double b[],int rank);
   double   eqstate(float salt,float temp,float pres);
   char    *extract(const char source[],int index,int nchr);
   int      findbin(const float *x,long npnt,const float xtrg,long *ndx);
   int      get_bit(int status,int bit);
   FILE    *get_ReqLogFile(void);
   FILE    *get_stderr(void);
   void     hunt(float xx[], unsigned long n, float x, unsigned long *jlo);
   int      inRange(double a,double x,double b);
   int      inCRange(double a,double x,double b);
   void     link_meta_cmds(struct MetaCmd *cmd,int ncmd,int argc,char *argv[]);
   void     machar(int *ibeta, int *it, int *irnd, int *ngrd,int *machep,
                   int *negep, int *iexp, int *minexp, int *maxexp, float *eps,
                   float *epsneg, float *xmin, float *xmax);
   char    *make_usage(struct MetaCmd *cmd,int ncmd); 
   void     matmul_(float lft[],float rt[],float prd[],int lmr,int lmc,int rmc,int lcd,
                  int rcd,int pcd);
   void    *MemReq(void *ptr,int n,int size,char *funcname);
   void     message(char *format,...);
   double   potential_temp(float salt,float temp,float pres,float refpres);
   void     set_bit(int *status,int bit);
   void     set_MemReqLog(FILE *ptr);
   int      solve(float a[],float x[],float b[],int rank);
   void     swifterr(char *format,...);
   char    *strloc(FILE *source,char *key,char *dest);
   char    *strlwr(char string[]);
   int      strseek(const char *key,FILE *source);
   char    *strupr(char string[]);
   void     validate(void *ptr,char *ptr_name,char *func_name);

#ifdef __cplusplus
}
#endif

#endif /* SWIFTWARE */
