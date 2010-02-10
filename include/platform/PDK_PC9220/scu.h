/********************************************************************************
* File Name     : include/asm-arm/arch-socle/pc9220-scu.h
* Author        : Ryan Chen
* Description   : Socle PC9220 SCU Service Header
*
* Copyright (C) Socle Tech. Corp.
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by the Free Software Foundation;
* either version 2 of the License, or (at your option) any later version.
* This program is distributed in the hope that it will be useful,  but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

*   Version      : 2,0,0,1
*   History      :
*      1. 2008/02/22 ryanchen create this file
*
********************************************************************************/

#ifndef __SOCLE_PC9220_SCU_H_INCLUDED
#define __SOCLE_PC9220_SCU_H_INCLUDED

#define SOCLE_SCU_CPU_CLOCK_33	0
#define SOCLE_SCU_CPU_CLOCK_66	1
#define SOCLE_SCU_CPU_CLOCK_80	2
#define SOCLE_SCU_CPU_CLOCK_100	3
#define SOCLE_SCU_CPU_CLOCK_132	4
#define SOCLE_SCU_CPU_CLOCK_133	5
#define SOCLE_SCU_CPU_CLOCK_166	6
#define SOCLE_SCU_CPU_CLOCK_200	7
#define SOCLE_SCU_CPU_CLOCK_240	8
#if 0
#define SOCLE_SCU_CPU_CLOCK_258	9
#else
#define SOCLE_SCU_CPU_CLOCK_252 9
#endif
#define SOCLE_SCU_CPU_CLOCK_264	10
#define SOCLE_SCU_CPU_CLOCK_266	11
#define SOCLE_SCU_CPU_CLOCK_280	12
#define SOCLE_SCU_CPU_CLOCK_300	13
#define SOCLE_SCU_CPU_CLOCK_320	14
#define SOCLE_SCU_CPU_CLOCK_340	15
#define SOCLE_SCU_CPU_CLOCK_350	16
#define SOCLE_SCU_CPU_CLOCK_360	17
#define SOCLE_SCU_CPU_CLOCK_400	18

	/* UART clock */
#define SOCLE_SCU_UART_CLOCK_176	0

	/* CPU/AHB clock ratio	*/
#define SOCLE_SCU_CLOCK_RATIO_1_1		0
#define SOCLE_SCU_CLOCK_RATIO_2_1		1
#define SOCLE_SCU_CLOCK_RATIO_3_1		2
#define SOCLE_SCU_CLOCK_RATIO_4_1		3

/* UPLL configuration */
extern int socle_scu_upll_set (int clock);
extern int socle_scu_upll_get (void);
/* UPLL power down/normal	*/
extern void socle_scu_upll_normal (void);
extern void socle_scu_upll_power_down (void);
extern int socle_scu_upll_status_get (void);
/* MPLL configuration */
extern int socle_scu_mpll_set (int clock);	
extern int socle_scu_mpll_get (void);
/* MPLL power down/normal	*/
extern void socle_scu_mpll_normal (void);	
extern void socle_scu_mpll_power_down (void);
extern int socle_scu_mpll_status_get (void);	//return 1:normal	0:slow


	/*	mclk enable 	*/
#define SOCLE_SCU_MCLK_DCM		26
#define SOCLE_SCU_MCLK_STSDR		25
#define SOCLE_SCU_MCLK_NFC			24
#define SOCLE_SCU_MCLK_HDMA		23
#define SOCLE_SCU_MCLK_VOP			22
#define SOCLE_SCU_MCLK_UHC0		21
#define SOCLE_SCU_MCLK_UDC		20
#define SOCLE_SCU_MCLK_UHC1		19
#define SOCLE_SCU_MCLK_MAC		18
#define SOCLE_SCU_MCLK_CLCD		17
#define SOCLE_SCU_MCLK_GPIO3		16
#define SOCLE_SCU_MCLK_GPIO2		15
#define SOCLE_SCU_MCLK_GPIO1		14
#define SOCLE_SCU_MCLK_GPIO0		13
#define SOCLE_SCU_MCLK_ADC		12
#define SOCLE_SCU_MCLK_PWM		11
#define SOCLE_SCU_MCLK_WDT		10
#define SOCLE_SCU_MCLK_RTC			9
#define SOCLE_SCU_MCLK_TIMER		8
#define SOCLE_SCU_MCLK_SDMMC		7
#define SOCLE_SCU_MCLK_I2S			6
#define SOCLE_SCU_MCLK_I2C			5
#define SOCLE_SCU_MCLK_SPI1		4
#define SOCLE_SCU_MCLK_SPI0		3
#define SOCLE_SCU_MCLK_UART2		2
#define SOCLE_SCU_MCLK_UART1		1
#define SOCLE_SCU_MCLK_UART0		0

	/*	mclk enable/disable	*/
