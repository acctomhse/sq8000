#include <test_item.h>
#include <genlib.h>
#include <global.h>
#include "timer-regs.h"
#include <platform.h>
#include "dependency.h"

#if defined (CONFIG_PC9220)
#include <scu.h>
#include "../../platform/arch/scu-reg.h"
#endif

static u32
socle_timer_read(u32 reg, u32 base)
{
	u32 val;

	base = base + reg;
	val = ioread32(base);
	return val;
}

static void
socle_timer_write(u32 reg, u32 val, u32 base)
{
	base = base + reg;
	iowrite32(val, base);
}

static void socle_timer_isr_0(void *param);
static void socle_timer_isr_1(void *param);
static void socle_timer_isr_2(void *param);

static u32 socle_timer_base;
static u32 socle_timer_conf[3];
static u32 socle_timer_cur_idx;
static u32 socle_timer_apb_clk;
static volatile u32 socle_timer_sec;
static u32 socle_timer_chain_mode = 0;
static int socle_timer_int_flag = 0;

extern struct test_item_container socle_timer_main_test_container;

extern int
TIMERTesting(int autotest)
{
	int ret = 0;

#ifdef CONFIG_PC9220
	socle_scu_dev_enable(SCU_DEVCON_TMR_GPIO);
#endif

	socle_timer_base = SOCLE_APB0_TIMER;
	socle_timer_apb_clk = socle_get_apb_clock();
	printf("Socle Timer: current apb clock is %d\n", socle_timer_apb_clk);

	/* Disable the timers */
	socle_timer_write(SOCLE_TMR0CON, SOCLE_TIMER_DIS, socle_timer_base);
	socle_timer_write(SOCLE_TMR1CON, SOCLE_TIMER_DIS, socle_timer_base);
	socle_timer_write(SOCLE_TMR2CON, SOCLE_TIMER_DIS, socle_timer_base);

	/* Register the interrupt service routines */
	request_irq(SOCLE_INTC_TMR0_0, socle_timer_isr_0, NULL);
	request_irq(SOCLE_INTC_TMR0_1, socle_timer_isr_1, NULL);
	request_irq(SOCLE_INTC_TMR0_2, socle_timer_isr_2, NULL);

	ret = test_item_ctrl(&socle_timer_main_test_container, autotest);

	/* Unregister the interrupt service routines */
	free_irq(SOCLE_INTC_TMR0_0);
	free_irq(SOCLE_INTC_TMR0_1);
	free_irq(SOCLE_INTC_TMR0_2);

#ifdef CONFIG_PC9220
	socle_scu_dev_disable(SCU_DEVCON_TMR_GPIO);
#endif

	return ret;
}

extern struct test_item_container socle_timer_mode_0_timer_test_container;

extern int 
socle_timer_mode_0_test(int autotest)
{
	int ret = 0;

	extern struct test_item socle_timer_main_test_items[];

	socle_timer_chain_mode = 0;

	/* If chain mode is used, we need to reset mode to 0 */
	if ((1 == socle_timer_main_test_items[1].enable) ||
	    (1 == socle_timer_main_test_items[2].enable))
		socle_timer_write(SOCLE_TMRMODE, SOCLE_TIMER_CHAIN_MODE_0, socle_timer_base);

	ret = test_item_ctrl(&socle_timer_mode_0_timer_test_container, autotest);
	return ret;
}

extern int socle_timer_mode_1(int autotest)
{
	int err_flag = 0;

	socle_timer_chain_mode = 1;
	socle_timer_write(SOCLE_TMRMODE, SOCLE_TIMER_CHAIN_MODE_1, socle_timer_base);
	socle_timer_conf[0] = SOCLE_TIMER_EN |
		SOCLE_TIMER_CNT_MODE_PERIODICAL |
		SOCLE_TIMER_PRESCALE_FACTOR_1_1 |
		SOCLE_TIMER_INT_MASK_DIS |
		SOCLE_TIMER_INT_EDGE_TRIG;
	socle_timer_conf[1] = SOCLE_TIMER_EN |
		SOCLE_TIMER_CNT_MODE_FREE_RUNNING |
		SOCLE_TIMER_PRESCALE_FACTOR_1_1 |
		SOCLE_TIMER_INT_MASK_DIS |
		SOCLE_TIMER_INT_LEVEL_TRIG;
	socle_timer_write(SOCLE_TMR0LR, socle_timer_apb_clk*2, socle_timer_base);
	socle_timer_write(SOCLE_TMR1LR, 2, socle_timer_base);
	socle_timer_sec = 0;
	printf("Timer will count 4 seconds at once\n");
	socle_timer_int_flag = 0;

	/* Enable the timer */
	socle_timer_write(SOCLE_TMR0CON, socle_timer_conf[0], socle_timer_base);
	socle_timer_write(SOCLE_TMR1CON, socle_timer_conf[1], socle_timer_base);

	if (socle_wait_for_int(&socle_timer_int_flag, 10)) {
		printf("Timeout\n");
		err_flag = -1;
	} else
		printf("%d second\n", socle_timer_sec);

	/* Disable the timer 0*/
	socle_timer_write(SOCLE_TMR0CON, SOCLE_TIMER_DIS, socle_timer_base);

	/* Disable the timer 1*/
	socle_timer_write(SOCLE_TMR1CON, SOCLE_TIMER_DIS, socle_timer_base);

	return err_flag;
}

