#include <test_item.h>
#include <global.h>
#include <dma/dma.h>
#include "spi-master.h"



/*function*/
static void socle_spi_tx_dma_page_interrupt(void *data);
static void socle_spi_rx_dma_page_interrupt(void *data);
static int socle_spi_master_internal_hdma_run(int autotest);

/*variable*/


static volatile int socle_spi_tx_complete_flag = 0;
static volatile int socle_spi_rx_complete_flag = 0;
static u32 socle_spi_tx_dma_ch_num, socle_spi_rx_dma_ch_num;
static u32 socle_spi_tx_dma_ext_hdreq, socle_spi_rx_dma_ext_hdreq;
static u32 socle_spi_base,socle_spi_dma_burst,char_len,lsb;
static u8 cpha,cpol,pure,regflag;
static u8 *socle_spi_pattern_buf = (u8 *)PATTERN_BUF_ADDR;
static u8 *socle_spi_cmpr_buf = (u8 *)(PATTERN_BUF_ADDR + PATTERN_BUF_SIZE);

static struct socle_dma_notifier socle_spi_tx_dma_notifier = {
	.page_interrupt = socle_spi_tx_dma_page_interrupt,
};

static struct socle_dma_notifier socle_spi_rx_dma_notifier = {
	.page_interrupt = socle_spi_rx_dma_page_interrupt,
};

extern struct test_item_container socle_spi_test_container;

extern int
SPITesting(int autotest)
{
	
	int ret = 0;
	ret = test_item_ctrl(&socle_spi_test_container, autotest);
	return ret;
}

extern struct test_item_container socle_spi_master_test_container;

extern int
socle_spi_master_test(int autotest)
{
	int ret = 0;
	ret = test_item_ctrl(&socle_spi_master_test_container, autotest);
	return ret;
}

extern struct test_item_container socle_spi_transfer_test_container;

extern int socle_spi_master0_test(int autotest)
{
	int ret = 0;
	
	{
		extern struct test_item socle_spi_transfer_test_items[];
		
		socle_spi_transfer_test_items[1].enable = SOCLE_SPI_EEPROM_0_TEST;
		socle_spi_transfer_test_items[2].enable = SOCLE_SPI_MARVEL_WIFI_0_TEST;
		socle_spi_transfer_test_items[3].enable = SOCLE_SPI_TSC2000_0_TEST;
	}	
	socle_spi_base = SOCLE_APB0_SPI0;
#ifdef CONFIG_PC9220
		socle_scu_dev_enable(SOCLE_DEVCON_SPI0);
		socle_scu_hdma_req45_spi0();
#endif
	socle_spi_master_init(socle_spi_base,SOCLE_INTC_SPI0);
		
	ret = test_item_ctrl(&socle_spi_transfer_test_container, autotest);
#ifdef CONFIG_PC9220
	socle_scu_dev_disable(SOCLE_DEVCON_SPI0);
#endif
	socle_spi_master_free();
	return ret;
}

extern int socle_spi_master1_test(int autotest)
{
	int ret = 0;
	{
		extern struct test_item socle_spi_transfer_test_items[];
		
		socle_spi_transfer_test_items[1].enable = SOCLE_SPI_EEPROM_1_TEST;
		socle_spi_transfer_test_items[2].enable = SOCLE_SPI_MARVEL_WIFI_1_TEST;
		socle_spi_transfer_test_items[3].enable = SOCLE_SPI_TSC2000_1_TEST;
	}
	socle_spi_base = SOCLE_APB0_SPI1;
#ifdef CONFIG_PC9220
		socle_scu_dev_enable(SOCLE_DEVCON_SPI1);
		socle_scu_hdma_req45_spi1();
#endif
	socle_spi_master_init(socle_spi_base,SOCLE_INTC_SPI1);
	ret = test_item_ctrl(&socle_spi_transfer_test_container, autotest);
#ifdef CONFIG_PC9220
	socle_scu_dev_disable(SOCLE_DEVCON_SPI1);
#endif
	socle_spi_master_free();	
	return ret;
}

extern struct test_item_container socle_spi_internal_test_container;

