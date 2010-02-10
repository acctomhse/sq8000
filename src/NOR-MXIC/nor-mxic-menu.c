#include <platform.h>
#include <test_item.h>

/*	NOR dependence	*/
#if defined SOCLE_NOR_FLASH_BANK1
	#define nor_1_auto 1
	#define nor_1_en 1
#else
	#define nor_1_auto 0
	#define nor_1_en 0
#endif


/* nor_main_container */	
extern int nor_mxic_test_0(int autotest);
extern int nor_mxic_test_1(int autotest);

struct test_item nor_mxic_main_items[]={
	{"NOR_Flash 0",
		nor_mxic_test_0,
		1,
		1},
	{"NOR_Flash 1",
		nor_mxic_test_1,
		nor_1_auto,
		nor_1_en}
};

struct test_item_container nor_mxic_main_container={
	.menu_name="NOR Flash Test Menu",
	.shell_name="nor",
	.items=nor_mxic_main_items,
	.test_item_size=sizeof(nor_mxic_main_items)
};




/* nor_test_container */	
extern int nor_mxic_sector_rw(int autotest);
extern int nor_mxic_read(int autotest);
extern int nor_mxic_write(int autotest);
extern int nor_mxic_erase_sector(int autotest);
extern int nor_mxic_erase_all(int autotest);

struct test_item nor_mxic_test_items[]={
	{"Sector r/w Test",
		nor_mxic_sector_rw,
		1,
		1},
	{"Read Test",
		nor_mxic_read,
		0,
		1},
	{"Write Test",
		nor_mxic_write,
		0,
		1},
	{"Erase Block",
		nor_mxic_erase_sector,
		0,
		1},
	{"Erase All",
		nor_mxic_erase_all,
		0,
		1}
};

struct test_item_container nor_mxic_test_container={
	.menu_name="NOR TEST Menu (MXIC)",
	.shell_name="nor_test",
	.items=nor_mxic_test_items,
	.test_item_size=sizeof(nor_mxic_test_items)
};


