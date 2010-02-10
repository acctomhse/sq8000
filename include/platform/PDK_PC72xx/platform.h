#ifndef _platform_H_
#define _platform_H_

#include <io.h>
#include <type.h>
#include <interrupt.h>

#define INTC_MAX_IRQ            	32			// interrupt source number
#define INTC_MAX_FIQ				32			// interrupt source number
#define MAX_SYSTEM_IRQ				32			// max. system interrupt source number

#define INTC_CONNECT_SYS_INT		SYS_INT_7	// interrupt controller connect which MIPS IP


#define EXT_OSC					12			// Ext OSC (MHz)
#define UPLL_XIN					(11.0592)		// UART Ext OSC (MHz)
#define SOCLE_UART_EXT_CLK			(11059200*8)


#define MAPPING_MASK				0x1FFFFFFF
#define CACHE_BASE					0x00000000	// Cache Kenel Mode

#define KSEG_TO_PHY(x)				( ((int) (x)) & MAPPING_MASK )
#define PHY_TO_CACHE(x)				( ( ((int) (x)) & MAPPING_MASK ) | CACHE_BASE )

// memory mapped address
#define	SOCLE_MEMORY_ADDR_START			0x80000000
#define	SOCLE_MEMORY_ADDR_SIZE			0x02000000

#define	SOCLE_MEMORY_BANKS	4

#define SOCLE_MM_DDR_SDR_BANK0			0x80000000
#define SOCLE_MM_DDR_SDR_BANK1			0x80800000	
#define SOCLE_MM_DDR_SDR_BANK2			0x81000000	
#define SOCLE_MM_DDR_SDR_BANK3			0x81800000


#define	SOCLE_NORFLASH_BANKS	2

#define SOCLE_NOR_FLASH_BANK0               0x10000000	
#define SOCLE_NOR_FLASH_BANK1               0x11000000	

#define SOCLE_CACHE_CTRL_BASE                   0xEFFF0000

/* AHB Peripherals */
#define SOCLE_AHB0_APB			0x18000000
#define SOCLE_AHB0_INTC			0x18050000
#define SOCLE_AHB0_HDMA			0x18054000
#define PANTHER7_AHB_0_HDMA_0		SOCLE_AHB0_HDMA
#define SOCLE_AHB0_DDRMC		0x18058000
#define SOCLE_AHB0_MAC			0x1805C000
#define SOCLE_AHB0_UDC			0x18060000
#define SOCLE_AHB0_UHC			0x18064000
#define SOCLE_AHB0_LCD			0x18040000
#define SOCLE_AHB0_ARBITER		0x1806C000
#define SOCLE_AHB0_NAND			0x18070000

/* APB Peripherals */
#define SOCLE_APB0_TIMER		0x18000000
#define SOCLE_APB0_RTC			0x18004000
#define SOCLE_APB0_WDT			0x18008000
#define SOCLE_APB0_GPIO0		0x1800C000
#define SOCLE_APB0_GPIO1		0x18010000
#define SOCLE_APB0_SPI			0x18014000
#define SOCLE_APB0_I2C			0x18018000
#define SOCLE_APB0_I2S			0x1801C000
#define SOCLE_APB0_SDMMC		0x18020000
#define SOCLE_APB0_TIMER_PWM		0x18024000
#define SOCLE_APB0_ADC			0x18028000
#define SOCLE_APB0_UART0		0x1802C000
#define SOCLE_APB0_UART1		0x18030000
#define SOCLE_APB0_UART2		0x18034000
#define SOCLE_APB0_UART3		0x18038000
#define SOCLE_APB0_SCU			0x1803C000
#define SOCLE_APB0_EXT			0x18040000

/* SCU setting for UART hardware DMA  & UPLL power normal*/
#define SOCLE_CHIPCFG_A		0x0000000c
#define SOCLE_PLLPARAM_B		0x00000008
#define SOCLE_UART_OCCUPY_HDMA23_M	0x000000c0
#define SOCLE_UART_OCCUPY_HDMA23_S	0x6
#define SOCLE_SCU_UPLL				0x00080000
#define SOCLE_SCU_UPLL_NORMAL		0x00000000

#define	PHY_IC_PLUS

// GPIO
#define SOCLE_GPIO_WITH_INT
#define SOCLE_GPIO_FLASH_LED_TEST	0
#define GPIO_FLASH_LED_PA	0
#define GPIO_FLASH_LED_PB	0

#endif //_platform_H_
