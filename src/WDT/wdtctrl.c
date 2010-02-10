#include <platform.h>
#include <irqs.h>
#include "regs-wdt.h"
#if defined (CONFIG_PC9220)
#include <scu.h>
#endif

extern int
wdt_watching(int autotest)
{
	int ret = 0, pseudo_flag = 0;

#if defined(CONFIG_CDK) || defined(CONFIG_PC9002) || defined(CONFIG_SCDK)
	iowrite32(ioread32(SOCLE_APB0_SCU + 0x28) | (0x1 << 13), SOCLE_APB0_SCU + 0x28);
#endif
#ifdef CONFIG_PC9220
	socle_scu_wdt_reset_enable(1);
#endif 

	// set reload reg, prescaler
	iowrite32(socle_get_apb_clock() / 60 * 3, SOCLE_WDTLR);
	WDT_PRESCALE(PRESCALE_64);

	printf("WDT: The system will reset now!!\n");

	WDT_RST_EN();
	WDT_EN();

	if (socle_wait_for_int(&pseudo_flag, 5)) {
		printf("Timeout!! The system does not reset!!\n");
		ret = -1;
	}

	return ret;
}
