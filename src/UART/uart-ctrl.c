#include <genlib.h>
#include <global.h>
#include <test_item.h>
#include <type.h>
#include <io.h>
#include <dma/dma.h>
#include "uart-regs.h"
#include "dependency.h"

#if defined (CONFIG_PC9220)
#include "../../platform/arch/scu-reg.h"
#endif

#if defined (CONFIG_PC7210) || defined (CONFIG_PDK) || defined (CONFIG_PC9220)
#include <scu.h>
#endif

static inline void
socle_uart_write(u32 reg, u32 value, u32 base)
{
	base = base + reg;
	iowrite32(value, base);
}

static inline u32
socle_uart_read(u32 reg, u32 base)
{
	u32 val;

	base = base + reg;
	val = ioread32(base);
	return val;
}

static void socle_uart_set_baudrate(int baudrate);
static int socle_uart_normal(int autotest);
static int socle_uart_hwdma_panther7_hdma(int autotest);
static void socle_uart_make_test_pattern(u8 *buffer, u32 size);
static void socle_uart_isr(void *pparam);
static void socle_uart_show_lsr_error(void);
static int socle_uart_compare_memory(u8 *mem, u8 *cmpr_mem, u32 size,
				     int skip_cmpr_result);
static void socle_uart_init_environment(int dma_req_en);
extern int socle_uart_hwdma_panther7_hdma_bursttype_test(int autotest);
static void socle_uart_tx_dma_page_interrupt(void *data);
static void socle_uart_rx_dma_page_interrupt(void *data);

static struct socle_dma_notifier socle_uart_tx_dma_notifier = {
	.page_interrupt = socle_uart_tx_dma_page_interrupt,
};

static struct socle_dma_notifier socle_uart_rx_dma_notifier = {
	.page_interrupt = socle_uart_rx_dma_page_interrupt,
};

static u32 socle_uart_tx_dma_ch_num, socle_uart_rx_dma_ch_num;
static u32 socle_uart_tx_dma_ext_hdreq, socle_uart_rx_dma_ext_hdreq;
static u32 socle_uart_dma_burst;
static u32 socle_uart_base;
int (*uart_test)(int autotest);

extern struct test_item_container socle_uart_ip_test_container;

extern int 
UARTTesting(int autotest)
{
	int ret = 0;
	
/*	UART clock enable	*/
#if defined(CONFIG_PDK) || defined(CONFIG_PC7210)
	socle_scu_upll_normal();
#endif

	ret = test_item_ctrl(&socle_uart_ip_test_container, autotest);
	return ret;
}

extern struct test_item_container socle_uart_type_test_container;

extern int
socle_uart_0_test(int autotest)
{
	int ret = 0;

	socle_uart_base = SOCLE_APB0_UART0;

#ifdef CONFIG_PC9220
	socle_scu_dev_enable(SOCLE_DEVCON_UART0);
	socle_scu_hdma_req01_uart(0);
	socle_scu_hdma_req23_uart(0);
#endif

	{
		extern struct test_item socle_uart_module_test_items[];
		
		socle_uart_module_test_items[0].enable = SOCLE_UART_GPS_0_TEST;
	}
	//2008 jsho add, for memu control
	{
		extern struct test_item socle_uart_transfer_test_items[];
		socle_uart_transfer_test_items[1].enable = SOCLE_UART_EXT_0_TEST;
	}
	/* Disable all interrupts */
	socle_uart_write(SOCLE_UART_IER,
			 SOCLE_UART_IE_MS_DIS |
			 SOCLE_UART_IE_LS_DIS |
			 SOCLE_UART_IE_TBE_DIS |
			 SOCLE_UART_IE_RBR_DIS,
			 socle_uart_base);

	/* Read IIR and LSR to clear the pending interrupt */
	socle_uart_read(SOCLE_UART_IIR, socle_uart_base);
	socle_uart_read(SOCLE_UART_LSR, socle_uart_base);

	request_irq(SOCLE_INTC_UART0, socle_uart_isr, NULL);

	socle_uart_tx_dma_ext_hdreq = 1; 
	socle_uart_rx_dma_ext_hdreq = 0;

	ret = test_item_ctrl(&socle_uart_type_test_container, autotest);

	free_irq(SOCLE_INTC_UART0);

#ifdef CONFIG_PC9220
	socle_scu_dev_disable(SOCLE_DEVCON_UART0);
#endif
	return ret;
}

