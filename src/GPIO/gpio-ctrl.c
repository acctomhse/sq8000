#include <global.h>
#include <test_item.h>
#include "gpio.h"
#include "dependency.h"

//#define CONFIG_GPIO_DEBUG
#ifdef CONFIG_GPIO_DEBUG
	#define GPIO_DBG(fmt, args...) printf("GPIO: %s(): " fmt, __FUNCTION__, ## args)
#else
	#define GPIO_DBG(fmt, args...)
#endif

struct test_env {
	unsigned int mode		: 1;
	unsigned int interrupt	: 1;
	unsigned int sense		: 1;
	unsigned int both_edge	: 1;
	unsigned int int_event	: 1;
};

// mode
#define NORMAL			0
#define TEST			1

// interrupt
#define WITHOUT_INT		0
#define WITH_INT		1

// sense
#define EDGE			0
#define LEVEL			1

// both_edge
#define SINGLE			0
#define BOTH			1

// int_event
#define LOW				0
#define HIGH			1

#define SCU_AHB_MODE (0x1<<6)
static struct test_env socle_gpio_test_env = {0};
static int pa, pb, pc, pd;
static int socle_gpio_hit;
static int socle_r_data;

static int socle_gpio_port_test(void);

extern struct test_item_container socle_gpio_main_container;

extern int
gpio_test(int autotest)
{
	int ret = 0;
#ifdef CONFIG_SCDK
	{	
  	extern struct test_item socle_gpio_main_test_items[];
  	//read scu to get amba mode
  	if(ioread32(SOCLE_APB0_SCU + 0x28) & SCU_AHB_MODE)
			socle_gpio_main_test_items[1].enable=1;
		else
			socle_gpio_main_test_items[1].enable=0;
	}
#endif
	
	ret = test_item_ctrl(&socle_gpio_main_container, autotest);
	
#ifndef CONFIG_SCDK	
	socle_init_gpio_irq();
#endif

	return ret;
}

extern struct test_item_container socle_gpio_mode_container;

extern int
socle_gpio_onboard_test(int autotest)
{
	int ret = 0;
#ifdef CONFIG_SCDK
	{	
  	extern struct test_item socle_gpio_int_test_items[];
		socle_gpio_int_test_items[1].enable=0;
		socle_set_gpio_base(0, SOCLE_APB0_GPIO0);
	}
#endif
	ret = test_item_ctrl(&socle_gpio_mode_container, autotest);
	return ret;
}

extern int
socle_gpio_fpga_test(int autotest)
{
	int ret = 0;
	{	
  	extern struct test_item socle_gpio_int_test_items[];
		socle_gpio_int_test_items[1].enable=1;  
	}
#ifdef CONFIG_SCDK
	socle_set_gpio_base(0, SOCLE_APB0_MP);
#ifdef SOCLE_GPIO_WITH_INT
	socle_set_gpio_irq(0, SOCLE_INTC_MPS0);
#endif
#endif
	ret = test_item_ctrl(&socle_gpio_mode_container, autotest);
	return ret;
}

extern struct test_item_container socle_gpio_int_main_container;

extern int
socle_gpio_test_mode(int autotest)
{
	int ret = 0;

	socle_gpio_test_env.mode = TEST;

	ret = test_item_ctrl(&socle_gpio_int_main_container, autotest);

	return ret;
}


extern struct test_item_container socle_gpio_normal_container;

extern int
socle_gpio_normal_mode(int autotest)
{
	int ret = 0;

	socle_gpio_test_env.mode = NORMAL;

	ret = test_item_ctrl(&socle_gpio_normal_container, autotest);

	return ret;
}


