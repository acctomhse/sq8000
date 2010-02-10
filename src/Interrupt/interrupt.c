#include <platform.h>
#include <irqs.h>
#include <global.h>
#include <genlib.h>
#include <test_item.h>
#include "interrupt.h"

#ifdef CONFIG_LDK5
static volatile int *int_flag = (int *)(0x42000000);
#endif

#define WAIT_LOOPS 1000

static void testISR (void* pparam);

struct _isr_data {
        int* m_pevent;
        int  m_irq;
};


extern struct test_item_container interrupt_main_container;

extern int
INTRTesting(int autotest)
{
     int ret = 0;

     ret = test_item_ctrl(&interrupt_main_container, autotest);
     
     return ret;
}


extern int
irq_test_mode_test(int autotest)
{
	int		event;
	int		num;
	int		result = 0;
	struct _isr_data	isrdata;

	// disable all interrupt
	iowrite32(0, SOCLE_INTC_IRQ_IECR(INTC_REG_BASE));

	// clear all interrupt
	iowrite32(0xFFFFFFFF, SOCLE_INTC_IRQ_ICCR(INTC_REG_BASE));
	
	// set interrupt controller into test mode
	iowrite32(1, SOCLE_INTC_IRQ_TEST(INTC_REG_BASE));

	printf("Vector Test ---\n");

	for (num = 0; num < INTC_MAX_IRQ; num++) {
		printf("%d ", num);
		event = 0;

		// prepare isr data
		isrdata.m_pevent = &event;
		isrdata.m_irq    = num;

		// enable interrupt
		request_irq(num, testISR, &isrdata);

		// s/w trigger interrupt
		iowrite32(0x1 << num, SOCLE_INTC_IRQ_ISCR(INTC_REG_BASE));

		if (socle_wait_for_int(&event, 1)) {
			printf(" --- Error Termination\n");
			
			// s/w trigger interrupt clear
			iowrite32(0, SOCLE_INTC_IRQ_ISCR(INTC_REG_BASE));
			
			// disable interrupt
			free_irq(num);

			result = -1;			
			break;
		}
		
		// s/w trigger interrupt clear
		iowrite32(0, SOCLE_INTC_IRQ_ISCR(INTC_REG_BASE));

		// disable interrupt
		free_irq(num);
	}
	if (event)
		printf(" --- Finish\n");

	// reset interrupt controller into normal mode
	iowrite32(0, SOCLE_INTC_IRQ_TEST(INTC_REG_BASE));
	
	// disable all interrupt
	iowrite32(0, SOCLE_INTC_IRQ_IECR(INTC_REG_BASE));

	// clear all interrupt
	iowrite32(0xFFFFFFFF, SOCLE_INTC_IRQ_ICCR(INTC_REG_BASE));

	return result;
}


extern int
fiq_test_mode_test(int autotest)
{
	int		event;
	int		num;
	int		result = 0;
	struct _isr_data	isrdata;

	// disable all interrupt
	iowrite32(0, SOCLE_INTC_FIQ_IECR(INTC_REG_BASE));

	// clear all interrupt
	iowrite32(0xFFFFFFFF, SOCLE_INTC_FIQ_ICCR(INTC_REG_BASE));
	
	// set interrupt controller into test mode
	iowrite32(1, SOCLE_INTC_FIQ_TEST(INTC_REG_BASE));

	printf("Vector Test ---\n");

	for (num = 0; num < INTC_MAX_FIQ; num++) {
		printf("%d ", num);
		event = 0;

		// prepare isr data
		isrdata.m_pevent = &event;
		isrdata.m_irq    = num;

		// enable interrupt
		request_fiq(num, testISR, &isrdata);

		// s/w trigger interrupt
		iowrite32(0x1 << num, SOCLE_INTC_FIQ_ISCR(INTC_REG_BASE));

		if (socle_wait_for_int(&event, 1)) {
			printf(" --- Error Termination\n");
			
			// s/w trigger interrupt clear
			iowrite32(0, SOCLE_INTC_FIQ_ISCR(INTC_REG_BASE));
			
			// disable interrupt
			free_fiq(num);

			result = -1;			
			break;
		}
		
		// s/w trigger interrupt clear
		iowrite32(0, SOCLE_INTC_FIQ_ISCR(INTC_REG_BASE));

		// disable interrupt
		free_fiq(num);
	}
	if (event)
		printf(" --- Finish\n");

	// reset interrupt controller into normal mode
	iowrite32(0, SOCLE_INTC_FIQ_TEST(INTC_REG_BASE));
	
	// disable all interrupt
	iowrite32(0, SOCLE_INTC_FIQ_IECR(INTC_REG_BASE));

	// clear all interrupt
	iowrite32(0xFFFFFFFF, SOCLE_INTC_FIQ_ICCR(INTC_REG_BASE));

	return result;
}


