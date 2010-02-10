#include "platform.h"
/*
 #----------------------------------------------------------------------------
 # Copyright (c) 2000, 2002 Embedded Performance Inc.
 #
 # This software is the property of Embedded Performance, Inc (EPI).
 # Embedded Performance specifically grants the user a license to use and
 # modify this software as required for operation in a product developed by
 # the user, provided this notice is not removed or altered.
 # The user is also granted a limited binary distribution license and may
 # distribute binary versions of this software, as modifed by the user, as
 # part of the users product.
 # In no case may this software be distributed in source form, nor may
 # binary versions be supplied as part of a compilation or assembler toolkit.
 #
 # All other rights are reserved by EPI.
 #
 # EPI MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH REGARD TO THIS
 # SOFTWARE.  IN NO EVENT SHALL EPI BE LIABLE FOR INCIDENTAL OR CONSEQUENTIAL
 # DAMAGES IN CONNECTION WITH OR ARISING FROM THE FURNISHING, PERFORMANCE, OR
 # USE OF THIS SOFTWARE.
 #
 #----------------------------------------------------------------------------
*/

#ifdef	SEMI_HOST_FUNCTION
/* This file must be compiled in MIPS32 mode, not MIPS16 */

#include <errno.h>
#include <time.h>
#include "genlib.h"
#include "epios.h"

#ifndef NULL
#define NULL 0
#endif


/*------------------------------------------------------------------------
**  The HIF mechanism requires execution of a hard-coded breakpoint.
**  Since not all processors use the same breakpoint opcode, the right
**  one must be declared here.
**
**  If you know the processor type you will be using, you can simply
**  select the appropriate value for EMUL_SWBP, as below.
**
**  #define EMUL_SWBP	".word 0x0000000E"	// RC323xx processors
**	#define EMUL_SWBP	".word 0x7000003F"	// Other EJTAG processors
**
**  Since EPI sample programs must run on a variety of processors, a more
**  complex macro is normally used to select the value at run time.  The
**  processor's PRID register value is used to determine the correct breakpoint
**  opcode.
*/
#define EMUL_SWBP   "mfc0   $8,$15; \
                     li $9,0x2620; beq $8,$9,1f; \
                     li $9,0x1800; beq $8,$9,1f; \
                     li $9, 0x1900;beq $8,$9,1f; \
                     .word 0x7000003F; b 2f; \
                     1: .word 0x0000000E; \
                     2: nop "
/*------------------------------------------------------------------------*/


/*
**  EPIOS Function Code (_hif_block.fcode) is a 32-bit word encoded as follows:
**
**      CCCCPPPP
**
**	Where CCCC is the command code (0=EPIOS Call, 1=Debug Print, FFFF=Illegal Trap)
**  and PPPP is the parameter of CCCC, if any, else 0.
*/

#define	EPI_exit	0x01	/* 1 */

#define	EPI_open	0x11	/* 17 */
#define	EPI_close	0x12	/* 18 */
#define	EPI_read	0x13	/* 19 */
#define	EPI_write	0x14	/* 20 */
#define EPI_lseek	0x15	/* 21 */
#define	EPI_remove	0x16	/* 22 */
#define	EPI_rename	0x17	/* 23 */
#define EPI_tmpnam	0x21	/* 33 */
#define EPI_time	0x31	/* 49 */
#define EPI_getenv	0x41	/* 65 */
#define EPI_gettz	0x43	/* 67 */

#define EPI_dbgprint        0x10000000
#define EPI_dbgprint_disabled        0 /* Debug print feature is disabled */
#define EPI_dbgprint_concurrent      1 /* Check for debug print while running */
#define EPI_dbgprint_halt            2 /* Only check for debug print after stop */


#define DBGPR_MSG_LENGTH 255


typedef unsigned int HIF_WORD ;

struct hif_mailbox_struct
{
	/* This is the HIF mailbox structure used to pass the HIF */
	/* function code and parameters to the debugger via MAJIC, */
	/* and to pass results back from the debugger via MAJIC. */
	HIF_WORD semap;
	HIF_WORD fcode;
	HIF_WORD parm1;
	HIF_WORD parm2;
	HIF_WORD parm3;
	HIF_WORD parm4;
	HIF_WORD debug_msg_length;
	unsigned char debug_msg[DBGPR_MSG_LENGTH+1];
};

