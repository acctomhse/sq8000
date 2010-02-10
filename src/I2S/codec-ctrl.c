#include <test_item.h>
#include "dependency.h"
#include "i2s-ctrl.h"
#include "uda1342ts.h"
#include "ms6335.h"
#include "codec-ctrl.h"


int (*socle_audio_control_function)(void);

//Audio DAC function
extern int 
aduio_get_dac_fs(void)
{
	int ret=0;
#if defined(CONFIG_MS6335)
	ret = 384;
#elif defined(CONFIG_UDA1342TS)
	ret = 768;
#else
	printf("no audio dac fs\n");
#endif
	return ret;
}

extern int
audio_dac_initialize(void)
{
	int ret = 0;
#if defined(CONFIG_MS6335)
	ret = ms6335_dac_initialize();
#elif defined(CONFIG_UDA1342TS)
	ret = uda1342ts_dac_initialize();
#else
	printf("no audio dac initialize\n");
#endif
	return ret;
}


extern int 
audio_dac_master_volume(u16 val)
{
	int ret = 0;
#if defined(CONFIG_MS6335	)
	ret = ms6335_dac_master_volume(val);
#elif defined CONFIG_UDA1342TS
	ret = uda1342ts_dac_master_volume(val);
#else
	printf("no audio adc master volume\n");
#endif
	return ret;
}

extern int 
audio_dac_enable_mixer(bool val)
{
	int ret = 0;
#ifdef CONFIG_UDA1342TS	
	ret = uda1342ts_dac_enable_mixer(val);
#else
	printf("no audio adc enable mixer\n");
#endif
	return ret;
}

extern int 
audio_dac_mixer_volume(u16 val)
{
	int ret = 0;
#ifdef CONFIG_UDA1342TS	
	ret = uda1342ts_dac_mixer_volume(val);
#else
	printf("no audio adc mixer volume\n");
#endif
	return ret;
}

extern int
audio_dac_mode(u16 mode)
{
	int ret = 0;
#ifdef CONFIG_UDA1342TS	
	ret = uda1342ts_dac_mode(mode);
#else
	printf("no audio adc mode\n");
#endif
	return ret;
}

extern int
audio_dac_bass_boost(u8 val)
{
	int ret = 0;
#ifdef CONFIG_UDA1342TS	
	ret = uda1342ts_dac_bass_boost(val);
#else
	printf("no audio adc bass boost\n");
#endif
	return ret;
}

extern int
audio_dac_treble(u8 val)
{
	int ret = 0;
#ifdef CONFIG_UDA1342TS	
	ret = uda1342ts_dac_treble(val);
#else
	printf("no audio dac treble\n");
#endif
	return ret;
}

extern int
audio_dac_mute(bool val)
{
	int ret = 0;
#if defined(CONFIG_MS6335	)	
	ret = ms6335_dac_mute(val);
#elif defined(CONFIG_UDA1342TS)
	ret = uda1342ts_dac_mute(val);
#else
	printf("no audio dac mute\n");
#endif
	return ret;
}

extern int
audio_dac_de_emphasis(u16 val)
{
	int ret = 0;
#ifdef CONFIG_UDA1342TS	
	ret = uda1342ts_dac_de_emphasis(val);
#else
	printf("no audio dac de_emphasis\n");
#endif
	return ret;
}

//Audio ADC function
extern int
audio_adc_initialize(void)
{
	int ret = 0;
#ifdef CONFIG_UDA1342TS
	ret = uda1342ts_adc_initialize();
#else
	printf("no audio adc initialize\n");
#endif
	return ret;
}

extern int
socle_adc_channel_contrl(u32 ch)
{
	int ret = 0;
#ifdef CONFIG_UDA1342TS
	ret = uda1342ts_set_input_channel(ch);
#else
	printf("no audio adc channel contrl\n");
#endif
	return ret;
}

extern int
audio_adc_mode(u16 val)
{
	int ret = 0;
#ifdef CONFIG_UDA1342TS	
	ret = uda1342ts_adc_mode(val);
#else
	printf("no audio adc mode\n");
#endif
	return ret;	
}

extern int
audio_adc_input_amplifier_gain(u8 reg, u16 val)
{
	int ret = 0;
#ifdef CONFIG_UDA1342TS	
	ret = uda1342ts_adc_input_amplifier_gain(reg,val);
#else
	printf("no audio adc input amplifier gain\n");
#endif
	return ret;
}

