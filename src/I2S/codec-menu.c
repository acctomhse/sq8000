#include <test_item.h>
#include "dependency.h"
#include "codec-ctrl.h"


extern int socle_audio_dac_master_volume_test(int autotest);
extern int socle_audio_dac_mixer_volume_test(int autotest);
extern int socle_audio_dac_bass_boost_test(int autotest);
extern int socle_audio_dac_treble_test(int autotest);
extern int socle_audio_dac_mute_test(int autotest);
extern int socle_audio_dac_de_emphasis_test(int autotest);

struct test_item socle_audio_dac_control_test_items[] = {
	{"Master Volume",
	 socle_audio_dac_master_volume_test,
	 1,
	 1},
	{"Mixer Volume",
	 socle_audio_dac_mixer_volume_test,
	 1,
	 UDA1342TS_MENU},
	{"Bass Booster",
	 socle_audio_dac_bass_boost_test,
	 1,
	 UDA1342TS_MENU},
	{"Treble",
	 socle_audio_dac_treble_test,
	 1,
	 UDA1342TS_MENU},
	{"Mute",
	 socle_audio_dac_mute_test,
	 1,
	 UDA1342TS_MENU},
	{"De-emphasis",
	 socle_audio_dac_de_emphasis_test,
	 1,
	 UDA1342TS_MENU},
};

struct test_item_container socle_audio_dac_control_test_container = {
	.menu_name = "Audio DAC Control Test Menu",
	.shell_name = "dac",
	.items = socle_audio_dac_control_test_items,
	.test_item_size = sizeof(socle_audio_dac_control_test_items),
};

extern int socle_audio_adc_input_amplifier_gain_test(int autotest);
extern int socle_audio_adc_mixer_gain_test(int autotest);
extern int socle_audio_adc_output_dc_filter_test(int autotest);
extern int socle_audio_adc_mixer_dc_filter_test(int autotest);

struct test_item socle_audio_adc_control_test_items[] = {
	{"Input Amplifier Gain",
	 socle_audio_adc_input_amplifier_gain_test,
	 0,
	 UDA1342TS_MENU},
	{"Mixer Gain",
	 socle_audio_adc_mixer_gain_test,
	 0,
	 UDA1342TS_MENU},
	{"Output DC Filtering",
	 socle_audio_adc_output_dc_filter_test,
	 0,
	 UDA1342TS_MENU},
	{"Mixer DC Filtering",
	 socle_audio_adc_mixer_dc_filter_test,
	 0,
	 UDA1342TS_MENU},
};

struct test_item_container socle_audio_adc_control_test_container = {
	.menu_name = "Audio ADC Control Test Menu",
	.shell_name = "adc",
	.items = socle_audio_adc_control_test_items,
	.test_item_size = sizeof(socle_audio_adc_control_test_items),
};

extern int socle_audio_adc_input_ch1_test(int autotest);
extern int socle_audio_adc_input_ch2_test(int autotest);

struct test_item socle_audio_adc_input_ch_test_items[] = {
	{"Line in test",
	 socle_audio_adc_input_ch1_test,
	 0,
	 1},
	{"Microphone test",
	 socle_audio_adc_input_ch2_test,
	 0,
#ifdef CONFIG_SCDK
	 1
#else
	 0
#endif
	 },
};

struct test_item_container socle_audio_adc_input_ch_test_container = {
	.menu_name = "I2S Input Channel Select Test Menu",
	.shell_name = "i2s",
	.items = socle_audio_adc_input_ch_test_items,
	.test_item_size = sizeof(socle_audio_adc_input_ch_test_items),
};

extern int socle_audio_codec_dac_test(int autotest);
extern int socle_audio_codec_adc_test(int autotest);

struct test_item socle_audio_codec_adc_dac_test_items[] = {
	{"Socle Codec DAC Test",
	 socle_audio_codec_dac_test,
	 1,
	 1},
	{"Socle Codec ADC Test",
	 socle_audio_codec_adc_test,
	 1,
	 UDA1342TS_MENU},
};

struct test_item_container socle_audio_codec_adc_dac_test_container = {
	.menu_name = "Socle Audio Codec Test",
	.shell_name = "codec",
	.items = socle_audio_codec_adc_dac_test_items,
	.test_item_size = sizeof(socle_audio_codec_adc_dac_test_items),
};

extern int socle_audio_codec_uda1342_test(int autotest);
extern int socle_audio_codec_ms6335_test(int autotest);

struct test_item socle_audio_codec_test_items[] = {
	{"Socle Codec UDA1342ts ADC/DAC Test",
	 socle_audio_codec_uda1342_test,
	 1,
	 UDA1342TS_MENU},
	{"Socle MOSA6335/KHTEKAD1181 test",
	 socle_audio_codec_ms6335_test,
	 1,
	 MS6335_MENU},
};

struct test_item_container socle_audio_codec_test_container = {
	.menu_name = "Socle Audio Codec Test",
	.shell_name = "codec",
	.items = socle_audio_codec_test_items,
	.test_item_size = sizeof(socle_audio_codec_test_items),
};

