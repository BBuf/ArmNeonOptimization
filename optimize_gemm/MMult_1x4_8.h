#include <stdio.h>

#define A(i,j) a[ (i)*lda + (j) ]
#define B(i,j) b[ (i)*ldb + (j) ]
#define C(i,j) c[ (i)*ldc + (j) ]

/* Routine for computing C = A * B + C */

void AddDot1x4( int k, float *a, int lda,  float *b, int ldb, float *c, int ldc )
{
  /* So, this routine computes four elements of C: 
           C( 0, 0 ), C( 0, 1 ), C( 0, 2 ), C( 0, 3 ).  
     Notice that this routine is called with c = C( i, j ) in the
     previous routine, so these are actually the elements 
           C( i, j ), C( i, j+1 ), C( i, j+2 ), C( i, j+3 ) 
	  
     in the original matrix C.
     We now unroll the loop */

  int p;
  register float 
    /* hold contributions to
       C( 0, 0 ), C( 0, 1 ), C( 0, 2 ), C( 0, 3 ) */
       c_00_reg,   c_01_reg,   c_02_reg,   c_03_reg,  
    /* holds A( 0, p ) */
       a_0p_reg;
  float 
    /* Point to the current elements in the four columns of B */
    *bp0_pntr, *bp1_pntr, *bp2_pntr, *bp3_pntr; 
    
  bp0_pntr = &B( 0, 0 );
  bp1_pntr = &B( 0, 1 );
  bp2_pntr = &B( 0, 2 );
  bp3_pntr = &B( 0, 3 );

  c_00_reg = 0.0; 
  c_01_reg = 0.0; 
  c_02_reg = 0.0; 
  c_03_reg = 0.0;
 
  for ( p=0; p<k; p+=4 ){
    a_0p_reg = A( 0, p );

    c_00_reg += a_0p_reg * *bp0_pntr++;
    c_01_reg += a_0p_reg * *bp1_pntr++;
    c_02_reg += a_0p_reg * *bp2_pntr++;
    c_03_reg += a_0p_reg * *bp3_pntr++;

    a_0p_reg = A( 0, p+1 );

    c_00_reg += a_0p_reg * *bp0_pntr++;
    c_01_reg += a_0p_reg * *bp1_pntr++;
    c_02_reg += a_0p_reg * *bp2_pntr++;
    c_03_reg += a_0p_reg * *bp3_pntr++;

    a_0p_reg = A( 0, p+2 );

    c_00_reg += a_0p_reg * *bp0_pntr++;
    c_01_reg += a_0p_reg * *bp1_pntr++;
    c_02_reg += a_0p_reg * *bp2_pntr++;
    c_03_reg += a_0p_reg * *bp3_pntr++;

    a_0p_reg = A( 0, p+3 );

    c_00_reg += a_0p_reg * *bp0_pntr++;
    c_01_reg += a_0p_reg * *bp1_pntr++;
    c_02_reg += a_0p_reg * *bp2_pntr++;
    c_03_reg += a_0p_reg * *bp3_pntr++;
  }

  C( 0, 0 ) += c_00_reg; 
  C( 0, 1 ) += c_01_reg; 
  C( 0, 2 ) += c_02_reg; 
  C( 0, 3 ) += c_03_reg;
}

void MY_MMult_1x4_8( int m, int n, int k, float *a, int lda, 
                                    float *b, int ldb,
                                    float *c, int ldc )
{
  int i, j;

  for ( j=0; j<n; j+=4 ){        /* Loop over the columns of C, unrolled by 4 */
    for ( i=0; i<m; i+=1 ){        /* Loop over the rows of C */
      /* Update C( i,j ), C( i,j+1 ), C( i,j+2 ), and C( i,j+3 ) in
	 one routine (four inner products) */

      AddDot1x4( k, &A( i,0 ), lda, &B( 0,j ), ldb, &C( i,j ), ldc );
    }
  }
}


