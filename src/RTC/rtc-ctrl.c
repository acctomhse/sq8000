#include <platform.h>
#include <global.h>
#include <irqs.h>
#include <test_item.h>
#include "rtc-ctrl.h"
#include "rtc-regs.h"


//#ifdef CONFIG_RTC_DEBUG
#ifdef CONFIG_RTC_DEBUG
	#define PDEBUG(fmt, args ...)	printf("%s: " fmt, __FUNCTION__ , ## args)
#else
	#define PDEBUG(fmt, args ...)
#endif


/* local function prototype */
static void rtc_reset(void);
static void rtc_isr_alarm_hit(void* pparam);
static int rtc_sub_alarm_test(u32_t f_time, u32_t f_date);
static void rtc_u32_to_time(struct rtc_time* tm, const u32_t temp);
static void rtc_u32_to_date(struct rtc_date* dt, const data_t temp);
static u32_t rtc_time_to_u32(const struct rtc_time* tm);
static u32_t rtc_date_to_u32(const struct rtc_date* dt);


static struct rtc_date dt, dt_a;
static struct rtc_time tm, tm_a;


extern struct test_item_container rtc_main_container;

extern int
RTCTesting(int autotest)
{
	int result = 0;
	
#if defined(CONFIG_LDK5) || defined(CONFIG_PDK) || defined(CONFIG_PC7210)
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
#endif	//CONFIG_LDK5

	RTC_SET_DIVIDER(RTC_DIVIDER);
	
	RTC_EN();
	
	result = test_item_ctrl(&rtc_main_container, autotest);
	
	RTC_DIS();
	
	return result;
}


extern struct test_item_container rtc_alarm_test_container;

extern int
rtc_alarm_test(int autotest)
{
	int result = 0;
	
	result = test_item_ctrl(&rtc_alarm_test_container, autotest);
	
	return result;
}

extern int
rtc_all_alarm_test(int autotest)
{
	int result = 0;
	
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

	// alarm time = Wed 17:58:15.10
	tm_a.dow      = 3;
	tm_a.ten_hr   = 1;
	tm_a.hr       = 7;
	tm_a.ten_min  = 5;
	tm_a.min      = 8;
	tm_a.ten_sec  = 1;
	tm_a.sec      = 5;
	tm_a.sos      = 10;

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
	
	// time = Wed 17:58:10.2
	tm.dow      = 3;
	tm.ten_hr   = 1;
	tm.hr       = 7;
	tm.ten_min  = 5;
	tm.min      = 8;
	tm.ten_sec  = 1;
	tm.sec      = 0;
	tm.sos      = 2;
	
	result = rtc_sub_alarm_test(SOCLE_RTC_DALRM_CD | SOCLE_RTC_DALRM_CM	| SOCLE_RTC_DALRM_CY | SOCLE_RTC_DALRM_CC,
								SOCLE_RTC_TALRM_CSOS | SOCLE_RTC_TALRM_CS | SOCLE_RTC_TALRM_CM | SOCLE_RTC_TALRM_CH | SOCLE_RTC_TALRM_CDOW);
	
	return result;
}

extern int
rtc_sixteen_of_second_test(int autotest)
{
	int result = 0;
	
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
	
	// alarm time = Wed 17:58:59.15
	tm_a.dow      = 3;
	tm_a.ten_hr   = 1;
	tm_a.hr       = 7;
	tm_a.ten_min  = 5;
	tm_a.min      = 8;
	tm_a.ten_sec  = 5;
	tm_a.sec      = 9;
	tm_a.sos      = 15;
	
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
	
	// time = Wed 17:58:59.0
	tm.dow      = 3;
	tm.ten_hr   = 1;
	tm.hr       = 7;
	tm.ten_min  = 5;
	tm.min      = 8;
	tm.ten_sec  = 5;
	tm.sec      = 9;
	tm.sos      = 0;
	
	result = rtc_sub_alarm_test(SOCLE_RTC_TALRM_CSOS, 0);
	
	return result;
}

extern int
rtc_second_test(int autotest)
{
	int result = 0;
	
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
	
	result = rtc_sub_alarm_test(SOCLE_RTC_TALRM_CS, 0);
	
	return result;
}