/*
**  The HIF interface structure tells MAJIC where the HIF mailbox is
**  located.  The HIF interface structure is hard coded at the address
**  below.  If MAJIC finds the HIF_SIGNATURE value in the second word,
**  then it will use the address value in the first word as the HIF
**  mailbox pointer.
*/

#define ADDR_HIF_INTERFACE	((struct hif_interface_struct *) 0xA0000078)
#define HIF_SIGNATURE		0x6A59EEB3

struct hif_interface_struct
{
	/* This is the HIF interface structure used to inform */
	/* MAJIC of the location of the HIF mailbox. */
	struct hif_mailbox_struct *ptr_hif_block;
	HIF_WORD signature;
};


static HIF_WORD dbgprint_mode = EPI_dbgprint_disabled;

struct hif_mailbox_struct	hif_mailbox;
static struct hif_interface_struct *ptr_hif_interface = ADDR_HIF_INTERFACE;

#define MEM_LIMIT 0x800000

extern unsigned int _ebss;
extern unsigned int _sys_heap_start;
extern unsigned int _sys_heap_end;
extern unsigned int _sys_heap_cur;


unsigned int HIF_errno;



/*-------------------------------------------------------------------
**
** _hif_init:	Initializes the EPIOS/HIF call interface to MAJIC.
**              This MUST be called prior to any other function
**				in this module.
**-------------------------------------------------------------------
*/

void _hif_init(void)
{
	HIF_WORD p_mbox;
	p_mbox = (HIF_WORD) &hif_mailbox;
	p_mbox |= 0xA0000000;

	ptr_hif_interface->ptr_hif_block = (struct hif_mailbox_struct *) p_mbox;
	ptr_hif_interface->signature = HIF_SIGNATURE;
}

/*-------------------------------------------------------------------
**
** _hif_call:	performs the actual EPIOS/HIF call by setting
**              up hif_mailbox via kseg1
**-------------------------------------------------------------------
*/

struct hif_mailbox_struct * _hif_call(
	HIF_WORD semap,
	HIF_WORD fcode,
	HIF_WORD parm1,
	HIF_WORD parm2,
	HIF_WORD parm3,
	HIF_WORD parm4	)
{
	struct hif_mailbox_struct *p_mbox;

	p_mbox = ptr_hif_interface->ptr_hif_block;

	p_mbox->fcode = fcode;
	p_mbox->parm1 = parm1;
	p_mbox->parm2 = parm2;
	p_mbox->parm3 = parm3;
	p_mbox->parm4 = parm4;
	p_mbox->semap = semap;

	/* Make the call via a hardcoded breakpoint */
	asm( EMUL_SWBP );
	asm( "nop" );
	asm( "nop" );

	return p_mbox;
}



/*-------------------------------------------------------------------
**
** _sysalloc: allocate count bytes from the operating system maintained
**           heap.  This is a very simple-minded heap.  Currently, the
**           higher level C library never calls sys_free() and thus
**           a real heap is not maintained.
**
**   syntax: void *_sysalloc( size_t count )
**
**-------------------------------------------------------------------
*/

void *_sysalloc ( int count )
{
	unsigned int new_cur;
	unsigned int temp = _sys_heap_cur;

	new_cur = _sys_heap_cur + count;
	if ( _sys_heap_cur > _sys_heap_end )
	{
		HIF_errno = 1;
		return NULL;
	}
	_sys_heap_cur = new_cur;
	return (void *)temp;
}




/*-------------------------------------------------------------------
**
** _sysfree: free a previously allocated region of memory
**
**  syntax: void _sysfree( void * )
**
**-------------------------------------------------------------------
*/

void _sysfree( void *mem )
{
	// sysfree does not need to be supported since C libraries
	// do not actually attemp to free memory.  Instead they
	// maintain there own free list.
	return;
}

int _getpsiz( void )
{
	return 	4096;  // somewhat arbitrary
}

int _open( char *path, int oflag, int mode )
{
	struct hif_mailbox_struct *p_mbox;

	p_mbox = _hif_call( 1,			 /* semap */
			EPI_open,
			(HIF_WORD) path,
			(HIF_WORD) oflag,
			(HIF_WORD) mode,
			0 );

	if ( p_mbox->semap != 0 )
	{
		HIF_errno = 1;
		return -1;
	}
	return p_mbox->parm1;
}

