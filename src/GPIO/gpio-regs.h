
#ifndef __GPIO_REG_H
#define __GPIO_REG_H

#define GPIO_PER_PORT_PIN_NUM		0x08
#define GPIO_PORT_NUM			0x04

#define GPIO_PADR			0X0000
#define GPIO_PACON			0X0004
#define GPIO_PBDR			0X0008
#define GPIO_PBCON			0X000c
#define GPIO_PCDR			0X0010
#define GPIO_PCCON			0X0014
#define GPIO_PDDR			0X0018
#define GPIO_PDCON			0X001c
#define GPIO_TEST			0X0020

#define GPIO_IEA			0X0024
#define GPIO_IEB			0X0028
#define GPIO_IEC			0X002c
#define GPIO_IED			0X0030

#define GPIO_ISA			0X0034
#define GPIO_ISB			0X0038
#define GPIO_ISC			0X003c
#define GPIO_ISD			0X0040

#define GPIO_IBEA			0X0044
#define GPIO_IBEB			0X0048
#define GPIO_IBEC			0X004c
#define GPIO_IBED			0X0050

#define GPIO_IEVA			0X0054
#define GPIO_IEVB			0X0058
#define GPIO_IEVC			0X005c
#define GPIO_IEVD			0X0060

#define GPIO_ICA			0X0064
#define GPIO_ICB			0X0068
#define GPIO_ICC			0X006c
#define GPIO_ICD			0X0070

#define GPIO_ISR			0X0074

#endif	//__GPIO_REG_H