extern int
socle_gpio_flash_leds(int autotest)
{
	int ret = 0;

#if (GPIO_FLASH_LED_PA == 1) && (GPIO_FLASH_LED_PB == 0)

	int i, test_pattern, mask = 0xFF;

	// normal mode
	socle_gpio_test_mode_en(PA, 0);

	// turn off leds
	socle_gpio_set_value_with_mask(PA, 0, mask);
	MSDELAY(10);

	for (i = 0, test_pattern = 0x01; i < 9; test_pattern <<= 1, i++) {
		socle_gpio_set_value_with_mask(PA, test_pattern, mask);
		MSDELAY(10);
	}

	for (i = 0, test_pattern = 0x80; i < 9; test_pattern >>= 1, i++) {
		socle_gpio_set_value_with_mask(PA, test_pattern, mask);
		MSDELAY(10);
	}

	for (i = 0; i < 2; i++) {
		socle_gpio_set_value_with_mask(PA, 0x55, mask);
		MSDELAY(20);

		socle_gpio_set_value_with_mask(PA, 0xAA, mask);
		MSDELAY(20);
	}

	// turn off leds
	socle_gpio_set_value_with_mask(PA, 0, mask);
	MSDELAY(10);

	for (i = 0; i < 2; i++) {
		socle_gpio_set_value_with_mask(PA, 0xFF, mask);
		MSDELAY(20);
		socle_gpio_set_value_with_mask(PA, 0x00, mask);
		MSDELAY(20);
	}

#elif (GPIO_FLASH_LED_PA == 1) && (GPIO_FLASH_LED_PB == 1)

	int i, test_pattern, mask = 0xFF;

	// normal mode
	socle_gpio_test_mode_en(PA, 0);

	// turn off leds
	socle_gpio_set_value_with_mask(PA, 0, mask);
	socle_gpio_set_value_with_mask(PB, 0, mask);
	MSDELAY(10);

	for (i = 0, test_pattern = 0x01; i < 9; test_pattern <<= 1, i++) {
		socle_gpio_set_value_with_mask(PA, test_pattern, mask);
		MSDELAY(10);
	}

	for (i = 0, test_pattern = 0x80; i < 9; test_pattern >>= 1, i++) {
		socle_gpio_set_value_with_mask(PB, test_pattern, mask);
		MSDELAY(10);
	}

	for (i = 0; i < 2; i++) {
		socle_gpio_set_value_with_mask(PA, 0x55, mask);
		socle_gpio_set_value_with_mask(PB, 0xAA, mask);
		MSDELAY(20);

		socle_gpio_set_value_with_mask(PA, 0xAA, mask);
		socle_gpio_set_value_with_mask(PB, 0x55, mask);
		MSDELAY(20);
	}

	// turn off leds
	socle_gpio_set_value_with_mask(PA, 0, mask);
	socle_gpio_set_value_with_mask(PB, 0, mask);
	MSDELAY(10);

	for (i = 0; i < 2; i++) {
		socle_gpio_set_value_with_mask(PA, 0xFF, mask);
		MSDELAY(20);
		socle_gpio_set_value_with_mask(PB, 0xFF, mask);
		MSDELAY(20);
		socle_gpio_set_value_with_mask(PA, 0x00, mask);
		MSDELAY(20);
		socle_gpio_set_value_with_mask(PB, 0x00, mask);
		MSDELAY(20);
	}

#else
	printf("Warning! No LEDs flashed\n\n");
	ret = -1;
#endif

	return ret;
}


extern struct test_item_container socle_gpio_rw_test_container;

extern int
socle_gpio_without_int(int autotest)
{
	int ret = 0;

	socle_gpio_test_env.interrupt = WITHOUT_INT;

	ret = test_item_ctrl(&socle_gpio_rw_test_container, autotest);

	return ret;
}


extern struct test_item_container socle_gpio_sense_container;

extern int
socle_gpio_with_int(int autotest)
{
	int ret = 0;

	socle_gpio_test_env.interrupt = WITH_INT;

	ret = test_item_ctrl(&socle_gpio_sense_container, autotest);

	return ret;
}

