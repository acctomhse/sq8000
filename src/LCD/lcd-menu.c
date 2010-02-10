#include <test_item.h>
#include "dependency.h"

extern int yuv_color_fill_black(int autotest);
extern int yuv_color_fill_red(int autotest);
extern int yuv_color_fill_green(int autotest);
extern int yuv_color_fill_blue(int autotest);
extern int yuv_color_fill_cyan(int autotest);
extern int yuv_color_fill_magenta(int autotest);
extern int yuv_color_fill_yellow(int autotest);
extern int yuv_color_fill_white(int autotest);
extern int yuv_color_bar_h_fill(int autotest);
extern int yuv_color_bar_v_fill(int autotest);
extern int yuv_color_bar_grid_fill(int autotest);

struct test_item yuv_color_fill_test_items[] = {
	{"black", 
	 yuv_color_fill_black,
	 1,
	 1},
	{"red", 
	 yuv_color_fill_red,
	 1,
	 1},
	 {"green", 
	 yuv_color_fill_green,
	 1,
	 1},
	 {"blue", 
	 yuv_color_fill_blue,
	 1,
	 1},
	 {"cyan", 
	 yuv_color_fill_cyan,
	 1,
	 1},
	 {"magenta", 
	 yuv_color_fill_magenta,
	 1,
	 1},
	 {"yellow", 
	 yuv_color_fill_yellow,
	 1,
	 1},
	 {"white", 
	 yuv_color_fill_white,
	 1,
	 1},
	 {"Color Bar horizontal", 
	 yuv_color_bar_h_fill,
	 1,
	 1},
	 {"Color Bar Vertical", 
	 yuv_color_bar_v_fill,
	 1,
	 1},
	 {"Color Grid", 
	 yuv_color_bar_grid_fill,
	 1,
	 1},
};

struct test_item_container yuv_color_fill_test_container = {
	.menu_name = "CLCD YUV COLOR FILL Menu",
	.shell_name = "clcd_yuv color",
	.items = yuv_color_fill_test_items,
	.test_item_size = sizeof(yuv_color_fill_test_items)
};

extern int lcd_color_fill(int autotest);
extern int lcd_inter(int autotest);

struct test_item lcd_intr_test_items[] = {
	{"color", 
	 lcd_color_fill,
	 1,
	 1},
	{"intr", 
	 lcd_inter,
	 1,
	 1},
};

struct test_item_container lcd_intr_test_container = {
	.menu_name = "LCD INTR Menu",
	.shell_name = "clcd_intr",
	.items = lcd_intr_test_items,
	.test_item_size = sizeof(lcd_intr_test_items)
};

extern int color_fill_black(int autotest);
extern int color_fill_red(int autotest);
extern int color_fill_green(int autotest);
extern int color_fill_blue(int autotest);
extern int color_fill_cyan(int autotest);
extern int color_fill_magenta(int autotest);
extern int color_fill_yellow(int autotest);
extern int color_fill_white(int autotest);

struct test_item color_fill_test_items[] = {
	{"black", 
	 color_fill_black,
	 1,
	 1},
	{"red", 
	 color_fill_red,
	 1,
	 1},
	 {"green", 
	 color_fill_green,
	 1,
	 1},
	 {"blue", 
	 color_fill_blue,
	 1,
	 1},
	 {"cyan", 
	 color_fill_cyan,
	 1,
	 1},
	 {"magenta", 
	 color_fill_magenta,
	 1,
	 1},
	 {"yellow", 
	 color_fill_yellow,
	 1,
	 1},
	 {"white", 
	 color_fill_white,
	 1,
	 1},
};

struct test_item_container color_fill_test_container = {
	.menu_name = "CLCD COLOR FILL Menu",
	.shell_name = "clcd_color",
	.items = color_fill_test_items,
	.test_item_size = sizeof(color_fill_test_items)
};

extern int bpp16_lum0_test(int autotest);
extern int bpp16_lumr_test(int autotest);
extern int bpp16_lumg_test(int autotest);
extern int bpp16_lumb_test(int autotest);
extern int bpp16_lumrgb_test(int autotest);

struct test_item socle_lcd_16bpp_lum_test_items[] = {
	{"LUM 0", 
	 bpp16_lum0_test,
	 1,
	 1},
	{"LUM R", 
	 bpp16_lumr_test,
	 1,
	 1},
	{"LUM G", 
	 bpp16_lumg_test,
	 1,
	 1},
	{"LUM B", 
	 bpp16_lumb_test,
	 1,
	 1},
	{"LUM RGB", 
	 bpp16_lumrgb_test,
	 1,
	 1},
	 
};

