#include "tps62353.h"
#include "../I2C/i2c.h"
#include <type.h>
#include <genlib.h>



static int inline
tps62353_write(struct i2c_client *client, u8 reg, u8 val)
{
	int ret;
	u8 buf[2];

	buf[0] = reg;
	buf[1] = val;
	
	//printf("buf[0] = 0x%x, buf[1] = 0x%x\n", buf[0], buf[1]);
	ret = i2c_master_send(client, buf, 2);
	
	//printf("tps62353_write ret = 0x%x\n", ret);
	if (ret != 2)
		return -1;
	else
		return 0;
}

static int inline
tps62353_read(struct i2c_client *client, u8 reg)
{
	int ret;
	struct i2c_msg msg[2];
	u8 buf = reg;
	u8 ret_buf;
	int ret_val;
       
	memset((void *)msg, 0x00, 2*sizeof(struct i2c_msg));
	msg[0].addr = client->addr;
	msg[0].buf = &buf;
	msg[0].len = 1;
	
	msg[1].addr = client->addr;
	msg[1].flags = I2C_M_RD;
	msg[1].buf = &ret_buf;
	msg[1].len = 1;
	ret = i2c_transfer(msg, 2);
	
			//printf("tps62353_read---ret = %x\n", ret);
	if (ret != 2)
		return -1;
	ret_val = ret_buf;

	//printf("tps62353_read : ret_val = 0x%x\n", ret_val);
	
	return ret_val;
}


extern int
tps62353_read_all(struct i2c_client *client)
{
	int tmp;

	tmp =  tps62353_read(client, TPS62353_VSEL0);
        if (-1 == tmp){
                printf("Reading TPS6235 is fail, reg: 0x%02x\n", TPS62353_VSEL0);
                return -1;
        }
	printf("TPS62353_VSEL0 = %x\n", tmp);

	tmp =  tps62353_read(client, TPS62353_VSEL1);
        if (-1 == tmp){
                printf("Reading TPS6235 is fail, reg: 0x%02x\n", TPS62353_VSEL1);
                return -1;
        }
	printf("TPS62353_VSEL1 = %x\n", tmp);

	tmp =  tps62353_read(client, TPS62353_CTRL1);
        if (-1 == tmp){
                printf("Reading TPS6235 is fail, reg: 0x%02x\n", TPS62353_CTRL1);
                return -1;
        }
        printf("TPS62353_CTRL1 = %x\n", tmp);	

	tmp =  tps62353_read(client, TPS62353_CTRL2);
        if (-1 == tmp){
                printf("Reading TPS6235 is fail, reg: 0x%02x\n", TPS62353_CTRL2);
                return -1;
        }
        printf("TPS62353_CTRL2 = %x\n", tmp);

	return 0;
}

extern int
tps62353_vsm0_set(struct i2c_client *client, u16 val)
{
	int ret, tmp;

	printf("tps62353_vsm0_set--read\n");
	tmp = tps62353_read(client, TPS62353_VSEL0);
	if (-1 == tmp){
                printf("Reading TPS6235 is fail, reg: 0x%02x\n", TPS62353_VSEL0);
                return -1;
        }

	tmp = tmp & ~TPS62353_VSEL0_VOLT_STEP_MULT_MASK;

	tmp = tmp | (val & TPS62353_VSEL0_VOLT_STEP_MULT_MASK);

	
	//printf("tps62353_vsm0_set--write\n");

	ret = tps62353_write(client, TPS62353_VSEL0, tmp);

	if (-1 == ret)
		printf("Writing TPS6235 is fail, reg: 0x%02x\n", TPS62353_VSEL0);
	//else
	//	printf("Writing TPS6235 is pass, reg: 0x%02x, val: 0x%02x\n", TPS62353_VSEL0, tmp);

	return ret;
}

extern int
tps62353_vsm0_get(struct i2c_client *client)
{
	
	int tmp;

	tmp = tps62353_read(client, TPS62353_VSEL0);
	if (-1 == tmp){
		printf("Reading TPS6235 is fail, reg: 0x%02x\n", TPS62353_VSEL0);
		return -1;
	}
	//else
	//	printf("Reading TPS6235 is pass, reg: 0x%02x\n", TPS62353_VSEL0);
	
	tmp = tmp & TPS62353_VSEL0_VOLT_STEP_MULT_MASK;

	return tmp;
}

