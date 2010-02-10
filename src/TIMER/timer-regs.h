#ifndef __SOCLE_TIMER_REGS_H_INCLUDED
#define __SOCLE_TIMER_REGS_H_INCLUDED

/*
 *  Registers
 *  */
#define SOCLE_TMR0LR 0x0000	/* load register */
#define SOCLE_TMR0CVR 0x0004	/* current value register */
#define SOCLE_TMR0CON 0x0008	/* control register */
#define SOCLE_TMR1LR 0x0010	/* load register */
#define SOCLE_TMR1CVR 0x0014	/* current value register */
#define SOCLE_TMR1CON 0x0018	/* control register */
#define SOCLE_TMR2LR 0x0020	/* load register */
#define SOCLE_TMR2CVR 0x0024	/* current value register */
#define SOCLE_TMR2CON 0x0028	/* control register */
#define SOCLE_TMRMODE 0x0030	/* chain mode register */

/*
 *  SOCLE_TMRxCON
 *  */
/* Timer enable/disable */
#define SOCLE_TIMER_DIS 0x0	/* disable */
#define SOCLE_TIMER_EN (0x1 << 8) /* enable*/

/* Timer counting mode selection */
#define SOCLE_TIMER_CNT_MODE_FREE_RUNNING 0x0 /* free-running */
#define SOCLE_TIMER_CNT_MODE_PERIODICAL (0x1 << 7) /* periodical */

/* Prescale factor */
#define SOCLE_TIMER_PRESCALE_FACTOR_1_1 0x0 /* 1 */
#define SOCLE_TIMER_PRESCALE_FACTOR_1_4 (0x1 << 4) /* 1/4 */
#define SOCLE_TIMER_PRESCALE_FACTOR_1_8 (0x2 << 4) /* 1/8 */
#define SOCLE_TIMER_PRESCALE_FACTOR_1_16 (0x3 << 4) /* 1/16 */
#define SOCLE_TIMER_PRESCALE_FACTOR_1_32 (0x4 << 4) /* 1/32 */
#define SOCLE_TIMER_PRESCALE_FACTOR_1_64 (0x5 << 4) /* 1/64 */
#define SOCLE_TIMER_PRESCALE_FACTOR_1_128 (0x6 << 4) /* 1/128 */
#define SOCLE_TIMER_PRESCALE_FACTOR_1_256 (0x7 << 4) /* 1/256 */

/* Interrupt mask */
#define SOCLE_TIMER_INT_MASK_DIS 0x0	/* disable */
#define SOCLE_TIMER_INT_MASK_EN (0x1 << 3) /* enable */

/* Interrupt clear */
#define SOCLE_TIMER_INT_FLAG (0x1 << 2) /* this bit is set when an interrupt is pending
					    * writing a '0' to this bit will clear the interrupt*/

/* Interrupt control bit */
#define SOCLE_TIMER_INT_EDGE_TRIG 0x0 /* edge-triggered */
#define SOCLE_TIMER_INT_LEVEL_TRIG (0x1 << 1) /* level-triggered */

/*
 *  SOCLE_TMRMODE
 *  */
#define SOCLE_TIMER_CHAIN_MODE_0 0x0 /* mode 0: timer0 or timer1 or timer2 */
#define SOCLE_TIMER_CHAIN_MODE_1 0x1 /* mode 1: chain timer0 and timer1 */
#define SOCLE_TIMER_CHAIN_MODE_2 0x2 /* mode 2: chain timer0 and timer 1 and timer2 */
#endif
