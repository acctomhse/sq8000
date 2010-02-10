#include "uda1342ts.h"
#include "../I2C/i2c.h"
#include "../I2C/dependency.h"

struct i2c_client uda1342ts_client;
static u16 default_system_reg_val = 0x1a02;

static int inline
uda1342ts_write(u8 reg, u16 val)
{
	int ret;
	u8 buf[3];

	buf[0] = reg;
	buf[1] = *((u8 *)&val + 1);
	buf[2] = *((u8 *)&val);
	ret = i2c_master_send(&uda1342ts_client, buf, 3);
	if (ret != 3)
		return -1;
	else
		return 0;
}

static int inline
uda1342ts_read(u8 reg)
{
	int ret;
	struct i2c_msg msg[2];
	u8 buf = reg;
	u8 ret_buf[2];
	int ret_val;
       
	memset((void *)msg, 0x00, 2*sizeof(struct i2c_msg));
	msg[0].addr = uda1342ts_client.addr;
	msg[1].addr = uda1342ts_client.addr;
	msg[0].buf = &buf;
	msg[0].len = 1;
	msg[1].flags = I2C_M_RD;
	msg[1].buf = ret_buf;
	msg[1].len = 2;
	ret = i2c_transfer(msg, 2);
	if (ret != 2)
		return -1;
	ret_val = (ret_buf[0] << 8) | ret_buf[1];
	return ret_val;
}

extern int
uda1342ts_dac_initialize(void)
{
	int ret;
	u16 val;

	printf("UDA1342TS Line Out initialize\n");
	i2c_master_initialize(SOCLE_APB0_I2C0, SOCLE_INTC_I2C0);
	uda1342ts_client.addr = UDA1342TS_SLAVE_ADDR_DAC;
	
	/* Reset the audio codec first */
	ret = uda1342ts_read(UDA1342TS_FUNC_SYSTEM);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_SYSTEM);
	}
	val = (u16)ret;
	val |= UDA1342TS_FUNC_SYSTEM_RST;
	ret = uda1342ts_write(UDA1342TS_FUNC_SYSTEM, val);
	if (-1 == ret)
		printf("Writing UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_SYSTEM);
	else
		printf("Writing UDA1342TS is pass, reg: 0x%02x, val: 0x%02x\n", UDA1342TS_FUNC_SYSTEM, val);
		
	ret = uda1342ts_read(UDA1342TS_FUNC_SYSTEM);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_SYSTEM);
	}
	printf("UDA1342TS_FUNC_SYSTEM: 0x%04x\n", ret);

	/* Set the system clock frequency */
	ret = uda1342ts_read(UDA1342TS_FUNC_SYSTEM);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_SYSTEM);
	}
	val = default_system_reg_val;
	val &= ~UDA1342TS_FUNC_SYSTEM_SYS_CLK_MASK;
	val |= UDA1342TS_FUNC_SYSTEM_768FS;
	ret = uda1342ts_write(UDA1342TS_FUNC_SYSTEM, val);
	if (-1 == ret)
		printf("Writing UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_SYSTEM);
	else
		printf("Writing UDA1342TS is pass, reg: 0x%02x, value: 0x%02x\n", UDA1342TS_FUNC_SYSTEM, val);
	ret = uda1342ts_read(UDA1342TS_FUNC_SYSTEM);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_SYSTEM);
	}
	printf("UDA1342TS_FUNC_SYSTEM: 0x%04x\n", ret);

#if 1
	ret = uda1342ts_read(UDA1342TS_FUNC_SUB_SYSTEM);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail\n");
	}
	printf("UDA1342TS_FUNC_SUB_SYSTEM: 0x%04x\n", ret);

	ret = uda1342ts_read(UDA1342TS_FUNC_DAC_FEATURES);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail\n");
	}
	printf("UDA1342TS_FUNC_DAC_FEATURES: 0x%04x\n", ret);

	ret = uda1342ts_read(UDA1342TS_FUNC_DAC_MASTER_VOLUME);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail\n");
	}
	printf("UDA1342TS_FUNC_DAC_MASTER_VOLUME: 0x%04x\n", ret);

	ret = uda1342ts_read(UDA1342TS_FUNC_DAC_MIXER_VOLUME);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail\n");
	}
	printf("UDA1342TS_FUNC_DAC_MIXER_VOLUME: 0x%04x\n", ret);

	ret = uda1342ts_read(UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN_CH1);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail\n");
	}
	printf("UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN_CH1: 0x%04x\n", ret);

	ret = uda1342ts_read(UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN_CH2);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail\n");
	}
	printf("UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN_CH2: 0x%04x\n", ret);

	ret = uda1342ts_read(UDA1342TS_FUNC_EVALUATION);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail\n");
	}
	printf("UDA1342TS_FUNC_EVALUATION: 0x%04x\n", ret);
