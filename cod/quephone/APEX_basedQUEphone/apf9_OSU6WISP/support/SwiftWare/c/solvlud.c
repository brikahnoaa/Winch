/****************************************/
/*       member of math library         */
/****************************************/
#include <defs.p>

#ifndef PRECISION
   #define PRECISION  float
   #define SOLVE      solve
   #define MATMUL_    matmul_
   #define SOLVE_FREE solve_free
#endif

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: solvlud.c,v 1.1 2006/11/18 17:20:24 swift Exp $
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RCS Log:
 *
 * $Log: solvlud.c,v $
 * Revision 1.1  2006/11/18 17:20:24  swift
 * Added new functionality to library.
 *
 * Revision 1.2  1996/08/24 20:24:36  swift
 * Removed reference to Macintosh builds.  Fixed minor glitch
 * in genericity between float & double.
 *
 * Revision 1.1  1996/06/15 16:33:11  swift
 * Initial revision
 *
 *========================================================================*/
/* function to solve matrix equation and return solution vector           */
/*========================================================================*/
/* declare funtion prototypes */
   static int  col_index_of_piv_row_max(PRECISION *work,int rank,int piv_row);
   static int  row_index_of_piv_col_max(PRECISION *work,int rank,int piv_col);
   static int  row_permutation_required(PRECISION *work,int rank,int piv_col,int imax,int jmax);
   static int  col_permutation_required(PRECISION *work,int rank,int piv_row,int imax,int jmax);
   static void permute_rows(PRECISION *work,int rank,int piv_row,int imax);
   static void permute_cols(PRECISION *work,int rank,int piv_col,int jmax,int var_pos_vectr[]);
   static void make_strang_mat(PRECISION *work,PRECISION *strang,int rank,int piv_row);
   void SOLVE_FREE(PRECISION *work,PRECISION *prd,PRECISION *strang,PRECISION *soln,int *var_pos_vectr);
/*
   This procedure solves the matrix equation Ax = b if a unique solution exists.
   The matrix A[][] is a rank x rank matrix and the vector b[] has dimension
   rank. If no solution exists, the funtion returns a zero value as well as sets
   the solution vector x[] to the null vector, a value of one is returned if the
   algorithm was successfull.  The solution algorithm is Gaussian elimination
   followed by forward substitution.  Row or column permutations are made as
   necessary in order to maximize the pivot element.

   Written by Dana Swift (July 1988)

*/
int SOLVE
(
   PRECISION   a[],
   PRECISION   x[],
   PRECISION   b[],
   int      rank
)
{
   PRECISION *work,*strang,*prd,*soln,*tmp;
   int i,j,piv_col,piv_row,*var_pos_vectr,imax,jmax;

   /* reserve space for work matrix */
   work = (PRECISION *)MemReq(NULL,(unsigned)(rank*(rank+1)),sizeof(PRECISION),"SOLVE(<work>)");

   /* reserve space for product matrix */
   prd = (PRECISION *)MemReq(NULL,(unsigned)(rank*(rank+1)),sizeof(PRECISION),"SOLVE(<prd>)");

   /* reserve space for strang matrix */
   strang = (PRECISION *)MemReq(NULL,(unsigned)(rank*rank),sizeof(PRECISION),"SOLVE(<strang>)");

   /* reserve space for solution vector */
   soln = (PRECISION *)MemReq(NULL,(unsigned)rank,sizeof(PRECISION),"SOLVE(<soln>)");

   /* reserve space for variable location vector */
   var_pos_vectr = (int *)MemReq(NULL,(unsigned)rank,sizeof(int),"SOLVE(<var_pos_vectr>)");

   /* initialize variable position vector and solution vector */
   for (i=0; i<rank; i++) {var_pos_vectr[i] = i; x[i] = 0.0;}

   /* insert characteristic matrix to left of result matrix in work array */
   for (i=0; i<rank; i++) 
   {
      for (j=0; j<rank; j++) work[n_(i,j,rank+1)] = a[n_(i,j,rank)];
      work[n_(i,rank,rank+1)] = b[i];
   }

   /* Gaussian elimination and forward substitution used below with row & col permutations */
   for (piv_row=0,piv_col=0; piv_col<rank; piv_col++,piv_row++) {

      /* search current pivot row to left of pivot col for max */
      jmax = col_index_of_piv_row_max(work,rank,piv_row);

      /* search current pivot col below pivot row for max */
      imax = row_index_of_piv_col_max(work,rank,piv_col);

      /* determine if row or column permutation required and do as req'd */
      if (row_permutation_required(work,rank,piv_col,imax,jmax)) 
      {
         /* permute piv_row and imax row */
         permute_rows(work,rank,piv_row,imax); 
      }
      else if (col_permutation_required(work,rank,piv_row,imax,jmax)) 
      {
         /* permute piv_col and jmax col */
         permute_cols(work,rank,piv_col,jmax,var_pos_vectr);
      }

      /* check for zero pivot indicating no inverse exists */
      if (!work[n_(piv_row,piv_col,rank+1)]) 
      {
         SOLVE_FREE(work,prd,strang,soln,var_pos_vectr); return(0);
      }

      /* create strang matrix to perform gaussian reduction */
      make_strang_mat(work,strang,rank,piv_row);

      /* perform gaussian reduction by multiplying augmented matrix by strang matrix */
      MATMUL_(strang,work,prd,rank,rank,rank+1,rank,rank+1,rank+1); 

      /* swap the matrices */
      tmp = work; work = prd; prd = tmp;
   }

   /* compute solution vector to permuted system */
   for (i=rank-1; i>=0; i--) 
   {
      for (soln[i]=0,j=i; j<rank; j++) if (j!=i) soln[i] -= work[n_(i,j,rank+1)] * soln[j];
      soln[i] += work[n_(i,rank,rank+1)]; soln[i] /= work[n_(i,i,rank+1)];
   }

   /* un-permute solution vector */
   for (i=0; i<rank; i++) x[var_pos_vectr[i]] = soln[i];

   /* release work space */
   SOLVE_FREE(work,prd,strang,soln,var_pos_vectr);

   return(1);
}

