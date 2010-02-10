#include <genlib.h>
#include <global.h>
#include "gpio.h"


//#define CONFIG_GPIO_KPD_DEBUG
#ifdef CONFIG_GPIO_KPD_DEBUG
	#define KPD_DBG(fmt, args...) printf("KPD: %s(): " fmt, __FUNCTION__, ## args)
#else
	#define KPD_DBG(fmt, args...)
#endif


#define KPD_NUM	4
static int kpd_matrix[KPD_NUM][KPD_NUM];
static int finish;

static int
kpd_value_convert(int val)
{
	int ret;

	KPD_DBG("val = 0x%x\n", val);

	switch (val) {
	case 1:
		ret = 0;
		break;
	case 2:
		ret = 1;
		break;
	case 4:
		ret = 2;
		break;
	case 8:
		ret = 3;
		break;
	default:
		KPD_DBG("Warning! val = 0x%02x\n", val);
		ret = -1;
	}

	return ret;
}

static void
pdk_check_kpd(void)
{
	int i, j, count = 0;

	printf("\nkpd_matrix: (O:press, X:unpress)\n       ");
	for (j = 0; j < KPD_NUM; j++) {
		printf("  col(%d)", j);
	}
	printf("\n");

	for (i = 0; i < KPD_NUM; i++) {
		printf("row(%d)", i);
		for (j = 0; j < KPD_NUM; j++) {
			if (kpd_matrix[i][j] == 1) {
				count++;
				printf("       O");
			} else
				printf("       X");
		}
		printf("\n");
	}

	if (count == KPD_NUM * KPD_NUM)
		finish = 1;
}

static void
pdk_gpio_kpd_isr(void *param)
{
	int tmp1, tmp2, row, col, *count = (int *)param;

	KPD_DBG("\n");

#if defined CONFIG_PC7210	//20080109 leonid+ for PDK-PC7210
	// read col pins
	// PA4, PA5
	socle_gpio_get_value_with_mask(PA, 0x30);
	// PF4, PF5
	socle_gpio_get_value_with_mask(PF, 0x30);

	// read status
	tmp1 = socle_gpio_get_interrupt_status_with_port(PA);
	tmp2 = socle_gpio_get_interrupt_status_with_port(PF);

	// clear interrupt
	socle_gpio_set_interrupt_clear(PA, tmp1);
	socle_gpio_set_interrupt_clear(PF, tmp2);

	KPD_DBG("tmp1 = 0x%x, tmp2 = 0x%x\n", tmp1, tmp2);
#if (KPD_NUM == 4)
	// 4 X 4
	col = kpd_value_convert((tmp1 & 0xf));
	row = kpd_value_convert(((tmp1 & 0x30) >> 4) | (tmp2 & 0x30) >> 2);
#endif
#if (KPD_NUM == 3)
	// 3 X 3
	col = kpd_value_convert(tmp1 & 0x7);
	row = kpd_value_convert(((tmp1 & 0x30) >> 4) | (tmp2 & 0x10) >> 2);
#endif
#elif defined CONFIG_PC9220	
	// read col pins
	socle_gpio_get_value_with_mask(PJ, 0xf0);

	// read status
	tmp1 = socle_gpio_get_interrupt_status_with_port(PJ);
	tmp2 = socle_gpio_get_value_with_mask(PJ, 0xf0);

	// clear interrupt
	socle_gpio_set_interrupt_clear(PJ, tmp1);

	KPD_DBG("tmp1 = 0x%x \n", tmp1);
	// 4 X 4
	col = kpd_value_convert((tmp1 & 0xf));
	row = kpd_value_convert((tmp2 & 0xf0) >> 4);


#else	//PDK-PC700x
	// read col pins
	// PA3, PA4, PA5
	socle_gpio_get_value_with_mask(PA, 0x38);
	// PC1
	socle_gpio_get_value_with_mask(PC, 0x2);

//	MSDELAY(300);

	// read status
	tmp1 = socle_gpio_get_interrupt_status_with_port(PA);
	tmp2 = socle_gpio_get_interrupt_status_with_port(PC);

	// clear interrupt
	socle_gpio_set_interrupt_clear(PA, tmp1);
	socle_gpio_set_interrupt_clear(PC, tmp2);

	KPD_DBG("tmp1 = 0x%x, tmp2 = 0x%x\n", tmp1, tmp2);
#if (KPD_NUM == 4)
	// 4 X 4
	col = kpd_value_convert((tmp1 & 0x7)  | ((tmp2 & 0x1) << 3));
	row = kpd_value_convert(((tmp1 & 0x38) >> 3) | (tmp2 & 0x2) << 2);
#endif
#if (KPD_NUM == 3)
	// 3 X 3
	col = kpd_value_convert(tmp1 & 0x7);
	row = kpd_value_convert((tmp1 & 0x38) >> 3);
#endif
#endif

	KPD_DBG("row = %d, col = %d\n", row, col);

	if ((-1 != row) && (-1 != col)) {
		kpd_matrix[row][col] = 1;
		(*count)++;
		printf("count = %d, row = %d, col = %d\n", *count, row, col);
	}

	if (*count >= KPD_NUM * KPD_NUM) {
		pdk_check_kpd();
	}

	MSDELAY(100);

#if defined CONFIG_PC7210	//20080109 leonid+ for PDK-PC7210
	// write col pins as 0
	// PA4, PA5
	socle_gpio_set_value_with_mask(PA, 0x0, 0x30);
	// PF4, PF5
	socle_gpio_set_value_with_mask(PF, 0x0, 0x30);
#elif defined CONFIG_PC9220	
	// write col pins as 0
	// PJ4, PJ5, PJ6, PJ7	
	socle_gpio_set_value_with_mask(PJ, 0x0, 0xf0);
#else	//PDK-PC700x
	// write col pins as 0
	// PA3, PA4, PA5
	socle_gpio_set_value_with_mask(PA, 0x0, 0x38);
	// PC1
	socle_gpio_set_value_with_mask(PC, 0x0, 0x2);
#endif
}

