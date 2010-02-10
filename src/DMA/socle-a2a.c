#include <type.h>
#include <io.h>
#include <genlib.h>
#include <dma/dma.h>
#include <interrupt.h>
#include "a2a-regs.h"


static int socle_a2a_request(u32 ch, struct socle_dma *dma);
static void socle_a2a_free(u32 ch, struct socle_dma *dma);
static void socle_a2a_enable(u32 ch, struct socle_dma *dma);
static void socle_a2a_disable(u32 ch, struct socle_dma *dma);
static void socle_a2a_isr_0(void *dma);
static void socle_a2a_isr_1(void *dma);

static u32 socle_a2a_channel[] = {0, 1};

struct socle_dma_ops socle_a2a_ops = {
	.request = socle_a2a_request,
	.free = socle_a2a_free,
	.enable = socle_a2a_enable,
	.disable = socle_a2a_disable,
};


struct socle_dma socle_a2a_channel_0 = {
	.dma_name = "SOCLE A2A Channel 0",
	.base_addr = SOCLE_BASE_A2A,
	.irq = SOCLE_INTC_A2A,
	.private_data = &socle_a2a_channel[0],
	.ops = &socle_a2a_ops,
};

struct socle_dma socle_a2a_channel_1 = {
	.dma_name = "SOCLE A2A Channel 1",
	.base_addr = SOCLE_BASE_A2A,
	.irq = SOCLE_INTC_A2A,
	.private_data = &socle_a2a_channel[1],
	.ops = &socle_a2a_ops,
};

static int
socle_a2a_request(u32 ch, struct socle_dma *dma)
{
	int ret = 0;
	u32 inter_ch = *((u32 *)dma->private_data);

	if (0 == inter_ch) 
		request_irq(dma->irq, socle_a2a_isr_0, dma);
	else if (1 == inter_ch)
		request_irq(dma->irq, socle_a2a_isr_1, dma);
	if (ret)
		printf("A2A: failed to request interrupt\n");
	return ret;
}

static void 
socle_a2a_free(u32 ch, struct socle_dma *dma)
{
	free_irq(dma->irq);
}

static void 
socle_a2a_enable(u32 ch, struct socle_dma *dma)
{
	u32 inter_ch = *((u32 *)dma->private_data);
	u32 conf = SOCLE_A2A_AUTORELOAD_DIS |
		SOCLE_A2A_HW_DMA_DIS |
		SOCLE_A2A_INT_MASK_N |
		SOCLE_A2A_FLY_DIS |
		SOCLE_A2A_TX_MODE_SINGLE |
		SOCLE_A2A_HDREQ0(0) |
		SOCLE_A2A_DIR_SRC_INC |
		SOCLE_A2A_DIR_DST_INC |
		SOCLE_A2A_DATA_SIZE_BYTE |
		SOCLE_A2A_SWDMA_OP_NO |
		SOCLE_A2A_DMA_MODE_SW;

	switch (dma->burst_type) {
	case SOCLE_DMA_BURST_SINGLE:
		conf |= SOCLE_A2A_TX_MODE_SINGLE;
		break;
	case SOCLE_DMA_BURST_INCR4:
		conf |= SOCLE_A2A_TX_MODE_INCR4;
		break;
	case SOCLE_DMA_BURST_INCR8:
		conf |= SOCLE_A2A_TX_MODE_INCR8;
		break;
	case SOCLE_DMA_BURST_INCR16:
		conf |= SOCLE_A2A_TX_MODE_INCR16;
		break;
	}
	conf |= SOCLE_A2A_HDREQ0(dma->ext_hdreq);
	if (SOCLE_DMA_DIR_FIXED == dma->src_dir)
		conf |= SOCLE_A2A_DIR_SRC_FIXED;
	if (SOCLE_DMA_DIR_FIXED == dma->dst_dir)
		conf |= SOCLE_A2A_DIR_DST_FIXED;		
	if (SOCLE_DMA_FLY_WRITE == dma->fly_op)
		conf |= SOCLE_A2A_FLY_EN;
	switch (dma->data_size) {
	case SOCLE_DMA_DATA_BYTE:
		conf |= SOCLE_A2A_DATA_SIZE_BYTE;
		break;
	case SOCLE_DMA_DATA_HALFWORD:
		conf |= SOCLE_A2A_DATA_SIZE_HALFWORD;
		break;
	case SOCLE_DMA_DATA_WORD:
		conf |= SOCLE_A2A_DATA_SIZE_WORD;
		break;
	}	
	if (SOCLE_DMA_MODE_HW == dma->mode){
		conf |= SOCLE_A2A_DMA_MODE_HW;
		conf |= SOCLE_A2A_HW_DMA_EN;
	}else{
		conf |= SOCLE_A2A_DMA_MODE_SW;
		conf |= SOCLE_A2A_SWDMA_OP_START;
	}
	switch (inter_ch) {
	case 0:    
		socle_a2a_write(SOCLE_A2A_ISRC0, dma->src_addr , dma->base_addr);
		socle_a2a_write(SOCLE_A2A_IDST0, dma->dst_addr , dma->base_addr);
		socle_a2a_write(SOCLE_A2A_ICNT0, dma->tx_cnt , dma->base_addr);
		socle_a2a_write(SOCLE_A2A_CON0, conf, dma->base_addr);
		break;
	case 1:
		socle_a2a_write(SOCLE_A2A_ISRC1, dma->src_addr , dma->base_addr);
		socle_a2a_write(SOCLE_A2A_IDST1, dma->dst_addr , dma->base_addr);
		socle_a2a_write(SOCLE_A2A_ICNT1, dma->tx_cnt , dma->base_addr);
		socle_a2a_write(SOCLE_A2A_CON1, conf, dma->base_addr);
		break;
	default:
		printf("A2A: unknown channel number %d\n", ch);
	}
}

