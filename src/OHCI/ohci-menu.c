#include <test_item.h>
#include "dependency.h"

/* hdma_main_container */
extern int ohci_0_test(int autotest);
extern int ohci_1_test(int autotest);

struct test_item ohci_test_items[]={
        {"OHCI0 Test",
                ohci_0_test,
                1,
                1},
        {"OHCI1 Test",
                ohci_1_test,
                1,
                SOCLE_OHCI1_TEST},
};

struct test_item_container ohci_main_container={
        .menu_name="OHCI Test Menu",
        .shell_name="ohci",
        .items=ohci_test_items,
        .test_item_size=sizeof(ohci_test_items)
};

