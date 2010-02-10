#include <test_item.h>
#include "dependency.h"

extern int socle_sdmmc_sd_bus_1_transfer_test(int autotest);
extern int socle_sdmmc_sd_bus_4_transfer_test(int autotest);
extern int socle_sdmmc_sd_erase(int autotest);
extern int socle_sdmmc_sd_misc_command_test(int autotest);

struct test_item socle_sdmmc_sd_main_test_items[] = {
	{"1 Bit Bus Transfer Test",
	 socle_sdmmc_sd_bus_1_transfer_test,
	 1,
	 1},
	{"4 Bit Bus Transfer Test",
	 socle_sdmmc_sd_bus_4_transfer_test,
	 1,
	 1},
	{"Erasing Test",
	 socle_sdmmc_sd_erase,
	 1,
	 1},
	{"Miscellaneous Command Test",
	 socle_sdmmc_sd_misc_command_test,
	 1,
	 1}
};

struct test_item_container socle_sdmmc_sd_main_test_container = {
	.menu_name = "SD CardA Test",
	.shell_name = "sd",
	.items = socle_sdmmc_sd_main_test_items,
	.test_item_size = sizeof(socle_sdmmc_sd_main_test_items)
};

extern int socle_sdmmc_mmc_single_block_transfer_test(int autotest);
extern int socle_sdmmc_mmc_multiple_block_transfer_test(int autotest);
extern int socle_sdmmc_mmc_single_block_sw_dma_a2a_transfer_test(int autotest);
extern int socle_sdmmc_mmc_multiple_block_sw_dma_a2a_transfer_test(int autotest);
extern int socle_sdmmc_mmc_single_block_sw_dma_hdma_transfer_test(int autotest);
extern int socle_sdmmc_mmc_multiple_block_sw_dma_hdma_transfer_test(int autotest);
extern int socle_sdmmc_mmc_single_block_sw_dma_panther7_hdma_transfer_test(int autotest);
extern int socle_sdmmc_mmc_multiple_block_sw_dma_panther7_hdma_transfer_test(int autotest);

struct test_item socle_sdmmc_mmc_main_test_items[] = {
	{"Single Block Transfer Test",
	 socle_sdmmc_mmc_single_block_transfer_test,
	 1,
	 1},
	{"Multiple Block Transfer Test",
	 socle_sdmmc_mmc_multiple_block_transfer_test,
	 1,
	 1},
	{"Single Block Software Dma with A2A Transfer Test",
	 socle_sdmmc_mmc_single_block_sw_dma_a2a_transfer_test,
	 1,
	 SOCLE_SDMMC_SWDMA_A2A_TEST},
	{"Multiple Block Software Dma with A2A Transfer Test",
	 socle_sdmmc_mmc_multiple_block_sw_dma_a2a_transfer_test,
	 1,
	 SOCLE_SDMMC_SWDMA_A2A_TEST},
	{"Single Block Software Dma with HDMA Transfer Test",
	 socle_sdmmc_mmc_single_block_sw_dma_hdma_transfer_test,
	 1,
	 SOCLE_SDMMC_SWDMA_HDMA_TEST},
	{"Multiple Block Software Dma with HDMA Transfer Test",
	 socle_sdmmc_mmc_multiple_block_sw_dma_hdma_transfer_test,
	 1,
	 SOCLE_SDMMC_SWDMA_HDMA_TEST},
	{"Single Block Software Dma with Panther7 HDMA Transfer Test",
	 socle_sdmmc_mmc_single_block_sw_dma_panther7_hdma_transfer_test,
	 1,
	 SOCLE_SDMMC_SWDMA_PANTHER7_HDMA_TEST},
	{"Multiple Block Software Dma with Panther7 HDMA Transfer Test",
	 socle_sdmmc_mmc_multiple_block_sw_dma_panther7_hdma_transfer_test,
	 1,
	 SOCLE_SDMMC_SWDMA_PANTHER7_HDMA_TEST},

};

struct test_item_container socle_sdmmc_mmc_main_test_container = {
	.menu_name = "MMC CardA Test",
	.shell_name = "mmc",
	.items = socle_sdmmc_mmc_main_test_items,
	.test_item_size = sizeof(socle_sdmmc_mmc_main_test_items)
};

extern int socle_sdmmc_sd_single_block_transfer_test(int autotest);
extern int socle_sdmmc_sd_multiple_block_transfer_test(int autotest);
extern int socle_sdmmc_sd_single_block_sw_dma_a2a_transfer_test(int autotest);
extern int socle_sdmmc_sd_multiple_block_sw_dma_a2a_transfer_test(int autotest);
extern int socle_sdmmc_sd_single_block_sw_dma_hdma_transfer_test(int autotest);
extern int socle_sdmmc_sd_multiple_block_sw_dma_hdma_transfer_test(int autotest);
extern int socle_sdmmc_sd_single_block_sw_dma_panther7_hdma_transfer_test(int autotest);
extern int socle_sdmmc_sd_multiple_block_sw_dma_panther7_hdma_transfer_test(int autotest);
extern int socle_sdmmc_sd_single_partial_read(int autotest);
extern int socle_sdmmc_sd_multiple_partial_read(int autotest);

