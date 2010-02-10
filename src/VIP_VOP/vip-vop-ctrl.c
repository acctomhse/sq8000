#include <test_item.h>
#include <genlib.h>
#include <global.h>
#include "I2C/i2c.h"
#include "I2C/TVP5145.h"
#include "I2C/CH700X.h"
#include "vip.h"
#include "vop.h"

#if defined (CONFIG_PC9220)
#include <scu.h>
#endif

#include "LCD/lcd-ctrl.h"

#define VIP_TO_VOP 0
#define VOP_TO_VIP 1
#define VIP_TO_LCD 2
#define VOP_COLOR_DISPLAY 3

#define COLOR_BAR_V 0
#define COLOR_BAR_H 1
#define COLOR_BAR_G 2

#ifdef CONFIG_LDK3V21
#define FRAME1_ADDR_Y  0x40a00000
#define FRAME1_ADDR_CB 0x40a60000
#define FRAME1_ADDR_CR 0x40a90000
#define FRAME2_ADDR_Y  0x40b00000
#define FRAME2_ADDR_CB 0x40b60000
#define FRAME2_ADDR_CR 0x40b90000
#else
#define FRAME1_ADDR_Y  0x00a00000
#define FRAME1_ADDR_CB 0x00a60000
#define FRAME1_ADDR_CR 0x00a90000
#define FRAME2_ADDR_Y  0x00b00000
#define FRAME2_ADDR_CB 0x00b60000
#define FRAME2_ADDR_CR 0x00b90000
#endif
//...............................
#define WHITE_Y		141
#define WHITE_CB	128
#define	WHITE_CR	128
#define YELLOW_Y	162
#define YELLOW_CB	44
#define	YELLOW_CR	142
#define CYAN_Y		131
#define CYAN_CB		156
#define	CYAN_CR		44
#define GREEN_Y		112
#define GREEN_CB	72
#define	GREEN_CR	58
#define MAGENTA_Y	84
#define MAGENTA_CB	184
#define	MAGENTA_CR	198
#define RED_Y		65
#define RED_CB		100
#define	RED_CR		212
#define BLUE_Y		35
#define BLUE_CB		212
#define	BLUE_CR		114
#define	BLACK_Y		16
#define BLACK_CB	128
#define	BLACK_CR	128

unsigned char colorpatternY[] = {
  	WHITE_Y, YELLOW_Y, CYAN_Y, GREEN_Y, MAGENTA_Y, RED_Y, BLUE_Y, BLACK_Y
};
unsigned char colorpatternCb[] = {
  	WHITE_CB, YELLOW_CB, CYAN_CB, GREEN_CB, MAGENTA_CB, RED_CB, BLUE_CB, BLACK_CB
};
unsigned char colorpatternCr[] = {
  	WHITE_CR, YELLOW_CR, CYAN_CR, GREEN_CR, MAGENTA_CR, RED_CR, BLUE_CR, BLACK_CR
};
//.................................
static int format,size,output422,test_flag;
static int socle_vip_to_vop_run(void);
static int socle_vop_to_vip_run(void);
static int socle_vip_to_lcd_run(void);
static int socle_vop_color_display_run(void);
static void socle_init_color_bar(int frame_size,int type);
static void socle_init_color(int frame_size,int color_num);
extern struct test_item_container socle_vip_vop_main_container;

extern int
VIP_VOP_Testing(int autotest)
{
	int ret = 0;
#ifdef CONFIG_PC9220
	socle_scu_dev_enable(SOCLE_DEVCON_I2C);
	socle_scu_dev_enable(SOCLE_DEVCON_LCDC_VOP);
#endif
	ret = test_item_ctrl(&socle_vip_vop_main_container, autotest);
#ifdef CONFIG_PC9220
	socle_scu_dev_disable(SOCLE_DEVCON_I2C);
	socle_scu_dev_disable(SOCLE_DEVCON_LCDC_VOP);
#endif
	return ret;
}

extern struct test_item_container socle_viop_format_container;

extern int
socle_vip_to_vop_test(int autotest)
{
	int ret = 0;
	test_flag=VIP_TO_VOP;
	ret = test_item_ctrl(&socle_viop_format_container, autotest);
	return ret;
}

