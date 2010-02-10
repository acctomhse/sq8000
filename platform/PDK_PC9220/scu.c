#include <type.h>
#include <io.h>
#include <platform.h>
#include <scu.h>
#include "scu-reg.h"



//#define SOCLE_SCU_DEBUG

#ifdef SOCLE_SCU_DEBUG
#define SCUDBUG(fmt, args...) printf("%s() " fmt, __FUNCTION__, ## args)
#else
#define SCUDBUG(fmt, args...)
#endif

static struct socle_clock_st {
	unsigned long cpu_clock;
	unsigned long ahb_clock;
	unsigned long apb_clock;
	unsigned long uart_clock;
}socle_clock;

static inline void
socle_scu_write(u32 value, u32 reg) 
{
	iowrite32(value, SOCLE_APB0_SCU+reg);
}

static inline u32
socle_scu_read(u32 reg)
{
	return ioread32(SOCLE_APB0_SCU+reg);
}

static inline void
socle_scu_show(char *info)
{
	printf("%s", info);
}


	/* read chip ID	*/
extern u32 
socle_scu_chip_id (void)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_CID);

	return tmp;
}

static int socle_scu_pll_formula (int m, int n, int od, int xin);

//#define MPLL_XIN					12288000		//(Hz)
//#define UPLL_XIN					11059200		//(Hz)
#if 0
#define MPLL_XIN                                        12288000                //(Hz)
#else
#define MPLL_XIN                                        12000000                //(Hz)
#endif

#define UPLL_XIN                                        11059200                //(Hz)

static int 
socle_scu_pll_formula (int m, int n, int od, int xin)
{
	u32 clock;

	xin = xin/1000;
	clock = xin * (m+1) / (n+1) / (od+1) ;
	clock = clock * 1000;

	return clock;
}

/* UPLL configuration */
extern int 
socle_scu_upll_set (int clock)
{
	u32 tmp, upll;

	switch(clock){
		case SOCLE_SCU_UART_CLOCK_176:			
			tmp = SCU_UART_CLOCK_176;
			break;
		default :		
			socle_scu_show("unknow upll clock !!\n");
			return -1;
			break;
	}

	upll = ((socle_scu_read(SOCLE_SCU_UPLLCON) & ~SCU_UPLLCON_PLL_MASK) | (tmp ));
	
	socle_scu_write(upll, SOCLE_SCU_UPLLCON);
	
	return 0;
}		

extern int 
socle_scu_upll_get (void)
{
	int m,n,od;
	u32 uclk;

	uclk = socle_scu_read(SOCLE_SCU_UPLLCON);
	
	n = (uclk & SCU_UPLLCON_N_MASK) >> SCU_UPLLCON_N;
	m = (uclk & SCU_UPLLCON_M_MASK) >>SCU_UPLLCON_M;
	od = (uclk & SCU_UPLLCON_OD_MASK) >> SCU_UPLLCON_OD;

	uclk = socle_scu_pll_formula(m, n, od,UPLL_XIN);

	return uclk;
}

/* UPLL power down/normal	*/
extern void 
socle_scu_upll_normal (void)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_UPLLCON);
	socle_scu_write(tmp & (~SCU_UPLLCON_PLL_PWR_DN) , SOCLE_SCU_UPLLCON);
	return ;
}
	
extern void 
socle_scu_upll_power_down (void)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_UPLLCON);
	socle_scu_write(tmp | SCU_UPLLCON_PLL_PWR_DN , SOCLE_SCU_UPLLCON);
	
	return ;
}

extern int 
socle_scu_upll_status_get (void)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_UPLLCON) & SCU_UPLLCON_PLL_PWR_DN;

	if(SCU_MPLLCON_PLL_PWR_DN == tmp)
		return 0;
	else
		return 1;
}

/* MPLL configuration */
extern int 
socle_scu_mpll_set (int clock)
{
	u32 tmp, mpll;
	
	switch(clock){
		case SOCLE_SCU_CPU_CLOCK_33 :			
			tmp = SCU_CPU_CLOCK_33;
			break;
		case SOCLE_SCU_CPU_CLOCK_66 :			
			tmp = SCU_CPU_CLOCK_66;
			break;
		case SOCLE_SCU_CPU_CLOCK_80 :			
			tmp = SCU_CPU_CLOCK_80;
			break;
		case SOCLE_SCU_CPU_CLOCK_100 :			
			tmp = SCU_CPU_CLOCK_100;
			break;
		case SOCLE_SCU_CPU_CLOCK_132 :			
			tmp = SCU_CPU_CLOCK_132;
			break;
		case SOCLE_SCU_CPU_CLOCK_133 :			
			tmp = SCU_CPU_CLOCK_133;
			break;
		case SOCLE_SCU_CPU_CLOCK_166 :			
			tmp = SCU_CPU_CLOCK_166;
			break;
		case SOCLE_SCU_CPU_CLOCK_200 :			
			tmp = SCU_CPU_CLOCK_200;
			break;
		case SOCLE_SCU_CPU_CLOCK_240 :			
			tmp = SCU_CPU_CLOCK_240;
			break;
#if 0
		case SOCLE_SCU_CPU_CLOCK_258 :			
			tmp = SCU_CPU_CLOCK_258;
			break;
#else
	case SOCLE_SCU_CPU_CLOCK_252 :
                        tmp = SCU_CPU_CLOCK_252;
                        break;
#endif
		case SOCLE_SCU_CPU_CLOCK_264 :			
			tmp = SCU_CPU_CLOCK_264;
			break;
		case SOCLE_SCU_CPU_CLOCK_266 :			
			tmp = SCU_CPU_CLOCK_266;
			break;
		case SOCLE_SCU_CPU_CLOCK_280 :			
			tmp = SCU_CPU_CLOCK_280;
			break;
		case SOCLE_SCU_CPU_CLOCK_300 :			
			tmp = SCU_CPU_CLOCK_300;
			break;
		case SOCLE_SCU_CPU_CLOCK_320 :			
			tmp = SCU_CPU_CLOCK_320;
			break;
		case SOCLE_SCU_CPU_CLOCK_340 :			
			tmp = SCU_CPU_CLOCK_340;
			break;
		case SOCLE_SCU_CPU_CLOCK_350 :			
			tmp = SCU_CPU_CLOCK_350;
			break;
		case SOCLE_SCU_CPU_CLOCK_360 :			
			tmp = SCU_CPU_CLOCK_360;
			break;
		case SOCLE_SCU_CPU_CLOCK_400 :			
			tmp = SCU_CPU_CLOCK_400;
			break;
		default :		
			socle_scu_show("unknow upll clock !!\n");
			return -1;
			break;
	}

	mpll = ((socle_scu_read(SOCLE_SCU_MPLLCON) & ~SCU_MPLLCON_PLL_MASK) | (tmp ));
	
	socle_scu_write(mpll, SOCLE_SCU_MPLLCON);
	
	socle_get_cpu_clock();	

	return 0;
}
	
