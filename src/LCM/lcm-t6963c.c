#include <global.h>
#include <genlib.h>
#include <platform.h>
#include <test_item.h>

#include "dependency.h"
#include "t6963c-regs.h"

static int t6963c_database = T6963C_BASE;

static inline u8
t6963c_read_data_port(void)
{
	return ioread16(t6963c_database);
}

static inline void
t6963c_write_data_port(u8 data)
{
	iowrite16(data, t6963c_database);
}

static inline u8
t6963c_read_cmd_port(void)
{
	return ioread16(t6963c_database + 2);
}

static inline void
t6963c_write_cmd_port(u8 data)
{
	iowrite16(data, t6963c_database + 2);
}

static inline int
t6963c_check_status(u8 flag)
{
#if 0
	int t = 0;

	while (!(t6963c_read_cmd_port() & flag)) {
		t++;
		if (t > 0x1000) {
			printf("t6963c_check_status timoe out!\n");
			return -1;
		}
	}
//#else
//	MSDELAY(1);
#endif
	return 0;
}

static inline int
t6963c_write_command(u8 cmd)
{
	if (t6963c_check_status(STATUS_EXEC_CAP | STATUS_DATA_RW_CAP))
		return -1;
	t6963c_write_cmd_port(cmd);
	return 0;
}

static inline int
t6963c_write_data_8(u8 data)
{
	if (t6963c_check_status(STATUS_EXEC_CAP | STATUS_DATA_RW_CAP))
		return -1;
	t6963c_write_data_port(data);
	return 0;
}

static inline int
t6963c_write_data_16(u16 data)
{
	// low byte
	if (t6963c_check_status(STATUS_EXEC_CAP | STATUS_DATA_RW_CAP))
		return -1;
	t6963c_write_data_port(data & 0xff);

	// high byte
	if (t6963c_check_status(STATUS_EXEC_CAP | STATUS_DATA_RW_CAP))
		return -1;
	t6963c_write_data_port((data & 0xff00) >> 8);

	return 0;
}

static inline int
t6963c_auto_write_data(u8 data)
{
	if (t6963c_check_status(STATUS_AUTO_DATA_W_CAP))
		return -1;
	t6963c_write_data_port(data);
	return 0;
}

static inline int
t6963c_write_single_display_data(u16 addr, u8 data, int num)
{
	int i;

	// set address pointer
	if (t6963c_write_data_16(addr))
		return -1;
	if (t6963c_write_command(CMD_SET_ADR_PTR))
		return -1;

	if (t6963c_write_command(CMD_SET_DATA_AUTO_W))
		return -1;

	for (i = 0; i < num; i++)
		if (t6963c_auto_write_data(data))
			return -1;

	if (t6963c_write_command(CMD_SET_AUTO_RESET))
		return -1;

	return 0;
}

static inline int
t6963c_write_multi_display_data(u16 addr, u8 *data, int num)
{
	int i;

	//printf("t6963c_write_multi_display_data(): addr=0x%04x, num=%d\n", addr, num);

	// set address pointer
	if (t6963c_write_data_16(addr))
		return -1;
	if (t6963c_write_command(CMD_SET_ADR_PTR))
		return -1;

	if (t6963c_write_command(CMD_SET_DATA_AUTO_W))
		return -1;

	for (i = 0; i < num; i++)
		if (t6963c_auto_write_data(data[i]))
			return -1;

	if (t6963c_write_command(CMD_SET_AUTO_RESET))
		return -1;

	return 0;
}

static inline int
t6963c_write_multi_display_char(u16 addr, u8 *data, int num)
{
	int i;

	//printf("t6963c_write_multi_display_char(): addr=0x%04x, num=%d\n", addr, num);

	// set address pointer
	if (t6963c_write_data_16(addr))
		return -1;
	if (t6963c_write_command(CMD_SET_ADR_PTR))
		return -1;

	if (t6963c_write_command(CMD_SET_DATA_AUTO_W))
		return -1;

#ifdef T6963C_REVERSE
	for (i = 0; i < num; i++)
		if (t6963c_auto_write_data(data[num - i - 1] + 0x80 - CHAR_SPACE))
			return -1;
#else
	for (i = 0; i < num; i++)
		if (t6963c_auto_write_data(data[i] - CHAR_SPACE))
			return -1;
#endif

	if (t6963c_write_command(CMD_SET_AUTO_RESET))
		return -1;

	return 0;
}

