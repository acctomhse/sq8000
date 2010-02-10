#include <global.h>
#include <irqs.h>
#include "pwmt-regs.h"
#include <scu.h>

//#define CONFIG_SOCLE_PWMT_DEBUG
#ifdef CONFIG_SOCLE_PWMT_DEBUG
	#define PWMT_DBG(fmt, args...) printf("SOCLE_PWMT: " fmt, ## args)
#else
	#define PWMT_DBG(fmt, args...)
#endif


static int pwmt_lock = 0;

/* socle_pwmt_driver */
static void socle_pwmt_claim_lock(void);
static void socle_pwmt_release_lock(void);
static void socle_pwmt_reset(struct socle_pwmt *p_pwm_st);
static void socle_pwmt_output_enable(struct socle_pwmt *p_pwm_st, int en);
static void socle_pwmt_enable(struct socle_pwmt *p_pwm_st, int en);
static void socle_pwmt_capture_mode_enable(struct socle_pwmt *p_pwm_st, int en);
static void socle_pwmt_clear_interrupt(struct socle_pwmt *p_pwm_st);
static void socle_pwmt_enable_interrupt(struct socle_pwmt *p_pwm_st, int en);
static void socle_pwmt_single_counter_mode_enable(struct socle_pwmt *p_pwm_st, int en);
static void socle_pwmt_set_counter(struct socle_pwmt *p_pwm_st, u32 data);
static u32 socle_pwmt_read_hrc(struct socle_pwmt *p_pwm_st);
static u32 socle_pwmt_read_lrc(struct socle_pwmt *p_pwm_st);
static void socle_pwmt_write_hrc(struct socle_pwmt *p_pwm_st, u32 data);
static void socle_pwmt_write_lrc(struct socle_pwmt *p_pwm_st, u32 data);
static u32 socle_pwmt_read_prescale_factor(struct socle_pwmt *p_pwm_st);
static void socle_pwmt_write_prescale_factor(struct socle_pwmt *p_pwm_st, u32 data);
static void socle_pwmt_set_ctrl(struct socle_pwmt *p_pwm_st, u32 data);

static struct socle_pwmt_driver socle_pwm_drv = {
	.claim_pwm_lock	= socle_pwmt_claim_lock,
	.release_pwm_lock	= socle_pwmt_release_lock,
	.reset	= socle_pwmt_reset,
	.output_enable	= socle_pwmt_output_enable,
	.enable	= socle_pwmt_enable,
	.capture_mode_enable	= socle_pwmt_capture_mode_enable,
	.clear_interrupt	= socle_pwmt_clear_interrupt,
	.enable_interrupt	= socle_pwmt_enable_interrupt,
	.single_counter_mode_enable	= socle_pwmt_single_counter_mode_enable,
	.set_counter	= socle_pwmt_set_counter,
	.read_hrc	= socle_pwmt_read_hrc,
	.read_lrc	= socle_pwmt_read_lrc,
	.write_hrc	= socle_pwmt_write_hrc,
	.write_lrc	= socle_pwmt_write_lrc,
	.read_prescale_factor	= socle_pwmt_read_prescale_factor,
	.write_prescale_factor	= socle_pwmt_write_prescale_factor,
	.set_ctrl	= socle_pwmt_set_ctrl,
};

/* socle pwmt devie */
static struct socle_pwmt socle_pwm[SOCLE_PWM_NUM];


static inline void
pwmt_read(u32 offset, u32 *data, u32 base)
{
	*data = ioread32(base + offset);

	PWMT_DBG("pwmt_read(): base:0x%08x, offset:0x%08x, data:0x%08x\n", base, offset, *data);
}

static inline void
pwmt_write(u32 offset, u32 data, u32 base)
{
	iowrite32(data, base + offset);

	PWMT_DBG("pwmt_write(): base:0x%08x, offset:0x%08x, data:0x%08x\n", base, offset, data);
}

static void
socle_pwmt_claim_lock(void)
{
	pwmt_lock = 1;
}

static void
socle_pwmt_release_lock(void)
{
	pwmt_lock = 0;
}

static void
socle_pwmt_reset(struct socle_pwmt *p_pwm_st)
{
	pwmt_write(PWMT_CTRL, PWMT_CTRL_RST, p_pwm_st->base);
}

static void
socle_pwmt_output_enable(struct socle_pwmt *p_pwm_st, int en)
{
	u32 tmp;

	pwmt_read(PWMT_CTRL, &tmp, p_pwm_st->base);

	if (en)
		pwmt_write(PWMT_CTRL, tmp | PWMT_CTRL_OPT_EN, p_pwm_st->base);
	else
		pwmt_write(PWMT_CTRL, tmp & ~PWMT_CTRL_OPT_EN, p_pwm_st->base);
}

static void
socle_pwmt_enable(struct socle_pwmt *p_pwm_st, int en)
{
	u32 tmp;
	
	pwmt_read(PWMT_CTRL, &tmp, p_pwm_st->base);

	if (en)
		pwmt_write(PWMT_CTRL, tmp | PWMT_CTRL_EN, p_pwm_st->base);
	else
		pwmt_write(PWMT_CTRL, tmp & ~PWMT_CTRL_EN, p_pwm_st->base);
}

static void
socle_pwmt_capture_mode_enable(struct socle_pwmt *p_pwm_st, int en)
{
	u32 tmp;
	
	pwmt_read(PWMT_CTRL, &tmp, p_pwm_st->base);

	if (en)
		pwmt_write(PWMT_CTRL, tmp | PWMT_CTRL_CAP, p_pwm_st->base);
	else
		pwmt_write(PWMT_CTRL, tmp & ~PWMT_CTRL_CAP, p_pwm_st->base);
}

