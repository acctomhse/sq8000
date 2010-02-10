#ifndef	_scu_h_include_
#define	_scu_h_include_

#include <type.h>

/*	SCU_MPLLCOM	*/
	/* the configuration value of MPLL for system clock usage */
extern void socle_scu_saturation_behavior_enable (void);
extern void socle_scu_saturation_behavior_disable (void);
extern int socle_scu_saturation_behavior_status (void);		//return 1:enable	0:disable	

extern void socle_scu_fast_locking_circuit_enable (void);
extern void socle_scu_fast_locking_circuit_disable (void);
extern int socle_scu_fast_locking_circuit_status (void);		//return 1:enable	0:disable				

extern void socle_scu_pll_reset (void);

extern void socle_scu_pll_power_down (void);
extern void socle_scu_pll_active (void);
extern int socle_scu_pll_power_down_status (void);			//return 1:power down	0:active

extern int socle_scu_pll_set (int clock);				//input CPU clock
extern int socle_scu_pll_get (void);					//return cpll clock value

	/* CPU clock */
#define SOCLE_SCU_CPU_CLOCK_33	0
#define SOCLE_SCU_CPU_CLOCK_66	1
#define SOCLE_SCU_CPU_CLOCK_80	2
#define SOCLE_SCU_CPU_CLOCK_100	3
#define SOCLE_SCU_CPU_CLOCK_132	4
#define SOCLE_SCU_CPU_CLOCK_133	5
#define SOCLE_SCU_CPU_CLOCK_200	6
#define SOCLE_SCU_CPU_CLOCK_264	7
#define SOCLE_SCU_CPU_CLOCK_266	8
#define SOCLE_SCU_CPU_CLOCK_280	9
#define SOCLE_SCU_CPU_CLOCK_300	10
#define SOCLE_SCU_CPU_CLOCK_320	11
#define SOCLE_SCU_CPU_CLOCK_340	12
#define SOCLE_SCU_CPU_CLOCK_350	13
#define SOCLE_SCU_CPU_CLOCK_360	14
#define SOCLE_SCU_CPU_CLOCK_400	15


/*	SCU_PLLLOCK		*/
	/* the lock time value of MPLL */
extern void socle_scu_mpll_lock_time_counter_set (int cnt);	//input PLL lock time counter for MPLL
extern int socle_scu_mpll_lock_time_counter_get (void);		//return PLL lock time counter for MPLL


/*	SCU_CLKSRC	*/
	/* the clock source control */
extern void socle_scu_clk_src_ext_input_set (void);
extern void socle_scu_clk_src_mpll_output_set (void);
extern int socle_scu_clk_src_status (void);					//return 1:MPLL output clock  0:External input clock

/*	SCU_CLKDIV	*/
	/* the clock divisor control */
extern int socle_scu_clock_ratio_set (int ratio);
extern int socle_scu_clock_ratio_get (void);

	/* socle_scu_clock_ratio_set /get parameter	*/
#define SOCLE_SCU_CLOCK_RATIO_2_1 0
#define SOCLE_SCU_CLOCK_RATIO_4_1	1
#define SOCLE_SCU_CLOCK_RATIO_8_1	2


/*	SCU_CLKOUT	*/
	/* the clock output configuration */
extern void socle_scu_clkout_src_mpll_output_set(void);		
extern void socle_scu_clkout_src_cpuclk_set(void);		
extern int socle_scu_clkout_src_status (void);				//return 1:CPUCLK clock  0:MPLL output clock

extern int socle_scu_clkout_divide_set (int div);
extern int socle_scu_clkout_divide_get (void);

	/* socle_scu_clkout_divide_set/get parameter	*/
#define SOCLE_SCU_CLKOUT_CLK_DIV_1	0
#define SOCLE_SCU_CLKOUT_CLK_DIV_2	1
#define SOCLE_SCU_CLKOUT_CLK_DIV_4	2
#define SOCLE_SCU_CLKOUT_CLK_DIV_8	3
#define SOCLE_SCU_CLKOUT_CLK_DIV_16	4
#define SOCLE_SCU_CLKOUT_CLK_DIV_32	5