extern int
socle_gpio_port_a_to_d(int autotest)
{
	int ret;

	pa = PA;
	pb = PB;
	pc = PC;
	pd = PD;

	ret = socle_gpio_port_test();

	return ret;
}

extern int
socle_gpio_port_e_to_h(int autotest)
{
	int ret;

	pa = PE;
	pb = PF;
	pc = PG;
	pd = PH;

	ret = socle_gpio_port_test();

	return ret;
}

extern int
socle_gpio_port_i_to_l(int autotest)
{
	int ret;

	pa = PI;
	pb = PJ;
	pc = PK;
	pd = PL;

	ret = socle_gpio_port_test();

	return ret;
}

extern int
socle_gpio_port_m_to_p(int autotest)
{
	int ret;

	pa = PM;
	pb = PN;
	pc = PO;
	pd = PP;

	ret = socle_gpio_port_test();

	return ret;
}

static int
socle_gpio_wo_int_rw_test(int pw, int pr)
{
	int ret = 0, test_pattern = 0XFF, mask = 0xFF, r_data;

	printf("from p%c to p%c...\n", pw + 'a', pr + 'a');

	socle_gpio_set_value_with_mask(pw, test_pattern, mask);
	r_data = socle_gpio_get_value_with_mask(pr, mask);

	if (test_pattern ^ r_data) {
		printf("Error! test_pattern = 0x%x, r_data = 0x%x\n", test_pattern, r_data);
		ret = -1;
	}

	return ret;
}

struct socle_gpio_int_s {
	int pw;	// current write port
	int pr;	// current read port
};

static void
socle_gpio_w_int_isr(void *pparam)
{
	int status, mask = 0xFF;
	int pw = ((struct socle_gpio_int_s *)pparam)->pw;
	int pr = ((struct socle_gpio_int_s *)pparam)->pr;

	socle_r_data = socle_gpio_get_value_with_mask(pr, mask);

	// in order to exit isr
	if (LEVEL == socle_gpio_test_env.sense) {
		if (LOW == socle_gpio_test_env.int_event) {
			socle_gpio_set_value_with_mask(pw, 0xFF, mask);
		} else if (HIGH == socle_gpio_test_env.int_event) {
			socle_gpio_set_value_with_mask(pw, 0x0, mask);
		}
	}

	// read status
	status = socle_gpio_get_interrupt_status_with_port(pr);

	// clear interrupt
	socle_gpio_set_interrupt_clear(pr, status);

	socle_gpio_hit++;
	GPIO_DBG("socle_gpio_hit = %d\n", socle_gpio_hit);
}