struct test_item socle_sdmmc_sd_xfer_test_items[] = {
	{"Single Block Transfer Test",
	 socle_sdmmc_sd_single_block_transfer_test,
	 1,
	 1},
	{"Multiple Block Transfer Test",
	 socle_sdmmc_sd_multiple_block_transfer_test,
	 1,
	 1},
	{"Single Block Software Dma with A2A Transfer Test",
	 socle_sdmmc_sd_single_block_sw_dma_a2a_transfer_test,
	 1,
	 SOCLE_SDMMC_SWDMA_A2A_TEST},
	{"Multiple Block Software Dma with A2A Transfer Test",
	 socle_sdmmc_sd_multiple_block_sw_dma_a2a_transfer_test,
	 1,
	 SOCLE_SDMMC_SWDMA_A2A_TEST},
	{"Single Block Software Dma with HDMA Transfer Test",
	 socle_sdmmc_sd_single_block_sw_dma_hdma_transfer_test,
	 1,
	 SOCLE_SDMMC_SWDMA_HDMA_TEST},
	{"Multiple Block Software Dma with HDMA Transfer Test",
	 socle_sdmmc_sd_multiple_block_sw_dma_hdma_transfer_test,
	 1,
	 SOCLE_SDMMC_SWDMA_HDMA_TEST},
	{"Single Block Software Dma with Panther7 HDMA Transfer Test",
	 socle_sdmmc_sd_single_block_sw_dma_panther7_hdma_transfer_test,
	 1,
	 SOCLE_SDMMC_SWDMA_PANTHER7_HDMA_TEST},
	{"Multiple Block Software Dma with Panther7 HDMA Transfer Test",
	 socle_sdmmc_sd_multiple_block_sw_dma_panther7_hdma_transfer_test,
	 1,
	 SOCLE_SDMMC_SWDMA_PANTHER7_HDMA_TEST},
	{"Single Block Partial Reading Test",
	 socle_sdmmc_sd_single_partial_read,
	 1,
	 1},
	{"Multiple Block Partial Reading Test",
	 socle_sdmmc_sd_multiple_partial_read,
	 1,
	 SOCLE_SDMMC_MUTLIPLE_PARTIAL_READ_TEST},
};

struct test_item_container socle_sdmmc_sd_xfer_test_container = {
	.menu_name = "SD CardA Transfer Test",
	.shell_name = "sd",
	.items = socle_sdmmc_sd_xfer_test_items,
	.test_item_size = sizeof(socle_sdmmc_sd_xfer_test_items)
};

extern int socle_sdmmc_transfer_width_byte(int autotest);
extern int socle_sdmmc_transfer_width_halfword(int autotest);
extern int socle_sdmmc_transfer_width_word(int autotest);

struct test_item socle_sdmmc_xfer_width_test_items[] = {
	{"Byte",
	 socle_sdmmc_transfer_width_byte,
	 1,
	 1},
	{"Halfword",
	 socle_sdmmc_transfer_width_halfword,
	 1,
	 1},
	{"Word",
	 socle_sdmmc_transfer_width_word,
	 1,
	 1},
};

struct test_item_container socle_sdmmc_xfer_width_test_container = {
	.menu_name = "CardA Transfer Width Test",
	.shell_name = "sd/mmc",
	.items = socle_sdmmc_xfer_width_test_items,
	.test_item_size = sizeof(socle_sdmmc_xfer_width_test_items)
};

extern int socle_sdmmc_swdma_burst_type_single_test(int autotest);
extern int socle_sdmmc_swdma_burst_type_incr4_test(int autotest);
extern int socle_sdmmc_swdma_burst_type_incr8_test(int autotest);
extern int socle_sdmmc_swdma_burst_type_incr16_test(int autotest);

struct test_item socle_sdmmc_swdma_burst_type_test_items[] = {
	{"Single",
	 socle_sdmmc_swdma_burst_type_single_test,
	 1,
	 1},
	{"INCR4",
	 socle_sdmmc_swdma_burst_type_incr4_test,
	 1,
	 1},
	{"INCR8",
	 socle_sdmmc_swdma_burst_type_incr8_test,
	 1,
	 1},
	{"INCR16",
	 socle_sdmmc_swdma_burst_type_incr16_test,
	 1,
	 1}
};

struct test_item_container socle_sdmmc_swdma_burst_type_test_container = {
	.menu_name = "SD/MMC Software DMA Burst Type Test Menu",
	.shell_name = "sd/mmc",
	.items = socle_sdmmc_swdma_burst_type_test_items,
	.test_item_size = sizeof(socle_sdmmc_swdma_burst_type_test_items)
};

extern int socle_sdmmc_sd_misc_command_9(int autotest);
extern int socle_sdmmc_sd_misc_command_10(int autotest);
extern int socle_sdmmc_sd_misc_command_13(int autotest);
extern int socle_sdmmc_sd_misc_command_15(int autotest);
extern int socle_sdmmc_sd_misc_app_command_51(int autotest);

struct test_item socle_sdmmc_sd_misc_command_test_items[] = {
	{"Command 9 (SEND_CSD)",
	 socle_sdmmc_sd_misc_command_9,
	 1,
	 1},
	{"Command 10 (SEND_CID)",
	 socle_sdmmc_sd_misc_command_10,
	 1,
	 1},
	{"Command 13 (SEND_STATUS)",
	 socle_sdmmc_sd_misc_command_13,
	 1,
	 1},
	{"Command 15 (GO_INACTIVE_STATE)",
	 socle_sdmmc_sd_misc_command_15,
	 0,
	 1},
	{"Application Specific Command 51 (SEND_SCR)",
	 socle_sdmmc_sd_misc_app_command_51,
	 1,
	 1}
};

struct test_item_container socle_sdmmc_sd_misc_command_test_container = {
	.menu_name = "SD Miscellaneouse Command Test Menu",
	.shell_name = "sd",
	.items = socle_sdmmc_sd_misc_command_test_items,
	.test_item_size = sizeof(socle_sdmmc_sd_misc_command_test_items)
};