int _close( int fd )
{
	struct hif_mailbox_struct *p_mbox;

	p_mbox = _hif_call( 1,			 /* semap */
			EPI_close,
			(HIF_WORD)fd,
			0,
			0,
			0 );

	if ( p_mbox->semap != 0 )
	{
		HIF_errno = 1;
		return -1;
	}
	return p_mbox->parm1;
}

int _read( int fd, void *buf, int count )
{
	struct hif_mailbox_struct *p_mbox;

	p_mbox = _hif_call( 1,			 /* semap */
			EPI_read,
			(HIF_WORD) fd,
			(HIF_WORD) buf,
			(HIF_WORD) count,
			0 );

	if ( p_mbox->semap != 0 )
	{
		HIF_errno = 1;
		return -1;
	}
	return p_mbox->parm1;
}

int _write( int fd, void *buf, int count )
{
	struct hif_mailbox_struct *p_mbox;

	p_mbox = _hif_call( 1,			 /* semap */
			EPI_write,
			(HIF_WORD) fd,
			(HIF_WORD) buf,
			(HIF_WORD) count,
			0 );

	if ( p_mbox->semap != 0 )
	{
		HIF_errno = 1;
		return -1;
	}
	return p_mbox->parm1;
}

void _exit( int code )
{
	while ( 1 )
	{
		_hif_call( 1,			 /* semap */
				EPI_exit,
				(HIF_WORD) code,
				0,
				0,
				0 );

		/* _hif_call should never return in this case */
	}
}

int _lseek( int fd, unsigned int relative_pos, int whence )
{
	struct hif_mailbox_struct *p_mbox;

	p_mbox = _hif_call( 1,			 /* semap */
			EPI_lseek,
			(HIF_WORD) fd,
			(HIF_WORD) relative_pos,
			(HIF_WORD) whence,
			0 );

	if ( p_mbox->semap != 0 )
	{
		HIF_errno = 1;
		return -1;
	}
	return p_mbox->parm1;
}

int remove( const char *name )
{
	struct hif_mailbox_struct *p_mbox;

	p_mbox = _hif_call( 1,			 /* semap */
			EPI_remove,
			(HIF_WORD) name,
			0,
			0,
			0 );

	if ( p_mbox->semap != 0 )
		HIF_errno = 1;
	return (int) p_mbox->parm1;
}

int rename( const char *oldname, const char *newname )
{
	struct hif_mailbox_struct *p_mbox;

	p_mbox = _hif_call( 1,			 /* semap */
			EPI_rename,
			(HIF_WORD) oldname,
			(HIF_WORD) newname,
			0,
			0 );

	if ( p_mbox->semap != 0 )
		HIF_errno = 1;
	return (int) p_mbox->parm1;
}

char *_tmpnam( char *buf )
{
	struct hif_mailbox_struct *p_mbox;

	p_mbox = _hif_call( 1,			 /* semap */
			EPI_tmpnam,
			(HIF_WORD) buf,
			0,
			0,
			0 );

	if ( p_mbox->semap != 0 )
	{
		HIF_errno = 1;
		return NULL;
	}
	return (char *) p_mbox->parm1;
}

char *getenv( const char *buf )
{
	struct hif_mailbox_struct *p_mbox;

	p_mbox = _hif_call( 1,			 /* semap */
			EPI_getenv,
			(HIF_WORD) buf,
			0,
			0,
			0 );

	if ( p_mbox->semap != 0 )
	{
		HIF_errno = 1;
		return NULL;
	}
	return (char *) p_mbox->parm1;
}

long _gettz( void )
{
	struct hif_mailbox_struct *p_mbox;

	p_mbox = _hif_call( 1,			 /* semap */
			EPI_gettz,
			0,
			0,
			0,
			0 );

	if ( p_mbox->semap != 0 )
	{
		HIF_errno = 1;
		return -1;
	}
	return p_mbox->parm1;
}

