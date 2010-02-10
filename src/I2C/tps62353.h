#ifndef _TPS62353_H_
#define _TPS62353_H_

#include <type.h>
#include "../I2C/i2c.h"

#define	TPS62353_I2C_CLIENT_ADDR		0x48

#define	TPS62353_VSEL0 		0x00	//VSEL0 register (r/w)
#define	TPS62353_VSEL1 		0x01	//VSEL1 register (r/w)
#define	TPS62353_CTRL1 		0x10	//CONTROL1 register (r/w)
#define	TPS62353_CTRL2 		0x11	//CONTROL22 register (r/w)


/*	
	TPS62353_VSEL0
*/
/*	VOLTAGE STEP MULTIPLIER, VSM0  --  6-bit unsigned binary linear coding.Code effective from 0 to 63 decimal	*/
#define	TPS62353_VSEL0_VOLT_STEP_MULT_MASK		0x3f		// 6-bit unsigned binary linear coding. Code effective from 0 to 63 decimal
/*	LIGHTPFM OPTIMIZE  --  This bit is internally mapped by VSEL1[6]. Writing a value in VSEL0[6] automatically updates VSEL1[6].	*/
#define	TPS62353_VSEL0_LIGHTPFM_OPTIMIZE_22		0			// LightPFM optimized for 2.2-uH inductor
#define	TPS62353_VSEL0_LIGHTPFM_OPTIMIZE_1		(1 << 6)		// LightPFM optimized for 1-uH inductor (default)
/*	EN_DCDC  --  This bit is internally mapped by VSEL1[6]. Writing a value in VSEL0[6] automatically updates VSEL1[6].	*/
#define	TPS62353_VSEL0_EN_DCDC_DIS				0			// Device in shutdown regardless of EN signal
#define	TPS62353_VSEL0_EN_DCDC_EN				(1 << 7)		// Device enabled when EN pin tied high (default)

/*	
	TPS62353_VSEL1
*/
/*	VOLTAGE STEP MULTIPLIER, VSM0  --  6-bit unsigned binary linear coding.Code effective from 0 to 63 decimal	*/
#define	TPS62353_VSEL1_VOLT_STEP_MULT_MASK		0x3f		// 6-bit unsigned binary linear coding. Code effective from 0 to 63 decimal
/*	LIGHTPFM OPTIMIZE  --  This bit is internally mapped by VSEL0[6]. Writing a value in VSEL1[6] automatically updates VSEL0[6].	*/
#define	TPS62353_VSEL1_LIGHTPFM_OPTIMIZE_22		0			// LightPFM optimized for 2.2-uH inductor
#define	TPS62353_VSEL1_LIGHTPFM_OPTIMIZE_1		(1 << 6)		// LightPFM optimized for 1-uH inductor (default)
/*	EN_DCDC  --  This bit is internally mapped by VSEL0[7]. Writing a value in VSEL1[7] automatically updates VSEL0[7].	*/
#define	TPS62353_VSEL1_EN_DCDC_DIS				0			// Device in shutdown regardless of EN signal
#define	TPS62353_VSEL1_EN_DCDC_EN				(1 << 7)		// Device enabled when EN pin tied high (default) 


/*	
	TPS62353_CONTROL1
*/
/*	MODE0 -- This bit defines the mode of operation for VSEL low	*/
#define	TPS62353_CTRL1_MODE0_LIGHT_PFM		0			//Light PFM with auto. transition to PWM (default)
#define	TPS62353_CTRL1_MODE0_FAST_PFM		(1 << 0)		//Fast PFM with auto. transition to PWM
/*	MODE1 -- This bit defines the mode of operation for VSEL high	*/
#define	TPS62353_CTRL1_MODE1_FORCED_PWM	0			//Forced PWM (default)
#define	TPS62353_CTRL1_MODE1_FAST_PFM		(1 << 1)		//Fast PFM with auto. transition to PWM
/*	MODE_CTRL	*/
#define	TPS62353_CTRL1_MODE_CTRL_MODE01		0			//Operation follows MODE0, MODE1 (default)
#define	TPS62353_CTRL1_MODE_CTRL_LIGHT_PFM		(1 << 2)		//Light PFM with auto. transition to PWM (VSEL independent)
#define	TPS62353_CTRL1_MODE_CTRL_FORCED_PWM	(2 << 2)		//Light PFM with auto. transition to PWM (VSEL independent)
#define	TPS62353_CTRL1_MODE_CTRL_FAST_PFM		(3 << 2)		//Fast PFM with auto. transition to PWM (VSEL independent)
#define	TPS62353_CTRL1_MODE_CTRL_MASK			(3 << 2)		//mask
/*	HW_nSW		*/
#define	TPS62353_CTRL1_HW_nSW_SW		0			//Output voltage controlled by software to the value defined in VSEL1.
#define	TPS62353_CTRL1_HW_nSW_HW		(1 << 4)		//Output voltage controlled by VSEL pin (default)
/*	EN_SYNC		*/
#define	TPS62353_CTRL1_EN_SYNC_DIS		0			//Disable synchronization to external clock signal (default)
#define	TPS62353_CTRL1_EN_SYNC_EN		(1 << 5)		//Enable synchronization to external clock signal