#endif
	return ret;
}

extern int
uda1342ts_adc_initialize(void)
{
	int ret;
	u16 val;

	printf("UDA1342TS Line In initialize\n");
	i2c_master_initialize(SOCLE_APB0_I2C0, SOCLE_INTC_I2C0);
	uda1342ts_client.addr = UDA1342TS_SLAVE_ADDR_ADC;
/* 	ret = uda1342ts_write(UDA1342TS_FUNC_SYSTEM, 0x0000); */
/* 	if (-1 == ret) { */
/* 		printf("Writing UDA1342TS is fail\n"); */
/* 	} */
	
	/* Reset the audio codec first */
	ret = uda1342ts_read(UDA1342TS_FUNC_SYSTEM);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_SYSTEM);
	}
	val = (u16)ret;
	val |= UDA1342TS_FUNC_SYSTEM_RST;
	ret = uda1342ts_write(UDA1342TS_FUNC_SYSTEM, val);
	if (-1 == ret)
		printf("Writing UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_SYSTEM);
	ret = uda1342ts_read(UDA1342TS_FUNC_SYSTEM);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, ref: 0x%02x\n", UDA1342TS_FUNC_SYSTEM);
	}
	printf("UDA1342TS_FUNC_SYSTEM: 0x%04x\n", ret);

	/* Set the system clock frequency */
	ret = uda1342ts_read(UDA1342TS_FUNC_SYSTEM);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_SYSTEM);
	}
	val = default_system_reg_val;
	val &= ~UDA1342TS_FUNC_SYSTEM_SYS_CLK_MASK;
	val |= UDA1342TS_FUNC_SYSTEM_768FS;
	ret = uda1342ts_write(UDA1342TS_FUNC_SYSTEM, val);
	if (-1 == ret)
		printf("Writing UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_SYSTEM);
	ret = uda1342ts_read(UDA1342TS_FUNC_SYSTEM);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_SYSTEM);
	}
	printf("UDA1342TS_FUNC_SYSTEM: 0x%04x\n", ret);

#if 1
	ret = uda1342ts_read(UDA1342TS_FUNC_SUB_SYSTEM);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail\n");
	}
	printf("UDA1342TS_FUNC_SUB_SYSTEM: 0x%04x\n", ret);

	ret = uda1342ts_read(UDA1342TS_FUNC_DAC_FEATURES);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail\n");
	}
	printf("UDA1342TS_FUNC_DAC_FEATURES: 0x%04x\n", ret);

	ret = uda1342ts_read(UDA1342TS_FUNC_DAC_MASTER_VOLUME);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail\n");
	}
	printf("UDA1342TS_FUNC_DAC_MASTER_VOLUME: 0x%04x\n", ret);

	ret = uda1342ts_read(UDA1342TS_FUNC_DAC_MIXER_VOLUME);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail\n");
	}
	printf("UDA1342TS_FUNC_DAC_MIXER_VOLUME: 0x%04x\n", ret);

	ret = uda1342ts_read(UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN_CH1);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail\n");
	}
	printf("UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN_CH1: 0x%04x\n", ret);

	ret = uda1342ts_read(UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN_CH2);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail\n");
	}
	printf("UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN_CH2: 0x%04x\n", ret);

	ret = uda1342ts_read(UDA1342TS_FUNC_EVALUATION);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail\n");
	}
	printf("UDA1342TS_FUNC_EVALUATION: 0x%04x\n", ret);
#endif
	return ret;
}

extern int 
uda1342ts_dac_master_volume(u16 val)
{
	int ret;

	val |= (val << 8);
	ret = uda1342ts_write(UDA1342TS_FUNC_DAC_MASTER_VOLUME, val);
	if (-1 == ret)
		printf("Writing UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_DAC_MASTER_VOLUME);
	ret = uda1342ts_read(UDA1342TS_FUNC_DAC_MASTER_VOLUME);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_DAC_MASTER_VOLUME);
	}
	printf("UDA1342TS_FUNC_DAC_MASTER_VOLUME: 0x%04x\n", ret);
	return 0;
}

