/*
 #----------------------------------------------------------------------------
 # Copyright (c) 2000 Embedded Performance Inc.
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

/*
**  Function prototypes for the EPIOS Host Interface Function (HIF) calls.
**  Refer to the function header comments in epios.c for details on these
**  functions.
**  
**  The EPI run-time library uses these calls, so the EPI sample programs
**  can use standard C calls such as printf(), fopen(), fread()...
**  When using third party compilation tools, you can either adapt their
**  run-time library to use the functions below, or your code can call them
**  directly.  If you choose the latter, we recommend using the macros that
**  are #defined below so that you can easily remove these hooks when MAJIC
**  is not in use.
**
**  The  _hif_init()  function MUST be called BEFORE any other function
**  in the epios module to initializes the HIF mailbox used to communicate
**  with MAJIC.  It should be called as early as possible in  your boot
**  code,certainly before you need to begin servicing critical interrupts.
**  The EPI sample programs call _hif_init() from the boot.s module, but
**  you can call it from main(), or whatever high level entry point is
**  appropriate.
*/
#ifndef _epios_h_included_
#define _epios_h_included_

#ifndef _TIME_H_
#include <time.h>
#endif

#include "global.h"

#ifdef	SEMI_HOST_FUNCTION

extern void _hif_init(void);

extern void *_sysalloc ( int count );
extern void _sysfree( void *mem );
extern int _getpsiz( void );
extern int _open( char *path, int oflag, int mode );
extern int _close( int fd );
extern int _read( int fd, void *buf, int count );
extern int _write( int fd, void *buf, int count );
extern void _exit( int code );
extern int _lseek( int fd, unsigned int relative_pos, int whence );
extern int remove( const char *name );
extern int rename( const char *oldname, const char *newname );
extern char *_tmpnam( char *buf );
extern char *getenv( const char *buf );
extern long _gettz( void );
extern time_t time( time_t *timer );

#define HIF_init     _hif_init

#define HIF_sysalloc _sysalloc
#define HIF_sysfree  _sysfree 
#define HIF_getpsiz  _getpsiz 
#define HIF_open     _open   
#define HIF_close    _close  
#define HIF_read     _read   
#define HIF_write    _write  
#define HIF_exit     _exit   
#define HIF_lseek    _lseek  
#define HIF_remove   remove  
#define HIF_rename   rename  
#define HIF_tmpnam   _tmpnam     
#define HIF_getenv   getenv  
#define HIF_gettz    _gettz  
#define HIF_time     time    


/*
**  These control how the EPIOS Debug Print feature communicates with
**  MAJIC.  The appropriate value for your system should be passed into
**  _debug_print_init().  Refer to the function description in epios.c
**  for details.
**
**  Note:  The  _debug_print_init()  function MUST be called BEFORE any
**         other  _debug_print calls.  Furthermore,  _debug_print()  and
**         _debug_print_wait() take no action if  _debug_print_init()
**         returned non-zero.
*/

extern int  _debug_print_init(void);
extern int  _debug_print(char *message);
extern int  _debug_print_ready(void);
extern void _debug_print_wait(char *message);

//Sherlock Add
extern void _debug_print_cHex(char *data, int length);
extern void _debug_print_sHex(short int *data, int length);
extern void _debug_print_lHex(long int *data, int length);
extern void	normal_itoa(int data, char *buff, unsigned int nRadix,unsigned int nor, int prefix);

#define HIF_debug_print_init 		_debug_print_init
#define HIF_debug_print     		_debug_print
#define HIF_debug_print_ready  		_debug_ready
#define HIF_debug_print_wait		_debug_print_wait
#define HIF_debug_print_cHex(x,y)   _debug_print_cHex((char *)x, (int)y);     
#define HIF_debug_print_sHex(x,y)	_debug_print_sHex((short int *)x, (int)y);
#define HIF_debug_print_lHex(x,y) 	_debug_print_lHex((long int *)x, (int)y);

#else	//#ifdef	SEMI_HOST_FUNCTION
#define HIF_init()     				 
                                    
#define HIF_sysalloc(x)             
#define HIF_sysfree(x)              
#define HIF_getpsiz()               
#define HIF_open(x,y,z)             
#define HIF_close(x)                
#define HIF_read(x,y,z)             
#define HIF_write(x,y,z)            
#define HIF_exit(x)                 
#define HIF_lseek(x,y,z)            
#define HIF_remove(x)               
#define HIF_rename(x,y)             
#define HIF_tmpnam(x)               
#define HIF_getenv(x)               
#define HIF_gettz()                 
#define HIF_time(x)                 

#define	normal_itoa(x,y,z,w,v)
                                    
#define HIF_debug_print_init()      
#define HIF_debug_print(x)          
#define HIF_debug_print_ready()     
#define HIF_debug_print_wait(x)     
#define HIF_debug_print_cHex(x,y)   
#define HIF_debug_print_sHex(x,y)   
#define HIF_debug_print_lHex(x,y)   

#endif  //#ifdef	SEMI_HOST_FUNCTION

#endif