static int
socle_gpio_w_int_rw_test(int pw, int pr)
{
	int ret = 0, irq, test_pattern, mask = 0xFF;
	struct socle_gpio_int_s socle_gpio_int_s = {pw, pr};

	printf("from p%c to p%c...\n", pw + 'a', pr + 'a');
	
	socle_gpio_hit = 0;
	irq = socle_gpio_get_irq(pr);
	request_irq(irq, socle_gpio_w_int_isr, &socle_gpio_int_s);

	if (LEVEL == socle_gpio_test_env.sense) {
		GPIO_DBG("level sensitive\n");
		socle_gpio_set_interrupt_sense_with_mask(pr, 0xFF, mask);

		if (LOW == socle_gpio_test_env.int_event) {
			GPIO_DBG("low level\n");
			socle_gpio_set_interrupt_event_with_mask(pr, 0x0, mask);

			//init
			test_pattern = 0XFF;
			socle_gpio_set_value_with_mask(pw, test_pattern, mask);
			socle_gpio_get_value_with_mask(pr, mask);
			socle_gpio_set_interrupt_mask_with_mask(pr, 0xFF, mask);

			test_pattern = 0X0;
			socle_gpio_set_value_with_mask(pw, test_pattern, mask);
			if (socle_wait_for_int(&socle_gpio_hit, 3)) {
				printf("Timeout!!\n");
				ret = -1;
			}

		} else if (HIGH == socle_gpio_test_env.int_event) {
			GPIO_DBG("high level\n");
			socle_gpio_set_interrupt_event_with_mask(pr, 0xFF, mask);

			//init
			test_pattern = 0X0;
			socle_gpio_set_value_with_mask(pw, test_pattern, mask);
			socle_gpio_get_value_with_mask(pr, mask);
			socle_gpio_set_interrupt_mask_with_mask(pr, 0xFF, mask);

			test_pattern = 0XFF;
			socle_gpio_set_value_with_mask(pw, test_pattern, mask);
			if (socle_wait_for_int(&socle_gpio_hit, 3)) {
				printf("Timeout!!\n");
				ret = -1;
			}

		}	//socle_gpio_test_env.int_event

	} else if (EDGE == socle_gpio_test_env.sense) {
		GPIO_DBG("edge sensitive\n");
		socle_gpio_set_interrupt_sense_with_mask(pr, 0x0, mask);

		if (SINGLE == socle_gpio_test_env.both_edge) {
			GPIO_DBG("single edge\n");
			socle_gpio_set_interrupt_both_edges_with_mask(pr, 0x0, mask);

			if (LOW == socle_gpio_test_env.int_event) {
				GPIO_DBG("falling edge\n");
				socle_gpio_set_interrupt_event_with_mask(pr, 0x0, mask);

				//init
				test_pattern = 0XFF;
				socle_gpio_set_value_with_mask(pw, test_pattern, mask);
				socle_gpio_get_value_with_mask(pr, mask);
				socle_gpio_set_interrupt_mask_with_mask(pr, 0xFF, mask);

				test_pattern = 0X0;
				socle_gpio_set_value_with_mask(pw, test_pattern, mask);
				if (socle_wait_for_int(&socle_gpio_hit, 3)) {
					printf("Timeout!!\n");
					ret = -1;
				}

			} else if (HIGH == socle_gpio_test_env.int_event) {
				GPIO_DBG("rising edge\n");
				socle_gpio_set_interrupt_event_with_mask(pr, 0xFF, mask);

				//init
				test_pattern = 0X0;
				socle_gpio_set_value_with_mask(pw, test_pattern, mask);
				socle_gpio_get_value_with_mask(pr, mask);
				socle_gpio_set_interrupt_mask_with_mask(pr, 0xFF, mask);

				test_pattern = 0XFF;
				socle_gpio_set_value_with_mask(pw, test_pattern, mask);
				if (socle_wait_for_int(&socle_gpio_hit, 3)) {
					printf("Timeout!!\n");
					ret = -1;
				}

			}	//socle_gpio_test_env.int_event

		} else if (BOTH == socle_gpio_test_env.both_edge) {
			GPIO_DBG("both edge\n");
			socle_gpio_set_interrupt_both_edges_with_mask(pr, 0xFF, mask);

			//init
			test_pattern = 0X0;
			socle_gpio_set_value_with_mask(pw, test_pattern, mask);
			socle_gpio_get_value_with_mask(pr, mask);
			socle_gpio_set_interrupt_mask_with_mask(pr, 0xFF, mask);

			test_pattern = 0XFF;
			socle_gpio_set_value_with_mask(pw, test_pattern, mask);
			if (socle_wait_for_int(&socle_gpio_hit, 3)) {
				printf("Timeout!!\n");
				ret = -1;
			}

			test_pattern = 0X0;
			socle_gpio_set_value_with_mask(pw, test_pattern, mask);
			if (socle_wait_for_int(&socle_gpio_hit, 3)) {
				printf("Timeout!!\n");
				ret = -1;
			}

		}	//socle_gpio_test_env.both_edge

	}	//socle_gpio_test_env.sense

	// disable interrupt
	socle_gpio_set_interrupt_mask_with_mask(pr, 0x0, mask);
	free_irq(irq);

	if (test_pattern != socle_r_data) {
		printf("Error! test_pattern = 0x%x, r_data = 0x%x\n", test_pattern, socle_r_data);
		ret = -1;
	}

	return ret;
}

