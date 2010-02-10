#include <test_item.h>
#include "dependency.h"

extern int socle_si4703_device_id_and_chip_id_test(int autotest);
extern int socle_si4703_tune_to_selected_frequency_test(int autotest);
extern int socle_si4703_increase_channel_test(int autotest);
extern int socle_si4703_decrease_channel_test(int autotest);
extern int socle_si4703_seek_up_test(int autotest);
extern int socle_si4703_seek_down_test(int autotest);

struct test_item socle_si4703_main_test_items[] = {
	{"Device ID and Chip ID Test",
	 socle_si4703_device_id_and_chip_id_test,
	 1,
	 1},
	 {"Tune to Selected Frequency Test",
	 socle_si4703_tune_to_selected_frequency_test,
	 1,
	 1},
	{"Increase Channel Test",
	 socle_si4703_increase_channel_test,
	 0,
	 1},
	{"Decrease Channel Test",
	 socle_si4703_decrease_channel_test,
	 0,
	 1},
	{"Seek Up Test",
	 socle_si4703_seek_up_test,
	 0,
	 1},
	{"Seek Down Test",
	 socle_si4703_seek_down_test,
	 0,
	 1},
};

struct test_item_container socle_si4703_test_container = {
	.menu_name = "Si4703 Test Main Menu",
	.shell_name = "si4703",
	.items = socle_si4703_main_test_items,
	.test_item_size = sizeof(socle_si4703_main_test_items),
};