extern int socle_timer_mode_2(int autotest)
{
	int err_flag = 0;

	socle_timer_chain_mode = 2;
	socle_timer_write(SOCLE_TMRMODE, SOCLE_TIMER_CHAIN_MODE_2, socle_timer_base);
	socle_timer_conf[0] = SOCLE_TIMER_EN |
		SOCLE_TIMER_CNT_MODE_PERIODICAL |
		SOCLE_TIMER_PRESCALE_FACTOR_1_1 |
		SOCLE_TIMER_INT_MASK_DIS |
		SOCLE_TIMER_INT_EDGE_TRIG;
	socle_timer_conf[1] = SOCLE_TIMER_EN |
		SOCLE_TIMER_CNT_MODE_PERIODICAL |
		SOCLE_TIMER_PRESCALE_FACTOR_1_1 |
		SOCLE_TIMER_INT_MASK_DIS |
		SOCLE_TIMER_INT_EDGE_TRIG;
	socle_timer_conf[2] = SOCLE_TIMER_EN |
		SOCLE_TIMER_CNT_MODE_FREE_RUNNING |
		SOCLE_TIMER_PRESCALE_FACTOR_1_1 |
		SOCLE_TIMER_INT_MASK_DIS |
		SOCLE_TIMER_INT_LEVEL_TRIG;
	socle_timer_write(SOCLE_TMR0LR, socle_timer_apb_clk*2, socle_timer_base);
	socle_timer_write(SOCLE_TMR1LR, 2, socle_timer_base);
	socle_timer_write(SOCLE_TMR2LR, 2, socle_timer_base);
	socle_timer_sec = 0;
	printf("Timer will count 8 seconds at once\n");

	socle_timer_int_flag = 0;

	/* Enable the timer */
	socle_timer_write(SOCLE_TMR0CON, socle_timer_conf[0], socle_timer_base);
	socle_timer_write(SOCLE_TMR1CON, socle_timer_conf[1], socle_timer_base);
	socle_timer_write(SOCLE_TMR2CON, socle_timer_conf[2], socle_timer_base);

	if (socle_wait_for_int(&socle_timer_int_flag, 15)) {
		printf("Timeout\n");
		err_flag = -1;
	} else
		printf("%d second\n", socle_timer_sec);

	/* Disable the timer 0*/
	socle_timer_write(SOCLE_TMR0CON, SOCLE_TIMER_DIS, socle_timer_base);

	/* Disable the timer 1*/
	socle_timer_write(SOCLE_TMR1CON, SOCLE_TIMER_DIS, socle_timer_base);

	/* Disable the timer 1*/
	socle_timer_write(SOCLE_TMR2CON, SOCLE_TIMER_DIS, socle_timer_base);

	return err_flag;
}

extern struct test_item_container socle_timer_cnt_mode_test_container;

extern int
socle_timer_mode_0_timer_0_test(int autotest)
{
	int ret = 0;

	socle_timer_cur_idx = 0;
	ret = test_item_ctrl(&socle_timer_cnt_mode_test_container, autotest);
	return ret;
}

extern int 
socle_timer_mode_0_timer_1_test(int autotest)
{
	int ret = 0;

	socle_timer_cur_idx = 1;
	ret = test_item_ctrl(&socle_timer_cnt_mode_test_container, autotest);
	return ret;
}

extern int 
socle_timer_mode_0_timer_2_test(int autotest)
{
	int ret = 0;

	socle_timer_cur_idx = 2;
	ret = test_item_ctrl(&socle_timer_cnt_mode_test_container, autotest);
	return ret;
}

