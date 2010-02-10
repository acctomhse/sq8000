#ifndef __HDMA_REGS_H_INCLUDED
#define __HDMA_REGS_H_INCLUDED

#include <io.h>
#include <type.h>
#include <platform.h>
#include <irqs.h>

/*	HDMA dependence	*/
#ifndef SOCLE_AHB0_HDMA
#define SOCLE_AHB0_HDMA -1
#endif

#ifndef SOCLE_INTC_HDMA
#define SOCLE_INTC_HDMA -1
#endif

#ifdef SOCLE_AHB0_HDMA
#define SOCLE_AHB0_HDMA0 SOCLE_AHB0_HDMA
#endif
#ifndef SOCLE_AHB0_HDMA1
#define SOCLE_AHB0_HDMA1 SOCLE_AHB0_HDMA0
#endif
#ifndef SOCLE_AHB0_HDMA2
#define SOCLE_AHB0_HDMA2 SOCLE_AHB0_HDMA0
#endif
#ifndef SOCLE_AHB0_HDMA3
#define SOCLE_AHB0_HDMA3 SOCLE_AHB0_HDMA0
#endif

#ifdef SOCLE_INTC_HDMA
#define SOCLE_INTC_HDMA0 SOCLE_INTC_HDMA
#endif
#ifndef SOCLE_INTC_HDMA1
#define SOCLE_INTC_HDMA1 SOCLE_INTC_HDMA
#endif
#ifndef SOCLE_INTC_HDMA2
#define SOCLE_INTC_HDMA2 SOCLE_INTC_HDMA
#endif
#ifndef SOCLE_INTC_HDMA3
#define SOCLE_INTC_HDMA3 SOCLE_INTC_HDMA
#endif

/*
 *  Register for HDMA
 *  */
#define SOCLE_HDMA_CON0			0x0000	/* HDMA channel 0 control register */
#define SOCLE_HDMA_CON1			0x0004	/* HDMA channel 1 control register */
#define SOCLE_HDMA_ISRC0		0x0008	/* HDMA channel 0 initial source address register */
#define SOCLE_HDMA_IDST0		0x000C	/* HDMA channel 0 initial destination address register */
#define SOCLE_HDMA_ICNT0		0x0010	/* HDMA channel 0 initial terminate count register */
#define SOCLE_HDMA_ISRC1		0x0014	/* HDMA channel 1 initial source address register */
#define SOCLE_HDMA_IDST1		0x0018	/* HDMA channel 1 initial destination address register */
#define SOCLE_HDMA_ICNT1		0x001C	/* HDMA channel 1 initial terminate count register */
#define SOCLE_HDMA_CSRC0		0x0020	/* HDMA channel 0 current source address register */
#define SOCLE_HDMA_CDST0		0x0024	/* HDMA channel 0 current destination address register */
#define SOCLE_HDMA_CCNT0		0x0028	/* HDMA channel 0 current count register */
#define SOCLE_HDMA_CSRC1		0x002C	/* HDMA channel 1 current source address register */
#define SOCLE_HDMA_CDST1		0x0030	/* HDMA channel 1 current destination address register */
#define SOCLE_HDMA_CCNT1		0x0034	/* HDMA channel 1current count register */
#define SOCLE_HDMA_ISR01		0x0038	/* HDMA DMA interrupt ch0/1 status register */
#define SOCLE_HDMA_DSR01		0x003C	/* HDMA DMA ch0/1 status register */
#define SOCLE_HDMA_ISCNT0		0x0040	/* HDMA channel 0 initial slice count register */
#define SOCLE_HDMA_IPNCNTD		0x0044	/* HDMA channel 0 initial page number count down */
#define SOCLE_HDMA_IADDR_BS0	0x0048	/* HDMA channel 0 initial address buffer size register */
#define SOCLE_HDMA_ISCNT1		0x004C	/* HDMA channel 1 initial slice count register */
#define SOCLE_HDMA_IPNCNT1		0x0050	/* HDMA channel 1 initial page number count down */
#define SOCLE_HDMA_IADDR_BS1	0x0054	/* HDMA channel 1 initial address buffer size register */
#define SOCLE_HDMA_CSCNT0		0x0058	/* HDMA channel 0 current slice count register */
#define SOCLE_HDMA_CPNCNTD0		0x005C	/* HDMA channel 0 current page number count down register */
#define SOCLE_HDMA_CADDR_BS0	0x0060	/* HDMA channel 0 current address buffer size register */
#define SOCLE_HDMA_CSCNT1		0x0064	/* HDMA channel 1 current slice count register */
#define SOCLE_HDMA_CPNCNTD1		0x0068	/* HDMA channel 1 current page number count down register */
#define SOCLE_HDMA_CADDR_BS1	0x006C	/* HDMA channel 1 current address buffer size register */
#define SOCLE_HDMA_PACNT0		0x0070	/* HDMA channel 0 page accumulation count register */
#define SOCLE_HDMA_PACNT1		0x0074	/* HDMA channel 1 page accumulation count register */
#define SOCLE_HDMA_PRIO			0x0078	/* HDMA arbitration priority mode */