static void
gpio_kpd_init(void)
{
	int i, j;
	KPD_DBG("\n");

	for (i = 0; i < KPD_NUM; i++)
		for (j = 0; j < KPD_NUM; j++)
			kpd_matrix[i][j] = 0;

#if defined (CONFIG_PC7210)	//20080109 leonid+ for PDK-PC7210
	// normal mode
	socle_gpio_test_mode_en(PA, 0);
	socle_gpio_test_mode_en(PF, 0);
	// single low edge trigger (row - PA0, PA1, PA2, PA3)
	// PA0, PA1, PA2, PA3
	socle_gpio_set_interrupt_sense_with_mask(PA, 0x0, 0xf);
	socle_gpio_set_interrupt_both_edges_with_mask(PA, 0x0, 0xf);
	socle_gpio_set_interrupt_event_with_mask(PA, 0x0, 0xf);

	// single high edge trigger (col - PA4, PA5, PF4, PF5)
	// PA4, PA5,
	socle_gpio_set_interrupt_sense_with_mask(PA, 0x0, 0x30);
	socle_gpio_set_interrupt_both_edges_with_mask(PA, 0x0, 0x30);
	socle_gpio_set_interrupt_event_with_mask(PA, 0x30, 0x30);
	// PF4, PF5
	socle_gpio_set_interrupt_sense_with_mask(PF, 0x0, 0x30);
	socle_gpio_set_interrupt_both_edges_with_mask(PF, 0x0, 0x30);
	socle_gpio_set_interrupt_event_with_mask(PF, 0x30, 0x30);

	// set row as input
	// PA0, PA1, PA2, PA3
	socle_gpio_get_value_with_mask(PA, 0xf);

	// write col pins as 0
	// PA4, PA5,
	socle_gpio_set_value_with_mask(PA, 0x0, 0x30);
	// PF4, PF5
	socle_gpio_set_value_with_mask(PF, 0x0, 0x30);

	// enable all interrupt
	socle_gpio_set_interrupt_mask_with_mask(PA, 0x3F, 0x3F);
	socle_gpio_set_interrupt_mask_with_mask(PF, 0x30, 0x30);
#elif CONFIG_PC9220	//20080109 leonid+ for PDK-PC7210
	// normal mode
	socle_gpio_test_mode_en(PJ, 0);

	// single low edge trigger (row - PJ0, PJ1, PJ2, PJ3)
	// PJ0, PJ1, PJ2, PJ3
	socle_gpio_set_interrupt_sense_with_mask(PJ, 0x0, 0xf);
	socle_gpio_set_interrupt_both_edges_with_mask(PJ, 0x0, 0xf);
	socle_gpio_set_interrupt_event_with_mask(PJ, 0x0, 0xf);

	// single high edge trigger (col - PJ4, PJ5, PJ6, PJ7)
	// PJ4, PJ5, PJ6, PJ7
	socle_gpio_set_interrupt_sense_with_mask(PJ, 0x0, 0xf0);
	socle_gpio_set_interrupt_both_edges_with_mask(PJ, 0x0, 0xf0);
	socle_gpio_set_interrupt_event_with_mask(PJ, 0xf0, 0xf0);

	// set row as input
	// PJ0, PJ1, PJ2, PJ3
	socle_gpio_get_value_with_mask(PJ, 0xf);

	// write col pins as 0
	// PJ4, PJ5,
	socle_gpio_set_value_with_mask(PJ, 0x0, 0xf0);

	// enable all interrupt
	socle_gpio_set_interrupt_mask_with_mask(PJ, 0xff, 0xff);

#else	//PDK-PC700x
	// normal mode
	socle_gpio_test_mode_en(PA, 0);
	// single low edge trigger (row - PA0, PA1, PA2, PC0)
	// PA0, PA1, PA2
	socle_gpio_set_interrupt_sense_with_mask(PA, 0x0, 0x7);
	socle_gpio_set_interrupt_both_edges_with_mask(PA, 0x0, 0x7);
	socle_gpio_set_interrupt_event_with_mask(PA, 0x0, 0x7);
	// PC0
	socle_gpio_set_interrupt_sense_with_mask(PC, 0x0, 0x1);
	socle_gpio_set_interrupt_both_edges_with_mask(PC, 0x0, 0x1);
	socle_gpio_set_interrupt_event_with_mask(PC, 0x0, 0x1);

	// single high edge trigger (col - PA3, PA4, PA5, PC1)
	// PA3, PA4, PA5,
	socle_gpio_set_interrupt_sense_with_mask(PA, 0x0, 0x38);
	socle_gpio_set_interrupt_both_edges_with_mask(PA, 0x0, 0x38);
	socle_gpio_set_interrupt_event_with_mask(PA, 0x38, 0x38);
	// PC1
	socle_gpio_set_interrupt_sense_with_mask(PC, 0x0, 0x2);
	socle_gpio_set_interrupt_both_edges_with_mask(PC, 0x0, 0x2);
	socle_gpio_set_interrupt_event_with_mask(PC, 0x2, 0x2);

	// set row as input
	// PA0, PA1, PA2
	socle_gpio_get_value_with_mask(PA, 0x7);
	// PC0
	socle_gpio_get_value_with_mask(PC, 0x1);

	// write col pins as 0
	// PA3, PA4, PA5
	socle_gpio_set_value_with_mask(PA, 0x0, 0x38);
	// PC1
	socle_gpio_set_value_with_mask(PC, 0x0, 0x2);

	// enable all interrupt
	socle_gpio_set_interrupt_mask_with_mask(PA, 0x3F, 0x3F);
	socle_gpio_set_interrupt_mask_with_mask(PC, 0x3, 0x3);
#endif
}


extern int
socle_gpio_kpd_test(int autotest)
{
	int ret = 0, count = 0, irq;

#ifdef CONFIG_PC9220	
	irq = socle_gpio_get_irq(PJ);
#else
	irq = socle_gpio_get_irq(PA);
#endif
	request_irq(irq, pdk_gpio_kpd_isr, (void*)&count);

	gpio_kpd_init();

	finish = 0;
	printf("Please press all keys and then exit.\n");
	while (1) {
		if (finish)
			break;
	};
	finish = 0;

	free_irq(irq);
	socle_init_gpio_irq();

	return ret;
}

