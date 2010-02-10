#include "spi-master.h"

static volatile int socle_spi_tx_complete_flag = 0;
static volatile int socle_spi_rx_complete_flag = 0;
static void *socle_spi_isr_tx_buf = 0;
static u32 socle_spi_isr_tx_buf_idx = 0;
static void *socle_spi_isr_rx_buf = 0;
static u32 socle_spi_isr_rx_buf_idx = 0;
static u32 socle_spi_isr_total_len = 0;
static int current_mode = MODE_CHAR_LEN_8;
static u32 socle_spi_base,socle_irq_num;
static void socle_spi_master_isr(void *pparam);
static u32 socle_spi_power(u32 base, u32 exp);

extern void
socle_spi_write(u32 val, u32 reg)
{
	iowrite32(val, socle_spi_base+reg);
}

extern u32
socle_spi_read(u32 reg)
{
	u32 val;
	val = ioread32(socle_spi_base+reg);
	return val;
}

extern void
socle_spi_master_init(u32 spi_base, u32 irq_num)
{
	socle_spi_base=spi_base;
	socle_irq_num=irq_num;
#ifdef CONFIG_PC9220
	socle_spi_write(
		socle_spi_read(SOCLE_SPI_FWCR) |
		SOCLE_SPI_MODE_MASTER,
		SOCLE_SPI_FWCR);
#endif
	request_irq(socle_irq_num, socle_spi_master_isr, NULL);
}

extern void
socle_spi_master_free()
{
	free_irq(socle_irq_num);
}

extern void
socle_spi_set_current_mode(int mode)
{
	current_mode=mode;
}

extern int
socle_spi_get_current_mode(void)
{
	return current_mode;
}

extern int
socle_spi_transfer(void *tx_buf, void *rx_buf, u32 len)
{	
	u32 tx_cnt = 0, rx_cnt = 0;
	socle_spi_isr_tx_buf_idx = 0;
	socle_spi_isr_rx_buf_idx = 0;
	socle_spi_isr_total_len = GET_TX_LEN(len);

	if (tx_buf != NULL)
		tx_cnt = GET_TX_LEN(len);
	if (rx_buf != NULL)
		rx_cnt = GET_RX_LEN(len);

	socle_spi_isr_tx_buf = tx_buf;
	socle_spi_isr_rx_buf = rx_buf;

	/* Reset the flags */
	socle_spi_tx_complete_flag = 0;
	socle_spi_rx_complete_flag = 0;

	/* Check if SPI controller is running */
	while (SOCLE_SPI_TXRX_RUN == (socle_spi_read(SOCLE_SPI_FWCR) & SOCLE_SPI_TXRX_RUN))
		/* NOP */;

	/* Set transfer & receive data count */
	socle_spi_write(tx_cnt, SOCLE_SPI_TXCR);
	socle_spi_write(rx_cnt, SOCLE_SPI_RXCR);
	
	if (tx_cnt > 0) {
		while (SOCLE_SPI_TXFIFO_FULL != (socle_spi_read(SOCLE_SPI_FCR) & SOCLE_SPI_TXFIFO_FULL)) {
			if (MODE_CHAR_LEN_8 == current_mode)
				socle_spi_write(((u8 *)socle_spi_isr_tx_buf)[socle_spi_isr_tx_buf_idx++], SOCLE_SPI_TXR); 
			else if (MODE_CHAR_LEN_16 == current_mode)
				socle_spi_write(((u16 *)socle_spi_isr_tx_buf)[socle_spi_isr_tx_buf_idx++], SOCLE_SPI_TXR);
			if (socle_spi_isr_tx_buf_idx == socle_spi_isr_total_len) {
				socle_spi_tx_complete_flag = 1;
				break;
			}
		}
	}
	if(socle_spi_tx_complete_flag!=1 && tx_cnt > 0)
		socle_spi_write(socle_spi_read(SOCLE_SPI_IER) | SOCLE_SPI_IER_TXFIFO_INT_EN, SOCLE_SPI_IER);

	/* Start SPI transfer */
	socle_spi_write(
			socle_spi_read(SOCLE_SPI_FWCR) |
			SOCLE_SPI_EN |
			SOCLE_SPI_TXRX_RUN,
			SOCLE_SPI_FWCR);

	/* Wait for transfer to be complete */
	if (rx_cnt > 0) {
		if (socle_wait_for_int(&socle_spi_rx_complete_flag, 30)) {
			printf("Timeout\n");
			return -1;
		}
	} 
	else {
		if (socle_wait_for_int(&socle_spi_tx_complete_flag, 30)) {
			printf("Timeout\n");
			return -1;
		}
	}
	while (SOCLE_SPI_TXRX_RUN == (socle_spi_read(SOCLE_SPI_FWCR) & SOCLE_SPI_TXRX_RUN))
		/* NOP */;
	return 0;
}

static u32
socle_spi_power(u32 base, u32 exp)
{
	u32 i;
	u32 val = 1;
	
	if (0 == exp)
		return 1;
	else {
		for (i = 0; i < exp; i++)
			val *= base;
		return val;
	}
}

