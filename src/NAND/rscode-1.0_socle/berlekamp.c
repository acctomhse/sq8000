/***********************************************************************
 * Berlekamp-Peterson and Berlekamp-Massey Algorithms for error-location
 *
 * From Cain, Clark, "Error-Correction Coding For Digital Communications", pp. 205.
 *
 * This finds the coefficients of the error locator polynomial.
 *
 * The roots are then found by looking for the values of a^n
 * where evaluating the polynomial yields zero.
 *
 * Error correction is done using the error-evaluator equation  on pp 207.
 *
 * hqm@ai.mit.edu   Henry Minsky
 */

/* #include <stdio.h> */
#include "genlib.h"
#include "ecc.h"


#define DEBUG 1
/* The Error Locator Polynomial, also known as Lambda or Sigma. Lambda[0] == 1 */
static unsigned short Lambda[MAXDEG];

/* The Error Evaluator Polynomial */
static unsigned short Omega[MAXDEG];

/* local ANSI declarations */
static int compute_discrepancy(unsigned short lambda[], unsigned short S[], int L, int n);
static void init_gamma(unsigned short gamma[]);
static void compute_modified_omega (void);
static void mul_z_poly (unsigned short src[]);

/* error locations found using Chien's search*/
static int ErrorLocs[256];
static int NErrors;

/* erasure flags */
static int ErasureLocs[256];
static int NErasures;

/* From  Cain, Clark, "Error-Correction Coding For Digital Communications", pp. 216. */
void
Modified_Berlekamp_Massey (void)
{	
  int n, L, L2, k, d, i;
  unsigned short psi[MAXDEG], psi2[MAXDEG], D[MAXDEG];
  unsigned short gamma[MAXDEG];
	
  /* initialize Gamma, the erasure locator polynomial */
  init_gamma(gamma);

  /* initialize to z */
  copy_poly(D, gamma);
  mul_z_poly(D);
	
  copy_poly(psi, gamma);	
  k = -1; L = NErasures;
	
  for (n = NErasures; n < npar; n++) {
	
    d = compute_discrepancy(psi, synBytes, L, n);
		
    if (d != 0) {
		
      /* psi2 = psi - d*D */
      for (i = 0; i < MAXDEG; i++) psi2[i] = psi[i] ^ gmult(d, D[i]);
		
		
      if (L < (n-k)) {
	L2 = n-k;
	k = n-L;
	/* D = scale_poly(ginv(d), psi); */
	for (i = 0; i < MAXDEG; i++) D[i] = gmult(psi[i], ginv(d));
	L = L2;
      }
			
      /* psi = psi2 */
      for (i = 0; i < MAXDEG; i++) psi[i] = psi2[i];
    }
		
    mul_z_poly(D);
  }
	
  for(i = 0; i < MAXDEG; i++) Lambda[i] = psi[i];
  compute_modified_omega();

	
}

/* given Psi (called Lambda in Modified_Berlekamp_Massey) and synBytes,
   compute the combined erasure/error evaluator polynomial as 
   Psi*S mod z^4
  */
void
compute_modified_omega ()
{
  int i;
  unsigned short product[MAXDEG*2];
	
  mult_polys(product, Lambda, synBytes);	
  zero_poly(Omega);
  for(i = 0; i < npar; i++) Omega[i] = product[i];

}

/* polynomial multiplication */
void
mult_polys (unsigned short dst[], unsigned short p1[], unsigned short p2[])
{
  int i, j;
  int tmp1[MAXDEG*2];
	

  for (i=0; i < (MAXDEG*2); i++) dst[i] = 0;
	
  for (i = 0; i < MAXDEG; i++) {
    for(j=MAXDEG; j<(MAXDEG*2); j++) tmp1[j]=0;
		
    /* scale tmp1 by p1[i] */
    for(j=0; j<MAXDEG; j++) tmp1[j]=gmult(p2[j], p1[i]);
    /* and mult (shift) tmp1 right by i */
    for (j = (MAXDEG*2)-1; j >= i; j--) tmp1[j] = tmp1[j-i];
    for (j = 0; j < i; j++) tmp1[j] = 0;
		
    /* add into partial product */
    for(j=0; j < (MAXDEG*2); j++) dst[j] ^= tmp1[j];
  }
}


	
/* gamma = product (1-z*a^Ij) for erasure locs Ij */
void
init_gamma (unsigned short gamma[])
{
  int e;
  unsigned short tmp[MAXDEG];
	
  zero_poly(gamma);
  zero_poly(tmp);
  gamma[0] = 1;
	
  for (e = 0; e < NErasures; e++) {
    copy_poly(tmp, gamma);
    scale_poly(gexp[ErasureLocs[e]], tmp);
    mul_z_poly(tmp);
    add_polys(gamma, tmp);
  }
}
	
	
	