extern int
tps62353_vsm1_set(struct i2c_client *client, u16 val)
{
	int ret, tmp;

	printf("tps62353_vsm1_set : val = %x\n", val);
	tmp = tps62353_read(client, TPS62353_VSEL1);
	if (-1 == tmp){
                printf("Reading TPS6235 is fail, reg: 0x%02x\n", TPS62353_VSEL1);
                return -1;
        }

	//printf("tps62353_vsm1_set : tmp1 = %x\n", tmp);
	
	tmp = tmp & ~TPS62353_VSEL1_VOLT_STEP_MULT_MASK;
	//printf("tps62353_vsm1_set : tmp2 = %x\n", tmp);
	
	tmp = tmp | (val & TPS62353_VSEL1_VOLT_STEP_MULT_MASK);
	//printf("tps62353_vsm1_set : tmp3 = %x\n", tmp);

	ret = tps62353_write(client, TPS62353_VSEL1, tmp);

	if (-1 == ret)
		printf("Writing TPS6235 is fail, reg: 0x%02x\n", TPS62353_VSEL1);
	//else
	//	printf("Writing TPS6235 is pass, reg: 0x%02x, val: 0x%02x\n", TPS62353_VSEL1, tmp);

	return ret;
}

extern int
tps62353_vsm1_get(struct i2c_client *client)
{
	int tmp;

	tmp = tps62353_read(client, TPS62353_VSEL1);
	if (-1 == tmp){
		printf("Reading TPS6235 is fail, reg: 0x%02x\n", TPS62353_VSEL1);
		return -1;
	}
	//else
	//	printf("Reading TPS6235 is pass, reg: 0x%02x\n", TPS62353_VSEL1);
	
	tmp = tmp & TPS62353_VSEL1_VOLT_STEP_MULT_MASK;

	return tmp;
}

extern int
tps62353_lightpfm_optimize_set(struct i2c_client *client, int par)
{
	int ret, tmp;

	tmp = tps62353_read(client, TPS62353_VSEL0);
	if (-1 == tmp){
                printf("Reading TPS6235 is fail, reg: 0x%02x\n", TPS62353_VSEL0);
                return -1;
        }

	if(LIGHTPFM_OPTIMIZE_1 == par)
		tmp = tmp | TPS62353_VSEL0_LIGHTPFM_OPTIMIZE_1;
	else
		tmp = tmp & ~TPS62353_VSEL0_LIGHTPFM_OPTIMIZE_1;

	ret = tps62353_write(client, TPS62353_VSEL0, tmp);

	if (-1 == ret)
		printf("Writing TPS6235 is fail, reg: 0x%02x\n", TPS62353_VSEL0);
	//else
	//	printf("Writing TPS6235 is pass, reg: 0x%02x, val: 0x%02x\n", TPS62353_VSEL0, tmp);

	return ret;
}

extern int
tps62353_en_dcdc_set(struct i2c_client *client, int en)		//1:enable	0:disable
{
	int ret, tmp;

	tmp = tps62353_read(client, TPS62353_VSEL0);
	if (-1 == tmp){
                printf("Reading TPS6235 is fail, reg: 0x%02x\n", TPS62353_VSEL0);
                return -1;
        }

	if(en == 1)
		tmp = tmp | TPS62353_VSEL0_EN_DCDC_EN;
	else
		tmp = tmp & ~TPS62353_VSEL0_EN_DCDC_EN;

	ret = tps62353_write(client, TPS62353_VSEL0, tmp);

	if (-1 == ret)
		printf("Writing TPS6235 is fail, reg: 0x%02x\n", TPS62353_VSEL0);
	//else
	//	printf("Writing TPS6235 is pass, reg: 0x%02x, val: 0x%02x\n", TPS62353_VSEL0, tmp);

	return ret;
}