extern int 
socle_timer_cnt_free_running(int autotest)
{
	int timer_conf_idx;
	int timer_load_idx;
	int err_flag = 0;

	switch(socle_timer_cur_idx) {
	case 0:
		timer_conf_idx = SOCLE_TMR0CON;
		timer_load_idx = SOCLE_TMR0LR;
		break;
	case 1:
		timer_conf_idx = SOCLE_TMR1CON;
		timer_load_idx = SOCLE_TMR1LR;
		break;
	case 2:
		timer_conf_idx = SOCLE_TMR2CON;
		timer_load_idx = SOCLE_TMR2LR;
		break;
	default:
		printf("Unknown timer's configuration base\n");
		return -1;
	}
	socle_timer_conf[socle_timer_cur_idx] = SOCLE_TIMER_EN |
		SOCLE_TIMER_CNT_MODE_FREE_RUNNING |
		SOCLE_TIMER_PRESCALE_FACTOR_1_1 |
		SOCLE_TIMER_INT_MASK_DIS |
		SOCLE_TIMER_INT_LEVEL_TRIG;
	socle_timer_write(timer_load_idx, socle_timer_apb_clk*2, socle_timer_base);
	socle_timer_sec = 0;

	printf("Timer will count 2 seconds at once\n");
	socle_timer_int_flag = 0;

	/* Enable the timer */
	socle_timer_write(timer_conf_idx, socle_timer_conf[socle_timer_cur_idx], socle_timer_base);

	if (socle_wait_for_int(&socle_timer_int_flag, 5)) {
		printf("Timeout\n");
		err_flag = -1;
		goto out;
	} else
		printf("%d second\n", socle_timer_sec);
	if (socle_wait_for_int(&socle_timer_int_flag, 5))
		printf("Timeout\n");
	else {
		printf("%d second\n", socle_timer_sec);
		err_flag = -1;
	}

out:		
	/* Disable the timer */
	socle_timer_write(timer_conf_idx, SOCLE_TIMER_DIS, socle_timer_base);
			
	return err_flag;
}

extern int 
socle_timer_cnt_periodical(int autotest)
{
	int timer_conf_idx;
	int timer_load_idx;
	int err_flag = 0;

	switch(socle_timer_cur_idx) {
	case 0:
		timer_conf_idx = SOCLE_TMR0CON;
		timer_load_idx = SOCLE_TMR0LR;
		break;
	case 1:
		timer_conf_idx = SOCLE_TMR1CON;
		timer_load_idx = SOCLE_TMR1LR;
		break;
	case 2:
		timer_conf_idx = SOCLE_TMR2CON;
		timer_load_idx = SOCLE_TMR2LR;
		break;
	default:
		printf("Unknown timer's configuration base\n");
		return -1;
	}
	socle_timer_conf[socle_timer_cur_idx] = SOCLE_TIMER_EN |
		SOCLE_TIMER_CNT_MODE_PERIODICAL |
		SOCLE_TIMER_PRESCALE_FACTOR_1_1 |
		SOCLE_TIMER_INT_MASK_DIS |
		SOCLE_TIMER_INT_LEVEL_TRIG;
	socle_timer_write(timer_load_idx, socle_timer_apb_clk*2, socle_timer_base);
	socle_timer_sec = 0;

	printf("Timer will count 2 seconds and repeat it again\n");

	socle_timer_int_flag = 0;

	/* Enable the timer */
	socle_timer_write(timer_conf_idx, socle_timer_conf[socle_timer_cur_idx], socle_timer_base);

	if (socle_wait_for_int(&socle_timer_int_flag, 5)) {
		printf("Timeout\n");
		err_flag = -1;
		goto out;
	} else
		printf("%d second\n", socle_timer_sec);
	if (socle_wait_for_int(&socle_timer_int_flag, 5)) {
		printf("Timeout\n");
		err_flag = -1;
	}else 
		printf("%d second\n", socle_timer_sec);
	
out:
	/* Disable the timer */
	socle_timer_write(timer_conf_idx, SOCLE_TIMER_DIS, socle_timer_base);

	return err_flag;
}

static void 
socle_timer_isr_0(void *param)
{
	socle_timer_int_flag = 1;

	/* Clear interrupt */
	socle_timer_write(SOCLE_TMR0CON, 
			  socle_timer_read(SOCLE_TMR0CON, socle_timer_base) & (~SOCLE_TIMER_INT_FLAG),
			  socle_timer_base);

	socle_timer_sec += 2;
}

static void 
socle_timer_isr_1(void *param)
{
	socle_timer_int_flag = 1;

	/* Clear interrupt */
	socle_timer_write(SOCLE_TMR1CON, 
			  socle_timer_read(SOCLE_TMR1CON, socle_timer_base) & (~SOCLE_TIMER_INT_FLAG),
			  socle_timer_base);

	if (1 == socle_timer_chain_mode)
		socle_timer_sec = 4;
	else
		socle_timer_sec += 2;
}

static void 
socle_timer_isr_2(void *param)
{
	socle_timer_int_flag = 1;

	/* Clear interrupt */
	socle_timer_write(SOCLE_TMR2CON, 
			  socle_timer_read(SOCLE_TMR2CON, socle_timer_base) & (~SOCLE_TIMER_INT_FLAG),
			  socle_timer_base);

	if (2 == socle_timer_chain_mode)
		socle_timer_sec = 8;
	else
		socle_timer_sec += 2;
}