extern int 
uda1342ts_dac_enable_mixer(bool val)
{
	int ret;
	u16 tmp;
	int err = 0;

	ret = uda1342ts_read(UDA1342TS_FUNC_SUB_SYSTEM);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_SUB_SYSTEM);
		err |= -1;
	}
	tmp = (u16)ret;
	printf("UDA1342TS_FUNC_SUB_SYSTEM: 0x%04x\n", tmp);
	if (true == val) 
		tmp |= UDA1342TS_FUNC_SUB_SYSTEM_DAC_MIXER_EN;
	else 
		tmp &= ~UDA1342TS_FUNC_SUB_SYSTEM_DAC_MIXER_EN;
	ret = uda1342ts_write(UDA1342TS_FUNC_SUB_SYSTEM, tmp);
	if (-1 == ret) {
		printf("Writing UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_SUB_SYSTEM);
		err |= -1;
	}
	ret = uda1342ts_read(UDA1342TS_FUNC_SUB_SYSTEM);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_SUB_SYSTEM);
		err |= -1;
	}
	tmp = (u16)ret;
	printf("UDA1342TS_FUNC_SUB_SYSTEM: 0x%04x\n", tmp);
	return err;
}

extern int 
uda1342ts_dac_mixer_volume(u16 val)
{
	int ret;

	val |= (val << 8);
	ret = uda1342ts_write(UDA1342TS_FUNC_DAC_MIXER_VOLUME, val);
	if (-1 == ret)
		printf("Writing UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_DAC_MIXER_VOLUME);
	ret = uda1342ts_read(UDA1342TS_FUNC_DAC_MIXER_VOLUME);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_DAC_MIXER_VOLUME);
	}
	printf("UDA1342TS_FUNC_DAC_MIXER_VOLUME: 0x%04x\n", ret);
	return 0;

}

extern int
uda1342ts_dac_mode(u16 mode)
{
	int ret;
	u16 tmp;
	int err = 0;

	ret = uda1342ts_read(UDA1342TS_FUNC_DAC_FEATURES);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_DAC_FEATURES);
		err |= -1;
	}
	tmp = (u16)ret;
	printf("UDA1342TS_FUNC_DAC_FEATURES: 0x%04x\n", tmp);
	tmp &= ~UDA1342TS_FUNC_DAC_FEATURES_MODE_MASK;
	tmp |= mode;
	ret = uda1342ts_write(UDA1342TS_FUNC_DAC_FEATURES, tmp);
	if (-1 == ret) {
		printf("Writing UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_DAC_FEATURES);
		err |= -1;
	}
	ret = uda1342ts_read(UDA1342TS_FUNC_DAC_FEATURES);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_DAC_FEATURES);
		err |= -1;
	}
	tmp = (u16)ret;
	printf("UDA1342TS_FUNC_DAC_FEATURES: 0x%04x\n", tmp);
	return err;
}

extern int
uda1342ts_dac_bass_boost(u8 val)
{
	int ret;
	u16 tmp;
	int err = 0;

	ret = uda1342ts_read(UDA1342TS_FUNC_DAC_FEATURES);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_DAC_FEATURES);
		err |= -1;
	}
	tmp = (u16)ret;
	printf("UDA1342TS_FUNC_DAC_FEATURES: 0x%04x\n", tmp);
	tmp &= ~UDA1342TS_FUNC_DAC_FEATURES_BASS_BOOST_MASK;
	tmp |= UDA1342TS_FUNC_DAC_FEATURES_BASS_BOOST(val);
	ret = uda1342ts_write(UDA1342TS_FUNC_DAC_FEATURES, tmp);
	if (-1 == ret) {
		printf("Writing UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_DAC_FEATURES);
		err |= -1;
	}
	ret = uda1342ts_read(UDA1342TS_FUNC_DAC_FEATURES);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_DAC_FEATURES);
		err |= -1;
	}
	tmp = (u16)ret;
	printf("UDA1342TS_FUNC_DAC_FEATURES: 0x%04x\n", tmp);
	return err;
}