extern int
tps62353_ctrl1_mode0_set(struct i2c_client *client, int par)
{
	int ret, tmp;

	tmp = tps62353_read(client, TPS62353_CTRL1);
	if (-1 == tmp){
                printf("Reading TPS6235 is fail, reg: 0x%02x\n", TPS62353_CTRL1);
                return -1;
        }

	if(MODE0_FAST_PFM == par)
		tmp = tmp & ~TPS62353_CTRL1_MODE0_FAST_PFM;
	else		
		tmp = tmp | TPS62353_CTRL1_MODE0_FAST_PFM;
	
	ret = tps62353_write(client, TPS62353_CTRL1, tmp);

	if (-1 == ret)
		printf("Writing TPS6235 is fail, reg: 0x%02x\n", TPS62353_CTRL1);
	//else
	//	printf("Writing TPS6235 is pass, reg: 0x%02x, val: 0x%02x\n", TPS62353_CTRL1, tmp);

	return ret;
}

extern int
tps62353_ctrl1_mode1_set(struct i2c_client *client, int par)
{
	int ret, tmp;

	tmp = tps62353_read(client, TPS62353_CTRL1);
	if (-1 == tmp){
                printf("Reading TPS6235 is fail, reg: 0x%02x\n", TPS62353_CTRL1);
                return -1;
        }

	if(MODE1_FORCED_PWM == par)
		tmp = tmp & ~TPS62353_CTRL1_MODE1_FAST_PFM;
	else		
		tmp = tmp | TPS62353_CTRL1_MODE1_FAST_PFM;
	
	ret = tps62353_write(client, TPS62353_CTRL1, tmp);

	if (-1 == ret)
		printf("Writing TPS6235 is fail, reg: 0x%02x\n", TPS62353_CTRL1);
	//else
	//	printf("Writing TPS6235 is pass, reg: 0x%02x, val: 0x%02x\n", TPS62353_CTRL1, tmp);

	return ret;
}

extern int
tps62353_ctrl1_mode_ctrl_set(struct i2c_client *client, int par)
{
	int ret, tmp;

	tmp = tps62353_read(client, TPS62353_CTRL1);
        if (-1 == tmp){
                printf("Reading TPS6235 is fail, reg: 0x%02x\n", TPS62353_CTRL1);
                return -1;
        }

	tmp = tmp & ~TPS62353_CTRL1_MODE_CTRL_MASK;

	switch(par){
		case MODE_CTRL_MODE01 :
			tmp = tmp | TPS62353_CTRL1_MODE_CTRL_MODE01;
			break;
		case MODE_CTRL_LIGHT_PFM :
			tmp = tmp | TPS62353_CTRL1_MODE_CTRL_LIGHT_PFM;
			break;
		case MODE_CTRL_FORCED_PWM:
			tmp = tmp | TPS62353_CTRL1_MODE_CTRL_FORCED_PWM;
			break;
		case MODE_CTRL_FAST_PFM:
			tmp = tmp | TPS62353_CTRL1_MODE_CTRL_FAST_PFM;
			break;
		default :
			printf("unknown parameter\n");
			return -1;
			break;
	}
	
	ret = tps62353_write(client, TPS62353_CTRL1, tmp);

	if (-1 == ret)
		printf("Writing TPS6235 is fail, reg: 0x%02x\n", TPS62353_CTRL1);
	//else
	//	printf("Writing TPS6235 is pass, reg: 0x%02x, val: 0x%02x\n", TPS62353_CTRL1, tmp);

	return ret;
}

extern int
tps62353_ctrl1_hw_nsw_set(struct i2c_client *client, int par)
{
	int ret, tmp;

	tmp = tps62353_read(client, TPS62353_CTRL1);
        if (-1 == tmp){
                printf("Reading TPS6235 is fail, reg: 0x%02x\n", TPS62353_CTRL1);
                return -1;
        }

	if(HW_nSW_SW== par)
		tmp = tmp & ~TPS62353_CTRL1_HW_nSW_HW;
	else		
		tmp = tmp | TPS62353_CTRL1_HW_nSW_HW;
	
	ret = tps62353_write(client, TPS62353_CTRL1, tmp);

	if (-1 == ret)
		printf("Writing TPS6235 is fail, reg: 0x%02x\n", TPS62353_CTRL1);
	//else
	//	printf("Writing TPS6235 is pass, reg: 0x%02x, val: 0x%02x\n", TPS62353_CTRL1, tmp);

	return ret;
}

