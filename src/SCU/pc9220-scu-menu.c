#include <test_item.h>

extern int socle_sleep_mode_test(int autotest);
extern int socle_stop_mode_test(int autotest);

struct test_item socle_mode_main_test_items[] = {
	{
		"Sleep Mode Test",
		socle_sleep_mode_test,
		1,
		1
	},
	{
		"Stop Mode Test",
		socle_stop_mode_test,
		0,
		1
	}
};

struct test_item_container socle_mode_main_container = {
     .menu_name = "Socle Sleep and Stop Mode Test Main Menu",
     .shell_name = "Mode",
     .items = socle_mode_main_test_items,
     .test_item_size = sizeof(socle_mode_main_test_items)
};


extern int socle_sleep_with_irq_test(int autotest);
extern int socle_sleep_with_fiq_test(int autotest);

struct test_item socle_sleep_main_test_items[] = {
	{
		"Wake Up with IRQ",
		socle_sleep_with_irq_test,
		1,
		1
	},
	{
		"Wake Up with FIQ",
		socle_sleep_with_fiq_test,
		0,
		1
	}
};

struct test_item_container socle_sleep_main_container = {
     .menu_name = "Socle Sleep Mode Test Main Menu",
     .shell_name = "Sleep",
     .items = socle_sleep_main_test_items,
     .test_item_size = sizeof(socle_sleep_main_test_items)
};


extern int socle_stop_with_irq_test(int autotest);
extern int socle_stop_with_fiq_test(int autotest);

struct test_item socle_stop_main_test_items[] = {
	{
		"Wake Up with IRQ",
		socle_stop_with_irq_test,
		1,
		1
	},
	{
		"Wake Up with FIQ",
		socle_stop_with_fiq_test,
		0,
		1
	}
};

struct test_item_container socle_stop_main_container = {
     .menu_name = "Socle Stop Mode Test Main Menu",
     .shell_name = "Stop",
     .items = socle_stop_main_test_items,
     .test_item_size = sizeof(socle_stop_main_test_items)
};