extern int
rtc_minute_test(int autotest)
{
	int result = 0;
	
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
	
	// alarm time = Wed 17:59:00.1
	tm_a.dow      = 3;
	tm_a.ten_hr   = 1;
	tm_a.hr       = 7;
	tm_a.ten_min  = 5;
	tm_a.min      = 9;
	tm_a.ten_sec  = 0;
	tm_a.sec      = 0;
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
	
	// time = Wed 17:58:55.0
	tm.dow      = 3;
	tm.ten_hr   = 1;
	tm.hr       = 7;
	tm.ten_min  = 5;
	tm.min      = 8;
	tm.ten_sec  = 5;
	tm.sec      = 5;
	tm.sos      = 0;
	
	result = rtc_sub_alarm_test(SOCLE_RTC_TALRM_CM, 0);
	
	return result;
}

extern int
rtc_hour_test(int autotest)
{
	int result = 0;
	
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
	
	// alarm time = Wed 18:00:01.1
	tm_a.dow      = 3;
	tm_a.ten_hr   = 1;
	tm_a.hr       = 8;
	tm_a.ten_min  = 0;
	tm_a.min      = 0;
	tm_a.ten_sec  = 0;
	tm_a.sec      = 1;
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
	
	// time = Wed 17:59:56.0
	tm.dow      = 3;
	tm.ten_hr   = 1;
	tm.hr       = 7;
	tm.ten_min  = 5;
	tm.min      = 9;
	tm.ten_sec  = 5;
	tm.sec      = 6;
	tm.sos      = 0;
	
	result = rtc_sub_alarm_test(SOCLE_RTC_TALRM_CH, 0);
	
	return result;
}

extern int
rtc_day_of_week_test(int autotest)
{
	int result = 0;
	
	// Set alarm date and alarm time
	// alarm date = C21 06/12/21
	dt_a.ten_cent = 2;
	dt_a.cent     = 1;
	dt_a.ten_yr   = 0;
	dt_a.yr       = 6;
	dt_a.ten_mth  = 1;
	dt_a.mth      = 2;
	dt_a.ten_day  = 2;
	dt_a.day      = 1;
	
	// alarm time = Thu 00:00:01.1
	tm_a.dow      = 4;
	tm_a.ten_hr   = 0;
	tm_a.hr       = 0;
	tm_a.ten_min  = 0;
	tm_a.min      = 0;
	tm_a.ten_sec  = 0;
	tm_a.sec      = 1;
	tm_a.sos      = 1;
	
	// Set date and time
	// date = C21 06/12/20
	dt.ten_cent = 2;
	dt.cent     = 1;
	dt.ten_yr   = 0;
	dt.yr       = 6;
	dt.ten_mth  = 1;
	dt.mth      = 2;
	dt.ten_day  = 2;
	dt.day      = 0;
	
	// time = Wed 23:59:56.0
	tm.dow      = 3;
	tm.ten_hr   = 2;
	tm.hr       = 3;
	tm.ten_min  = 5;
	tm.min      = 9;
	tm.ten_sec  = 5;
	tm.sec      = 6;
	tm.sos      = 0;
	
	result = rtc_sub_alarm_test(SOCLE_RTC_TALRM_CDOW, 0);
	
	return result;
}

extern int
rtc_day_test(int autotest)
{
	int result = 0;
	
	// Set alarm date and alarm time
	// alarm date = C21 06/12/21
	dt_a.ten_cent = 2;
	dt_a.cent     = 1;
	dt_a.ten_yr   = 0;
	dt_a.yr       = 6;
	dt_a.ten_mth  = 1;
	dt_a.mth      = 2;
	dt_a.ten_day  = 2;
	dt_a.day      = 1;
	
	// alarm time = Thu 00:00:01.1
	tm_a.dow      = 4;
	tm_a.ten_hr   = 0;
	tm_a.hr       = 0;
	tm_a.ten_min  = 0;
	tm_a.min      = 0;
	tm_a.ten_sec  = 0;
	tm_a.sec      = 1;
	tm_a.sos      = 1;
	
	// Set date and time
	// date = C21 06/12/20
	dt.ten_cent = 2;
	dt.cent     = 1;
	dt.ten_yr   = 0;
	dt.yr       = 6;
	dt.ten_mth  = 1;
	dt.mth      = 2;
	dt.ten_day  = 2;
	dt.day      = 0;
	
	// time = Wed 23:59:56.0
	tm.dow      = 3;
	tm.ten_hr   = 2;
	tm.hr       = 3;
	tm.ten_min  = 5;
	tm.min      = 9;
	tm.ten_sec  = 5;
	tm.sec      = 6;
	tm.sos      = 0;
	
	result = rtc_sub_alarm_test(0, SOCLE_RTC_DALRM_CD);
	
	return result;
}