extern int 
socle_spi_internal_test(int autotest)
{
	int ret = 0;
	ret = test_item_ctrl(&socle_spi_internal_test_container, autotest);
	return ret;
}

extern struct test_item_container socle_spi_hdma_burst_type_test_container;

extern int
socle_spi_internal_hdma_test(int autotest)
{
	int ret = 0;	
	socle_spi_tx_dma_ext_hdreq = 5;
	socle_spi_rx_dma_ext_hdreq = 4;

	socle_spi_tx_dma_ch_num = PANTHER7_HDMA_CH_1;
	socle_spi_rx_dma_ch_num = PANTHER7_HDMA_CH_0;
	
	socle_request_dma(socle_spi_tx_dma_ch_num, &socle_spi_tx_dma_notifier);
	socle_request_dma(socle_spi_rx_dma_ch_num, &socle_spi_rx_dma_notifier);
	ret = test_item_ctrl(&socle_spi_hdma_burst_type_test_container, autotest);
	socle_disable_dma(socle_spi_tx_dma_ch_num);
	socle_disable_dma(socle_spi_rx_dma_ch_num);
	socle_free_dma(socle_spi_tx_dma_ch_num);
	socle_free_dma(socle_spi_rx_dma_ch_num);
	return ret;
}

extern int
socle_spi_hdma_burst_single_test(int autotest)
{
	socle_spi_dma_burst = SOCLE_DMA_BURST_SINGLE;
	return socle_spi_master_internal_hdma_run(autotest);
}

extern int
socle_spi_hdma_burst_incr4_test(int autotest)
{
	socle_spi_dma_burst = SOCLE_DMA_BURST_INCR4;
	return socle_spi_master_internal_hdma_run(autotest);
}

extern struct test_item_container socle_spi_master_slave_model_test_container;

extern int
socle_spi_socle_slave_test(int autotest)
{
	int ret = 0;
  ret = test_item_ctrl(&socle_spi_master_slave_model_test_container, autotest);
	return ret;
}


extern struct test_item_container socle_spi_master_slave_mode_test_container;

extern int
socle_spi_master_slave_protocol_test(int autotest)
{
  int ret = 0;
  pure=0;
  ret = test_item_ctrl(&socle_spi_master_slave_mode_test_container, autotest);
	return ret;
}

extern int
socle_spi_master_slave_pure_test(int autotest)
{
  int ret = 0;
  pure=1;
  regflag=0;
  ret = test_item_ctrl(&socle_spi_master_slave_mode_test_container, autotest);
	return ret;
}

extern int
socle_spi_master_slave_reg_test(int autotest)
{
	int ret = 0;
	pure=1;
  regflag=1;
  ret = test_item_ctrl(&socle_spi_master_slave_mode_test_container, autotest);
	return ret;
}

extern struct test_item_container socle_spi_slave_type_test_container;
extern struct test_item_container socle_spi_master_sb_test_container;

extern int
socle_spi_master_slave_mode0_test(int autotest)
{
	int ret=0;
	cpha=SOCLE_SPI_CPHA_0;
	cpol=SOCLE_SPI_CPOL_0;
	if(pure)
		ret = test_item_ctrl(&socle_spi_master_sb_test_container, autotest);
	else
		ret = test_item_ctrl(&socle_spi_slave_type_test_container, autotest);
	return ret;
}

extern int
socle_spi_master_slave_mode1_test(int autotest)
{
	int ret = 0;
	cpha=SOCLE_SPI_CPHA_0;
	cpol=SOCLE_SPI_CPOL_1;
	if(pure)
		ret = test_item_ctrl(&socle_spi_master_sb_test_container, autotest);
	else
		ret = test_item_ctrl(&socle_spi_slave_type_test_container, autotest);
	return ret;
}

extern int
socle_spi_master_slave_mode2_test(int autotest)
{
	int ret = 0;
	cpha=SOCLE_SPI_CPHA_1;
	cpol=SOCLE_SPI_CPOL_0;
	if(pure)
		ret = test_item_ctrl(&socle_spi_master_sb_test_container, autotest);
	else
		ret = test_item_ctrl(&socle_spi_slave_type_test_container, autotest);
	return ret;
}