extern int 
socle_scu_mpll_get (void)
{
	u32 m,n,od;
	u32 mclk;

	mclk = socle_scu_read(SOCLE_SCU_MPLLCON);
	
	//n = (mclk & SCU_MPLLCON_N_MASK) >> SCU_MPLLCON_N;
	n = (mclk & SCU_MPLLCON_N_MASK) ;
	n = n >> SCU_MPLLCON_N;
	m = (mclk & SCU_MPLLCON_M_MASK) >> SCU_MPLLCON_M;
	od = (mclk & SCU_MPLLCON_OD_MASK) >> SCU_MPLLCON_OD;

	mclk = socle_scu_pll_formula(m, n, od, MPLL_XIN);

	return mclk;
}


/* MPLL power down/normal	*/
extern void 
socle_scu_mpll_normal (void)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_PWMCON);
	socle_scu_write(tmp | SCU_PWMCON_PWR_NOR , SOCLE_SCU_PWMCON);
	return ;
}
	
extern void 
socle_scu_mpll_power_down (void)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_PWMCON);
	socle_scu_write(tmp & ~SCU_PWMCON_PWR_NOR , SOCLE_SCU_PWMCON);
	
	return ;
}


extern int 
socle_scu_mpll_status_get (void)		//return 1:normal	0:slow
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_PWMCON) & SCU_PWMCON_PWR_NOR;

	if(SCU_PWMCON_PWR_NOR == tmp)
		return 1;
	else
		return 0;
}


extern int 
socle_scu_uart_clk_get (int uart)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_MCLKDIV);

	switch(uart){
		case 0 :
			tmp = (tmp & SCU_MCLKDIV_UART0_CLK_M) >> SCU_MCLKDIV_UART0_CLK_S;
			break;
		case 1:
			tmp = (tmp & SCU_MCLKDIV_UART1_CLK_M) >> SCU_MCLKDIV_UART1_CLK_S;
			break;
		case 2:
			tmp = (tmp & SCU_MCLKDIV_UART2_CLK_M) >> SCU_MCLKDIV_UART2_CLK_S;
			break;
		default :
			socle_scu_show("unknow UART index\n");
			return -1;
			break;			
	}

	switch(tmp){
		case SCU_MCLKDIV_UART_CLK_24:
			tmp = SOCLE_SCU_UART_CLK_24;
			break;		
		case SCU_MCLKDIV_UART_CLK_UPLL:
			tmp = SOCLE_SCU_UART_CLK_UPLL;
			break;			
		case SCU_MCLKDIV_UART_CLK_UPLL_2:
			tmp = SOCLE_SCU_UART_CLK_UPLL_2;
			break;		
		case SCU_MCLKDIV_UART_CLK_UPLL_4:
			tmp = SOCLE_SCU_UART_CLK_UPLL_4;
			break;
	}
		
	return tmp;
}

	/*	mclk enable/disable	*/
extern int 
socle_scu_mclk_enable (int ip)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_MCLKEN);
	
	switch(ip){
		case SOCLE_SCU_MCLK_DCM :
			tmp = tmp | SCU_MCLK_DCM;
			break;
		case SOCLE_SCU_MCLK_STSDR :
			tmp = tmp | SCU_MCLK_STSDR;
			break;
		case SOCLE_SCU_MCLK_NFC :
			tmp = tmp | SCU_MCLK_NFC;
			break;
		case SOCLE_SCU_MCLK_HDMA :
			tmp = tmp | SCU_MCLK_HDMA;
			break;
		case SOCLE_SCU_MCLK_VOP :
			tmp = tmp | SCU_MCLK_VOP;
			break;
		case SOCLE_SCU_MCLK_UHC0 :
			tmp = tmp | SCU_MCLK_UHC0;
			break;
		case SOCLE_SCU_MCLK_UDC :
			tmp = tmp | SCU_MCLK_UDC;
			break;
		case SOCLE_SCU_MCLK_UHC1 :
			tmp = tmp | SCU_MCLK_UHC1;
			break;			
		case SOCLE_SCU_MCLK_MAC :
			tmp = tmp | SCU_MCLK_MAC;
			break;		
		case SOCLE_SCU_MCLK_CLCD :
			tmp = tmp | SCU_MCLK_CLCD;
			break;
		case SOCLE_SCU_MCLK_GPIO3 :
			tmp = tmp | SCU_MCLK_GPIO3;
			break;
		case SOCLE_SCU_MCLK_GPIO2 :
			tmp = tmp | SCU_MCLK_GPIO2;
			break;
		case SOCLE_SCU_MCLK_GPIO1 :
			tmp = tmp | SCU_MCLK_GPIO1;
			break;
		case SOCLE_SCU_MCLK_GPIO0 :
			tmp = tmp | SCU_MCLK_GPIO0;
			break;
		case SOCLE_SCU_MCLK_ADC :
			tmp = tmp | SCU_MCLK_ADC;
			break;
		case SOCLE_SCU_MCLK_PWM :
			tmp = tmp | SCU_MCLK_PWM;
			break;
		case SOCLE_SCU_MCLK_WDT :
			tmp = tmp | SCU_MCLK_WDT;
			break;
		case SOCLE_SCU_MCLK_RTC :
			tmp = tmp | SCU_MCLK_RTC;
			break;
		case SOCLE_SCU_MCLK_TIMER :
			tmp = tmp | SCU_MCLK_TIMER;
			break;
		case SOCLE_SCU_MCLK_SDMMC :
			tmp = tmp | SCU_MCLK_SDMMC;
			break;
		case SOCLE_SCU_MCLK_I2S :
			tmp = tmp | SCU_MCLK_I2S;
			break;
		case SOCLE_SCU_MCLK_I2C :
			tmp = tmp | SCU_MCLK_I2C;
			break;
		case SOCLE_SCU_MCLK_SPI1 :
			tmp = tmp | SCU_MCLK_SPI1;
			break;
		case SOCLE_SCU_MCLK_SPI0 :
			tmp = tmp | SCU_MCLK_SPI0;
			break;
		case SOCLE_SCU_MCLK_UART2 :
			tmp = tmp | SCU_MCLK_UART2;
			break;
		case SOCLE_SCU_MCLK_UART1 :
			tmp = tmp | SCU_MCLK_UART1;
			break;
		case SOCLE_SCU_MCLK_UART0 :
			tmp = tmp | SCU_MCLK_UART0;
			break;
		default :
			socle_scu_show("unknow IP number\n");
			return -1;
			break;
	}
				
	socle_scu_write(tmp, SOCLE_SCU_MCLKEN);	
	
	return 0;
}
	