extern int
rtc_month_test(int autotest)
{
	int result = 0;
	
	// Set alarm date and alarm time
	// alarm date = C21 07/01/01
	dt_a.ten_cent = 2;
	dt_a.cent     = 1;
	dt_a.ten_yr   = 0;
	dt_a.yr       = 7;
	dt_a.ten_mth  = 0;
	dt_a.mth      = 1;
	dt_a.ten_day  = 0;
	dt_a.day      = 1;
	
	// alarm time = Thu 00:00:01.1
	tm_a.dow      = 4;
	tm_a.ten_hr   = 0;
	tm_a.hr       = 0;
	tm_a.ten_min  = 0;
	tm_a.min      = 0;
	tm_a.ten_sec  = 0;
	tm_a.sec      = 1;
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
	
	// time = Wed 23:59:56.0
	tm.dow      = 3;
	tm.ten_hr   = 2;
	tm.hr       = 3;
	tm.ten_min  = 5;
	tm.min      = 9;
	tm.ten_sec  = 5;
	tm.sec      = 6;
	tm.sos      = 0;
	
	result = rtc_sub_alarm_test(0, SOCLE_RTC_DALRM_CM);
	
	return result;
}

extern int
rtc_year_test(int autotest)
{
	int result = 0;
	
	// Set alarm date and alarm time
	// alarm date = C21 07/01/01
	dt_a.ten_cent = 2;
	dt_a.cent     = 1;
	dt_a.ten_yr   = 0;
	dt_a.yr       = 7;
	dt_a.ten_mth  = 0;
	dt_a.mth      = 1;
	dt_a.ten_day  = 0;
	dt_a.day      = 1;
	
	// alarm time = Thu 00:00:01.1
	tm_a.dow      = 4;
	tm_a.ten_hr   = 0;
	tm_a.hr       = 0;
	tm_a.ten_min  = 0;
	tm_a.min      = 0;
	tm_a.ten_sec  = 0;
	tm_a.sec      = 1;
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
	
	// time = Wed 23:59:56.0
	tm.dow      = 3;
	tm.ten_hr   = 2;
	tm.hr       = 3;
	tm.ten_min  = 5;
	tm.min      = 9;
	tm.ten_sec  = 5;
	tm.sec      = 6;
	tm.sos      = 0;
	
	result = rtc_sub_alarm_test(0, SOCLE_RTC_DALRM_CY);
	
	return result;
}