struct test_item_container socle_lcd_16bpp_lum_test_container = {
	.menu_name = "16bpp lum",
	.shell_name = "16bpp lum",
	.items = socle_lcd_16bpp_lum_test_items,
	.test_item_size = sizeof(socle_lcd_16bpp_lum_test_items)
};

extern int bpp16_red_test(int autotest);
extern int bpp16_green_test(int autotest);
extern int bpp16_blue_test(int autotest);
extern int bpp16_lum_test(int autotest);

struct test_item socle_lcd_16bpp_test_items[] = {
	{"Red Mode", 
	 bpp16_red_test,
	 1,
	 1},
	{"Green Mode", 
	 bpp16_green_test,
	 1,
	 1},
	{"Blue Mode", 
	 bpp16_blue_test,
	 1,
	 1},
	{"LUM Mode", 
	 bpp16_lum_test,
	 1,
	 1},
};

struct test_item_container socle_lcd_controller_16bpp_test_container = {
	.menu_name = "SOCLE LCD 16 BPP Ctrl Menu",
	.shell_name = "socle_bpp16",
	.items = socle_lcd_16bpp_test_items,
	.test_item_size = sizeof(socle_lcd_16bpp_test_items)
};

extern int yuv_single_frame_test(int autotest);
extern int yuv_double_frame_test(int autotest);

struct test_item socle_lcd_yuv_frame_items[] = {
	{"YUV Single Frame", 
	 yuv_single_frame_test,
	 1,
	 1},
	{"YUV double Frame", 
	 yuv_double_frame_test,
	 1,
	 1},
};

struct test_item_container socle_lcd_yuv_frame_test_container = {
	.menu_name = "SOCLE LCD YUV Frame",
	.shell_name = "socle_lcd",
	.items = socle_lcd_yuv_frame_items,
	.test_item_size = sizeof(socle_lcd_yuv_frame_items)
};

extern int single_frame_test(int autotest);
extern int double_frame_test(int autotest);

struct test_item socle_lcd_test_items[] = {
	{"Single Frame", 
	 single_frame_test,
	 1,
	 1},
	{"double Frame", 
	 double_frame_test,
	 1,
	 1},
};

struct test_item_container socle_lcd_controller_test_container = {
	.menu_name = "SOCLE LCD Ctrl Menu",
	.shell_name = "socle_lcd",
	.items = socle_lcd_test_items,
	.test_item_size = sizeof(socle_lcd_test_items)
};

extern int tft_lut_test(int autotest);
extern int tft_16bpp_test(int autotest);
extern int tft_24bpp_test(int autotest);

struct test_item socle_lcd_bpp_items[] = {
	{"TFT LUT", 
	 tft_lut_test,
	 1,
	 1},
	{"TFT 16 BPP", 
	 tft_16bpp_test,
	 1,
	 1},
	{"TFT 24 BPP", 
	 tft_24bpp_test,
	 1,
	 1},
};

struct test_item_container socle_lcd_controller_bpp_container = {
	.menu_name = "SOCLE LCD Bpp Menu",
	.shell_name = "socle_lcd_bpp",
	.items = socle_lcd_bpp_items,
	.test_item_size = sizeof(socle_lcd_bpp_items)
};

extern int lcd_yuv420_mode_test(int autotest);
extern int lcd_yuv422_mode_test(int autotest);

struct test_item socle_lcd_yuv_items[] = {
	{"LCD YUV420 format display", 
	 lcd_yuv420_mode_test,
	 1,
	 1},
	{"LCD YUV422 format display", 
	 lcd_yuv422_mode_test,
	 1,
	 1},
};

struct test_item_container socle_lcd_controller_yuv_container = {
	.menu_name = "SOCLE LCD YUV Menu",
	.shell_name = "socle_lcd_yuv format",
	.items = socle_lcd_yuv_items,
	.test_item_size = sizeof(socle_lcd_yuv_items)
};


extern int lcd_rgb_mode_test(int autotest);
extern int lcd_yuv_mode_test(int autotest);

struct test_item socle_lcd_format_items[] = {
	{"LCD RGB format display", 
	 lcd_rgb_mode_test,
	 1,
	 1},
	{"LCD YUV format display", 
	 lcd_yuv_mode_test,
	 LCD_YUV_MODE_TEST,
	 LCD_YUV_MODE_TEST},
};

struct test_item_container socle_lcd_controller_format_container = {
	.menu_name = "SOCLE LCD Bpp Menu",
	.shell_name = "socle_lcd_bpp",
	.items = socle_lcd_format_items,
	.test_item_size = sizeof(socle_lcd_format_items)
};


