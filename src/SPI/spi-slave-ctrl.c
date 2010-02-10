#include <test_item.h>
#include <global.h>
#include <dma/dma.h>
#include "spi-regs.h"
#include "dependency.h"

#if defined (CONFIG_PC9220)
#include <scu.h>
#include "../../platform/arch/scu-reg.h"
#endif


/* Commands */

#define CMD_READ 0x01	/* read data from memory array */
#define CMD_WRITE 0x02	/* write data to memory array */
#define CMD_DMARD 0x03
#define CMD_DMAWR 0x04
#define CMD_EXIT 0x0F
#define REG_CMD_RESET_EN 0x0E
#define REG_CMD_RESET 0x0C

#define STA_READY              0x01
#define STA_BUSY               0x00
#define STA_RESET_EN          (0x01 << 1)
#define STA_CMD                0x00
#define STA_GET_ADDR_HI   		 0x10
#define STA_GET_ADDR_LOW       0x11
#define STA_GET_SIZE_HI        0x12
#define STA_GET_SIZE_LOW       0x13

#define PATTERN_BUF_SIZE  2048
#define PATTERN_BUF_SIZE_PURE 8
static void socle_spi_slave_protocol_isr(void *data);
static void socle_spi_slave_pure_isr(void *data);
static void socle_spi_slave_reg_isr(void *data);
static void socle_spi_slave_tx_dma_page_interrupt(void *data);
static void socle_spi_slave_rx_dma_page_interrupt(void *data);


static struct socle_dma_notifier socle_spi_slave_tx_dma_notifier = {
	.page_interrupt = socle_spi_slave_tx_dma_page_interrupt,
};

static struct socle_dma_notifier socle_spi_slave_rx_dma_notifier = {
	.page_interrupt = socle_spi_slave_rx_dma_page_interrupt,
};

static int socle_spi_slave_protocol_run(void);
static int socle_spi_slave_pure_hdma_run(u32 socle_spi_slave_dma_burst);
static void socle_spi_slave_write(void);
static void socle_spi_slave_read(void);
static void socle_spi_slave_dma_read(void);
static void socle_spi_slave_dma_write(void);
static void socle_spi_slave_run_state(void);
static void socle_spi_slave_protocol_reset(void);
static int socle_spi_reg_rw_run(void);
static int start,protocol,regflag;
static u32 socle_spi_base,socle_spi_irq,cpha,cpol,lsb,char_len;
static u8 state,command;
static u32 socle_spi_tx_dma_ch_num, socle_spi_rx_dma_ch_num;
static u32 socle_spi_tx_dma_ext_hdreq, socle_spi_rx_dma_ext_hdreq;
static u16 temp_addr,addr_temp,temp_size,count;
static u8 *storage = (u8 *)0x00a00000;
static u16 *storage16= (u16 *)0x00a01000;
static volatile int socle_spi_tx_complete_flag = 0;
static volatile int socle_spi_rx_complete_flag = 0;
static volatile int socle_spi_tx_finish_flag = 0;



static inline void
socle_spi_write(u32 val, u32 reg)
{
	iowrite32(val, socle_spi_base+reg);
}

static inline u32
socle_spi_read(u32 reg)
{
	return ioread32(socle_spi_base+reg);
}

extern struct test_item_container socle_spi_slave_test_container;

extern int
socle_spi_slave_test(int autotest)
{
	int ret = 0;
	ret = test_item_ctrl(&socle_spi_slave_test_container, autotest);
	return ret;
}

extern struct test_item_container socle_spi_slave_model_test_container;

int socle_spi_slave0_test(int autotest)
{
	int ret = 0;
#ifdef CONFIG_PC9220
	socle_scu_dev_enable(SOCLE_DEVCON_SPI0);
	socle_scu_hdma_req45_spi0();
#endif
	
	socle_spi_base = SOCLE_APB0_SPI0;
	socle_spi_irq = SOCLE_INTC_SPI0;
	socle_spi_tx_dma_ext_hdreq = 5;
	socle_spi_rx_dma_ext_hdreq = 4;

	socle_spi_tx_dma_ch_num = PANTHER7_HDMA_CH_1;
	socle_spi_rx_dma_ch_num = PANTHER7_HDMA_CH_0;
	socle_spi_write(SOCLE_SPI_SOFT_RST, SOCLE_SPI_FWCR);
	ret = test_item_ctrl(&socle_spi_slave_model_test_container, autotest);

#ifdef CONFIG_PC9220
	socle_scu_dev_disable(SOCLE_DEVCON_SPI0);
#endif
	
	return ret;

}

int socle_spi_slave1_test(int autotest)
{
	int ret = 0;
#ifdef CONFIG_PC9220
	socle_scu_dev_enable(SOCLE_DEVCON_SPI1);
	socle_scu_hdma_req45_spi1();
#endif	
	socle_spi_base = SOCLE_APB0_SPI1;
	socle_spi_irq = SOCLE_INTC_SPI1;
	socle_spi_tx_dma_ext_hdreq = 5;
	socle_spi_rx_dma_ext_hdreq = 4;

	socle_spi_tx_dma_ch_num = PANTHER7_HDMA_CH_1;
	socle_spi_rx_dma_ch_num = PANTHER7_HDMA_CH_0;
	socle_spi_write(SOCLE_SPI_SOFT_RST, SOCLE_SPI_FWCR);
	ret = test_item_ctrl(&socle_spi_slave_model_test_container, autotest);
#ifdef CONFIG_PC9220
	socle_scu_dev_disable(SOCLE_DEVCON_SPI1);
#endif
	
	return ret;

}

