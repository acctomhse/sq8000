#include <test_item.h>
#include "dependency.h"

extern int socle_i2s_normal_test(int autotest);
extern int socle_i2s_hwdma_panther7_hdma_test(int auotest);

struct test_item socle_i2s_tx_test_items[] = {
	{"Normal Test",
	 socle_i2s_normal_test,
	 1,
	 1},
	{"Hardware Dma with Panther7 HDMA",
	 socle_i2s_hwdma_panther7_hdma_test,
	 SOCLE_I2S_HDMA_AUTO_TEST,
	 SOCLE_I2S_SUPPORT_HWDMA_PANTHER7_HDMA_TEST},
	
};

struct test_item_container socle_i2s_tx_test_container = {
	.menu_name = "I2S Transfer Test Menu",
	.shell_name = "i2s",
	.items = socle_i2s_tx_test_items,
	.test_item_size = sizeof(socle_i2s_tx_test_items),
};

extern int socle_i2s_hwdma_panther7_hdma_direct_test(int autotest);
extern int socle_i2s_hwdma_panther7_hdma_ring_buffer_test(int autotest);

struct test_item socle_i2s_tx_hwdma_panther7_hdma_test_items[] = {
	{"Direct Hardware DMA",
	 socle_i2s_hwdma_panther7_hdma_direct_test,
	 1,
	 1,},
	{"Ring Buffer Hardware DMA",
	 socle_i2s_hwdma_panther7_hdma_ring_buffer_test,
	 0,
	 0,},
};

struct test_item_container socle_i2s_tx_hwdma_panther7_hdma_test_container = {
	.menu_name = "I2S Hardware DMA With PANTHER7 HDMA Test Menu",
	.shell_name = "i2s",
	.items = socle_i2s_tx_hwdma_panther7_hdma_test_items,
	.test_item_size = sizeof(socle_i2s_tx_hwdma_panther7_hdma_test_items),
};

extern int socle_i2s_oversample_rate_32_test(int autotest);
extern int socle_i2s_oversample_rate_64_test(int autotest);
extern int socle_i2s_oversample_rate_128_test(int autotest);

struct test_item socle_i2s_oversample_rate_test_items[] = {
	{"32fs",
	 socle_i2s_oversample_rate_32_test,
	 1,
	 1},
	{"64fs",
	 socle_i2s_oversample_rate_64_test,
	 1,
	 1},
	{"128fs",
	 socle_i2s_oversample_rate_128_test,
	 1,
	 1},
};

struct test_item_container socle_i2s_oversample_rate_test_container = {
	.menu_name = "I2S Oversampling Rate Test Menu",
	.shell_name = "i2s",
	.items = socle_i2s_oversample_rate_test_items,
	.test_item_size = sizeof(socle_i2s_oversample_rate_test_items),
};

extern int socle_i2s_sample_resolution_8(int autotest);
extern int socle_i2s_sample_resolution_16(int autotest);
extern int socle_i2s_sample_resolution_20(int autotest);
extern int socle_i2s_sample_resolution_24(int autotest);

struct test_item socle_i2s_sample_res_test_items[] = {
	{"8 bits",
	 socle_i2s_sample_resolution_8,
	 1,
	 1},
	{"16 bits",
	 socle_i2s_sample_resolution_16,
	 1,
	 1},
	{"20 bits",
	 socle_i2s_sample_resolution_20,
	 1,
	 0},
	{"24 bits",
	 socle_i2s_sample_resolution_24,
	 1,
	 0},
};

struct test_item_container socle_i2s_sample_res_test_container = {
	.menu_name = "I2S Sample Data Resolution Test Menu",
	.shell_name = "i2s",
	.items = socle_i2s_sample_res_test_items,
	.test_item_size = sizeof(socle_i2s_sample_res_test_items),
};

extern int socle_i2s_tx_master_rx_slave(int autotest);
extern int socle_i2s_rx_master_tx_slave(int autotest);

struct test_item socle_i2s_master_slave_test_items[] = {
	{"Transmitter as Master and Receiver as Slave",
	 socle_i2s_tx_master_rx_slave,
	 1,
	 1},
	{"Receiver as Master and Transmitter as Slave",
	 socle_i2s_rx_master_tx_slave,
	 1,
	 1},
};

struct test_item_container socle_i2s_master_slave_test_container = {
	.menu_name = "I2S Master/Slave Test Menu",
	.shell_name = "i2s",
	.items = socle_i2s_master_slave_test_items,
	.test_item_size = sizeof(socle_i2s_master_slave_test_items),
};

extern int socle_i2s_hwdma_panther7_hdma_burst_single(int auotest);
extern int socle_i2s_hwdma_panther7_hdma_burst_incr4(int auotest);
extern int socle_i2s_hwdma_panther7_hdma_burst_incr8(int auotest);
extern int socle_i2s_hwdma_panther7_hdma_burst_incr16(int auotest);

struct test_item socle_i2s_hwdma_panther7_hdma_burst_test_items[] = {
	{"Single",
	 socle_i2s_hwdma_panther7_hdma_burst_single,
	 1,
	 1},
	{"INCR4",
	 socle_i2s_hwdma_panther7_hdma_burst_incr4,
	 1,
	 1},
	{"INCR8",
	 socle_i2s_hwdma_panther7_hdma_burst_incr8,
	 1,
	 SOCLE_I2S_HWDMA_BURST_INCR8},
	{"INCR16",
	 socle_i2s_hwdma_panther7_hdma_burst_incr16,
	 1,
	 SOCLE_I2S_HWDMA_BURST_INCR16},
};

