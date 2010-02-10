#include <global.h>
#include <genlib.h>
#include <irqs.h>
#include <platform.h>
#include <test_item.h>
#include <dma/dma.h>
#include <revision.h>

#ifdef CONFIG_GPIO
#include <GPIO/gpio.h>
#endif

#ifdef CONFIG_MP_GPIO
#include <MP_GPIO/mp-gpio.h>
#endif 

#ifdef CONFIG_ADC_PWM
#include <ADC_PWM/pwmt-regs.h>
#endif

#ifdef CONFIG_MIPS
#include <mips.h>
#include <ArchDefs.h>
#endif

#ifdef SEMI_HOST_FUNCTION
#include <epios.h>	//For EPI SemiHost HIF Function
#endif



#if defined (CONFIG_PC7210) || defined (CONFIG_PDK) || defined (CONFIG_PC9220)
#include <scu.h>
#endif

#if 0
#define BENCHMARK_TESTING
#endif

#define SCU_AHB_MODE (0x1<<6)

extern struct test_item_container main_test_item_container;
int cpu_clk_mhz;
char cpu_type[] = CPU_TYPE;
int socle_memory_addr_start = SOCLE_MEMORY_ADDR_START;

/*============================================*/

#if defined(SEMI_HOST_FUNCTION_ARM) && !defined(UART_DEBUG)
static char f_name[]={':','t','t','\0'};
static u32_t sysopen_array[]={(u32_t)f_name,1,3};
static int
semihost_init(u32_t* array)
{
	int c;
	__asm__ __volatile__ (
		"mov r0,#1 \n"
		"mov r1,%[input]\n"
		"swi #0x123456\n"
		"mov %[output],r0"
		:[output] "=r" (c)
		:[input] "r" (array));
	return c;
}
#endif	//end SEMI_HOST_FUNCTION_ARM

static void
socle_init_interrupt(void)
{
	// set upt default handle to system (CPU) interrupt
#if defined(CPU_ARCH_MIPS)
	initSysIntTrap();
#endif
	// initial interrupt controller
	init_irq_controller();
	init_fiq_controller();

#if defined(CPU_ARCH_MIPS)
	/* do not enable INT 6, this is for internal timer interrupt source (for 4Kc) */
	EnableInt_x(M_StatusIM0 | M_StatusIM1 | M_StatusIM2 | M_StatusIM3 |
	            M_StatusIM4 | M_StatusIM5 | M_StatusIM7);
#elif defined(CPU_ARCH_ARM)
	enable_irq();
	enable_fiq();
#endif
}


#if defined(SEMI_HOST_FUNCTION_ARM) && defined(UART_DEBUG)

#define FCR_FIFO_EN     0x01            /* Fifo enable */
#define FCR_RXSR        0x02            /* Receiver soft reset */
#define FCR_TXSR        0x04            /* Transmitter soft reset */

#define MCR_DTR         0x01
#define MCR_RTS         0x02
#define MCR_DMA_EN      0x04
#define MCR_TX_DFR      0x08

#define LCR_WLS_MSK     0x03            /* character length slect mask */
#define LCR_WLS_5       0x00            /* 5 bit character length */
#define LCR_WLS_6       0x01            /* 6 bit character length */
#define LCR_WLS_7       0x02            /* 7 bit character length */
#define LCR_WLS_8       0x03            /* 8 bit character length */
#define LCR_STB         0x04            /* Number of stop Bits, off = 1, on = 1.5 or 2) */
#define LCR_PEN         0x08            /* Parity eneble */
#define LCR_EPS         0x10            /* Even Parity Select */
#define LCR_STKP        0x20            /* Stick Parity */
#define LCR_SBRK        0x40            /* Set Break */
#define LCR_BKSE        0x80            /* Bank select enable */

/* useful defaults for LCR */
#define LCR_8N1		0x03

#define LCRVAL	LCR_8N1                                  /* 8 data, 1 stop, no parity */
#define MCRVAL	(MCR_DTR | MCR_RTS)                      /* RTS/DTR */
#define FCRVAL	(FCR_FIFO_EN | FCR_RXSR | FCR_TXSR )     /* Clear & enable FIFOs */


static void
ns16550_init (int baud_divisor)
{
        volatile unsigned long *p;

#if defined(CONFIG_PDK) || defined(CONFIG_PC7210)
	if(((ioread32(SOCLE_APB0_SCU+0xc) >> 13) & 0x7) !=6)		//20071231 leonid+ for check scu_ucfg
		p=(unsigned long *)SOCLE_APB0_UART1;
	else
		p=(unsigned long *)SOCLE_APB0_UART0;
#else
		p=(unsigned long *)SOCLE_APB0_UART0;
#endif

        *(p+1) = 0x0;
        *(p+3) = LCR_BKSE | LCRVAL;
        /* MSB first */
        *(p+1) = (baud_divisor >> 8) & 0xff;
        *p = baud_divisor & 0xff;
        *(p+3) = LCRVAL;
        *(p+4) = MCRVAL;
        *(p+2) = FCRVAL;
}
#endif