extern int
socle_uart_1_test(int autotest)
{
	int ret = 0;

	socle_uart_base = SOCLE_APB0_UART1;

#ifdef CONFIG_PC9220
	socle_scu_dev_enable(SOCLE_DEVCON_UART1);
	socle_scu_hdma_req01_uart(1);
	socle_scu_hdma_req23_uart(1);
#endif

	{
		extern struct test_item socle_uart_module_test_items[];
		
		socle_uart_module_test_items[0].enable = SOCLE_UART_GPS_1_TEST;
	}
	//2008 jsho add, for memu control
	{
		extern struct test_item socle_uart_transfer_test_items[];
		socle_uart_transfer_test_items[1].enable = SOCLE_UART_EXT_1_TEST;
	}
	/* Disable all interrupts */
	socle_uart_write(SOCLE_UART_IER,
			 SOCLE_UART_IE_MS_DIS |
			 SOCLE_UART_IE_LS_DIS |
			 SOCLE_UART_IE_TBE_DIS |
			 SOCLE_UART_IE_RBR_DIS,
			 socle_uart_base);

	/* Read IIR and LSR to clear the pending interrupt */
	socle_uart_read(SOCLE_UART_IIR, socle_uart_base);
	socle_uart_read(SOCLE_UART_LSR, socle_uart_base);

	request_irq(SOCLE_INTC_UART1, socle_uart_isr, NULL);

	socle_uart_tx_dma_ext_hdreq = 3; 
	socle_uart_rx_dma_ext_hdreq = 2;

#if defined(CONFIG_PDK) || defined(CONFIG_PC7210)
	socle_scu_hdma_req23_uart(1);
#endif

	ret = test_item_ctrl(&socle_uart_type_test_container, autotest);

	free_irq(SOCLE_INTC_UART1);

#ifdef CONFIG_PC9220
        socle_scu_dev_disable(SOCLE_DEVCON_UART1);
#endif

	return ret;
}

extern int
socle_uart_2_test(int autotest)
{
	int ret = 0;

	socle_uart_base = SOCLE_APB0_UART2;

#ifdef CONFIG_PC9220
        socle_scu_dev_enable(SOCLE_DEVCON_UART2);
        socle_scu_hdma_req01_uart(2);
        socle_scu_hdma_req23_uart(2);
#endif

	{
		extern struct test_item socle_uart_module_test_items[];
		
		socle_uart_module_test_items[0].enable = SOCLE_UART_GPS_2_TEST;
	}
	//2008 jsho add, for memu control
	{
		extern struct test_item socle_uart_transfer_test_items[];
		socle_uart_transfer_test_items[1].enable = SOCLE_UART_EXT_2_TEST;
	}
	/* Disable all interrupts */
	socle_uart_write(SOCLE_UART_IER,
			 SOCLE_UART_IE_MS_DIS |
			 SOCLE_UART_IE_LS_DIS |
			 SOCLE_UART_IE_TBE_DIS |
			 SOCLE_UART_IE_RBR_DIS,
			 socle_uart_base);

	/* Read IIR and LSR to clear the pending interrupt */
	socle_uart_read(SOCLE_UART_IIR, socle_uart_base);
	socle_uart_read(SOCLE_UART_LSR, socle_uart_base);

	request_irq(SOCLE_INTC_UART2, socle_uart_isr, NULL);

	socle_uart_tx_dma_ext_hdreq = 3; 
	socle_uart_rx_dma_ext_hdreq = 2;

#if defined(CONFIG_PDK) || defined(CONFIG_PC7210)
	socle_scu_hdma_req23_uart(2);
#endif

	ret = test_item_ctrl(&socle_uart_type_test_container, autotest);

	free_irq(SOCLE_INTC_UART2);

#ifdef CONFIG_PC9220
        socle_scu_dev_disable(SOCLE_DEVCON_UART2);
#endif

	return ret;
}

