#ifndef __A2A_REGS_H_INCLUDED
#define __A2A_REGS_H_INCLUDED

#include <type.h>
#include <platform.h>
#include <irqs.h>

/*	A2A dependence	*/
#ifndef SOCLE_AHB0_A2A_DMA
#define SOCLE_AHB0_A2A_DMA -1
#endif

#ifndef SOCLE_INTC_A2A_bri_DMA
#define SOCLE_INTC_A2A_bri_DMA -1
#endif

#define SOCLE_BASE_A2A SOCLE_AHB0_A2A_DMA
#define SOCLE_INTC_A2A SOCLE_INTC_A2A_bri_DMA

/*
 *  Register for A2A
 *  */
#define SOCLE_A2A_CON0		0x0000	/* A2A channel 0 control register */
#define SOCLE_A2A_ISRC0		0x0004	/* A2A channel 0 initial source address register */
#define SOCLE_A2A_IDST0		0x0008	/* A2A channel 0 initial destination address register */
#define SOCLE_A2A_ICNT0		0x000C	/* A2A channel 0 initial terminate count register */
#define SOCLE_A2A_CSRC0		0x0010	/* A2A channel 0 current source address register */
#define SOCLE_A2A_CDST0		0x0014	/* A2A channel 0 current destination address register */
#define SOCLE_A2A_CCNT0		0x0018	/* A2A channel 0 current count register */
#define SOCLE_A2A_CON1		0x001C	/* A2A channel 1 control register */
#define SOCLE_A2A_ISRC1		0x0020	/* A2A channel 1 initial source address register */
#define SOCLE_A2A_IDST1		0x0024	/* A2A channel 1 initial destination address register */
#define SOCLE_A2A_ICNT1		0x0028	/* A2A channel 1 initial terminate count register */
#define SOCLE_A2A_CSRC1		0x002C	/* A2A channel 1 current source address register */
#define SOCLE_A2A_CDST1		0x0030	/* A2A channel 1 current destination address register */
#define SOCLE_A2A_CCNT1		0x0034	/* A2A channel 1current count register */
#define SOCLE_A2A_INT_STS	0x0038	/* A2A DMA interrupt status register */
#define SOCLE_A2A_DMA_STS	0x003C	/* A2A DMA channel status register */
#define SOCLE_A2A_ERR_ADR1	0x0040	/* Address of primary bus error */
#define SOCLE_A2A_ERR_OP1	0x0044	/* Operation of primary bus error */
#define SOCLE_A2A_ERR_ADR2	0x0048	/* Address of secondary bus error */
#define SOCLE_A2A_ERR_OP2	0x004C	/* Operation of secondary bus error */
#define SOCLE_A2A_LCNT0		0x0050	/* A2A channel 0 on the fly mode AHB bus Lock Count register */
#define SOCLE_A2A_LCNT1		0x0054	/* A2A channel 1 on the fly mode AHB bus Lock Count register */
#define SOCLE_A2A_DOMAIN	0x0058	/* A2A channel 0 on the fly mode AHB bus Lock Count register */


static inline void
socle_a2a_write(u32 reg, u32 value, u32 base) 
{
		//iowrite32(value, base+reg);
		writew(value, (base + reg));
}

static inline u32
socle_a2a_read(u32 reg, u32 base)
{
		//ioread32(base+reg);
		return readw(base + reg);
}

/*
 *  A2A_CONx
 *  */
/* Auto-reload */
#define SOCLE_A2A_AUTORELOAD_DIS 0x0 /* disable */
#define SOCLE_A2A_AUTORELOAD_EN (0x1 << 14) /* Enable */

/* DMA HW enable/disable */
#define SOCLE_A2A_HW_DMA_DIS 0x0 /* disable */
#define SOCLE_A2A_HW_DMA_EN (0x1 << 13) /* Enable */

/* Interrupt Mask */
#define SOCLE_A2A_INT_MASK	   0x0	/* polling mode */
#define SOCLE_A2A_INT_MASK_N (0x1 << 12)	/* interrupt mode */

/* On the fly mode */
#define SOCLE_A2A_FLY_DIS 0x0	/* disable on the fly */
#define SOCLE_A2A_FLY_EN (0x1 << 11) /* enable on the fly */

/* Transfer Mode */
#define SOCLE_A2A_TX_MODE_SINGLE 0x0	/* Single */
#define SOCLE_A2A_TX_MODE_INCR4 (0x1 << 9) /* INCR4 */
#define SOCLE_A2A_TX_MODE_INCR8 (0x2 << 9) /* INCR8 */
#define SOCLE_A2A_TX_MODE_INCR16 (0x3 << 9) /* INCR16 */

/* External HDREQ source selection */
#define SOCLE_A2A_HDREQ0(x) (((x) & 0x3) << 7)


/* Direction of source address */
#define SOCLE_A2A_DIR_SRC_INC 0x0	/* increment */
#define SOCLE_A2A_DIR_SRC_FIXED (0x1 << 6) /* fixed */

/* Direction of destination address */
#define SOCLE_A2A_DIR_DST_INC 0x0	/* increment */
#define SOCLE_A2A_DIR_DST_FIXED (0x1 << 5) /* fixed */