extern int
tps62353_ctrl1_en_sync_set(struct i2c_client *client, int en)
{
	int ret, tmp;

	tmp = tps62353_read(client, TPS62353_CTRL1);
        if (-1 == tmp){
                printf("Reading TPS6235 is fail, reg: 0x%02x\n", TPS62353_CTRL1);
                return -1;
        }

	if(en == 1)
		tmp = tmp | TPS62353_CTRL1_EN_SYNC_EN;
	else		
		tmp = tmp & ~TPS62353_CTRL1_EN_SYNC_EN;
	
	ret = tps62353_write(client, TPS62353_CTRL1, tmp);

	if (-1 == ret)
		printf("Writing TPS6235 is fail, reg: 0x%02x\n", TPS62353_CTRL1);
	//else
	//	printf("Writing TPS6235 is pass, reg: 0x%02x, val: 0x%02x\n", TPS62353_CTRL1, tmp);

	return ret;
}

extern int
tps62353_ctrl2_defslew_set(struct i2c_client *client, int par)
{
	int ret, tmp;

	tmp = tps62353_read(client, TPS62353_CTRL2);
        if (-1 == tmp){
                printf("Reading TPS6235 is fail, reg: 0x%02x\n", TPS62353_CTRL2);
                return -1;
        }

	tmp = tmp & ~TPS62353_CTRL2_DEFSLEW_MASK;

	switch(par){
		case DEFSLEW_0_15:
			tmp = tmp | TPS62353_CTRL2_DEFSLEW_0_15;
			break;
		case DEFSLEW_0_3:
			tmp = tmp | TPS62353_CTRL2_DEFSLEW_0_3;
			break;
		case DEFSLEW_0_6:
			tmp = tmp | TPS62353_CTRL2_DEFSLEW_0_6;
			break;
		case DEFSLEW_1_2:
			tmp = tmp | TPS62353_CTRL2_DEFSLEW_1_2;
			break;
		case DEFSLEW_2_4:
			tmp = tmp | TPS62353_CTRL2_DEFSLEW_2_4;
			break;
		case DEFSLEW_4_8:
			tmp = tmp | TPS62353_CTRL2_DEFSLEW_4_8;
			break;
		case DEFSLEW_9_6:
			tmp = tmp | TPS62353_CTRL2_DEFSLEW_9_6;
			break;
		case DEFSLEW_IMMED:
			tmp = tmp | TPS62353_CTRL2_DEFSLEW_IMMED;
			break;
		default :
			printf("unknown defslew parameter\n");
			return -1;
			break;
	}
	
	
	ret = tps62353_write(client, TPS62353_CTRL2, tmp);

	if (-1 == ret)
		printf("Writing TPS6235 is fail, reg: 0x%02x\n", TPS62353_CTRL2);
	//else
	//	printf("Writing TPS6235 is pass, reg: 0x%02x, val: 0x%02x\n", TPS62353_CTRL2, tmp);

	return ret;
}

extern int
tps62353_ctrl2_pll_mult_set(struct i2c_client *client, int par)
{
	int ret, tmp;

	tmp = tps62353_read(client, TPS62353_CTRL2);
        if (-1 == tmp){
                printf("Reading TPS6235 is fail, reg: 0x%02x\n", TPS62353_CTRL2);
                return -1;
        }

	tmp = tmp & ~TPS62353_CTRL2_PLL_MASK;

	switch(par){
		case PLL_MULT_1:
			tmp = tmp | TPS62353_CTRL2_PLL_MULT_1;
			break;
		case PLL_MULT_2:
			tmp = tmp | TPS62353_CTRL2_PLL_MULT_2;
			break;
		case PLL_MULT_3:
			tmp = tmp | TPS62353_CTRL2_PLL_MULT_3;
			break;
		case PLL_MULT_4:
			tmp = tmp | TPS62353_CTRL2_PLL_MULT_4;
			break;
		default :
			printf("unknown pll mult parameter\n");
			return -1;
			break;
	}
	
	
	ret = tps62353_write(client, TPS62353_CTRL2, tmp);

	if (-1 == ret)
		printf("Writing TPS6235 is fail, reg: 0x%02x\n", TPS62353_CTRL2);
	//else
	//	printf("Writing TPS6235 is pass, reg: 0x%02x, val: 0x%02x\n", TPS62353_CTRL2, tmp);

	return ret;
}