static int
socle_gpio_port_test(void)
{
	int ret = 0;

	if (NORMAL == socle_gpio_test_env.mode) {
		GPIO_DBG("normal mode\n");
		socle_gpio_test_mode_en(pa, 0);
	} else if (TEST == socle_gpio_test_env.mode) {
		GPIO_DBG("test mode\n");
		socle_gpio_test_mode_en(pa, 1);
	}

	if (WITHOUT_INT == socle_gpio_test_env.interrupt) {
		GPIO_DBG("without interrupt\n");

		//Tmode0, loop back pb to pa
		socle_gpio_test_mode_ctrl(pa, PB2PA);
		ret |= socle_gpio_wo_int_rw_test(pb, pa);

		//Tmode1, loop back pa to pb
		socle_gpio_test_mode_ctrl(pa, PA2PB);
		ret |= socle_gpio_wo_int_rw_test(pa, pb);

		//Tmode2, loop back pd to pc
		socle_gpio_test_mode_ctrl(pa, PD2PC);
		ret |= socle_gpio_wo_int_rw_test(pd, pc);

		//Tmode3, loop back pc to pd
		socle_gpio_test_mode_ctrl(pa, PC2PD);
		ret |= socle_gpio_wo_int_rw_test(pc, pd);

	} else if (WITH_INT == socle_gpio_test_env.interrupt) {
		GPIO_DBG("with interrupt\n");

		//Tmode0, loop back pbt to pa
		socle_gpio_test_mode_ctrl(pa, PB2PA);
		ret |= socle_gpio_w_int_rw_test(pb, pa);

		//Tmode1, loop back pa to pb
		socle_gpio_test_mode_ctrl(pa, PA2PB);
		ret |= socle_gpio_w_int_rw_test(pa, pb);

		//Tmode2, loop back pd to pc
		socle_gpio_test_mode_ctrl(pa, PD2PC);
		ret |= socle_gpio_w_int_rw_test(pd, pc);

		//Tmode3, loop back pc to pd
		socle_gpio_test_mode_ctrl(pa, PC2PD);
		ret |= socle_gpio_w_int_rw_test(pc, pd);

	}	//socle_gpio_test_env.interrupt

	return ret;
}


extern struct test_item_container socle_gpio_event_container;

extern int
socle_gpio_level_sense(int autotest)
{
	int ret = 0;

	socle_gpio_test_env.sense = LEVEL;

	ret = test_item_ctrl(&socle_gpio_event_container, autotest);

	return ret;
}

extern int
socle_gpio_lo_event(int autotest)
{
	int ret = 0;

	socle_gpio_test_env.int_event = LOW;

	ret = test_item_ctrl(&socle_gpio_rw_test_container, autotest);

	return ret;
}

extern int
socle_gpio_hi_event(int autotest)
{
	int ret = 0;

	socle_gpio_test_env.int_event = HIGH;

	ret = test_item_ctrl(&socle_gpio_rw_test_container, autotest);

	return ret;
}


extern struct test_item_container socle_gpio_edge_container;

extern int
socle_gpio_edge_sense(int autotest)
{
	int ret = 0;

	socle_gpio_test_env.sense = EDGE;

	ret = test_item_ctrl(&socle_gpio_edge_container, autotest);

	return ret;
}

extern int
socle_gpio_single_edge(int autotest)
{
	int ret = 0;

	socle_gpio_test_env.both_edge = SINGLE;

	ret = test_item_ctrl(&socle_gpio_event_container, autotest);

	return ret;
}

extern int
socle_gpio_both_edge(int autotest)
{
	int ret = 0;

	socle_gpio_test_env.both_edge = BOTH;

	ret = test_item_ctrl(&socle_gpio_rw_test_container, autotest);

	return ret;
}



