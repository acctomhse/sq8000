/* #include <stdio.h> */
/* #include <stdlib.h> */
#include "ecc.h"


#define PPOLY 0x1D 


int gexp[2048];
int glog[1024];


static void init_exp_table (void);


void
init_galois_tables (void)
{	

  init_exp_table();
}


static void
init_exp_table (void)
{
  int i, z;
  int pinit,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10;

  pinit = p2 = p3 = p4 = p5 = p6 = p7 = p8 = p9 = p10 =0;
  p1 = 1;

  gexp[0] = 1;
  gexp[1023] = gexp[0];
  glog[0] = 0;		

  for (i = 1; i < 1024; i++) {
    pinit = p10;
    p10= p9;
    p9 = p8;
    p8 = p7;
    p7 = p6;
    p6 = p5;
    p5 = p4 ;
    p4 = p3 ^ pinit;
    p3 = p2 ;
    p2 = p1;
    p1 = pinit;
    gexp[i] = p1 + p2*2 + p3*4 + p4*8 + p5*16 + p6*32 + p7*64 + p8*128 + p9*256 + p10*512;
    gexp[i+1023] = gexp[i];
  }
  
	
  for (i = 1; i < 1024; i++) {
    for (z = 0; z < 1024; z++) {
      if (gexp[z] == i) {
	glog[i] = z;
	break;
      }
    }
  }
}


int gmult(int a, int b)
{
  int i,j;
  if (a==0 || b == 0) return (0);
  i = glog[a];
  j = glog[b];
  return (gexp[i+j]);
}
		

int ginv (int elt) 
{ 
  return (gexp[1023-glog[elt]]);
}