extern int
tps62353_ctrl2_pwrok_get(struct i2c_client *client)		//return 1:within its nominal range   0:below its target regulation voltage. 
{
	int ret, tmp;

	tmp = tps62353_read(client, TPS62353_CTRL2);
        if (-1 == tmp){
                printf("Reading TPS6235 is fail, reg: 0x%02x\n", TPS62353_CTRL2);
                return -1;
        }

	tmp = tmp & TPS62353_CTRL2_PWROK_WITHIN_RANGE;

	if(TPS62353_CTRL2_PWROK_WITHIN_RANGE == tmp)
		ret = 1;
	else
		ret = 0;
	
	return ret;
}

extern int
tps62353_ctrl2_output_discharge_act_set(struct i2c_client *client, int act)		
{
	int ret, tmp;

	tmp = tps62353_read(client, TPS62353_CTRL2);
        if (-1 == tmp){
                printf("Reading TPS6235 is fail, reg: 0x%02x\n", TPS62353_CTRL2);
                return -1;
        }

	if(act == 1)
		tmp = tmp | TPS62353_CTRL2_OUTPUT_DISCHARGE_ACT;
	else
		tmp = tmp & ~TPS62353_CTRL2_OUTPUT_DISCHARGE_ACT;
		
	ret = tps62353_write(client, TPS62353_CTRL2, tmp);

	if (-1 == ret)
		printf("Writing TPS6235 is fail, reg: 0x%02x\n", TPS62353_CTRL2);
	//else
	//	printf("Writing TPS6235 is pass, reg: 0x%02x, val: 0x%02x\n", TPS62353_CTRL2, tmp);

	return ret;
}

extern int
tps62353_ctrl2_go(struct i2c_client *client)		
{
	int ret, tmp;

	tmp = tps62353_read(client, TPS62353_CTRL2);
        if (-1 == tmp){
                printf("Reading TPS6235 is fail, reg: 0x%02x\n", TPS62353_CTRL2);
                return -1;
        }

	tmp = tmp | TPS62353_CTRL2_GO_EN;
		
	ret = tps62353_write(client, TPS62353_CTRL2, tmp);

	if (-1 == ret)
		printf("Writing TPS6235 is fail, reg: 0x%02x\n", TPS62353_CTRL2);
	//else
	//	printf("Writing TPS6235 is pass, reg: 0x%02x, val: 0x%02x\n", TPS62353_CTRL2, tmp);

	return ret;
}


extern int
tps62353_vsel0_set(struct i2c_client *client, u32 val)
{
	int ret;

	ret = tps62353_write(client, TPS62353_VSEL0, val);

	if (-1 == ret)
		printf("Writing TPS6235 is fail, reg: 0x%02x\n", TPS62353_VSEL0);
	//else
	//	printf("Writing TPS6235 is pass, reg: 0x%02x, val: 0x%02x\n", TPS62353_VSEL0, val);

	return ret;
}

extern int
tps62353_vsel1_set(struct i2c_client *client, u32 val)
{
	int ret;

	ret = tps62353_write(client, TPS62353_VSEL1, val);

	if (-1 == ret)
		printf("Writing TPS6235 is fail, reg: 0x%02x\n", TPS62353_VSEL1);
	//else
	//	printf("Writing TPS6235 is pass, reg: 0x%02x, val: 0x%02x\n", TPS62353_VSEL1, val);

	return ret;
}

extern int
tps62353_ctrl1_set(struct i2c_client *client, u32 val)
{
	int ret;

	ret = tps62353_write(client, TPS62353_CTRL1, val);

	if (-1 == ret)
		printf("Writing TPS6235 is fail, reg: 0x%02x\n", TPS62353_CTRL1);
	//else
	//	printf("Writing TPS6235 is pass, reg: 0x%02x, val: 0x%02x\n", TPS62353_CTRL1, val);

	return ret;
}

extern int
tps62353_ctrl2_set(struct i2c_client *client, u32 val)
{
	int ret;

	ret = tps62353_write(client, TPS62353_CTRL2, val);

	if (-1 == ret)
		printf("Writing TPS6235 is fail, reg: 0x%02x\n", TPS62353_CTRL2);
	//else
	//	printf("Writing TPS6235 is pass, reg: 0x%02x, val: 0x%02x\n", TPS62353_CTRL2, val);

	return ret;
}



