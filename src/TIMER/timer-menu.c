#include <test_item.h>
#include "dependency.h"

extern int socle_timer_mode_0_test(int autotest);
extern int socle_timer_mode_1(int autotest);
extern int socle_timer_mode_2(int autotest);

struct test_item socle_timer_main_test_items[] = {
	{"Mode 0",
	 socle_timer_mode_0_test,
	 1,
	 1},
	{"Mode 1",
	 socle_timer_mode_1,
	 1,
	 SOCLE_TIMER_CHAIN_MODE_1_TEST},
	{"Mode 2",
	 socle_timer_mode_2,
	 1,
	 SOCLE_TIMER_CHAIN_MODE_2_TEST},
};

struct test_item_container socle_timer_main_test_container = {
	.menu_name = "SOCLE Timer Test Main Menu",
	.shell_name = "socle_timer",
	.items = socle_timer_main_test_items,
	.test_item_size = sizeof(socle_timer_main_test_items),
};

extern int socle_timer_mode_0_timer_0_test(int autotest);
extern int socle_timer_mode_0_timer_1_test(int autotest);
extern int socle_timer_mode_0_timer_2_test(int autotest);

struct test_item socle_timer_mode_0_timer_test_items[] = {
	{"Timer 0",
	 socle_timer_mode_0_timer_0_test,
	 1,
	 1},
	{"Timer 1",
	 socle_timer_mode_0_timer_1_test,
	 1,
	 1},
	{"Timer 2",
	 socle_timer_mode_0_timer_2_test,
	 1,
	 1},
};

struct test_item_container socle_timer_mode_0_timer_test_container = {
	.menu_name = "SOCLE Timer Mode 0 Timer Test Menu",
	.shell_name = "socle_timer",
	.items = socle_timer_mode_0_timer_test_items,
	.test_item_size = sizeof(socle_timer_mode_0_timer_test_items),
};

extern int socle_timer_cnt_free_running(int autotest);
extern int socle_timer_cnt_periodical(int autotest);

struct test_item socle_timer_cnt_mode_test_items[] = {
	{"Free-Running",
	 socle_timer_cnt_free_running,
	 1,
	 1},
	{"Periodical",
	 socle_timer_cnt_periodical,
	 1,
	 1},
};

struct test_item_container socle_timer_cnt_mode_test_container = {
	.menu_name = "SOCLE Timer Counting Mode Test Menu",
	.shell_name = "socle_timer",
	.items = socle_timer_cnt_mode_test_items,
	.test_item_size = sizeof(socle_timer_cnt_mode_test_items),
};

