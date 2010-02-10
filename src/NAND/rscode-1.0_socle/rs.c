/* #include <stdio.h> */
/* #include <ctype.h> */
#include "genlib.h"
#include "ecc.h"

unsigned short pBytes[MAXDEG];
unsigned short synBytes[MAXDEG];
unsigned short genPoly[MAXDEG*2];

int DEBUG = FALSE;

static void
compute_genpoly (int nbytes, unsigned short genpoly[]);


void
initialize_ecc ()
{  
    init_galois_tables();

    compute_genpoly(npar, genPoly);
}


#if 0
void
mult_polys (unsigned short dst[], unsigned short p1[], unsigned short p2[])
{
  int i, j;
  int tmp1[MAXDEG*2];
	

  for (i=0; i < (MAXDEG*2); i++) dst[i] = 0;
	
  for (i = 0; i < MAXDEG; i++) {
    for(j=MAXDEG; j<(MAXDEG*2); j++) tmp1[j]=0;
		

    for(j=0; j<MAXDEG; j++) tmp1[j]=gmult(p2[j], p1[i]);

    for (j = (MAXDEG*2)-1; j >= i; j--) tmp1[j] = tmp1[j-i];
    for (j = 0; j < i; j++) tmp1[j] = 0;
		

    for(j=0; j < (MAXDEG*2); j++) dst[j] ^= tmp1[j];
  }
}

void copy_poly (unsigned short dst[], unsigned short src[]) 
{
  int i;
  for (i = 0; i < MAXDEG; i++) dst[i] = src[i];
}

void zero_poly (unsigned short poly[]) 
{
  int i;
  for (i = 0; i < MAXDEG; i++) poly[i] = 0;
}
#endif


void
zero_fill_from (unsigned char buf[], int from, int to)
{
  int i;
  for (i = from; i < to; i++) buf[i] = 0;
}


void
print_parity (void)
{ 
  int i;
  printf("Parity Bytes: ");
  for (i = 0; i < npar; i++) 
    printf("[%d]:%x, ",i,pBytes[i]);
  printf("\n");
}


void print_syndrome (void)
{ 
  int i;
  printf("Syndrome Bytes:");
  for (i = 0; i < npar; i++) 
    printf("[%d]:%3x ",i,synBytes[i]);
  printf("\n");
}

/* Append the parity bytes onto the end of the message */
void
build_codeword (unsigned char msg[], int nbytes, unsigned short dst[])
{
  int i;
	
  for (i = 0; i < nbytes; i++) dst[i] = msg[i];
	
  for (i = 0; i < npar; i++) {
    dst[i+nbytes] = pBytes[npar-1-i];
  }
}
	
 
void
decode_data(unsigned short data[], int nbytes)
{
  int i, j, sum;
  for (j = 0; j < npar;  j++) {
    sum	= 0;
    for (i = 0; i < nbytes; i++) {
      sum = data[i] ^ gmult(gexp[j+1], sum);
    }
    synBytes[j]  = sum;
  }
}

int
check_syndrome (void)
{
  int i, nz = 0;
  for (i =0 ; i < npar; i++) {
    if (synBytes[i] != 0) nz = 1;
  }
  return nz;
}


void
debug_check_syndrome (void)
{	
  int i;
	
  for (i = 0; i < 3; i++) {
    printf(" inv log S[%d]/S[%d] = %d\n", i, i+1, 
	   glog[gmult(synBytes[i], ginv(synBytes[i+1]))]);
  }
}


static void
compute_genpoly (int nbytes, unsigned short genpoly[])
{
  unsigned short i, tp[256], tp1[256];

  zero_poly(tp1);
  tp1[0] = 1;

  for (i = 1; i <= nbytes; i++) {
    zero_poly(tp);
    tp[0] = gexp[i];		/* set up x+a^n */
    tp[1] = 1;
	  
    mult_polys(genpoly, tp, tp1);
    copy_poly(tp1, genpoly);
  }
}

void
encode_data (unsigned char msg[], int nbytes, unsigned short dst[])
{
  int i, LFSR[npar+1],dbyte, j;
	
  for(i=0; i < npar+1; i++) LFSR[i]=0;

  for (i = 0; i < nbytes; i++) {
    dbyte = msg[i] ^ LFSR[npar-1];
    for (j = npar-1; j > 0; j--) {
      LFSR[j] = LFSR[j-1] ^ gmult(genPoly[j], dbyte);
    }
    LFSR[0] = gmult(genPoly[0], dbyte);
  }

  for (i = 0; i < npar; i++) 
    pBytes[i] = LFSR[i];

  build_codeword(msg, nbytes, dst);
}