extern int socle_scu_mclk_enable (int ip);
extern int socle_scu_mclk_disable (int ip);


	/*	aclk enable 	*/
#define SOCLE_SCU_ACLK_SDC_HARDMACRO	19
#define SOCLE_SCU_ACLK_MAC_HARDMACRO	18
#define SOCLE_SCU_ACLK_LCDC_HARDMACRO	17
#define SOCLE_SCU_ACLK_UDC_HARDMACRO	16
#define SOCLE_SCU_ACLK_MPLL_OSC_CRYSTAL	15
#define SOCLE_SCU_ACLK_MPLL_OSC_CLK		14
#define SOCLE_SCU_ACLK_ST_SDR				13
#define SOCLE_SCU_ACLK_HS_UART			12
#define SOCLE_SCU_ACLK_LCD_VOP			11
#define SOCLE_SCU_ACLK_MAC				10
#define SOCLE_SCU_ACLK_UHC0				9
#define SOCLE_SCU_ACLK_UHC1				8
#define SOCLE_SCU_ACLK_UTMI_UHC1			7
#define SOCLE_SCU_ACLK_UTMI_UHC0			6
#define SOCLE_SCU_ACLK_UDC					5
#define SOCLE_SCU_ACLK_ADC					4
#define SOCLE_SCU_ACLK_I2S					3
#define SOCLE_SCU_ACLK_UART2				2
#define SOCLE_SCU_ACLK_UART1				1
#define SOCLE_SCU_ACLK_UART0				0

	/*	aclk enable/disable	*/
extern int socle_scu_aclk_enable (int ip);
extern int socle_scu_aclk_disable (int ip);





	/* read chip ID	*/
extern u32 socle_scu_chip_id (void);

/*	SCU_PLLPARAM_A	*/
	/* UPLL configuration */
extern int socle_scu_upll_set (int clock);
extern int socle_scu_upll_get (void);
	/* CPLL configuration */
extern int socle_scu_cpll_set (int clock);
extern int socle_scu_cpll_get (void);			//return cpll clock value

/*	SCU_PLLPARAM_B	*/
	/* UPLL power down/normal	*/
extern void socle_scu_upll_normal (void);
extern void socle_scu_upll_power_down (void);
extern int socle_scu_upll_status_get(void);		//1:normal , 0:down 
	/* CPLL power down/normal	*/
extern void socle_scu_cpll_normal (void);
extern void socle_scu_cpll_power_down (void);
extern int socle_scu_cpll_status_get(void);		//1:normal , 0:down 
	/* CPU/AHB clock ratio	*/
extern int socle_scu_clock_ratio_set (int ratio);
extern int socle_scu_clock_ratio_get (void);






#define SOCLE_SCU_UART_CLK_24			0
#define SOCLE_SCU_UART_CLK_UPLL		1
#define SOCLE_SCU_UART_CLK_UPLL_2	 	2
#define SOCLE_SCU_UART_CLK_UPLL_4	 	3

	/* PLL lock period	*/
extern void socle_scu_pll_lock_period_set (int period);		//period minimum value is 2
extern int socle_scu_pll_lock_period_get (void);
extern void socle_scu_adc_clk_div_set (int div);		//div minimum value is 2
extern int socle_scu_adc_clk_div_get (void);
extern int socle_scu_uart_clk_24_set (int uart);
extern int socle_scu_uart_clk_upll_set (int uart);
extern int socle_scu_uart_clk_upll_2_set (int uart);
extern int socle_scu_uart_clk_upll_4_set (int uart);
extern int socle_scu_uart_clk_get (int uart);


	/*	SDRAM data bus width status	*/
#define SOCLE_SCU_SDRAM_BUS_WIDTH_32	 1
#define SOCLE_SCU_SDRAM_BUS_WIDTH_16	 0
	/*	DCM mode setting status	*/