/* Command of Software DMA operation */
#define SOCLE_A2A_SWDMA_OP_NO 0x0	/* no command */
#define SOCLE_A2A_SWDMA_OP_START (0x1 << 3) /* start software DMA operation */
#define SOCLE_A2A_SWDMA_OP_PAUSE (0x2 << 3) /* pause software DMA operation */
#define SOCLE_A2A_SWDMA_OP_CANCEL (0x3 << 3)	/* cancel software DMA operation */

#define SOCLE_A2A_SWDMA_OP_MASK (0x3 << 3)	/* Mask of Software DMA operation */

/* Data size for transfer */
#define SOCLE_A2A_DATA_SIZE_BYTE 0x0	/* byte */
#define SOCLE_A2A_DATA_SIZE_HALFWORD (0x1 << 1) /* halfword */
#define SOCLE_A2A_DATA_SIZE_WORD (0x2 << 1) /* word */

/* DMA mode */
#define SOCLE_A2A_DMA_MODE_HW 0x0 /* HW mode */
#define SOCLE_A2A_DMA_MODE_SW 0x1 /* SW mode */

/*
 *  A2A_ISRCx
 *  */
/* A2A initial source address register for channel x */

/*
 *  A2A_CSRCx
 *  */
/* A2A current source address register for channel x */

/*
 *  A2A_IDSTx
 *  */
/* A2A initial destination address register for channel x */

/*
 *  
 *  A2A_CDSTx
 *  */
/* A2A current destination address register for channel x */

/*
 *  A2A_ICNTx
 *  */
/* A2A current terminate count register for channel x */

/* 
 * A2A_CCNTx
 * */
/* A2A current terminate count register for channel x */

/*
 *  A2A_INT_STS
 *  */
/* Secondary BUS (AHB_2) Error Interrupt active, clear after read */
#define SOCLE_A2A_SEC_BUS_ERR_N 0x0 /* no bus error mask */
#define SOCLE_A2A_SEC_BUS_ERR  (0x1 << 3) /* bus error interrupt active */

/* Primary BUS (AHB_1) Error Interrupt active, clear after read */
#define SOCLE_A2A_PRI_BUS_ERR_N 0x0 /* no bus error mask */
#define SOCLE_A2A_PRI_BUS_ERR  (0x1 << 2) /* bus error interrupt active */

/* Channel 1 Interrupt active, clear interrupt after read */
#define SOCLE_A2A_CH1_INT_ACT_N 0x0	/* not active */
#define SOCLE_A2A_CH1_INT_ACT  (0x1 << 1) /* active */

/* Channel 0 Interrupt active, clear interrupt after read */
#define SOCLE_A2A_CH0_INT_ACT_N  0x0	/* not active */
#define SOCLE_A2A_CH0_INT_ACT    0x1	/* active */

/*
 *  A2A_DMA_STS
 *  */
/* Channel 1 DMA status */
#define SOCLE_A2A_CH1_STAT_READY		0x0			/* channel1 is ready */
#define SOCLE_A2A_CH1_STAT_READY_N	(0x1 << 1)	/* channel1 is performing DMA */

/* Channel 0 DMA status */
#define SOCLE_A2A_CH0_STAT_READY		0x0			/* channel0 is ready */
#define SOCLE_A2A_CH0_STAT_READY_N	0x1			/* channel0 is performing DMA */


/*
 *  A2A_ERR_ADDRx
 *  */
/* Address when primary/secondary bus error */

/*
 *  A2A_ERR_OPx
 *  */
/* Operation when primary/secondary bus error */
#define SOCLE_A2A_BUS_ERR_READ		0x0			/* Bus error when read operation */
#define SOCLE_A2A_BUS_ERR_WRITE		0x1			/* Bus error when write operation */

/*
 * A2A_LCNTx
 * */
/* Bus lock counts at on-the-fly mode */
#define SOCLE_A2A_BUS_RELEASE_NEVER		0x0			/* Never release bus until DMA process end */
#define SOCLE_A2A_BUS_RELEASE_16		0x1			/* Bus Release every 16-beats data transfer */
#define SOCLE_A2A_BUS_RELEASE_32		0x2			/* Bus Release every 32-beats data transfer */
#define SOCLE_A2A_BUS_RELEASE_64		0x3			/* Bus Release every 64-beats data transfer */
#define SOCLE_A2A_BUS_RELEASE_128		0x4			/* Bus Release every 128-beats data transfer */
#define SOCLE_A2A_BUS_RELEASE_256		0x5			/* Bus Release every 256-beats data transfer */
#define SOCLE_A2A_BUS_RELEASE_512		0x6			/* Bus Release every 512-beats data transfer */
#define SOCLE_A2A_BUS_RELEASE_1024		0x7			/* Bus Release every 1024-beats data transfer */

/*
 * A2A_DOMAIN
 * */
/* A2A DMA source and destination domain selection */
#define SOCLE_A2A_DST_DOMAIN_AHB0		0x0			/* Destination address is on AHB0 */
#define SOCLE_A2A_DST_DOMAIN_AHB1		(0x1 << 1)	/* Destination address is on AHB1 */
#define SOCLE_A2A_SRC_DOMAIN_AHB0		0x0			/* Source address is on AHB0 */
#define SOCLE_A2A_SRC_DOMAIN_AHB1		0x1			/* Source address is on AHB1 */

#define SOCLE_A2A_DST_DOMAIN_MASK		(0x1 << 1)	
#define SOCLE_A2A_SRC_DOMAIN_MASK		0x1			

#endif