extern int
socle_vop_to_vip_test(int autotest)
{
	int ret = 0;
	test_flag=VOP_TO_VIP;
	ret = test_item_ctrl(&socle_viop_format_container, autotest);
	return ret;
}

extern int
socle_vip_to_lcd_test(int autotest)
{
	int ret = 0;
	test_flag=VIP_TO_LCD;
	extern struct test_item socle_viop_size_test_items[];	
	socle_viop_size_test_items[0].enable = 0;
	socle_viop_size_test_items[1].enable = 1;
	socle_viop_size_test_items[2].enable = 0;
	socle_viop_size_test_items[3].enable = 0;
	ret = test_item_ctrl(&socle_viop_format_container, autotest);
	socle_viop_size_test_items[0].enable = 1;
	socle_viop_size_test_items[1].enable = 1;
	socle_viop_size_test_items[2].enable = 1;
	socle_viop_size_test_items[3].enable = 1;
	return ret;
}

extern int
socle_vop_color_display_test(int autotest)
{
	int ret = 0;
	test_flag=VOP_COLOR_DISPLAY;
	ret = test_item_ctrl(&socle_viop_format_container, autotest);
	return ret;
}

extern struct test_item_container socle_viop_size_container;

extern int
socle_viop_ntsc_test(int autotest)
{
	int ret = 0;
	format=FORMAT_NTSC;
	ret = test_item_ctrl(&socle_viop_size_container, autotest);
	return ret;
}

extern int
socle_viop_pal_test(int autotest)
{
	int ret = 0;
	format=FORMAT_PAL;
	ret = test_item_ctrl(&socle_viop_size_container, autotest);
	return ret;
}

extern struct test_item_container socle_vip_42x_container;
extern int
socle_viop_cif_test(int autotest)
{
	int ret = 0;
	size=FRAMESIZE_CIF;
	switch(test_flag) {
		case VIP_TO_VOP:
			ret=socle_vip_to_vop_run();
			break;
		case VOP_TO_VIP:
			ret=socle_vop_to_vip_run();
			break;
		case VIP_TO_LCD:
			ret=test_item_ctrl(&socle_vip_42x_container, autotest);
			break;
		case VOP_COLOR_DISPLAY:
			ret=socle_vop_color_display_run();
			break;
	}
	return ret;
}

extern int
socle_viop_qvga_test(int autotest)
{
	int ret = 0;
	size=FRAMESIZE_QVGA;
	switch(test_flag) {
		case VIP_TO_VOP:
			ret=socle_vip_to_vop_run();
			break;
		case VOP_TO_VIP:
			ret=socle_vop_to_vip_run();
			break;
		case VIP_TO_LCD:
			ret=test_item_ctrl(&socle_vip_42x_container, autotest);
			break;
		case VOP_COLOR_DISPLAY:
			ret=socle_vop_color_display_run();
			break;
	}
	return ret;
}

extern int
socle_viop_vga_test(int autotest)
{
	int ret = 0;
	size=FRAMESIZE_VGA;
	switch(test_flag) {
		case VIP_TO_VOP:
			ret=socle_vip_to_vop_run();
			break;
		case VOP_TO_VIP:
			ret=socle_vop_to_vip_run();
			break;
		case VIP_TO_LCD:
			ret=test_item_ctrl(&socle_vip_42x_container, autotest);
			break;
		case VOP_COLOR_DISPLAY:
			ret=socle_vop_color_display_run();
			break;
	}
	return ret;
}

extern int
socle_viop_d1_test(int autotest)
{
	int ret = 0;
	size=FRAMESIZE_D1;
	switch(test_flag) {
		case VIP_TO_VOP:
			ret=socle_vip_to_vop_run();
			break;
		case VOP_TO_VIP:
			ret=socle_vop_to_vip_run();
			break;
		case VIP_TO_LCD:
			ret=test_item_ctrl(&socle_vip_42x_container, autotest);
			break;
		case VOP_COLOR_DISPLAY:
			ret=socle_vop_color_display_run();
			break;
	}
	return ret;
}

