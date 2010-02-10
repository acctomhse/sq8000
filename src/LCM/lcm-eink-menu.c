#include <test_item.h>
#include "dependency.h"

extern int eink_2_level_bar_test(int autotest);
extern int eink_4_level_bar_test(int autotest);
extern int eink_4_level_image_test(int autotest);

struct test_item socle_eink_main_test_items[] = {
	{
		"2 Level Bar Test",
		eink_2_level_bar_test,
		1,
		1
	},
	{
		"4 Level Bar Test",
		eink_4_level_bar_test,
		1,
		1
	},
	{
		"4 Level Image Test",
		eink_4_level_image_test,
		0,
		1
	}
};

struct test_item_container socle_eink_main_container = {
     .menu_name = "Socle Eink Test Main Menu",
     .shell_name = "Eink",
     .items = socle_eink_main_test_items,
     .test_item_size = sizeof(socle_eink_main_test_items)
};

