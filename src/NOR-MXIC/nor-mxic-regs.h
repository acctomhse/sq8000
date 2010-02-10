#include <platform.h>
#include <irqs.h>
#include <type.h>

#ifdef SOCLE_NOR_FLASH_BANK0
#define SOCLE_NOR_BASE0	SOCLE_NOR_FLASH_BANK0;
#else 
#error ¡§we don¡¦t define IP address¡¨ 
#endif

#if defined SOCLE_NOR_FLASH_BANK1
	#define SOCLE_NOR_BASE1	SOCLE_NOR_FLASH_BANK1
#else
	#define SOCLE_NOR_BASE1	-1
#endif

#define FLASH_TOTAL_SEC_NUM       71
#define FLASH_SIZE   0x400000

#define FLASH_SECTOR_SIZE  0x10000

#if defined (CONFIG_PC9002) || (CONFIG_PC9220)
 	#define MODE_BASE (SOCLE_AHB0_SDRSTMC+0x8020)
#endif

#if defined(CONFIG_PDK) || defined(CONFIG_PC7210)
 	#define MODE_BASE (SOCLE_AHB0_DDRMC+0x220)
#endif

/*define errror code*/
#define FLASH_NOERROR				 0
#define FLASH_ERROR				  	 1
#define FLASH_NOTSUPPORT			-1/*the flash memory is not support in this version*/
#define FLASH_INIT_ERROR			-2/*read flash identifier error*/
#define FLASH_VPP_ERROR 			-3/*Vpp range error*/
#define FLASH_CMDSEQ_ERROR		-4/*command sequence error*/
#define FLASH_ERASE_ERROR			-5/*erase error*/
#define FLASH_PROGRAM_ERROR		-6/*flash program error*/
#define FLASH_OVERFLOW				-7/*the size of the data exceeds the size of flash*/
#define FLASH_NOT_INIT				-8/*the flash is not initialized*/