/*	channel 2 & channel 3	*/
#define SOCLE_HDMA_CON2			0x1000	/* HDMA channel 2 control register */
#define SOCLE_HDMA_CON3			0x1004	/* HDMA channel 3 control register */
#define SOCLE_HDMA_ISRC2		0x1008	/* HDMA channel 2 initial source address register */
#define SOCLE_HDMA_IDST2		0x100C	/* HDMA channel 2 initial destination address register */
#define SOCLE_HDMA_ICNT2		0x1010	/* HDMA channel 2 initial terminate count register */
#define SOCLE_HDMA_ISRC3		0x1014	/* HDMA channel 3 initial source address register */
#define SOCLE_HDMA_IDST3		0x1018	/* HDMA channel 3 initial destination address register */
#define SOCLE_HDMA_ICNT3		0x101C	/* HDMA channel 3 initial terminate count register */
#define SOCLE_HDMA_CSRC2		0x1020	/* HDMA channel 2 current source address register */
#define SOCLE_HDMA_CDST2		0x1024	/* HDMA channel 2 current destination address register */
#define SOCLE_HDMA_CCNT2		0x1028	/* HDMA channel 2 current count register */
#define SOCLE_HDMA_CSRC3		0x102C	/* HDMA channel 3 current source address register */
#define SOCLE_HDMA_CDST3		0x1030	/* HDMA channel 3 current destination address register */
#define SOCLE_HDMA_CCNT3		0x1034	/* HDMA channel 3 current count register */
#define SOCLE_HDMA_ISR23		0x1038	/* HDMA DMA interrupt ch2/3 status register */
#define SOCLE_HDMA_DSR23		0x103C	/* HDMA DMA ch2/3 status register */

static inline void
socle_hdma_write(u32 reg, u32 value, u32 base) 
{
		iowrite32(value, base+reg);
}

static inline u32
socle_hdma_read(u32 reg, u32 base)
{
		return ioread32(base+reg);
}

/*
 *  HDMA_CONx
 *  */
/* Clear CPNCNTDx register */
#define SOCLE_HDMA_CPNCNTD_CLR_N 0x0	/* not clear */
#define SOCLE_HDMA_CPNCNTD_CLR (0x1 << 23)	/* clear */

/* Hardware HDMA turnaround enable/disable */
#define SOCLE_HDMA_HWDMA_TURNAROUND_DIS 0x0	/* disable */
#define SOCLE_HDMA_HWDMA_TURNAROUND_EN (0x1 << 22) /* enable */

/* HDMA channel enable/disable */
#define SOCLE_HDMA_CH_DIS 0x0	/* disable */
#define SOCLE_HDMA_CH_EN (0x1 << 21) /* enable */

/* Auto-reload and Auto-start after DMA count is 0 */
#define SOCLE_HDMA_AUTORELOAD_DIS 0x0 /* disable */
#define SOCLE_HDMA_AUTORELOAD_EN (0x1 << 20) /* Enable */

/* Interrupt Mode Set */
#define SOCLE_HDMA_INT_MODE_POLL 0x0	/* polling mode */
#define SOCLE_HDMA_INT_MODE_INT (0x1 << 18)	/* interrupt mode */

/* On the fly mode */
#define SOCLE_HDMA_FLY_DIS	 0x0	/* disable on the fly */
#define SOCLE_HDMA_FLY_READ (0x1 << 16) /* Read on the fly(read from OFM bus) */
#define SOCLE_HDMA_FLY_WRITE (0x2 << 16) /* Write on the fly(write to OFM bus) */

/* Transfer Mode */
#define SOCLE_HDMA_TX_MODE_SINGLE 0x0	/* Single */
#define SOCLE_HDMA_TX_MODE_INCR4 (0x3 << 13) /* INCR4 */
#define SOCLE_HDMA_TX_MODE_INCR8 (0x5 << 13) /* INCR8 */
#define SOCLE_HDMA_TX_MODE_INCR16 (0x7 << 13) /* INCR16 */