/*	SCU_HCLKEN	*/
	/*  the enable bit for HCLK clock of individual peripheral */
extern int socle_scu_hclk_enable (int ip);
extern int socle_scu_hclk_disable (int ip);
extern int socle_scu_hclk_status (int ip);			//return 1:enable	0:disable	

	/* socle_scu_hclk_xxx parameter		*/
#define SOCLE_SCU_HCLKEN_UHC		0
#define SOCLE_SCU_HCLKEN_DMA		1
#define SOCLE_SCU_HCLKEN_CLCD		2
#define SOCLE_SCU_HCLKEN_NFC		3
#define SOCLE_SCU_HCLKEN_SRAM		4


/*	SCU_PCLKEN	*/
	/*  the enable bit for PCLK clock of individual peripheral*/
extern int socle_scu_pclk_enable (int ip);
extern int socle_scu_pclk_disable (int ip);
extern int socle_scu_pclk_status (int ip);			//return 1:enable	0:disable	
	/* socle_scu_pclk_xxx parameter		*/
#define SOCLE_SCU_PCLKEN_UART0	0
#define SOCLE_SCU_PCLKEN_UART1	1
#define SOCLE_SCU_PCLKEN_SPI		2
#define SOCLE_SCU_PCLKEN_I2C0		3
#define SOCLE_SCU_PCLKEN_I2C1		4
#define SOCLE_SCU_PCLKEN_I2C2		5
#define SOCLE_SCU_PCLKEN_I2S		6
#define SOCLE_SCU_PCLKEN_SDMMC	7
#define SOCLE_SCU_PCLKEN_TIMER	8
#define SOCLE_SCU_PCLKEN_GPIO		9
#define SOCLE_SCU_PCLKEN_RTC		10
#define SOCLE_SCU_PCLKEN_WDT		11
#define SOCLE_SCU_PCLKEN_PWM		12


/*	SCU_SCLKEN	*/
	/*  the enable bit for special clock of individual peripheral */
extern int socle_scu_sclk_enable (int ip);
extern int socle_scu_sclk_disable (int ip);
extern int socle_scu_sclk_status (int ip);			//return 1:enable	0:disable	

	/* socle_scu_sclk_xxx parameter		*/
#define SOCLE_SCU_SCLKEN_UART0	0
#define SOCLE_SCU_SCLKEN_UART1	1
#define SOCLE_SCU_SCLKEN_I2S		2
#define SOCLE_SCU_SCLKEN_UHC		3
#define SOCLE_SCU_SCLKEN_LCD		4
#define SOCLE_SCU_SCLKEN_RTC		5


/*	SCU_SWRST	*/
	/*	generate software reset	*/
extern void socle_scu_sw_reset (void);

/*	SCU_REMAP	*/
	/*	generate software reset	*/
extern void socle_scu_sw_remap (void);

/*	SCU_PWRMODE	*/
	/*	the power mode control bit	*/
extern void socle_scu_power_mode_idle_set (void);
extern void socle_scu_power_mode_stop_set (void);
extern void socle_scu_power_mode_sleep_set (void);


/*	SCU_PWRCFG	*/
	/*	the power management configuration bit	*/
extern int socle_scu_pwrcfg_filter_enable (int ip);
extern int socle_scu_pwrcfg_filter_disable (int ip);
extern int socle_scu_pwrcfg_filter_status (int ip);			//return 1:enable	0:disable	

	/*	socle_scu_pwrcfg_filter_xxx parameter	*/
#define SOCLE_SCU_PWRCFG_EXT_INT0	0
#define SOCLE_SCU_PWRCFG_EXT_INT1	1
#define SOCLE_SCU_PWRCFG_GPIO0		2
#define SOCLE_SCU_PWRCFG_GPIO1		3
#define SOCLE_SCU_PWRCFG_GPIO2		4
#define SOCLE_SCU_PWRCFG_GPIO3		5
#define SOCLE_SCU_PWRCFG_GPIO4		6
#define SOCLE_SCU_PWRCFG_GPIO5		7
#define SOCLE_SCU_PWRCFG_GPIO6		8
#define SOCLE_SCU_PWRCFG_GPIO7		9