extern int
uda1342ts_dac_treble(u8 val)
{
	int ret;
	u16 tmp;
	int err = 0;

	ret = uda1342ts_read(UDA1342TS_FUNC_DAC_FEATURES);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_DAC_FEATURES);
		err |= -1;
	}
	tmp = (u16)ret;
	printf("UDA1342TS_FUNC_DAC_FEATURES: 0x%04x\n", tmp);
	tmp &= ~UDA1342TS_FUNC_DAC_FEATURES_TREBLE_MASK;
	tmp |= UDA1342TS_FUNC_DAC_FEATURES_TREBLE(val);
	ret = uda1342ts_write(UDA1342TS_FUNC_DAC_FEATURES, tmp);
	if (-1 == ret) {
		printf("Writing UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_DAC_FEATURES);
		err |= -1;
	}
	ret = uda1342ts_read(UDA1342TS_FUNC_DAC_FEATURES);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_DAC_FEATURES);
		err |= -1;
	}
	tmp = (u16)ret;
	printf("UDA1342TS_FUNC_DAC_FEATURES: 0x%04x\n", tmp);
	return err;
}

extern int
uda1342ts_dac_mute(bool val)
{
	int ret;
	u16 tmp;
	int err = 0;

	ret = uda1342ts_read(UDA1342TS_FUNC_DAC_FEATURES);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_DAC_FEATURES);
		err |= -1;
	}
	tmp = (u16)ret;
	printf("UDA1342TS_FUNC_DAC_FEATURES: 0x%04x\n", tmp);
	tmp &= ~UDA1342TS_FUNC_DAC_FEATURES_QUICK_MUTE_MODE;
	if (true == val)
		tmp |= UDA1342TS_FUNC_DAC_FEATURES_DAC_MUTE;
	else 
		tmp &= ~UDA1342TS_FUNC_DAC_FEATURES_DAC_MUTE;
	ret = uda1342ts_write(UDA1342TS_FUNC_DAC_FEATURES, tmp);
	if (-1 == ret) {
		printf("Writing UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_DAC_FEATURES);
		err |= -1;
	}
	ret = uda1342ts_read(UDA1342TS_FUNC_DAC_FEATURES);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_DAC_FEATURES);
		err |= -1;
	}
	tmp = (u16)ret;
	printf("UDA1342TS_FUNC_DAC_FEATURES: 0x%04x\n", tmp);
	return err;
}

extern int
uda1342ts_dac_de_emphasis(u16 val)
{
	int ret;
	u16 tmp;
	int err = 0;

	ret = uda1342ts_read(UDA1342TS_FUNC_DAC_FEATURES);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_DAC_FEATURES);
		err |= -1;
	}
	tmp = (u16)ret;
	printf("UDA1342TS_FUNC_DAC_FEATURES: 0x%04x\n", tmp);
	tmp &= ~UDA1342TS_FUNC_DAC_FEATURES_DE_EMPHASIS_MASK;
	tmp |= val;
	ret = uda1342ts_write(UDA1342TS_FUNC_DAC_FEATURES, tmp);
	if (-1 == ret) {
		printf("Writing UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_DAC_FEATURES);
		err |= -1;
	}
	ret = uda1342ts_read(UDA1342TS_FUNC_DAC_FEATURES);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_DAC_FEATURES);
		err |= -1;
	}
	tmp = (u16)ret;
	printf("UDA1342TS_FUNC_DAC_FEATURES: 0x%04x\n", tmp);
	return err;
}

extern int
uda1342ts_adc_mode(u16 val)
{
	int ret;
	u16 tmp;
	int err = 0;

	ret = uda1342ts_read(UDA1342TS_FUNC_SYSTEM);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_SYSTEM);
		err |= -1;
	}
	tmp = (u16)ret;
	printf("UDA1342TS_FUNC_SYSTEM: 0x%04x\n", tmp);
	tmp &= ~UDA1342TS_FUNC_SYSTEM_ADC_MODE_MASK;
	tmp |= val;
	ret = uda1342ts_write(UDA1342TS_FUNC_SYSTEM, tmp);
	if (-1 == ret) {
		printf("Writing UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_SYSTEM);
		err |= -1;
	}
	ret = uda1342ts_read(UDA1342TS_FUNC_SYSTEM);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_SYSTEM);
		err |= -1;
	}
	tmp = (u16)ret;
	printf("UDA1342TS_FUNC_SYSTEM: 0x%04x\n", tmp);
	return err;
}


extern int
uda1342ts_adc_input_amplifier_gain(u8 reg, u16 val)
{
	int ret;
	u16 tmp;
	int err = 0;

	ret = uda1342ts_read(reg);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", reg);
		err |= -1;
	}
	tmp = (u16)ret;
	printf("UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN: 0x%04x\n", tmp);
	tmp &= ~UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN_INPUT_MASK;
	tmp |= UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN_INPUT(val);
	ret = uda1342ts_write(reg, tmp);
	if (-1 == ret) {
		printf("Writing UDA1342TS is fail, reg: 0x%02x\n", reg);
		err |= -1;
	}
	ret = uda1342ts_read(reg);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", reg);
		err |= -1;
	}
	tmp = (u16)ret;
	printf("UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN: 0x%04x\n", tmp);
	return err;
}