extern struct test_item_container socle_spi_slave_mode_test_container;

extern int
socle_spi_slave_protocol_test(int autotest)
{
	int ret = 0;
	protocol=1;
	ret = test_item_ctrl(&socle_spi_slave_mode_test_container, autotest);
	return ret;
}

extern int
socle_spi_slave_pure_test(int autotest)
{
	int ret = 0;
	protocol=0;
	regflag=0;
	ret = test_item_ctrl(&socle_spi_slave_mode_test_container, autotest);
	return ret;
}

extern int
socle_spi_reg_rw_test(int autotest)
{
	int ret = 0;
	regflag=1;
	protocol=0;
	ret = test_item_ctrl(&socle_spi_slave_mode_test_container, autotest);
	return ret;
	
}

extern struct test_item_container socle_spi_slave_sb_test_container;

extern int
socle_spi_slave_mode0_test(int autotest)
{
	int ret = 0;
	cpha=SOCLE_SPI_CPHA_0;
	cpol=SOCLE_SPI_CPOL_0;
	if(protocol)
		ret = socle_spi_slave_protocol_run();
	else
		ret = test_item_ctrl(&socle_spi_slave_sb_test_container, autotest);
	return ret;
}

extern int 
socle_spi_slave_mode1_test(int autotest)
{
	int ret = 0;
	cpha=SOCLE_SPI_CPHA_0;
	cpol=SOCLE_SPI_CPOL_1;
	if(protocol)
		ret = socle_spi_slave_protocol_run();
	else
		ret = test_item_ctrl(&socle_spi_slave_sb_test_container, autotest);
	return ret;
}


extern int 
socle_spi_slave_mode2_test(int autotest)
{
	int ret = 0;
	cpha=SOCLE_SPI_CPHA_1;
	cpol=SOCLE_SPI_CPOL_0;
	if(protocol)
		ret = socle_spi_slave_protocol_run();
	else
		ret = test_item_ctrl(&socle_spi_slave_sb_test_container, autotest);
	return ret;
}

extern int 
socle_spi_slave_mode3_test(int autotest)
{
	int ret = 0;
	cpha=SOCLE_SPI_CPHA_1;
	cpol=SOCLE_SPI_CPOL_1;
	if(protocol)
		ret = socle_spi_slave_protocol_run();
	else
		ret = test_item_ctrl(&socle_spi_slave_sb_test_container, autotest);
	return ret;
}

extern struct test_item_container socle_spi_slave_ch_test_container;

extern int 
socle_spi_slave_lsb_test(int autotest)
{
	int ret = 0;
	lsb= SOCLE_SPI_TX_LSB_FIRST;
	ret = test_item_ctrl(&socle_spi_slave_ch_test_container, autotest);
	return ret;
}

extern int 
socle_spi_slave_msb_test(int autotest)
{
	int ret = 0;
	lsb = SOCLE_SPI_TX_MSB_FIRST;
	ret = test_item_ctrl(&socle_spi_slave_ch_test_container, autotest);
	return ret;
}

extern struct test_item_container socle_spi_slave_pure_type_test_container;

extern int
socle_spi_slave_ch8_test(int autotest)
{
	int ret = 0;
	char_len= SOCLE_SPI_CHAR_LEN_8;
	if(regflag)
		ret = socle_spi_reg_rw_run();
	else
		ret = test_item_ctrl(&socle_spi_slave_pure_type_test_container, autotest);
	return ret;
}

extern int
socle_spi_slave_ch16_test(int autotest)
{
	int ret = 0;
	char_len= SOCLE_SPI_CHAR_LEN_16;
	if(regflag)
		ret = socle_spi_reg_rw_run();
	else
		ret = test_item_ctrl(&socle_spi_slave_pure_type_test_container, autotest);
	return ret;
}

extern struct test_item_container socle_spi_slave_burst_type_test_container;

extern int
socle_spi_slave_pure_hdma_test(int autotest)
{
	int ret = 0;
	ret = test_item_ctrl(&socle_spi_slave_burst_type_test_container, autotest);
	return ret;
}

extern int
socle_spi_slave_hdma_burst_single(int autotest)
{
	return socle_spi_slave_pure_hdma_run(SOCLE_DMA_BURST_SINGLE);
}
extern int
socle_spi_slave_hdma_burst_incr4(int autotest)
{
	return socle_spi_slave_pure_hdma_run(SOCLE_DMA_BURST_INCR4);
}


static int
socle_spi_slave_protocol_run(void)
{
	socle_spi_write(STA_BUSY, SOCLE_SPI_TX_REG0);
	request_irq(socle_spi_irq, socle_spi_slave_protocol_isr, NULL);
	socle_request_dma(socle_spi_rx_dma_ch_num, &socle_spi_slave_rx_dma_notifier);
	socle_request_dma(socle_spi_tx_dma_ch_num, &socle_spi_slave_tx_dma_notifier);
	
	memset(storage, 0,PATTERN_BUF_SIZE );
	start=1;
	socle_spi_slave_protocol_reset();
	socle_spi_write(STA_READY, SOCLE_SPI_TX_REG0);
	printf("Slave Start\n");
	while(start) {
		if(state==CMD_WRITE)
			socle_spi_slave_write();
		else if(state==CMD_READ)
			socle_spi_slave_read();
		else if(state==CMD_DMAWR)
			socle_spi_slave_dma_write();
		else if(state==CMD_DMARD)
			socle_spi_slave_dma_read();
	}
	
	socle_spi_write(SOCLE_SPI_SOFT_RST, SOCLE_SPI_FWCR);
	socle_spi_write(STA_BUSY, SOCLE_SPI_TX_REG0);
	printf("program stop\n");	
	socle_free_dma(socle_spi_rx_dma_ch_num);
	socle_free_dma(socle_spi_tx_dma_ch_num);
	free_irq(socle_spi_irq);
	return 0;
}