static inline int
t6963c_write_ext_char_gen_data(u8 *data, int base, int num)
{
	// write external character genrator data
	if (t6963c_write_multi_display_data(CG_RAM_ADDR(base), data, num))
		return -1;
	return 0;
}

static inline int
t6963c_write_attribute_data(int line, int column, int num, int attr_data)
{
#ifdef T6963C_REVERSE
	if (t6963c_write_single_display_data(GRA_HOME_ADDR + (TXT_LINE - line - 1) * COLUMN + (COLUMN - column - 1) - num, attr_data, num))
		return -1;
#else
	if (t6963c_write_single_display_data(GRA_HOME_ADDR + line * COLUMN + column, attr_data, num))
		return -1;
#endif
	return 0;
}

static inline int
t6963c_write_string_data(int line, int column, u8 *str, int len)
{
#ifdef T6963C_REVERSE
	if (t6963c_write_multi_display_char(TXT_HOME_ADDR + (TXT_LINE - line - 1) * COLUMN + (COLUMN - column - 1) - len, str, len))
		return -1;
#else
	if (t6963c_write_multi_display_char(TXT_HOME_ADDR + line * COLUMN + column, str, len))
		return -1;
#endif
	return 0;
}

static int
socle_lcm_init(void)
{
	// set text home address TXT_HOME_ADDR
	if (t6963c_write_data_16(TXT_HOME_ADDR))
		return -1;
	if (t6963c_write_command(CMD_SET_TXT_HOM_ADR))
		return -1;

	// set graphic home address GRA_HOME_ADDR
	if (t6963c_write_data_16(GRA_HOME_ADDR))
		return -1;
	if (t6963c_write_command(CMD_SET_GRA_HOM_ADR))
		return -1;

	// set text area COLUMN columns
	if (t6963c_write_data_16(COLUMN))
		return -1;
	if (t6963c_write_command(CMD_SET_TXT_ARE))
		return -1;

	// set graphic area COLUMN columns
	if (t6963c_write_data_16(COLUMN))
		return -1;
	if (t6963c_write_command(CMD_SET_GRA_ARE))
		return -1;

	// set "xor mode", "internal character generater mode"
	if (t6963c_write_command(CMD_SET_TXT_ATT_MODE))	//CMD_SET_XOR_MODE
		return -1;

	// set offset register
	if (t6963c_write_data_16(OFFSET_REG_DATA))
		return -1;
	if (t6963c_write_command(CMD_SET_OFF_REG))
		return -1;

	// set display mode
	if (t6963c_write_command(CMD_SET_TXT_ON_GRA_ON))
		return -1;

	// clear all code
	if (t6963c_write_single_display_data(TXT_HOME_ADDR, 0x00, COLUMN * TXT_LINE))
		return -1;
	if (t6963c_write_single_display_data(GRA_HOME_ADDR, 0x00, COLUMN * GRA_LINE))
		return -1;

	return 0;
}

#if 0
static int
socle_lcm_t6963c_test_sub(void)
{
	int i;

	if (socle_lcm_init())
		return -1;

	if (t6963c_write_ext_char_gen_data(extcg, 0x80, DATA_NUM(extcg)))
		return -1;

	// write text diaplay data (internal CG)
		// set address pointer 1 line, 1 column
	if (t6963c_write_multi_display_data(TXT_HOME_ADDR + 1 * COLUMN + 1, intcg1, DATA_NUM(intcg1)))
		return -1;

	// write text diaplay data (internal CG)
		// set address pointer 3 line, 2 column
	if (t6963c_write_multi_display_data(TXT_HOME_ADDR + 3 * COLUMN + 2, intcg2, DATA_NUM(intcg2)))
		return -1;

	// write text diaplay data (externel CG upper part)
		// set address pointer 5 line, 8 column
	if (t6963c_write_multi_display_data(TXT_HOME_ADDR + 5 * COLUMN + 8, expart1, DATA_NUM(expart1)))
		return -1;

	// write text diaplay data (externel CG lower part)
		// set address pointer 6 line, 8 column
	if (t6963c_write_multi_display_data(TXT_HOME_ADDR + 6 * COLUMN + 8, expart2, DATA_NUM(expart2)))
		return -1;

	for (i = 0; i < 5; i++) {
		// write text diaplay data (1 ~ 5)
			// set address pointer 6 line, 4 column
		if (t6963c_write_single_display_data(TXT_HOME_ADDR + 6 * COLUMN + 4, 0x11 + i, 1))
			return -1;

		if (t6963c_write_single_display_data(GRA_HOME_ADDR, 0x3f, COLUMN * GRA_LINE))
			return -1;
		MSDELAY(100);

		if (t6963c_write_single_display_data(GRA_HOME_ADDR, 0x00, COLUMN * GRA_LINE))
			return -1;
		MSDELAY(100);
	}

	return 0;
}
#endif

