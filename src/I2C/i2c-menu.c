#include <test_item.h>
#include "dependency.h"

extern int	socle_i2c0_test(int autotest);
extern int	socle_i2c1_test(int autotest);
extern int	socle_i2c2_test(int autotest);

struct test_item	socle_i2c_pre_test_items[] = {
	{ "I2C0 Test",
         socle_i2c0_test,
         1,
         1
        },
        {"I2C1 Test",
         socle_i2c1_test,
         SOCLE_I2C1_TEST,
         SOCLE_I2C1_TEST,
        },
        {"I2C2 Test",
         socle_i2c2_test,
         SOCLE_I2C2_TEST,
         SOCLE_I2C2_TEST,
        },
};

struct test_item_container socle_i2c_pre_test_container = {
        .menu_name = "I2C Test Menu",
        .shell_name = "i2c",
        .items = socle_i2c_pre_test_items,
        .test_item_size = sizeof(socle_i2c_pre_test_items),
};

extern int socle_i2c_internal_loopback_test(int autotest);
extern int socle_i2c_general_call_address_test(int autotest);
extern int socle_i2c_at24c02b_eeprom_test(int autotest);
extern int tps62353_test(int autotest);
extern int socle_i2c_si4703_tuner_test(int autotest);

struct test_item socle_i2c_main_test_items[] = {
	{"Internal Loopback Test",
	 socle_i2c_internal_loopback_test,
	 1,
	 1
	},
	{"General Call Address",
	 socle_i2c_general_call_address_test,
	 1,
	 1
	},
	{"AT24C02B EEPROM Test",
	 socle_i2c_at24c02b_eeprom_test,
	 1,
	 SOCLE_I2C_AT24C02B_EEPROM_TEST
	},
	{"TPS62353 Voltage Test",
	 tps62353_test,
	 SOCLE_I2C_TPS62353_TEST,
	 SOCLE_I2C_TPS62353_TEST,
	},
#ifdef CONFIG_SI4703
	{"Si4703 FM Radio Tuner Test",
	 socle_i2c_si4703_tuner_test,
	 1,
	 1
	},
#endif
};

struct test_item_container socle_i2c_main_test_container = {
	.menu_name = "I2C Test Main Menu",
	.shell_name = "i2c",
	.items = socle_i2c_main_test_items,
	.test_item_size = sizeof(socle_i2c_main_test_items),
};

extern int socle_i2c_address_7_test(int autotest);
extern int socle_i2c_address_10_test(int autotest);

struct test_item socle_i2c_address_test_items[] = {
	{"7-bit Address",
	 socle_i2c_address_7_test,
	 1,
	 1,
	},
	{"10-bit Address",
	 socle_i2c_address_10_test,
	 1,
	 1
	},
};

struct test_item_container socle_i2c_address_test_container = {
	.menu_name = "I2C Address Test Menu",
	.shell_name = "i2c",
	.items = socle_i2c_address_test_items,
	.test_item_size = sizeof(socle_i2c_address_test_items),
};

extern int socle_i2c_transfer_normal_format_test(int autotest);
extern int socle_i2c_transfer_combined_format_test(int autotest);

struct test_item socle_i2c_xfer_test_items[] = {
	{"Normal Format",
	 socle_i2c_transfer_normal_format_test,
	 1,
	 1,
	},
	{"Combined Format",
	 socle_i2c_transfer_combined_format_test,
	 1,
	 1
	},
};

struct test_item_container socle_i2c_xfer_test_container = {
	.menu_name = "I2C Transfer Test Menu",
	.shell_name = "i2c",
	.items = socle_i2c_xfer_test_items,
	.test_item_size = sizeof(socle_i2c_xfer_test_items),
};

extern int socle_i2c_general_call_address_reset_test(int autotest);
extern int socle_i2c_general_call_address_write_test(int autotest);
extern int socle_i2c_hardware_general_call_address_test(int autotest);

struct test_item socle_i2c_gen_call_addr_test_items[] = {
	{"Reset and write programmable part of slave address by hardware",
	 socle_i2c_general_call_address_reset_test,
	 1,
	 1,
	},
	{"Write programmable part of slave address by hardware",
	 socle_i2c_general_call_address_write_test,
	 1,
	 1
	},
	{"Hardware general call",
	 socle_i2c_hardware_general_call_address_test,
	 1,
	 1,
	},
};

struct test_item_container socle_i2c_gen_call_addr_test_container = {
	.menu_name = "I2C General Call Address Test Menu",
	.shell_name = "i2c",
	.items = socle_i2c_gen_call_addr_test_items,
	.test_item_size = sizeof(socle_i2c_gen_call_addr_test_items),
};
