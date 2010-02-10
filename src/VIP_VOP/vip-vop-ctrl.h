#ifndef _MPEG4CTRL_H_
#define _MPEG4CTRL_H_

#include "mpeg4enc.h"
#include "mpeg4dec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ldk	//LDK setting
//#define pc9220	//pc9220

#define MM_SDRAM_BANK0             	0xA0000000
#define MM_SDRAM_BANK1          	0xA8000000
#define MM_SDRAM_BANK2          	0xB0000000
#define MM_SDRAM_BANK3          	0xB8000000


#define MM_SDRAM_BANK0_TOP		   	0xA7FFFFFF
#define MM_SDRAM_BANK1_TOP			0xAFFFFFFF
#define MM_SDRAM_BANK2_TOP			0xB7FFFFFF
#define MM_SDRAM_BANK3_TOP			0x7FFFFFFF
#define MM_SDRAM_TOTAL_TOP			MM_SDRAM_BANK3_TOP


// Memory Patition For Test Program
/* DDR Part Memory
  --------------------------------------	Top 
 |	Memory Chunk for malloc				|
  --------------------------------------   
 |	Memory Buffer for MPEG Test			| 
  --------------------------------------   
 |	Memory Buffer for MPEG 				|
  -------------------------------------- 
 |	Memory Buffer (for Buffer)   	    |
  --------------------------------------   
 |	Memory Buffer (for Test)   	    	|
  --------------------------------------   
 |										|
 |					:					|
 |										|
  --------------------------------------
*/
#define MM_CHUNK_SIZE				0x01000000
#define MM_CHUNK_START				((MM_SDRAM_BANK0_TOP+1)-MM_CHUNK_SIZE)


/* MPEG4_MEM1_TEST
  -------------------------------------------------- 
 |	Memory for MPEG Test ENCODE	Bit Stream input	| 
  -------------------------------------------------- 
 |	Memory for MPEG Test ENCODE	Bit Stream output	|  
  -------------------------------------------------- 
 |	Memory for MPEG Test DECODE	Bit Stream input	| 
  -------------------------------------------------- 
 |	Memory for MPEG Test DECODE	Bit Stream output	|  
  --------------------------------------------------
 
*/
#define	MPEG4_ENC_TIN_SIZE			0x04000000
//#define	MPEG4_ENC_TOUT_SIZE			0x00400000	
#define	MPEG4_ENC_TOUT_SIZE			0x00400000	
#define	MPEG4_DEC_TIN_SIZE			0x00400000
#define	MPEG4_DEC_TOUT_SIZE			0x04000000
#define	MPEG4_ENC_TCTRL_SIZE		0x00010000
#define MM_MPEG4_TEST_SIZE			(MPEG4_ENC_TIN_SIZE+ MPEG4_ENC_TOUT_SIZE+ MPEG4_DEC_TIN_SIZE+ MPEG4_DEC_TOUT_SIZE+ MPEG4_ENC_TCTRL_SIZE)

#define MM_MPEG4_TEST_START			(MM_CHUNK_START)
#define	MPEG4_ENC_TIN_START			(MM_MPEG4_TEST_START-MPEG4_ENC_TIN_SIZE)
#define	MPEG4_ENC_TOUT_START		(MPEG4_ENC_TIN_START-MPEG4_ENC_TOUT_SIZE)
#define	MPEG4_DEC_TIN_START			(MPEG4_ENC_TOUT_START-MPEG4_DEC_TIN_SIZE)
#define	MPEG4_DEC_TOUT_START		(MPEG4_DEC_TIN_START-MPEG4_DEC_TOUT_SIZE)
#define	MPEG4_ENC_TCTRL_START		(MPEG4_DEC_TOUT_START-MPEG4_ENC_TCTRL_SIZE)
#define MPEG4_ENC_TEXT_NEXT_START	(MPEG4_ENC_TCTRL_START)

#define MM_MPEG4_SIZE				0x01000000
#define MM_MPEG4_START				(MPEG4_ENC_TEXT_NEXT_START-MM_MPEG4_SIZE)
#define MM_BUFFER_SIZE				0x00010000
#define MM_BUFFER_START				(MM_MPEG4_START-MM_BUFFER_SIZE)
#define MM_TESTBUFFER_SIZE			0x00010000
#define MM_TESTBUFFER_START			(MM_BUFFER_START-MM_TESTBUFFER_SIZE)
#define MM_FREE_TOP					(MM_TESTBUFFER_START-1)

/* SRAM Part Memory
  --------------------------------------	Top 
 |	Memory Buffer for MPEG				|
  --------------------------------------
*/
#define	MM_DEDICATE_MPEG4_SIZE		(MM_SDRAM_BANK1_TOP-MM_SDRAM_BANK1+1)
#define	MM_DEDICATE_MPEG4_START		(MM_SDRAM_BANK1)
//Just Use one memory for non real time test
//#define	MM_DEDICATE_MPEG4_START		((MM_FREE_TOP+1)-MM_DEDICATE_MPEG4_SIZE)

// AHB 2 device base address define
#define AHB2_BASE					0xB8000000
#define ARBITER2_BASE              	(AHB2_BASE)
#define MPEG4_ENCODER_BASE			(AHB2_BASE + 0x00020000)
#define MPEG4_DECODER_BASE			(AHB2_BASE + 0x00040000)
#ifdef ldk
#define MPEG4_VIDEOIN_BASE			(AHB2_BASE + 0x00060000)
#define MPEG4_VIDEOOUT_BASE			(AHB2_BASE + 0x00080000)
#else
//danny testing --s  //pc-9220
#define MPEG4_VIDEOIN_BASE			(0x18120000)
#define MPEG4_VIDEOOUT_BASE		(0x18100000)
//danny testing --e 
#endif

//danny testing --s
//pc9220 
extern	int VIP_VOP_Testing(int autotest);
//danny testing --e
extern	int		MPEG4Testing(int autotest);
extern	void	Display_MPEG4ParaMenu(void);
extern	void	Display_MPEG4Menu(void);
extern	int		Paser_MPEG4ParaSetting(char cmd);
extern	void	MPEG4testmem_init(void);
extern	void 	MPEG4_testshowmemmap(void); 
extern	int		MPEG4testctrl_init(void);
extern	int		MPEG4testdev_init(void);
extern	int		MPEG4MemCopyBack_init(void);
extern	void	EncMemCopyBack(mpeg4enc_dev * pDrvCtrl, char * data_pt, int size);
extern	void	EncEndofVOPS(void);
extern	int		DecMemCopyBack(mpeg4dec_dev * pDrvCtrl, int framesize, char * y_pt, char * cb_pt, char * cr_pt);
extern	int		MPEG4MemCopyBack_end(void);
extern	void 	filestrvar(char * varout, char * filename);
extern	void 	mpeg4InitColorMem(int patterntype, int framesize);
extern  void    ReFillmpeg4DecWrite(void *para1, int Empty);
extern  char *  Vip_FileOutAlloc(int size);

#ifdef __cplusplus
}
#endif	

#define O_RDONLY             00
#define O_WRONLY      01
#define O_CREAT            0100 /* not fcntl */

#endif // _MPEG4CTRL_H_