/* External HDREQ source selection */
#define SOCLE_HDMA_HDREQ0(x) (((x) & 0xf) << 9)

#define SOCLE_HDMA_HDREQ_UART0 0x0			/* from UART0 */
#define SOCLE_HDMA_HDREQ_UART1 (0x1 << 9)		/* from UART1 */
#define SOCLE_HDMA_HDREQ_UART2 (0x2 << 9)		/* from UART2 */
#define SOCLE_HDMA_HDREQ_SPI0  (0x3 << 9)		/* from SPI0 */
#define SOCLE_HDMA_HDREQ_SPI1  (0x4 << 9)		/* from SPI1 */
#define SOCLE_HDMA_HDREQ_SD_MMC (0x5 << 9)	/* from SD/MMC */
#define SOCLE_HDMA_HDREQ_MP0   (0x6 << 9)		/* from Metal-Programmable block 0 */
#define SOCLE_HDMA_HDREQ_MP1   (0x7 << 9)		/* from Metal-Programmable block 1 */

/* Direction of source address */
#define SOCLE_HDMA_DIR_SRC_INC 0x0	/* increment */
#define SOCLE_HDMA_DIR_SRC_FIXED (0x1 << 7) /* fixed */

/* Direction of destination address */
#define SOCLE_HDMA_DIR_DST_INC 0x0	/* increment */
#define SOCLE_HDMA_DIR_DST_FIXED (0x1 << 5) /* fixed */

/* Data size for transfer */
#define SOCLE_HDMA_DATA_SIZE_BYTE 0x0	/* byte */
#define SOCLE_HDMA_DATA_SIZE_HALFWORD (0x1 << 3) /* halfword */
#define SOCLE_HDMA_DATA_SIZE_WORD (0x2 << 3) /* word */

/* Command of Software DMA operation */
#define SOCLE_HDMA_SWDMA_OP_NO 0x0	/* no command */
#define SOCLE_HDMA_SWDMA_OP_START (0x1 << 1) /* start software DMA operation */
#define SOCLE_HDMA_SWDMA_OP_PAUSE (0x2 << 1) /* pause software DMA operation */
#define SOCLE_HDMA_SWDMA_OP_CANCEL (0x3 << 1)	/* cancel software DMA operation */

#define SOCLE_HDMA_SWDMA_OP_MASK (0x3 << 1)	/* Mask of Software DMA operation */

/* Disable / Enable hardware trigger DMA mode */
#define SOCLE_HDMA_HWDMA_TRIGGER_DIS 0x0 /* disable */
#define SOCLE_HDMA_HWDMA_TRIGGER_EN	 0x1 /* enable */

/*
 *  HDMA_ISRCx
 *  */
/* HDMA initial source address register for channel x */

/*
 *  HDMA_CSRCx
 *  */
/* HDMA current source address register for channel x */

/*
 *  HDMA_IDSTx
 *  */
/* HDMA initial destination address register for channel x */

/*
 *  
 *  HDMA_CDSTx
 *  */
/* HDMA current destination address register for channel x */

/*
 *  HDMA_ICNTx
 *  */
/* HDMA current terminate count register for channel x */

/* 
 * HDMA_CCNTx
 * */
/* HDMA current terminate count register for channel x */

/*
 *  HDMA_ISR
 *  */
/* Mask channel1 Page Interrupt */
#define SOCLE_HDMA_CH1_PAGE_INT_MASK_N 0x0 /* not mask */
#define SOCLE_HDMA_CH1_PAGE_INT_MASK  (0x1 << 7) /* mask */

/* Mask channel0 Page Interrupt */
#define SOCLE_HDMA_CH0_PAGE_INT_MASK_N 0x0 /* not mask */
#define SOCLE_HDMA_CH0_PAGE_INT_MASK  (0x1 << 6) /* mask */

/* Mask channel1 Interrupt */
#define SOCLE_HDMA_CH1_INT_MASK_N 0x0 /* not mask */
#define SOCLE_HDMA_CH1_INT_MASK  (0x1 << 5) /* mask */

/* Mask channel0 Interrupt */
#define SOCLE_HDMA_CH0_INT_MASK_N 0x0 /* not mask */
#define SOCLE_HDMA_CH0_INT_MASK  (0x1 << 4) /* mask */