void 
compute_next_omega (int d, int A[], int dst[], int src[])
{
  int i;
  for ( i = 0; i < MAXDEG;  i++) {
    dst[i] = src[i] ^ gmult(d, A[i]);
  }
}
	


int
compute_discrepancy (unsigned short lambda[], unsigned short S[], int L, int n)
{
  int i, sum=0;
	
  for (i = 0; i <= L; i++) 
    sum ^= gmult(lambda[i], S[n-i]);
  return (sum);
}

/********** polynomial arithmetic *******************/

void add_polys (unsigned short dst[], unsigned short src[]) 
{
  int i;
  for (i = 0; i < MAXDEG; i++) dst[i] ^= src[i];
}

void copy_poly (unsigned short dst[], unsigned short src[]) 
{
  int i;
  for (i = 0; i < MAXDEG; i++) dst[i] = src[i];
}

void scale_poly (int k, unsigned short poly[]) 
{	
  int i;
  for (i = 0; i < MAXDEG; i++) poly[i] = gmult(k, poly[i]);
}


void zero_poly (unsigned short poly[]) 
{
  int i;
  for (i = 0; i < MAXDEG; i++) poly[i] = 0;
}


/* multiply by z, i.e., shift right by 1 */
static void mul_z_poly (unsigned short src[])
{
  int i;
  for (i = MAXDEG-1; i > 0; i--) src[i] = src[i-1];
  src[0] = 0;
}


/* Finds all the roots of an error-locator polynomial with coefficients
 * Lambda[j] by evaluating Lambda at successive values of alpha. 
 * 
 * This can be tested with the decoder's equations case.
 */


void 
Find_Roots (void)
{
  int sum, r, k;	
  NErrors = 0;
  
//  for (r = 1; r < 256; r++) {
  for (r = 1; r < 1024; r++) {
    sum = 0;
    /* evaluate lambda at r */
    for (k = 0; k < npar+1; k++) {
//      sum ^= gmult(gexp[(k*r)%255], Lambda[k]);
      sum ^= gmult(gexp[(k*r)%1023], Lambda[k]);
    }
    if (sum == 0) 
      { 
//	ErrorLocs[NErrors] = (255-r); NErrors++; 
	ErrorLocs[NErrors] = (1023-r); NErrors++; 
//	if (DEBUG) fprintf(stderr, "Root found at r = %d, (255-r) = %d\n", r, (255-r));
	if (DEBUG) printf("Root found at r = %d, (1023-r) = %d\n", r, (1023-r));
      }
  }
}

/* Combined Erasure And Error Magnitude Computation 
 * 
 * Pass in the codeword, its size in bytes, as well as
 * an array of any known erasure locations, along the number
 * of these erasures.
 * 
 * Evaluate Omega(actually Psi)/Lambda' at the roots
 * alpha^(-i) for error locs i. 
 *
 * Returns 1 if everything ok, or 0 if an out-of-bounds error is found
 *
 */

int correct_errors_erasures (unsigned short codeword[], 
//int correct_errors_erasures (unsigned char codeword[], 
			 int csize)
{
  int r, i=0, j, err;

  /* If you want to take advantage of erasure correction, be sure to
     set NErasures and ErasureLocs[] with the locations of erasures. 
     */

  Modified_Berlekamp_Massey();
  Find_Roots();
  

  if ((NErrors <= npar) && NErrors > 0) { 

    /* first check for illegal error locs */
    for (r = 0; r < NErrors; r++) {
      if (ErrorLocs[r] >= csize) {
	if (DEBUG) printf("Error loc i=%d outside of codeword length %d\n", i, csize);
	return(0);
      }
    }

    for (r = 0; r < NErrors; r++) {
      int num, denom;
      i = ErrorLocs[r];
      /* evaluate Omega at alpha^(-i) */

      num = 0;
      for (j = 0; j < MAXDEG; j++) 
//	num ^= gmult(Omega[j], gexp[((255-i)*j)%255]);
	num ^= gmult(Omega[j], gexp[((1023-i)*j)%1023]);
      
      /* evaluate Lambda' (derivative) at alpha^(-i) ; all odd powers disappear */
      denom = 0;
      for (j = 1; j < MAXDEG; j += 2) {
//	denom ^= gmult(Lambda[j], gexp[((255-i)*(j-1)) % 255]);
	denom ^= gmult(Lambda[j], gexp[((1023-i)*(j-1)) % 1023]);
      }
      
      err = gmult(num, ginv(denom));
      if (DEBUG) printf("Error magnitude %#x at loc %d\n", err, csize-i);
      
//      codeword[csize-i-1] ^= err;
      codeword[csize-i-1] ^= (unsigned char)err;
    }
    return(1);
  }
  else {
    if (DEBUG && NErrors) printf("Uncorrectable codeword\n");
    return(0);
  }
}