extern int socle_scu_ext_int0_trigger_type_set (int type);
extern int socle_scu_ext_int0_trigger_type_get (void);
extern int socle_scu_ext_int1_trigger_type_set (int type);
extern int socle_scu_ext_int1_trigger_type_get (void);

	/*	socle_scu_ext_intx_trigger_type_set /get parameter	*/
#define SOCLE_SCU_PWRCFG_HIGH_LEVEL		0
#define SOCLE_SCU_PWRCFG_LOW_LEVEL		1
#define SOCLE_SCU_PWRCFG_RISING_LEVEL	2
#define SOCLE_SCU_PWRCFG_FALLING_LEVEL	3


extern int socle_scu_stabdbywifi_set (int type);
extern int socle_scu_stabdbywifi_get (void);

	/*	socle_scu_stabdbywifi_set /get parameter	*/
#define SOCLE_SCU_PWRCFG_STANDBYWIFI_IGNORE	0
#define SOCLE_SCU_PWRCFG_STANDBYWIFI_IDLE		1
#define SOCLE_SCU_PWRCFG_STANDBYWIFI_STOP		2


extern void socle_scu_rtc_alarm_int_wakeup_mask (void);
extern void socle_scu_rtc_alarm_int_wakeup_unmask (void);
extern int socle_scu_rtc_alarm_int_wakeup_mask_status (void);		//return 1:masked  0:no mask

extern void socle_scu_gpio_inf_wakeup_mask (void);
extern void socle_scu_gpio_inf_wakeup_unmask (void);
extern int socle_scu_gpio_inf_wakeup_mask_status (void);			//return 1:masked  0:no mask

extern int socle_scu_ext_int_wakeup_mask_set (int set);	
extern int socle_scu_ext_int_wakeup_mask_get (void);

	/*	socle_scu_ext_int_wakeup_mask_set /get  parameter	*/
#define SOCLE_SCU_PWRCFG_MASK_NO				0
#define SOCLE_SCU_PWRCFG_MASK_EXT_INT0		1
#define SOCLE_SCU_PWRCFG_MASK_EXT_INT1		2
#define SOCLE_SCU_PWRCFG_MASK_EXT_INT01		3


/*	SCU_PWREN	*/
	/*	the internal core power gating control	*/	
extern void socle_scu_embedded_sram_retent_enable (void);	
extern void socle_scu_embedded_sram_retent_disable (void);		
extern int socle_scu_embedded_sram_retent_status (void);			//return 1:enable  0:disable
extern void socle_scu_embedded_sram_block1_reset (void);	
extern void socle_scu_embedded_sram_block0_reset (void);	
extern void socle_scu_uhc_block_reset (void);	
extern void socle_scu_embedded_sram_core_power_enable (void);	
extern void socle_scu_embedded_sram_core_power_disable (void);		
extern int socle_scu_embedded_sram_core_power_status (void);			//return 1:enable  0:disable
extern void socle_scu_uhc_core_power_enable (void);	
extern void socle_scu_uhc_core_power_disable (void);		
extern int socle_scu_uhc_core_power_status (void);			//return 1:enable  0:disable


/*	SCU_RSTCNT	*/
	/*	reset status bits	*/
extern void socle_scu_rstcnt_pclk_counter_set (int cnt);		//input internal reset counter 
extern int socle_scu_rstcnt_pclk_counter_get (void);			//return input internal reset counter 
extern void socle_scu_rstcnt_ext_counter_set (int cnt);			//input external power source reset counter 
extern int socle_scu_rstcnt_ext_counter_get (void);			//return external power source reset counter 

/*	SCU_RSTSTAT	*/
	/*	the power mode control bit	*/
extern int socle_scu_rststat_sw_reset_status (void);			//return 1:sw reset  0:other reset
extern int socle_scu_rststat_wakeup_reset_status (void);		//return 1:wakeup reset  0:other reset
extern int socle_scu_rststat_wdt_reset_status (void);			//return 1:watch-dog reset  0:other reset
extern int socle_scu_rststat_hw_reset_status (void);			//return 1:hw reset  0:other reset

/*	SCU_WKUPSTAT	*/
	/*	the trigger source for exiting SLEEP mode	*/
