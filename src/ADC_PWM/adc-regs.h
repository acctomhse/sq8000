#ifndef	__SOCLE_ADC_REGS_H_INCLUDED
#define	__SOCLE_ADC_REGS_H_INCLUDED

#include <global.h>

/* REG OFFSET */
#define ADC_DATA		0X0000
#define ADC_STAS		0X0004
#define ADC_CTRL		0X0008


/* ADC_DATA (0X0000) */
//[9:0]
#define ADC_CONV_VAL_MSK		BIT_MASK(10)

/* ADC_STAS (0X0004) */
//[0]
#define ADC_CONV_STAS		BIT_SHIHT(0)
#define ACD_STAS_STOP		0X0


/* ADC_CTRL (0X0008) */
//[2:0]
#define ADC_CH_MSK		BIT_MASK(3)
#define ADC_CH_0		0X0
#define ADC_CH_1		0X1
#define ADC_CH_2		0X2
#define ADC_CH_3		0X3
#define ADC_CH_4		0X4
#define ADC_CH_5		0X5
#define ADC_CH_6		0X6
#define ADC_CH_7		0X7

//[3]
#define ADC_PWR_UP			BIT_SHIHT(3)

//[4]
#define ADC_STR_CONV		BIT_SHIHT(4)

//[5]
#define ADC_INT_EN			BIT_SHIHT(5)

//[6]
#define ADC_INT_STAS		BIT_SHIHT(6)

#endif //__SOCLE_ADC_REGS_H_INCLUDED
