#include <global.h>
#include <test_item.h>


extern struct test_item_container socle_lcm_main_container;

extern int
lcm_test(int autotest)
{
	int ret = 0;

	ret = test_item_ctrl(&socle_lcm_main_container, autotest);
	
	return ret;
}

