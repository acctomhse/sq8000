#ifndef	_udc_reg_h_
#define	_udc_reg_h_

#include "type.h"

#ifdef	UDC_GAIN_PHY
#undef		UDC_SOCLE_PHY
#else
#define		UDC_SOCLE_PHY
#endif

/* UDC Registers Offset Define */

// for FPGA
//#define UDC_REG_BASE		SOCLE_AHB0_ES0
//#define UDC_INT				SOCLE_INTC_EXTERNAL0

#ifndef UDC_REG_BASE
#define UDC_REG_BASE		SOCLE_AHB0_UDC
#endif

#ifndef UDC_INT
#define UDC_INT		SOCLE_INTC_UDC
#endif


#define	PHY_TEST_EN			(UDC_REG_BASE +	0x0000)		/* PHY. Enable Register (option) */
#define	PHY_TEST			(UDC_REG_BASE +	0x0004)		/* USB  PHY. Test Register (option) */
#define	UDC_DEVCTL			(UDC_REG_BASE +	0x0008)		/* Device Control Register */
#define	UDC_DEVINFO			(UDC_REG_BASE +	0x0010)		/* Device Info. Register */
#define	UDC_ENINT			(UDC_REG_BASE +	0x0014)		/* UDC Interrupt Enable Register */
#define	UDC_INTFLG			(UDC_REG_BASE +	0x0018)		/* UDC Interrupt Flag Register */
#define	UDC_INTCON			(UDC_REG_BASE +	0x001c)		/* UDC Interrupt Control Register */
#define	UDC_SETUP1			(UDC_REG_BASE +	0x0020)		/* UDC Setup Status Register 1 (Read-Only) */
#define	UDC_SETUP2			(UDC_REG_BASE +	0x0024)		/* UDC Setup Status Register 2 (Read-Only) */
#define	UDC_AHBCON			(UDC_REG_BASE +	0x0028)		/* UDC AHB Control Register */
/* ----------------------------------------- */
/* End-Point 0, Ctrl_OUT */
#define	UDC_RX0STAT			(UDC_REG_BASE +	0x0030)		/* UDC ENP 0 Ctrl_OUT Receive Status Register */
#define	UDC_RX0CON			(UDC_REG_BASE +	0x0034)		/* UDC ENP 0 Ctrl_OUT Receive Control Register */
#define	UDC_DMA0CTLO		(UDC_REG_BASE +	0x0038)		/* UDC ENP 0 Ctrl_OUT DMA Control Register */
#define	UDC_DMA0LM_OADDR	(UDC_REG_BASE +	0x003c)		/* UDC ENP 0 Ctrl_OUT DMA Local Memory Address Register */
/* End-Point 0, Ctrl_IN */
#define	UDC_TX0STAT			(UDC_REG_BASE +	0x0040)		/* UDC ENP 0 Ctrl_IN Transmit Status Register */
#define	UDC_TX0CON			(UDC_REG_BASE +	0x0044)		/* UDC ENP 0 Ctrl_IN Transmit Control Register */
#define	UDC_TX0BUF			(UDC_REG_BASE +	0x0048)		/* UDC ENP 0 Ctrl_IN Buffer Status Register */
#define	UDC_DMA0CTLI		(UDC_REG_BASE +	0x004c)		/* UDC ENP 0 Ctrl_IN DMA Control Register */
#define	UDC_DMA0LM_IADDR	(UDC_REG_BASE +	0x0050)		/* UDC ENP 0 Ctrl_IN DMA Local Memory Address Register */
/* ----------------------------------------- */
/* End-Point 1, Bulk_OUT */
#define	UDC_RX1STAT			(UDC_REG_BASE +	0x0054)		/* UDC ENP 1 Bulk_OUT Receive Status Register */
#define	UDC_RX1CON			(UDC_REG_BASE +	0x0058)		/* UDC ENP 1 Bulk_OUT Receive Control Register */
#define	UDC_DMA1CTRLO		(UDC_REG_BASE +	0x005c)		/* UDC ENP 1 Bulk_OUT DMA Control Register */
#define	UDC_DMA1LM_OADDR	(UDC_REG_BASE +	0x0060)		/* UDC ENP 1 Bulk_OUT DMA Local Memory Address Register */
/* End-Point 2, Bulk_IN */
#define	UDC_TX2STAT			(UDC_REG_BASE +	0x0064)		/* UDC ENP 2 Bulk_IN Transmit Status Register */
#define	UDC_TX2CON			(UDC_REG_BASE +	0x0068)		/* UDC ENP 2 Bulk_IN Transmit Control Register */
#define	UDC_TX2BUF			(UDC_REG_BASE +	0x006c)		/* UDC ENP 2 Intr_IN Buffer Status Register */
#define	UDC_DMA2CTRLI		(UDC_REG_BASE +	0x0070)		/* UDC ENP 2 Bulk_IN DMA Control Register */
#define	UDC_DMA2LM_IADDR	(UDC_REG_BASE +	0x0074)		/* UDC ENP 2 Bulk_IN DMA Local Memory Address Register */
/* End-Point 3, Intr_IN */
#define	UDC_TX3STAT			(UDC_REG_BASE +	0x0078)		/* UDC ENP 3 Intr_IN Transmit Status Register */
#define	UDC_TX3CON			(UDC_REG_BASE +	0x007c)		/* UDC ENP 3 Intr_IN Transmit Control Register */
#define	UDC_TX3BUF			(UDC_REG_BASE +	0x0080)		/* UDC ENP 3 Intr_IN Buffer Status Register */
#define	UDC_DMA3CTRLI		(UDC_REG_BASE +	0x0084)		/* UDC ENP 3 Intr_IN DMA Control Register */
#define	UDC_DMA3LM_IADDR	(UDC_REG_BASE +	0x0088)		/* UDC ENP 3 Intr_IN DMA Local Memory Address Register */
/* ----------------------------------------- */
/* End-Point 4, Bulk_OUT */
#define	UDC_RX4STAT			(UDC_REG_BASE +	0x008c)		/* UDC ENP 4 Bulk_OUT Receive Status Register */
#define	UDC_RX4CON			(UDC_REG_BASE +	0x0090)		/* UDC ENP 4 Bulk_OUT Receive Control Register */
#define	UDC_DMA4CTRLO		(UDC_REG_BASE +	0x0094)		/* UDC ENP 4 Bulk_OUT DMA Control Register */
#define	UDC_DMA4LM_OADDR	(UDC_REG_BASE +	0x0098)		/* UDC ENP 4 Bulk_OUT DMA Local Memory Address Register */
/* End-Point 5, Bulk_IN */
#define	UDC_TX5STAT			(UDC_REG_BASE +	0x009c)		/* UDC ENP 5 Bulk_IN Transmit Status Register */
#define	UDC_TX5CON			(UDC_REG_BASE +	0x00a0)		/* UDC ENP 5 Bulk_IN Transmit Control Register */
#define	UDC_TX5BUF			(UDC_REG_BASE +	0x00a4)		/* UDC ENP 5 Intr_IN Buffer Status Register */
#define	UDC_DMA5CTRLI		(UDC_REG_BASE +	0x00a8)		/* UDC ENP 5 Bulk_IN DMA Control Register */
#define	UDC_DMA5LM_IADDR	(UDC_REG_BASE +	0x00ac)		/* UDC ENP 5 Bulk_IN DMA Local Memory Address Register */
/* End-Point 6, Intr_IN */
#define	UDC_TX6STAT			(UDC_REG_BASE +	0x00b0)		/* UDC ENP 6 Intr_IN Transmit Status Register */
#define	UDC_TX6CON			(UDC_REG_BASE +	0x00b4)		/* UDC ENP 6 Intr_IN Transmit Control Register */
#define	UDC_TX6BUF			(UDC_REG_BASE +	0x00b8)		/* UDC ENP 6 Intr_IN Buffer Status Register */
#define	UDC_DMA6CTRLI		(UDC_REG_BASE +	0x00bc)		/* UDC ENP 6 Intr_IN DMA Control Register */
#define	UDC_DMA6LM_IADDR	(UDC_REG_BASE +	0x00c0)		/* UDC ENP 6 Intr_IN DMA Local Memory Address Register */
/* ----------------------------------------- */
/* End-Point 7, Bulk_OUT */
#define	UDC_RX7STAT			(UDC_REG_BASE +	0x00c4)		/* UDC ENP 7 Bulk_OUT Receive Status Register */
#define	UDC_RX7CON			(UDC_REG_BASE +	0x00c8)		/* UDC ENP 7 Bulk_OUT Receive Control Register */
#define	UDC_DMA7CTRLO		(UDC_REG_BASE +	0x00cc)		/* UDC ENP 7 Bulk_OUT DMA Control Register */
#define	UDC_DMA7LM_OADDR	(UDC_REG_BASE +	0x00d0)		/* UDC ENP 7 Bulk_OUT DMA Local Memory Address Register */
/* End-Point 8, Bulk_IN */
#define	UDC_TX8STAT			(UDC_REG_BASE +	0x00d4)		/* UDC ENP 8 Bulk_IN Transmit Status Register */
#define	UDC_TX8CON			(UDC_REG_BASE +	0x00d8)		/* UDC ENP 8 Bulk_IN Transmit Control Register */
#define	UDC_TX8BUF			(UDC_REG_BASE +	0x00dc)		/* UDC ENP 8 Intr_IN Buffer Status Register */
#define	UDC_DMA8CTRLI		(UDC_REG_BASE +	0x00e0)		/* UDC ENP 8 Bulk_IN DMA Control Register */
#define	UDC_DMA8LM_IADDR	(UDC_REG_BASE +	0x00e4)		/* UDC ENP 8 Bulk_IN DMA Local Memory Address Register */
/* End-Point 9, Intr_IN */
#define	UDC_TX9STAT			(UDC_REG_BASE +	0x00e8)		/* UDC ENP 9 Intr_IN Transmit Status Register */
#define	UDC_TX9CON			(UDC_REG_BASE +	0x00ec)		/* UDC ENP 9 Intr_IN Transmit Control Register */
#define	UDC_TX9BUF			(UDC_REG_BASE +	0x00f0)		/* UDC ENP 9 Intr_IN Buffer Status Register */
#define	UDC_DMA9CTRLI		(UDC_REG_BASE +	0x00f4)		/* UDC ENP 9 Intr_IN DMA Control Register */
#define	UDC_DMA9LM_IADDR	(UDC_REG_BASE +	0x00f8)		/* UDC ENP 9 Intr_IN DMA Local Memory Address Register */
/* ----------------------------------------- */
/* End-Point 10, Bulk_OUT */
#define	UDC_RX10STAT		(UDC_REG_BASE +	0x00fc)		/* UDC ENP 10 Bulk_OUT Receive Status Register */
#define	UDC_RX10CON			(UDC_REG_BASE +	0x0100)		/* UDC ENP 10 Bulk_OUT Receive Control Register */
#define	UDC_DMA10CTRLO		(UDC_REG_BASE +	0x0104)		/* UDC ENP 10 Bulk_OUT DMA Control Register */
#define	UDC_DMA10LM_OADDR	(UDC_REG_BASE +	0x0108)		/* UDC ENP 10 Bulk_OUT DMA Local Memory Address Register */
/* End-Point 11, Bulk_IN */
#define	UDC_TX11STAT		(UDC_REG_BASE +	0x010c)		/* UDC ENP 11 Bulk_IN Transmit Status Register */
#define	UDC_TX11CON			(UDC_REG_BASE +	0x0110)		/* UDC ENP 11 Bulk_IN Transmit Control Register */
#define	UDC_TX11BUF			(UDC_REG_BASE +	0x0114)		/* UDC ENP 11 Intr_IN Buffer Status Register */
#define	UDC_DMA11CTRLI		(UDC_REG_BASE +	0x0118)		/* UDC ENP 11 Bulk_IN DMA Control Register */
#define	UDC_DMA11LM_IADDR	(UDC_REG_BASE +	0x011c)		/* UDC ENP 11 Bulk_IN DMA Local Memory Address Register */
/* End-Point 12, Intr_IN */
#define	UDC_TX12STAT		(UDC_REG_BASE +	0x0120)		/* UDC ENP 12 Intr_IN Transmit Status Register */
#define	UDC_TX12CON			(UDC_REG_BASE +	0x0124)		/* UDC ENP 12 Intr_IN Transmit Control Register */
#define	UDC_TX12BUF			(UDC_REG_BASE +	0x0128)		/* UDC ENP 12 Intr_IN Buffer Status Register */
#define	UDC_DMA12CTRLI		(UDC_REG_BASE +	0x012c)		/* UDC ENP 12 Intr_IN DMA Control Register */
#define	UDC_DMA12LM_IADDR	(UDC_REG_BASE +	0x0130)		/* UDC ENP 12 Intr_IN DMA Local Memory Address Register */
/* ----------------------------------------- */
/* End-Point 13, Bulk_OUT */
#define	UDC_RX13STAT		(UDC_REG_BASE +	0x0134)		/* UDC ENP 13 Bulk_OUT Receive Status Register */
#define	UDC_RX13CON			(UDC_REG_BASE +	0x0138)		/* UDC ENP 13 Bulk_OUT Receive Control Register */
#define	UDC_DMA13CTRLO		(UDC_REG_BASE +	0x013c)		/* UDC ENP 13 Bulk_OUT DMA Control Register */
#define	UDC_DMA13LM_OADDR	(UDC_REG_BASE +	0x0140)		/* UDC ENP 13 Bulk_OUT DMA Local Memory Address Register */
/* End-Point 14, Bulk_IN */
#define	UDC_TX14STAT		(UDC_REG_BASE +	0x0144)		/* UDC ENP 14 Bulk_IN Transmit Status Register */
#define	UDC_TX14CON			(UDC_REG_BASE +	0x0148)		/* UDC ENP 14 Bulk_IN Transmit Control Register */
#define	UDC_TX14BUF			(UDC_REG_BASE +	0x014c)		/* UDC ENP 14 Intr_IN Buffer Status Register */
#define	UDC_DMA14CTRLI		(UDC_REG_BASE +	0x0150)		/* UDC ENP 14 Bulk_IN DMA Control Register */
#define	UDC_DMA14LM_IADDR	(UDC_REG_BASE +	0x0154)		/* UDC ENP 14 Bulk_IN DMA Local Memory Address Register */
/* End-Point 15, Intr_IN */
#define	UDC_TX15STAT		(UDC_REG_BASE +	0x0158)		/* UDC ENP 15 Intr_IN Transmit Status Register */
#define	UDC_TX15CON			(UDC_REG_BASE +	0x015c)		/* UDC ENP 15 Intr_IN Transmit Control Register */
#define	UDC_TX15BUF			(UDC_REG_BASE +	0x0160)		/* UDC ENP 15 Intr_IN Buffer Status Register */
#define	UDC_DMA15CTRLI		(UDC_REG_BASE +	0x0164)		/* UDC ENP 15 Intr_IN DMA Control Register */
#define	UDC_DMA15LM_IADDR	(UDC_REG_BASE +	0x0168)		/* UDC ENP 15 Intr_IN DMA Local Memory Address Register */