static void 
socle_a2a_disable(u32 ch, struct socle_dma *dma)
{
	u32 inter_ch = *((u32 *)dma->private_data);
	u32 tmp;

		/* Clear channel 0 interrupt flag */
	tmp = socle_a2a_read(SOCLE_A2A_INT_STS, dma->base_addr);
	
	switch (inter_ch) {
	case 0:
			/* Clear channel 0 interrupt flag */
		tmp = socle_a2a_read(SOCLE_A2A_INT_STS, dma->base_addr);
			/* Clear channel 0 configuration register */
		socle_a2a_write(SOCLE_A2A_CON0, 0, dma->base_addr);
		socle_a2a_write(SOCLE_A2A_ISRC0, 0 , dma->base_addr);
		socle_a2a_write(SOCLE_A2A_IDST0, 0 , dma->base_addr);
		socle_a2a_write(SOCLE_A2A_ICNT0, 0, dma->base_addr);
		
		break;
	case 1:
			/* Clear channel 1 interrupt flag */
		tmp = socle_a2a_read(SOCLE_A2A_INT_STS, dma->base_addr);
		/* Clear channel 1 configuration register */
		socle_a2a_write(SOCLE_A2A_CON1, 0, dma->base_addr);
		socle_a2a_write(SOCLE_A2A_ISRC1, 0, dma->base_addr);
		socle_a2a_write(SOCLE_A2A_IDST1, 0, dma->base_addr);
		socle_a2a_write(SOCLE_A2A_ICNT1, 0, dma->base_addr);
		
		break;
	default:
		printf("A2A: unknown channel number %d\n", ch);
		return;
	}
}

static void
socle_a2a_isr_0(void *_dma)
{
	u32 int_stat;
	struct socle_dma *dma = _dma;
	int *err = (int *) dma->notifier->data;

	int_stat = socle_a2a_read(SOCLE_A2A_INT_STS, dma->base_addr);
	int_stat &= (SOCLE_A2A_CH0_INT_ACT | 
		     SOCLE_A2A_PRI_BUS_ERR |
		     SOCLE_A2A_SEC_BUS_ERR);

	if (int_stat & SOCLE_A2A_CH0_INT_ACT) {
		if (dma->notifier->complete)
			dma->notifier->complete(dma->notifier->data);
	}
	if (int_stat & SOCLE_A2A_PRI_BUS_ERR) {
		if (dma->notifier->error){
			*err = 1;
			dma->notifier->error(dma->notifier->data);
		}
	}
	if (int_stat & SOCLE_A2A_SEC_BUS_ERR) {
		if (dma->notifier->error){
			*err = 2;
			dma->notifier->error(dma->notifier->data);
		}
	}
}

static void
socle_a2a_isr_1(void *_dma)
{
	u32 int_stat;
	struct socle_dma *dma = _dma;

	int_stat = socle_a2a_read(SOCLE_A2A_INT_STS, dma->base_addr);
	int_stat &= (SOCLE_A2A_CH1_INT_ACT | 
		     SOCLE_A2A_PRI_BUS_ERR |
		     SOCLE_A2A_SEC_BUS_ERR);

	if (int_stat & SOCLE_A2A_CH1_INT_ACT) {
		if (dma->notifier->complete)
			dma->notifier->complete(dma->notifier->data);
	}
	if (int_stat & SOCLE_A2A_PRI_BUS_ERR) {
		if (dma->notifier->error)
			dma->notifier->error(dma->notifier->data);
	}
	if (int_stat & SOCLE_A2A_SEC_BUS_ERR) {
		if (dma->notifier->error)
			dma->notifier->error(dma->notifier->data);
	}
}