static void
socle_pwmt_clear_interrupt(struct socle_pwmt *p_pwm_st)
{
	u32 tmp;
	
	pwmt_read(PWMT_CTRL, &tmp, p_pwm_st->base);
	pwmt_write(PWMT_CTRL, tmp | PWMT_CTRL_INT_CLR, p_pwm_st->base);
}

static void
socle_pwmt_enable_interrupt(struct socle_pwmt *p_pwm_st, int en)
{
	u32 tmp;
	
	pwmt_read(PWMT_CTRL, &tmp, p_pwm_st->base);

	if (en)
		pwmt_write(PWMT_CTRL, tmp | PWMT_CTRL_INT_EN, p_pwm_st->base);
	else
		pwmt_write(PWMT_CTRL, tmp & ~PWMT_CTRL_INT_EN, p_pwm_st->base);
}

static void
socle_pwmt_single_counter_mode_enable(struct socle_pwmt *p_pwm_st, int en)
{
	u32 tmp;
	
	pwmt_read(PWMT_CTRL, &tmp, p_pwm_st->base);

	if (en)
		pwmt_write(PWMT_CTRL, tmp | PWMT_CTRL_SIG_CNTR, p_pwm_st->base);
	else
		pwmt_write(PWMT_CTRL, tmp & ~PWMT_CTRL_SIG_CNTR, p_pwm_st->base);
}

static void
socle_pwmt_set_counter(struct socle_pwmt *p_pwm_st, u32 data)
{
	pwmt_write(PWMT_CNTR, data, p_pwm_st->base);
}

static u32
socle_pwmt_read_hrc(struct socle_pwmt *p_pwm_st)
{
	u32 data;

	pwmt_read(PWMT_HRC, &data, p_pwm_st->base);
	
	return data;
}

static u32
socle_pwmt_read_lrc(struct socle_pwmt *p_pwm_st)
{
	u32 data;

	pwmt_read(PWMT_LRC, &data, p_pwm_st->base);
	
	return data;
}

static void
socle_pwmt_write_hrc(struct socle_pwmt *p_pwm_st, u32 data)
{
	pwmt_write(PWMT_HRC, data, p_pwm_st->base);
}

static void
socle_pwmt_write_lrc(struct socle_pwmt *p_pwm_st, u32 data)
{
	pwmt_write(PWMT_LRC, data, p_pwm_st->base);
}

static u32
socle_pwmt_read_prescale_factor(struct socle_pwmt *p_pwm_st)
{
	u32 data;

	pwmt_read(PWMT_CTRL, &data, p_pwm_st->base);
	data = (data & PWMT_CTRL_PRESCALE_MSK) >> PWMT_CTRL_PRESCALE_S;

	return data;
}

static void
socle_pwmt_write_prescale_factor(struct socle_pwmt *p_pwm_st, u32 data)
{
	u32 tmp;

	pwmt_read(PWMT_CTRL, &tmp, p_pwm_st->base);
	pwmt_write(PWMT_CTRL, (tmp & PWMT_CTRL_PRESCALE_MSK) | (data << PWMT_CTRL_PRESCALE_S), p_pwm_st->base);
}

static void
socle_pwmt_set_ctrl(struct socle_pwmt *p_pwm_st, u32 data)
{
	pwmt_write(PWMT_CTRL, data, p_pwm_st->base);
}


extern struct socle_pwmt *
get_socle_pwmt_structure(int num)
{
	struct socle_pwmt *p = NULL;

	if (SOCLE_PWM_NUM <= num) {
		printf("Error request PWMT num = %d!, max num = %d\n", num, SOCLE_PWM_NUM - 1);
		return p;
	}

	socle_pwmt_claim_lock();

	if (socle_pwm[num].busy) {
		printf("PWMT[%d] is busy!\n", num);
	} else {
		socle_pwm[num].busy = 1;
		p = &socle_pwm[num];
	}
	
	PWMT_DBG("PWMT[%d] base = 0x%08x, irq = %d\n", num, p->base, p->irq);

	socle_pwmt_release_lock();

	return p;
}

extern int
release_socle_pwmt_structure(int num)
{
	int ret = 0;

	if (SOCLE_PWM_NUM <= num) {
		printf("Error request PWMT num = %d!, max num = %d\n", num, SOCLE_PWM_NUM - 1);
		return -1;
	}

	socle_pwmt_claim_lock();

	socle_pwm[num].busy = 0;
	
	PWMT_DBG("Release PWMT[%d]!\n", num);

	socle_pwmt_release_lock();

	return ret;
}


extern void
socle_init_pwmt(void)
{
	int irq, i;
	u32 base;

	PWMT_DBG("pwmt init\n");

#ifdef CONFIG_PC9220
	socle_scu_dev_enable(SOCLE_DEVCON_PWM0);
	socle_scu_dev_enable(SOCLE_DEVCON_PWM1);
#endif

	base = SOCLE_APB0_TIMER_PWM;
	irq = SOCLE_INTC_PWM0;

	for (i = 0; i < SOCLE_PWM_NUM; i++) {
		if (i) {
			base += PWMT_BASE_OFFSET;
			irq++;
		}

		PWMT_DBG("socle_pwm[%d] base = 0x%08x, irq = %d\n", i, base, irq);
		socle_pwm[i].base = base;
		socle_pwm[i].irq = irq;
		socle_pwm[i].drv = &socle_pwm_drv;
		socle_pwm[i].busy = 0;

		socle_pwmt_reset(&socle_pwm[i]);
	}
}