/* Bulk_OUT, Receive End Point */
#define	UDC_RXSTAT(x)		(x + 0x00)				/* UDC Bulk_OUT Receive Status Register */
#define	UDC_RXCON(x)		(x + 0x04)				/* UDC Bulk_OUT Receive Control Register */
#define	UDC_DMACTRLO(x)		(x + 0x08)				/* UDC Bulk_OUT DMA Control Register */
#define	UDC_DMALM_OADDR(x)	(x + 0x0c)				/* UDC Bulk_OUT DMA Local Memory Address Register */
/* Bulk_IN, Transmit End Point */
/* Intr_IN, Interrupt Transfer End Point */
#define	UDC_TXSTAT(x)		(x + 0x00)				/* Bulk_IN/Intr_IN Transmit Status Register */
#define	UDC_TXCON(x)		(x + 0x04)				/* Bulk_IN/Intr_IN Transmit Control Register */
#define	UDC_TXBUF(x)		(x + 0x08)				/* Bulk_IN/Intr_IN Buffer Status Register */
#define	UDC_DMACTRLI(x)		(x + 0x0c)				/* Bulk_IN/Intr_IN DMA Control Register */
#define	UDC_DMALM_IADDR(x)	(x + 0x10)				/* Bulk_IN/Intr_IN DMA Local Memory Address Register */

