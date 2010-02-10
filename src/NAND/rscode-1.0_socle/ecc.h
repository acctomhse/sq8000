#define MAXNPAR 12
extern int npar;		//6 for 8 bytes , 12 for 15 bytes 

#ifndef TRUE
#define TRUE 1
#endif

#define FALSE 0
typedef unsigned long BIT32;
typedef unsigned short BIT16;

#define MAXDEG (MAXNPAR*2)

extern unsigned short pBytes[MAXDEG];
extern unsigned short synBytes[MAXDEG];
extern int DEBUG;

void initialize_ecc (void);
int check_syndrome (void);
void decode_data (unsigned short data[], int nbytes);
void encode_data (unsigned char msg[], int nbytes, unsigned short dst[]);

BIT16 crc_ccitt(unsigned char *msg, int len);

extern int gexp[];
extern int glog[];

void init_galois_tables (void);
int ginv(int elt); 
int gmult(int a, int b);


int correct_errors_erasures (unsigned short codeword[], int csize);
int correct_errors_erasures_char (unsigned char codeword[], int csize);
extern int correct_errors_hw_ecc(unsigned char *codeword, int csize);

void add_polys(unsigned short dst[], unsigned short src[]) ;
void scale_poly(int k, unsigned short poly[]);
void mult_polys(unsigned short dst[], unsigned short p1[], unsigned short p2[]);

void copy_poly(unsigned short dst[], unsigned short src[]);
void zero_poly(unsigned short poly[]);
void print_syndrome (void);
void print_parity(void);