struct test_item_container socle_i2s_hwdma_panther7_hdma_burst_test_container = {
	.menu_name = "I2S Hardware DMA with PANTHER7 HDMA Burst Type Test Menu",
	.shell_name = "i2s",
	.items = socle_i2s_hwdma_panther7_hdma_burst_test_items,
	.test_item_size = sizeof(socle_i2s_hwdma_panther7_hdma_burst_test_items),
};

extern int socle_i2s_input_ch1_test(int autotest);
extern int socle_i2s_input_ch2_test(int autotest);

struct test_item socle_i2s_input_ch_test_items[] = {
	{"Line in test",
	 socle_i2s_input_ch1_test,
	 0,
	 1},
	{"Microphone test",
	 socle_i2s_input_ch2_test,
	 0,
#ifdef CONFIG_SCDK
	 1
#else
	 0
#endif
	 },
};

struct test_item_container socle_i2s_input_ch_test_container = {
	.menu_name = "I2S Input Channel Select Test Menu",
	.shell_name = "i2s",
	.items = socle_i2s_input_ch_test_items,
	.test_item_size = sizeof(socle_i2s_input_ch_test_items),
};

extern int socle_i2s_play_pcm_8_test(int autotest);
extern int socle_i2s_play_pcm_16_test(int autotest);

struct test_item socle_i2s_play_pcm_sample_res_test_items[] = {
	{"8 bits PCM file",
	 socle_i2s_play_pcm_8_test,
	 0,
	 1},
	{"16 bits PCM file",
	 socle_i2s_play_pcm_16_test,
	 0,
	 1},
};

struct test_item_container socle_i2s_play_pcm_sample_res_test_container = {
	.menu_name = "I2S Playing PCM File Sample Data Resolution Test Menu",
	.shell_name = "i2s",
	.items = socle_i2s_play_pcm_sample_res_test_items,
	.test_item_size = sizeof(socle_i2s_play_pcm_sample_res_test_items),
};

extern int socle_i2s_play_pcm_normal_test(int autotest);
extern int socle_i2s_play_pcm_hwdma_panther7_hdma_test(int autotest);

struct test_item socle_i2s_play_pcm_mode_test_items[] = {
	{"Playing PCM file in normal mode",
	 socle_i2s_play_pcm_normal_test,
	 0,
	 1},
	{"Playing PCM file in hardware dma mode with PANTHER7 HDMA",
	 socle_i2s_play_pcm_hwdma_panther7_hdma_test,
	 0,
	 SOCLE_I2S_SUPPORT_HWDMA_PANTHER7_HDMA_TEST},
};

struct test_item_container socle_i2s_play_pcm_mode_test_container = {
	.menu_name = "I2S Playing PCM File Mode Test Menu",
	.shell_name = "i2s",
	.items = socle_i2s_play_pcm_mode_test_items,
	.test_item_size = sizeof(socle_i2s_play_pcm_mode_test_items),
};

extern int socle_i2s_capture_pcm_normal_test(int autotest);
extern int socle_i2s_capture_pcm_hwdma_panther7_hdma_test(int autotest);

struct test_item socle_i2s_capture_mode_test_items[] = {
	{"Capture in normal mode",
	 socle_i2s_capture_pcm_normal_test,
	 0,
	 1},
	{"Capture in hardware dma mode with PANTHER7 HDMA",
	 socle_i2s_capture_pcm_hwdma_panther7_hdma_test,
	 0,
	 SOCLE_I2S_SUPPORT_HWDMA_PANTHER7_HDMA_TEST},
};

struct test_item_container socle_i2s_capture_mode_test_container = {
	.menu_name = "I2S Capture Mode Test Menu",
	.shell_name = "i2s",
	.items = socle_i2s_capture_mode_test_items,
	.test_item_size = sizeof(socle_i2s_capture_mode_test_items),
};

extern int socle_i2s_bus_i2s_test(int autotest);
extern int socle_i2s_bus_lsb_test(int autotest);
extern int socle_i2s_bus_msb_test(int autotest);

struct test_item socle_i2s_bus_type_test_items[] = {
	{"I2S",
	 socle_i2s_bus_i2s_test,
	 1,
	 1},
	{"Left-Justified (LSB First)",
	 socle_i2s_bus_lsb_test,
	 1,
	 SOCLE_I2S_BUS_LSB_TEST},
	{"Right-Justified (MSB First)",
	 socle_i2s_bus_msb_test,
	 1,
	 SOCLE_I2S_BUS_MSB_TEST},
};

struct test_item_container socle_i2s_bus_type_test_container = {
	.menu_name = "I2S Bus Type Test Menu",
	.shell_name = "i2s",
	.items = socle_i2s_bus_type_test_items,
	.test_item_size = sizeof(socle_i2s_bus_type_test_items),
};

extern int socle_i2s_internal_loopback_test(int autotest);
extern int socle_i2s_play_pcm_test(int autotest);
extern int socle_i2s_capture_test(int autotest);
extern int socle_adc_dac_function_test(int autotest);
extern int socle_audio_codec_function_test(int autotest);

struct test_item socle_i2s_main_test_items[] = {
	{"Internal Loopback Test",
	 socle_i2s_internal_loopback_test,
	 1,
	 1},
	{"Play PCM file Test",
	 socle_i2s_play_pcm_test,
	 0,
	 1},
	{"Capture Test",
	 socle_i2s_capture_test,
	 0,
	 1},
	{"ADC/ADC Function Test",
	 socle_adc_dac_function_test,
	 0,
	 1},
	{"Audio Codec Library Function Test",
	 socle_audio_codec_function_test,
	 0,
	 0},
};

struct test_item_container socle_i2s_main_test_container = {
	.menu_name = "I2S Test Main Menu",
	.shell_name = "i2s",
	.items = socle_i2s_main_test_items,
	.test_item_size = sizeof(socle_i2s_main_test_items),
};