extern int
socle_uart_3_test(int autotest)
{
	int ret = 0;

	socle_uart_base = SOCLE_APB0_UART3;

	{
		extern struct test_item socle_uart_module_test_items[];
		
		socle_uart_module_test_items[0].enable = SOCLE_UART_GPS_3_TEST;
	}
	//2008 jsho add, for memu control
	{
		extern struct test_item socle_uart_transfer_test_items[];
		socle_uart_transfer_test_items[1].enable = SOCLE_UART_EXT_3_TEST;
	}
	/* Disable all interrupts */
	socle_uart_write(SOCLE_UART_IER,
			 SOCLE_UART_IE_MS_DIS |
			 SOCLE_UART_IE_LS_DIS |
			 SOCLE_UART_IE_TBE_DIS |
			 SOCLE_UART_IE_RBR_DIS,
			 socle_uart_base);

	/* Read IIR and LSR to clear the pending interrupt */
	socle_uart_read(SOCLE_UART_IIR, socle_uart_base);
	socle_uart_read(SOCLE_UART_LSR, socle_uart_base);

	request_irq(SOCLE_INTC_UART3, socle_uart_isr, NULL);

	socle_uart_tx_dma_ext_hdreq = 3; 
	socle_uart_rx_dma_ext_hdreq = 2;

#if defined(CONFIG_PDK) || defined(CONFIG_PC7210)
	socle_scu_hdma_req23_uart(3);
#endif

	ret = test_item_ctrl(&socle_uart_type_test_container, autotest);

	free_irq(SOCLE_INTC_UART3);

	return ret;
}

extern struct test_item_container socle_uart_transfer_test_container;

extern int
socle_uart_normal_test(int autotest)
{
	int ret = 0;

	uart_test = socle_uart_normal;
	ret = test_item_ctrl(&socle_uart_transfer_test_container, autotest);
	return ret;
}

extern struct test_item_container socle_uart_panther7_hdma_bursttype_test_container;

extern int 
socle_uart_hwdma_panther7_hdma_test(int autotest)
{
	int ret = 0;

	uart_test = socle_uart_hwdma_panther7_hdma_bursttype_test;
	socle_uart_tx_dma_ch_num = PANTHER7_HDMA_CH_0;
	socle_uart_rx_dma_ch_num = PANTHER7_HDMA_CH_1;
	socle_request_dma(socle_uart_tx_dma_ch_num, &socle_uart_tx_dma_notifier);
	socle_request_dma(socle_uart_rx_dma_ch_num, &socle_uart_rx_dma_notifier);
	ret = test_item_ctrl(&socle_uart_transfer_test_container, autotest);
	socle_disable_dma(socle_uart_tx_dma_ch_num);
	socle_disable_dma(socle_uart_rx_dma_ch_num);
	socle_free_dma(socle_uart_tx_dma_ch_num);
	socle_free_dma(socle_uart_rx_dma_ch_num);
	return ret;
}

extern struct test_item_container socle_uart_panther7_hdma_bursttype_test_container;

extern int socle_uart_hwdma_panther7_hdma_bursttype_test(int autotest)
{
	int ret = 0;

	ret = test_item_ctrl(&socle_uart_panther7_hdma_bursttype_test_container, autotest);
	return ret;
}

extern int 
socle_uart_panther7_hdma_bursttype_single(int autotest)
{
	socle_uart_dma_burst = SOCLE_DMA_BURST_SINGLE;
	return socle_uart_hwdma_panther7_hdma(autotest);
}

extern int 
socle_uart_panther7_hdma_bursttype_incr4(int autotest)
{
	socle_uart_dma_burst = SOCLE_DMA_BURST_INCR4;
	return socle_uart_hwdma_panther7_hdma(autotest);
}

extern int 
socle_uart_panther7_hdma_bursttype_incr8(int autotest)
{
	socle_uart_dma_burst = SOCLE_DMA_BURST_INCR8;
	return socle_uart_hwdma_panther7_hdma(autotest);
}

extern struct test_item_container socle_uart_baudrate_test_container;

extern int
socle_uart_inter_loopback_test(int autotest)
{
	int ret = 0;

	socle_uart_write(SOCLE_UART_MCR, SOCLE_UART_MCR_LOOPBACK, socle_uart_base);
	ret = test_item_ctrl(&socle_uart_baudrate_test_container, autotest);
	return ret;
}

extern int
socle_uart_ext_loopback_test(int autotest)
{
	int ret = 0;

	socle_uart_write(SOCLE_UART_MCR, SOCLE_UART_MCR_NORMAL, socle_uart_base);
	ret = test_item_ctrl(&socle_uart_baudrate_test_container, autotest);

	return ret;
}