extern int socle_vip_422_test(int autotest)
{
	int ret = 0;
	output422=1;
	ret |= socle_vip_to_lcd_run();
	return ret;
	
}

extern int socle_vip_420_test(int autotest)
{
	int ret = 0;
	output422=0;
	ret |= socle_vip_to_lcd_run();
	return ret;
}

static int
socle_vip_to_vop_run(void)
{
	int ret=0;
	u8 buf;
	ret |= tvp5145I2C_init();
	ret |= ch700x_init(format);

	vipReset();
	vopReset();
	ret |= vipSetOutFormat(format);
	ret |= vopSetOutFormat(format);
	ret |= vipSetFrameSize(size);
	ret |= vopSetFrameSize(size);
	ret |= vipSetFrameCaptureAddr(FRAME1, FRAME1_ADDR_Y, FRAME1_ADDR_CB, FRAME1_ADDR_CR);
	ret |= vipSetFrameCaptureAddr(FRAME2, FRAME2_ADDR_Y, FRAME2_ADDR_CB, FRAME2_ADDR_CR);
	ret |= vopSetFrameDisplayAddr(FRAME1, FRAME1_ADDR_Y, FRAME1_ADDR_CB, FRAME1_ADDR_CR);
	ret |= vopSetFrameDisplayAddr(FRAME2, FRAME2_ADDR_Y, FRAME2_ADDR_CB, FRAME2_ADDR_CR);
	ret |= vipSetFrameMode(CONTINUOUS);
	ret |= vopSetFrameMode(TWO_FRAME);
	if(ret)
		return -1;
	vipStart();
	vopStart();
	printf("=================\n");
	printf("Displaying...\n");
	printf("Press enter to stop display...\n");
	scanf("%c",&buf);
	if(buf){
		vipStop();
		vopStop();
	}
	return ret;
}

static int
socle_vop_to_vip_run(void)
{
	int ret=0;
	socle_init_color(size,3);
	ret |= tvp5145I2C_init();
	ret |= ch700x_init(format);

	vipReset();
	vopReset();
	ret |= vopSetOutFormat(format);
	ret |= vipSetOutFormat(format);
	ret |= vopSetFrameSize(size);
	ret |= vipSetFrameSize(size);
	ret |= vopSetFrameDisplayAddr(FRAME1, FRAME1_ADDR_Y, FRAME1_ADDR_CB, FRAME1_ADDR_CR);
	ret |= vipSetFrameCaptureAddr(FRAME1, FRAME2_ADDR_Y, FRAME2_ADDR_CB, FRAME2_ADDR_CR);
	ret |= vipSetFrameMode(ONE_FRAME);
	ret |= vopSetFrameMode(ONE_FRAME);
	if(ret)
		return -1;
		
	vopStart();
	MSDELAY(5000);
	vipStart();
	MSDELAY(5000);
	vipStop();
	vopStop();
	

	return ret;
}

static int
socle_vip_to_lcd_run(void)
{
	int ret=0;
	u8 buf;
	ret |= tvp5145I2C_init();
	vipReset();
	ret |= vipSetOutFormat(format);
	ret |= vipSetFrameSize(size);
	ret |= vipSetFrameCaptureAddr(FRAME1, FRAME1_ADDR_Y, FRAME1_ADDR_CB, FRAME1_ADDR_CR);
	ret |= vipSetFrameCaptureAddr(FRAME2, FRAME1_ADDR_Y, FRAME1_ADDR_CB, FRAME1_ADDR_CR);
	ret |= vipSetFrameMode(CONTINUOUS);
	ret |= vipSet422Output(output422);
	if(ret)
		return -1;
	//init lcd
	Socle_vip_lcd_initial(output422);
	
	vipStart();
	printf("=================\n");
	printf("Displaying...\n");
	printf("Press enter to stop display...\n");
	scanf("%c",&buf);
	if(buf){
		vipStop();
		Socle_vip_lcd_stop();
	}
	return ret;
}