/* Channel 1 Page Interrupt active, write "0" to clear intterupt
 * and write "1" is no affect*/
#define SOCLE_HDMA_CH1_PAGE_INT_ACT_N 0x0	/* not active */
#define SOCLE_HDMA_CH1_PAGE_INT_ACT  (0x1 << 3) /* active */

/* Channel 0 Page Interrupt active, write "0" to clear intterupt
 * and write "1" is no affect*/
#define SOCLE_HDMA_CH0_PAGE_INT_ACT_N 0x0	/* not active */
#define SOCLE_HDMA_CH0_PAGE_INT_ACT  (0x1 << 2) /* active */

/* Channel 1 Interrupt active, write "0" to clear intterupt
 * and write "1" is no affect*/
#define SOCLE_HDMA_CH1_INT_ACT_N   0x0	/* not active */
#define SOCLE_HDMA_CH1_INT_ACT    (0x1 << 1) /* active */
#define SOCLE_HDMA_CH1_INT_S	0x0	/* channel 1 interrupt status shift */

/* Channel 0 Interrupt active, write "0" to clear intterupt
 * and write "1" is no affect*/
#define SOCLE_HDMA_CH0_INT_ACT_N 0x0	/* not active */
#define SOCLE_HDMA_CH0_INT_ACT   0x1	/* active */
#define SOCLE_HDMA_CH0_INT_S	0x0	/* channel 0 interrupt status shift */

/* Channel 3 Interrupt active, write "0" to clear intterupt
 * and write "1" is no affect*/
#define SOCLE_HDMA_CH3_INT_ACT_N   0x0	/* not active */
#define SOCLE_HDMA_CH3_INT_ACT    (0x1 << 1) /* active */
#define SOCLE_HDMA_CH3_INT_S	0x0	/* channel 3 interrupt status shift */

/* Channel 2 Interrupt active, write "0" to clear intterupt
 * and write "1" is no affect*/
#define SOCLE_HDMA_CH2_INT_ACT_N   0x0	/* not active */
#define SOCLE_HDMA_CH2_INT_ACT     0x1	/* active */
#define SOCLE_HDMA_CH2_INT_S	0x0	/* channel 2 interrupt status shift */

/*
 *  HDMA_DSR
 *  */
/* Channel 1 status */
#define SOCLE_HDMA_CH1_STAT_READY	   	0x0			/* channel1 is ready */
#define SOCLE_HDMA_CH1_STAT_READY_N	   (0x1 << 1)	/* channel1 is performing DMA */

/* Channel 0 status */
#define SOCLE_HDMA_CH0_STAT_READY		0x0			/* channel0 is ready */
#define SOCLE_HDMA_CH0_STAT_READY_N	    0x1			/* channel0 is performing DMA */


/* Channel 3 status */
#define SOCLE_HDMA_CH3_STAT_READY		0x0			/* channel3 is ready */
#define SOCLE_HDMA_CH3_STAT_READY_N	   (0x1 << 1)	/* channel3 is performing DMA */

/* Channel 2 status */
#define SOCLE_HDMA_CH2_STAT_READY		0x0			/* channel2 is ready */
#define SOCLE_HDMA_CH2_STAT_READY_N	    0x1			/* channel2 is performing DMA */


/*
 *  HDMA_ISCNTx
 *  */
/* HDMA initial slice count register for channel x */

/*
 *  HDMA_CSCNTx
 *  */
/* HDMA current slice count register for channel x */

/*
 * HDMA_IPNCNTDx
 * */
/* HDMA initial total page number count down register */
/* The value will be accumulated if you write it again and again */

/*
 *  HDMA_CPNCNTDx
 *  */
/* HDMA current total page number count down regiter */

/*
 *  HDMA_CADDR_BSx
 *  */
/* HDMA current source address buffer size register */

/*
 *  HDMA_PACNTx
 *  */
/* HDMA page accumulation count register */

/*
 *  HDMA_PRIO
 *  */
/* HDMA arbiter priority mode */
#define SOCLE_HDMA_PRIORITY_CH0_HIGH 0x0 /* channel 0 > channel 1 */
#define SOCLE_HDMA_PRIORITY_CH1_HIGH (0x1 < 1) /* channel 1 > channel 0 */

/* HDMA arbiter priority mode */
#define SOCLE_HDMA_PRIORITY_MODE_ROUND_ROBIN 0x0 /* round-robin */
#define SOCLE_HDMA_PRIORITY_MODE_FIXED_ARBITRATION /* fixed arbitration */

#endif
