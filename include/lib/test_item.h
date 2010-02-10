#ifndef __TEST_ITEMS_H_INCLUDED
#define __TEST_ITEMS_H_INCLUDED

#include "type.h"

struct test_item_container {
	char menu_name[100];				// menu name
	char shell_name[20];				// shell name
	struct test_item *items;			// test items pointer
	int test_item_size;					// size of test items
};

struct test_item {
     char item_name[100];				// test item name
     int (*item_func)(int autotest);	// test function
     bool allow_autotest;				// 1: allow autotest, 0: not allow autotest
     bool enable;						// 1: enable, 0: disable
};

extern int test_item_ctrl(struct test_item_container *container, int autotest);

#endif
