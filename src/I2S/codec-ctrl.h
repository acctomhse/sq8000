#ifndef __AUDIO_CODEC_H_INCLUDED
#define __AUDIO_CODEC_H_INCLUDED

#include <type.h>

#ifdef CONFIG_MS6335
#define MAX_VOLUME 31
#define MS6335_MENU 1
#else
#define MS6335_MENU 0
#endif

#ifdef CONFIG_UDA1342TS
#define MAX_VOLUME 255
#define UDA1342TS_MENU 1
#else
#define UDA1342TS_MENU 0
#endif

//for pcm play test wih audio codec
extern int (*socle_audio_control_function)(void);

//Audio DAC function
extern int aduio_get_dac_fs(void);
extern int audio_dac_initialize(void);
extern int audio_dac_master_volume(u16 val);
extern int audio_dac_enable_mixer(bool val);
extern int audio_dac_mixer_volume(u16 val);
extern int audio_dac_mode(u16 mode);
extern int audio_dac_bass_boost(u8 val);
extern int audio_dac_treble(u8 val);
extern int audio_dac_mute(bool val);
//1:enable mute , 0:disable mute
extern int audio_dac_de_emphasis(u16 val);
//Audio ADC function
extern int socle_adc_channel_contrl(u32 ch);
extern int audio_adc_initialize(void);
extern int audio_adc_mode(u16 val);
extern int audio_adc_input_amplifier_gain(u8 reg, u16 val);
extern int audio_adc_mixer_gain(u8 reg, u16 val);
extern int audio_output_dc_filter(bool val);
extern int audio_mixer_dc_filter(bool val);



#endif
