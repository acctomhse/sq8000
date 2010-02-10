#include <test_item.h>

/*	test item define	*/
#if defined(CONFIG_CDK) || defined(CONFIG_PC9002) || defined(CONFIG_PC9220) || defined(CONFIG_SCDK) || defined(CONFIG_MSMV)
#define HDMA_CH2_AUTO	1
#define	HDMA_CH2_ENABLE	1
#define	HDMA_CH3_AUTO	1
#define	HDMA_CH3_ENABLE	1
#else
#define HDMA_CH2_AUTO	0
#define	HDMA_CH2_ENABLE	0
#define	HDMA_CH3_AUTO	0
#define	HDMA_CH3_ENABLE	0
#endif


/* hdma_main_container */	
extern int hdma_set_parameter(int autotest);
extern int hdma_set_address_size(int autotest);
extern int hdma_test_nand(int autotest);

struct test_item hdma_test_items[]={
	{"Set DMA Parameter and run",
		hdma_set_parameter,
		1,
		1},
	{"Set addr and count",
		hdma_set_address_size,
		0,
		1},
	{"Test for NAND Flash",
		hdma_test_nand,
		0,
		0}
};

struct test_item_container hdma_main_container={
	.menu_name="HDMA Test Menu",
	.shell_name="hdma",
	.items=hdma_test_items,
	.test_item_size=sizeof(hdma_test_items)
};

/* hdma_para_setting_container */	
extern int hdma_channel_0(int autotest);
extern int hdma_channel_1(int autotest);
extern int hdma_channel_2(int autotest);
extern int hdma_channel_3(int autotest);

struct test_item hdma_channel_items[]={
	{"Channel 0",
		hdma_channel_0,
		1,
		1},
	{"Channel 1",
		hdma_channel_1,
		1,
		1},
	{"Channel 2",
		hdma_channel_2,
		HDMA_CH2_AUTO,
		HDMA_CH2_ENABLE},
	{"Channel 3",
		hdma_channel_3,
		HDMA_CH3_AUTO,
		HDMA_CH3_ENABLE}
};

struct test_item_container hdma_para_setting_container={
	.menu_name="HDMA Test Channel Menu",
	.shell_name="ch",
	.items=hdma_channel_items,
	.test_item_size=sizeof(hdma_channel_items)
};

/* hdma_ch_container */
extern int hdma_fly_disable(int autotest);
extern int hdma_fly_enable(int autotest);

struct test_item hdma_fly_items[]={
	{"Fly Disable",
		hdma_fly_disable,
		1,
		1},
	{"Fly Enable",
		hdma_fly_enable,
		0,
		0}
};

struct test_item_container hdma_ch_container={
	.menu_name="HDMA On the fly Menu",
	.shell_name="on the fly",
	.items=hdma_fly_items,
	.test_item_size=sizeof(hdma_fly_items)
};

/* hdma_fly_container */
extern int hdma_autoreload_disable(int autotest);
extern int hdma_autoreload_enable(int autotest);

struct test_item hdma_autoreload_items[]={
	{"Autoreload Disable",
		hdma_autoreload_disable,
		1,
		1},
	{"Autoreload Enable",
		hdma_autoreload_enable,
		0,
		1}
};

struct test_item_container hdma_fly_container={
	.menu_name="HDMA Autoreload Menu",
	.shell_name="autoreload",
	.items=hdma_autoreload_items,
	.test_item_size=sizeof(hdma_autoreload_items)
};

/* hdma_autoreload_container */
extern int hdma_source_direction_fixed(int autotest);
extern int hdma_source_direction_increment(int autotest);

struct test_item hdma_src_dir_items[]={
	{"Src Increment",
		hdma_source_direction_increment,
		1,
		1},
	{"Src Fixed",
		hdma_source_direction_fixed,
		1,
		1}
};

struct test_item_container hdma_autoreload_container={
	.menu_name="HDMA Source Direction Menu",
	.shell_name="src dir",
	.items=hdma_src_dir_items,
	.test_item_size=sizeof(hdma_src_dir_items)
};

/* hdma_src_dir_container */
extern int hdma_destination_direction_fixed(int autotest);
extern int hdma_destination_direction_increment(int autotest);

struct test_item hdma_dst_dir_items[]={
	{"Dst Increment",
		hdma_destination_direction_increment,
		1,
		1},
	{"Dst Fixed",
		hdma_destination_direction_fixed,
		1,
		1}
};

struct test_item_container hdma_src_dir_container={
	.menu_name="HDMA Destination Direction Menu",
	.shell_name="dst dir",
	.items=hdma_dst_dir_items,
	.test_item_size=sizeof(hdma_dst_dir_items)
};


/* hdma_dst_dir_container */
extern int hdma_burst_type_single(int autotest);
extern int hdma_burst_type_incr4(int autotest);
extern int hdma_burst_type_incr8(int autotest);
extern int hdma_burst_type_incr16(int autotest);

struct test_item hdma_burst_type_items[]={
	{"Single",
		hdma_burst_type_single,
		1,
		1},
	{"INCR4",
		hdma_burst_type_incr4,
		1,
		1},
	{"INCR8",
		hdma_burst_type_incr8,
		1,
		1},
	{"INCR16",
		hdma_burst_type_incr16,
		1,
		1}
};

struct test_item_container hdma_dst_dir_container={
	.menu_name="HDMA Burst Type Menu",
	.shell_name="burst type",
	.items=hdma_burst_type_items,
	.test_item_size=sizeof(hdma_burst_type_items)
};

/* hdma_burst_type_container */
extern int hdma_data_size_byte(int autotest);
extern int hdma_data_size_halfword(int autotest);
extern int hdma_data_size_word(int autotest);

struct test_item hdma_data_size_items[]={
	{"Byte",
		hdma_data_size_byte,
		1,
		1},
	{"Halfword",
		hdma_data_size_halfword,
		1,
		1},
	{"Word",
		hdma_data_size_word,
		1,
		1}
};

struct test_item_container hdma_burst_type_container={
	.menu_name="HDMA Data Size Menu",
	.shell_name="data size",
	.items=hdma_data_size_items,
	.test_item_size=sizeof(hdma_data_size_items)
};

/* hdma_data_size_container */
extern int socle_hdma_software_dma_tranfer(int autotest);
extern int socle_hdma_show_setting(int autotest);

struct test_item hdma_running_items[]={
	{"Running",
		socle_hdma_software_dma_tranfer,
		1,
		1},
	{"Show setting",
		socle_hdma_show_setting,
		0,
		1}
};

struct test_item_container hdma_data_size_container={
	.menu_name="HDMA Running Menu",
	.shell_name="running",
	.items=hdma_running_items,
	.test_item_size=sizeof(hdma_running_items)
};