static int
socle_lcm_t6963c_print_bar(int line, int bar)
{
	if ((line < 0) || (line >= TXT_LINE))
		return -1;

	if (bar) {
		if (t6963c_write_attribute_data(line, 0, COLUMN - 1, REVERSE_DISP))
			return -1;
	} else {
		if (t6963c_write_attribute_data(line, 0, COLUMN - 1, NORMAL_DISP))
			return -1;
	}

	return 0;
}

static int
socle_lcm_t6963c_print_str_with_attr(int line, int column, u8 *str, int attr_data)
{
	int len;

	if ((line < 0) || (column < 0) || (line >= TXT_LINE) || (column >= COLUMN))
		return -1;

	len = strlen(str);
	if ((column + len) >= COLUMN)
		len = COLUMN - column - 1;

	if (t6963c_write_attribute_data(line, column, len, attr_data))
		return -1;

	if (t6963c_write_string_data(line, column, str, len))
		return -1;

	return 0;
}

#ifdef T6963C_REVERSE
static int
socle_lcm_t6963c_draw_line_with_attr(int line, int column, int vertical, int len, int attr_data)
{
	int i;
	char sq[2]= "";

	if ((line < 0) || (column < 0) || (line >= TXT_LINE) || (column >= COLUMN))
		return -1;

	if (vertical) {
		if ((len < 0) || (len > TXT_LINE))
			return -1;

		if ((line + len) > TXT_LINE)
			len = TXT_LINE - line;

		sq[0] = RECTANGLE_V;
		for (i = 0; i < len; i++) {
			if (socle_lcm_t6963c_print_str_with_attr(line + i, column, sq, attr_data))
				return -1;
		}
	} else {
		if ((len < 0) || (len >= COLUMN))
			return -1;

		if ((column + len) >= COLUMN)
			len = COLUMN - column - 1;

		sq[0] = RECTANGLE_H;
		for (i = 0; i < len; i++) {
			if (socle_lcm_t6963c_print_str_with_attr(line, column + i, sq, attr_data))
				return -1;
		}
	}

	return 0;
}

static int
socle_lcm_t6963c_draw_rectangle_with_attr(int line, int column, int w, int h, int attr_data)
{
	int i;
	char sq[2]= "";

	if ((line < 0) || (column < 0) || (line >= TXT_LINE) || (column >= COLUMN))
		return -1;
	if ((w < 2) || (h < 2) || (w >= COLUMN) || (h > TXT_LINE))
		return -1;

	if ((line + h) > TXT_LINE)
		h = TXT_LINE - line;
	if ((column + w) >= COLUMN)
		w = COLUMN - column - 1;

	sq[0] = RECTANGLE_U_L;
	if (socle_lcm_t6963c_print_str_with_attr(line, column, sq, attr_data))
		return -1;
	sq[0] = RECTANGLE_H;
	for (i = 0; i < (w - 2); i++) {
		if (socle_lcm_t6963c_print_str_with_attr(line, column + 1 + i, sq, attr_data))
			return -1;
	}
	sq[0] = RECTANGLE_U_R;
	if (socle_lcm_t6963c_print_str_with_attr(line, column + w - 1, sq, attr_data))
		return -1;

	for (i = 0; i < (h - 2); i++) {
		sq[0] = RECTANGLE_V;
		if (socle_lcm_t6963c_print_str_with_attr(line + 1 + i, column, sq, attr_data))
			return -1;
		sq[0] = RECTANGLE_V;
		if (socle_lcm_t6963c_print_str_with_attr(line + 1 + i, column + w - 1, sq, attr_data))
			return -1;
	}

	sq[0] = RECTANGLE_L_L;
	if (socle_lcm_t6963c_print_str_with_attr(line + h -1, column, sq, attr_data))
		return -1;
	sq[0] = RECTANGLE_H;
	for (i = 0; i < (w - 2); i++) {
		if (socle_lcm_t6963c_print_str_with_attr(line + h -1, column + 1 + i, sq, attr_data))
			return -1;
	}
	sq[0] = RECTANGLE_L_R;
	if (socle_lcm_t6963c_print_str_with_attr(line + h -1, column + w - 1, sq, attr_data))
		return -1;

	return 0;
}
#endif