extern int 
socle_scu_mclk_disable (int ip)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_MCLKEN);
	
	switch(ip){
		case SOCLE_SCU_MCLK_DCM :
			tmp = tmp & ~SCU_MCLK_DCM;
			break;
		case SOCLE_SCU_MCLK_STSDR :
			tmp = tmp & ~SCU_MCLK_STSDR;
			break;
		case SOCLE_SCU_MCLK_NFC :
			tmp = tmp & ~SCU_MCLK_NFC;
			break;
		case SOCLE_SCU_MCLK_HDMA :
			tmp = tmp & ~SCU_MCLK_HDMA;
			break;
		case SOCLE_SCU_MCLK_VOP :
			tmp = tmp & ~SCU_MCLK_VOP;
			break;
		case SOCLE_SCU_MCLK_UHC0 :
			tmp = tmp & ~SCU_MCLK_UHC0;
			break;
		case SOCLE_SCU_MCLK_UDC :
			tmp = tmp & ~SCU_MCLK_UDC;
			break;
		case SOCLE_SCU_MCLK_UHC1 :
			tmp = tmp & ~SCU_MCLK_UHC1;
			break;			
		case SOCLE_SCU_MCLK_MAC :
			tmp = tmp & ~SCU_MCLK_MAC;
			break;		
		case SOCLE_SCU_MCLK_CLCD :
			tmp = tmp & ~SCU_MCLK_CLCD;
			break;
		case SOCLE_SCU_MCLK_GPIO3 :
			tmp = tmp & ~SCU_MCLK_GPIO3;
			break;
		case SOCLE_SCU_MCLK_GPIO2 :
			tmp = tmp & ~SCU_MCLK_GPIO2;
			break;
		case SOCLE_SCU_MCLK_GPIO1 :
			tmp = tmp & ~SCU_MCLK_GPIO1;
			break;
		case SOCLE_SCU_MCLK_GPIO0 :
			tmp = tmp & ~SCU_MCLK_GPIO0;
			break;
		case SOCLE_SCU_MCLK_ADC :
			tmp = tmp & ~SCU_MCLK_ADC;
			break;
		case SOCLE_SCU_MCLK_PWM :
			tmp = tmp & ~SCU_MCLK_PWM;
			break;
		case SOCLE_SCU_MCLK_WDT :
			tmp = tmp & ~SCU_MCLK_WDT;
			break;
		case SOCLE_SCU_MCLK_RTC :
			tmp = tmp & ~SCU_MCLK_RTC;
			break;
		case SOCLE_SCU_MCLK_TIMER :
			tmp = tmp & ~SCU_MCLK_TIMER;
			break;
		case SOCLE_SCU_MCLK_SDMMC :
			tmp = tmp & ~SCU_MCLK_SDMMC;
			break;
		case SOCLE_SCU_MCLK_I2S :
			tmp = tmp & ~SCU_MCLK_I2S;
			break;
		case SOCLE_SCU_MCLK_I2C :
			tmp = tmp & ~SCU_MCLK_I2C;
			break;
		case SOCLE_SCU_MCLK_SPI1 :
			tmp = tmp & ~SCU_MCLK_SPI1;
			break;
		case SOCLE_SCU_MCLK_SPI0 :
			tmp = tmp & ~SCU_MCLK_SPI0;
			break;
		case SOCLE_SCU_MCLK_UART2 :
			tmp = tmp & ~SCU_MCLK_UART2;
			break;
		case SOCLE_SCU_MCLK_UART1 :
			tmp = tmp & ~SCU_MCLK_UART1;
			break;
		case SOCLE_SCU_MCLK_UART0 :
			tmp = tmp & ~SCU_MCLK_UART0;
			break;
		default :
			socle_scu_show("unknow IP number\n");
			return -1;
			break;
	}
				
	socle_scu_write(tmp, SOCLE_SCU_MCLKEN);	
	
	return 0;
}

	/*	aclk enable/disable	*/
extern int 
socle_scu_aclk_enable (int ip)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_ACLKEN);
	
	switch(ip){
		case SOCLE_SCU_ACLK_SDC_HARDMACRO :
			tmp = tmp | SCU_ACLK_SDC_HARDMACRO;
			break;
		case SOCLE_SCU_ACLK_MAC_HARDMACRO :
			tmp = tmp | SCU_ACLK_MAC_HARDMACRO;
			break;
		case SOCLE_SCU_ACLK_LCDC_HARDMACRO :
			tmp = tmp | SCU_ACLK_LCDC_HARDMACRO;
			break;
		case SOCLE_SCU_ACLK_UDC_HARDMACRO :
			tmp = tmp | SCU_ACLK_UDC_HARDMACRO;
			break;
		case SOCLE_SCU_ACLK_MPLL_OSC_CRYSTAL :
			tmp = tmp | SCU_ACLK_MPLL_OSC_CRYSTAL;
			break;
		case SOCLE_SCU_ACLK_MPLL_OSC_CLK :
			tmp = tmp | SCU_ACLK_MPLL_OSC_CLK;
			break;
		case SOCLE_SCU_ACLK_ST_SDR :
			tmp = tmp | SCU_ACLK_ST_SDR;
			break;
		case SOCLE_SCU_ACLK_HS_UART :
			tmp = tmp | SCU_ACLK_HS_UART;
			break;
		case SOCLE_SCU_ACLK_LCD_VOP :
			tmp = tmp | SCU_ACLK_LCD_VOP;
			break;		
		case SOCLE_SCU_ACLK_MAC :
			tmp = tmp | SCU_ACLK_MAC;
			break;
		case SOCLE_SCU_ACLK_UHC0 :
			tmp = tmp | SCU_ACLK_UHC0;
			break;
		case SOCLE_SCU_ACLK_UHC1 :
			tmp = tmp | SCU_ACLK_UHC1;
			break;
		case SOCLE_SCU_ACLK_UTMI_UHC1 :
			tmp = tmp | SCU_ACLK_UTMI_UHC1;
			break;
		case SOCLE_SCU_ACLK_UTMI_UHC0 :
			tmp = tmp | SCU_ACLK_UTMI_UHC0;
			break;
		case SOCLE_SCU_ACLK_UDC :
			tmp = tmp | SCU_ACLK_UDC;
			break;
		case SOCLE_SCU_ACLK_ADC :
			tmp = tmp | SCU_ACLK_ADC;
			break;
		case SOCLE_SCU_ACLK_I2S :
			tmp = tmp | SCU_ACLK_I2S;
			break;
		case SOCLE_SCU_ACLK_UART2 :
			tmp = tmp | SCU_ACLK_UART2;
			break;
		case SOCLE_SCU_ACLK_UART1 :
			tmp = tmp | SCU_ACLK_UART1;
			break;
		case SOCLE_SCU_ACLK_UART0 :
			tmp = tmp | SCU_ACLK_UART0;
			break;
		default :
			socle_scu_show("unknow IP number\n");
			return -1;
			break;
	}
				
	socle_scu_write(tmp, SOCLE_SCU_ACLKEN);	
	
	return 0;
}
	
