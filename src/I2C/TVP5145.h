#ifndef _TVP5145_H_
#define _TVP5145_H_

#include <type.h>


#define	TVP5145FPF_I2C_CLIENT_ADDR		0x5c


//register addr
#define   TVP5145_INPUT_SE1_REG              0x0	 
//#define   TVP5145_OPERATOR_REG             0x2 
#define   TVP5145_OUTPUT_DATA_RATE_REG       0xd
#define   TVP5145_INPUT_SE2_REG              0x20
#define   TVP5145_MICPROC_START_REG          0x7f
#define   TVP5145_FIFO_RESET_REG             0xc9
#define   TVP5145_VDP_CLOCK_REG              0xce
#define   TVP5145_FULL_FIELD_EN_REG          0xcf 
#define   TVP5145_LINE_D0_REG                0xd0 
#define   TVP5145_LINE_D1_REG                0xd1
#define   TVP5145_LINE_D2_REG                0xd2
#define   TVP5145_LINE_D3_REG                0xd3
#define   TVP5145_LINE_D4_REG                0xd4
#define   TVP5145_LINE_D5_REG                0xd5
#define   TVP5145_LINE_D6_REG                0xd6
#define   TVP5145_LINE_D7_REG                0xd7
#define   TVP5145_LINE_D8_REG                0xd8
#define   TVP5145_LINE_D9_REG                0xd9
#define   TVP5145_LINE_Da_REG                0xda
#define   TVP5145_LINE_Db_REG                0xdb
#define   TVP5145_LINE_Dc_REG                0xdc
#define   TVP5145_LINE_Dd_REG                0xdd
#define   TVP5145_LINE_De_REG                0xde
#define   TVP5145_LINE_Df_REG                0xdf
#define   TVP5145_LINE_E0_REG                0xe0 
#define   TVP5145_LINE_E1_REG                0xe1
#define   TVP5145_LINE_E2_REG                0xe2
#define   TVP5145_LINE_E3_REG                0xe3
#define   TVP5145_LINE_E4_REG                0xe4
#define   TVP5145_LINE_E5_REG                0xe5
#define   TVP5145_LINE_E6_REG                0xe6
#define   TVP5145_LINE_E7_REG                0xe7
#define   TVP5145_LINE_E8_REG                0xe8
#define   TVP5145_LINE_E9_REG                0xe9
#define   TVP5145_LINE_Ea_REG                0xea
#define   TVP5145_LINE_Eb_REG                0xeb
#define   TVP5145_LINE_Ec_REG                0xec
#define   TVP5145_LINE_Ed_REG                0xed
#define   TVP5145_LINE_Ee_REG                0xee
#define   TVP5145_LINE_Ef_REG                0xef
#define   TVP5145_LINE_F0_REG                0xf0 
#define   TVP5145_LINE_F1_REG                0xf1
#define   TVP5145_LINE_F2_REG                0xf2
#define   TVP5145_LINE_F3_REG                0xf3
#define   TVP5145_LINE_F4_REG                0xf4
#define   TVP5145_LINE_F5_REG                0xf5
#define   TVP5145_LINE_F6_REG                0xf6
#define   TVP5145_LINE_F7_REG                0xf7
#define   TVP5145_LINE_F8_REG                0xf8
#define   TVP5145_LINE_F9_REG                0xf9
#define   TVP5145_LINE_Fa_REG                0xfa
#define   TVP5145_LINE_Fb_REG                0xfb
#define   TVP5145_FULL_FIELD_REG             0xfc


//register's value
#define   INPUTSE1_SVIDEO_2A     	      0x0
#define   INPUTSE1_COMPOSITE_2A         0x8
#define   INPUT_SVIDEO_REG20_VAL        0x2     
#define   INPUT_COMPOSITE_REG20_VAL     0x3 
#define   OUTPUT_RATE_YUV422_8BIT       0x47
#define   USE_NTSC_BT601                0xf4

extern int tvp5145I2C_init(void);

#endif //_TVP5145_H_