static int
socle_lcm_t6963c_test_sub(void)
{
	int i, j;

	if (socle_lcm_init())
		return -1;

#ifdef T6963C_REVERSE
	if (t6963c_write_ext_char_gen_data(ascii_table, 0x80, DATA_NUM(ascii_table)))
		return -1;
#endif
	
	if (socle_lcm_t6963c_print_str_with_attr(1, 1, "Diagnostic  Program", BLINK_NORMAL_DISP))
		return -1;
	if (socle_lcm_t6963c_print_str_with_attr(3, 2, "Socle Tech. Corp.", NORMAL_DISP))
		return -1;

	for (i = 0; i < 3; i++) {
		if (socle_lcm_t6963c_print_bar(i + 5, 1))
			return -1;

		for (j = 0; j < 16; j++) {
			char word[2] = "";
			word[0] = '_';
			if (t6963c_write_string_data(i + 5, j + 2, word, 1))
				return -1;
			MSDELAY(100);

			word[0] = 0x30 + i * 16 + j;
			if (t6963c_write_string_data(i + 5, j + 2, word, 1))
				return -1;
		}

		if (socle_lcm_t6963c_print_bar(i + 5, 0))
			return -1;
	}

	for (i = 0; i < 3; i++) {
		if (socle_lcm_t6963c_print_str_with_attr(i + 5, 2, "                ", NORMAL_DISP))
			return -1;
	}

	//flash
	for (i = 0; i < 4; i++) {
		if (socle_lcm_t6963c_print_bar(3, 1))
			return -1;
		MSDELAY(100);

		if (socle_lcm_t6963c_print_bar(3, 0))
			return -1;
		MSDELAY(100);
	}
	if (socle_lcm_t6963c_print_bar(3, 1))
		return -1;

#ifdef T6963C_REVERSE
	if (socle_lcm_t6963c_draw_rectangle_with_attr(0, 0, COLUMN - 1, 5, NORMAL_DISP))
		return -1;
	if (socle_lcm_t6963c_draw_line_with_attr(2, 1, 0, COLUMN - 3,  NORMAL_DISP))
		return -1;
#endif

	if (socle_lcm_t6963c_print_str_with_attr(5, 0, "www.socle-tech.com.tw", REVERSE_DISP))
		return -1;
	if (socle_lcm_t6963c_print_str_with_attr(6, 1, "TEL : 886-3-5163166", REVERSE_DISP))
		return -1;
	if (socle_lcm_t6963c_print_str_with_attr(7, 1, "FAX : 886-3-5163177", REVERSE_DISP))
		return -1;

	if (t6963c_write_attribute_data(3, 1, COLUMN - 4, BLINK_REVERSE_DISP))
		return -1;

	return 0;
}




#include "../GPIO/gpio.h"

extern int
socle_lcm_t6963c_test(int autotest)
{
	int ret = 0;

#ifdef CONFIG_PC7210
// 20080714 cyli add for INR
#include "../ADC_PWM/pwmt-regs.h"
#include "../GPIO/gpio.h"

	struct socle_pwmt *p;

	// backlight
	p = get_socle_pwmt_structure(T6963C_BKLGT_PWM_IDX);
	if (NULL == p) {
		printf("socle_lcm_t6963c_test() can't get PWMT structure!!\n");
		return -1;
	}

	p->drv->reset(p);
	p->drv->write_hrc(p, 0x50);
	p->drv->write_lrc(p, 0x1000);
	p->drv->enable(p, 1);
	p->drv->output_enable(p, 1);

	release_socle_pwmt_structure(T6963C_BKLGT_PWM_IDX);

	// contrast
	p = get_socle_pwmt_structure(T6963C_CNTRT_PWM_IDX);
	if (NULL == p) {
		printf("socle_lcm_t6963c_test() can't get PWMT structure!!\n");
		return -1;
	}

	p->drv->reset(p);
	p->drv->write_hrc(p, 0x50);
	p->drv->write_lrc(p, 0x50);
	p->drv->enable(p, 1);
	p->drv->output_enable(p, 1);

	release_socle_pwmt_structure(T6963C_CNTRT_PWM_IDX);

	// set CS0 chip selection to LCM
	socle_gpio_set_value_with_mask(PA, 0, SHIFT_MASK(5));				// PA5 = 0
#endif

	ret = socle_lcm_t6963c_test_sub();

#ifdef CONFIG_PC7210
	// set CS0 chip selection to NOR
	socle_gpio_set_value_with_mask(PA, SHIFT_MASK(5), SHIFT_MASK(5));				// PA5 = 1
#endif

	return ret;
}