static void
socle_spi_slave_protocol_isr(void *pparam)
{
	u32 tmp = socle_spi_read(SOCLE_SPI_ISR);
	
	if (SOCLE_SPI_RX_DATA_AVAIL_INT == (tmp & SOCLE_SPI_RX_DATA_AVAIL_INT)) {
		if(state!=CMD_WRITE)
			socle_spi_slave_run_state();
		else {			
			while (SOCLE_SPI_RXFIFO_DATA_AVAIL == (socle_spi_read(SOCLE_SPI_FCR) & SOCLE_SPI_RXFIFO_DATA_AVAIL)){
				storage[temp_addr+count] = socle_spi_read(SOCLE_SPI_RXR);
				count++;
				if(temp_size==count) {
					socle_spi_rx_complete_flag=1;
					break;
				}
			}
		}
		return;
	}
	 	
	if (SOCLE_SPI_TXFIFO_INT == (tmp & SOCLE_SPI_TXFIFO_INT)) {
		do {   
			socle_spi_write(storage[temp_addr+count], SOCLE_SPI_TXR);
			count++;
			if(temp_size==count) {
				socle_spi_write(socle_spi_read(SOCLE_SPI_IER) & ~SOCLE_SPI_IER_TXFIFO_INT_EN, SOCLE_SPI_IER);
				socle_spi_tx_complete_flag=1;
				return;
			} 
		} while (SOCLE_SPI_TXFIFO_FULL != (socle_spi_read(SOCLE_SPI_FCR) & SOCLE_SPI_TXFIFO_FULL));
		return;
	}
	
	if (SOCLE_SPI_TXFIFO_EMPTY_INT == (tmp & SOCLE_SPI_TXFIFO_EMPTY_INT)) {
			socle_spi_write(socle_spi_read(SOCLE_SPI_IER) & ~SOCLE_SPI_IER_TXFIFO_EMPTY_INT_EN, SOCLE_SPI_IER);
			socle_spi_tx_finish_flag=1;
    	return; 
	}
	
	if (SOCLE_SPI_RX_REG0_INT == (tmp & SOCLE_SPI_RX_REG0_INT)) {
		socle_spi_write(SOCLE_SPI_RX_REG0_INT,SOCLE_SPI_ISR);
		if( REG_CMD_RESET_EN == socle_spi_read(SOCLE_SPI_RX_REG0))
			socle_spi_write(STA_RESET_EN, SOCLE_SPI_TX_REG0);
		else if( REG_CMD_RESET == socle_spi_read(SOCLE_SPI_RX_REG0)) {
			if(STA_RESET_EN != socle_spi_read(SOCLE_SPI_TX_REG0))
				return;
			socle_spi_slave_protocol_reset();
			if(state > CMD_DMAWR || state < CMD_READ) {
				state=STA_CMD;
				socle_spi_write(STA_READY, SOCLE_SPI_TX_REG0);
			}
		}				
	}
	
	if(SOCLE_SPI_IER_CHAR_LEN_INT_EN == (tmp & SOCLE_SPI_CHAR_LEN_INT)) {
		socle_spi_write(SOCLE_SPI_CHAR_LEN_INT,SOCLE_SPI_ISR);
		printf("Wrong bits\n");
		start=0;
	}
	
}

static void 
socle_spi_slave_run_state(void)
{
	switch(state) {
  	case STA_CMD:
  		command=socle_spi_read(SOCLE_SPI_RXR);
    	if(command == CMD_READ || command == CMD_WRITE || command == CMD_DMAWR || command == CMD_DMARD) {
    		socle_spi_write(STA_BUSY, SOCLE_SPI_TX_REG0);
    		state=STA_GET_ADDR_HI;
    	}		
			else if(command == CMD_EXIT)
				start=0;
			break;
    case STA_GET_ADDR_HI:
     	temp_addr = (socle_spi_read(SOCLE_SPI_RXR) << 8);
     	state=STA_GET_ADDR_LOW;
     	break;     
    case STA_GET_ADDR_LOW:
    	temp_addr = (temp_addr | socle_spi_read(SOCLE_SPI_RXR));
    	state=STA_GET_SIZE_HI;               	
     	break;
    case STA_GET_SIZE_HI:
    	temp_size = (socle_spi_read(SOCLE_SPI_RXR) << 8);
    	state=STA_GET_SIZE_LOW;
     	break;
    case STA_GET_SIZE_LOW:
    	temp_size = (temp_size | socle_spi_read(SOCLE_SPI_RXR));
    	state=command;
    	socle_spi_write(socle_spi_read(SOCLE_SPI_IER)& ~SOCLE_SPI_IER_RXAVAIL_INT_EN, SOCLE_SPI_IER);    	
    	break;
 	}
} 