extern u32
socle_spi_calculate_divisor(u32 clk)
{
	u8 div_high_3 = 0, div_low_3 = 0, spi_cdvr;
	u32 sclk_divisor, sclk, pclk, power;

	/*
	 *  SCLK Divisor = (SPICDVR[5:3] + 1) * 2 power(SPICDVR[2:0] + 1)
	 *  SCLK = PCLK / SCLK Divisor
	 *  */
	pclk = socle_get_apb_clock();
	while (1) {
		power = socle_spi_power(2, div_low_3+1);
		for (div_high_3 = 0; div_high_3 < 8; div_high_3++) {
			sclk_divisor = (div_high_3 + 1) * power;
			sclk = pclk / sclk_divisor;
			if (sclk < clk)
				goto out;
		}
		div_low_3++;
	}
out:
	spi_cdvr = (div_high_3 << 3) | div_low_3;
	printf("\nsclk is %d, divisor is 0x%08x\n", sclk, spi_cdvr);
	return spi_cdvr;
}

static void 
socle_spi_master_isr(void *pparam)
{
	/* Read & clear interrupt status */
	u32 isr_tmp = socle_spi_read(SOCLE_SPI_ISR);
	
	if (SOCLE_SPI_RXFIFO_OVR_INT == (isr_tmp & SOCLE_SPI_RXFIFO_OVR_INT)) {
		socle_spi_tx_complete_flag = -1;
		socle_spi_rx_complete_flag = -1;
		printf("\nReceive FIFO is full and another character has been received in the receiver shift register\n");
		return;
	}
	/* Check is receive complete */
	if (SOCLE_SPI_RX_COMPLETE_INT == (isr_tmp & SOCLE_SPI_RX_COMPLETE_INT)) {
		while (SOCLE_SPI_RXFIFO_DATA_AVAIL == (socle_spi_read(SOCLE_SPI_FCR) & SOCLE_SPI_RXFIFO_DATA_AVAIL)) {
			if (MODE_CHAR_LEN_8 == current_mode)
				((u8 *)socle_spi_isr_rx_buf)[socle_spi_isr_rx_buf_idx++] = socle_spi_read(SOCLE_SPI_RXR);
			else if (MODE_CHAR_LEN_16 == current_mode)
				((u16 *)socle_spi_isr_rx_buf)[socle_spi_isr_rx_buf_idx++] = socle_spi_read(SOCLE_SPI_RXR);		
		}
		socle_spi_rx_complete_flag = 1;
		return;
	}
	/* Check if any receive data is available */
	if (SOCLE_SPI_RXFIFO_INT == (isr_tmp & SOCLE_SPI_RXFIFO_INT)) {
		while (SOCLE_SPI_RXFIFO_DATA_AVAIL == (socle_spi_read(SOCLE_SPI_FCR) & SOCLE_SPI_RXFIFO_DATA_AVAIL)) {
			if (MODE_CHAR_LEN_8 == current_mode)
				((u8 *)socle_spi_isr_rx_buf)[socle_spi_isr_rx_buf_idx++] = socle_spi_read(SOCLE_SPI_RXR);
			else if (MODE_CHAR_LEN_16 == current_mode) {
				((u16 *)socle_spi_isr_rx_buf)[socle_spi_isr_rx_buf_idx++] = socle_spi_read(SOCLE_SPI_RXR);
			}
		}
		return;
	}
	/* Check if the transmit FIFO is available */
	if (SOCLE_SPI_TXFIFO_INT == (isr_tmp & SOCLE_SPI_TXFIFO_INT)) {
		while (SOCLE_SPI_TXFIFO_FULL != (socle_spi_read(SOCLE_SPI_FCR) & SOCLE_SPI_TXFIFO_FULL)) {
			if (MODE_CHAR_LEN_8 == current_mode)
				socle_spi_write(((u8 *)socle_spi_isr_tx_buf)[socle_spi_isr_tx_buf_idx++], SOCLE_SPI_TXR);
			else if (MODE_CHAR_LEN_16 == current_mode)
				socle_spi_write(((u16 *)socle_spi_isr_tx_buf)[socle_spi_isr_tx_buf_idx++], SOCLE_SPI_TXR);
			if (socle_spi_isr_tx_buf_idx == socle_spi_isr_total_len) {
				socle_spi_write(socle_spi_read(SOCLE_SPI_IER) & ~SOCLE_SPI_IER_TXFIFO_INT_EN, SOCLE_SPI_IER);
				socle_spi_tx_complete_flag = 1;
				return;
			}
		}
		return;
	}
	
}

extern void
socle_spi_make_test_pattern(u8 *buf, u32 size)
{
	int i;    
	for (i = 0; i < size; i++)
		buf[i] = i;
}



extern int
socle_spi_compare_memory(u8 *mem, u8 *cmpr_mem, u32 size, int skip_cmpr_result)
{
	int i;
	u32 *mem_32 = (u32 *)mem;
	u32 *cmpr_mem_32 = (u32 *)cmpr_mem;
	int err_flag = 0;

	for (i = 0; i < (size >> 2); i++) {
		if (mem_32[i] != cmpr_mem_32[i]) {
			err_flag |= -1;
			if (!skip_cmpr_result)
				printf("\nWord %d, 0x%08x (0x%08x) != 0x%08x (0x%08x)", i, mem_32[i], &mem_32[i], 
				       cmpr_mem_32[i], &cmpr_mem_32[i]);
		}
	}
	return err_flag;
}