extern struct test_item_container socle_uart_module_test_container;

extern int
socle_uart_module_test(int autotest)
{
	int ret = 0;

	socle_uart_write(SOCLE_UART_MCR, SOCLE_UART_MCR_NORMAL, socle_uart_base);
	ret = test_item_ctrl(&socle_uart_module_test_container, autotest);

	return ret;
}

extern int
socle_uart_gps_test(int autotest)
{
	int ret = 0, i = 0, tmp[1000]={0}, idx = 0;

	socle_uart_set_baudrate(9600);

	/* Initialize the test environment */
	socle_uart_init_environment(0);

	printf("GPS Data:\n");
	while (i < 1000) {
		if (SOCLE_UART_LSR_DR == (socle_uart_read(SOCLE_UART_LSR, socle_uart_base) & SOCLE_UART_LSR_DR)) {
			while (SOCLE_UART_LSR_DR == (socle_uart_read(SOCLE_UART_LSR, socle_uart_base) & SOCLE_UART_LSR_DR)) {
				tmp[idx] = socle_uart_read(SOCLE_UART_RBR, socle_uart_base);
				idx++;
				//printf("%d(%c) ", tmp[idx], tmp[idx]);
			}
			//printf("\n");
			i++;
		}
		if (idx >= 1000)
			break;
	}

	for (i = 0; i < idx; i++) {
		if ((10 == tmp[i]) || (13 == tmp[i]))
			printf("\n");
		else
			printf("%c", tmp[i]);
//			printf("%d(%c) ", tmp[i], tmp[i]);
	}
	printf("\n");

	return ret;
}


extern int
socle_uart_baudrate_75(int autotest)
{
	socle_uart_set_baudrate(75);
	return uart_test(autotest);
}

extern int
socle_uart_baudrate_300(int autotest)
{
	socle_uart_set_baudrate(300);
	return uart_test(autotest);
}

extern int
socle_uart_baudrate_1200(int autotest)
{
	socle_uart_set_baudrate(1200);
	return uart_test(autotest);
}

extern int
socle_uart_baudrate_2400(int autotest)
{
	socle_uart_set_baudrate(2400);
	return uart_test(autotest);
}

extern int
socle_uart_baudrate_9600(int autotest)
{
	socle_uart_set_baudrate(9600);
	return uart_test(autotest);
}

extern int
socle_uart_baudrate_19200(int autotest)
{
	socle_uart_set_baudrate(19200);
	return uart_test(autotest);
}

extern int
socle_uart_baudrate_38400(int autotest)
{
	socle_uart_set_baudrate(38400);
	return uart_test(autotest);
}

extern int
socle_uart_baudrate_57600(int autotest)
{
	socle_uart_set_baudrate(57600);
	return uart_test(autotest);
}

extern int
socle_uart_baudrate_115200(int autotest)
{
	socle_uart_set_baudrate(115200);
	return uart_test(autotest);
}

static void
socle_uart_set_baudrate(int baudrate)
{
	u32 old_lcr;
	u32 dlsb, dmsb;

	dlsb = SOCLE_UART_EXT_CLK / (16 * baudrate);
	dmsb = dlsb >> 8;
	dlsb &= 0xff;
	old_lcr = socle_uart_read(SOCLE_UART_LCR, socle_uart_base);
	socle_uart_write(SOCLE_UART_LCR, old_lcr|SOCLE_UART_LCR_DIV_LATCH_EN, socle_uart_base);
	socle_uart_write(SOCLE_UART_DLL, dlsb, socle_uart_base);
	socle_uart_write(SOCLE_UART_DLH, dmsb, socle_uart_base);
	socle_uart_write(SOCLE_UART_LCR, old_lcr&(~SOCLE_UART_LCR_DIV_LATCH_EN), socle_uart_base);
}

#define PATTERN_BUF_ADDR 0x00a00000
#define PATTERN_BUF_SIZE 2048

static u8 *socle_uart_pattern_buf = (u8 *)PATTERN_BUF_ADDR;
static u32 socle_uart_pattern_buf_idx = 0;
static u8 *socle_uart_cmpr_buf = (u8 *)(PATTERN_BUF_ADDR + PATTERN_BUF_SIZE);
static u32 socle_uart_cmpr_buf_idx = 0;
static volatile int socle_uart_tx_complete_flag = 0;
static volatile int socle_uart_rx_complete_flag = 0;