static void
socle_spi_slave_write(void)
{
	socle_spi_rx_complete_flag=0;
	count=0;
	socle_spi_write(socle_spi_read(SOCLE_SPI_IER) | SOCLE_SPI_IER_RXAVAIL_INT_EN , SOCLE_SPI_IER);
	if (socle_wait_for_int(&socle_spi_rx_complete_flag, 30)) {
		printf("Rx Timeout\n");
	}
	if(SOCLE_SPI_RXFIFO_DATA_AVAIL == (ioread32(socle_spi_base+SOCLE_SPI_FCR) & SOCLE_SPI_RXFIFO_DATA_AVAIL))
		iowrite32(ioread32(socle_spi_base+SOCLE_SPI_FCR) | SOCLE_SPI_RXFIFO_CLR, socle_spi_base+SOCLE_SPI_FCR);
	
	state=STA_CMD;
	socle_spi_write(STA_READY, SOCLE_SPI_TX_REG0);
}


static void
socle_spi_slave_dma_write(void)
{
	socle_spi_rx_complete_flag = 0;
	
	socle_disable_dma(socle_spi_rx_dma_ch_num);
	socle_set_dma_mode(socle_spi_rx_dma_ch_num, SOCLE_DMA_MODE_SLICE);
	socle_set_dma_ext_hdreq_number(socle_spi_rx_dma_ch_num, socle_spi_rx_dma_ext_hdreq);
	socle_set_dma_burst_type(socle_spi_rx_dma_ch_num, SOCLE_DMA_BURST_INCR4);
	socle_set_dma_source_address(socle_spi_rx_dma_ch_num, SOCLE_SPI_RXR+socle_spi_base);
	socle_set_dma_source_direction(socle_spi_rx_dma_ch_num, SOCLE_DMA_DIR_FIXED);
	socle_set_dma_destination_address(socle_spi_rx_dma_ch_num, (u32)&storage[temp_addr]);
	socle_set_dma_destination_direction(socle_spi_rx_dma_ch_num, SOCLE_DMA_DIR_INCR);
	socle_set_dma_data_size(socle_spi_rx_dma_ch_num, SOCLE_DMA_DATA_BYTE);
	socle_set_dma_slice_count(socle_spi_rx_dma_ch_num, 4);
	socle_set_dma_page_number(socle_spi_rx_dma_ch_num, 1);	
	socle_set_dma_transfer_count(socle_spi_rx_dma_ch_num, temp_size);
	socle_enable_dma(socle_spi_rx_dma_ch_num);
	
	socle_spi_write(socle_spi_read(SOCLE_SPI_FWCR) | SOCLE_SPI_DMA_REQ, SOCLE_SPI_FWCR);
	if (socle_wait_for_int(&socle_spi_rx_complete_flag, 30)) {
		printf("DMA Rx Timeout\n");
	}
	socle_disable_dma(socle_spi_rx_dma_ch_num);
	socle_spi_write(socle_spi_read(SOCLE_SPI_FWCR) & ~SOCLE_SPI_DMA_REQ, SOCLE_SPI_FWCR);
	
	if(SOCLE_SPI_RXFIFO_DATA_AVAIL == (socle_spi_read(SOCLE_SPI_FCR) & SOCLE_SPI_RXFIFO_DATA_AVAIL))
		socle_spi_write(socle_spi_read(SOCLE_SPI_FCR) | SOCLE_SPI_RXFIFO_CLR, SOCLE_SPI_FCR);
	
	state=STA_CMD;
	socle_spi_write(socle_spi_read(SOCLE_SPI_IER) | SOCLE_SPI_IER_RXAVAIL_INT_EN , SOCLE_SPI_IER);	
	socle_spi_write(STA_READY, SOCLE_SPI_TX_REG0);
	
}


static void
socle_spi_slave_read(void)
{
  socle_spi_tx_complete_flag=0;
  socle_spi_tx_finish_flag=0;
  count=0;
  
  
  while(socle_spi_tx_complete_flag!=1) {
  	while (SOCLE_SPI_TXFIFO_FULL != (socle_spi_read(SOCLE_SPI_FCR) & SOCLE_SPI_TXFIFO_FULL)) {
  		socle_spi_write(storage[temp_addr+count], SOCLE_SPI_TXR);
			count++;
			if(temp_size==count) {
				socle_spi_tx_complete_flag=1;
				break;
			}
  	}
  }
  
  socle_spi_write(socle_spi_read(SOCLE_SPI_IER) | 
						SOCLE_SPI_IER_TXFIFO_EMPTY_INT_EN,
						SOCLE_SPI_IER);
						
	if(socle_wait_for_int(&socle_spi_tx_finish_flag, 30)) {
		printf("Tx Timeout\n");
	}
	if(SOCLE_SPI_RXFIFO_DATA_AVAIL == (socle_spi_read(SOCLE_SPI_FCR) & SOCLE_SPI_RXFIFO_DATA_AVAIL))
		socle_spi_write(socle_spi_read(SOCLE_SPI_FCR) | SOCLE_SPI_RXFIFO_CLR, SOCLE_SPI_FCR);
	if(SOCLE_SPI_SLAVE_TxFF_EMPTY != (socle_spi_read(SOCLE_SPI_FIFO_STATUS) & SOCLE_SPI_SLAVE_TxFF_EMPTY))
		socle_spi_write(socle_spi_read(SOCLE_SPI_FCR) | SOCLE_SPI_TXFIFO_CLR, SOCLE_SPI_FCR);
	state=STA_CMD;
	socle_spi_write(socle_spi_read(SOCLE_SPI_IER) | SOCLE_SPI_IER_RXAVAIL_INT_EN , SOCLE_SPI_IER);
	socle_spi_write(STA_READY, SOCLE_SPI_TX_REG0);
	
}