extern int
socle_spi_master_slave_mode3_test(int autotest)
{
	int ret = 0;
	cpha=SOCLE_SPI_CPHA_1;
	cpol=SOCLE_SPI_CPOL_1;
	if(pure)
		ret = test_item_ctrl(&socle_spi_master_sb_test_container, autotest);
	else
		ret = test_item_ctrl(&socle_spi_slave_type_test_container, autotest);
	return ret;
}

extern struct test_item_container socle_spi_master_ch_test_container;

extern int
socle_spi_master_lsb_test(int autotest)
{
	int ret = 0;
	lsb=SOCLE_SPI_TX_LSB_FIRST;
	ret = test_item_ctrl(&socle_spi_master_ch_test_container, autotest);
	return ret;
}

extern int
socle_spi_master_msb_test(int autotest)
{
	int ret = 0;
	lsb=SOCLE_SPI_TX_MSB_FIRST;
	ret = test_item_ctrl(&socle_spi_master_ch_test_container, autotest);
	return ret;
}

extern int socle_spi_slave_run(int type);
extern int socle_spi_slave_pure_run(void);
extern int socle_spi_slave_reg_run(void);

extern int
socle_spi_master_ch8_test(int autotest)
{
	int ret = 0;
	char_len=SOCLE_SPI_CHAR_LEN_8;
	socle_spi_set_current_mode(MODE_CHAR_LEN_8);
	
		/* Configure SPI controller */
	socle_spi_write(
#ifdef CONFIG_PC9220 
		SOCLE_SPI_MASTER_SIGNAL_CTL_HW |
		SOCLE_SPI_MASTER_SIGNAL_ACT_NO |	
		SOCLE_SPI_MODE_MASTER |
#endif
    SOCLE_SPI_SOFT_N_RST |
		SOCLE_SPI_TXRX_N_RUN |
		SOCLE_SPI_CLK_IDLE_AST |
		SOCLE_SPI_TXRX_SIMULT_DIS |
		cpol |
		cpha |
		lsb |
		SOCLE_SPI_OP_NORMAL,
		SOCLE_SPI_FWCR);
		
	socle_spi_write(
		char_len |
		SOCLE_SPI_SLAVE_SEL_0 |
		SOCLE_SPI_CLK_DIV(SLAVE_DIV),
		SOCLE_SPI_SSCR);
	
	if(regflag)
		ret = socle_spi_slave_reg_run();
	else
		ret = socle_spi_slave_pure_run();
	return ret;
}
extern int
socle_spi_master_ch16_test(int autotest)
{
	int ret = 0;
	char_len=SOCLE_SPI_CHAR_LEN_16;
	socle_spi_set_current_mode(MODE_CHAR_LEN_16);
	
		/* Configure SPI controller */
	socle_spi_write(
#ifdef CONFIG_PC9220 
		SOCLE_SPI_MASTER_SIGNAL_CTL_HW |
		SOCLE_SPI_MASTER_SIGNAL_ACT_NO |	
		SOCLE_SPI_MODE_MASTER |
#endif
    SOCLE_SPI_SOFT_N_RST |
		SOCLE_SPI_TXRX_N_RUN |
		SOCLE_SPI_CLK_IDLE_AST |
		SOCLE_SPI_TXRX_SIMULT_DIS |
		cpol |
		cpha |
		lsb |
		SOCLE_SPI_OP_NORMAL,
		SOCLE_SPI_FWCR);
		
	socle_spi_write(
		char_len |
		SOCLE_SPI_SLAVE_SEL_0 |
		SOCLE_SPI_CLK_DIV(SLAVE_DIV),
		SOCLE_SPI_SSCR);
	
	if(regflag)
		ret = socle_spi_slave_reg_run();
	else
		ret = socle_spi_slave_pure_run();
	return ret;
}