static int
socle_uart_normal(int autotest)
{
	/* Initialize the test environment */
	socle_uart_init_environment(0);

	/* Make the test pattern */
	socle_uart_make_test_pattern(socle_uart_pattern_buf, PATTERN_BUF_SIZE);

	/* Trigger the transfer */
	socle_uart_write(SOCLE_UART_THR, socle_uart_pattern_buf[socle_uart_pattern_buf_idx++], socle_uart_base);

	/* Enable specify interrupts */
	socle_uart_write(SOCLE_UART_IER,
			 SOCLE_UART_IE_MS_DIS |
			 SOCLE_UART_IE_LS_EN |
			 SOCLE_UART_IE_TBE_EN |
			 SOCLE_UART_IE_RBR_EN,
			 socle_uart_base);

	/* Wait for transfer to be complete */
	if (socle_wait_for_int(&socle_uart_rx_complete_flag, 10)) {
		printf("Timeout\n");
		return -1;
	}

	if (-1 == socle_uart_rx_complete_flag)
		return -1; 
	else
		/* Compare the memory */
		return socle_uart_compare_memory(socle_uart_pattern_buf, socle_uart_cmpr_buf, PATTERN_BUF_SIZE,
						 autotest);
}

static void
socle_uart_make_test_pattern(u8 *buf, u32 size)
{
	int i;

	for (i = 0; i < size; i++)
		buf[i] = i;
}

static void 
socle_uart_isr(void* pparam)
{
	u32 tmp;
	int i;

	do {
		/* Read & clear the interrupt status */
		tmp = socle_uart_read(SOCLE_UART_IIR, socle_uart_base);

		/* Check if any error occur */
		if (SOCLE_UART_IIR_LS_INT == (tmp & SOCLE_UART_IIR_LS_INT)) {
			socle_uart_rx_complete_flag = -1;
			socle_uart_show_lsr_error();
			return;
		}
	
		/* Check if it is receive data interrupt or not */
		if ((SOCLE_UART_IIR_RBR_INT == (tmp & SOCLE_UART_IIR_RBR_INT)) || 
		    (SOCLE_UART_IIR_TO_INT == (tmp & SOCLE_UART_IIR_TO_INT))) {
//		    	printf("cyli test rx %d\n", socle_uart_cmpr_buf_idx);
			while (SOCLE_UART_LSR_DR == (socle_uart_read(SOCLE_UART_LSR, socle_uart_base) & SOCLE_UART_LSR_DR))
				socle_uart_cmpr_buf[socle_uart_cmpr_buf_idx++] = socle_uart_read(SOCLE_UART_RBR, socle_uart_base);
			if (PATTERN_BUF_SIZE == socle_uart_cmpr_buf_idx) {
				socle_uart_rx_complete_flag = 1;
				return;
			}
		}

		/* Check if it is the transmit buffer empty interrupt or not */
		if (SOCLE_UART_IIR_TBE_INT == (tmp & SOCLE_UART_IIR_TBE_INT)) {
			if (socle_uart_pattern_buf_idx > PATTERN_BUF_SIZE) {
				socle_uart_rx_complete_flag = -1;
				printf("\nTransfered data is more than the size of test pattern: %d\n", socle_uart_pattern_buf_idx);
				return;
	
			}
			
			if (PATTERN_BUF_SIZE == socle_uart_pattern_buf_idx) {
				return;
			}else {
				if (1 == socle_uart_pattern_buf_idx) {
					for (i = 0; i < (SOCLE_UART_FIFO_SIZE - 1); i++)
						socle_uart_write(SOCLE_UART_THR, socle_uart_pattern_buf[socle_uart_pattern_buf_idx++], socle_uart_base);
				} else {
					for (i = 0; i < SOCLE_UART_FIFO_SIZE; i++) 
						socle_uart_write(SOCLE_UART_THR, socle_uart_pattern_buf[socle_uart_pattern_buf_idx++], socle_uart_base);
				}
			}
		}
	} while (0 == (tmp & SOCLE_UART_IIR_PEND_INT));
}

