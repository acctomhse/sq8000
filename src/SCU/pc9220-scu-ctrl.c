#include <global.h>
#include <test_item.h>
#include <scu.h>


#include "../RTC/rtc-ctrl.h"
#include "../RTC/rtc-regs.h"

extern void cpu_arm926_do_idle(void);

extern void
cpu_arm926_do_idle(void)
{
	unsigned long tmp0, tmp1, tmp2;

	__asm__ __volatile__(
		"mov     %0, #0 \n"
		"mrc     p15, 0, %1, c1, c0, 0 \n"			// Read control register
		"mcr     p15, 0, %0, c7, c10, 4 \n"			// Drain write buffer
		"bic     %2, %1, #1 << 12 \n"
		"mcr     p15, 0, %2, c1, c0, 0 \n"			// Disable I cache
		"mcr     p15, 0, %0, c7, c0, 4 \n"			// Wait for interrupt
		"mcr     p15, 0, %1, c1, c0, 0 \n"			// Restore ICache enable
		: "=r" (tmp0), "=r" (tmp1), "=r" (tmp2)
		:
		: "memory");
}


static void wake_up_with_rtc_second(void);

extern struct test_item_container socle_mode_main_container;

extern int
pc9220_scu_test(int autotest)
{
	return test_item_ctrl(&socle_mode_main_container, autotest);
}


extern struct test_item_container socle_sleep_main_container;

extern int
socle_sleep_mode_test(int autotest)
{
	return test_item_ctrl(&socle_sleep_main_container, autotest);
}


extern struct test_item_container socle_stop_main_container;

extern int
socle_stop_mode_test(int autotest)
{
	return test_item_ctrl(&socle_stop_main_container, autotest);
}


extern int
socle_sleep_with_irq_test(int autotest)
{
	u32 check;

	check = ioread32(SOCLE_RTC_TIME);
	RESET_RTC_CIRCUIT();
	MSDELAY(1000);
	if(ioread32(SOCLE_RTC_TIME) == check)
		RESET_RTC_COUNTER();       
	if (RTC_IS_PWFAIL) {
		printf("Warning: RTC Power Fail!!\n");
		printf("\tPlease change your battery and then reset RTC again...\n");
	}
	if(!RTC_IS_GOOD){
		printf("RTC is not Power Good\n");
	}

	RTC_SET_DIVIDER(RTC_DIVIDER);
	
	RTC_EN();

	wake_up_with_rtc_second();

	printf("Sleeping... ");
	socle_scu_pw_standbywfi_enable(1);
	cpu_arm926_do_idle();
	socle_scu_pw_standbywfi_enable(0);
	printf("Wake up!\n");

	RTC_DIS();

	return 0;
}

extern int
socle_sleep_with_fiq_test(int autotest)
{
	printf("Press FIQ pin to wake up the system!\n");

	printf("Sleeping... ");
	socle_scu_pw_standbywfi_enable(1);
	socle_scu_dev_enable(SOCLE_DEVCON_EXT_INT0_NFIQ);
	cpu_arm926_do_idle();
	socle_scu_pw_standbywfi_enable(0);
	printf("Wake up!\n");

	return 0;
}

extern int
socle_stop_with_irq_test(int autotest)
{
	u32 check;

	check = ioread32(SOCLE_RTC_TIME);
	RESET_RTC_CIRCUIT();
	MSDELAY(1000);
	if(ioread32(SOCLE_RTC_TIME) == check)
		RESET_RTC_COUNTER();       
	if (RTC_IS_PWFAIL) {
		printf("Warning: RTC Power Fail!!\n");
		printf("\tPlease change your battery and then reset RTC again...\n");
	}
	if(!RTC_IS_GOOD){
		printf("RTC is not Power Good\n");
	}

	RTC_SET_DIVIDER(RTC_DIVIDER);
	
	RTC_EN();

	wake_up_with_rtc_second();

	printf("Stoping... ");
	socle_scu_stop_mode_enable(1);
	printf("Wake up!\n");

	RTC_DIS();

	return 0;
}

extern int
socle_stop_with_fiq_test(int autotest)
{
	printf("Press FIQ pin to wake up the system!\n");

	printf("Stoping... ");
	socle_scu_dev_enable(SOCLE_DEVCON_EXT_INT0_NFIQ);
	socle_scu_stop_mode_enable(1);
	printf("Wake up!\n");

	return 0;
}


///////////////////////////// RTC
static struct rtc_date dt, dt_a;
static struct rtc_time tm, tm_a;

static void
rtc_isr_alarm_hit(void* pparam)
{
	int	*p_hit = (int*)pparam;

	RTC_ALRM_DIS();
	
	printf("RTC Alarm!!\n");
	*p_hit = 1;
}

