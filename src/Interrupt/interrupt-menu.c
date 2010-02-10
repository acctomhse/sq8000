#include <test_item.h>

extern int irq_test_mode_test(int autotest);
extern int fiq_test_mode_test(int autotest);
extern int int0_side_test(int autotest);
extern int int1_side_test(int autotest);

struct test_item interrupt_main_test_items[] = {
	{
		"Local Interrupt to Local CPU Test (IRQ)",
		irq_test_mode_test,
		1,
		1
	},
#if defined(CONFIG_CDK) || defined(CONFIG_PC9002) || defined(CONFIG_SCDK) || defined(CONFIG_MSMV)
	{
		"Local Interrupt to Local CPU Test (FIQ)",
		fiq_test_mode_test,
		1,
		1
	},
#endif
#if defined(CONFIG_LDK5) && (defined(CONFIG_ARM9) || defined(CONFIG_ARM9_HI))
	{
		"Interrupt1 to Interrupt0 Test (AHB0 side)",
		int0_side_test,
		0,
		1
	},
#endif
#if defined(CONFIG_LDK5) && (defined(CONFIG_ARM7) || defined(CONFIG_ARM7_HI))
	{
		"Interrupt1 to Interrupt0 Test (AHB1 side)",
		int1_side_test,
		0,
		1
	}
#endif
};

struct test_item_container interrupt_main_container = {
     .menu_name = "Interrupt Test Main Menu",
     .shell_name = "Interrupt",
     .items = interrupt_main_test_items,
     .test_item_size = sizeof(interrupt_main_test_items)
};


