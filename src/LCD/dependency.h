#include <platform.h>
#include <irqs.h>

/* Set the IP's base address */
#ifndef SOCLE_LCD_BASE
#ifdef SOCLE_AHB0_LCD
#define SOCLE_LCD_BASE	SOCLE_AHB0_LCD
#else
#define SOCLE_LCD_BASE	SOCLE_AHB0_ES3
//#error "LCD IP base address is not defined"
#endif
#endif

/* Set the IP's irq */
 
#ifndef SOCLE_LCD_IRQ
#ifdef SOCLE_INTC_LCD
#define SOCLE_LCD_IRQ	SOCLE_INTC_LCD
#else
#define SOCLE_LCD_IRQ	SOCLE_INTC_EXTERNAL2
//#error "LCD IRQ is not defined"
#endif
#endif

/* Set IP version dependance */

#ifdef CONFIG_PDK
#define SOCLE_YUV_TEST 0
#else
#define SOCLE_YUV_TEST 1
#endif
 
#ifdef CONFIG_PC7210
#define LCD_YUV_MODE_TEST 0
#else
#define LCD_YUV_MODE_TEST 1
#endif
 
