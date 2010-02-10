#ifndef _platform_H_
#define _platform_H_

#include <io.h>
#include <type.h>
#include <interrupt.h>

#define INTC_MAX_IRQ            	32			// interrupt source number
#define INTC_MAX_FIQ				32			// interrupt source number
#define MAX_SYSTEM_IRQ				32			// max. system interrupt source number

#define INTC_CONNECT_SYS_INT		SYS_INT_7	// interrupt controller connect which MIPS IP


#define EXT_OSC					33			// Ext OSC (MHz)
#define SOCLE_UART_EXT_CLK			24000000


#ifdef CONFIG_ARM9
#define MAPPING_MASK				0x3FFFFFFF
#define CACHE_BASE					0x80000000	// Cache Kenel Mode
#endif

#if defined(CONFIG_ARM7) || defined(CONFIG_ARM7_HI)
#define MAPPING_MASK				0x3FFFFFFF
#define CACHE_BASE					0x40000000	// Cache Kenel Mode
#endif

#define KSEG_TO_PHY(x)				( ((int) (x)) & MAPPING_MASK )
#define PHY_TO_CACHE(x)				( ( ((int) (x)) & MAPPING_MASK ) | CACHE_BASE )


// memory mapped address
#define	SOCLE_MEMORY_ADDR_START			0x40000000
#define	SOCLE_MEMORY_ADDR_SIZE			0x20000000

#define	SOCLE_MEMORY_BANKS	5

#define SOCLE_MM_DDR_SDR_BANK0			0x00000000
#define SOCLE_MM_DDR_SDR_BANK1			0x40000000	
#define SOCLE_MM_DDR_SDR_BANK2			0x50000000	
#define SOCLE_MM_DDR_SDR_BANK3			0xF0000000
#define SOCLE_MM_DDR_SDR_BANK4			0x60000000


#define	SOCLE_NORFLASH_BANKS	1
#define SOCLE_NOR_FLASH_BANK0           0x10000000	

#define SOCLE_CACHE_CTRL_BASE			0xEFFF0000

#define SOCLE_BUS0_PCI	              0x1B600000


#ifdef CONFIG_ARM9

// AHB0 device base address define
#define SOCLE_AHB0_ES6    	        0x18100000
#define SOCLE_AHB0_ES5    	        0x180FC000
#define SOCLE_AHB0_ES4    	        0x180F8000
#define SOCLE_AHB0_ES3    	        0x180F4000
#define SOCLE_AHB0_ES2        	    0x180F0000
#define SOCLE_AHB0_ES1            	0x180EC000
#define SOCLE_AHB0_ES0				0x180E8000
	
#define SOCLE_AHB0_NAND	            0x180D0000
#define SOCLE_AHB0_DDRMC			    0x180B4000	
#define SOCLE_AHB0_SDRSTMC            0x180B0000
#define SOCLE_AHB0_IDE	            0x180A8000
#define SOCLE_AHB0_UHC	            0x180A4000
#define SOCLE_AHB0_UDC	            0x180A0000
#define SOCLE_AHB0_MAC1               0x1809C000
#define SOCLE_AHB0_MAC0               0x18098000
#define SOCLE_AHB0_A2A_DMA            0x18094000
#define SOCLE_AHB0_HDMA	            0x18090000
#define SOCLE_AHB0_CPU_DebugIF        0x1808C000
#define SOCLE_AHB0_CPU_Mailbox        0x18088000
#define SOCLE_AHB0_Arbiter            0x18084000
#define SOCLE_AHB0_INTC		        0x18080000

// APB0 device base address define
#define SOCLE_APB0_I2S       			0x18028000
#define SOCLE_APB0_SDMMC	    	        0x18024000
#define SOCLE_APB0_I2C 	            0x18020000
#define SOCLE_APB0_SCU                0x1801C000
#define SOCLE_APB0_SPI	            0x18018000
#define SOCLE_APB0_RTC	            0x18014000
#define SOCLE_APB0_WDT	            0x18010000
#define SOCLE_APB0_GPIO	            0x1800C000
#define SOCLE_APB0_UART1              0x18008000
#define SOCLE_APB0_UART0              0x18004000
#define SOCLE_APB0_TIMER	            0x18000000


// AHB1 device base address define
#define SOCLE_AHB1_ES6    	        0x18700000
#define SOCLE_AHB1_ES5    	        0x186FC000
#define SOCLE_AHB1_ES4    	        0x186F8000
#define SOCLE_AHB1_ES3    	        0x186F4000
#define SOCLE_AHB1_ES2        	    0x186F0000
#define SOCLE_AHB1_ES1            	0x186EC000
#define SOCLE_AHB1_ES0				0x186E8000

