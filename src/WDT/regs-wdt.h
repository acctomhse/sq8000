#ifndef _wdtctrl_h_included_
#define _wdtctrl_h_included_

#include "platform.h"


#if defined(CONFIG_LDK5) && defined(CONFIG_ARM7_HI)
#define WDT_REG_BASE		SOCLE_APB1_WDT
#endif

#ifndef WDT_REG_BASE
#define WDT_REG_BASE		SOCLE_APB0_WDT
#endif


#define SOCLE_WDTLR          (addr_t)(WDT_REG_BASE + 0x0000)
#define SOCLE_WDTCVR         (addr_t)(WDT_REG_BASE + 0x0004)
#define SOCLE_WDTCON         (addr_t)(WDT_REG_BASE + 0x0008)

#define WDT_SHIFT_RST (1<<4)
#define WDT_SHIFT_EN  (1<<3)

// macro lib
#define WDT_CTRL_SET_FLAG(x)    (writew(readw(SOCLE_WDTCON) | (x), SOCLE_WDTCON))
#define WDT_CTRL_CLR_FLAG(x)    (writew(readw(SOCLE_WDTCON) & ~(x),SOCLE_WDTCON))

#define WDT_RST_EN()   			WDT_CTRL_SET_FLAG(WDT_SHIFT_RST)
#define WDT_RST_DIS()  			WDT_CTRL_CLR_FLAG(WDT_SHIFT_RST)
#define WDT_EN()       			WDT_CTRL_SET_FLAG(WDT_SHIFT_EN)
#define WDT_DIS()      			WDT_CTRL_CLR_FLAG(WDT_SHIFT_EN)
#define WDT_PRESCALE(x)         (writew((readw(SOCLE_WDTCON) & ~0x7) | (x) , SOCLE_WDTCON))


// const for TIMER and WATCHDOG
#define PRESCALE_1              0
#define PRESCALE_4              1
#define PRESCALE_8              2
#define PRESCALE_16             3
#define PRESCALE_32             4
#define PRESCALE_64             5
#define PRESCALE_128            6
#define PRESCALE_256            7


#ifdef __cplusplus
extern "C" {
#endif

/*int wdt_raw_register_rw();
int wdt_watching();
int wdt_barking();
void wdt_isr_watching(void* pparam);
void wdt_isr_not_barking(void* pparam);*/
int wdt_test_entry(void);
int wdt_raw_register_rw(void);
int wdt_barking(void);
void wdt_isr_watching(void* pparam);
void wdt_isr_not_barking(void* pparam);


#ifdef __cplusplus
}
#endif

#endif // _wdtctrl_h_included_
