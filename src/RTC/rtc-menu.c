#include <test_item.h>

extern int auto_poweroff_alarm_test(int autotest);
extern int rtc_alarm_test(int autotest);
extern int rtc_set_time_test(int autotest);
extern int rtc_set_date_test(int autotest);
extern int rtc_set_time_alarm_test(int autotest);
extern int rtc_set_date_alarm_test(int autotest);
extern int rtc_get_time_test(int autotest);
extern int rtc_get_date_test(int autotest);
extern int rtc_get_time_alarm_test(int autotest);
extern int rtc_get_date_alarm_test(int autotest);
extern int rtc_wait_for_alarm_test(int autotest);
extern int rtc_enter_poweroff_mode_test(int autotest);

struct test_item rtc_main_test_items[] = {
	{
		"RTC Alarm Test",
		rtc_alarm_test,
		1,
		1
	},
#ifdef CONFIG_LDK5
	{
		"Auto Poweroff Alarm Test",
		auto_poweroff_alarm_test,
		0,
		1
	},
#endif
	{
		"Set RTC Time",
		rtc_set_time_test,
		0,
		1
	},
	{
		"Set RTC Date",
		rtc_set_date_test,
		0,
		1
	},
	{
		"Set RTC Alarm Time",
		rtc_set_time_alarm_test,
		0,
		1
	},
	{
		"Set RTC Alarm Date",
		rtc_set_date_alarm_test,
		0,
		1
	},
	{
		"Get RTC Time",
		rtc_get_time_test,
		0,
		1
	},
	{
		"Get RTC Date",
		rtc_get_date_test,
		0,
		1
	},
	{
		"Get RTC Alarm Time",
		rtc_get_time_alarm_test,
		0,
		1
	},
	{
		"Get RTC Alarm Date",
		rtc_get_date_alarm_test,
		0,
		1
	},
	{
		"Wait for the Alarm",
		rtc_wait_for_alarm_test,
		0,
		1
	},
#ifdef CONFIG_LDK5
	{
		"Enter RTC Power Off Mode",
		rtc_enter_poweroff_mode_test,
		0,
		1
	}
#endif
};

struct test_item_container rtc_main_container = {
     .menu_name = "RTC Test Main Menu",
     .shell_name = "RTC",
     .items = rtc_main_test_items,
     .test_item_size = sizeof(rtc_main_test_items)
};


extern int rtc_all_alarm_test(int autotest);
extern int rtc_sixteen_of_second_test(int autotest);
extern int rtc_second_test(int autotest);
extern int rtc_minute_test(int autotest);
extern int rtc_hour_test(int autotest);
extern int rtc_day_of_week_test(int autotest);
extern int rtc_day_test(int autotest);
extern int rtc_month_test(int autotest);
extern int rtc_year_test(int autotest);
extern int rtc_cenury_test(int autotest);

struct test_item rtc_alarm_test_items[] = {
	{
		"RTC Sixteen of Second Test",
		rtc_sixteen_of_second_test,
		1,
		1
	},
	{
		"RTC Second Test",
		rtc_second_test,
		1,
		1
	},
	{
		"RTC Minute Test",
		rtc_minute_test,
		1,
		1
	},
	{
		"RTC Hour Test",
		rtc_hour_test,
		1,
		1
	},
	{
		"RTC Day of Week Test",
		rtc_day_of_week_test,
		1,
		1
	},
	{
		"RTC Day Test",
		rtc_day_test,
		1,
		1
	},
	{
		"RTC Month Test",
		rtc_month_test,
		1,
		1
	},
	{
		"RTC Year Test",
		rtc_year_test,
		1,
		1
	},
	{
		"RTC Century Test",
		rtc_cenury_test,
		1,
		1
	},
	{
		"RTC All Alarm Test",
		rtc_all_alarm_test,
		1,
		1
	},
};

struct test_item_container rtc_alarm_test_container = {
     .menu_name = "RTC Alarm Test Menu",
     .shell_name = "Alarm",
     .items = rtc_alarm_test_items,
     .test_item_size = sizeof(rtc_alarm_test_items)
};
