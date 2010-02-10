#include <test_item.h>
#include "dependency.h"

extern int socle_gpio_onboard_test(int autotest);
extern int socle_gpio_fpga_test(int autotest);

struct test_item socle_gpio_main_test_items[] = {
	{
		"General Test",
		socle_gpio_onboard_test,
		1,
		1
	},
	{
		"FPGA GPIO Test(APB)",
		socle_gpio_fpga_test,
		1,
		SCDK_FPGA_TEST
	}
};

struct test_item_container socle_gpio_main_container = {
     .menu_name = "Socle GPIO Test Main Menu",
     .shell_name = "GPIO",
     .items = socle_gpio_main_test_items,
     .test_item_size = sizeof(socle_gpio_main_test_items)
};

extern int socle_gpio_test_mode(int autotest);
extern int socle_gpio_normal_mode(int autotest);

struct test_item socle_gpio_mode_test_items[] = {
	{
		"Test Mode Test",
		socle_gpio_test_mode,
		1,
		1
	},
	{
		"Normal Mode Test",
		socle_gpio_normal_mode,
		1,
		1
	}
};

struct test_item_container socle_gpio_mode_container = {
     .menu_name = "Socle GPIO Test Mode Menu",
     .shell_name = "GPIO",
     .items = socle_gpio_mode_test_items,
     .test_item_size = sizeof(socle_gpio_mode_test_items)
};


extern int socle_gpio_flash_leds(int autotest);
extern int socle_gpio_kpd_test(int autotest);

struct test_item socle_gpio_normal_test_items[] = {
	{
		"Flash LEDs Test",
		socle_gpio_flash_leds,
		1,
		SOCLE_GPIO_FLASH_LED_TEST
	},
	{
		"KeyPad Test",
		socle_gpio_kpd_test,
		0,
		SOCLE_GPIO_KPD_TEST
	},
};

struct test_item_container socle_gpio_normal_container = {
     .menu_name = "Socle GPIO Normal Test Main Menu",
     .shell_name = "GPIO",
     .items = socle_gpio_normal_test_items,
     .test_item_size = sizeof(socle_gpio_normal_test_items)
};


extern int socle_gpio_without_int(int autotest);
extern int socle_gpio_with_int(int autotest);

struct test_item socle_gpio_int_test_items[] = {
	{
		"Without Interrupt Test",
		socle_gpio_without_int,
		1,
		1
	},
	{
		"With Interrupt Test",
		socle_gpio_with_int,
		1,
		SOCLE_GPIO_SUPPORT_INT
	}
};

struct test_item_container socle_gpio_int_main_container = {
     .menu_name = "Socle GPIO Interrupt Test Main Menu",
     .shell_name = "GPIO",
     .items = socle_gpio_int_test_items,
     .test_item_size = sizeof(socle_gpio_int_test_items)
};



extern int socle_gpio_port_a_to_d(int autotest);
extern int socle_gpio_port_e_to_h(int autotest);
extern int socle_gpio_port_i_to_l(int autotest);
extern int socle_gpio_port_m_to_p(int autotest);

struct test_item socle_gpio_rw_test_items[] = {
	{
		"Port A ~ D",
		socle_gpio_port_a_to_d,
		1,
		1
	},
	{
		"Port E ~ H",
		socle_gpio_port_e_to_h,
		1,
		SOCLE_GPIO_GP1
	},
	{
		"Port I ~ L",
		socle_gpio_port_i_to_l,
		1,
		SOCLE_GPIO_GP2
	},
	{
		"Port M ~ P",
		socle_gpio_port_m_to_p,
		1,
		SOCLE_GPIO_GP3
	},
};

struct test_item_container socle_gpio_rw_test_container = {
     .menu_name = "Socle GPIO Read Write Test Main Menu",
     .shell_name = "GPIO",
     .items = socle_gpio_rw_test_items,
     .test_item_size = sizeof(socle_gpio_rw_test_items)
};



extern int socle_gpio_level_sense(int autotest);
extern int socle_gpio_edge_sense(int autotest);

struct test_item socle_gpio_sense_test_items[] = {
	{
		"Level Sensitive",
		socle_gpio_level_sense,
		1,
		1
	},
	{
		"Edge Sensitive",
		socle_gpio_edge_sense,
		1,
		1
	}
};

struct test_item_container socle_gpio_sense_container = {
     .menu_name = "Socle GPIO Sense Test Main Menu",
     .shell_name = "GPIO",
     .items = socle_gpio_sense_test_items,
     .test_item_size = sizeof(socle_gpio_sense_test_items)
};


extern int socle_gpio_lo_event(int autotest);
extern int socle_gpio_hi_event(int autotest);

struct test_item socle_gpio_event_test_items[] = {
	{
		"Falling Edge or Low Level",
		socle_gpio_lo_event,
		1,
		1
	},
	{
		"Rising Edge or High Level",
		socle_gpio_hi_event,
		1,
		1
	}
};

struct test_item_container socle_gpio_event_container = {
     .menu_name = "Socle GPIO Event Test Main Menu",
     .shell_name = "GPIO",
     .items = socle_gpio_event_test_items,
     .test_item_size = sizeof(socle_gpio_event_test_items)
};


extern int socle_gpio_single_edge(int autotest);
extern int socle_gpio_both_edge(int autotest);

struct test_item socle_gpio_edge_test_items[] = {
	{
		"Single Edge",
		socle_gpio_single_edge,
		1,
		1
	},
	{
		"Both Edge",
		socle_gpio_both_edge,
		1,
		1
	}
};

struct test_item_container socle_gpio_edge_container = {
     .menu_name = "Socle GPIO Event Test Main Menu",
     .shell_name = "GPIO",
     .items = socle_gpio_edge_test_items,
     .test_item_size = sizeof(socle_gpio_edge_test_items)
};


