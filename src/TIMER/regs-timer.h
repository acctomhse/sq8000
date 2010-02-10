#ifndef _regs_timer_H_
#define _regs_timer_H_

#if defined(CONFIG_LDK5) && defined(CONFIG_ARM7_HI)
#define TIMER_REG_BASE		SOCLE_APB1_TIMER
#endif

#ifndef TIMER_REG_BASE
#define TIMER_REG_BASE		SOCLE_APB0_TIMER
#endif

#define SOCLE_TMR0LR		(addr_t)(TIMER_REG_BASE + 0x0000)
#define SOCLE_TMR0CVR		(addr_t)(TIMER_REG_BASE + 0x0004)
#define SOCLE_TMR0CON		(addr_t)(TIMER_REG_BASE + 0x0008)
#define SOCLE_TMR1LR		(addr_t)(TIMER_REG_BASE + 0x0010)
#define SOCLE_TMR1CVR		(addr_t)(TIMER_REG_BASE + 0x0014)
#define SOCLE_TMR1CON		(addr_t)(TIMER_REG_BASE + 0x0018)

// control flag
#define TMR_EN                (1<<8)
#define TMR_PERIOD_MODE       (1<<7)  // 0:free-running, 1:periodical
#define TMR_PRESCALE_SHIFT    4
#define TMR_IMASK             (1<<3)


///////////////////////////////////
//
// macro lib
//
////////////////////////////////////

//
// TMR0
//

#define TMR0_SET_FLAG(flag)			(writew(readw(SOCLE_TMR0CON) |  flag, SOCLE_TMR0CON))
#define TMR0_CLR_FLAG(flag)			(writew(readw(SOCLE_TMR0CON) & ~flag, SOCLE_TMR0CON))
#define TMR0_CLR_INT()				(writew(readw(SOCLE_TMR0CON) & ~0x04, SOCLE_TMR0CON))

#define TMR0_EN()					TMR0_SET_FLAG(TMR_EN)
#define TMR0_DIS()					TMR0_CLR_FLAG(TMR_EN)
#define TMR0_PERIOD_MODE()			TMR0_SET_FLAG(TMR_PERIOD_MODE)
#define TMR0_FREE_RUN_MODE()		TMR0_CLR_FLAG(TMR_PERIOD_MODE)
#define TMR0_PRESCALE(x)			(writew((readw(SOCLE_TMR0CON) & 0xffffff8f) | ((x) << TMR_PRESCALE_SHIFT), SOCLE_TMR0CON))
#define TMR0_INT_MASK()				TMR0_SET_FLAG(TMR_IMASK)
#define TMR0_INT_UNMASK()			TMR0_CLR_FLAG(TMR_IMASK)
#define TMR0_INT_CLEAR()			TMR0_SET_FLAG(TMR_INT_CLEAR)
#define TMR0_INT_PENDING()			TMR0_SET_FLAG(TMR_INT_PENDING)
#define TMR0_INT_EDGE_TRIGGER()		TMR0_SET_FLAG(TMR_INT_EDGE_TRIGGER)
#define TMR0_INT_LEVEL_TRIGGER()	TMR0_SET_FLAG(TMR_INT_LEVEL_TRIGGER)

#define TMR1_SET_FLAG(flag)			(writew(readw(SOCLE_TMR1CON) |  flag, SOCLE_TMR1CON))
#define TMR1_CLR_FLAG(flag)			(writew(readw(SOCLE_TMR1CON) & ~flag, SOCLE_TMR1CON))
#define TMR1_CLR_INT()				(writew(readw(SOCLE_TMR1CON) & ~0x04, SOCLE_TMR1CON))

#define TMR1_EN()					TMR1_SET_FLAG(TMR_EN)
#define TMR1_DIS()					TMR1_CLR_FLAG(TMR_EN)
#define TMR1_PERIOD_MODE()			TMR1_SET_FLAG(TMR_PERIOD_MODE)
#define TMR1_FREE_RUN_MODE()		TMR1_CLR_FLAG(TMR_PERIOD_MODE)
#define TMR1_PRESCALE(x)			(writew((readw(SOCLE_TMR1CON) & 0xffffff8f) | ((x) << TMR_PRESCALE_SHIFT), SOCLE_TMR1CON))
#define TMR1_INT_MASK()				TMR1_SET_FLAG(TMR_IMASK)
#define TMR1_INT_UNMASK()			TMR1_CLR_FLAG(TMR_IMASK)
#define TMR1_INT_CLEAR()			TMR1_SET_FLAG(TMR_INT_CLEAR)
#define TMR1_INT_PENDING()			TMR1_SET_FLAG(TMR_INT_PENDING)
#define TMR1_INT_EDGE_TRIGGER()		TMR1_SET_FLAG(TMR_INT_EDGE_TRIGGER)
#define TMR1_INT_LEVEL_TRIGGER()	TMR1_SET_FLAG(TMR_INT_LEVEL_TRIGGER)

//define Timer TMRxCON
#define TMR_INT_CLEAR         (1<<2)
#define TMR_INT_PENDING       (1<<1)	// ?????
#define TMR_INT_EDGE_TRIGGER  (0<<1)
#define TMR_INT_LEVEL_TRIGGER (1<<1)

// const for TIMER and WATCHDOG
#define PRESCALE_1              0
#define PRESCALE_4              1
#define PRESCALE_8              2
#define PRESCALE_16             3
#define PRESCALE_32             4
#define PRESCALE_64             5
#define PRESCALE_128            6
#define PRESCALE_256            7

#endif //_regs_timer_H_