extern int 
uda1342ts_adc_mixer_gain(u8 reg, u16 val)
{
	int ret;
	u16 tmp;
	int err = 0;

	ret = uda1342ts_read(reg);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", reg);
		err |= -1;
	}
	tmp = (u16)ret;
	printf("UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN: 0x%04x\n", tmp);
	tmp &= ~UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN_MIXER_MASK;
	tmp |= UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN_MIXER(val);
	ret = uda1342ts_write(reg, tmp);
	if (-1 == ret) {
		printf("Writing UDA1342TS is fail, reg: 0x%02x\n", reg);
		err |= -1;
	}
	ret = uda1342ts_read(reg);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", reg);
		err |= -1;
	}
	tmp = (u16)ret;
	printf("UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN: 0x%04x\n", tmp);
	return err;
}

extern int 
uda1342ts_output_dc_filter(bool val)
{
	int ret;
	u16 tmp;
	int err = 0;

	ret = uda1342ts_read(UDA1342TS_FUNC_SYSTEM);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_SYSTEM);
		err |= -1;
	}
	tmp = (u16)ret;
	printf("UDA1342TS_FUNC_SYSTEM: 0x%04x\n", tmp);
	if (true == val)
		tmp |= UDA1342TS_FUNC_SYSTEM_OUTPUT_DC_FILTER_EN;
	else
		tmp &= ~UDA1342TS_FUNC_SYSTEM_OUTPUT_DC_FILTER_EN;
	ret = uda1342ts_write(UDA1342TS_FUNC_SYSTEM, tmp);
	if (-1 == ret) {
		printf("Writing UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_SYSTEM);
		err |= -1;
	}
	ret = uda1342ts_read(UDA1342TS_FUNC_SYSTEM);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_SYSTEM);
		err |= -1;
	}
	tmp = (u16)ret;
	printf("UDA1342TS_FUNC_SYSTEM: 0x%04x\n", tmp);
	return err;
}

extern int 
uda1342ts_mixer_dc_filter(bool val)
{
	int ret;
	u16 tmp;
	int err = 0;

	ret = uda1342ts_read(UDA1342TS_FUNC_SYSTEM);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_SYSTEM);
		err |= -1;
	}
	tmp = (u16)ret;
	printf("UDA1342TS_FUNC_SYSTEM: 0x%04x\n", tmp);
	if (true == val)
		tmp &= ~UDA1342TS_FUNC_SYSTEM_MIXER_DC_FILTER_DIS;
	else
		tmp |= UDA1342TS_FUNC_SYSTEM_MIXER_DC_FILTER_DIS;
	ret = uda1342ts_write(UDA1342TS_FUNC_SYSTEM, tmp);
	if (-1 == ret) {
		printf("Writing UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_SYSTEM);
		err |= -1;
	}
	ret = uda1342ts_read(UDA1342TS_FUNC_SYSTEM);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_SYSTEM);
		err |= -1;
	}
	tmp = (u16)ret;
	printf("UDA1342TS_FUNC_SYSTEM: 0x%04x\n", tmp);
	return err;
}

extern int 
uda1342ts_set_input_channel(u32 ch)
{
	int ret;
	u16 val;
	
	val = uda1342ts_read(UDA1342TS_FUNC_SYSTEM);
	if (ch == 1)
		ch = UDA1342TS_FUNC_SYSTEM_INPUT_SEL_DOUBLE_DIFF_1;
	else
		ch = UDA1342TS_FUNC_SYSTEM_INPUT_SEL_DOUBLE_DIFF_2;
	
	val = (val & ~(0x7<<9)) | ch;
	
	printf("set input channel\n");
	ret = uda1342ts_write(UDA1342TS_FUNC_SYSTEM, val);
	if (-1 == ret)
		printf("Writing UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_SYSTEM);
	ret = uda1342ts_read(UDA1342TS_FUNC_SYSTEM);
	if (-1 == ret) {
		printf("Reading UDA1342TS is fail, reg: 0x%02x\n", UDA1342TS_FUNC_SYSTEM);
	}
	printf("UDA1342TS_FUNC_SYSTEM: 0x%04x\n", ret);
	return ret;
}