static void
socle_init_platform(void)
{
#ifdef CONFIG_INR_PC7230
// 20080714 cyli add for INR
#ifndef CONFIG_GPIO
#error Does not select CONFIG_GPIO
#endif
#define DEVICES_POWER_ON()		socle_gpio_set_value_with_mask(PA, SHIFT_MASK(0), SHIFT_MASK(0))	// PA0 = 1
#define DEVICES_POWER_OFF()	socle_gpio_set_value_with_mask(PA, 0, SHIFT_MASK(0))				// PA0 = 0

#define STANDBY_LED_ON()		socle_gpio_set_value_with_mask(PA, SHIFT_MASK(3), SHIFT_MASK(3))	// PA3 = 1
#define STANDBY_LED_OFF()		socle_gpio_set_value_with_mask(PA, 0, SHIFT_MASK(3))				// PA3 = 0

#define OSC_ENABLE()			socle_gpio_set_value_with_mask(PA, SHIFT_MASK(2), SHIFT_MASK(2))	// PA2 = 1
#define OSC_DISABLE()			socle_gpio_set_value_with_mask(PA, 0, SHIFT_MASK(2))				// PA2 = 0

#define DEVICES_RESET_ON()		socle_gpio_set_value_with_mask(PA, 0, SHIFT_MASK(1))				// PA1 = 0
#define DEVICES_RESET_OFF()		socle_gpio_set_value_with_mask(PA, SHIFT_MASK(1), SHIFT_MASK(1))	// PA1 = 1

#define FM_VIO_POWER_ON()		socle_gpio_set_value_with_mask(PE, SHIFT_MASK(3), SHIFT_MASK(3))	// PE3 = 1
#define FM_VIO_POWER_OFF()		socle_gpio_set_value_with_mask(PE, 0, SHIFT_MASK(3))				// PE3 = 0

#define FM_RESET_ON()			socle_gpio_set_value_with_mask(PE, 0x08, 0x08)						// PE4 = 0
#define FM_RESET_OFF()			socle_gpio_set_value_with_mask(PE, 0x18, 0x18)						// PE4 = 1
//#define FM_RESET_ON()			socle_gpio_set_value_with_mask(PE, 0, SHIFT_MASK(4))				// PE4 = 0
//#define FM_RESET_OFF()			socle_gpio_set_value_with_mask(PE, SHIFT_MASK(4), SHIFT_MASK(4))	// PE4 = 1

	DEVICES_POWER_ON();
	STANDBY_LED_OFF();
	OSC_ENABLE();

	MSDELAY(5);
	FM_VIO_POWER_ON();

	DEVICES_RESET_OFF();
	DEVICES_RESET_ON();
	MSDELAY(100);
	DEVICES_RESET_OFF();

	FM_RESET_OFF();
	FM_RESET_ON();
	MSDELAY(150);
	FM_RESET_OFF();
#endif
}

static void
socle_init_service(void)
{
	socle_init_interrupt();

	socle_init_dma();

#ifdef CONFIG_GPIO
	#ifndef CONFIG_SCDK   //if not scdk then request irq
		socle_init_gpio_irq();
	#endif
#endif

#ifdef CONFIG_MP_GPIO
	#ifdef CONFIG_SCDK
		if(!(ioread32(SOCLE_APB0_SCU + 0x28) & SCU_AHB_MODE)) //not amba mode
			socle_mp_gpio_init_irq();
	#else
		socle_mp_gpio_init_irq();
	#endif
#endif

#ifdef CONFIG_ADC_PWM
	socle_init_pwmt();
#endif
}


extern int
TestEntry(void)
{
	int result = 0;

#if defined(SEMI_HOST_FUNCTION_ARM) && defined(UART_DEBUG)

	#if defined(CONFIG_PDK) || defined(CONFIG_PC7210)
		socle_scu_upll_normal();
	#endif

	#ifdef CONFIG_PC9220
        	socle_scu_dev_enable(SOCLE_DEVCON_UART0);
        	//socle_scu_dev_enable(SCU_DEVCON_UART_HDMA23);   //for test
	#endif

	ns16550_init(SOCLE_UART_EXT_CLK/16/115200);	//uart initial
#endif

	socle_init_service();
	socle_init_platform();

#ifdef SEMI_HOST_FUNCTION
	HIF_debug_print_init();
#endif

#if 1
#if defined(SEMI_HOST_FUNCTION_ARM) && !defined(UART_DEBUG)
	{
		int file_handle = 0;
		file_handle = semihost_init(sysopen_array);
		printf("Semi_host file handle = 0x%8x\n", file_handle);
	}
#endif
#endif

	printf("\nRevision: %s\n" "Build on %s %s\n", REVISION, __DATE__, __TIME__);

	cpu_clk_mhz = socle_get_cpu_clock() / (1000 * 1000);

	result = test_item_ctrl(&main_test_item_container, 0);
	
	printf("\n\nExit the Socle Diagnostic Program!!\n\n");

	return result;
}