#define SOCLE_AHB1_VOP			    0x184C4000	
#define SOCLE_AHB1_VIP            	0x184C0000	
#define SOCLE_AHB1_MPEG4_DE	        0x184BC000
#define SOCLE_AHB1_MPEG4_EN	        0x184B8000
#define SOCLE_AHB1_Arbiter            0x18484000
#define SOCLE_AHB1_INTC		        0x18480000

// APB1 device base address define
#define SOCLE_APB1_WDT	            0x18410000
#define SOCLE_APB1_GPIO	            0x1840C000
#define SOCLE_APB1_UART0              0x18404000
#define SOCLE_APB1_TIMER	            0x18400000

#endif //CONFIG_ARM9


#if defined(CONFIG_ARM7) || defined(CONFIG_ARM7_HI)

// AHB0 device base address define
#define SOCLE_AHB0_ES6    	        0x18900000
#define SOCLE_AHB0_ES5    	        0x188FC000
#define SOCLE_AHB0_ES4    	        0x188F8000
#define SOCLE_AHB0_ES3    	        0x188F4000
#define SOCLE_AHB0_ES2        	    0x188F0000
#define SOCLE_AHB0_ES1            	0x188EC000
#define SOCLE_AHB0_ES0				0x188E8000
	
#define SOCLE_AHB0_NAND	            0x188D0000
#define SOCLE_AHB0_DDRMC			    0x188B4000	
#define SOCLE_AHB0_SDRSTMC            0x188B0000
#define SOCLE_AHB0_IDE	            0x188A8000
#define SOCLE_AHB0_UHC	            0x188A4000
#define SOCLE_AHB0_UDC	            0x188A0000
#define SOCLE_AHB0_MAC1               0x1889C000
#define SOCLE_AHB0_MAC0               0x18898000
#define SOCLE_AHB0_A2A_DMA            0x18894000
#define SOCLE_AHB0_HDMA	            0x18890000
#define SOCLE_AHB0_Arbiter            0x18884000
#define SOCLE_AHB0_INTC		        0x18880000

// APB0 device base address define
#define SOCLE_APB0_I2S       			0x18828000
#define SOCLE_APB0_SDMMC	    	        0x18824000
#define SOCLE_APB0_I2C 	            0x18820000
#define SOCLE_APB0_SCU                0x1881C000
#define SOCLE_APB0_SPI	            0x18818000
#define SOCLE_APB0_RTC	            0x18814000
#define SOCLE_APB0_WDT	            0x18810000
#define SOCLE_APB0_GPIO	            0x1880C000
#define SOCLE_APB0_UART1              0x18808000
#define SOCLE_APB0_UART0              0x18804000
#define SOCLE_APB0_TIMER	            0x18800000



// AHB1 device base address define
#define SOCLE_AHB1_ES6    	        0x18300000
#define SOCLE_AHB1_ES5    	        0x182FC000
#define SOCLE_AHB1_ES4    	        0x182F8000
#define SOCLE_AHB1_ES3    	        0x182F4000
#define SOCLE_AHB1_ES2        	    0x182F0000
#define SOCLE_AHB1_ES1            	0x182EC000
#define SOCLE_AHB1_ES0				0x182E8000

#define SOCLE_AHB1_VOP			    0x180C4000	
#define SOCLE_AHB1_VIP            	0x180C0000	
#define SOCLE_AHB1_MPEG4_DE	        0x180BC000
#define SOCLE_AHB1_MPEG4_EN	        0x180B8000
#define SOCLE_AHB1_CPU_DebugIF        0x1808C000
#define SOCLE_AHB1_CPU_Mailbox	    0x18088000
#define SOCLE_AHB1_Arbiter            0x18084000
#define SOCLE_AHB1_INTC		        0x18080000

// APB1 device base address define
#define SOCLE_APB1_WDT	            0x18010000
#define SOCLE_APB1_GPIO	            0x1800C000
#define SOCLE_APB1_UART0              0x18004000
#define SOCLE_APB1_TIMER	            0x18000000

#endif //CONFIG_ARM7 || CONFIG_ARM7_HI



#define	PHY_RTL8201

// GPIO
//#define SOCLE_GPIO_WITH_INT
#define SOCLE_GPIO_FLASH_LED_TEST	0
#define GPIO_FLASH_LED_PA	0
#define GPIO_FLASH_LED_PB	0

#endif //_platform_H_
