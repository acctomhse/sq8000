#include <global.h>
#include <genlib.h>
#include <platform.h>
#include <test_item.h>

#ifdef CONFIG_PC9220
#include <GPIO/gpio.h>
#else
#include <MP_GPIO/mp-gpio.h>
#include <MP_GPIO/regs-mp-gpio.h>
#endif


#define PATTERN_BASE		(0xa00000)
#define BMP_BASE			(0xb00000)
#define EINK_DELAY			(1000)

#ifdef CONFIG_PC9220
static int eink_database = 0x11000000;
#else
static int eink_database = 0x10000000;
#endif


// define pin
#ifdef CONFIG_PC9220

#define EINK_SET_WUP()				socle_gpio_set_value_with_mask(PN, SHIFT_MASK(4), SHIFT_MASK(4))
#define EINK_CLR_WUP()				socle_gpio_set_value_with_mask(PN, 0, SHIFT_MASK(4))

#define EINK_SET_CD_CMD()			socle_gpio_set_value_with_mask(PN, SHIFT_MASK(5), SHIFT_MASK(5)) 
#define EINK_SET_CD_DAT()			socle_gpio_set_value_with_mask(PN, 0, SHIFT_MASK(5))

#define EINK_SET_RW_WR()			socle_gpio_set_value_with_mask(PN, 0, SHIFT_MASK(7))
#define EINK_SET_RW_RD()			socle_gpio_set_value_with_mask(PN, SHIFT_MASK(7), SHIFT_MASK(7))


#else


#ifdef CONFIG_MP_GPIO

#define EINK_SET_WUP()				socle_mp_gpio_set_port_num_value(PO, 7, 1)
#define EINK_CLR_WUP()				socle_mp_gpio_set_port_num_value(PO, 7, 0)

#define EINK_SET_CD_CMD()			socle_mp_gpio_set_port_num_value(PP, 1, 1)
#define EINK_SET_CD_DAT()			socle_mp_gpio_set_port_num_value(PP, 1, 0)

#define EINK_SET_DS()				socle_mp_gpio_set_port_num_value(PP, 3, 0)
#define EINK_CLR_DS()				socle_mp_gpio_set_port_num_value(PP, 3, 1)

#define EINK_SET_ACK_INPUT()		socle_mp_gpio_set_port_num_direction(PP, 5, 0)
#define EINK_GET_ACK()				(socle_mp_gpio_get_port_value(PP) & 0x20)

#define EINK_SET_RW_WR()			socle_mp_gpio_set_port_num_value(PP, 7, 0)
#define EINK_SET_RW_RD()			socle_mp_gpio_set_port_num_value(PP, 7, 1)

#else

#warning No define for EINK control, suggest with CONFIG_MP_GPIO

#define EINK_SET_WUP()
#define EINK_CLR_WUP()

#define EINK_SET_CD_CMD()
#define EINK_SET_CD_DAT()

#define EINK_SET_RW_WR()
#define EINK_SET_RW_RD()

#endif	//CONFIG_MP_GPIO

#endif

// define command
#define  dc_NewImage          0xA0
#define  dc_StopNewImage      0xA1
#define  dc_DisplayImage      0xA2
#define  dc_PartialImage      0xB0
#define  dc_DisplayPartial    0xB1
#define  dc_Reset             0xEE
#define  dc_SetDepth          0xF3
#define  dc_EraseDisplay      0xA3
#define  dc_Rotate            0xF5
#define  dc_Positive          0xF7
#define  dc_Negative          0xF8
#define  dc_GoToNormal        0xF0
#define  dc_GoToSleep         0xF1
#define  dc_GoToStandBy       0xF2
#define  dc_WriteToFlash      0x01
#define  dc_ReadFromFlash     0x02
#define  dc_Init              0xA4
#define  dc_AutoRefreshOn     0xF9
#define  dc_AutoRefreshOff    0xFA
#define  dc_SetRefresh        0xFB
#define  dc_ForcedRefresh     0xFC
#define  dc_GetRefresh        0xFD
#define  dc_RestoreImage      0xA5
#define  dc_ControllerVersion 0xE0
#define  dc_SoftwareVersion   0xE1
#define  dc_DisplaySize       0xE2
#define  dc_GetStatus         0xAA
#define  dc_Temperature       0x21
#define  dc_WriteRegister     0x10
#define  dc_ReadRegister      0x11
#define  dc_Abort             0xA1



static inline u8
eink_read(void)
{
	return ioread8(eink_database);
}

static inline void
eink_write(u8 data)
{
	iowrite8(data, eink_database);
}



static void
eink_wake_up(void)
{
	EINK_SET_WUP();

	// only for DS = 0
	eink_write(0x0);
	EINK_CLR_WUP();
}

static void
eink_write_command(u8 cmd)
{
	EINK_SET_CD_CMD();
	EINK_SET_RW_WR();

	eink_write(cmd);
//	EINK_SET_DS();
//	EINK_CLR_DS();
}

static void
eink_write_data(u8 data)
{
	EINK_SET_CD_DAT();
	EINK_SET_RW_WR();

	eink_write(data);
//	EINK_SET_DS();
//	EINK_CLR_DS();
}