/* Bulk_OUT, Receive End Point */
#define	RXSTAT(x)			(x + 0x00)				/* Bulk_OUT Receive Status Register */
#define	RXCON(x)			(x + 0x04)				/* Bulk_OUT Receive Control Register */
#define	DMACTRLO(x)			(x + 0x08)				/* Bulk_OUT DMA Control Register */
#define	DMALM_OADDR(x)		(x + 0x0c)				/* Bulk_OUT DMA Local Memory Address Register */
/* Bulk_IN, Transmit End Point */
#define	TXSTAT(x)			(x + 0x10)				/* Bulk_IN Transmit Status Register */
#define	TXCON(x)			(x + 0x14)				/* Bulk_IN Transmit Control Register */
#define	TXBUF(x)			(x + 0x18)				/* Intr_IN Buffer Status Register */
#define	DMACTRLI(x)			(x + 0x1c)				/* Bulk_IN DMA Control Register */
#define	DMALM_IADDR(x)		(x + 0x20)				/* Bulk_IN DMA Local Memory Address Register */
/* Intr_IN, Interrupt Transfer End Point */
#define	INT_TXSTAT(x)		(x + 0x24)				/* Intr_IN Transmit Status Register */
#define	INT_TXCON(x)		(x + 0x28)				/* Intr_IN Transmit Control Register */
#define	INT_TXBUF(x)		(x + 0x2c)				/* Intr_IN Buffer Status Register */
#define	INT_DMACTRLI(x)		(x + 0x30)				/* Intr_IN DMA Control Register */
#define	INT_DMALM_IADDR(x)	(x + 0x34)				/* Intr_IN DMA Local Memory Address Register */