static void
socle_spi_slave_dma_read(void)
{	
	socle_spi_tx_complete_flag=0;
  socle_spi_tx_finish_flag=0;
  
	socle_disable_dma(socle_spi_tx_dma_ch_num);
	socle_set_dma_mode(socle_spi_tx_dma_ch_num, SOCLE_DMA_MODE_SLICE);
	socle_set_dma_ext_hdreq_number(socle_spi_tx_dma_ch_num, socle_spi_tx_dma_ext_hdreq);
	socle_set_dma_burst_type(socle_spi_tx_dma_ch_num, SOCLE_DMA_BURST_INCR4);	
	socle_set_dma_source_address(socle_spi_tx_dma_ch_num, (u32)&storage[temp_addr]);
	socle_set_dma_source_direction(socle_spi_tx_dma_ch_num, SOCLE_DMA_DIR_INCR);
	socle_set_dma_destination_address(socle_spi_tx_dma_ch_num, SOCLE_SPI_TXR+socle_spi_base);
	socle_set_dma_destination_direction(socle_spi_tx_dma_ch_num, SOCLE_DMA_DIR_FIXED);
	socle_set_dma_data_size(socle_spi_tx_dma_ch_num, SOCLE_DMA_DATA_BYTE);
	socle_set_dma_slice_count(socle_spi_tx_dma_ch_num, 4);
	socle_set_dma_page_number(socle_spi_tx_dma_ch_num, 1);
	socle_set_dma_transfer_count(socle_spi_tx_dma_ch_num, temp_size);
	socle_enable_dma(socle_spi_tx_dma_ch_num);
	
	socle_spi_write(socle_spi_read(SOCLE_SPI_FWCR)| SOCLE_SPI_DMA_REQ, SOCLE_SPI_FWCR);
	if (socle_wait_for_int(&socle_spi_tx_complete_flag, 30)) {
		printf("DMA Tx Timeout\n");
	}
	socle_spi_write(socle_spi_read(SOCLE_SPI_FWCR) & ~SOCLE_SPI_DMA_REQ, SOCLE_SPI_FWCR);
	socle_disable_dma(socle_spi_tx_dma_ch_num);
	
	socle_spi_write(socle_spi_read(SOCLE_SPI_IER) |
						SOCLE_SPI_IER_TXFIFO_EMPTY_INT_EN,
						SOCLE_SPI_IER);
	
	if (socle_wait_for_int(&socle_spi_tx_finish_flag, 10)) {
		printf("Tx Timeout\n");
	}
						
	if(SOCLE_SPI_RXFIFO_DATA_AVAIL == (socle_spi_read(SOCLE_SPI_FCR) & SOCLE_SPI_RXFIFO_DATA_AVAIL))
		socle_spi_write(socle_spi_read(SOCLE_SPI_FCR) | SOCLE_SPI_RXFIFO_CLR, SOCLE_SPI_FCR);
	if(SOCLE_SPI_SLAVE_TxFF_EMPTY != (socle_spi_read(SOCLE_SPI_FIFO_STATUS) & SOCLE_SPI_SLAVE_TxFF_EMPTY))
		socle_spi_write(socle_spi_read(SOCLE_SPI_FCR) | SOCLE_SPI_TXFIFO_CLR, SOCLE_SPI_FCR);
	
	state=STA_CMD;	
	socle_spi_write(socle_spi_read(SOCLE_SPI_IER) | SOCLE_SPI_IER_RXAVAIL_INT_EN , SOCLE_SPI_IER);
	socle_spi_write(STA_READY, SOCLE_SPI_TX_REG0);
	
}



static void
socle_spi_slave_protocol_reset(void)
{
	socle_spi_write(SOCLE_SPI_SOFT_RST, SOCLE_SPI_FWCR);
	socle_spi_write(SOCLE_SPI_CHAR_LEN_8, SOCLE_SPI_SSCR);
	
	/* Configure FIFO and clear Tx & Rx FIFO */
	socle_spi_write(
			SOCLE_SPI_RXFIFO_INT_TRIGGER_LEVEL_4 |
			SOCLE_SPI_TXFIFO_INT_TRIGGER_LEVEL_4 |
			SOCLE_SPI_RXFIFO_CLR |
			SOCLE_SPI_TXFIFO_CLR,
			SOCLE_SPI_FCR);
     
	/* Enable SPI interrupt */
	socle_spi_write(
			//SOCLE_SPI_IER_WRONG_BIT_INT_EN |
			SOCLE_SPI_IER_RXFIFO_OVR_INT_EN |
			SOCLE_SPI_IER_RX_REG0_INT_EN |
			//SOCLE_SPI_IER_SS_INT_EN |
			SOCLE_SPI_IER_RXAVAIL_INT_EN,
			SOCLE_SPI_IER);
			
	socle_spi_write(
			SOCLE_SPI_MODE_SLAVE |
			SOCLE_SPI_EN |
			cpol |
			cpha |
			SOCLE_SPI_TX_MSB_FIRST,
			SOCLE_SPI_FWCR);
	
	count=0;
	socle_spi_tx_complete_flag=1;
	socle_spi_rx_complete_flag=1;
	socle_spi_tx_finish_flag=1;
	
}


static void
socle_spi_slave_tx_dma_page_interrupt(void *data)
{	
	socle_spi_tx_complete_flag = 1;
}

static void
socle_spi_slave_rx_dma_page_interrupt(void *data)
{	
	socle_spi_rx_complete_flag = 1;
}