extern int socle_scu_wkupstat_status (void);					//return -1 : no wake-up source			
extern int socle_scu_wkupstat_clear (int src);		

	/*	socle_scu_wkupstat_xxx triggle source		*/
#define SOCLE_SCU_WKUPSTAT_EXT_INT	(1 << 0)
#define SOCLE_SCU_WKUPSTAT_GPIO		(1 << 1)
#define SOCLE_SCU_WKUPSTAT_RTC		(1 << 2)	



/*	SCU_MEMISLP		*/
	/*	the output pin value of ST/SDR memory interface in SLEEP mode	*/
extern int socle_scu_memislp_ouput0_set (int pin);
extern int socle_scu_memislp_ouput1_set (int pin);
extern int socle_scu_memislp_ouput_dis_input_set (int pin);
extern int socle_scu_memislp_status (int pin);					//return 0:output0  1:output1  2:output disable or input

	/*	socle_scu_memislp_xxx pin parameter		*/
#define SOCLE_SCU_MEMISLP_ST_WEN		0
#define SOCLE_SCU_MEMISLP_ST_OEN		1
#define SOCLE_SCU_MEMISLP_ST_CSN		2	
#define SOCLE_SCU_MEMISLP_SD_CLKOUT	3
#define SOCLE_SCU_MEMISLP_SD_CKE		4
#define SOCLE_SCU_MEMISLP_SD_BA		5
#define SOCLE_SCU_MEMISLP_SD_DQM		6
#define SOCLE_SCU_MEMISLP_SD_WEN		7
#define SOCLE_SCU_MEMISLP_SD_RASN	8
#define SOCLE_SCU_MEMISLP_SD_CASN	9
#define SOCLE_SCU_MEMISLP_SD_CEN		10
#define SOCLE_SCU_MEMISLP_SD_DQ		11
#define SOCLE_SCU_MEMISLP_SD_ADDR	12


/*	SCU_NFISLP	*/
	/*	the output pin value of NAND-Flash memory interface in SLEEP mode	*/
extern int socle_scu_nfislp_ouput0_set (int pin);
extern int socle_scu_nfislp_ouput1_set (int pin);
extern int socle_scu_nfislp_ouput_dis_input_set (int pin);
extern int socle_scu_nfislp_status (int pin);						//return 0:output0  1:output1  2:output disable or input

	/*	socle_scu_memislp_xxx pin parameter		*/
#define SOCLE_SCU_NFISLP_NF_IO			0
#define SOCLE_SCU_NFISLP_NF_CLE		1
#define SOCLE_SCU_NFISLP_NF_ALE		2	
#define SOCLE_SCU_NFISLP_NF_CEN		3
#define SOCLE_SCU_NFISLP_NF_REN		4
#define SOCLE_SCU_NFISLP_NF_WEN		5
#define SOCLE_SCU_NFISLP_NF_WPN		6


/*	SCU_USBISLP	*/
	/*	the output pin value of USB interface in SLEEP mode	*/
extern int socle_scu_usbislp_ouput0_set (int pin);
extern int socle_scu_usbislp_ouput1_set (int pin);
extern int socle_scu_usbislp_ouput_dis_input_set (int pin);
extern int socle_scu_usbislp_status (int pin);						//return 0:output0  1:output1  2:output disable or input

	/*	socle_scu_memislp_xxx pin parameter		*/
#define SOCLE_SCU_USBISLP_VBUS_UHC			0
#define SOCLE_SCU_USBISLP_USB_SUSPENDM		1
#define SOCLE_SCU_USBISLP_USB_POR				2	
#define SOCLE_SCU_USBISLP_USB_FS_XCVR_OWN	3
#define SOCLE_SCU_USBISLP_USB_TX_ENB			4
#define SOCLE_SCU_USBISLP_USB_FS_SE0_EXT		5
#define SOCLE_SCU_USBISLP_USB_FS_DATA_EXT	6
#define SOCLE_SCU_USBISLP_USB_TX_VALIDH		7
#define SOCLE_SCU_USBISLP_USB_TX_VALID		8
#define SOCLE_SCU_USBISLP_USB_DATAI			9
#define SOCLE_SCU_USBISLP_USB_DM_PULLDOWN	10	
#define SOCLE_SCU_USBISLP_USB_DP_PULLDOWN	11
#define SOCLE_SCU_USBISLP_USB_XCVR_SEL		12
#define SOCLE_SCU_USBISLP_USB_TERM_SEL		13
#define SOCLE_SCU_USBISLP_USB_OPMODE			14