/* PHY_TEST_EN */
#define	PHY_TEST_CLK_EN		(1<<0)	/* For Socle's PHY Test	clock enable */
#define	PHY_TEST_CLK		(1<<1)	/* Enable Socle's PHY analog_test pin  */

/* PHY_TEST	*/				/* For Socle's PHY Test	mode use */
#define	PHY_TEST_ADDR		0x00
#define	PHY_TEST_DATA_IN	0x04

/* UDC_DEVCTL */
#define	DEV_FULL_SPD		(0x03)	// Full speed (USB 1.1) define
#define	DEV_SPEED			(0x00)
#define	DEV_RMTWKP			(1<<2)
#define	DEV_SELF_PWR		(1<<3)
#define	DEV_SOFT_CN			(1<<4)
#define	DEV_RESUME			(1<<5)
#define	DEV_PHY16BIT		(1<<6)
#define	SOFT_POR			(1<<7)
#define	CSR_DONE			(1<<8)

/* UDC_DEVINFO	*/
#define	DEV_ADDR			(0x0000007f)	// Device address's bits mask
#define	DEV_EN				(0x00000080)	// Device enable bit mask
#define	CFG_NUM				(0x00000f00)	// Configuration number bits mask
#define	IF_NUM				(0x0000f000)	// Interface number bits mask
#define	ALT_NUM				(0x000f0000)	// Alternate setting number bits mask
#define	VBUS_STS			(0x00100000)	// VBUS status bit mask
#define	ENUM_SPEED_MASK		(0x00600000)	// Enum speed bits mask
#define	ENUM_FULL_SPEED		(0x00600000)	// FULL speed value
#define	ENUM_HIGH_SPEED		(0x00000000)	// HIGH speed value
//cyli++ 01/17/07
#define ENDPT_NUM_MASK		(0x01800000)	// End-point number bits mask
#define ENDPT_NUM_16		(0x00000000)	// 16 end-point value
#define ENDPT_NUM_10            (0x00800000)    // 10 end-point value
#define ENDPT_NUM_4             (0x01000000)    // 4 end-point value