#if defined(CONFIG_LDK5) && (defined(CONFIG_ARM9) || defined(CONFIG_ARM9_HI))
static void int0_handler (void* pparam);
extern int
int0_side_test(int autotest)
{
	volatile int int_num = 0;

	printf("\nARM9 Side...\n");
	
	// enable interrupt
	request_irq(SOCLE_INTC_INTC1, int0_handler, (void*)&int_num);

	printf("Waiting for interrupt...\n");
	while(int_num < INTC_MAX_IRQ);

	// disable interrupt
	free_irq(SOCLE_INTC_INTC1);
	
	return 0;
}

static void
int0_handler (void* pparam)
{
	int *int_num = (int *)pparam;
	
	//cyli fix but not test
	//*int_num = readw(INTC1_ISR);
	//printf("Receive Interrupt[%d]!\n", *int_num);
	//printf("cyli Receive Interrupt[%d]!\n", ioread32(SOCLE_INTC_IRQ_ISR(SOCLE_AHB1_INTC)));
	
	printf("Receive Interrupt[%d]!\n", (*int_num)++);
	*int_flag = 1;
}
#endif	//CONFIG_LDK5 && (CONFIG_ARM9 || CONFIG_ARM9_HI)


#if defined(CONFIG_LDK5) && (defined(CONFIG_ARM7) || defined(CONFIG_ARM7_HI))
extern int
int1_side_test(int autotest)
{
	int		num, result = 0;

	printf("\nARM7 Side...\n");
	
	// disable all interrupt
	iowrite32(0, SOCLE_INTC_IRQ_IECR(INTC_REG_BASE));

	// clear all interrupt
	iowrite32(0xFFFFFFFF, SOCLE_INTC_IRQ_ICCR(INTC_REG_BASE));
	
	// set interrupt controller into test mode
	iowrite32(1, SOCLE_INTC_IRQ_TEST(INTC_REG_BASE));

	printf("Vector Test ---\n");

	for (num = 0; num < INTC_MAX_IRQ; num++) {
		printf("%d ", num);

		*int_flag = 0;
		
		// enable interrupt
		// irq enable
		iowrite32(ioread32(SOCLE_INTC_IRQ_IECR(INTC_REG_BASE)) | (0x1 << num),
					SOCLE_INTC_IRQ_IECR(INTC_REG_BASE));
		// irq set mask
		iowrite32(ioread32(SOCLE_INTC_IRQ_IMR(INTC_REG_BASE)) | (0x1 << num),
					SOCLE_INTC_IRQ_IMR(INTC_REG_BASE));

		// s/w trigger interrupt
		iowrite32(0x1 << num, SOCLE_INTC_IRQ_ISCR(INTC_REG_BASE));

		//wait to trigger another interrupt
		while(!(*int_flag));

		// s/w trigger interrupt clear
		iowrite32(0, SOCLE_INTC_IRQ_ISCR(INTC_REG_BASE));

		// disable interrupt
		free_irq(num);
	}
	printf(" --- Finish\n");

	// reset interrupt controller into normal mode
	iowrite32(0, SOCLE_INTC_IRQ_TEST(INTC_REG_BASE));
	
	// disable all interrupt
	iowrite32(0, SOCLE_INTC_IRQ_IECR(INTC_REG_BASE));

	// clear all interrupt
	iowrite32(0xFFFFFFFF, SOCLE_INTC_IRQ_ICCR(INTC_REG_BASE));

	return result;
}
#endif	//CONFIG_LDK5 && (CONFIG_ARM7 || CONFIG_ARM7_HI)


static void
testISR (void* pparam)
{
	struct _isr_data* pdata = (struct _isr_data*)pparam;

	*(pdata->m_pevent) = 1;
}

