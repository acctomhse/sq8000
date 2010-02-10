#include <global.h>
#include <test_item.h>
#include "tps62353.h"
#include "i2c.h"


struct i2c_client tps62353_client;
static int tps62353_type;
	
int (*tps62353_vsm_set)(struct i2c_client *client, u16 val);
int (*tps62353_vsm_get)(struct i2c_client *client);


extern void
tps62353_set_type(int type)
{
	tps62353_type = type;
}


extern struct test_item_container socle_tps62353_main_container;

extern int
tps62353_test(int autotest)
{
	int ret;
	
	tps62353_client.addr = TPS62353_I2C_CLIENT_ADDR;

	ret = tps62353_en_dcdc_set(&tps62353_client, 1);	//device enable
	if(ret == -1){
		printf("tps62353 enable fails\n");
		return -1;
	}
	
	ret = test_item_ctrl(&socle_tps62353_main_container, autotest);	
	
	return ret;
}


extern struct test_item_container socle_tps62353_voltage_container;
extern struct test_item_container socle_tps62353_regs_container;

extern int 
socle_voltage_test(int autotest)
{
	int ret=0;
	
	if(tps62353_type == 0){
		tps62353_vsm_set = tps62353_vsm0_set;
		tps62353_vsm_get = tps62353_vsm0_get;
	}else{		
		tps62353_vsm_set = tps62353_vsm1_set;
		tps62353_vsm_get = tps62353_vsm1_get;
	}

	ret = test_item_ctrl(&socle_tps62353_voltage_container, autotest);

	return ret;
}

extern int 
socle_other_test(int autotest)
{
	int ret=0;

	ret = test_item_ctrl(&socle_tps62353_regs_container, autotest);

	return ret;	

}

extern int 
socle_voltage_increase (int autotest)
{
	int ret, tmp1, tmp2;
	
	tmp1 = (*tps62353_vsm_get)(&tps62353_client);
        //printf("current voltage step multiplier : %02d\n", tmp1);

	tmp2 = tmp1 + 4;

	if(tmp2 > 63)
		tmp2 = 63;
	
	ret = (*tps62353_vsm_set)(&tps62353_client, tmp2);
	if(ret == -1){
		printf("voltage step multiplier set error\n");
		return -1;
	}

	printf("increase voltage step multiplier from %02d to %02d\n", tmp1, tmp2);
	
	return 0;
}

extern int 
socle_voltage_decrease (int autotest)
{
	int ret, tmp1, tmp2;

	tmp1 = (*tps62353_vsm_get)(&tps62353_client);
	//printf("current voltage step multiplier : %02d\n", tmp1);

	tmp2 = tmp1 - 4;

	if(tmp2 < 0)
		tmp2 = 0;
	
	ret = (*tps62353_vsm_set)(&tps62353_client, tmp2);
	if(ret == -1){
		printf("voltage step multiplier set error\n");
		return -1;
	}

	printf("decrease voltage step multiplier from %02d to %02d\n", tmp1, tmp2);
	
	return 0;
}

extern int 
socle_voltage_set (int autotest)
{
	int ret, tmp;

	tmp = (*tps62353_vsm_get)(&tps62353_client);
	//printf("current voltage step multiplier : %02d\n", tmp);
	
	printf("set voltage step multiplier (0~63) : ");
	scanf("%d\n", &tmp);
	if(tmp <0 || tmp > 63){		
		printf("error : out of range\n");
		return -1;
	}
	
	ret = (*tps62353_vsm_set)(&tps62353_client, tmp);

	if(ret == -1){
		printf("voltage step multiplier set error\n");
		return -1;
	}
	
	printf("voltage step multiplier  : %02d\n", tmp);

	return 0;
}

extern int
socle_voltage_show (int autotest)
{
        int tmp;

        tmp = (*tps62353_vsm_get)(&tps62353_client);
	if (-1 == tmp){
                return -1;
        }

        printf("current voltage step multiplier : %02d\n", tmp);

        return 0;
}


extern int 
socle_regs_vsel0_set (int autotest)
{
	int ret;
	u32 tmp;

	ret = tps62353_read_all(&tps62353_client);
	if (-1 == tmp){
                return -1;
        }

	printf("set regs vsel0 : 0x");
	scanf("%x\n", &tmp);
	ret = tps62353_vsel0_set(&tps62353_client, tmp);
	ret = tps62353_read_all(&tps62353_client);

	return ret;
}

extern int 
socle_regs_vsel1_set (int autotest)
{
	int ret;
	u32 tmp;

	ret = tps62353_read_all(&tps62353_client);
	if (-1 == tmp){
                return -1;
        }

	printf("set regs vsel1 : 0x");
	scanf("%x\n", &tmp);
	ret = tps62353_vsel1_set(&tps62353_client, tmp);
	ret = tps62353_read_all(&tps62353_client);

	return ret;
}

extern int 
socle_regs_ctrl1_set (int autotest)
{
	int ret;
	u32 tmp;

	ret = tps62353_read_all(&tps62353_client);
	if (-1 == tmp){
                return -1;
        }

	printf("set regs ctrl1 : 0x");
	scanf("%x\n", &tmp);
	ret = tps62353_ctrl1_set(&tps62353_client, tmp);
	ret = tps62353_read_all(&tps62353_client);

	return ret;
}

extern int 
socle_regs_ctrl2_set (int autotest)
{
	int ret;
	u32 tmp;

	ret = tps62353_read_all(&tps62353_client);
	if (-1 == tmp){
                return -1;
        }

	printf("set regs ctrl2 : 0x");
	scanf("%x\n", &tmp);
	ret = tps62353_ctrl2_set(&tps62353_client, tmp);
	ret = tps62353_read_all(&tps62353_client);

	return ret;
}


