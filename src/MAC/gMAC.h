#ifndef _gMAC_h_included_
#define _gMAC_h_included_

#include <platform.h>

#if (CONFIG_SEDK)
#define MAC_INT		SOCLE_INTC_MAC1
#endif


#ifndef SOCLE_AHB0_MAC0
#define SOCLE_AHB0_MAC0		SOCLE_AHB0_MAC
#endif

#ifndef SOCLE_INTC_MAC0
#define SOCLE_INTC_MAC0		SOCLE_INTC_MAC
#endif

#ifndef MAC_REG_BASE
#define MAC_REG_BASE		SOCLE_AHB0_MAC0
#endif

#ifndef MAC_INT
#define MAC_INT		SOCLE_INTC_MAC0
#endif


// MAC Debug Mode
#define SMAC_DEBUG
//#define SMAC_DEVICE_DEBUG
#define SMAC_INTCOUNTER_DEBUG
#define SMAC_ERRORDEBUG
//#define SMAC_MII_DEBUG
//#define SMAC_CSR_DEBUG
//#define SMAC_DATA_DEBUG
#define SMAC_TEST

// Endian Define
#define VERIABLE_BYTE_ORDER	// We defien this if we want change the Byte Order (Test Only)
//#define DES_BYTE_ORDER		F_LITTLE_ENDIAN
//#define DATAB_BYTE_ORDER	F_LITTLE_ENDIAN
//#define DES_BYTE_ORDER		F_BIG_ENDIAN
//#define DATAB_BYTE_ORDER	F_BIG_ENDIAN

// Addess Define
//#define MAC_BASE_ADDR		SOCLE_AHB0_MAC0		//AHB direct connect
//#define MAC_BASE_ADDR           SOCLE_AHB0_ES0  //AHB0
//#define MAC_BASE_ADDR		AHB_SLOT0_BASE  //AHB0

// Include EEPROM or not 
#define MAC_INCLUDE_EEPROM

//////////////////////////////////////////////////////////////////////
// Function	Complier Define
//////////////////////////////////////////////////////////////////////

/* Define DES MODE */
///////////////////////////////////////
//#define MAC_DES_RINGMODE
//#define MAC_DES_CHAINMODE
#define MAC_DES_BOTHMODE

#if ( defined (MAC_DES_RINGMODE) || defined (MAC_DES_BOTHMODE) )
#define MAC_DES_RINGSKIP
#endif

/* Define DATA Buffer MODE */
//////////////////////////////////////
#if ( defined (MAC_DES_RINGMODE) || defined (MAC_DES_BOTHMODE) )
#define MAC_DATA_BUFFER2_USED
#endif

#define MAC_DATA_BUFFER_SIZEVARIABLE

/* Rx Scheme */
#define	MAC_RX_POLLSCHEME

// Default MAC Address
#define DEFAULT_MAC_ADDR	0x534F434C4500	//"SOCLE"-00

#endif  // _gMAC_h_included_