extern int
socle_spi_slave_pure_normal_run(int autotest)
{
	
	socle_spi_rx_complete_flag=0;
	socle_spi_tx_complete_flag=0;
	socle_spi_tx_finish_flag=0;
	/* Reset SPI controller */
	socle_spi_write(SOCLE_SPI_SOFT_RST, SOCLE_SPI_FWCR);
	
	request_irq(socle_spi_irq, socle_spi_slave_pure_isr, NULL);
	
	memset(storage, 0, PATTERN_BUF_SIZE);
	memset((u8 *)storage16, 0, PATTERN_BUF_SIZE);
	temp_addr=0;
	addr_temp=0;
	socle_spi_write(char_len, SOCLE_SPI_SSCR);
	
	/* Configure FIFO and clear Tx & Rx FIFO */
	socle_spi_write(
			SOCLE_SPI_RXFIFO_INT_TRIGGER_LEVEL_2 |
			SOCLE_SPI_TXFIFO_INT_TRIGGER_LEVEL_2 |
			SOCLE_SPI_RXFIFO_CLR |
			SOCLE_SPI_TXFIFO_CLR,
			SOCLE_SPI_FCR);
     
	/* Enable SPI interrupt */
	socle_spi_write(
			SOCLE_SPI_IER_RXFIFO_OVR_INT_EN |
			//SOCLE_SPI_IER_SS_INT_EN |
			SOCLE_SPI_IER_RXFIFO_INT_EN ,
			SOCLE_SPI_IER);
			
	socle_spi_write(
			SOCLE_SPI_MODE_SLAVE |
			SOCLE_SPI_EN |
			cpol |
			cpha |
			lsb,
			SOCLE_SPI_FWCR);
	printf("Wait data\n");
	
	if (socle_wait_for_int(&socle_spi_rx_complete_flag, 30)) {
		printf("Rx Timeout\n");
	}
	printf("Rx Finish\n");

	socle_spi_write(
			(socle_spi_read(SOCLE_SPI_IER) & ~SOCLE_SPI_IER_RXFIFO_INT_EN) |
			SOCLE_SPI_IER_TXFIFO_INT_EN,
			SOCLE_SPI_IER);
			
	if (socle_wait_for_int(&socle_spi_tx_complete_flag, 10)) {
		printf("Tx Timeout\n");
	}
	socle_spi_write(socle_spi_read(SOCLE_SPI_IER) |
						SOCLE_SPI_IER_TXFIFO_EMPTY_INT_EN,
						SOCLE_SPI_IER);
	if (socle_wait_for_int(&socle_spi_tx_finish_flag, 10)) {
		printf("Tx Timeout\n");
	}
	printf("Tx Finish\n");
	free_irq(socle_spi_irq);
	printf("Transfer End\n");
	
	return 0;

}

