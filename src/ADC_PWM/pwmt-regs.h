#ifndef __SOCLE_PWM_REG_H
#define __SOCLE_PWM_REG_H

#include "dependency.h"

#define PWMT_BASE_OFFSET		0X0010

#define PWMT_CNTR				0X0000
#define PWMT_HRC				0X0004
#define PWMT_LRC				0X0008
#define PWMT_CTRL				0X000C


// PWMT_CTRL (0X000C)
#define PWMT_CTRL_PRESCALE_MSK	0x00003E00
#define PWMT_CTRL_PRESCALE_S	9

#define PWMT_CTRL_CAP			BIT_SHIHT(8)
#define PWMT_CTRL_RST			BIT_SHIHT(7)
#define PWMT_CTRL_INT_CLR		BIT_SHIHT(6)
#define PWMT_CTRL_INT_EN		BIT_SHIHT(5)
#define PWMT_CTRL_SIG_CNTR		BIT_SHIHT(4)
#define PWMT_CTRL_OPT_EN		BIT_SHIHT(3)
#define PWMT_CTRL_EN			BIT_SHIHT(0)

struct socle_pwmt {
	int busy;
	unsigned int base;
	int irq;
	struct socle_pwmt_driver *drv;
};

struct socle_pwmt_driver {
	void (*claim_pwm_lock)(void);
	void (*release_pwm_lock)(void);
	void (*reset)(struct socle_pwmt *);
	void (*output_enable)(struct socle_pwmt *, int);		// 1:enable, 0:disable
	void (*enable)(struct socle_pwmt *, int);				// 1:enable, 0:disable
	void (*capture_mode_enable)(struct socle_pwmt *, int);				// 1:enable, 0:disable
	void (*clear_interrupt)(struct socle_pwmt *);
	void (*enable_interrupt)(struct socle_pwmt *, int);		// 1:enable, 0:disable
	void (*single_counter_mode_enable)(struct socle_pwmt *, int);				// 1:enable, 0:disable
	void (*set_counter)(struct socle_pwmt *, unsigned int);
	unsigned int (*read_hrc)(struct socle_pwmt *);
	unsigned int (*read_lrc)(struct socle_pwmt *);
	void (*write_hrc)(struct socle_pwmt *, unsigned int);
	void (*write_lrc)(struct socle_pwmt *, unsigned int);
	unsigned int (*read_prescale_factor)(struct socle_pwmt *);
	void (*write_prescale_factor)(struct socle_pwmt *, unsigned int);
	void (*set_ctrl)(struct socle_pwmt *, unsigned int);
};


extern struct socle_pwmt * get_socle_pwmt_structure(int num);
extern int release_socle_pwmt_structure(int num);
extern void socle_init_pwmt(void);


#endif	//__SOCLE_PWM_REG_H