extern int
socle_spi_slave_normal_test(int autotest)
{
	int type=SLAVE_NORMAL;
	
	socle_spi_write(
#ifdef CONFIG_PC9220
		SOCLE_SPI_MASTER_SIGNAL_CTL_HW |
		SOCLE_SPI_MASTER_SIGNAL_ACT_NO |		
		SOCLE_SPI_MODE_MASTER |
#endif
		SOCLE_SPI_SOFT_N_RST |
		SOCLE_SPI_TXRX_N_RUN |
		SOCLE_SPI_CLK_IDLE_AST |
		SOCLE_SPI_TXRX_SIMULT_DIS |
		cpol |
		cpha |
		SOCLE_SPI_TX_MSB_FIRST |
		SOCLE_SPI_OP_NORMAL,
		SOCLE_SPI_FWCR);
		
	return socle_spi_slave_run(type);
}

extern int 
socle_spi_slave_hdma_test(int autotest)
{
	int type=SLAVE_HDMA;
	
		socle_spi_write(
#ifdef CONFIG_PC9220
		SOCLE_SPI_MASTER_SIGNAL_CTL_HW |
		SOCLE_SPI_MASTER_SIGNAL_ACT_NO |		
		SOCLE_SPI_MODE_MASTER |
#endif
		SOCLE_SPI_SOFT_N_RST |
		SOCLE_SPI_TXRX_N_RUN |
		SOCLE_SPI_CLK_IDLE_AST |
		SOCLE_SPI_TXRX_SIMULT_DIS |
		cpol |
		cpha |
		SOCLE_SPI_TX_MSB_FIRST |
		SOCLE_SPI_OP_NORMAL,
		SOCLE_SPI_FWCR);
		
	return socle_spi_slave_run(type);
}

extern int socle_spi_slave_reset_test(int autotest)
{
	int type=SLAVE_RESET;
	
		socle_spi_write(
#ifdef CONFIG_PC9220
		SOCLE_SPI_MASTER_SIGNAL_CTL_HW |
		SOCLE_SPI_MASTER_SIGNAL_ACT_NO |		
		SOCLE_SPI_MODE_MASTER |
#endif
		SOCLE_SPI_SOFT_N_RST |
		SOCLE_SPI_TXRX_N_RUN |
		SOCLE_SPI_CLK_IDLE_AST |
		SOCLE_SPI_TXRX_SIMULT_DIS |
		cpol |
		cpha |
		SOCLE_SPI_TX_MSB_FIRST |
		SOCLE_SPI_OP_NORMAL,
		SOCLE_SPI_FWCR);
		
	return socle_spi_slave_run(type);
}



extern int
socle_spi_internal_normal_run(int autotest)
{
    /* Reset SPI controller */
  socle_spi_write(
  	socle_spi_read(SOCLE_SPI_FWCR) |
    SOCLE_SPI_SOFT_RST, 
    SOCLE_SPI_FWCR);
    
    /* Configure SPI controller */
	socle_spi_write(
#ifdef CONFIG_PC9220
		SOCLE_SPI_MASTER_SIGNAL_CTL_HW |
		SOCLE_SPI_MASTER_SIGNAL_ACT_NO |		
		SOCLE_SPI_MODE_MASTER |
#endif
		SOCLE_SPI_SOFT_N_RST |
		SOCLE_SPI_TXRX_N_RUN |
		SOCLE_SPI_CLK_IDLE_AST |
		SOCLE_SPI_TXRX_SIMULT_DIS |
		SOCLE_SPI_CPOL_0 |
		SOCLE_SPI_CPHA_0 |
		SOCLE_SPI_TX_LSB_FIRST |
		SOCLE_SPI_OP_LOOPBACK,
		SOCLE_SPI_FWCR);
			
	/* Enable SPI interrupt */
	socle_spi_write(
		//SOCLE_SPI_IER_RXFIFO_OVR_INT_EN |
    //SOCLE_SPI_IER_TXFIFO_INT_EN |
		SOCLE_SPI_IER_RXFIFO_INT_EN |
		SOCLE_SPI_IER_RX_COMPLETE_INT_EN,
		SOCLE_SPI_IER);

	/* Configure FIFO and clear Tx & Rx FIFO */
	socle_spi_write(
		SOCLE_SPI_RXFIFO_INT_TRIGGER_LEVEL_4 |
		SOCLE_SPI_TXFIFO_INT_TRIGGER_LEVEL_4 |
		SOCLE_SPI_RXFIFO_CLR |
		SOCLE_SPI_TXFIFO_CLR,
		SOCLE_SPI_FCR);
     
	/* Set the SPI slaves select and characteristic control register */
	socle_spi_write(
		SOCLE_SPI_CHAR_LEN_8 |
		SOCLE_SPI_SLAVE_SEL_0 |
		SOCLE_SPI_CLK_DIV(LOOKBACK_DIV),
		SOCLE_SPI_SSCR);

	/* Config SPI clock delay */
	socle_spi_write(
		SOCLE_SPI_PBTXRX_DELAY_NONE |
		SOCLE_SPI_PBCT_DELAY_NONE |
		SOCLE_SPI_PBCA_DELAY_1_2,
		SOCLE_SPI_DLYCR);


	/* Set per char length */
	socle_spi_set_current_mode(MODE_CHAR_LEN_8);
	/* Clear pattern buffer and compare buffer */
	memset(socle_spi_pattern_buf, 0x0, PATTERN_BUF_SIZE);
	memset(socle_spi_cmpr_buf, 0x0, PATTERN_BUF_SIZE);

	/* Make test pattern */
	socle_spi_make_test_pattern(socle_spi_pattern_buf, PATTERN_BUF_SIZE);

	if (socle_spi_transfer(socle_spi_pattern_buf, socle_spi_cmpr_buf, SET_TX_RX_LEN(PATTERN_BUF_SIZE, PATTERN_BUF_SIZE)))
		return -1;
	return socle_spi_compare_memory(socle_spi_pattern_buf, socle_spi_cmpr_buf, PATTERN_BUF_SIZE, autotest);

}