static void
socle_uart_show_lsr_error(void)
{
	u32 lsr;

	lsr = socle_uart_read(SOCLE_UART_LSR, socle_uart_base);
	if (SOCLE_UART_LSR_ERR == (lsr & SOCLE_UART_LSR_ERR))
		printf("\nSocle UART host:At least one parity error, framming error or break indications have been received and are inside the FIFO\n");
	if (SOCLE_UART_LSR_TEMT == (lsr & SOCLE_UART_LSR_TEMT))
		printf("\nSocle UART host: Transmitter Empty indicator\n");
	if (SOCLE_UART_LSR_THRE == (lsr & SOCLE_UART_LSR_THRE))
		printf("\nSocle UART host: Transmit FIFO is empty\n");
	if (SOCLE_UART_LSR_BI == (lsr & SOCLE_UART_LSR_BI))
		printf("\nSocle UART host: Break Interrupt indicator\n");
	if (SOCLE_UART_LSR_FE == (lsr & SOCLE_UART_LSR_FE))
		printf("\nSocle UART host: Framming Error (FE) indicator\n");
	if (SOCLE_UART_LSR_PE == (lsr & SOCLE_UART_LSR_PE))
		printf("\nSocle UART host: Parity Error indicator\n");
	if (SOCLE_UART_LSR_OE == (lsr & SOCLE_UART_LSR_OE))
		printf("\nSocle UART host: Overrun Error indicator\n");
	if (SOCLE_UART_LSR_DR == (lsr & SOCLE_UART_LSR_DR))
		printf("\nSocle UART host: Data Ready indicator\n");
}

static int
socle_uart_compare_memory(u8 *mem, u8 *cmpr_mem, u32 size,
			  int skip_cmpr_result)
{
	int i;
	u32 *mem32 = (u32 *)mem;
	u32 *cmpr_mem32 = (u32 *)cmpr_mem;
	int err_flag = 0;

	for (i = 0; i < (size >> 2); i++) {
		if (mem32[i] != cmpr_mem32[i]) {
			err_flag |= -1;
			if (!skip_cmpr_result) {
				printf("\nWord %d, 0x%08x (0x%08x) != 0x%08x (0x%08x)", i, mem32[i], &mem32[i], 
				       cmpr_mem32[i], &cmpr_mem32[i]);
			}
		}
	}
	printf("\n");
	return err_flag;
}

static void
socle_uart_init_environment(int dma_req_en)
{
	/* Reset the variables */
	socle_uart_rx_complete_flag = 0;
	socle_uart_tx_complete_flag = 0;
	socle_uart_pattern_buf_idx = 0;
	socle_uart_cmpr_buf_idx = 0;

	/* Clear the buffers */
	memset(socle_uart_pattern_buf, 0x0, PATTERN_BUF_SIZE);     
	memset(socle_uart_cmpr_buf, 0x0, PATTERN_BUF_SIZE);     


	/* Read & clear the interrupt status */
	(void)socle_uart_read(SOCLE_UART_IIR, socle_uart_base);	

	/* Use external clock */
	socle_uart_write(SOCLE_UART_CTRL, 0x1, socle_uart_base);

	/* Setup the FIFO control */
	socle_uart_write(SOCLE_UART_FCR,
			 SOCLE_UART_FCR_THRESH_8 |
			 SOCLE_UART_FCR_TX_CLR |
			 SOCLE_UART_FCR_RX_CLR |
			 SOCLE_UART_FCR_FIFO_EN |
			 (dma_req_en << 3),
			 socle_uart_base);

	/* Setup the Line control */
	socle_uart_write(SOCLE_UART_LCR,
			 SOCLE_UART_LCR_DIV_LATCH_DIS |
			 SOCLE_UART_LCR_BRK_DIS |
			 SOCLE_UART_LCR_STICK_DIS |
			 SOCLE_UART_LCR_PARITY_ODD |
			 SOCLE_UART_LCR_PARITY_DIS |
			 SOCLE_UART_LCR_STOP_1 |
			 SOCLE_UART_LCR_NBITS_8,
			 socle_uart_base);
}

