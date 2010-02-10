#ifndef _tsc2000_h_included_
#define _tsc2000_h_included_

/* TSC2000 register definition */
//PAGE 0
#define TSC2000_REG_X			((0 << 11) | (0 << 5))
#define TSC2000_REG_Y			((0 << 11) | (1 << 5))
#define TSC2000_REG_Z1			((0 << 11) | (2 << 5))
#define TSC2000_REG_Z2			((0 << 11) | (3 << 5))
#define TSC2000_REG_BAT1		((0 << 11) | (5 << 5))
#define TSC2000_REG_BAT2		((0 << 11) | (6 << 5))
#define TSC2000_REG_AUX1		((0 << 11) | (7 << 5))
#define TSC2000_REG_AUX2		((0 << 11) | (8 << 5))
#define TSC2000_REG_TEMP1		((0 << 11) | (9 << 5))
#define TSC2000_REG_TEMP2		((0 << 11) | (0xA << 5))
#define TSC2000_REG_DAC		((0 << 11) | (0xB << 5))
#define TSC2000_REG_ZERO		((0 << 11) | (0x10 << 5))
//PAGE 1
#define TSC2000_REG_ADC		((1 << 11) | (0 << 5))
#define TSC2000_REG_DACCTL		((1 << 11) | (2 << 5))
#define TSC2000_REG_REF		((1 << 11) | (3 << 5))
#define TSC2000_REG_RESET		((1 << 11) | (4 << 5))
#define TSC2000_REG_CONFIG		((1 << 11) | (5 << 5))

/* bit definition of TSC2000 ADC register */
#define TC_PSM          (1 << 15)
#define TC_STS          (1 << 14)
#define TC_AD3          (1 << 13)
#define TC_AD2          (1 << 12)
#define TC_AD1          (1 << 11)
#define TC_AD0          (1 << 10)
#define TC_RS1          (1 << 9)
#define TC_RS0          (1 << 8)
#define TC_AV1          (1 << 7)
#define TC_AV0          (1 << 6)
#define TC_CL1          (1 << 5)
#define TC_CL0          (1 << 4)
#define TC_PV2          (1 << 3)
#define TC_PV1          (1 << 2)
#define TC_PV0          (1 << 1)

/* default value for TSC2000 ADC register for use with touch functions */
#define DEFAULT_ADC		(TC_PV1 | TC_AV0 | TC_AV1 | TC_RS0)

#define TSC2000_INTR		SOCLE_INTC_EXTERNAL0

#define IS_PEN_DOWN(x)	( x & (1<<15) )

#define TS_PAGE1_PREFIX	(0x0800)
#define TS_REG_OFFSET	(5)
#define TS_ADC_DATA_READY	(1<<14)
#define IS_TS_ADC_DATA_READY(x)	(x & TS_ADC_DATA_READY)


//TSC2000 CMD WORD
#define READ_CMD 			0x8000
#define WRITE_CMD			0x7fff

#endif // _tsc2000_h_included_