extern int 
socle_scu_aclk_disable (int ip)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_ACLKEN);
	
	switch(ip){
		case SOCLE_SCU_ACLK_SDC_HARDMACRO :
			tmp = tmp & ~SCU_ACLK_SDC_HARDMACRO;
			break;
		case SOCLE_SCU_ACLK_MAC_HARDMACRO :
			tmp = tmp & ~SCU_ACLK_MAC_HARDMACRO;
			break;
		case SOCLE_SCU_ACLK_LCDC_HARDMACRO :
			tmp = tmp & ~SCU_ACLK_LCDC_HARDMACRO;
			break;
		case SOCLE_SCU_ACLK_UDC_HARDMACRO :
			tmp = tmp & ~SCU_ACLK_UDC_HARDMACRO;
			break;
		case SOCLE_SCU_ACLK_MPLL_OSC_CRYSTAL :
			tmp = tmp & ~SCU_ACLK_MPLL_OSC_CRYSTAL;
			break;
		case SOCLE_SCU_ACLK_MPLL_OSC_CLK :
			tmp = tmp & ~SCU_ACLK_MPLL_OSC_CLK;
			break;
		case SOCLE_SCU_ACLK_ST_SDR :
			tmp = tmp & ~SCU_ACLK_ST_SDR;
			break;
		case SOCLE_SCU_ACLK_HS_UART :
			tmp = tmp & ~SCU_ACLK_HS_UART;
			break;
		case SOCLE_SCU_ACLK_LCD_VOP :
			tmp = tmp & ~SCU_ACLK_LCD_VOP;
			break;		
		case SOCLE_SCU_ACLK_MAC :
			tmp = tmp & ~SCU_ACLK_MAC;
			break;
		case SOCLE_SCU_ACLK_UHC0 :
			tmp = tmp & ~SCU_ACLK_UHC0;
			break;
		case SOCLE_SCU_ACLK_UHC1 :
			tmp = tmp & ~SCU_ACLK_UHC1;
			break;
		case SOCLE_SCU_ACLK_UTMI_UHC1 :
			tmp = tmp & ~SCU_ACLK_UTMI_UHC1;
			break;
		case SOCLE_SCU_ACLK_UTMI_UHC0 :
			tmp = tmp & ~SCU_ACLK_UTMI_UHC0;
			break;
		case SOCLE_SCU_ACLK_UDC :
			tmp = tmp & ~SCU_ACLK_UDC;
			break;
		case SOCLE_SCU_ACLK_ADC :
			tmp = tmp & ~SCU_ACLK_ADC;
			break;
		case SOCLE_SCU_ACLK_I2S :
			tmp = tmp & ~SCU_ACLK_I2S;
			break;
		case SOCLE_SCU_ACLK_UART2 :
			tmp = tmp & ~SCU_ACLK_UART2;
			break;
		case SOCLE_SCU_ACLK_UART1 :
			tmp = tmp & ~SCU_ACLK_UART1;
			break;
		case SOCLE_SCU_ACLK_UART0 :
			tmp = tmp & ~SCU_ACLK_UART0;
			break;
		default :
			socle_scu_show("unknow IP number\n");
			return -1;
			break;
	}
				
	socle_scu_write(tmp, SOCLE_SCU_ACLKEN);	
	
	return 0;
}









/*	MCLKDIV		*/

	/* PLL lock period	*/
extern void 
socle_scu_pll_lock_period_set (int period)	
{
	u32 tmp;

	if(period < 2)
		period = 2;

	tmp = socle_scu_read(SOCLE_SCU_MCLKDIV) & ~SCU_MCLKDIV_PLL_LOCK_PERIOD_M;
	tmp = tmp | period;
	socle_scu_write(tmp, SOCLE_SCU_MCLKDIV);
	
	return ;
}	

extern int 
socle_scu_pll_lock_period_get ()	
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_MCLKDIV) & SCU_MCLKDIV_PLL_LOCK_PERIOD_M;
	tmp = tmp >> SCU_MCLKDIV_PLL_LOCK_PERIOD_S;
	
	return tmp;
}	

extern void 
socle_scu_adc_clk_div_set (int div)	
{
	u32 tmp;

	if(div < 2)
		div = 2;

	tmp = socle_scu_read(SOCLE_SCU_MCLKDIV) & ~SCU_MCLKDIV_ADC_CLK_DIV_M;
	tmp = tmp | div;
	socle_scu_write(tmp, SOCLE_SCU_MCLKDIV);
	
	return ;
}	

extern int 
socle_scu_adc_clk_div_get ()	
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_MCLKDIV) & SCU_MCLKDIV_ADC_CLK_DIV_M;
	tmp = tmp >> SCU_MCLKDIV_ADC_CLK_DIV_S;
	
	return tmp;
}	