time_t time( time_t *timer )
{
	struct hif_mailbox_struct *p_mbox;

	p_mbox = _hif_call( 1,			 /* semap */
			EPI_time,
			0,
			0,
			0,
			0 );

	if ( p_mbox->semap != 0 )
	{
		HIF_errno = 1;
		return 0;
	}
	if ( timer != NULL )
		*timer = (time_t)p_mbox->parm1;

	return (time_t) p_mbox->parm1;
}




/* Override the libraries optmized strcpy function.  Some
** processors do not implement the unaligned load which our
** libraries optimized version use.  Avoid the issue by overriding
** the function with this one.
*/
/*
char *strcpy( char *strDestination, const char *strSource )
{
	char *dest = strDestination;
	if ( strSource == NULL || strDestination == NULL )
		return strDestination;
	while ( *strSource != '\0' )
		*strDestination++ = *strSource++;
	*strDestination = '\0';
	return dest;
}
*/

/*
**  Concurrent Debug Print initialization function.  MUST be called AFTER
**  _hif_init(), but BEFORE _debug_print().  Returns 1 if successfully
**  initialized and ready for use, 0 on failure.
*/

int _debug_print_init(void)
{
	int ret_ok;
	struct hif_mailbox_struct *p_mbox;

	p_mbox = _hif_call( (HIF_WORD) EPI_dbgprint,	/* semap */
			0,
			0,
			0,
			0,
			0 );

	if ( p_mbox->semap != 0 )
	{
		/* Failure--debug print disabled */
		dbgprint_mode = EPI_dbgprint_disabled;
		p_mbox->semap = 0;
		ret_ok = 0;
	}
	else
	{
		/* Success--save the requested mode */
		dbgprint_mode = p_mbox->parm1;

		if (	(dbgprint_mode != EPI_dbgprint_concurrent)
			&&	(dbgprint_mode != EPI_dbgprint_halt) )
		{
			dbgprint_mode = EPI_dbgprint_disabled;
			ret_ok = 0;
		}
		else
			ret_ok = 1;
	}

	return ret_ok;
}

/*
**  Concurrent Debug Print output function, transfers a \0
**  terminated ASCII message string to the debugger via MAJIC
**  using the method selected in _debug_print_init().  The
**  message must be less than 256 bytes (inc \0).  Returns
**  1 on success, 0 on failure.
*/
int _debug_print(char *message)
{
	int ret_ok;
	struct hif_mailbox_struct *p_mbox = ptr_hif_interface->ptr_hif_block;

	if ( _debug_print_ready() )		/* Channel is ready for use */
	{
		int  length;
		char *ps = message;				/* source pointer */
		char *pd = p_mbox->debug_msg;	/* dest pointer */

		/* copy message and count length */
		for (length = 0; (*ps) && (length < DBGPR_MSG_LENGTH); ++length)
			*(pd++) = *(ps++);

		*pd = '\0';
		++length;		/* must include '\0' */

		/* send the message */
		p_mbox->debug_msg_length = length;

		if (dbgprint_mode == EPI_dbgprint_halt)
		{
			/* This alerts MAJIC to pause execution and check hif_mailbox */
			_hif_call((HIF_WORD) EPI_dbgprint | dbgprint_mode, /* semap */
					0,
					0,
					0,
					0,
					0 );
		}
		/* else MAJIC constantly polls via DMA and will notice hif_mailbox */

		ret_ok = 1;			/* Message was posted */
	}
	else
	{
		ret_ok = 0;			/* Mailbox is full (not yet emptied by MAJIC) */
	}

	return ret_ok;
}

int _debug_print_ready(void)
{
	int ret_ok;
	struct hif_mailbox_struct *p_mbox = ptr_hif_interface->ptr_hif_block;

	if (	(dbgprint_mode == EPI_dbgprint_halt)
		||	(dbgprint_mode == EPI_dbgprint_concurrent) )
	{
		if (p_mbox->debug_msg_length == 0)
			ret_ok = 1;			/* Channel is ready for use */
		else
			ret_ok = 0;			/* Mailbox is full (not yet emptied by MAJIC) */
	}
	else
	{
		ret_ok = 0;				/* Channel was not properly initialized */
	}

	return ret_ok;
}


/*------------------------------------------------------------------------
**  If the debug channel was properly initialized, calls _debug_print()
**  to post the message, then waits until the message has been picked up
**  before returning.
**------------------------------------------------------------------------
*/

