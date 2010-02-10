#include <test_item.h>

extern int cdk_scu_frequence_88M(int autotest);
extern int cdk_scu_frequence_133M(int autotest);
extern int cdk_scu_frequence_266M(int autotest);

struct test_item cdk_scu_frequence_test_items[] = {
	{"88MHz", 
	 cdk_scu_frequence_88M,
	 0,
	 1},
	{"133MHz", 
	 cdk_scu_frequence_133M,
	 0,
	 1},
	{"266MHz", 
	 cdk_scu_frequence_266M,
	 0,
	 1},
};

struct test_item_container cdk_scu_frequence_test_container = {
	.menu_name = "CDK SCU Frequence Menu",
	.shell_name = "scu_frq",
	.items = cdk_scu_frequence_test_items,
	.test_item_size = sizeof(cdk_scu_frequence_test_items)
};


extern int cdk_scu_mode_1vs1(int autotest);
extern int cdk_scu_mode_2vs1(int autotest);
extern int cdk_scu_mode_3vs1(int autotest);
extern int cdk_scu_mode_4vs1(int autotest);
extern int cdk_scu_mode_8vs1(int autotest);

struct test_item cdk_scu_mode_test_items[] = {
	{"1:1", 
	 cdk_scu_mode_1vs1,
	 1,
	 1},
	{"2:1", 
	 cdk_scu_mode_2vs1,
	 1,
	 1},
	 {"3:1", 
	 cdk_scu_mode_3vs1,
	 1,
	 1},
	 {"4:1", 
	 cdk_scu_mode_4vs1,
	 1,
	 1},
	 {"8:1", 
	 cdk_scu_mode_8vs1,
	 1,
	 1},
};

struct test_item_container cdk_scu_mode_test_container = {
	.menu_name = "CDK SCU Mode Menu",
	.shell_name = "scu_mode",
	.items = cdk_scu_mode_test_items,
	.test_item_size = sizeof(cdk_scu_mode_test_items)
};


extern int cdk_scu_stability_test(int autotest);

struct test_item cdk_scu_test_items[] = {
	{"Stability Test", 
	 cdk_scu_stability_test,
	 1,
	 1}
};

struct test_item_container cdk_scu_test_container = {
	.menu_name = "CDK SCU Test Menu",
	.shell_name = "scu_test",
	.items = cdk_scu_test_items,
	.test_item_size = sizeof(cdk_scu_test_items)
};
