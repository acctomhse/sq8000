#ifndef _platform_H_
#define _platform_H_

#include <io.h>
#include <type.h>
#include <interrupt.h>

#define INTC_MAX_IRQ            	31			// interrupt source number
#define INTC_MAX_FIQ				31			// interrupt source number
#define MAX_SYSTEM_IRQ				31			// max. system interrupt source number

#define INTC_CONNECT_SYS_INT		SYS_INT_7	// interrupt controller connect which MIPS IP


#define EXT_OSC					48			// Ext OSC (MHz)
#define SOCLE_UART_EXT_CLK			48000000


#define MAPPING_MASK				0x1FFFFFFF
#define CACHE_BASE				0x60000000	// Cache Kenel Mode

#define KSEG_TO_PHY(x)				( ((int) (x)) & MAPPING_MASK )
#define PHY_TO_CACHE(x)				( ( ((int) (x)) & MAPPING_MASK ) | CACHE_BASE )

// memory mapped address
#define	SOCLE_MEMORY_ADDR_START			0x40000000
#define	SOCLE_MEMORY_ADDR_SIZE			0x02000000

#define	SOCLE_MEMORY_BANKS			2

#define SOCLE_MM_DDR_SDR_BANK0			0x00000000
#define SOCLE_MM_DDR_SDR_BANK1			0x40000000

#define	SOCLE_NORFLASH_BANKS			1

#define SOCLE_NOR_FLASH_BANK0               	0x16000000	

#define SOCLE_SRAM_BANK0_START		0x1CD20000
#define SOCLE_SRAM_BANK1_START		0x1CD30000

#define SOCLE_SRAM_BANK0_SIZE			0x2040
#define SOCLE_SRAM_BANK1_SIZE			0x2040


// APB device base address define
#define SOCLE_APB0_SCU		0x1D1A0000
#define SOCLE_APB0_WDT		0x1D180000
#define SOCLE_APB0_RTC		0x1D160000
#define SOCLE_APB0_GPIO		0x1D140000
#define SOCLE_APB0_TIMER_PWM	0x1D120000
#define SOCLE_APB0_TIMER	0x1D100000
#define SOCLE_APB0_I2S		0x1D0C0000
#define SOCLE_APB0_I2C0		0x1D0A0000
#define SOCLE_APB0_I2C2		0x1D080000
#define SOCLE_APB0_SPI0		0x1D060000
#define SOCLE_APB0_I2C1		0x1D040000
#define SOCLE_APB0_UART1	0x1D020000
#define SOCLE_APB0_UART0	0x1D000000

// AHB device base address define
#define SOCLE_AHB0_EBD_SRAM	0x1CD20000
#define SOCLE_AHB0_NAND		0x1CB20000
#define SOCLE_AHB0_LCD		0x1CB00000
#define SOCLE_AHB0_HDMA		0x180C0000
#define SOCLE_AHB0_UHC		0x18080000
#define SOCLE_AHB0_INTC		0x18040000
#define SOCLE_AHB0_ARBITER	0x18020000
#define SOCLE_AHB0_SDRSTMC	0x18000000	
#define SOCLE_AHB0_NAND		0x1CB20000	

#define SOCLE_APB0_MP		SOCLE_APB0_GPIO
#define PANTHER7_AHB_0_HDMA_0	SOCLE_AHB0_HDMA

// GPIO
//#define SOCLE_GPIO_WITH_INT
#define SOCLE_GPIO_FLASH_LED_TEST	1
#define GPIO_FLASH_LED_PA		1
#define GPIO_FLASH_LED_PB		0

#endif //_platform_H_