extern int
audio_adc_mixer_gain(u8 reg, u16 val)
{
	int ret = 0;
#ifdef CONFIG_UDA1342TS	
	ret = uda1342ts_adc_mixer_gain(reg,val);
#else
	printf("no audio adc mixer gain\n");
#endif
	return ret;
}

extern int 
audio_output_dc_filter(bool val)
{
	int ret = 0;
#ifdef CONFIG_UDA1342TS	
	ret = uda1342ts_output_dc_filter(val);
#else
	printf("no audio output dc filter\n");
#endif
	return ret;
}

extern int 
audio_mixer_dc_filter(bool val)
{
	int ret = 0;
#ifdef CONFIG_UDA1342TS	
	ret = uda1342ts_mixer_dc_filter(val);
#else
	printf("no audio mixer dc filter\n");
#endif
	return ret;
}

static int
socle_audio_dac_master_volume_increment(void)
{
	int i, ret;

	/* Audio codec volume up */
#ifdef CONFIG_UDA1342TS
        for (i = MAX_VOLUME; i >= 0 ; i -= 25) {
#else
	for (i = 0; i <= MAX_VOLUME ; i++) {
#endif
		ret = audio_dac_master_volume(i);
		if (ret)
			return -1;
		MSDELAY(1000);
	}
#ifdef CONFIG_UDA1342TS
	ret = audio_dac_master_volume(0);
#else
	ret = audio_dac_master_volume(MAX_VOLUME);
#endif

	if (ret)
		return -1;

	return 0;
}

static int
socle_audio_dac_master_volume_decrement(void)
{
	int i, ret;

	/* Audio codec volume down */
#ifdef CONFIG_UDA1342TS	
	for (i = 0; i <= MAX_VOLUME; i += 25) {
#else
	for (i = MAX_VOLUME; i >= 0; i--) {
#endif
		ret = audio_dac_master_volume(i);
		if (ret)
			return -1;
		MSDELAY(1000);
	}
#ifdef CONFIG_UDA1342TS
	ret = audio_dac_master_volume(0);
#else
	ret = audio_dac_master_volume(MAX_VOLUME);
#endif

	if (ret)
		return -1;

	return 0;
}

static int
socle_audio_dac_mixer_volume_increment(void)
{
	int i, ret;

	/* Audio codec mixer volume up */
	for (i = 200; i > 0 ; i -= 20) {
		ret = audio_dac_mixer_volume(i);
		if (ret)
			return -1;
		MSDELAY(1000);
	}
	ret = audio_dac_master_volume(0);
	if (ret)
		return -1;

	return 0;
}

static int
socle_audio_dac_mixer_volume_decrement(void)
{
	int i, ret;

	/* Audio codec mixer volume down */
	for (i = 0; i < 200; i += 20) {
		ret = audio_dac_mixer_volume(i);
		if (ret)
			return -1;
		MSDELAY(1000);
	}
	ret = audio_dac_mixer_volume(200);
	if (ret)
		return -1;

	return 0;
}

static int
socle_audio_dac_bass_boost_increment(void)
{
	int i, ret;

	/* Audio codec bass boost up */
	for (i = 0; i < 16; i+= 3) {
		ret = audio_dac_bass_boost(i);
		if (ret)
			return -1;
		MSDELAY(1000);
	}
	ret = audio_dac_bass_boost(15);
	if (ret)
		return -1;

	return 0;
}

static int
socle_audio_dac_bass_boost_decrement(void)
{
	int i, ret;

	/* Audio codec bass boost up */
	for (i = 15; i > 0 ; i-= 3) {
		ret = audio_dac_bass_boost(i);
		if (ret)
			return -1;
		MSDELAY(1000);
	}
	ret = audio_dac_bass_boost(0);
	if (ret)
		return -1;

	return 0;
}

static int
socle_audio_dac_treble_increment(void)
{
	int i, ret;

	/* Audio codec bass boost up */
	for (i = 0; i < 4; i++ ) {
		ret = audio_dac_treble(i);
		if (ret)
			return -1;
		MSDELAY(1000);
	}

	return 0;
}

static int
socle_audio_dac_treble_decrement(void)
{
	int i, ret;

	/* Audio codec bass boost up */
	for (i = 3; i >= 0 ; i--) {
		ret = audio_dac_treble(i);
		if (ret)
			return -1;
		MSDELAY(1000);
	}

	return 0;
}

static int
socle_audio_dac_mute(void)
{
	int ret;

	MSDELAY(1000);
	ret = audio_dac_mute(true);
	if (ret)
		return -1;
	MSDELAY(1000);
	ret = audio_dac_mute(false);
	if (ret)
		return -1;

	return 0;
}

static int
socle_audio_dac_de_emphasis(void)
{
	int ret;

	MSDELAY(1000);
	ret = audio_dac_de_emphasis(UDA1342TS_FUNC_DAC_FEATURES_DE_EMPHASIS_44_1KHZ);
	if (ret)
		return -1;

	return 0;
}

static int
socle_audio_adc_input_amplifier_gain_increment(void)
{
	int i, ret;

	/* Audio codec input amplifier gain up */
	for (i = 0; i <= 8 ; i++) {
		ret = audio_adc_input_amplifier_gain(UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN_CH1, i);
		if (ret)
			return -1;
		MSDELAY(1000);
	}

	return 0;
}


static int
socle_audio_adc_mixer_gain_increment(void)
{
	int i, ret;

	/* Audio codec mixer gain up */
	for (i = 0; i < 48; i += 5) {
		ret = audio_adc_mixer_gain(UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN_CH1, i);
		if (ret)
			return -1;
		MSDELAY(1000);
	}

	ret = audio_adc_mixer_gain(UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN_CH1, 48);
	if (ret)
		return -1;
	return 0;
}

static int
socle_audio_adc_mixer_gain_decrement(void)
{
	int i, ret;

	/* Audio codec mixer gain down */
	for (i = 255; i > 128; i -= 13) {
		ret = audio_adc_mixer_gain(UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN_CH1, i);
		if (ret)
			return -1;
		MSDELAY(1000);
	}

	ret = audio_adc_mixer_gain(UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN_CH1, 128);
	if (ret)
		return -1;
	return 0;
}

static int 
socle_audio_adc_output_dc_filter(void)
{
	int ret;

	MSDELAY(3000);
	ret = audio_output_dc_filter(true);
	if (ret)
		return -1;
	MSDELAY(3000);
	ret = audio_output_dc_filter(false);
	if (ret)
		return -1;

	return 0;
}

static int 
socle_audio_adc_mixer_dc_filter(void)
{
	int ret;

	MSDELAY(3000);
	ret = audio_mixer_dc_filter(true);
	if (ret)
		return -1;
	MSDELAY(3000);
	ret = audio_mixer_dc_filter(false);
	if (ret)
		return -1;

	return 0;
}

//DAC Test 
extern int 
socle_audio_dac_master_volume_test(int autotest)
{
	int ret = 0;

	printf("Volume increment\n");
	socle_audio_control_function = socle_audio_dac_master_volume_increment;
	ret |= socle_i2s_play_pcm_normal(autotest);
	printf("Volume decrement\n");
	socle_audio_control_function = socle_audio_dac_master_volume_decrement;
	ret |= socle_i2s_play_pcm_normal(autotest);
	return ret;
}

extern int 
socle_audio_dac_mixer_volume_test(int autotest)
{
	int ret = 0;


	/* Enable the DAC mixer */


	printf("Volume increment\n");
	socle_audio_control_function = socle_audio_dac_mixer_volume_increment;
	ret |= socle_i2s_play_pcm_normal(autotest);
	printf("Volume decrement\n");
	socle_audio_control_function = socle_audio_dac_mixer_volume_decrement;
	ret |= socle_i2s_play_pcm_normal(autotest);
	return ret;
}

extern int 
socle_audio_dac_bass_boost_test(int autotest)
{
	int ret = 0;


	/* Set the mode to max*/


	printf("Bass boost increment\n");
	socle_audio_control_function =  socle_audio_dac_bass_boost_increment;
	ret |= socle_i2s_play_pcm_normal(autotest);
	printf("Bass boost decrement\n");
	socle_audio_control_function = socle_audio_dac_bass_boost_decrement;
	ret |= socle_i2s_play_pcm_normal(autotest);
	return ret;
}

extern int 
socle_audio_dac_treble_test(int autotest)
{
	int ret = 0;

	/* Set the mode to max*/
	audio_dac_mode(UDA1342TS_FUNC_DAC_FEATURES_MAX);

	printf("Treble increment\n");
	socle_audio_control_function = socle_audio_dac_treble_increment;
	ret |= socle_i2s_play_pcm_normal(autotest);
	printf("Treble decrement\n");
	socle_audio_control_function = socle_audio_dac_treble_decrement;
	ret |= socle_i2s_play_pcm_normal(autotest);
	return ret;
}

extern int 
socle_audio_dac_mute_test(int autotest)
{
	int ret = 0;

	/* Enable the DAC mixer */
	audio_dac_enable_mixer(true);

	socle_audio_control_function = socle_audio_dac_mute;
	ret = socle_i2s_play_pcm_normal(autotest);
	return ret;
}

extern int 
socle_audio_dac_de_emphasis_test(int autotest)
{
	int ret = 0;

	socle_audio_control_function = socle_audio_dac_de_emphasis;
	ret = socle_i2s_play_pcm_normal(autotest);
	return ret;
}

//ADC Test

extern int 
socle_audio_adc_input_amplifier_gain_test(int autotest)
{
	int ret = 0;

	/* Record */

	printf("Input amplifier gain increment\n");
	socle_audio_control_function = socle_audio_adc_input_amplifier_gain_increment;
	ret |= socle_i2s_capture_pcm_normal(autotest);


	/* Play */
	socle_audio_control_function = NULL;
	ret |= socle_i2s_play_pcm_normal(autotest);


	return ret;
}

extern int 
socle_audio_adc_mixer_gain_test(int autotest)
{
	int ret = 0;

	/* Record */

	printf("Mixer gain increment\n");
	socle_audio_control_function = socle_audio_adc_mixer_gain_increment;
	ret |= socle_i2s_capture_pcm_normal(autotest);

	/* Play */
	socle_audio_control_function = NULL;
	ret |= socle_i2s_play_pcm_normal(autotest);

	/* Record */


	printf("Mixer gain decrement\n");
	socle_audio_control_function = socle_audio_adc_mixer_gain_decrement;
	ret |= socle_i2s_capture_pcm_normal(autotest);


	/* Play */
	socle_audio_control_function = NULL;
	ret |= socle_i2s_play_pcm_normal(autotest);


	return ret;
}

extern int 
socle_audio_adc_output_dc_filter_test(int autotest)
{
	int ret;

	/* Record */

	socle_audio_control_function = socle_audio_adc_output_dc_filter;
	ret |= socle_i2s_capture_pcm_normal(autotest);


	/* Play */
	socle_audio_control_function = NULL;
	ret |= socle_i2s_play_pcm_normal(autotest);


	return ret;
}

extern int 
socle_audio_adc_mixer_dc_filter_test(int autotest)
{
	int ret;

	/* Record */

	socle_audio_control_function = socle_audio_adc_mixer_dc_filter;
	ret |= socle_i2s_capture_pcm_normal(autotest);


	/* Play */
	socle_audio_control_function = NULL;
	ret |= socle_i2s_play_pcm_normal(autotest);


	return ret;
}

extern struct test_item_container socle_audio_dac_control_test_container;

extern int 
socle_audio_codec_dac_test(int autotest)
{
	int ret;

	ret = test_item_ctrl(&socle_audio_dac_control_test_container, autotest);	
	return ret;

}

extern struct test_item_container socle_audio_adc_control_test_container;

extern int
socle_audio_adc_input_ch1_test(int autotest)
{
	int ret = 0;
	socle_adc_channel_contrl(1);
	ret = test_item_ctrl(&socle_audio_adc_control_test_container, autotest);
	return ret;
}

extern int
socle_audio_adc_input_ch2_test(int autotest)
{
	int ret = 0;
	socle_adc_channel_contrl(2);	
	ret = test_item_ctrl(&socle_audio_adc_control_test_container, autotest);
	return ret;
}

extern struct test_item_container socle_audio_adc_input_ch_test_container;

extern int 
socle_audio_codec_adc_test(int autotest)
{
	int ret;
	ret = test_item_ctrl(&socle_audio_adc_input_ch_test_container, autotest);	
	return ret;

}

extern struct test_item_container socle_audio_codec_adc_dac_test_container;	

extern int 
socle_audio_codec_uda1342_test(int autotest)
{
	int ret;
	
	ret = test_item_ctrl(&socle_audio_codec_adc_dac_test_container, autotest);	
	return ret;

}

extern int 
socle_audio_codec_ms6335_test(int autotest)
{
	int ret;

	ret = test_item_ctrl(&socle_audio_codec_adc_dac_test_container, autotest);	
	return ret;

}

