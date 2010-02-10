#include <test_item.h>
#include "Si4703.h"
#include "i2c.h"
#include "dependency.h"
#include "../GPIO/gpio.h"
#include "../I2S/codec-ctrl.h"

//#define CONFIG_SI4703C_DBG

static struct i2c_client si4703_client = {0};
static struct si4703_struct si4703 = {0};
static volatile int stc = 0;
static int si4703_chan = 0;

static inline int
si4703_write_data(u8 *data, int len)
{
	int ret;

#ifdef CONFIG_SI4703C_DBG
	int i;
	printf("send:");
	for (i = 0; i < len; i++) {
		if (!(i % 2))
			printf("\n");			
		printf("%02x ", data[i]);
	}
	printf("\n");
#endif

	ret = i2c_master_send(&si4703_client, data, len);

	if (ret != len)
		return -1;
	return 0;
}

static inline int
si4703_read_data(u8 *data, int len)
{
	int ret;

#ifdef CONFIG_SI4703C_DBG
	int i;
	printf("before receive:");
	for (i = 0; i < len; i++) {
		if (!(i % 2))
			printf("\n");			
		printf("%02x ", data[i]);
	}
	printf("\n");
#endif

	ret = i2c_master_recv(&si4703_client, data, len);

#ifdef CONFIG_SI4703C_DBG
	printf("after receive:");
	for (i = 0; i < len; i++) {
		if (!(i % 2))
			printf("\n");			
		printf("%02x ", data[i]);
	}
	printf("\n");
#endif

	if (ret != len)
		return -1;
	return 0;
}

static inline int
si4703_power_up(void)
{
	u16 val;

	val = POWERCFG_ENABLE;
	si4703.reg_02_h = TO_REG_8_h(val);
	si4703.reg_02_l = TO_REG_8_l(val);
	if (si4703_write_data((u8 *)&(si4703.reg_02_h), 2)) {
		return -1;
	}

	MSDELAY(500);	// wait for powerup
	MSDELAY(500);

	val = POWERCFG_ENABLE | POWERCFG_DMUTE;
	si4703.reg_02_h = TO_REG_8_h(val);
	si4703.reg_02_l = TO_REG_8_l(val);
	if (si4703_write_data((u8 *)&(si4703.reg_02_h), 2)) {
		return -1;
	}

	return 0;
}

static int
si4703_read_status_rssi(void)
{
	u16 status;

	if (si4703_read_data((u8 *)&(si4703.reg_0a_h), 2)) {
		return -1;
	}

	status = TO_REG_16(si4703.reg_0a_h, si4703.reg_0a_l);

//	printf("status = 0x%x\n", status);

	if (status & STATUSRSSI_STC) {
		//printf("Seek/Tune Complete\n");
		stc = 1;
	} else {
		stc = 0;
	}

	if (stc) {
		if (status & STATUSRSSI_ST)
			printf("The station is \"stereo\"\n");
		else
			printf("The station is \"mono\"\n");
	}

	return 0;
}

static int
si4703_read_frequency(void)
{
	u16 channel;
	int freq;

	if (si4703_read_data((u8 *)&(si4703.reg_0a_h), 4)) {
		return -1;
	}

	channel = TO_REG_16(si4703.reg_0b_h, si4703.reg_0b_l);
	//printf("si4703_read_frequency channel = %x %x\n", channel, READCHAN_READCHAN(channel));
	si4703_chan = READCHAN_READCHAN(channel);
	freq = 5 * si4703_chan + 8750;
	//freq = 0.05 * ((int)READCHAN_READCHAN(channel)) + 87.5;
	printf("si4703_read_frequency(): freq = %d.%02d\n", freq / 100, freq % 100);

	return 0;
}

extern int
si4703_tune_frequency(int chan)
//si4703_tune_frequency(float freq)
{
	u16 val;
//	int chan;

//	chan = (int)((freq - 87.5) / 0.05) + 1;
	val = CHANNEL_TUNE | CHANNEL_CHAN(chan);
	si4703.reg_03_h = TO_REG_8_h(val);
	si4703.reg_03_l = TO_REG_8_l(val);

	val = SYSCONFIG2_BAND(0x0) | SYSCONFIG2_SPACE(0x2) | SYSCONFIG2_VOLUME(0xf);
	si4703.reg_05_h = TO_REG_8_h(val);
	si4703.reg_05_l = TO_REG_8_l(val);

	if (si4703_write_data((u8 *)&(si4703.reg_02_h), 8)) {
		return -1;
	}

	while (!stc) {
		if (si4703_read_status_rssi()) {
			return -1;
		}
		MSDELAY(3);
	};	// wait for seek/tune complete

	// stop tuning
	val = TO_REG_16(si4703.reg_03_h, si4703.reg_03_l) & ~CHANNEL_TUNE;
	si4703.reg_03_h = TO_REG_8_h(val);
	si4703.reg_03_l = TO_REG_8_l(val);

	if (si4703_write_data((u8 *)&(si4703.reg_02_h), 4)) {
		return -1;
	}

	while (stc) {
		if (si4703_read_status_rssi()) {
			return -1;
		}
	};	// check to clear seek/tune complete

	if (si4703_read_frequency()) {
		return -1;
	}

	return 0;
}