/* UDC_ENINT */
#define	EN_SOF_INTR			(1<<0)		/* Receive Start-Of-Frame Interrupt */
#define	EN_SETUP_INTR		(1<<1)		/* Receive SETUP package */
#define	EN_IN0_INTR			(1<<2)		/* ENP 0  Ctrl_IN  Transmit interrupt */
#define	EN_OUT0_INTR		(1<<3)		/* ENP 0  Ctrl_OUT Receive interrupt  */
#define	EN_USBRST_INTR		(1<<4)		/* USB Reset   Interrupt */
#define	EN_RSUME_INTR		(1<<5)		/* USB Resume  Interrupt */
#define	EN_SUSP_INTR		(1<<6)		/* USB Suspend Interrupt */

#define	EN_ENP1_INTR		(1<<8)		/* ENP 1  Bulk_OUT Receive Interrupt  */
#define	EN_ENP2_INTR		(1<<9)		/* ENP 2  Bulk_IN  Transmit Interrupt */
#define	EN_ENP3_INTR		(1<<10)		/* ENP 3  Intr_IN  Transmit Interrupt */
#define	EN_ENP4_INTR		(1<<11)		/* ENP 4  Bulk_OUT Receive Interrupt  */
#define	EN_ENP5_INTR		(1<<12)		/* ENP 5  Bulk_IN  Transmit Interrupt */
#define	EN_ENP6_INTR		(1<<13)		/* ENP 6  Intr_IN  Transmit Interrupt */
#define	EN_ENP7_INTR		(1<<14)		/* ENP 7  Bulk_OUT Receive Interrupt  */
#define	EN_ENP8_INTR		(1<<15)		/* ENP 8  Bulk_IN  Transmit Interrupt */
#define	EN_ENP9_INTR		(1<<16)		/* ENP 9  Intr_IN  Transmit Interrupt */
#define	EN_ENP10_INTR		(1<<17)		/* ENP 10 Bulk_OUT Receive Interrupt  */
#define	EN_ENP11_INTR		(1<<18)		/* ENP 11 Bulk_IN  Transmit Interrupt */
#define	EN_ENP12_INTR		(1<<19)		/* ENP 12 Intr_IN  Transmit Interrupt */
#define	EN_ENP13_INTR		(1<<20)		/* ENP 13 Bulk_OUT Receive Interrupt  */
#define	EN_ENP14_INTR		(1<<21)		/* ENP 14 Bulk_IN  Transmit Interrupt */
#define	EN_ENP15_INTR		(1<<22)		/* ENP 15 Intr_IN  Transmit Interrupt */
#define	EN_BOUTALL_INTR		(EN_ENP1_INTR | EN_ENP4_INTR | EN_ENP7_INTR | EN_ENP10_INTR | EN_ENP13_INTR)
#define	EN_BINALL_INTR		(EN_ENP2_INTR | EN_ENP5_INTR | EN_ENP8_INTR | EN_ENP11_INTR | EN_ENP14_INTR)
#define	EN_IINALL_INTR		(EN_ENP3_INTR | EN_ENP6_INTR | EN_ENP9_INTR | EN_ENP12_INTR | EN_ENP15_INTR)

