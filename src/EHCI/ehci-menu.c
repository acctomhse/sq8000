#include <test_item.h>
#include "dependency.h"

/* hdma_main_container */
extern int ehci_0_test(int autotest);
extern int ehci_1_test(int autotest);

struct test_item ehci_test_items[]={
        {"EHCI0 Test",
                ehci_0_test,
                1,
                1},
        {"EHCI1 Test",
                ehci_1_test,
                1,
                SOCLE_EHCI1_TEST},
};

struct test_item_container ehci_main_container={
        .menu_name="EHCI Test Menu",
        .shell_name="ehci",
        .items=ehci_test_items,
        .test_item_size=sizeof(ehci_test_items)
};