static int
socle_spi_master_internal_hdma_run(int autotest)
{	
	socle_spi_tx_complete_flag = 0;
	socle_spi_rx_complete_flag = 0; 
	
	/* Set per char length */
	socle_spi_set_current_mode(MODE_CHAR_LEN_8);
	
	memset(socle_spi_pattern_buf, 0x0, PATTERN_BUF_SIZE);
	memset(socle_spi_cmpr_buf, 0x0, PATTERN_BUF_SIZE);
	socle_spi_make_test_pattern(socle_spi_pattern_buf, PATTERN_BUF_SIZE);
	
	socle_spi_write(
    socle_spi_read(SOCLE_SPI_FWCR) |
		SOCLE_SPI_SOFT_RST, 
    SOCLE_SPI_FWCR);
		
	socle_spi_write(
		SOCLE_SPI_IER_TXFIFO_INT_DIS |
		SOCLE_SPI_IER_RXFIFO_INT_DIS |
		//SOCLE_SPI_IER_RXFIFO_OVR_INT_EN |
		SOCLE_SPI_IER_RX_COMPLETE_INT_DIS,
		SOCLE_SPI_IER);
	
	socle_spi_write(
		SOCLE_SPI_RXFIFO_INT_TRIGGER_LEVEL_4 |
		SOCLE_SPI_TXFIFO_INT_TRIGGER_LEVEL_4 |
		SOCLE_SPI_RXFIFO_CLR |
		SOCLE_SPI_TXFIFO_CLR,
		SOCLE_SPI_FCR);
			
	socle_spi_write(
		SOCLE_SPI_PBTXRX_DELAY_NONE |
		SOCLE_SPI_PBCT_DELAY_NONE |
		SOCLE_SPI_PBCA_DELAY_256,
		SOCLE_SPI_DLYCR);
	
	socle_spi_write(
		SOCLE_SPI_CHAR_LEN_8 |
		SOCLE_SPI_SLAVE_SEL_0 |
		SOCLE_SPI_CLK_DIV(LOOKBACK_DIV),
		SOCLE_SPI_SSCR);

	socle_spi_write(PATTERN_BUF_SIZE, SOCLE_SPI_TXCR);
	socle_spi_write(PATTERN_BUF_SIZE, SOCLE_SPI_RXCR);

	socle_disable_dma(socle_spi_tx_dma_ch_num);
	socle_set_dma_mode(socle_spi_tx_dma_ch_num, SOCLE_DMA_MODE_SLICE);
	socle_set_dma_ext_hdreq_number(socle_spi_tx_dma_ch_num, socle_spi_tx_dma_ext_hdreq);
	socle_set_dma_burst_type(socle_spi_tx_dma_ch_num, socle_spi_dma_burst);
	socle_set_dma_source_address(socle_spi_tx_dma_ch_num, (u32)socle_spi_pattern_buf);
	socle_set_dma_destination_address(socle_spi_tx_dma_ch_num, SOCLE_SPI_TXR+socle_spi_base);
	socle_set_dma_source_direction(socle_spi_tx_dma_ch_num, SOCLE_DMA_DIR_INCR);
	socle_set_dma_destination_direction(socle_spi_tx_dma_ch_num, SOCLE_DMA_DIR_FIXED);
	socle_set_dma_data_size(socle_spi_tx_dma_ch_num, SOCLE_DMA_DATA_BYTE);
	socle_set_dma_transfer_count(socle_spi_tx_dma_ch_num, PATTERN_BUF_SIZE);
	socle_set_dma_slice_count(socle_spi_tx_dma_ch_num, 4);
	socle_set_dma_page_number(socle_spi_tx_dma_ch_num, 1);
	socle_set_dma_buffer_size(socle_spi_tx_dma_ch_num, PATTERN_BUF_SIZE);
		
	socle_disable_dma(socle_spi_rx_dma_ch_num);
	socle_set_dma_mode(socle_spi_rx_dma_ch_num, SOCLE_DMA_MODE_SLICE);
	socle_set_dma_ext_hdreq_number(socle_spi_rx_dma_ch_num, socle_spi_rx_dma_ext_hdreq);
	socle_set_dma_burst_type(socle_spi_rx_dma_ch_num, socle_spi_dma_burst);
	socle_set_dma_source_address(socle_spi_rx_dma_ch_num, SOCLE_SPI_RXR+socle_spi_base);
	socle_set_dma_destination_address(socle_spi_rx_dma_ch_num, (u32)socle_spi_cmpr_buf);
	socle_set_dma_source_direction(socle_spi_rx_dma_ch_num, SOCLE_DMA_DIR_FIXED);
	socle_set_dma_destination_direction(socle_spi_rx_dma_ch_num, SOCLE_DMA_DIR_INCR);
	socle_set_dma_data_size(socle_spi_rx_dma_ch_num, SOCLE_DMA_DATA_BYTE);
	socle_set_dma_transfer_count(socle_spi_rx_dma_ch_num, PATTERN_BUF_SIZE);
	socle_set_dma_slice_count(socle_spi_rx_dma_ch_num, 4);
	socle_set_dma_page_number(socle_spi_rx_dma_ch_num, 1);
	socle_set_dma_buffer_size(socle_spi_rx_dma_ch_num, PATTERN_BUF_SIZE);

	socle_enable_dma(socle_spi_tx_dma_ch_num);
	socle_enable_dma(socle_spi_rx_dma_ch_num);
			
	socle_spi_write(
#ifdef CONFIG_PC9220
		SOCLE_SPI_MASTER_SIGNAL_CTL_HW |
		SOCLE_SPI_MASTER_SIGNAL_ACT_NO |		
		SOCLE_SPI_MODE_MASTER |
#endif
		SOCLE_SPI_EN |
		SOCLE_SPI_TXRX_RUN |
		SOCLE_SPI_CLK_IDLE_AST |
		SOCLE_SPI_TXRX_SIMULT_EN |
		SOCLE_SPI_CPOL_0 |
		SOCLE_SPI_CPHA_0 |
		SOCLE_SPI_TX_MSB_FIRST |
		SOCLE_SPI_DMA_REQ |
		SOCLE_SPI_OP_LOOPBACK,			
		SOCLE_SPI_FWCR);

	if (socle_wait_for_int(&socle_spi_rx_complete_flag, 30))
		printf("Timeout\n");
	
	return socle_spi_compare_memory(socle_spi_pattern_buf, socle_spi_cmpr_buf, PATTERN_BUF_SIZE, autotest);
}

static void 
socle_spi_tx_dma_page_interrupt(void *data)
{
	socle_spi_tx_complete_flag = 1;
}

static void 
socle_spi_rx_dma_page_interrupt(void *data)
{
	socle_spi_rx_complete_flag = 1;
}