/*	SCU_LCDISLP	*/
	/*	the output pin value of LCD interface in SLEEP mode	*/
extern int socle_scu_lcdislp_ouput0_set (int pin);
extern int socle_scu_lcdislp_ouput1_set (int pin);
extern int socle_scu_lcdislp_ouput_dis_input_set (int pin);
extern int socle_scu_lcdislp_status (int pin);						//return 0:output0  1:output1  2:output disable or input

	/*	socle_scu_memislp_xxx pin parameter		*/
#define SOCLE_SCU_LCDISLP_LCD_POWER		0
#define SOCLE_SCU_LCDISLP_LCD_LP			1
#define SOCLE_SCU_LCDISLP_LCD_LE			2	
#define SOCLE_SCU_LCDISLP_LCD_FP			3
#define SOCLE_SCU_LCDISLP_LCD_DATA		4
#define SOCLE_SCU_LCDISLP_LCD_CP			5
#define SOCLE_SCU_LCDISLP_LCD_AC			6


/*	SCU_PERI0SLP	*/
	/*	the output pin value of peripherals interface including UART, SPI, I2C, I2S, and PWM in SLEEP mode	*/
extern int socle_scu_peri0slp_ouput0_set (int pin);
extern int socle_scu_peri0slp_ouput1_set (int pin);
extern int socle_scu_peri0slp_ouput_dis_input_set (int pin);
extern int socle_scu_peri0slp_status (int pin);						//return 0:output0  1:output1  2:output disable or input

	/*	socle_scu_memislp_xxx pin parameter		*/
#define SOCLE_SCU_PERI0SLP_PWM				0
#define SOCLE_SCU_PERI0SLP_I2S_SDO			1
#define SOCLE_SCU_PERI0SLP_I2S_TXLRCK			2	
#define SOCLE_SCU_PERI0SLP_I2S_TXSCLK			3
#define SOCLE_SCU_PERI0SLP_I2C2_SDA			4
#define SOCLE_SCU_PERI0SLP_I2C2_SCL			5
#define SOCLE_SCU_PERI0SLP_I2C1_SDA			6
#define SOCLE_SCU_PERI0SLP_I2C1_SCL			7
#define SOCLE_SCU_PERI0SLP_I2C0_SDA			8
#define SOCLE_SCU_PERI0SLP_I2C0_SCL			9	
#define SOCLE_SCU_PERI0SLP_SPI_MOSI			10
#define SOCLE_SCU_PERI0SLP_SPI_SSN			11
#define SOCLE_SCU_PERI0SLP_SPI_SCK			12
#define SOCLE_SCU_PERI0SLP_UART1_TXD			13
#define SOCLE_SCU_PERI0SLP_UART0_TXD			14
#define SOCLE_SCU_PERI0SLP_GPIO				15


/*	SCU_PERI1SLP	*/
	/*	the output pin value of peripherals interface including SD/MMC in SLEEP mode	*/
extern int socle_scu_peri1slp_ouput0_set (int pin);
extern int socle_scu_peri1slp_ouput1_set (int pin);
extern int socle_scu_peri1slp_ouput_dis_input_set (int pin);
extern int socle_scu_peri1slp_status (int pin);						//return 0:output0  1:output1  2:output disable or input

	/*	socle_scu_memislp_xxx pin parameter		*/
#define SOCLE_SCU_PERI1SLP_SDC_CLK			0
#define SOCLE_SCU_PERI1SLP_SDC_PWR			1
#define SOCLE_SCU_PERI1SLP_SDC_DATA			2	
#define SOCLE_SCU_PERI1SLP_SDC_CMD			3


/*	SCU_SLPEN	*/
	/*	SLEEP mode IO configuration register	*/