extern int 
socle_scu_uart_clk_24_set (int uart)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_MCLKDIV);

	switch(uart){
		case 0 :
			tmp = (tmp & ~SCU_MCLKDIV_UART0_CLK_M) | (SCU_MCLKDIV_UART_CLK_24 << SCU_MCLKDIV_UART0_CLK_S);
			break;
		case 1:
			tmp = (tmp & ~SCU_MCLKDIV_UART1_CLK_M) | (SCU_MCLKDIV_UART_CLK_24 << SCU_MCLKDIV_UART1_CLK_S);
			break;
		case 2:
			tmp = (tmp & ~SCU_MCLKDIV_UART2_CLK_M) | (SCU_MCLKDIV_UART_CLK_24 << SCU_MCLKDIV_UART2_CLK_S);
			break;
		default :
			socle_scu_show("unknow UART index\n");
			return -1;
			break;			
	}
	socle_scu_write(tmp, SOCLE_SCU_MCLKDIV);
		
	return 0;
}

extern int 
socle_scu_uart_clk_upll_set (int uart)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_MCLKDIV);

	switch(uart){
		case 0 :
			tmp = (tmp & ~SCU_MCLKDIV_UART0_CLK_M) | (SCU_MCLKDIV_UART_CLK_UPLL << SCU_MCLKDIV_UART0_CLK_S);
			break;
		case 1:
			tmp = (tmp & ~SCU_MCLKDIV_UART1_CLK_M) | (SCU_MCLKDIV_UART_CLK_UPLL << SCU_MCLKDIV_UART1_CLK_S);
			break;
		case 2:
			tmp = (tmp & ~SCU_MCLKDIV_UART2_CLK_M) | (SCU_MCLKDIV_UART_CLK_UPLL << SCU_MCLKDIV_UART2_CLK_S);
			break;
		default :
			socle_scu_show("unknow UART index\n");
			return -1;
			break;			
	}
	socle_scu_write(tmp, SOCLE_SCU_MCLKDIV);
	
	return 0;
}

extern int 
socle_scu_uart_clk_upll_2_set (int uart)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_MCLKDIV);

	switch(uart){
		case 0 :
			tmp = (tmp & ~SCU_MCLKDIV_UART0_CLK_M) | (SCU_MCLKDIV_UART_CLK_UPLL_2 << SCU_MCLKDIV_UART0_CLK_S);
			break;
		case 1:
			tmp = (tmp & ~SCU_MCLKDIV_UART1_CLK_M) | (SCU_MCLKDIV_UART_CLK_UPLL_2 << SCU_MCLKDIV_UART1_CLK_S);
			break;
		case 2:
			tmp = (tmp & ~SCU_MCLKDIV_UART2_CLK_M) | (SCU_MCLKDIV_UART_CLK_UPLL_2 << SCU_MCLKDIV_UART2_CLK_S);
			break;
		default :
			socle_scu_show("unknow UART index\n");
			return -1;
			break;			
	}
	socle_scu_write(tmp, SOCLE_SCU_MCLKDIV);
	
	return 0;
}

extern int 
socle_scu_uart_clk_upll_4_set (int uart)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_MCLKDIV);

	switch(uart){
		case 0 :
			tmp = (tmp & ~SCU_MCLKDIV_UART0_CLK_M) | (SCU_MCLKDIV_UART_CLK_UPLL_4 << SCU_MCLKDIV_UART0_CLK_S);
			break;
		case 1:
			tmp = (tmp & ~SCU_MCLKDIV_UART1_CLK_M) | (SCU_MCLKDIV_UART_CLK_UPLL_4 << SCU_MCLKDIV_UART1_CLK_S);
			break;
		case 2:
			tmp = (tmp & ~SCU_MCLKDIV_UART2_CLK_M) | (SCU_MCLKDIV_UART_CLK_UPLL_4 << SCU_MCLKDIV_UART2_CLK_S);
			break;
		default :
			socle_scu_show("unknow UART index\n");
			return -1;
			break;			
	}
	socle_scu_write(tmp, SOCLE_SCU_MCLKDIV);

	return 0;
	
}

/* CPU/AHB clock ratio	*/
extern int 
socle_scu_clock_ratio_set (int ratio)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_MCLKDIV) & ~SCU_MCLKDIV_CLK_RATIO_MASK;

	switch(ratio){
		case SOCLE_SCU_CLOCK_RATIO_1_1 :
			tmp = tmp |SCU_MCLKDIV_CLK_RATIO_1_1;
			break;
		case SOCLE_SCU_CLOCK_RATIO_2_1 :
			tmp = tmp |SCU_MCLKDIV_CLK_RATIO_2_1;
			break;
		case SOCLE_SCU_CLOCK_RATIO_3_1 :
			tmp = tmp |SCU_MCLKDIV_CLK_RATIO_3_1;
			break;
		case SOCLE_SCU_CLOCK_RATIO_4_1 :
			tmp = tmp |SCU_MCLKDIV_CLK_RATIO_4_1;
			break;
		default :
			socle_scu_show("unknow ratio value\n");
			return -1;
			break;			
	}
	socle_scu_write(tmp, SOCLE_SCU_MCLKDIV);
	
	return 0;
}
	
extern int 
socle_scu_clock_ratio_get (void)
{
	u32 tmp;
	int ratio;

	tmp = socle_scu_read(SOCLE_SCU_MCLKDIV) & SCU_MCLKDIV_CLK_RATIO_MASK;

	switch(tmp){
		case SCU_MCLKDIV_CLK_RATIO_1_1 :
			ratio = 1;
			break;
		case SCU_MCLKDIV_CLK_RATIO_2_1 :
			ratio = 2;
			break;
		case SCU_MCLKDIV_CLK_RATIO_3_1 :
			ratio = 3;
			break;
		case SCU_MCLKDIV_CLK_RATIO_4_1 :
			ratio = 4;
			break;
		default :
			socle_scu_show("unknow ratio value\n");
			return -1;
			break;
	}	
	
	return ratio;
	
}



//Power Mode setting 
	/*	SDRAM data bus width status	*/
extern int 
socle_scu_sdram_bus_width_status (void)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_PWMCON) & SCU_PWMCON_SDR_WIDTH;

	if(tmp == SCU_PWMCON_SDR_WIDTH_32)
		return SOCLE_SCU_SDRAM_BUS_WIDTH_32;
	else
		return SOCLE_SCU_SDRAM_BUS_WIDTH_16;
	
}
	/*	DCM mode setting status	*/
extern int 
socle_scu_dcm_mode_status (void)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_PWMCON) & SCU_PWMCON_DCM_MODE;

	if(tmp == SCU_PWMCON_DCM_MODE_DCM)
		return SOCLE_SCU_DCM_MODE_DCM;
	else
		return SOCLE_SCU_DCM_MODE_NOR;
	
}
	
	/*	USB mode setting status	*/