int correct_errors_erasures_char (unsigned char codeword[], 
			 int csize)
{
  int r, i=0, j, err;

  /* If you want to take advantage of erasure correction, be sure to
     set NErasures and ErasureLocs[] with the locations of erasures. 
     */

  Modified_Berlekamp_Massey();
  Find_Roots();
  

  if ((NErrors <= npar) && NErrors > 0) { 

    /* first check for illegal error locs */
    for (r = 0; r < NErrors; r++) {
      if (ErrorLocs[r] >= csize) {
	if (DEBUG) printf("Error loc i=%d outside of codeword length %d\n", i, csize);
	return(0);
      }
    }

    for (r = 0; r < NErrors; r++) {
      int num, denom;
      i = ErrorLocs[r];
      /* evaluate Omega at alpha^(-i) */

      num = 0;
      for (j = 0; j < MAXDEG; j++) 
//	num ^= gmult(Omega[j], gexp[((255-i)*j)%255]);
	num ^= gmult(Omega[j], gexp[((1023-i)*j)%1023]);
      
      /* evaluate Lambda' (derivative) at alpha^(-i) ; all odd powers disappear */
      denom = 0;
      for (j = 1; j < MAXDEG; j += 2) {
//	denom ^= gmult(Lambda[j], gexp[((255-i)*(j-1)) % 255]);
	denom ^= gmult(Lambda[j], gexp[((1023-i)*(j-1)) % 1023]);
      }
      
      err = gmult(num, ginv(denom));
      if (DEBUG) printf("Error magnitude %#x at loc %d\n", err, csize-i);
      
	{
	int index;
	unsigned long tmp;
	unsigned long *p;
	p=(unsigned long*)codeword;
	p+=(csize-i-1)/4;
	index=(csize-i-1)%4;
	tmp=err<<(index*8);
	*p ^= tmp;
//      codeword[csize-i-1] ^= err;
//      codeword[csize-i-1] ^= (unsigned char)err;
	}
    }
    return(1);
  }
  else {
    if (DEBUG && NErrors) printf("Uncorrectable codeword\n");
    return(0);
  }
}

extern int
correct_errors_hw_ecc(unsigned char *codeword, int csize)
{
     int r, i =0, j, err;

     /* If you want to take advantage of erasure correction, be sure to
      * set NErasures and ErasureLocs[] with the locations of erasures.
      * */

     Modified_Berlekamp_Massey();
     Find_Roots();

     if ((NErrors <= npar) && (NErrors > 0)) {
	  /* First check for illegal error locs */
	  for (r = 0; r < NErrors; r++) {
	       if (ErrorLocs[r] >= csize) {
		    if (DEBUG)
			 printf("Error loc i = %d outside ot codeword length %d\n", i, csize);
		    return 1;
	       }
	  }

	  for (r = 0; r < NErrors; r++) {
	       int num, denom;
	       i = ErrorLocs[r];

	       /* Evaluate Omega at alpha^(-i) */
	       num = 0;
	       for (j = 0; j < MAXDEG; j++)
		    num ^= gmult(Omega[j], gexp[((1023-i)*j)%1023]);

	       /* Evaluate Lambda' (derivative) at alpha^(-i) ; all odd powers disappear */
	       denom = 0;
	       for (j = 1; j < MAXDEG; j += 2)
		    denom ^= gmult(Lambda[j], gexp[((1023-i)*(j-1)) % 1023]);

	       err = gmult(num, ginv(denom));
	       if (DEBUG)
		    printf("Error magnitue %#x at loc %d\n", err, csize-i-1);

	       /* Skip the correction on ecc code area */
	       if ((csize - i - 1) > (csize - npar - 1))
		    continue;

	       codeword[csize-i-1] ^= (unsigned char)err;
	       if (DEBUG)
		    printf("Corrected value is %x\n", codeword[csize-i-1]);
	  }
	  return 0;
     } else {
	  if (DEBUG && NErrors)
	       printf("Uncorrectable codeword\n");
	  return 1;
     }
}
