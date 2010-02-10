#include <test_item.h>
#include "dependency.h"


extern int socle_voltage_test(int autotest);
extern int socle_other_test(int autotest);

struct test_item socle_tps_main_items[] = {
	{"Voltage Test",
	 socle_voltage_test,
	 1,
	 1},
	{"Other Test",
	 socle_other_test,
	 1,
	 1},
};

struct test_item_container socle_tps62353_main_container = {
	.menu_name = "TPS Main Test ",
	.shell_name = "TPS",
	.items = socle_tps_main_items,
	.test_item_size = sizeof(socle_tps_main_items)
};


extern int socle_voltage_increase (int autotest);
extern int socle_voltage_decrease (int autotest);
extern int socle_voltage_set (int autotest);
extern int socle_voltage_show (int autotest);

struct test_item socle_tps_voltage_items[] = {
	{"Voltage Increase (+4)",
	 socle_voltage_increase,
	 1,
	 1},
	{"Voltage Decrease (-4)",
	 socle_voltage_decrease,
	 1,
	 1},
	{"Voltage set",
	 socle_voltage_set,
	 0,
	 1},
	{"Voltage show",
	 socle_voltage_show,
	 0,
	 1},
};

struct test_item_container socle_tps62353_voltage_container = {
	.menu_name = "TPS Voltage Test ",
	.shell_name = "Voltage",
	.items = socle_tps_voltage_items,
	.test_item_size = sizeof(socle_tps_voltage_items)
};

extern int socle_regs_vsel0_set (int autotest);
extern int socle_regs_vsel1_set (int autotest);
extern int socle_regs_ctrl1_set (int autotest);
extern int socle_regs_ctrl2_set (int autotest);

struct test_item socle_tps_regs_items[] = {
	{"VSEL0",
	 socle_regs_vsel0_set,
	 0,
	 1},
	{"VSEL1",
	 socle_regs_vsel1_set,
	 0,
	 1},
	{"CONTROL1",
	 socle_regs_ctrl1_set,
	 0,
	 1},
	{"CONTROL2",
	 socle_regs_ctrl2_set,
	 0,
	 1},
};

struct test_item_container socle_tps62353_regs_container = {
	.menu_name = "TPS Regs Test ",
	.shell_name = "Regs",
	.items = socle_tps_regs_items,
	.test_item_size = sizeof(socle_tps_regs_items)
};