extern int 
socle_scu_usb_mode_status (void)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_PWMCON) & SCU_PWMCON_USB_MODE;

	if(tmp == SCU_PWMCON_USB_MODE_UDC)
		return SOCLE_SCU_USB_MODE_UDC;
	else
		return SOCLE_SCU_USB_MODE_UHC;
	
}
	
	/*	TPS MAC status	*/
extern int 
socle_scu_tps_mac_status (void)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_PWMCON) & SCU_PWMCON_TPS_MAC;

	if(tmp == SCU_PWMCON_TPS_MAC)
		return 1;
	else
		return 0;
	
}
	
	/*	RPS MAC status	*/
extern int 
socle_scu_rps_mac_status (void)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_PWMCON) & SCU_PWMCON_RPS_MAC;

	if(tmp == SCU_PWMCON_RPS_MAC)
		return 1;
	else
		return 0;
	
}
	
	/*	Boot source selection status	*/
extern int 
socle_scu_boot_source_status (void)
{
	u32 tmp, status;

	tmp = socle_scu_read(SOCLE_SCU_PWMCON) & SCU_PWMCON_BOOT_MODE;

	switch(tmp){
		case SCU_PWMCON_BOOT_MODE_NOR_16 :
			status = SOCLE_SCU_BOOT_NOR_16;
			break;
		case SCU_PWMCON_BOOT_MODE_NOR_8 :
			status = SOCLE_SCU_BOOT_NOR_8;
			break;
		case SCU_PWMCON_BOOT_MODE_NAND :
			status = SOCLE_SCU_BOOT_NAND;
			break;
		case SCU_PWMCON_BOOT_MODE_ISP:
			status = SOCLE_SCU_BOOT_ISP_ROM;
			break;
	}
		return status;
		
}


extern int 
socle_scu_auto_boot_fail_status (void)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_PWMCON) & SCU_PWMCON_BOOT_FAIL;

	if(tmp == SCU_PWMCON_BOOT_FAIL)
		return 1;
	else
		return 0;
	
}

	/*	Stand by wait for interrupt */
extern void 
socle_scu_pw_standbywfi_enable (int i)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_PWMCON);

	if(i ==1)
		socle_scu_write(tmp | SCU_PWMCON_STANDBYWFI , SOCLE_SCU_PWMCON);
	else 
		socle_scu_write(tmp & ~SCU_PWMCON_STANDBYWFI , SOCLE_SCU_PWMCON);
	
	return ;
}

	/*	stop mode -- systen clock	*/
extern void 
socle_scu_stop_mode_enable (int i)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_PWMCON);

	if(i == 1)
		socle_scu_write(tmp | SCU_PWMCON_PWR_STOP , SOCLE_SCU_PWMCON);
	else 
		socle_scu_write(tmp & ~SCU_PWMCON_PWR_STOP , SOCLE_SCU_PWMCON);

	return ;
}


	/*	slow mode -- systen clock	*/
extern void 
socle_scu_slow_mode_disable (int i)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_PWMCON);

	if(i == 1)
		socle_scu_write(tmp & ~SCU_PWMCON_PWR_NOR, SOCLE_SCU_PWMCON);
	else 
		socle_scu_write(tmp | SCU_PWMCON_PWR_NOR, SOCLE_SCU_PWMCON);

	return ;
}

extern void 
socle_scu_sw_reset(void)
{
	socle_scu_write(socle_scu_chip_id(), SOCLE_SCU_SWRST);

}

extern void 
socle_scu_sw_remap(void)
{
	socle_scu_write(socle_scu_chip_id(), SOCLE_SCU_REMAP);

}


