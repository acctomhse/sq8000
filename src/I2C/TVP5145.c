#include "TVP5145.h"
#include "i2c.h"
#include "dependency.h"

struct i2c_client tvp5145_client;

static int inline
tvp5145_write(u8 reg, u8 val)
{
	int ret;
	u8 buf[2];

	buf[0] = reg;
	buf[1] = val;
	ret = i2c_master_send(&tvp5145_client, buf, 2);
	if (ret != 2)
		return -1;
	else
		return 0;
}

static int inline
tvp5145_read(u8 reg)
{
	int ret;
	struct i2c_msg msg[2];
	u8 buf = reg;
	u8 ret_buf;
	int ret_val;
       
	memset((void *)msg, 0x00, 2*sizeof(struct i2c_msg));
	msg[0].addr = tvp5145_client.addr;
	msg[1].addr = tvp5145_client.addr;
	msg[0].buf = &buf;
	msg[0].len = 1;
	msg[1].flags = I2C_M_RD;
	msg[1].buf = &ret_buf;
	msg[1].len = 1;
	ret = i2c_transfer(msg, 2);
	if (ret != 2)
		return -1;
	ret_val = ret_buf;
	return ret_val;
}

const char tvp5145fpf_initset[] =		// Reg Addr, set value
{
	
	TVP5145_MICPROC_START_REG,            1,
	TVP5145_FIFO_RESET_REG,               1,

  TVP5145_INPUT_SE1_REG,				        INPUTSE1_COMPOSITE_2A,
	TVP5145_INPUT_SE2_REG,                INPUT_COMPOSITE_REG20_VAL,
	
	TVP5145_OUTPUT_DATA_RATE_REG,         OUTPUT_RATE_YUV422_8BIT,
	TVP5145_LINE_D0_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_D1_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_D2_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_D3_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_D4_REG,                  USE_NTSC_BT601,
  TVP5145_LINE_D5_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_D6_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_D7_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_D8_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_D9_REG,                  USE_NTSC_BT601,
  TVP5145_LINE_Da_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_Db_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_Dc_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_Dd_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_De_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_Df_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_E0_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_E1_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_E2_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_E3_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_E4_REG,                  USE_NTSC_BT601,
  TVP5145_LINE_E5_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_E6_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_E7_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_E8_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_E9_REG,                  USE_NTSC_BT601,
  TVP5145_LINE_Ea_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_Eb_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_Ec_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_Ed_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_Ee_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_Ef_REG,                  USE_NTSC_BT601,
  TVP5145_LINE_F0_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_F1_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_F2_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_F3_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_F4_REG,                  USE_NTSC_BT601,
  TVP5145_LINE_F5_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_F6_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_F7_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_F8_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_F9_REG,                  USE_NTSC_BT601,
  TVP5145_LINE_Fa_REG,                  USE_NTSC_BT601,
	TVP5145_LINE_Fb_REG,                  USE_NTSC_BT601,
	TVP5145_FULL_FIELD_REG,               USE_NTSC_BT601,
	//TVP5145_VDP_CLOCK_REG,1
};

extern int
tvp5145I2C_init(void)
{
	int ret=0;
	printf("init tvp5145\n");
	i2c_master_initialize(SOCLE_APB0_I2C0, SOCLE_INTC_I2C0);
	tvp5145_client.addr = TVP5145FPF_I2C_CLIENT_ADDR;
	/*
	int i;
	for(i=0;i<sizeof(tvp5145fpf_initset);i=i+2)
	{		
		ret |= tvp5145_write(tvp5145fpf_initset[i],tvp5145fpf_initset[i+1]);
		MSDELAY(50);
	}
	*/
	//for full field reg test
	
	ret |=tvp5145_write(TVP5145_MICPROC_START_REG,1);
	MSDELAY(50);
	ret |=tvp5145_write(TVP5145_FIFO_RESET_REG,1);
	MSDELAY(50);
	ret |=tvp5145_write(TVP5145_INPUT_SE1_REG,INPUTSE1_COMPOSITE_2A);
	MSDELAY(50);
	ret |=tvp5145_write(TVP5145_INPUT_SE2_REG,INPUT_COMPOSITE_REG20_VAL);
	MSDELAY(50);
	ret |=tvp5145_write(TVP5145_OUTPUT_DATA_RATE_REG,OUTPUT_RATE_YUV422_8BIT);
	MSDELAY(50);
	//ret |=tvp5145_write(TVP5145_VDP_CLOCK_REG,1);
	//MSDELAY(50);
	ret |=tvp5145_write(TVP5145_FULL_FIELD_EN_REG,1);
	MSDELAY(50);
	ret |=tvp5145_write(TVP5145_FULL_FIELD_REG,USE_NTSC_BT601);
	MSDELAY(50);
	
	printf("Reg TVP5145_INPUT_SE1_REG = %x\n",tvp5145_read(TVP5145_INPUT_SE1_REG));
	printf("Reg TVP5145_INPUT_SE2_REG = %x\n",tvp5145_read(TVP5145_INPUT_SE2_REG));
	printf("Reg TVP5145_OUTPUT_DATA_RATE_REG = %x\n",tvp5145_read(TVP5145_OUTPUT_DATA_RATE_REG));
	printf("Reg TVP5145_FULL_FIELD_EN_REG = %x\n",tvp5145_read(TVP5145_FULL_FIELD_EN_REG));
	printf("Reg TVP5145_FULL_FIELD_REG = %x\n",tvp5145_read(TVP5145_FULL_FIELD_REG));
	printf("Reg TVP5145_VDP_CLOCK_REG = %x\n",tvp5145_read(TVP5145_VDP_CLOCK_REG));
	
	printf("init tvp5145 finish\n");
  return ret;
}
