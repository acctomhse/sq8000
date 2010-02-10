#ifndef	_irqs_h_included_
#define	_irqs_h_included_

#include "interrupt.h"

#define	SOCLE_INTC_EXTERNAL18		31
#define	SOCLE_INTC_SDMMC			30
#define SOCLE_INTC_A2PDMA			29
#define	SOCLE_INTC_I2C		 		28
//MPEG4 INT define
#define	SOCLE_INTC_VOP				27
#define	SOCLE_INTC_VIP				26
#define	SOCLE_INTC_MPEG4_ENC		25
#define	SOCLE_INTC_MPEG4_DEC		24
//#define	SOCLE_INTC_CPU_WBERR	23
#define SOCLE_INTC_A2A_bri_DMA		23
#define	SOCLE_INTC_EXTERNAL12		22
#define	SOCLE_INTC_EXTERNAL11		21
#define	SOCLE_INTC_EXTERNAL10		20
#define	SOCLE_INTC_EXTERNAL9		19
#define	SOCLE_INTC_EXTERNAL8		18
#define	SOCLE_INTC_EXTERNAL7		17
#define	SOCLE_INTC_EXTERNAL6		16
#define	SOCLE_INTC_EXTERNAL5		15
#define	SOCLE_INTC_EXTERNAL4		14
#define	SOCLE_INTC_EXTERNAL3		13
#define	SOCLE_INTC_EXTERNAL2		12
#define	SOCLE_INTC_EXTERNAL1		11
#define SOCLE_INTC_EXTERNAL0		10
#define SOCLE_INTC_UHC			SOCLE_INTC_EXTERNAL0
#define SOCLE_INTC_UDC				9
#define SOCLE_INTC_MAC1   	      	8
#define SOCLE_INTC_MAC0				7
#define SOCLE_INTC_RTC				6
#define SOCLE_INTC_TMR0_2			5
#define SOCLE_INTC_TMR0_1			4
#define SOCLE_INTC_TMR0_0			3
#define SOCLE_INTC_UART1			2
#define SOCLE_INTC_UART0			1
#define SOCLE_INTC_SPI				0

#endif	//_irqs_h_included_