static void
eink_read_data(u8 *data)
{
//	EINK_SET_CD_DAT();
	EINK_SET_RW_RD();

//	EINK_SET_DS();
	*data = eink_read();
//	EINK_CLR_DS();
}

static inline void
eink_initial(void)
{
	eink_write_command(dc_EraseDisplay);
	eink_write_data(0x1);
	MSDELAY(EINK_DELAY);
}

static void
convert_bmp_to_image(u8 *bmp, u8 *image)
{
	int max_x = 600, max_y = 800, i, j;
	u8 tmp, *img = image + 0x20000;

//	printf("convert_bmp_to_image: form 0x%08x to 0x%08x, tmp=0x%08x\n", bmp, image, img);

	// cut header
	bmp = bmp + 118;

	// convert from 4bits to 2 bits
	for (i = 0; i < (max_x * max_y / 2); i++) {
		if (!(i%2)) {
//			tmp = (bmp[i] & 0x03) | ((bmp[i] & 0x30) >> 2);
//			tmp = ((bmp[i] & 0x0c) >> 2) | ((bmp[i] & 0xc0) >> 4);

//			tmp = (((bmp[i] & 0x0c) >> 2) | ((bmp[i] & 0xc0) >> 4)) << 4;
			tmp = ((bmp[i] & 0x03) | ((bmp[i] & 0x30) >> 2)) << 4;
		} else {
//			tmp |= ((bmp[i] & 0x03) | ((bmp[i] & 0x30) >> 2)) << 4;
//			tmp |= (((bmp[i] & 0x0c) >> 2) | ((bmp[i] & 0xc0) >> 4)) << 4;

//			tmp |= ((bmp[i] & 0x0c) >> 2) | ((bmp[i] & 0xc0) >> 4);
			tmp |= (bmp[i] & 0x03) | ((bmp[i] & 0x30) >> 2);
			img[i / 2] = tmp;
		}
	}

	// reverse bmp
	for (i = 0; i < (max_y); i++)
		for (j = 0; j < (max_x / 4); j++)
			*image++ = *(img + (max_y - i - 1) * (max_x / 4) + j);

}

static void
eink_update_image(u8 *image, int size)
{
	int i;

	//printf("eink_update_image: base=0x%08x, size=%d\n", image, size);
	eink_initial();
	eink_write_command(dc_ForcedRefresh);
	eink_write_command(dc_NewImage);

	EINK_SET_CD_DAT();
	EINK_SET_RW_WR();

	for (i = 0; i < size; i++) {
		iowrite8(image[i], eink_database);
		//MSDELAY(2);
		//eink_write(0);
		//eink_write(image[i]);
}
	eink_write_command(dc_StopNewImage);	
	eink_write_command(dc_DisplayImage);
	
}



static int
eink_bar_test(int level)
{
	int i, size;
	u8 *pattern = (u8 *)(PATTERN_BASE), data;

	eink_initial();

	if (2 == level) {
		// Black/White
		size = 60000;
		eink_write_command(dc_SetDepth);
		eink_write_data(0x0);
	} else if (4 == level) {
		// 4 level gray scale
		size = 120000;
		eink_write_command(dc_SetDepth);
		eink_write_data(0x2);
	} else
		return -1;

	// fill pattern
	for (i = 0; i < size; i++) {
		if (2 == level) {
			// Black/White
			if (i < (size/2))
				pattern[i] = 0x00;
			else
				pattern[i] = 0xff;
		} else if (4 == level) {
			// 4 level gray scale
			if (i < (size/4))
				pattern[i] = 0x00;
			else if ((i >= (size/4)) && (i < (size/4*2)))
				pattern[i] = 0x55;
			else if ((i >= (size/4*2)) && (i < (size/4*3)))
				pattern[i] = 0xaa;
			else
				pattern[i] = 0xff;
		}
	}

	// rotation
	for (i = 0; i < 4; i++) {
		eink_write_command(dc_Rotate);
		eink_write_data(i);
		eink_update_image(pattern, size);
		MSDELAY(EINK_DELAY);
	}

  eink_write_command(dc_GetStatus);
	eink_read_data(&data);
	printf("Get Status = 0x%02x\n", data);
	return 0;
}


extern struct test_item_container socle_eink_main_container;

extern int
socle_lcm_eink_test(int autotest)
{
	return test_item_ctrl(&socle_eink_main_container, autotest);
}


extern int
eink_2_level_bar_test(int autotest)
{
	return eink_bar_test(2);
}

extern int
eink_4_level_bar_test(int autotest)
{
	return eink_bar_test(4);
}

extern int
eink_4_level_image_test(int autotest)
{
	int img_base = BMP_BASE + 0x20000, size = 120000;
	u8 data;

	printf("Loading 4 level BMP from 0x%08x...\n", BMP_BASE);

	eink_initial();

	// 4 level gray scale
	eink_write_command(dc_SetDepth);
	eink_write_data(0x2);
	MSDELAY(EINK_DELAY);

	eink_write_command(dc_Rotate);
	eink_write_data(0x1);
	MSDELAY(EINK_DELAY);

	eink_write_command(dc_GetStatus);
	eink_read_data(&data);
	printf("Get Status = 0x%02x\n", data);

	convert_bmp_to_image((u8 *)BMP_BASE, (u8 *)img_base);

	eink_update_image((u8 *)img_base, size);

	return 0;
}

