#ifndef _lcd_ctrl_h_included_
#define _lcd_ctrl_h_included_

int Socle_Color_16BPP_FSFill (int color , u32 color_base);
int Socle_LUT_Fill(int index , u32 color_base);
int Socle_ColorFSFill (u32 color , u32 color_base);
extern void Socle_vip_lcd_initial(int output422);
extern void Socle_vip_lcd_stop(void);
#endif // _lcd_ctrl_h_included_
