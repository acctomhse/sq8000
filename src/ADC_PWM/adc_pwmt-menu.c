#include <test_item.h>
#include "dependency.h"

extern int socle_adc_test(int autotest);
extern int socle_pwm_test(int autotest);

struct test_item adc_pwm_main_test_items[] = {
	{
		"ADC Test",
		socle_adc_test,
		1,
		SOCLE_ADC_TEST
	},
	{
		"PWMT Test",
		socle_pwm_test,
		1,
		1
	}
};

struct test_item_container adc_pwm_main_container = {
     .menu_name = "ADC and PWMT Test Main Menu",
     .shell_name = "ADC_PWMT",
     .items = adc_pwm_main_test_items,
     .test_item_size = sizeof(adc_pwm_main_test_items)
};


extern int socle_adc_conv_w_poll(int autotest);
extern int socle_adc_conv_w_int(int autotest);

struct test_item adc_converse_select_test_items[] = {
	{
		"ADC Conversion with Poll Mode",
		socle_adc_conv_w_poll,
		1,
		1
	},
	{
		"ADC Conversion with Interrupt Mode",
		socle_adc_conv_w_int,
		1,
		1
	}
};

struct test_item_container adc_converse_select_container = {
     .menu_name = "ADC Conversion Select Main Menu",
     .shell_name = "Conv",
     .items = adc_converse_select_test_items,
     .test_item_size = sizeof(adc_converse_select_test_items)
};


extern int socle_adc_ch_test_w_pwmt(int autotest);
extern int socle_adc_ch_test_w_board(int autotest);

struct test_item adc_main_test_items[] = {
	{
		"ADC Channel Test with PWMT",
		socle_adc_ch_test_w_pwmt,
		1,
		1
	},
	{
		"ADC Channel Test on Board",
		socle_adc_ch_test_w_board,
		0,
		1
	}
};

struct test_item_container adc_main_container = {
     .menu_name = "ADC Test Main Menu",
     .shell_name = "ADC",
     .items = adc_main_test_items,
     .test_item_size = sizeof(adc_main_test_items)
};


extern int socle_pwmt_timer_mode(int autotest);
extern int socle_pwmt_capture_mode(int autotest);

struct test_item pwmt_main_test_items[] = {
	{
		"Timer Mode",
		socle_pwmt_timer_mode,
		1,
		1
	},
	{
		"Capture Mode",
		socle_pwmt_capture_mode,
		1,
		1
	}
};

struct test_item_container pwmt_main_container = {
     .menu_name = "PWM Timer Test Main Menu",
     .shell_name = "PWMT",
     .items = pwmt_main_test_items,
     .test_item_size = sizeof(pwmt_main_test_items)
};


extern int socle_pwmt_timer_0(int autotest);
extern int socle_pwmt_timer_1(int autotest);
extern int socle_pwmt_timer_2(int autotest);
extern int socle_pwmt_timer_3(int autotest);

struct test_item pwmt_test_items[] = {
	{
		"PWM Timer 0",
		socle_pwmt_timer_0,
		1,
		SOCLE_PWMT_0_ENABLE
	},
	{
		"PWM Timer 1",
		socle_pwmt_timer_1,
		1,
		SOCLE_PWMT_1_ENABLE
	},
	{
		"PWM Timer 2",
		socle_pwmt_timer_0,
		1,
		SOCLE_PWMT_2_ENABLE
	},
	{
		"PWM Timer 3",
		socle_pwmt_timer_0,
		1,
		SOCLE_PWMT_3_ENABLE
	},
};

struct test_item_container pwmt_test_container = {
     .menu_name = "PWM Timer Test Menu",
     .shell_name = "PWMT",
     .items = pwmt_test_items,
     .test_item_size = sizeof(pwmt_test_items)
};


extern int socle_pwmt_periodical_counter_mode_test(int autotest);
extern int socle_pwmt_single_counter_mode_test(int autotest);