/*------------------------------------------------------------------------*/
/* function to release work space back to heap                            */
/*------------------------------------------------------------------------*/
void SOLVE_FREE(PRECISION *work,PRECISION *prd,PRECISION *strang,PRECISION *soln,int *var_pos_vectr)
{
   MemReq(work,0,0,"SOLVE_FREE(<work>)");
   MemReq(prd,0,0,"SOLVE_FREE(<prd>)");
   MemReq(strang,0,0,"SOLVE_FREE(<strang>)");
   MemReq(soln,0,0,"SOLVE_FREE(<soln>)");
   MemReq(var_pos_vectr,0,0,"SOLVE_FREE(<var_pos_vectr>)");
}

/*------------------------------------------------------------------------*/
/* Procedure to search current pivot row to left of pivot col for max     */
/*------------------------------------------------------------------------*/
static int col_index_of_piv_row_max
(
   PRECISION   *work,
   int      rank,
   int      piv_row
)
{
   int j,jmax;

   for (jmax=piv_row,j=piv_row; j<rank; j++) 
   {
      if (fabs(work[n_(piv_row,j,rank+1)]) > fabs(work[n_(piv_row,jmax,rank+1)])) jmax=j;
   }

   return(jmax);
}

/*------------------------------------------------------------------------*/
/* Procedure to search current pivot col below pivot row for max          */
/*------------------------------------------------------------------------*/
static int row_index_of_piv_col_max
   (
      PRECISION   *work,
      int      rank,
      int      piv_col
   )
{
   int i,imax;

   for (imax=piv_col,i=piv_col; i<rank; i++) 
   {
      if (fabs(work[n_(i,piv_col,rank+1)]) > fabs(work[n_(imax,piv_col,rank+1)])) imax=i;
   }

   return(imax);
}

/*------------------------------------------------------------------------*/
/* function to determine if row permutation required to maximize pivot    */
/*------------------------------------------------------------------------*/
static int row_permutation_required
(
   PRECISION   *work,
   int      rank,
   int      piv_col,
   int      imax,
   int      jmax
)
{
   int rpr=0,piv_row;
   piv_row = piv_col;

   if (work[n_(imax,piv_col,rank+1)] >= work[n_(piv_row,jmax,rank+1)]) 
   {
      if (imax!=piv_row) rpr = 1;
   }

   return(rpr);
}

/*------------------------------------------------------------------------*/
/* function to determine if row permutation required to maximize pivot    */
/*------------------------------------------------------------------------*/
static int col_permutation_required
(
   PRECISION   *work,
   int      rank,
   int      piv_row,
   int      imax,
   int      jmax
)
{
   int cpr=0,piv_col;
   piv_col = piv_row;

   if (work[n_(imax,piv_col,rank+1)] < work[n_(piv_row,jmax,rank+1)])
   {
      if (jmax!=piv_col) cpr = 1;
   }

   return(cpr);
}

/*------------------------------------------------------------------------*/
/* procedure to permute two rows                                          */
/*------------------------------------------------------------------------*/
static void permute_rows
(
   PRECISION   *work,
   int      rank,
   int      piv_row,
   int      imax
)
{
   int j;
   PRECISION tmp;

   for (j=0; j<=rank; j++) 
   {
      tmp = work[n_(piv_row,j,rank+1)];
      work[n_(piv_row,j,rank+1)] = work[n_(imax,j,rank+1)];
      work[n_(imax,j,rank+1)] = tmp;
   }
}

/*------------------------------------------------------------------------*/
/* procedure to permute two columns                                       */
/*------------------------------------------------------------------------*/
static void permute_cols
(
   PRECISION   work[],
   int      rank,
   int      piv_col,
   int      jmax,
   int      var_pos_vectr[]
)
{
   int i,itmp;
   PRECISION tmp;

   for (i=0; i<rank; i++) 
   {
      tmp = work[n_(i,piv_col,rank+1)];
      work[n_(i,piv_col,rank+1)] = work[n_(i,jmax,rank+1)];
      work[n_(i,jmax,rank+1)] = tmp;
   }

   /* adjust variable position vectr */
   itmp = var_pos_vectr[piv_col];
   var_pos_vectr[piv_col] = var_pos_vectr[jmax];
   var_pos_vectr[jmax] = itmp;
}

/*------------------------------------------------------------------------*/
/* procedure to create strang matrix to perform gaussian reduction        */
/*------------------------------------------------------------------------*/
static void make_strang_mat
(
   PRECISION   *work,
   PRECISION   *strang,
   int      rank,
   int      piv_row
)
{
   int i,j,piv_col;

   /* create unit matrix */
   for (i=0; i<rank; i++) 
   {
      for (j=0; j<rank; j++) strang[n_(i,j,rank)] = 0.0;
      strang[n_(i,i,rank)] = 1.0;
   }

   /* insert reduction coefficients into strang matrix */
   for (piv_col=piv_row,i=piv_row+1; i<rank; i++) 
   {
       strang[n_(i,piv_col,rank)] = -work[n_(i,piv_col,rank+1)] / work[n_(piv_row,piv_col,rank+1)];
   }
}
 