// Device control 
extern int 
socle_scu_dev_enable(u32 dev)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_DEVCON) ;
	
	switch(dev){
		case SOCLE_DEVCON_WDT :
			tmp = tmp | SCU_DEVCON_WDT_RST;
			break;
		case SOCLE_DEVCON_NFC :
			tmp = tmp | SCU_DEVCON_NFC_GPIO;
			break;
		case SOCLE_DEVCON_MAC :
			tmp = tmp | SCU_DEVCON_MAC_GPIO;
			break;
		case SOCLE_DEVCON_TMR :
			tmp = tmp | SCU_DEVCON_TMR_GPIO;
			break;
		case SOCLE_DEVCON_PWM0 :
			tmp = tmp | SCU_DEVCON_PWM0_GPIO;
			break;
		case SOCLE_DEVCON_PWM1 :
			tmp = tmp | SCU_DEVCON_PWM1_GPIO;
			break;
		case SOCLE_DEVCON_EXT_INT0 :
			tmp = (tmp & ~SCU_DEVCON_INT0) | SCU_DEVCON_INT0_EXT_INT0;
			break;
		case SOCLE_DEVCON_EXT_INT0_NFIQ :
			tmp = (tmp & ~SCU_DEVCON_INT0) | SCU_DEVCON_INT0_NFIQ;
			break;
		case SOCLE_DEVCON_EXT_INT1 :
			tmp = tmp | SCU_DEVCON_INT1;
			break;
		case SOCLE_DEVCON_LCDC :
			tmp = (tmp & ~SCU_DEVCON_LCD_GPIO) | SCU_DEVCON_LCD_GPIO_LCD;
			break;
		case SOCLE_DEVCON_LCDC_VOP :
			tmp = (tmp & ~SCU_DEVCON_LCD_GPIO) | SCU_DEVCON_LCD_GPIO_VOP;
			break;
		case SOCLE_DEVCON_SPI0 :
			tmp = tmp | SCU_DEVCON_SPI0_GPIO;
			break;
		case SOCLE_DEVCON_SPI1 :
			tmp = tmp | SCU_DEVCON_SPI1_GPIO;
			break;
		case SOCLE_DEVCON_I2S_TX :
			tmp = (tmp & ~SCU_DEVCON_I2S_GPIO) | SCU_DEVCON_I2S_GPIO_TX;
			break;
		case SOCLE_DEVCON_I2S_RX :
			tmp = (tmp & ~SCU_DEVCON_I2S_GPIO) | SCU_DEVCON_I2S_GPIO_RX;
			break;
		case SOCLE_DEVCON_I2S_TX_RX :
			tmp = (tmp & ~SCU_DEVCON_I2S_GPIO) | SCU_DEVCON_I2S_GPIO_TX_RX;
			break;
		case SOCLE_DEVCON_I2C :
			tmp = tmp | SCU_DEVCON_I2C_GPIO;
			break;
		case SOCLE_DEVCON_SDMMC :
			tmp = tmp | SCU_DEVCON_SDMMC_GPIO;
			break;
		case SOCLE_DEVCON_UART0 :
			tmp = tmp | SCU_DEVCON_UART0_GPIO;
			break;
		case SOCLE_DEVCON_UART1 :
			tmp = tmp | SCU_DEVCON_UART1_GPIO;
			break;
		case SOCLE_DEVCON_UART2 :
			tmp = tmp | SCU_DEVCON_UART2_GPIO;
			break;
		default :
			socle_scu_show("unknow IP number\n");
			return -1;
			break;
	}
				
	socle_scu_write(tmp, SOCLE_SCU_DEVCON);	
	
	return 0;
}

	
extern int 
socle_scu_dev_disable(u32 dev)
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_DEVCON) ;
	
	switch(dev){
		case SOCLE_DEVCON_NFC :
			tmp = tmp & ~SCU_DEVCON_NFC_GPIO;
			break;
		case SOCLE_DEVCON_MAC :
			tmp = tmp & ~SCU_DEVCON_MAC_GPIO;
			break;
		case SOCLE_DEVCON_TMR :
			tmp = tmp & ~SCU_DEVCON_TMR_GPIO;
			break;
		case SOCLE_DEVCON_PWM0 :
			tmp = tmp & ~SCU_DEVCON_PWM0_GPIO;
			break;
		case SOCLE_DEVCON_PWM1 :
			tmp = tmp & ~SCU_DEVCON_PWM1_GPIO;
			break;
		case SOCLE_DEVCON_EXT_INT0 :
			tmp = tmp & ~SCU_DEVCON_INT0;
			break;
		case SOCLE_DEVCON_EXT_INT0_NFIQ :
			tmp = tmp & ~SCU_DEVCON_INT0;
			break;
		case SOCLE_DEVCON_EXT_INT1 :
			tmp = tmp & ~SCU_DEVCON_INT1;
			break;
		case SOCLE_DEVCON_LCDC :
			tmp = tmp & ~SCU_DEVCON_LCD_GPIO;
			break;
		case SOCLE_DEVCON_LCDC_VOP :
			tmp = tmp & ~SCU_DEVCON_LCD_GPIO;
			break;
		case SOCLE_DEVCON_SPI0 :
			tmp = tmp & ~SCU_DEVCON_SPI0_GPIO;
			break;
		case SOCLE_DEVCON_SPI1 :
			tmp = tmp & ~SCU_DEVCON_SPI1_GPIO;
			break;
		case SOCLE_DEVCON_I2S_TX :
			tmp = tmp & ~SCU_DEVCON_I2S_GPIO;
			break;
		case SOCLE_DEVCON_I2S_RX :
			tmp = tmp & ~SCU_DEVCON_I2S_GPIO;
			break;
		case SOCLE_DEVCON_I2S_TX_RX :
			tmp = tmp & ~SCU_DEVCON_I2S_GPIO;
			break;
		case SOCLE_DEVCON_I2C :
			tmp = tmp & ~SCU_DEVCON_I2C_GPIO;
			break;
		case SOCLE_DEVCON_SDMMC :
			tmp = tmp & ~SCU_DEVCON_SDMMC_GPIO;
			break;
		case SOCLE_DEVCON_UART0 :
			tmp = tmp & ~SCU_DEVCON_UART0_GPIO;
			break;
		case SOCLE_DEVCON_UART1 :
			tmp = tmp & ~SCU_DEVCON_UART1_GPIO;
			break;
		case SOCLE_DEVCON_UART2 :
			tmp = tmp & ~SCU_DEVCON_UART2_GPIO;
			break;
		default :
			socle_scu_show("unknow IP number\n");
			return -1;
			break;
	}
				
	socle_scu_write(tmp, SOCLE_SCU_DEVCON);	
	
	return 0;

}

extern void 
socle_scu_lcdc_clk_input_mpll_outpput(void)
{
	u32 tmp;
	
	tmp = socle_scu_read(SOCLE_SCU_DEVCON) ;
	tmp |= SCU_DEVCON_LCD_CLK_MPLL_OUTPUT;

	socle_scu_write(tmp, SOCLE_SCU_DEVCON);

}

extern void 
socle_scu_lcdc_clk_input_mpll_xin(void)
{
	u32 tmp;
	
	tmp = socle_scu_read(SOCLE_SCU_DEVCON) ;
	tmp &= ~SCU_DEVCON_LCD_CLK_MPLL_OUTPUT;

	socle_scu_write(tmp, SOCLE_SCU_DEVCON);

}


extern void 
socle_scu_hdma_req45_spi0(void)
{
	u32 tmp;
	
	tmp = socle_scu_read(SOCLE_SCU_DEVCON) ;
	tmp &= ~SCU_DEVCON_HDMA45_SPI1;

	socle_scu_write(tmp, SOCLE_SCU_DEVCON);

}

extern void 
socle_scu_hdma_req45_spi1(void)
{
	u32 tmp;
	
	tmp = socle_scu_read(SOCLE_SCU_DEVCON) ;
	tmp |= SCU_DEVCON_HDMA45_SPI1;

	socle_scu_write(tmp, SOCLE_SCU_DEVCON);

}

extern void 
socle_scu_uhc0_48clock_input_upll(void)
{
	u32 tmp;
	
	tmp = socle_scu_read(SOCLE_SCU_DEVCON) ;
	tmp |= SCU_DEVCON_UHC0_48CLK_UPLL;

	socle_scu_write(tmp, SOCLE_SCU_DEVCON);

}

extern void 
socle_scu_uhc0_48clock_input_otg_phy(void)
{
	u32 tmp;
	
	tmp = socle_scu_read(SOCLE_SCU_DEVCON) ;
	tmp &= ~SCU_DEVCON_UHC0_48CLK_UPLL;

	socle_scu_write(tmp, SOCLE_SCU_DEVCON);

}

extern void 
socle_scu_uhc1_48clock_input_upll(void)
{
	u32 tmp;
	
	tmp = socle_scu_read(SOCLE_SCU_DEVCON) ;
	tmp |= SCU_DEVCON_UHC1_48CLK_UPLL;

	socle_scu_write(tmp, SOCLE_SCU_DEVCON);

}

extern void 
socle_scu_uhc1_48clock_input_otg_phy(void)
{
	u32 tmp;
	
	tmp = socle_scu_read(SOCLE_SCU_DEVCON) ;
	tmp &= ~SCU_DEVCON_UHC1_48CLK_UPLL;

	socle_scu_write(tmp, SOCLE_SCU_DEVCON);

}