static int
socle_spi_slave_pure_hdma_run(u32 socle_spi_slave_dma_burst)
{
	
	socle_spi_write(SOCLE_SPI_SOFT_RST, SOCLE_SPI_FWCR);
	
	request_irq(socle_spi_irq, socle_spi_slave_pure_isr, NULL);
	socle_request_dma(socle_spi_rx_dma_ch_num, &socle_spi_slave_rx_dma_notifier);
	socle_request_dma(socle_spi_tx_dma_ch_num, &socle_spi_slave_tx_dma_notifier);
	socle_disable_dma(socle_spi_rx_dma_ch_num);
	socle_disable_dma(socle_spi_tx_dma_ch_num);
	
	socle_spi_tx_complete_flag = 0;
	socle_spi_rx_complete_flag = 0;
	socle_spi_tx_finish_flag=0;
	memset(storage, 0, PATTERN_BUF_SIZE);
	memset((u8 *)storage16, 0, PATTERN_BUF_SIZE);
	
	socle_spi_write(char_len, SOCLE_SPI_SSCR);
	
	/* Configure FIFO and clear Tx & Rx FIFO */
	socle_spi_write(
			SOCLE_SPI_RXFIFO_INT_TRIGGER_LEVEL_4 |
			SOCLE_SPI_TXFIFO_INT_TRIGGER_LEVEL_4 |
			SOCLE_SPI_RXFIFO_CLR |
			SOCLE_SPI_TXFIFO_CLR,
			SOCLE_SPI_FCR);
     
	/* Enable SPI interrupt */
	socle_spi_write(
			SOCLE_SPI_IER_RXFIFO_OVR_INT_EN,
			//SOCLE_SPI_IER_SS_INT_EN,
			SOCLE_SPI_IER);
			
	socle_spi_write(
			SOCLE_SPI_MODE_SLAVE |
			SOCLE_SPI_EN |
			cpol |
			cpha |
			lsb,
			SOCLE_SPI_FWCR);
	
	socle_set_dma_mode(socle_spi_rx_dma_ch_num, SOCLE_DMA_MODE_SLICE);
	socle_set_dma_ext_hdreq_number(socle_spi_rx_dma_ch_num, socle_spi_rx_dma_ext_hdreq);
	socle_set_dma_burst_type(socle_spi_rx_dma_ch_num, socle_spi_slave_dma_burst);
	socle_set_dma_source_address(socle_spi_rx_dma_ch_num, SOCLE_SPI_RXR+socle_spi_base);
	socle_set_dma_source_direction(socle_spi_rx_dma_ch_num, SOCLE_DMA_DIR_FIXED);
	socle_set_dma_destination_direction(socle_spi_rx_dma_ch_num, SOCLE_DMA_DIR_INCR);
	socle_set_dma_slice_count(socle_spi_rx_dma_ch_num, 4);
	socle_set_dma_page_number(socle_spi_rx_dma_ch_num, 1);
	if(char_len==SOCLE_SPI_CHAR_LEN_8) {
		socle_set_dma_transfer_count(socle_spi_rx_dma_ch_num, PATTERN_BUF_SIZE_PURE);
		socle_set_dma_destination_address(socle_spi_rx_dma_ch_num, (u32)&storage[0]);
		//socle_set_dma_destination_address(socle_spi_rx_dma_ch_num, (u32)storage);
		socle_set_dma_data_size(socle_spi_rx_dma_ch_num, SOCLE_DMA_DATA_BYTE);
	}
	else if(char_len==SOCLE_SPI_CHAR_LEN_16) {
		socle_set_dma_transfer_count(socle_spi_rx_dma_ch_num, PATTERN_BUF_SIZE_PURE*2);
		socle_set_dma_destination_address(socle_spi_rx_dma_ch_num, (u32)&storage16[0]);
		socle_set_dma_data_size(socle_spi_rx_dma_ch_num, SOCLE_DMA_DATA_HALFWORD);
	}
	
	socle_set_dma_mode(socle_spi_tx_dma_ch_num, SOCLE_DMA_MODE_SLICE);
	socle_set_dma_ext_hdreq_number(socle_spi_tx_dma_ch_num, socle_spi_tx_dma_ext_hdreq);
	socle_set_dma_burst_type(socle_spi_tx_dma_ch_num, socle_spi_slave_dma_burst);
	socle_set_dma_destination_address(socle_spi_tx_dma_ch_num, SOCLE_SPI_TXR+socle_spi_base);
	socle_set_dma_source_direction(socle_spi_tx_dma_ch_num, SOCLE_DMA_DIR_INCR);
	socle_set_dma_destination_direction(socle_spi_tx_dma_ch_num, SOCLE_DMA_DIR_FIXED);
	socle_set_dma_slice_count(socle_spi_tx_dma_ch_num, 4);
	socle_set_dma_page_number(socle_spi_tx_dma_ch_num, 1);
	if(char_len==SOCLE_SPI_CHAR_LEN_8) {
		socle_set_dma_transfer_count(socle_spi_tx_dma_ch_num, PATTERN_BUF_SIZE_PURE);
		socle_set_dma_source_address(socle_spi_tx_dma_ch_num, (u32)&storage[0]);
		//socle_set_dma_source_address(socle_spi_tx_dma_ch_num, (u32)storage);
		socle_set_dma_data_size(socle_spi_tx_dma_ch_num, SOCLE_DMA_DATA_BYTE);
	}
	else if(char_len==SOCLE_SPI_CHAR_LEN_16) {
		socle_set_dma_transfer_count(socle_spi_tx_dma_ch_num, PATTERN_BUF_SIZE_PURE*2);
		socle_set_dma_source_address(socle_spi_tx_dma_ch_num, (u32)&storage16[0]);
		socle_set_dma_data_size(socle_spi_tx_dma_ch_num, SOCLE_DMA_DATA_HALFWORD);
	}
	
	
	socle_enable_dma(socle_spi_rx_dma_ch_num);
	socle_spi_write(socle_spi_read(SOCLE_SPI_FWCR)| SOCLE_SPI_DMA_REQ, SOCLE_SPI_FWCR);
	
	printf("Wait data\n");
	if (socle_wait_for_int(&socle_spi_rx_complete_flag, 30)) {
		printf("DMA Rx Timeout\n");
		return -1;
	}
	socle_disable_dma(socle_spi_rx_dma_ch_num);
	
	printf("DMA Rx Finish\n");
	
	socle_enable_dma(socle_spi_tx_dma_ch_num);
	
	if (socle_wait_for_int(&socle_spi_tx_complete_flag, 10)) {
		printf("DMA Tx Timeout\n");
	}
	socle_disable_dma(socle_spi_tx_dma_ch_num);
	socle_spi_write(socle_spi_read(SOCLE_SPI_FWCR) & ~SOCLE_SPI_DMA_REQ, SOCLE_SPI_FWCR);
	
	socle_spi_write(socle_spi_read(SOCLE_SPI_IER) |
						SOCLE_SPI_IER_TXFIFO_EMPTY_INT_EN,
						SOCLE_SPI_IER);
	if (socle_wait_for_int(&socle_spi_tx_finish_flag, 10)) {
		printf("Tx Timeout\n");
		return -1;
	}
	
	printf("DMA Tx Finish\n");
	socle_free_dma(socle_spi_rx_dma_ch_num);
	socle_free_dma(socle_spi_tx_dma_ch_num);
	free_irq(socle_spi_irq);
	printf("Transfer End\n");

	return 0;
}

