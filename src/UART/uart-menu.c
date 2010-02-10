#include <test_item.h>
#include "dependency.h"

extern int socle_uart_0_test(int autotest);
extern int socle_uart_1_test(int autotest);
extern int socle_uart_2_test(int autotest);
extern int socle_uart_3_test(int autotest);

struct test_item socle_uart_ip_test_items[] = {
	{"UART 0 Test",
	 socle_uart_0_test,
	 1,
	 SOCLE_UART_IP_0_TEST},
	{"UART 1 Test",
	 socle_uart_1_test,
	 1,
	 SOCLE_UART_IP_1_TEST},
	{"UART 2 Test",
	 socle_uart_2_test,
	 1,
	 SOCLE_UART_IP_2_TEST},
	{"UART 3 Test",
	 socle_uart_3_test,
	 1,
	 SOCLE_UART_IP_3_TEST}
};

struct test_item_container socle_uart_ip_test_container = {
	.menu_name = "UART IP Test ",
	.shell_name = "uart",
	.items = socle_uart_ip_test_items,
	.test_item_size = sizeof(socle_uart_ip_test_items)
};

extern int socle_uart_normal_test(int autotest);
extern int socle_uart_hwdma_panther7_hdma_test(int autotest);

struct test_item socle_uart_type_test_items[] = {
	{"Normal Test",
	 socle_uart_normal_test,
	 1,
	 1},
	{"Hardware Dma with Panther7 HDMA Test",
	 socle_uart_hwdma_panther7_hdma_test,
	 1,
	 SOCLE_UART_HWDMA_PANTHER7_HDMA_TEST}
};

struct test_item_container socle_uart_type_test_container = {
	.menu_name = "UART Type Test",
	.shell_name = "uart",
	.items = socle_uart_type_test_items,
	.test_item_size = sizeof(socle_uart_type_test_items)
};

extern int socle_uart_inter_loopback_test(int autotest);
extern int socle_uart_ext_loopback_test(int autotest);
extern int socle_uart_module_test(int autotest);

struct test_item socle_uart_transfer_test_items[] = {
	{"Internal Loopback Test",
	 socle_uart_inter_loopback_test,
	 1,
	 1},
	{"Externel Loopback Test",
	 socle_uart_ext_loopback_test,
	 1,
	 1},
	{"Module Test",
	 socle_uart_module_test,
	 1,
	 1}
};

struct test_item_container socle_uart_transfer_test_container = {
	.menu_name = "UART Transfer Test",
	.shell_name = "uart",
	.items = socle_uart_transfer_test_items,
	.test_item_size = sizeof(socle_uart_transfer_test_items)
};

extern int socle_uart_gps_test(int autotest);

struct test_item socle_uart_module_test_items[] = {
	{"GPS Test",
	 socle_uart_gps_test,
	 1,
	 1},
};

struct test_item_container socle_uart_module_test_container = {
	.menu_name = "UART Module Test",
	.shell_name = "uart",
	.items = socle_uart_module_test_items,
	.test_item_size = sizeof(socle_uart_module_test_items)
};

extern int socle_uart_baudrate_75(int autotest);
extern int socle_uart_baudrate_300(int autotest);
extern int socle_uart_baudrate_1200(int autotest);
extern int socle_uart_baudrate_2400(int autotest);
extern int socle_uart_baudrate_9600(int autotest);
extern int socle_uart_baudrate_19200(int autotest);
extern int socle_uart_baudrate_38400(int autotest);
extern int socle_uart_baudrate_57600(int autotest);
extern int socle_uart_baudrate_115200(int autotest);

struct test_item socle_uart_baudrate_test_items[] = {
	{"75",
	 socle_uart_baudrate_75,
	 1,
	 0},
	{"300",
	 socle_uart_baudrate_300,
	 1,
	 0},
	{"1200",
	 socle_uart_baudrate_1200,
	 1,
	 0},
	{"2400",
	 socle_uart_baudrate_2400,
	 1,
	 0},
	{"9600",
	 socle_uart_baudrate_9600,
	 1,
	 1},
	{"19200",
	 socle_uart_baudrate_19200,
	 1,
	 1},
	{"38400",
	 socle_uart_baudrate_38400,
	 1,
	 1},
	{"57600",
	 socle_uart_baudrate_57600,
	 1,
	 1},
	{"115200",
	 socle_uart_baudrate_115200,
	 1,
	 1},
};

struct test_item_container socle_uart_baudrate_test_container = {
	.menu_name = "UART Baudrate Test",
	.shell_name = "uart",
	.items = socle_uart_baudrate_test_items,
	.test_item_size = sizeof(socle_uart_baudrate_test_items)
};

extern int socle_uart_panther7_hdma_bursttype_single(int autotest);
extern int socle_uart_panther7_hdma_bursttype_incr4(int autotest);
extern int socle_uart_panther7_hdma_bursttype_incr8(int autotest);

struct test_item socle_uart_panther7_hdma_bursttype_test_items[] = {
	{"Single",
	 socle_uart_panther7_hdma_bursttype_single,
	 1,
	 1},
	{"INCR4",
	 socle_uart_panther7_hdma_bursttype_incr4,
	 1,
	 1},
	{"INCR8",
	 socle_uart_panther7_hdma_bursttype_incr8,
	 1,
	 1}
};

struct test_item_container socle_uart_panther7_hdma_bursttype_test_container = {
	.menu_name = "UART Panther7 HDMA Burst Type Test",
	.shell_name = "uart",
	.items = socle_uart_panther7_hdma_bursttype_test_items,
	.test_item_size = sizeof(socle_uart_panther7_hdma_bursttype_test_items)
};