static int 
socle_uart_hwdma_panther7_hdma(int autotest)
{
	/* Initialize the test environment */
	socle_uart_init_environment(1);

	/* Enable specify interrupts */
	socle_uart_write(SOCLE_UART_IER,
			 SOCLE_UART_IE_MS_DIS |
			 SOCLE_UART_IE_LS_EN |
			 SOCLE_UART_IE_TBE_DIS |
			 SOCLE_UART_IE_RBR_DIS,
			 socle_uart_base);

	/* Make the test pattern */
	socle_uart_make_test_pattern(socle_uart_pattern_buf, PATTERN_BUF_SIZE);

	/* Configure the hardware dma settng of HDMA for tx channels */
	socle_disable_dma(socle_uart_tx_dma_ch_num);
	socle_set_dma_mode(socle_uart_tx_dma_ch_num, SOCLE_DMA_MODE_SLICE);
	socle_set_dma_ext_hdreq_number(socle_uart_tx_dma_ch_num, socle_uart_tx_dma_ext_hdreq);
	socle_set_dma_burst_type(socle_uart_tx_dma_ch_num, socle_uart_dma_burst);
	socle_set_dma_source_address(socle_uart_tx_dma_ch_num, (u32)socle_uart_pattern_buf);
	socle_set_dma_destination_address(socle_uart_tx_dma_ch_num, SOCLE_UART_THR+socle_uart_base);
	socle_set_dma_source_direction(socle_uart_tx_dma_ch_num, SOCLE_DMA_DIR_INCR);
	socle_set_dma_destination_direction(socle_uart_tx_dma_ch_num, SOCLE_DMA_DIR_FIXED);
	socle_set_dma_data_size(socle_uart_tx_dma_ch_num, SOCLE_DMA_DATA_BYTE);
	socle_set_dma_transfer_count(socle_uart_tx_dma_ch_num, PATTERN_BUF_SIZE);
	socle_set_dma_slice_count(socle_uart_tx_dma_ch_num, 8);
	socle_set_dma_page_number(socle_uart_tx_dma_ch_num, 1);
	socle_set_dma_buffer_size(socle_uart_tx_dma_ch_num, PATTERN_BUF_SIZE);

	/* Configure the hardware dma settng of HDMA for rx channels */
	socle_disable_dma(socle_uart_rx_dma_ch_num);
	socle_set_dma_mode(socle_uart_rx_dma_ch_num, SOCLE_DMA_MODE_SLICE);
	socle_set_dma_ext_hdreq_number(socle_uart_rx_dma_ch_num, socle_uart_rx_dma_ext_hdreq);
	socle_set_dma_burst_type(socle_uart_rx_dma_ch_num, socle_uart_dma_burst);
	socle_set_dma_source_address(socle_uart_rx_dma_ch_num, SOCLE_UART_RBR+socle_uart_base);
	socle_set_dma_destination_address(socle_uart_rx_dma_ch_num, (u32)socle_uart_cmpr_buf);
	socle_set_dma_source_direction(socle_uart_rx_dma_ch_num, SOCLE_DMA_DIR_FIXED);
	socle_set_dma_destination_direction(socle_uart_rx_dma_ch_num, SOCLE_DMA_DIR_INCR);
	socle_set_dma_data_size(socle_uart_rx_dma_ch_num, SOCLE_DMA_DATA_BYTE);
	socle_set_dma_transfer_count(socle_uart_rx_dma_ch_num, PATTERN_BUF_SIZE);
	socle_set_dma_slice_count(socle_uart_rx_dma_ch_num, 8);
	socle_set_dma_page_number(socle_uart_rx_dma_ch_num, 1);
	socle_set_dma_buffer_size(socle_uart_rx_dma_ch_num, PATTERN_BUF_SIZE);

	/* Enable the dma to run*/
	socle_enable_dma(socle_uart_tx_dma_ch_num);
	socle_enable_dma(socle_uart_rx_dma_ch_num);

	/* Wait for transfer to be complete */
	if (socle_wait_for_int(&socle_uart_rx_complete_flag, 80)) {
		printf("Timeout\n");
		return -1;
	}

	if (-1 == socle_uart_rx_complete_flag)
		return -1; 
	else
		/* Compare the memory */
		return socle_uart_compare_memory(socle_uart_pattern_buf, socle_uart_cmpr_buf, PATTERN_BUF_SIZE,
						 autotest);
}

static void 
socle_uart_tx_dma_page_interrupt(void *data)
{
	socle_uart_tx_complete_flag = 1;
}

static void 
socle_uart_rx_dma_page_interrupt(void *data)
{
	socle_uart_rx_complete_flag = 1;
}











