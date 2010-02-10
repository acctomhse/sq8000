#include <platform.h>
#include <irqs.h>

#ifdef CONFIG_PC9220
#define LCM_EINK_TEST		1
#else
#define LCM_EINK_TEST		0
#endif

#ifdef CONFIG_INR_PC7230
#define LCM_T6963C_TEST		1
#else
#define LCM_T6963C_TEST		0
#endif

#ifdef CONFIG_INR_PC7230
#define T6963C_BASE		SOCLE_NOR_FLASH_BANK0
#define T6963C_BKLGT_PWM_IDX		0	// backlight
#define T6963C_CNTRT_PWM_IDX		1	// contrast
#else
#define T6963C_BASE		-1
#define T6963C_BKLGT_PWM_IDX		-1
#define T6963C_CNTRT_PWM_IDX		-1
#endif