#define SOCLE_SCU_DCM_MODE_DCM			 1
#define SOCLE_SCU_DCM_MODE_NOR			 0
	/*	USB mode setting status	*/
#define SOCLE_SCU_USB_MODE_UDC			 1
#define SOCLE_SCU_USB_MODE_UHC			 0

	/*	Boot source selection status	*/
#define SOCLE_SCU_BOOT_NOR_16	 	3
#define SOCLE_SCU_BOOT_NOR_8	 	2
#define SOCLE_SCU_BOOT_NAND	 	1
#define SOCLE_SCU_BOOT_ISP_ROM	0

extern int socle_scu_sdram_bus_width_status (void);
extern int socle_scu_dcm_mode_status (void);
extern int socle_scu_usb_mode_status (void);
extern int socle_scu_boot_source_status (void);
extern int socle_scu_tps_mac_status (void);
extern int socle_scu_tps_mac_status (void);
extern int socle_scu_auto_boot_fail_status (void);
extern void socle_scu_pw_standbywfi_enable (int i);
extern void socle_scu_stop_mode_enable (int i);
extern void socle_scu_slow_mode_disable (int i);


extern void socle_scu_sw_reset(void);

extern void socle_scu_sw_remap(void);



#define SOCLE_DEVCON_NFC				0
#define SOCLE_DEVCON_MAC				1
#define SOCLE_DEVCON_TMR				2
#define SOCLE_DEVCON_PWM0				3
#define SOCLE_DEVCON_PWM1				4
#define SOCLE_DEVCON_EXT_INT0			5
#define SOCLE_DEVCON_EXT_INT0_NFIQ	6
#define SOCLE_DEVCON_EXT_INT1			7
#define SOCLE_DEVCON_LCDC				8
#define SOCLE_DEVCON_LCDC_VOP			9
#define SOCLE_DEVCON_SPI0				10
#define SOCLE_DEVCON_SPI1				11
#define SOCLE_DEVCON_I2S_TX			12
#define SOCLE_DEVCON_I2S_RX			13
#define SOCLE_DEVCON_I2S_TX_RX		14
#define SOCLE_DEVCON_I2C				15
#define SOCLE_DEVCON_SDMMC			16
#define SOCLE_DEVCON_UART0			17
#define SOCLE_DEVCON_UART1			18
#define SOCLE_DEVCON_UART2			19
#define SOCLE_DEVCON_WDT				23

extern int socle_scu_dev_enable(u32 dev);
extern int socle_scu_dev_disable(u32 dev);

extern void socle_scu_lcdc_clk_input_mpll_outpput(void);
extern void socle_scu_lcdc_clk_input_mpll_xin(void);
extern void socle_scu_hdma_req45_spi0(void);
extern void socle_scu_hdma_req45_spi1(void);
extern void socle_scu_uhc0_48clock_input_upll(void);
extern void socle_scu_uhc0_48clock_input_otg_phy(void);
extern void socle_scu_uhc1_48clock_input_upll(void);
extern void socle_scu_uhc1_48clock_input_otg_phy(void);
extern int socle_scu_hdma_req01_uart(int uart);
extern int socle_scu_hdma_req23_uart(int uart);
extern void socle_scu_wdt_reset_enable(int en);
extern void socle_scu_sw_reset_enable(int en);
extern void socle_scu_nfiq_polarity_high(int en);


/*	SCU_INFORM	*/
extern void socle_scu_info0_set (u32 inf);		
extern u32 socle_scu_info0_get (void);						//return information0 value
extern void socle_scu_info1_set (u32 inf);		
extern u32 socle_scu_info1_get (void);						//return information1 value	
extern void socle_scu_info2_set (u32 inf);		
extern u32 socle_scu_info2_get (void);						//return information2 value	
extern void socle_scu_info3_set (u32 inf);		
extern u32 socle_scu_info3_get (void);						//return information3 value	


extern unsigned long socle_get_cpu_clock (void);		//return CPU clock (Hz)
extern unsigned long socle_get_ahb_clock (void);		//return AHB clock (Hz)
extern unsigned long socle_get_apb_clock (void);		//return APB clock (Hz)
extern unsigned long socle_get_uart_clock (int uart);		//return UART clock (Hz)

#endif




