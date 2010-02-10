#include <test_item.h>
#include "dependency.h"

extern int mem_range_test(int autotest);
extern int mem_bank_test(int autotest);
extern int mem_align_test(int autotest);
extern int repeat_write_test(int autotest);
extern int scdk_sram_test(int autotest);
extern int msdr_rw_test(int autotest);

struct test_item mem_main_test_items[] = {
	{
		"All Memeory Range Test",
		mem_range_test,
		1,
		1
	},
	{
		"Memory Bank Test",
		mem_bank_test,
		1,
		1
	},
	{
		"Memory Bank Align Test",
		mem_align_test,
		0,
		1
	},
	{
		"Repeat Someone Address Test",
		repeat_write_test,
		0,
		1
	},
	{
		"SCDK FPGA SRAM Test",
		scdk_sram_test,
		1,
		SCDK_FPGA_TEST
	},
	{
		"MSDR Test",
		msdr_rw_test,
		0,
		MSDR_TEST
	}
};

struct test_item_container mem_main_container = {
     .menu_name = "Memory Test Main Menu",
     .shell_name = "Memory",
     .items = mem_main_test_items,
     .test_item_size = sizeof(mem_main_test_items)
};

extern int mem_bank_normal_test(int autotest);
extern int mem_bank_burn_in_test(int autotest);

struct test_item mem_bank_test_items[] = {
	{
		"Bank Normal Test",
		mem_bank_normal_test,
		0,
		1
	},
	{
		"Bank Burn in Test",
		mem_bank_burn_in_test,
		0,
		1
	}
};

struct test_item_container mem_bank_test_container = {
     .menu_name = "Memory Bank Test Menu",
     .shell_name = "Memory Bank",
     .items = mem_bank_test_items,
     .test_item_size = sizeof(mem_bank_test_items)
};

extern int mem_pattern0_test(int autotest);
extern int mem_pattern1_test(int autotest);
extern int mem_pattern2_test(int autotest);
extern int mem_pattern3_test(int autotest);
extern int mem_pattern4_test(int autotest);
extern int mem_pattern5_test(int autotest);
extern int mem_pattern6_test(int autotest);
extern int mem_pattern7_test(int autotest);
extern int mem_pattern8_test(int autotest);
extern int mem_pattern9_test(int autotest);

struct test_item mem_range_test_items[] = {
	{
		"PATTERN0 Test",
		mem_pattern0_test,
		1,
		1
	},
	{
		"PATTERN1 Test",
		mem_pattern1_test,
		1,
		1
	},
	{
		"PATTERN2 Test",
		mem_pattern2_test,
		1,
		1
	},
	{
		"PATTERN3 Test",
		mem_pattern3_test,
		1,
		1
	},
	{
		"PATTERN4 Test",
		mem_pattern4_test,
		1,
		1
	},
	{
		"PATTERN5 Test",
		mem_pattern5_test,
		1,
		1
	},
	{
		"PATTERN6 Test",
		mem_pattern6_test,
		1,
		1
	},
	{
		"PATTERN7 Test",
		mem_pattern7_test,
		1,
		1
	},
	{
		"PATTERN8 Test",
		mem_pattern8_test,
		1,
		1
	},
};

struct test_item_container mem_range_test_container = {
     .menu_name = "Memory Range Pattern Test Menu",
     .shell_name = "Memory Patterm",
     .items = mem_range_test_items,
     .test_item_size = sizeof(mem_range_test_items)
};


extern int msdr_write_test(int autotest);
extern int msdr_compare_test(int autotest);

struct test_item msdr_test_items[] = {
	{
		"Make test pattern",
		msdr_write_test,
		0,
		1
	},
	{
		"Compare pattern",
		msdr_compare_test,
		0,
		1
	}
};

struct test_item_container msdr_test_container = {
     .menu_name = "MSDR Test Menu",
     .shell_name = "MSDR",
     .items = msdr_test_items,
     .test_item_size = sizeof(msdr_test_items)
};