void _debug_print_wait(char *message)
{
	if (	(dbgprint_mode == EPI_dbgprint_halt)
		||	(dbgprint_mode == EPI_dbgprint_concurrent) )
	{
		if (_debug_print(message))
		{
			while ( !_debug_print_ready() )
			{
				/* This space intentionally left blank */
			}
		}
	}
}

/*------------------------------------------------------------------------
**	Sherlock Add print out Hex value
**------------------------------------------------------------------------
*/
void _debug_print_cHex(char *data, int length)
{
char 				buff[6];
char				Txbuff[128+10+1];
char				*Txbuff_pt;
unsigned char		cdata;

	Txbuff_pt = Txbuff;
	*Txbuff_pt = 0 ;
	while(length)
	{
		cdata=(unsigned char)(*data);
		normal_itoa ((int)(cdata), buff, 16, 2, 1);
		Txbuff_pt = strcat(Txbuff_pt, buff);
		Txbuff_pt = strcat(Txbuff_pt, " ");
		data++;
		length--;
		if( (strlen(Txbuff_pt) > 128) || (length == 0) )
		{
			HIF_write( 1, Txbuff_pt, strlen(Txbuff_pt) );
			Txbuff_pt = Txbuff;
			*Txbuff_pt = 0 ;
		}
	}
}

void _debug_print_sHex(short int *data, int length)
{
char 				buff[8];
char				Txbuff[128+10+1];
char				*Txbuff_pt;
unsigned short int	sdata;

	Txbuff_pt = Txbuff;
	*Txbuff_pt = 0 ;
	while(length)
	{
		sdata=(unsigned short int)(*data);
		normal_itoa ((int)(sdata), buff, 16, 4, 1);
		Txbuff_pt = strcat(Txbuff_pt, buff);
		Txbuff_pt = strcat(Txbuff_pt, " ");
		data++;
		length--;
		if( (strlen(Txbuff_pt) > 128) || (length == 0) )
		{
			HIF_write( 1, Txbuff_pt, strlen(Txbuff_pt) );
			Txbuff_pt = Txbuff;
			*Txbuff_pt = 0 ;
		}
	}
}

void _debug_print_lHex(long int *data, int length)
{
char 				buff[12];
char				Txbuff[128+10+1];
char				*Txbuff_pt;
int	 				idata;

	Txbuff_pt = Txbuff;
	*Txbuff_pt = 0 ;
	while(length)
	{
		idata=(int)(*data);
		normal_itoa ((int)(idata), buff, 16 ,8, 1);
		Txbuff_pt = strcat(Txbuff_pt, buff);
		Txbuff_pt = strcat(Txbuff_pt, " ");
		data++;
		length--;
		if( (strlen(Txbuff_pt) > 128) || (length == 0) )
		{
			HIF_write( 1, Txbuff_pt, strlen(Txbuff_pt) );
			Txbuff_pt = Txbuff;
			*Txbuff_pt = 0 ;
		}
	}
}

void	normal_itoa(int data, char *buff, unsigned int nRadix,unsigned int nor, int prefix)
{
char	buff_temp[24];
char *	temp_Pt;
int 	size;

	temp_Pt=buff_temp;
	itoa (data, temp_Pt, nRadix);

	size=strlen(buff_temp);
	if((*temp_Pt)=='-')
	{
		*buff=*temp_Pt;
		buff++;
		temp_Pt++;
		size--;		
	}

	if(prefix)
	{
		if(nRadix==16)	{*buff='0';	buff++;	*buff='x';	buff++;}
		if(nRadix==8)	{*buff='0';	buff++;	*buff='o';	buff++;}
		if(nRadix==2)	{*buff='0';	buff++;	*buff='b';	buff++;}
	}	
	
	while(nor>size)
	{
		*buff='0';	buff++;	nor--;
	}
	while(size)
	{
		*buff=*temp_Pt;	buff++;	temp_Pt++; size--;
	}
	*buff=0;	//END
}

#endif //#ifdef	SEMI_HOST_FUNCTION






#ifdef SEMI_HOST_FUNCTION_ARM

#include "genlib.h"

void HIF_debug_print_wait(char* p)
{
		printf("%s",p);
	
}

#if 0
void HIF_write( void *fop, char* p, int n )
{
	while(n--)
		printf("%c",*p++);
	
}