static int
socle_vop_color_display_run(void)
{
	int ret=0;
	u8 buf;
	
	socle_init_color_bar(size,COLOR_BAR_G);
	vopReset();
	ret |= ch700x_init(format);
	
	ret |= vopSetOutFormat(format);
	ret |= vopSetFrameSize(size);
	ret |= vopSetFrameDisplayAddr(FRAME1, FRAME1_ADDR_Y, FRAME1_ADDR_CB, FRAME1_ADDR_CR);
	ret |= vopSetFrameMode(ONE_FRAME);
	if(ret)
		return -1;
	vopStart();
	printf("=================\n");
	printf("Displaying...\n");
	printf("Press enter to stop display...\n");
	scanf("%c",&buf);
	if(buf){
		vopStop();
	}
	return ret;
}

static void
socle_init_color_bar(int frame_size,int type)
{
	  u8 *pt_Y1=(u8 *)FRAME1_ADDR_Y;
   	u8 *pt_Cb1=(u8 *)FRAME1_ADDR_CB;
  	u8 *pt_Cr1=(u8 *)FRAME1_ADDR_CR;
	  int i, j, k,l,hsize,vsize,block=16;
		switch(frame_size) {
			case FRAMESIZE_CIF:
				hsize=352;
				vsize=288;
				break;
			case FRAMESIZE_QVGA:
				hsize=320;
				vsize=240;
				break;
			case FRAMESIZE_VGA:
				hsize=640;
				vsize=480;
				break;
			case FRAMESIZE_D1:
				hsize=720;
				vsize=480;
				break;
		}
		
		if(type==COLOR_BAR_V) {
  		for (j = 0; j <vsize; j++){
	  		k = 0;
	  		for (i = hsize/block; i > 0; i--){
	      	memset(pt_Y1, colorpatternY[k], block);
	      	pt_Y1 += (block);
	      	if ((j & 0x01) == 0){
		  			memset(pt_Cb1, colorpatternCb[k], (block/2));
		  			pt_Cb1 += (block/2);
		  			memset(pt_Cr1, colorpatternCr[k], (block/2));
		  			pt_Cr1 += (block/2);
					}
	    		k=(k+1)%8;
	    	}
			}
		}
		else if(type==COLOR_BAR_H){
			k=0;
			for (j = 0; j < vsize/block; j++){
		  	memset (pt_Y1, colorpatternY[k], hsize*block);
	  		pt_Y1 +=(hsize*block);
	  		memset (pt_Cb1, colorpatternCb[k], hsize*block/4);
	  		pt_Cb1 += (hsize*block/4);
	  		memset (pt_Cr1, colorpatternCr[k], hsize*block/4);
	  		pt_Cr1 += (hsize*block/4);
	  		k=(k+1)%8;
	  	}
		}
		else {
			
    	for (j = 0; j < (vsize/block); j++){
	  		for (l = 0; l < block; l++){
	      	k = j%8;
	      	for (i = hsize/block; i > 0; i--){
		  			memset (pt_Y1, colorpatternY[k], block);
		  			pt_Y1 += block;
		  			if ((l & 0x01) == 0){
		      		memset (pt_Cb1, colorpatternCb[k], (block/2));
		      		pt_Cb1 += (block/2);
		      		memset (pt_Cr1, colorpatternCr[k], (block/2));
		     	 		pt_Cr1 += (block/2);
		   			}
		  			k=(k+1)%8;
					}
	  		}
			}
		}
}

static void
socle_init_color(int frame_size,int color_num)
{
	u8 *pt_Y1=(u8 *)FRAME1_ADDR_Y;
  u8 *pt_Cb1=(u8 *)FRAME1_ADDR_CB;
  u8 *pt_Cr1=(u8 *)FRAME1_ADDR_CR;
	int hsize,vsize;
	switch(frame_size) {
			case FRAMESIZE_CIF:
				hsize=352;
				vsize=288;
				break;
			case FRAMESIZE_QVGA:
				hsize=320;
				vsize=240;
				break;
			case FRAMESIZE_VGA:
				hsize=640;
				vsize=480;
				break;
			case FRAMESIZE_D1:
				hsize=720;
				vsize=480;
				break;
	}
	memset(pt_Y1, colorpatternY[color_num], hsize*vsize);
  memset(pt_Cb1, colorpatternCb[color_num], hsize*vsize/4);
  memset(pt_Cr1, colorpatternCr[color_num], hsize*vsize/4);
}