/* UDC_INTFLG */
#define	SOF_INTR			(1<<0)		/* Receive Start-Of-Frame Interrupt */
#define	SETUP_INTR			(1<<1)		/* Receive SETUP package */
#define	IN0_INTR			(1<<2)		/* ENP 0  Ctrl_IN  Transmit interrupt */
#define	OUT0_INTR			(1<<3)		/* ENP 0  Ctrl_OUT Receive interrupt  */
#define	USBRST_INTR			(1<<4)		/* USB Reset   Interrupt */
#define	RSUME_INTR			(1<<5)		/* USB Resume  Interrupt */
#define	SUSP_INTR			(1<<6)		/* USB Suspend Interrupt */
#define	VBUS_INTR			(1<<7)		/* USB VBUS    Interrupt */

#define	ENP1_INTR			(1<<8)		/* ENP 1  Bulk_OUT Receive Interrupt  */
#define	ENP2_INTR			(1<<9)		/* ENP 2  Bulk_IN  Transmit Interrupt */
#define	ENP3_INTR			(1<<10)		/* ENP 3  Intr_IN  Transmit Interrupt */
#define	ENP4_INTR			(1<<11)		/* ENP 4  Bulk_OUT Receive Interrupt  */
#define	ENP5_INTR			(1<<12)		/* ENP 5  Bulk_IN  Transmit Interrupt */
#define	ENP6_INTR			(1<<13)		/* ENP 6  Intr_IN  Transmit Interrupt */
#define	ENP7_INTR			(1<<14)		/* ENP 7  Bulk_OUT Receive Interrupt  */
#define	ENP8_INTR			(1<<15)		/* ENP 8  Bulk_IN  Transmit Interrupt */
#define	ENP9_INTR			(1<<16)		/* ENP 9  Intr_IN  Transmit Interrupt */
#define	ENP10_INTR			(1<<17)		/* ENP 10 Bulk_OUT Receive Interrupt  */
#define	ENP11_INTR			(1<<18)		/* ENP 11 Bulk_IN  Transmit Interrupt */
#define	ENP12_INTR			(1<<19)		/* ENP 12 Intr_IN  Transmit Interrupt */
#define	ENP13_INTR			(1<<20)		/* ENP 13 Bulk_OUT Receive Interrupt  */
#define	ENP14_INTR			(1<<21)		/* ENP 14 Bulk_IN  Transmit Interrupt */
#define	ENP15_INTR			(1<<22)		/* ENP 15 Intr_IN  Transmit Interrupt */