/*	
	TPS62353_CONTROL2
*/
/*	DEFSLEW -- DEFSLEW defines the output voltage ramp rate	*/
#define	TPS62353_CTRL2_DEFSLEW_0_15		0			//0.15 mV/us
#define	TPS62353_CTRL2_DEFSLEW_0_3		(1 << 0)		//0.3 mV/us
#define	TPS62353_CTRL2_DEFSLEW_0_6		(2 << 0)		//0.6 mV/us
#define	TPS62353_CTRL2_DEFSLEW_1_2		(3 << 0)		//1.2 mV/us
#define	TPS62353_CTRL2_DEFSLEW_2_4		(4 << 0)		//2.4 mV/us
#define	TPS62353_CTRL2_DEFSLEW_4_8		(5 << 0)		//4.8 mV/us
#define	TPS62353_CTRL2_DEFSLEW_9_6		(6 << 0)		//9.6 mV/us
#define	TPS62353_CTRL2_DEFSLEW_IMMED	(7 << 0)		//Immediate (default)
#define	TPS62353_CTRL2_DEFSLEW_MASK		(0x7 << 0) 	//mask
/*	PLL_MULT -- PLL_MULT defines the synchronization clock multiplier ratio	*/
#define	TPS62353_CTRL2_PLL_MULT_1		0			//x1 - f(SYNC) = 3 MHz กำ 12% (default)
#define	TPS62353_CTRL2_PLL_MULT_2		(1 << 3)		//x2 - f(SYNC) = 1.5 MHz กำ 12%
#define	TPS62353_CTRL2_PLL_MULT_3		(2 << 3)		//x3 - f(SYNC) = 1 MHz กำ 12%
#define	TPS62353_CTRL2_PLL_MULT_4		(3 << 3)		//x4 - f(SYNC) = 750 kHz กำ 12%
#define	TPS62353_CTRL2_PLL_MASK			(3 << 3)		//mask
/*	PWROK -- (READ ONLY)	*/
#define	TPS62353_CTRL2_PWROK_BELOW_TARGET		0		//Indicates that the output voltage is below its target regulation voltage. 
																//This bit is zero if the converter is disabled.
#define	TPS62353_CTRL2_PWROK_WITHIN_RANGE		(1 << 5)	//Indicates that the output voltage is within its nominal range
/*	OUTPUT_DISCHARGE	*/
#define	TPS62353_CTRL2_OUTPUT_DISCHARGE_NOACT	0		//The dc/dc output capacitor is not actively discharged when the converter is disabled (default).
#define	TPS62353_CTRL2_OUTPUT_DISCHARGE_ACT	(1 << 6)	//The dc/dc output capacitor is actively discharged when the converter is disabled.
/*	GO	*/
#define	TPS62353_CTRL2_GO_DIS	0			//No change in the output voltage (default).
#define	TPS62353_CTRL2_GO_EN		(1 << 7)		//The output voltage is changed with the ramp rate defined in DEFSLEW.


/*	TPS62353_VSEL parameter	*/
/*	MODE0	*/
#define	LIGHTPFM_OPTIMIZE_22		0		
#define	LIGHTPFM_OPTIMIZE_1		1

/*	TPS62353_CONTROL1 parameter	*/
/*	MODE0	*/
#define	MODE0_LIGHT_PFM			0		
#define	MODE0_FAST_PFM			1
/*	MODE1	*/
#define	MODE1_FORCED_PWM		0			
#define	MODE1_FAST_PFM			1
/*	MODE_CTRL	*/
#define	MODE_CTRL_MODE01			0			
#define	MODE_CTRL_LIGHT_PFM		1	
#define	MODE_CTRL_FORCED_PWM	2	
#define	MODE_CTRL_FAST_PFM		3	
/*	HW_nSW		*/
#define	HW_nSW_SW				0			
#define	HW_nSW_HW				1		

/*	TPS62353_CONTROL2 parameter	*/
/*	DEFSLEW	*/
#define	DEFSLEW_0_15		0			
#define	DEFSLEW_0_3		1
#define	DEFSLEW_0_6		2
#define	DEFSLEW_1_2		3
#define	DEFSLEW_2_4		4
#define	DEFSLEW_4_8		5
#define	DEFSLEW_9_6		6
#define	DEFSLEW_IMMED		7
/*	PLL_MULT	*/
#define	PLL_MULT_1			0			
#define	PLL_MULT_2			1
#define	PLL_MULT_3			2
#define	PLL_MULT_4			3



extern void tps62353_set_type(int type);
extern int tps62353_read_all(struct i2c_client *client);
extern int tps62353_vsm0_set(struct i2c_client *client, u16 val);
extern int tps62353_vsm0_get(struct i2c_client *client);
extern int tps62353_vsm1_set(struct i2c_client *client, u16 val);
extern int tps62353_vsm1_get(struct i2c_client *client);
extern int tps62353_lightpfm_optimize_set(struct i2c_client *client, int par);
extern int tps62353_en_dcdc_set(struct i2c_client *client, int en);			//1:enable	0:disable
extern int tps62353_ctrl1_mode0_set(struct i2c_client *client, int par);
extern int tps62353_ctrl1_mode1_set(struct i2c_client *client, int par);
extern int tps62353_ctrl1_mode_ctrl_set(struct i2c_client *client, int par);
extern int tps62353_ctrl1_hw_nsw_set(struct i2c_client *client, int par);
extern int tps62353_ctrl1_en_sync_set(struct i2c_client *client, int en);
extern int tps62353_ctrl2_defslew_set(struct i2c_client *client, int par);
extern int tps62353_ctrl2_pll_mult_set(struct i2c_client *client, int par);
extern int tps62353_ctrl2_pwrok_get(struct i2c_client *client);		//return 1:within its nominal range   0:below its target regulation voltage. 
extern int tps62353_ctrl2_output_discharge_act_set(struct i2c_client *client, int act);
extern int tps62353_ctrl2_go(struct i2c_client *client);


extern int tps62353_vsel0_set(struct i2c_client *client, u32 val);
extern int tps62353_vsel1_set(struct i2c_client *client, u32 val);
extern int tps62353_ctrl1_set(struct i2c_client *client, u32 val);
extern int tps62353_ctrl2_set(struct i2c_client *client, u32 val);



#endif	//_TPS62353_H_