static void
socle_spi_slave_pure_isr(void *pparam)
{
	u32 tmp = socle_spi_read(SOCLE_SPI_ISR);
	
	if (SOCLE_SPI_RXFIFO_OVR_INT == (tmp & SOCLE_SPI_RXFIFO_OVR_INT)) {
		printf("\nReceive FIFO is full and another character has been received in the receiver shift register\n");
		return;
	}	

	if (SOCLE_SPI_RXFIFO_INT == (tmp & SOCLE_SPI_RXFIFO_INT)) {
		while (SOCLE_SPI_RXFIFO_DATA_AVAIL == (socle_spi_read(SOCLE_SPI_FCR) & SOCLE_SPI_RXFIFO_DATA_AVAIL)) {
				if(char_len==SOCLE_SPI_CHAR_LEN_8)
					storage[addr_temp] = socle_spi_read(SOCLE_SPI_RXR);
				else if(char_len==SOCLE_SPI_CHAR_LEN_16)
					storage16[addr_temp] = socle_spi_read(SOCLE_SPI_RXR);
				addr_temp++;
				if(addr_temp==PATTERN_BUF_SIZE_PURE) {
					socle_spi_rx_complete_flag=1;
					break;
				}
		}		
		return;
	}
 	
	if (SOCLE_SPI_TXFIFO_INT == (tmp & SOCLE_SPI_TXFIFO_INT)) {
		while (SOCLE_SPI_TXFIFO_FULL != (socle_spi_read(SOCLE_SPI_FCR) & SOCLE_SPI_TXFIFO_FULL)) {
			if(char_len==SOCLE_SPI_CHAR_LEN_8)
				socle_spi_write(storage[temp_addr], SOCLE_SPI_TXR);
			else if(char_len==SOCLE_SPI_CHAR_LEN_16)
				socle_spi_write(storage16[temp_addr], SOCLE_SPI_TXR);
			temp_addr++;
			if(temp_addr==PATTERN_BUF_SIZE_PURE) {
				socle_spi_write(socle_spi_read(SOCLE_SPI_IER) & ~SOCLE_SPI_IER_TXFIFO_INT_EN, SOCLE_SPI_IER);
				socle_spi_tx_complete_flag=1;
				break;
			}
		}
		return;
	}
	if (SOCLE_SPI_TXFIFO_EMPTY_INT == (tmp & SOCLE_SPI_TXFIFO_EMPTY_INT)) {
			socle_spi_write(socle_spi_read(SOCLE_SPI_IER) & ~SOCLE_SPI_IER_TXFIFO_EMPTY_INT_EN, SOCLE_SPI_IER);
			socle_spi_tx_finish_flag=1;
    	return; 
	}
}

static int
socle_spi_reg_rw_run(void)
{	
	request_irq(socle_spi_irq, socle_spi_slave_reg_isr, NULL);
	
	socle_spi_write(SOCLE_SPI_SOFT_RST, SOCLE_SPI_FWCR);
	socle_spi_write(char_len, SOCLE_SPI_SSCR);
	
	/* Enable SPI interrupt */
	socle_spi_write(
			SOCLE_SPI_IER_RX_REG0_INT_EN | 
			SOCLE_SPI_IER_RX_REG1_INT_EN |
			SOCLE_SPI_IER_RX_REG2_INT_EN | 
			SOCLE_SPI_IER_RX_REG3_INT_EN,
			SOCLE_SPI_IER);
			
	socle_spi_write(
			SOCLE_SPI_MODE_SLAVE |
			SOCLE_SPI_EN |
			cpol |
			cpha |
			lsb,
			SOCLE_SPI_FWCR);
	
	socle_spi_write(0x11, SOCLE_SPI_TX_REG0);
	socle_spi_write(0, SOCLE_SPI_TX_REG1);
	socle_spi_write(0, SOCLE_SPI_TX_REG2);
	socle_spi_write(0, SOCLE_SPI_TX_REG3);
	while(regflag);
	
	free_irq(socle_spi_irq);
	regflag=1;
	printf("Reg Test Finish\n");
	return 0;
}

static void
socle_spi_slave_reg_isr(void *pparam)
{
	u32 tmp = socle_spi_read(SOCLE_SPI_ISR);
	
	if (SOCLE_SPI_RX_REG0_INT == (tmp & SOCLE_SPI_RX_REG0_INT)) {
		socle_spi_write(SOCLE_SPI_RX_REG0_INT,SOCLE_SPI_ISR);
		u32 regvalue=socle_spi_read(SOCLE_SPI_RX_REG0);
		printf("RxReg0: %x\n", regvalue);
		if( 0xAA == regvalue)
			socle_spi_write(0x22, SOCLE_SPI_TX_REG1);				
		return;
	}	

	if (SOCLE_SPI_RX_REG1_INT == (tmp & SOCLE_SPI_RX_REG1_INT)) {
		socle_spi_write(SOCLE_SPI_RX_REG1_INT,SOCLE_SPI_ISR);
		u32 regvalue=socle_spi_read(SOCLE_SPI_RX_REG1);
		printf("RxReg1: %x\n", regvalue);
		if( 0xBB == regvalue)
			socle_spi_write(0x33, SOCLE_SPI_TX_REG2);				
		return;
	}
 	
	if (SOCLE_SPI_RX_REG2_INT == (tmp & SOCLE_SPI_RX_REG2_INT)) {
		socle_spi_write(SOCLE_SPI_RX_REG2_INT,SOCLE_SPI_ISR);
		u32 regvalue=socle_spi_read(SOCLE_SPI_RX_REG2);
		printf("RxReg2: %x\n", regvalue);
		if( 0xCC == regvalue)
			socle_spi_write(0x44, SOCLE_SPI_TX_REG3);				
		return;
	}
	if (SOCLE_SPI_RX_REG3_INT == (tmp & SOCLE_SPI_RX_REG3_INT)) {
		socle_spi_write(SOCLE_SPI_RX_REG3_INT,SOCLE_SPI_ISR);
		u32 regvalue=socle_spi_read(SOCLE_SPI_RX_REG3);
		printf("RxReg3: %x\n", regvalue);
		if( 0xDD == regvalue)
			regflag=0;				
		return;
	}
}