extern int
si4703_seek_channel(int seekup)
{
	u16 val;

	val = TO_REG_16(si4703.reg_02_h, si4703.reg_02_l) | POWERCFG_SEEK;
	if (seekup)
		val |= POWERCFG_SEEKUP;
	else
		val &= ~POWERCFG_SEEKUP;
	si4703.reg_02_h = TO_REG_8_h(val);
	si4703.reg_02_l = TO_REG_8_l(val);

	val = SYSCONFIG2_SEEKTH(0xf) | SYSCONFIG2_BAND(0x0) | SYSCONFIG2_SPACE(0x2) | SYSCONFIG2_VOLUME(0xf);
	si4703.reg_05_h = TO_REG_8_h(val);
	si4703.reg_05_l = TO_REG_8_l(val);

	val = SYSCONFIG3_SKSNR(0x6) | SYSCONFIG3_SKCNT(0x4);
	si4703.reg_06_h = TO_REG_8_h(val);
	si4703.reg_06_l = TO_REG_8_l(val);

	if (si4703_write_data((u8 *)&(si4703.reg_02_h), 10)) {
		return -1;
	}

	while (!stc) {
		if (si4703_read_status_rssi()) {
			return -1;
		}
		MSDELAY(3);
	};	// wait for seek/tune complete

	// stop seeking
	val = TO_REG_16(si4703.reg_02_h, si4703.reg_02_l) & ~POWERCFG_SEEK;
	si4703.reg_02_h = TO_REG_8_h(val);
	si4703.reg_02_l = TO_REG_8_l(val);

	if (si4703_write_data((u8 *)&(si4703.reg_02_h), 2)) {
		return -1;
	}

	while (stc) {
		if (si4703_read_status_rssi()) {
			return -1;
		}
	};	// check to clear seek/tune complete

	if (si4703_read_frequency()) {
		return -1;
	}

	return 0;
}


extern struct test_item_container socle_si4703_test_container;

extern int
socle_i2c_si4703_tuner_test(int autotest)
{
	int ret;

#ifdef CONFIG_PC7210
	// set audio source selection to FM
	socle_gpio_set_value_with_mask(PE, 0x18, 0x3f);				// PE[1:0] = [00]
#endif

#ifdef CONFIG_I2S
	if (audio_dac_initialize()) {
		printf("audio_dac_initialize fail\n");
		return -1;
	}
#endif

	i2c_master_initialize(SOCLE_APB0_I2C0, SOCLE_INTC_I2C0);

	si4703_client.addr = SI4703_SLAVE_ADDR;
	si4703_client.flags = 0;

	if (si4703_power_up()) {
		printf("si4703_power_up fail\n");
		return -1;
	}

	ret = test_item_ctrl(&socle_si4703_test_container, autotest);

	return 0;
}

extern int
socle_si4703_device_id_and_chip_id_test(int autotest)
{
	u16 val;

	if (si4703_read_data((u8 *)&(si4703.reg_0a_h), 16)) {
		return -1;
	}

	val = TO_REG_16(si4703.reg_00_h, si4703.reg_00_l);
	if (SI4703_DEVICEID != val) {
		printf("SI4703_DEVICEID(0x%04x) != 0x%04x\n", val, SI4703_DEVICEID);
		return -1;
	} else
		printf("Device ID = 0x%04x\n", val);

	val = TO_REG_16(si4703.reg_01_h, si4703.reg_01_l);
	if (SI4703_CHIPID != val) {
		printf("SI4703_CHIPID(0x%04x) != 0x%04x\n", val, SI4703_CHIPID);
		return -1;
	} else
		printf("Chip ID = 0x%04x\n", val);

	return 0;
}

extern int
socle_si4703_tune_to_selected_frequency_test(int autotest)
{
	if (autotest) {
//		si4703_chan = (int)((100.1 - 87.5) / 0.05) + 1;
		si4703_chan = (10010 - 8750) / 5;	// 100.1 MHz
	} else {
		int m, k;
		printf("Please input frequency, ex: 100.1, freq = ");
		scanf("%d.%d", &m, &k);
		si4703_chan = (m * 100 + k * 10 - 8750) / 5;
	}

	return si4703_tune_frequency(si4703_chan);
}

extern int
socle_si4703_increase_channel_test(int autotest)
{
	si4703_chan++;
	return si4703_tune_frequency(si4703_chan);
}

extern int
socle_si4703_decrease_channel_test(int autotest)
{
	si4703_chan--;
	return si4703_tune_frequency(si4703_chan);
}

extern int
socle_si4703_seek_up_test(int autotest)
{
	return si4703_seek_channel(1);
}

extern int
socle_si4703_seek_down_test(int autotest)
{
	return si4703_seek_channel(0);
}