/* UDC_INTCON */
#define	UDC_INTEN			(1<<0)		/* UDC Interrupt Enable bit */
#define	UDC_INTEDGE_TRIG	(1<<1)		/* UDC Interrupt Edge Trig bit */
#define	UDC_INTHIGH_ACT		(1<<2)		/* UDC Interrupt Active High Trig bit */

/* UDC_SETUP1 */

/* UDC_SETUP2 */


/* Bulk-OUT End Point Define */

/* RXSTAT (Bulk_OUT, Read-Only) */
#define	RxCNT				(0x7ff)		/* Bulk_OUT DMA Receive Count Mask */
#define	RxVOID				(1<<16)		/* Bulk_OUT Stall Status bit */
#define	RxERR				(1<<17)		/* Bulk_OUT DMA Receive Error bit */
#define	RxACK				(1<<18)		/* Bulk_OUT Bus get ACK (transaction was successful) */
#define	RxFULL				(1<<24)		/* Bulk_OUT Data Buffer Status, 1 -> buffer full */
#define	RxOVF				(1<<25)		/* Bulk_OUT Receive Overflow */

/* RXCON (Bulk_OUT) */
#define	RxCLR				(1<<1)		/* Bulk_OUT Flush FIFO */
#define	RxSTALL				(1<<2)		/* Bulk_OUT Stall */
#define	RxNAK				(1<<3)		/* Bulk_OUT Response NACK */
#define	RxEPEN				(1<<4)		/* Bulk_OUT Enable */
#define	RxVOIDINTEN			(1<<5)		/* Bulk_OUT Voild Interrupt Enable */
#define	RxERRINTEN			(1<<6)		/* Bulk_OUT Error Interrupt Enable */
#define	RxACKINTEN			(1<<7)		/* Bulk_OUT Receive ACK Interrupt Enable */
#define	RxENPNUM			(0x0f00)	/* Bulk_OUT ENP Number Mask */

/* Bulk-IN End Point Define */
/* Intr_IN End Point Define */