void HIF_read( void *fop, char* p, int n )
{
#if 0
	while(n--)
		*p++=getchar();
#else
		*p++=getchar();
#endif
}
#endif

	
void HIF_debug_print_lHex(int *p, int n )
{
	printf("0x%08x",*p);
}

	
void HIF_debug_print_cHex(char* p, int n )
{
	printf("0x%02x",*p);
}

	
void HIF_debug_print_sHex(int* p, int n )
{
	printf("0x%04x",*p);
}

#define r	0x0
#define b	0x1
#define w	0x4
#define plus	0x2
#define a	0x8
#define O_RDONLY		(r)
#define O_CREATorO_WRONLY	((w)+(plus)+(b))
int HIF_open( char *path, int oflag, int mode )
{
	int c;
	int tmp[256/4];
	int setblock[3];


	strcpy(tmp,path);
	setblock[0]=(int)tmp;
//	if(oflag==(O_CREATorO_WRONLY))
//		setblock[1]=7;
//	else
		setblock[1]=oflag;
	setblock[2]=strlen(tmp);

	__asm__ __volatile__ (
		"mov r0,#1 \n"
		"mov r1,%[input]\n"
		"swi #0x123456\n"
		"mov %[output],r0"
		:[output] "=r" (c)
		:[input] "r" (setblock));
	return c;
}
int HIF_close( int fd )
{
	int c;
	__asm__ __volatile__ (
		"mov r0,#2 \n"
		"mov r1,%[input]\n"
		"swi #0x123456\n"
		"mov %[output],r0"
		:[output] "=r" (c)
		:[input] "r" (&fd));
	return c;
}

int HIF_write( int fd, void *buf, int count )
{
	int c;
	int setblock[3];


	setblock[0]=fd;
	setblock[1]=buf;
	setblock[2]=count;

	__asm__ __volatile__ (
		"mov r0,#5 \n"
		"mov r1,%[input]\n"
		"swi #0x123456\n"
		"mov %[output],r0"
		:[output] "=r" (c)
		:[input] "r" (setblock));

	if (c == 0)
		return count;
	else
		return c;
//todo: error handling
}

int HIF_flen( int fd )
{
	int c;
	__asm__ __volatile__ (
		"mov r0,#12 \n"
		"mov r1,%[input]\n"
		"swi #0x123456\n"
		"mov %[output],r0"
		:[output] "=r" (c)
		:[input] "r" (&fd));
	return c;
}
int HIF_read( int fd, void *buf, int count )
{
	int c;
	int setblock[3];
	int flen;
#if 1
	int *p;

	flen=HIF_flen(fd);
	if(count>flen)
		count=flen;
//todo:other error handling

	p=setblock;
	*p=fd;
	*(p+1)=buf;
//	*(p+2)=flen;
	*(p+2)=count;
#else
	setblock[0]=fd;
	setblock[1]=buf;
	setblock[2]=count;
#endif

	__asm__ __volatile__ (
		"mov r0,#6 \n"
		"mov r1,%[input]\n"
		"swi #0x123456\n"
		"mov %[output],r0"
		:[output] "=r" (c)
		:[input] "r" (setblock));
//	return c;
//todo:other error handling
//	return flen;
	return count;
}

int HIF_lseek( int fd, unsigned int relative_pos, int whence )
{
	int c;
	int setblock[2];


	setblock[0]=fd;
	setblock[1]=relative_pos;

	__asm__ __volatile__ (
		"mov r0,#10 \n"
		"mov r1,%[input]\n"
		"swi #0x123456\n"
		"mov %[output],r0"
		:[output] "=r" (c)
		:[input] "r" (setblock));
	return c;
}

int HIF_remove( const char *name )
{
	int c;
	int setblock[2];
	int tmp[256/4];


	strcpy(tmp,name);
	setblock[0]=tmp;
	setblock[1]=strlen(tmp);
	*((char*)tmp+setblock[1])=0; //NULL terminated

	__asm__ __volatile__ (
		"mov r0,#14 \n"
		"mov r1,%[input]\n"
		"swi #0x123456\n"
		"mov %[output],r0"
		:[output] "=r" (c)
		:[input] "r" (setblock));
	return c;
}
#endif //#ifdef SEMI_HOST_FUNCTION_ARM