extern int
rtc_cenury_test(int autotest)
{
	int result = 0;
	
	// Set alarm date and alarm time
	// alarm date = C22 00/01/01
	dt_a.ten_cent = 2;
	dt_a.cent     = 2;
	dt_a.ten_yr   = 0;
	dt_a.yr       = 0;
	dt_a.ten_mth  = 0;
	dt_a.mth      = 1;
	dt_a.ten_day  = 0;
	dt_a.day      = 1;
	
	// alarm time = Thu 00:00:01.1
	tm_a.dow      = 4;
	tm_a.ten_hr   = 0;
	tm_a.hr       = 0;
	tm_a.ten_min  = 0;
	tm_a.min      = 0;
	tm_a.ten_sec  = 0;
	tm_a.sec      = 1;
	tm_a.sos      = 1;
	
	// Set date and time
	// date = C21 99/12/31
	dt.ten_cent = 2;
	dt.cent     = 1;
	dt.ten_yr   = 9;
	dt.yr       = 9;
	dt.ten_mth  = 1;
	dt.mth      = 2;
	dt.ten_day  = 3;
	dt.day      = 1;
	
	// time = Wed 23:59:56.0
	tm.dow      = 3;
	tm.ten_hr   = 2;
	tm.hr       = 3;
	tm.ten_min  = 5;
	tm.min      = 9;
	tm.ten_sec  = 5;
	tm.sec      = 6;
	tm.sos      = 0;
	
	result = rtc_sub_alarm_test(0, SOCLE_RTC_DALRM_CC);
	
	return result;
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

	printf("Waiting for the alarm...\n");
	
	if (socle_wait_for_int(&hit, 10)) {
		printf("Timeout!!\n");
		result = -1;
	}

	// disable interrupt
	free_irq(RTC_INT);

	return result;
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


extern int
auto_poweroff_alarm_test(int autotest)
{
	int count_down_s = 5;
	
	rtc_reset();

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
	rtc_set_date_alarm(&dt_a,
			SOCLE_RTC_DALRM_CD	|
			SOCLE_RTC_DALRM_CM	|
			SOCLE_RTC_DALRM_CY	|
			SOCLE_RTC_DALRM_CC);

	// alarm time = Wed 17:58:1count_down_s.10
	tm_a.dow      = 3;
	tm_a.ten_hr   = 1;
	tm_a.hr       = 7;
	tm_a.ten_min  = 5;
	tm_a.min      = 8;
	tm_a.ten_sec  = 1;
	tm_a.sec      = count_down_s;
	tm_a.sos      = 10;
	rtc_set_time_alarm(&tm_a,
			SOCLE_RTC_TALRM_CSOS	|
			SOCLE_RTC_TALRM_CS	|
			SOCLE_RTC_TALRM_CM	|
			SOCLE_RTC_TALRM_CH	|
			SOCLE_RTC_TALRM_CDOW);

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
	rtc_set_date(&dt);
	
	// time = Wed 17:58:10.2
	tm.dow      = 3;
	tm.ten_hr   = 1;
	tm.hr       = 7;
	tm.ten_min  = 5;
	tm.min      = 8;
	tm.ten_sec  = 1;
	tm.sec      = 0;
	tm.sos      = 2;
	rtc_set_time(&tm);

	printf("Count down %d seconds...\n", count_down_s);

	printf("Sleeping...\n");
	EN_RTC_ALARM_POWER_ON();
	ENTER_RTC_POWER_OFF_MODE();
	
	return 0;
}


static void
rtc_isr_alarm_hit(void* pparam)
{
	int	*p_hit = (int*)pparam;

	RTC_ALRM_DIS();
	
	printf("RTC Alarm!!\n");
	*p_hit = 1;
}


extern int
rtc_set_time_test(int autotest)
{
	rtc_time_t time;
	
	printf("Please input the RTC time, ex: on Monday, 5:30:40:00 PM\n");
	printf("Set Time = 1:17:30:40:00, Your Time = ");
	scanf("%d:%1d%1d:%1d%1d:%1d%1d:%d", &time.dow, &time.ten_hr, &time.hr,	\
				&time.ten_min, &time.min, &time.ten_sec, &time.sec, &time.sos);
	
	PDEBUG("Input Time = %d:%d%d:%d%d:%d%d:%d\n", time.dow, time.ten_hr, time.hr,	\
				time.ten_min, time.min, time.ten_sec, time.sec, time.sos);

	rtc_set_time(&time);

	return 0;
}

extern int
rtc_set_date_test(int autotest)
{
	rtc_date_t date;

	printf("Please input the RTC date, ex: on 21st centry, 06/05/23\n");
	printf("Set Date = 21:06:05:23, Your Date = ");
	scanf("%1d%1d:%1d%1d:%1d%1d:%1d%1d", &date.ten_cent, &date.cent, &date.ten_yr, &date.yr,	\
				&date.ten_mth, &date.mth, &date.ten_day, &date.day);
	
	PDEBUG("Input Date = %d%d:%d%d:%d%d:%d%d\n", date.ten_cent, date.cent, date.ten_yr, date.yr,	\
				date.ten_mth, date.mth, date.ten_day, date.day);

	rtc_set_date(&date);
	
	return 0;
}

extern int
rtc_set_time_alarm_test(int autotest)
{
	u32_t flag = 0;
	rtc_time_t time;
	char input[1];
	
	printf("Please input the RTC alarm time, ex: on Monday, 5:30:40:00 PM\n");
	printf("Set Time = 1:17:30:40:00, Your Time = ");
	scanf("%d:%1d%1d:%1d%1d:%1d%1d:%d", &time.dow, &time.ten_hr, &time.hr,	\
				&time.ten_min, &time.min, &time.ten_sec, &time.sec, &time.sos);
	
	PDEBUG("Input Time = %d:%d%d:%d%d:%d%d:%d\n", time.dow, time.ten_hr, time.hr,	\
				time.ten_min, time.min, time.ten_sec, time.sec, time.sos);
	
	printf("Alarm Time on Sixteen of Second? (y/n)... ");
	input[0] = getchar();			
	if (('y' == input[0]) || ('Y' == input[0]))
		flag |= SOCLE_RTC_TALRM_CSOS;

	printf("Alarm Time on Second? (y/n)... ");
	input[0] = getchar();
	if (('y' == input[0]) || ('Y' == input[0]))
		flag |= SOCLE_RTC_TALRM_CS;

	printf("Alarm Time on Minute? (y/n)... ");
	input[0] = getchar();
	if (('y' == input[0]) || ('Y' == input[0]))
		flag |= SOCLE_RTC_TALRM_CM;

	printf("Alarm Time on Hour? (y/n)... ");
	input[0] = getchar();
	if (('y' == input[0]) || ('Y' == input[0]))
		flag |= SOCLE_RTC_TALRM_CH;

	printf("Alarm Time on Day of Week? (y/n)... ");
	input[0] = getchar();
	if (('y' == input[0]) || ('Y' == input[0]))
		flag |= SOCLE_RTC_TALRM_CDOW;

	rtc_set_time_alarm(&time, flag);

	return 0;
}

extern int
rtc_set_date_alarm_test(int autotest)
{
	u32_t flag = 0;
	rtc_date_t date;
	char input[1];
	
	printf("Please input the RTC alarm date, ex: on 21st centry, 06/05/23\n");
	printf("Set Date = 21:06:05:23, Your Date = ");
	scanf("%1d%1d:%1d%1d:%1d%1d:%1d%1d", &date.ten_cent, &date.cent, &date.ten_yr, &date.yr,	\
				&date.ten_mth, &date.mth, &date.ten_day, &date.day);
	
	PDEBUG("Input Date = %d%d:%d%d:%d%d:%d%d\n", date.ten_cent, date.cent, date.ten_yr, date.yr,	\
				date.ten_mth, date.mth, date.ten_day, date.day);
	
	printf("Alarm Date on Day? (y/n)... ");
	input[0] = getchar();
	if (('y' == input[0]) || ('Y' == input[0]))
		flag |= SOCLE_RTC_DALRM_CD;

	printf("Alarm Date on Month? (y/n)... ");
	input[0] = getchar();
	if (('y' == input[0]) || ('Y' == input[0]))
		flag |= SOCLE_RTC_DALRM_CM;

	printf("Alarm Date on Year? (y/n)... ");
	input[0] = getchar();
	if (('y' == input[0]) || ('Y' == input[0]))
		flag |= SOCLE_RTC_DALRM_CY;

	printf("Alarm Date on Century? (y/n)... ");
	input[0] = getchar();
	if (('y' == input[0]) || ('Y' == input[0]))
		flag |= SOCLE_RTC_DALRM_CC;

	rtc_set_date_alarm(&date, flag);

	return 0;
}

extern int
rtc_get_time_test(int autotest)
{
	rtc_time_t time;
	
	rtc_get_time(&time);
	
	printf("RTC Time = W%d %d%d:%d%d:%d%d:%d\n", time.dow, time.ten_hr, time.hr,	\
				time.ten_min, time.min, time.ten_sec, time.sec, time.sos);

	return 0;
}

extern int
rtc_get_date_test(int autotest)
{
	rtc_date_t date;
	
	rtc_get_date(&date);
	
	printf("RTC Date = C%d%d %d%d/%d%d/%d%d\n", date.ten_cent, date.cent, date.ten_yr, date.yr,	\
				date.ten_mth, date.mth, date.ten_day, date.day);

	return 0;
}

extern int
rtc_get_time_alarm_test(int autotest)
{
	u32_t flag = 0;
	rtc_time_t time;
	
	rtc_get_time_alarm(&time, &flag);
	
	printf("Alarm Time = W%d %d%d:%d%d:%d%d:%d\n", time.dow, time.ten_hr, time.hr,	\
				time.ten_min, time.min, time.ten_sec, time.sec, time.sos);
	PDEBUG("Alarm Time Flag = 0x%08x\n", flag);

	printf("Alarm Time on: ");
	if (flag & SOCLE_RTC_TALRM_CSOS)
		printf("\"Sixteen of Second\" ");
	if (flag & SOCLE_RTC_TALRM_CS)
		printf("\"Second\" ");
	if (flag & SOCLE_RTC_TALRM_CM)
		printf("\"Minute\" ");
	if (flag & SOCLE_RTC_TALRM_CH)
		printf("\"Hour\" ");
	if (flag & SOCLE_RTC_TALRM_CDOW)
		printf("\"Day of Week\"");

	printf("\n");
	return 0;
}

extern int
rtc_get_date_alarm_test(int autotest)
{
	u32_t flag = 0;
	rtc_date_t date;
	
	rtc_get_date_alarm(&date, &flag);

	printf("Alarm Date = C%d%d %d%d/%d%d/%d%d\n", date.ten_cent, date.cent, date.ten_yr, date.yr,	\
				date.ten_mth, date.mth, date.ten_day, date.day);
	PDEBUG("Alarm Date Flag = 0x%08x\n", flag);

	printf("Alarm Date on: ");
	if (flag & SOCLE_RTC_DALRM_CD)
		printf("\"Day\" ");
	if (flag & SOCLE_RTC_DALRM_CM)
		printf("\"Month\" ");
	if (flag & SOCLE_RTC_DALRM_CY)
		printf("\"Year\" ");
	if (flag & SOCLE_RTC_DALRM_CC)
		printf("\"Century\"");

	printf("\n");
	return 0;
}

extern int
rtc_wait_for_alarm_test(int autotest)
{
	char input[1];
	volatile int hit;
	
	printf("Reminder: You must set up the alarm first!\n");
	printf("Are you sure? (y/n)... ");
	input[0] = getchar();
	
	if (('y' == input[0]) || ('Y' == input[0])) {
		hit = 0;
		
		// enable interrupt
		request_irq(RTC_INT, rtc_isr_alarm_hit, (void *)&hit);

		printf("Waiting for the alarm...\n");
		while (!hit) ;

		// disable interrupt
		free_irq(RTC_INT);
	}

	return 0;
}

extern int
rtc_enter_poweroff_mode_test(int autotest)
{
	char input[1];
	
	printf("Reminder: You must set up the wakeup alarm first!\n");
	printf("Are you sure? (y/n)... ");
	input[0] = getchar();
	
	if (('y' == input[0]) || ('Y' == input[0])) {
		printf("Sleeping...\n");
		EN_RTC_ALARM_POWER_ON();
		ENTER_RTC_POWER_OFF_MODE();
	}

	return 0;
}



//////////////////////////////////////////
//
//    rtc controller lib
//
//////////////////////////////////////////

// rtc_u32_to_time(): given u32 time, fill the rtc_time struct
// IN : given temp
// OUT : result time struct
static void
rtc_u32_to_time(struct rtc_time* tm, const u32_t temp)
{
	tm->sos      = (temp >> SOCLE_RTC_TIME_SOS) & BIT_MASK(4);
	tm->sec      = (temp >> SOCLE_RTC_TIME_S)   & BIT_MASK(4);
	tm->ten_sec  = (temp >> SOCLE_RTC_TIME_TS)  & BIT_MASK(3);
	tm->min      = (temp >> SOCLE_RTC_TIME_M)   & BIT_MASK(4);
	tm->ten_min  = (temp >> SOCLE_RTC_TIME_TM)  & BIT_MASK(3);
	tm->hr       = (temp >> SOCLE_RTC_TIME_H)   & BIT_MASK(4);
	tm->ten_hr   = (temp >> SOCLE_RTC_TIME_TH)  & BIT_MASK(2);
	tm->dow      = (temp >> SOCLE_RTC_TIME_DOW) & BIT_MASK(3);
}

// rtc_u32_to_date() : given u32 date, fill the rtc_date struct
// IN : given temp
// OUT : result date struct
static void
rtc_u32_to_date(struct rtc_date* dt, const data_t temp)
{
	dt->day      = (temp >> SOCLE_RTC_DATE_D)   & BIT_MASK(4);
	dt->ten_day  = (temp >> SOCLE_RTC_DATE_TD)  & BIT_MASK(2);
	dt->mth      = (temp >> SOCLE_RTC_DATE_M)   & BIT_MASK(4);
	dt->ten_mth  = (temp >> SOCLE_RTC_DATE_TM)  & BIT_MASK(1);
	dt->yr       = (temp >> SOCLE_RTC_DATE_Y)   & BIT_MASK(4);
	dt->ten_yr   = (temp >> SOCLE_RTC_DATE_TY)  & BIT_MASK(4);
	dt->cent     = (temp >> SOCLE_RTC_DATE_C)   & BIT_MASK(4);
	dt->ten_cent = (temp >> SOCLE_RTC_DATE_TC)  & BIT_MASK(4);
}

// rtc_time_to_u32() : given RtcTime struct , fill back the data
// IN : given tm
// return u32 time
static u32_t
rtc_time_to_u32(const struct rtc_time* tm)
{
	u32_t	temp = 0;

	temp |= (tm->dow         << SOCLE_RTC_TIME_DOW);
	temp |= (tm->ten_hr      << SOCLE_RTC_TIME_TH );
	temp |= (tm->hr          << SOCLE_RTC_TIME_H  );
	temp |= (tm->ten_min     << SOCLE_RTC_TIME_TM );
	temp |= (tm->min         << SOCLE_RTC_TIME_M  );
	temp |= (tm->ten_sec     << SOCLE_RTC_TIME_TS );
	temp |= (tm->sec         << SOCLE_RTC_TIME_S  );
	temp |= (tm->sos         << SOCLE_RTC_TIME_SOS);

	return temp;
}


// rtc_date_to_u32() : given RtcTime struct , fill back the data
// IN : given tm
// return u32 time
static u32_t
rtc_date_to_u32(const struct rtc_date* dt)
{
	u32_t	temp = 0;

	temp |= (dt->day      << SOCLE_RTC_DATE_D );
	temp |= (dt->ten_day  << SOCLE_RTC_DATE_TD);
	temp |= (dt->mth      << SOCLE_RTC_DATE_M );
	temp |= (dt->ten_mth  << SOCLE_RTC_DATE_TM);
	temp |= (dt->yr       << SOCLE_RTC_DATE_Y );
	temp |= (dt->ten_yr   << SOCLE_RTC_DATE_TY);
	temp |= (dt->cent     << SOCLE_RTC_DATE_C );
	temp |= (dt->ten_cent << SOCLE_RTC_DATE_TC);

	return temp;
}

// IN: tm
extern void
rtc_set_time(const struct rtc_time* tm)
{
	u32_t temp = rtc_time_to_u32(tm);
	writew(temp, SOCLE_RTC_TIME);
}

// IN: dt
extern void
rtc_set_date(const struct rtc_date* dt)
{
	u32_t temp = rtc_date_to_u32(dt);
	writew(temp, SOCLE_RTC_DATE);
}

// IN: tm,flag
extern void
rtc_set_time_alarm(const struct rtc_time* tm, const u32_t flag)
{
	u32_t temp = rtc_time_to_u32(tm);
	writew(temp | flag, SOCLE_RTC_TALRM);
}

// IN: dt,flag
extern void
rtc_set_date_alarm(const struct rtc_date* dt, const u32_t flag)
{
	u32_t temp = rtc_date_to_u32(dt);
	writew(temp | flag, SOCLE_RTC_DALRM);
}

// IN/OUT: tm
extern void
rtc_get_time(struct rtc_time* tm)
{
	u32_t temp = readw(SOCLE_RTC_TIME);

	return rtc_u32_to_time(tm, temp);
}

// IN/OUT: dt
extern void
rtc_get_date(struct rtc_date* dt)
{
	u32_t temp = readw(SOCLE_RTC_DATE);
	rtc_u32_to_date(dt, temp);
}


// IN/OUT: tm, flag
extern void
rtc_get_time_alarm(struct rtc_time* tm, u32_t* flag)
{
	u32_t temp = readw(SOCLE_RTC_TALRM);
	rtc_u32_to_time(tm, temp);
	*flag = temp & (SOCLE_RTC_TALRM_CSOS	|
			SOCLE_RTC_TALRM_CS	|
			SOCLE_RTC_TALRM_CM	|
			SOCLE_RTC_TALRM_CH	|
			SOCLE_RTC_TALRM_CDOW);
}

// IN/OUT: dt, flag
extern void
rtc_get_date_alarm(struct rtc_date* dt, u32_t* flag)
{
	u32_t temp = readw(SOCLE_RTC_DALRM);
	rtc_u32_to_date(dt, temp);
	*flag = temp & (SOCLE_RTC_DALRM_CD	|
			SOCLE_RTC_DALRM_CM	|
			SOCLE_RTC_DALRM_CY	|
			SOCLE_RTC_DALRM_CC);
}