extern int 
socle_scu_hdma_req01_uart(int uart)
{
	u32 tmp;
	
	tmp = socle_scu_read(SOCLE_SCU_DEVCON) ;
	tmp &= ~SCU_DEVCON_UART_HDMA01_M;
	switch(uart){
		case 0:
			tmp |= (UART0_WITH_HDMA << SCU_DEVCON_UART_HDMA01_S);
			break;
		case 1:
			tmp |= (UART1_WITH_HDMA << SCU_DEVCON_UART_HDMA01_S);
			break;
		case 2:
			tmp |= (UART2_WITH_HDMA << SCU_DEVCON_UART_HDMA01_S);
			break;
		default :
			socle_scu_show("unknow uart number\n");
			return -1;
			break;			
	}

	socle_scu_write(tmp, SOCLE_SCU_DEVCON);
	
	return 0;
}

extern int 
socle_scu_hdma_req23_uart(int uart)
{
	u32 tmp;
	
	tmp = socle_scu_read(SOCLE_SCU_DEVCON) ;
	tmp &= ~SCU_DEVCON_UART_HDMA23_M;
	switch(uart){
		case 0:
			tmp |= (UART0_WITH_HDMA << SCU_DEVCON_UART_HDMA23_S);
			break;
		case 1:
			tmp |= (UART1_WITH_HDMA << SCU_DEVCON_UART_HDMA23_S);
			break;
		case 2:
			tmp |= (UART2_WITH_HDMA << SCU_DEVCON_UART_HDMA23_S);
			break;
		default :
			socle_scu_show("unknow uart number\n");
			return -1;
			break;			
	}

	socle_scu_write(tmp, SOCLE_SCU_DEVCON);
	
	return 0;
}

extern void 
socle_scu_wdt_reset_enable(int en)
{
	u32 tmp;
	
	tmp = socle_scu_read(SOCLE_SCU_DEVCON) ;
	
	if(en == 1)
		tmp |= SCU_DEVCON_WDT_RST;
	else
		tmp &= ~SCU_DEVCON_WDT_RST;
		
	socle_scu_write(tmp, SOCLE_SCU_DEVCON);	
}

extern void 
socle_scu_sw_reset_enable(int en)
{
	u32 tmp;
	
	tmp = socle_scu_read(SOCLE_SCU_DEVCON) ;
	
	if(en == 1)
		tmp |= SCU_DEVCON_SW_RST;
	else
		tmp &= ~SCU_DEVCON_SW_RST;
		
	socle_scu_write(tmp, SOCLE_SCU_DEVCON);	
}

extern void 
socle_scu_nfiq_polarity_high(int en)
{
	u32 tmp;
	
	tmp = socle_scu_read(SOCLE_SCU_DEVCON) ;
	
	if(en == 1)
		tmp |= SCU_DEVCON_FIQ_POLAR_HIGH;
	else
		tmp &= ~SCU_DEVCON_FIQ_POLAR_HIGH;
		
	socle_scu_write(tmp, SOCLE_SCU_DEVCON);	
}

/*	SCU_INFORM0	*/
	/*	User defined information register	*/
extern void 
socle_scu_info0_set (u32 inf)	
{
	socle_scu_write(inf, SOCLE_SCU_INFORM0);
			
	return ;
}		

extern u32 
socle_scu_info0_get ()						//return information0 value
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_INFORM0);
			
	return tmp;
}		

/*	SCU_INFORM1	*/
	/*	User defined information register	*/
extern void 
socle_scu_info1_set (u32 inf)
{
	socle_scu_write(inf, SOCLE_SCU_INFORM1);
			
	return ;
}		

extern u32 
socle_scu_info1_get ()						//return information1 value		
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_INFORM1);
			
	return tmp;
}		

/*	SCU_INFORM2	*/
	/*	User defined information register	*/
extern void 
socle_scu_info2_set (u32 inf)
{
	socle_scu_write(inf, SOCLE_SCU_INFORM2);
			
	return ;
}		

extern u32 
socle_scu_info2_get ()						//return information2 value
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_INFORM2);
			
	return tmp;
}				

/*	SCU_INFORM3	*/
	/*	User defined information register	*/
extern void 
socle_scu_info3_set (u32 inf)
{
	socle_scu_write(inf, SOCLE_SCU_INFORM3);
			
	return ;
}		
		
extern u32 
socle_scu_info3_get ()						//return information3 value	
{
	u32 tmp;

	tmp = socle_scu_read(SOCLE_SCU_INFORM3);
			
	return tmp;
}					



extern unsigned long
socle_get_cpu_clock (void)
{	
	/*	get power mode */
	if(0 == socle_scu_mpll_status_get())
		socle_clock.cpu_clock = MPLL_XIN;	
	else
		socle_clock.cpu_clock = socle_scu_mpll_get();
				
	return socle_clock.cpu_clock ;
}

extern unsigned long
socle_get_ahb_clock (void)
{
	int ratio;
	
	ratio = socle_scu_clock_ratio_get();

	socle_clock.ahb_clock = socle_get_cpu_clock() / ratio;
	
	return socle_clock.ahb_clock;
}

extern unsigned long
socle_get_apb_clock (void)
{
	socle_clock.apb_clock = socle_get_cpu_clock() / 2;
	
	return socle_clock.apb_clock;
}

#define SOCLE_SCU_UART_CLK_24			0
#define SOCLE_SCU_UART_CLK_UPLL		1
#define SOCLE_SCU_UART_CLK_UPLL_2	 	2
#define SOCLE_SCU_UART_CLK_UPLL_4	 	3

extern unsigned long
socle_get_uart_clock (int uart)
{
	int tmp;
	int div;

	/*	get power mode */
	tmp = socle_scu_uart_clk_get(uart);
	switch(tmp){
		case SOCLE_SCU_UART_CLK_24 :
			socle_clock.uart_clock = MPLL_XIN;
			return socle_clock.uart_clock;
			break;
		case SOCLE_SCU_UART_CLK_UPLL:
			div = 1;
			break;
		case SOCLE_SCU_UART_CLK_UPLL_2:
			div = 2;
			break;
		case SOCLE_SCU_UART_CLK_UPLL_4:
			div = 4;
			break;
		default :
			return -1;
			break;
	}
	
	socle_clock.uart_clock = socle_scu_upll_get() / div;
					
	return socle_clock.uart_clock ;
}


