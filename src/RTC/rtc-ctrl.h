#ifndef _rtc_ctrl_h_included_
#define _rtc_ctrl_h_included_

#include "global.h"


//linux------------------------------
/*
struct rtc_time {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};
*/
//----------------------------------


typedef struct rtc_time {
  int dow;
  int ten_hr;
  int hr;
  int ten_min;
  int min;
  int ten_sec;
  int sec;
  int sos;
} rtc_time_t;

typedef struct rtc_date {
  int ten_cent;
  int cent;
  int ten_yr;
  int yr;
  int ten_mth;
  int mth;
  int ten_day;
  int day;
} rtc_date_t;


// IN: tm
extern void rtc_set_time(const struct rtc_time* tm);

// IN: dt
extern void rtc_set_date(const struct rtc_date* dt);

// IN: tm,flag
extern void rtc_set_time_alarm(const struct rtc_time* tm, const u32_t flag);

// IN: dt,flag
extern void rtc_set_date_alarm(const struct rtc_date* dt, const u32_t flag);


// IN/OUT: tm
extern void rtc_get_time(struct rtc_time* tm);

// IN/OUT: dt
extern void rtc_get_date(struct rtc_date* dt);

// IN/OUT: tm, flag
extern void rtc_get_time_alarm(struct rtc_time* tm, u32_t* flag);

// IN/OUT: dt, flag
extern void rtc_get_date_alarm(struct rtc_date* dt, u32_t* flag);


#endif // _rtc_ctrl_h_included_