#include "../I2S/codec-ctrl.h"
static int vol = MAX_VOLUME / 2;
static int vol_change = 0;
static void
volume_tuner_isr (void *pparam)
{
	int clockwise = socle_gpio_get_value_with_mask(PF, 0x20);

	if (clockwise)
		vol++;
	else
		vol--;

	if (vol > MAX_VOLUME)
		vol = MAX_VOLUME;
	if (vol < 0)
		vol = 0;

	vol_change = 1;
	printf("volume_tuner_isr(): vol = %d\n", vol);
}


extern int
inr_pc7230_line_in_test(int autotest)
{
	if (audio_dac_initialize()) {
		printf("audio_dac_initialize fail\n");
		return -1;
	}

	if (socle_request_gpio_irq(SET_GPIO_PIN_NUM(PF, 4), volume_tuner_isr, GPIO_INT_SENSE_EDGE | GPIO_INT_SINGLE_EDGE | GPIO_INT_EVENT_HI, NULL)) {
		printf("I2STesting(): GPIO pin[%d] is busy!\n", SET_GPIO_PIN_NUM(PF, 4));
		return -1;
	}

	// set audio source selection to Line-in
	socle_gpio_set_value_with_mask(PE, 0x1a, 0x3f);				// PE[1:0] = [10]
	socle_gpio_set_value_with_mask(PE, 0x1e, 0x3f);				// PE[1:0] = [10], PE2 = 1

	while (1) {
		if (vol_change) {
			audio_dac_master_volume(vol);
			vol_change = 0;
		}
	}
	return 0;
}

extern int
inr_pc7230_ipod_ducking_test(int autotest)
{
	if (audio_dac_initialize()) {
		printf("audio_dac_initialize fail\n");
		return -1;
	}

	if (socle_request_gpio_irq(SET_GPIO_PIN_NUM(PF, 4), volume_tuner_isr, GPIO_INT_SENSE_EDGE | GPIO_INT_SINGLE_EDGE | GPIO_INT_EVENT_HI, NULL)) {
		printf("I2STesting(): GPIO pin[%d] is busy!\n", SET_GPIO_PIN_NUM(PF, 4));
		return -1;
	}

	// set audio source selection to iPod
	socle_gpio_set_value_with_mask(PE, 0x19, 0x3f);				// PE[1:0] = [01]
	socle_gpio_set_value_with_mask(PE, 0x39, 0x3f);				// PE[1:0] = [01], PE5 = 1

	while (1) {
		if (vol_change) {
			audio_dac_master_volume(vol);
			vol_change = 0;
		}
	}
	return 0;
}

static void
iPod_detection_isr (void *pparam)
{
	int plug = socle_gpio_get_value_with_mask(PA, 0x10);

	if (plug)
		printf("iPod plug\n");
	else
		printf("iPod unplug\n");
}

static void
line_in_detection_isr (void *pparam)
{
	int plug = socle_gpio_get_value_with_mask(PF, 0x80);

	if (plug)
		printf("Line-in plug\n");
	else
		printf("Line-in unplug\n");
}

extern int
inr_pc7230_ipod_and_line_in_detection_test(int autotest)
{
	if (socle_request_gpio_irq(SET_GPIO_PIN_NUM(PA, 4), iPod_detection_isr, GPIO_INT_SENSE_EDGE | GPIO_INT_BOTH_EDGE, NULL)) {
		printf("inr_pc7230_ipod_and_line_in_detection_test(): GPIO pin[%d] is busy!\n", SET_GPIO_PIN_NUM(PA, 4));
		return -1;
	}

	if (socle_request_gpio_irq(SET_GPIO_PIN_NUM(PF, 7), line_in_detection_isr, GPIO_INT_SENSE_EDGE | GPIO_INT_BOTH_EDGE, NULL)) {
		printf("inr_pc7230_ipod_and_line_in_detection_test(): GPIO pin[%d] is busy!\n", SET_GPIO_PIN_NUM(PF, 7));
		return -1;
	}
	return 0;
}