/* TXSTAT (Bulk_IN) */
#define	TxCNT				(0x7ff)		/* Bulk_IN DMA Transmit Count Mask */
#define	TxVOID				(1<<16)		/* Bulk_IN Stall Status bit */
#define	TxERR				(1<<17)		/* Bulk_IN DMA Transmit Error bit */
#define	TxACK				(1<<18)		/* Bulk_IN Bus get ACK (transaction was successful) */
#define	TxDMADN				(1<<19)		/* Bulk_IN DMA transmit complete */

/* TXCON (Bulk_IN) */
#define	TxCLR				(1<<0)		/* Bulk_IN Flush FIFO */
#define	TxSTALL				(1<<1)		/* Bulk_IN Stall */
#define	TxNAK				(1<<2)		/* Bulk_IN Response NACK */
#define	TxEPEN				(1<<3)		/* Bulk_IN Enable */
#define	TxVOIDINTEN			(1<<4)		/* Bulk_IN Voild Interrupt Enable */
#define	TxERRINTEN			(1<<5)		/* Bulk_IN Error Interrupt Enable */
#define	TxACKINTEN			(1<<6)		/* Bulk_IN Receive ACK Interrupt Enable */
#define	TxDMADN_EN			(1<<7)		/* Bulk_IN DMA Enable */
#define	TxENPNUM			(0x0f00)	/* Bulk_IN ENP Number Mask */

/* TXBUF (Bulk_IN, Read-Only) */
#define	TxFULL				(1<<0)		/* Bulk_IN Data Buffer Status, 1 -> buffer full */
#define	TxURF				(1<<1)		/* Bulk_IN Transmit Underflow */
#define	TxDS0				(1<<2)		/* Bulk_IN Data Set 0 Status bit, 1 -> buffer full */
#define	TxDS1				(1<<3)		/* Bulk_IN Data Set 1 Status bit, 1 -> buffer full */

/* DMA Status Bit Mask */
#define	ENP_DMASTATUS		(1<<0)
#define	ENP_DMA_START		(1<<0)

/* register	macro */
#define	RETRIEVE_COUNT(x)	((x) & 0x7ff)

/* ivan 040803 */

/* For SET_UDC20_HIGH_SPEED */
// #define SET_UDC20_HIGH_SPEED
//#ifdef SET_UDC20_HIGH_SPEED
     #define CTL_MAX_PKT            0x40
     #define BULK_PKT_LSB           0x00
     #define BULK_PKT_MSB           0x02
     #define INTRIN_PKT_LSB         0x20
     #define INTRIN_PKT_MSB         0x00
//#else
//     #define CTL_MAX_PKT            0x40
     #define FS_BULK_PKT_LSB        0x40
     #define FS_BULK_PKT_MSB        0x00
//     #define FS_INTRIN_PKT_LSB      0x20
//     #define FS_INTRIN_PKT_MSB      0x00
//#endif

/* Endpoint number Config SET 1*/
#define BLKOUT_ENDP_NUM_SET1	    0x01
#define BLKIN_ENDP_NUM_SET1		    0x82
#define INTRIN_ENDP_NUM_SET1	    0x83

/* Endpoint number Config SET 2*/
#define BLKOUT_ENDP_NUM_SET2	    0x04
#define BLKIN_ENDP_NUM_SET2		    0x85
#define INTRIN_ENDP_NUM_SET2	    0x86

/* Endpoint number Config SET 3*/
#define BLKOUT_ENDP_NUM_SET3	    0x07
#define BLKIN_ENDP_NUM_SET3		    0x88
#define INTRIN_ENDP_NUM_SET3	    0x89

/* Endpoint number Config SET 4*/
#define BLKOUT_ENDP_NUM_SET4	    0x0a
#define BLKIN_ENDP_NUM_SET4		    0x8b
#define INTRIN_ENDP_NUM_SET4	    0x8c

/* Endpoint number Config SET 5*/
#define BLKOUT_ENDP_NUM_SET5	    0x0d
#define BLKIN_ENDP_NUM_SET5		    0x8e
#define INTRIN_ENDP_NUM_SET5	    0x8f
/* end of ivan 040803 */

#endif /* _udc_reg_h_ */
