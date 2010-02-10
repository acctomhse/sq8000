#ifndef	_irqs_h_included_
#define	_irqs_h_included_

#define	SOCLE_INTC_EXTERNAL1		30
#define	SOCLE_INTC_EXTERNAL0		29
#define SOCLE_INTC_GPIO			28
#define	SOCLE_INTC_NAND			27
#define	SOCLE_INTC_CLCD			25
#define	SOCLE_INTC_HDMA			22
#define	SOCLE_INTC_UHC			20
#define SOCLE_INTC_RTC			13
#define SOCLE_INTC_PWM1         	12
#define SOCLE_INTC_PWM0      	  	11
#define	SOCLE_INTC_TMR0_2		10
#define SOCLE_INTC_TMR0_1     		9
#define SOCLE_INTC_TMR0_0        	8
#define SOCLE_INTC_I2S        	 	6
#define SOCLE_INTC_I2C0        	 	5
#define SOCLE_INTC_I2C2       	 	4
#define SOCLE_INTC_SPI0      	 	3
#define SOCLE_INTC_I2C1    	   	2
#define SOCLE_INTC_UART1        	1
#define SOCLE_INTC_UART0        	0

#define SOCLE_INTC_MPS2			SOCLE_INTC_GPIO
#define PANTHER7_INTC_HDMA_0		SOCLE_INTC_HDMA

#endif	//_irqs_h_included_
