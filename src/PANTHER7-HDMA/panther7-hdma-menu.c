#include <test_item.h>

#ifdef CONFIG_SCDK
#define SCDK_FPGA_TEST 1
#else
#define SCDK_FPGA_TEST 0
#endif

extern int panther7_hdma_onboard_test(int autotest);
extern int panther7_hdma_fpga_test(int autotest);

struct test_item panther7_hdma_main_test_items[] = {
	{"General Test",
	 panther7_hdma_onboard_test,
	 1,
	 1},
	{"FPGA EXT Board Test(AHB)",
	 panther7_hdma_fpga_test,
	 1,
	 SCDK_FPGA_TEST}
};

struct test_item_container panther7_hdma_main_test_container = {
	.menu_name = "PANTHER7 HDMA Test Main Menu",
	.shell_name = "panther7 hdma",
	.items = panther7_hdma_main_test_items,
	.test_item_size = sizeof(panther7_hdma_main_test_items)
};

extern int panther7_hdma_channel_0_test(int autotest);
extern int panther7_hdma_channel_1_test(int autotest);

struct test_item panther7_hdma_ch_items[] = {
	{"Channel 0",
	 panther7_hdma_channel_0_test,
	 1,
	 1},
	{"Channel 1",
	 panther7_hdma_channel_1_test,
	 1,
	 1}
};

struct test_item_container panther7_hdma_ch_test_container = {
	.menu_name = "PANTHER7 HDMA Test Channel Menu",
	.shell_name = "panther7 hdma",
	.items = panther7_hdma_ch_items,
	.test_item_size = sizeof(panther7_hdma_ch_items)
};

extern int panther7_hdma_fly_disable(int autotest);
extern int panther7_hdma_fly_read(int autotest);
extern int panther7_hdma_fly_write(int autotest);

struct test_item panther7_hdma_fly_items[] = {
	{"Fly Disable",
	 panther7_hdma_fly_disable,
	 1,
	 1},
	{"Fly Read",
	 panther7_hdma_fly_read,
	 0,
	 1},
	{"Fly Write",
	 panther7_hdma_fly_write,
	 0,
	 1}


};

struct test_item_container panther7_hdma_fly_test_container = {
	.menu_name = "PANTHER7 HDMA On The Fly Menu",
	.shell_name = "panther7 hdma",
	.items = panther7_hdma_fly_items,
	.test_item_size = sizeof(panther7_hdma_fly_items)
};

extern int panther7_hdma_source_direction_fixed(int autotest);
extern int panther7_hdma_source_direction_increment(int autotest);

struct test_item panther7_hdma_src_dir_items[] = {
	{"Source INCR",
	 panther7_hdma_source_direction_increment,
	 1,
	 1},
	{"Source Fixed",
	 panther7_hdma_source_direction_fixed,
	 1,
	 1}
};

struct test_item_container panther7_hdma_src_dir_test_container = {
	.menu_name = "PANTHER7HDMA Source Direction Menu",
	.shell_name = "panther7 hdma",
	.items = panther7_hdma_src_dir_items,
	.test_item_size = sizeof(panther7_hdma_src_dir_items)
};

extern int panther7_hdma_destination_direction_fixed(int autotest);
extern int panther7_hdma_destination_direction_increment(int autotest);

struct test_item panther7_hdma_dst_dir_items[] = {
	{"Destination INCR",
	 panther7_hdma_destination_direction_increment,
	 1,
	 1},
	{"Destination FIXED",
	 panther7_hdma_destination_direction_fixed,
	 1,
	 1}
};

struct test_item_container panther7_hdma_dst_dir_test_container = {
	.menu_name = "PANTHER7_HDMA Destination Direction Menu",
	.shell_name = "panther7 hdma",
	.items = panther7_hdma_dst_dir_items,
	.test_item_size = sizeof(panther7_hdma_dst_dir_items)
};

extern int panther7_hdma_burst_single(int autotest);
extern int panther7_hdma_burst_incr4(int autotest);
extern int panther7_hdma_burst_incr8(int autotest);
extern int panther7_hdma_burst_incr16(int autotest);

struct test_item panther7_hdma_burst_items[] = {
	{"Single",
	 panther7_hdma_burst_single,
	 1,
	 1},
	{"INCR4",
	 panther7_hdma_burst_incr4,
	 1,
	 1},
	{"INCR8",
	 panther7_hdma_burst_incr8,
	 1,
	 1},
	{"INCR16",
	 panther7_hdma_burst_incr16,
	 1,
	 1}
};

struct test_item_container panther7_hdma_burst_test_container = {
	.menu_name = "PANTHER7_HDMA Burst Type Menu",
	.shell_name = "panther7 hdma",
	.items = panther7_hdma_burst_items,
	.test_item_size = sizeof(panther7_hdma_burst_items)
};

extern int panther7_hdma_data_byte(int autotest);
extern int panther7_hdma_data_halfword(int autotest);
extern int panther7_hdma_data_word(int autotest);

struct test_item panther7_hdma_data_items[] = {
	{"Byte",
	 panther7_hdma_data_byte,
	 1,
	 1},
	{"Halfword",
	 panther7_hdma_data_halfword,
	 1,
	 1},
	{"Word",
	 panther7_hdma_data_word,
	 1,
	 1}
};

struct test_item_container panther7_hdma_data_test_container = {
	.menu_name = "PANTHER7 HDMA Data Size Menu",
	.shell_name = "panther7 hdma",
	.items = panther7_hdma_data_items,
	.test_item_size = sizeof(panther7_hdma_data_items)
};

extern int panther7_hdma_run(int autotest);
extern int panther7_hdma_burn_in(int autotest);

struct test_item panther7_hdma_run_items[] = {
        {"Run",
         panther7_hdma_run,
         1,
         1},
        {"Burn in",
         panther7_hdma_burn_in,
         0,
         0},
};

struct test_item_container panther7_hdma_run_test_container = {
        .menu_name = "PANTHER7 HDMA Run Menu",
        .shell_name = "panther7 hdma",
        .items = panther7_hdma_run_items,
        .test_item_size = sizeof(panther7_hdma_run_items)
};