struct test_item pwmt_counter_mode_test_items[] = {
	{
		"PWM Timer Periodical Counter Mode",
		socle_pwmt_periodical_counter_mode_test,
		1,
		1
	},
	{
		"PWM Timer Single Counter Mode",
		socle_pwmt_single_counter_mode_test,
		1,
		1
	}
};

struct test_item_container pwmt_counter_mode_container = {
     .menu_name = "PWM Timer Counter Mode Test Menu",
     .shell_name = "Counter",
     .items = pwmt_counter_mode_test_items,
     .test_item_size = sizeof(pwmt_counter_mode_test_items)
};

extern int socle_pwmt_capture_mode_irq (int autotest);
extern int socle_pwmt_capture_mode_pull (int autotest);

struct test_item pwmt_capture_mode_test_items[] = {
	{
		"irq mode",
		socle_pwmt_capture_mode_irq,
		0,
		0
	},
	{
		"pulling mode",
		socle_pwmt_capture_mode_pull,	
		1,
		1,
	}
};

struct test_item_container pwmt_capture_mode_container = {
     .menu_name = "PWM Timer Counter Mode Test Menu",
     .shell_name = "Capture mode",
     .items = pwmt_capture_mode_test_items,
     .test_item_size = sizeof(pwmt_capture_mode_test_items)
};

extern int socle_pwmt_capture_mode_loopback(int autotest);
extern int socle_pwmt_capture_mode_current_sense(int autotest);

struct test_item pwmt_capture_mode_item_test_items[] = {
	{
		"loopback",
		socle_pwmt_capture_mode_loopback,
		1,
		SOCLE_PWMT_CAPTURE_LOOP,
	},
	{
		"current sense",
		socle_pwmt_capture_mode_current_sense,	
		0,
		SOCLE_PWMT_CURRENT_SENSE_TEST,
	}
};

struct test_item_container pwmt_capture_mode_item_container = {
     .menu_name = "PWM Timer Capture Mode item Test Menu",
     .shell_name = "Capture item",
     .items = pwmt_capture_mode_item_test_items,
     .test_item_size = sizeof(pwmt_capture_mode_item_test_items)
};

extern int socle_pwmt_capture_mode_loop_duty_75(int autotest);
extern int socle_pwmt_capture_mode_loop_duty_50(int autotest);
extern int socle_pwmt_capture_mode_loop_duty_25(int autotest);

struct test_item pwmt_capture_mode_loop_test_items[] = {
	{
		"Loopback duty 75",
		socle_pwmt_capture_mode_loop_duty_75,
		1,
		1
	},
	{
		"cLoopback duty 50",
		socle_pwmt_capture_mode_loop_duty_50,	
		1,
		1
	},
	{
		"cLoopback duty 25",
		socle_pwmt_capture_mode_loop_duty_25,	
		1,
		1
	}
};

struct test_item_container pwmt_capture_mode_loop_container = {
     .menu_name = "PWM Timer Capture Mode loop Test Menu",
     .shell_name = "Capture Loop",
     .items = pwmt_capture_mode_loop_test_items,
     .test_item_size = sizeof(pwmt_capture_mode_loop_test_items)
};

extern int socle_pwmt_capture_mode_from_0_to_1(int autotest);
extern int socle_pwmt_capture_mode_from_1_to_0(int autotest);

struct test_item pwmt_capture_mode_loop_duty_test_items[] = {
	{
		"PWMT0 tx, PWM1 rx",
		socle_pwmt_capture_mode_from_0_to_1,
		1,
		1
	},
	{
		"PWMT1 tx, PWM0 rx",
		socle_pwmt_capture_mode_from_1_to_0,
		1,
		1
	}
};

struct test_item_container pwmt_capture_mode_loop_duty_container = {
     .menu_name = "PWM Timer Capture Mode loop duty Test Menu",
     .shell_name = "Capture duty",
     .items = pwmt_capture_mode_loop_duty_test_items,
     .test_item_size = sizeof(pwmt_capture_mode_loop_duty_test_items)
};