extern void socle_scu_sleep_io_conf_slpen_set (void);
extern void socle_scu_sleep_io_conf_auto_set (void);
extern int socle_scu_sleep_io_conf_status (void);				//return 1:automatically by SLEEP mode	0:set by SCU_SLPEN[0] bit	
extern void socle_scu_sleep_io_en_ext_output_set (void);
extern void socle_scu_sleep_io_en_normal_set (void);
extern int socle_scu_sleep_io_en_status (void);				//return 1:external output pins are controlled by SCU_xxx	0:change output into normal function


/*	SCU_IOMODE	*/
	/*	GPIO function register	*/
extern void socle_scu_iomode_gpio7_set (void);
extern void socle_scu_iomode_i2c2_sda_set (void);
extern int socle_scu_iomode_gpio7_status (void);				//return 1:I2C2_SDA function	0:GPIO[7] function	
extern void socle_scu_iomode_gpio6_set (void);
extern void socle_scu_iomode_i2c2_scl_set (void);
extern int socle_scu_iomode_gpio6_status (void);				//return 1:I2C2_SCL function	0:GPIO[6] function	
extern void socle_scu_iomode_gpio5_set (void);
extern void socle_scu_iomode_i2c1_sda_set (void);
extern int socle_scu_iomode_gpio5_status (void);				//return 1:I2C1_SDA function	0:GPIO[5] function	
extern void socle_scu_iomode_gpio4_set (void);
extern void socle_scu_iomode_i2c1_scl_set (void);
extern int socle_scu_iomode_gpio4_status (void);				//return 1:I2C1_SCL function	0:GPIO[4] function	
extern void socle_scu_iomode_gpio0_set (void);
extern void socle_scu_iomode_pllout_set (void);
extern int socle_scu_iomode_gpio0_status (void);				//return 1:PLLOUT function	0:GPIO[0] function	


/*	SCU_CHIPID	*/
	/*	Chip ID register	*/
extern u32 socle_scu_chip_id_get (void);							//return chip ID	

/*	SCU_INFORM0	*/
	/*	User defined information register	*/
extern void socle_scu_info0_set (u32 inf);		
extern u32 socle_scu_info0_get (void);						//return information0 value		

/*	SCU_INFORM1	*/
	/*	User defined information register	*/
extern void socle_scu_info1_set (u32 inf);		
extern u32 socle_scu_info1_get (void);						//return information1 value		

/*	SCU_INFORM2	*/
	/*	User defined information register	*/
extern void socle_scu_info2_set (u32 inf);		
extern u32 socle_scu_info2_get (void);						//return information2 value		

/*	SCU_INFORM3	*/
	/*	User defined information register	*/
extern void socle_scu_info3_set (u32 inf);		
extern u32 socle_scu_info3_get (void);						//return information3 value		

/*	SCU_CHIPMD	*/
	/*	PLL related setting	*/
extern int socle_scu_dcm_mode_status (void);				//return 1:DCM test mode  0:Normal mode	
extern void socle_scu_pmu_mode_pmu_debug_set (void);
extern void socle_scu_pmu_mode_normal_set (void);
extern int socle_scu_pmu_mode_status (void);				//return 1:Debug and test mode  0:Normal function mode	


/*	SCU_PLL	*/
	/*	PLL related setting	*/
extern int socle_scu_pll_relock_status (void);				//return 0:PLL is locked  1:PLL is start to re-configure 2:pPLL is on re-configuring and unlocked	
extern void socle_scu_pll_lock_counter_enable (void);		
extern void socle_scu_pll_lock_counter_disable (void);		
extern int socle_scu_pll_lock_counter_status (void);		//return 1:disable  0:enable


/*	SCU_DBCT	*/
	/*	PLL related setting	*/
extern void socle_scu_debounce_time_set (int time);				
extern int socle_scu_debounce_time_get (void);				//return Debounce time for GPIO and EXT_INT noise filter


extern unsigned long socle_get_cpu_clock (void);		//return CPU clock (Hz)
extern unsigned long socle_get_ahb_clock (void);		//return AHB clock (Hz)
extern unsigned long socle_get_apb_clock (void);		//return APB clock (Hz)


#endif	//scu_h_included_