static void
rtc_reset(void)
{
	struct rtc_date dt;
	struct rtc_time tm;
	
	RTC_ALRM_DIS();
	
	// Set date and time
	dt.ten_cent = 0;
	dt.cent     = 0;
	dt.ten_yr   = 0;
	dt.yr       = 0;
	dt.ten_mth  = 0;
	dt.mth      = 0;
	dt.ten_day  = 0;
	dt.day      = 0;
	
	tm.dow      = 0;
	tm.ten_hr   = 0;
	tm.hr       = 0;
	tm.ten_min  = 0;
	tm.min      = 0;
	tm.ten_sec  = 0;
	tm.sec      = 0;
	tm.sos      = 0;
	
	rtc_set_date(&dt);
	rtc_set_time(&tm);
}


static int
rtc_sub_alarm_test(u32_t f_time, u32_t f_date)
{
	volatile int	hit;
	int result = 0;
	
	rtc_reset();

	printf("RTC Time = W%d %d%d:%d%d:%d%d:%d\n", tm.dow, tm.ten_hr, tm.hr,	\
				tm.ten_min, tm.min, tm.ten_sec, tm.sec, tm.sos);

	printf("RTC Date = C%d%d %d%d/%d%d/%d%d\n", dt.ten_cent, dt.cent, dt.ten_yr, dt.yr,	\
				dt.ten_mth, dt.mth, dt.ten_day, dt.day);

	printf("Alarm Time = W%d %d%d:%d%d:%d%d:%d\n", tm_a.dow, tm_a.ten_hr, tm_a.hr,	\
				tm_a.ten_min, tm_a.min, tm_a.ten_sec, tm_a.sec, tm_a.sos);

	printf("Alarm Date = C%d%d %d%d/%d%d/%d%d\n", dt_a.ten_cent, dt_a.cent, dt_a.ten_yr, dt_a.yr,	\
				dt_a.ten_mth, dt_a.mth, dt_a.ten_day, dt_a.day);

	printf("Alarm Time on: ");
	if (f_time & SOCLE_RTC_TALRM_CSOS)
		printf("\"Sixteen of Second\" ");
	if (f_time & SOCLE_RTC_TALRM_CS)
		printf("\"Second\" ");
	if (f_time & SOCLE_RTC_TALRM_CM)
		printf("\"Minute\" ");
	if (f_time & SOCLE_RTC_TALRM_CH)
		printf("\"Hour\" ");
	if (f_time & SOCLE_RTC_TALRM_CDOW)
		printf("\"Day of Week\"");
	printf("\n");	

	printf("Alarm Date on: ");
	if (f_date & SOCLE_RTC_DALRM_CD)
		printf("\"Day\" ");
	if (f_date & SOCLE_RTC_DALRM_CM)
		printf("\"Month\" ");
	if (f_date & SOCLE_RTC_DALRM_CY)
		printf("\"Year\" ");
	if (f_date & SOCLE_RTC_DALRM_CC)
		printf("\"Century\"");
	printf("\n");

	rtc_set_time_alarm(&tm_a, f_time);
	rtc_set_date_alarm(&dt_a, f_date);

	hit = 0;

	// enable interrupt
	request_irq(RTC_INT, rtc_isr_alarm_hit, (void *)&hit);

	rtc_set_date(&dt);
	rtc_set_time(&tm);

#if 0
	printf("Waiting for the alarm...\n");

	if (socle_wait_for_int(&hit, 10)) {
		printf("Timeout!!\n");
		result = -1;
	}

	// disable interrupt
	free_irq(RTC_INT);
#endif

	return result;
}

static void
wake_up_with_rtc_second(void)
{
	// Set alarm date and alarm time
	// alarm date = C21 06/12/31
	dt_a.ten_cent = 2;
	dt_a.cent     = 1;
	dt_a.ten_yr   = 0;
	dt_a.yr       = 6;
	dt_a.ten_mth  = 1;
	dt_a.mth      = 2;
	dt_a.ten_day  = 3;
	dt_a.day      = 1;
	
	// alarm time = Wed 17:58:59.1
	tm_a.dow      = 3;
	tm_a.ten_hr   = 1;
	tm_a.hr       = 7;
	tm_a.ten_min  = 5;
	tm_a.min      = 8;
	tm_a.ten_sec  = 5;
	tm_a.sec      = 9;
	tm_a.sos      = 1;
	
	// Set date and time
	// date = C21 06/12/31
	dt.ten_cent = 2;
	dt.cent     = 1;
	dt.ten_yr   = 0;
	dt.yr       = 6;
	dt.ten_mth  = 1;
	dt.mth      = 2;
	dt.ten_day  = 3;
	dt.day      = 1;
	
	// time = Wed 17:58:54.0
	tm.dow      = 3;
	tm.ten_hr   = 1;
	tm.hr       = 7;
	tm.ten_min  = 5;
	tm.min      = 8;
	tm.ten_sec  = 5;
	tm.sec      = 4;
	tm.sos      = 0;
	
	rtc_sub_alarm_test(SOCLE_RTC_TALRM_CS, 0);
}

