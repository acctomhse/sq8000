#ifndef	_scu_h_include_
#define	_scu_h_include_

#include <type.h>

	/* CPU clock */
#define SOCLE_SCU_CPU_CLOCK_33	0
#define SOCLE_SCU_CPU_CLOCK_40	1
#define SOCLE_SCU_CPU_CLOCK_50	2
#define SOCLE_SCU_CPU_CLOCK_66	3
#define SOCLE_SCU_CPU_CLOCK_83	4
#define SOCLE_SCU_CPU_CLOCK_90	5
#define SOCLE_SCU_CPU_CLOCK_100	6
#define SOCLE_SCU_CPU_CLOCK_120	7
#define SOCLE_SCU_CPU_CLOCK_132	8
#define SOCLE_SCU_CPU_CLOCK_133	9
#define SOCLE_SCU_CPU_CLOCK_166	10

	/* UART clock */
#define SOCLE_SCU_UART_CLOCK_88	0

	/* CPU/AHB clock ratio	*/
#define SOCLE_SCU_CLOCK_RATIO_1_1		0
#define SOCLE_SCU_CLOCK_RATIO_2_1		1
#define SOCLE_SCU_CLOCK_RATIO_3_1		2
#define SOCLE_SCU_CLOCK_RATIO_4_1		3

	/* select USB Tranceiver play Downstream or Upstream	*/
#define SOCLE_SCU_USB_TRAN_UPSTREAM		0
#define SOCLE_SCU_USB_TRAN_DOWNSTREAM	1

	/*	pclk enable 	*/
#define SOCLE_SCU_ADCCLK			24
#define SOCLE_SCU_PCLK_UART3		21
#define SOCLE_SCU_PCLK_UART2		20
#define SOCLE_SCU_PCLK_UART1		19
#define SOCLE_SCU_PCLK_UART0		18
#define SOCLE_SCU_PCLK_ADC			17
#define SOCLE_SCU_PCLK_PWM		16
#define SOCLE_SCU_PCLK_SDC			15
#define SOCLE_SCU_PCLK_I2S			14
#define SOCLE_SCU_PCLK_I2C			13
#define SOCLE_SCU_PCLK_SPI			12
#define SOCLE_SCU_PCLK_GPIO		11
#define SOCLE_SCU_PCLK_WDT		10
#define SOCLE_SCU_PCLK_RTC			9
#define SOCLE_SCU_PCLK_TMR			8
#define SOCLE_SCU_PCLK_LCD			6
#define SOCLE_SCU_PCLK_NFC			5
#define SOCLE_SCU_PCLK_UDC			4
#define SOCLE_SCU_PCLK_UHC			3
#define SOCLE_SCU_PCLK_MAC			2
#define SOCLE_SCU_PCLK_HDMA		1
#define SOCLE_SCU_PCLK_SDRSTMC	0

	/*	SDRAM data bus width status	*/
#define SOCLE_SCU_SDRAM_BUS_WIDTH_32	 1
#define SOCLE_SCU_SDRAM_BUS_WIDTH_16	 0

	/*	Boot source selection status	*/
#define SOCLE_SCU_BOOT_NOR_16	 	3
#define SOCLE_SCU_BOOT_NOR_8	 	2
#define SOCLE_SCU_BOOT_NAND	 	1
#define SOCLE_SCU_BOOT_ISP_ROM	0




/*	SCU_P7CID	*/
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
	/* PLL lock period	*/
extern void socle_scu_pll_lock_period_set (int period);
	
/*	SCU_CHIPCFG_A	*/
	/*	Force USB PHY's PLL Powered in Suspend	*/
extern void socle_scu_usb_pll_powered (void);
extern void socle_scu_usb_pll_power_save (void);
	/*	DCFG MODE	*/
extern void socle_scu_dcfg_mode_set (int mode);
	/*	UCFG MODE	*/
extern void socle_scu_ucfg_mode_set (int mode);
	/*	Config UARTx as irDA function	*/
extern int socle_scu_irda_enable (int uart);
	/*	Config UARTx as UART function	*/
extern int socle_scu_irda_disable (int uart);
	/*	select which UART will occupy HDMA request 2/3 for UART Tx/Rx	*/
extern int socle_scu_hdma_req23_uart (int uart);
	/*	select which UART will occupy HDMA request 0/1 for UART Tx/Rx	*/
extern int socle_scu_hdma_req01_uart (int uart);
	/*	select USB Tranceiver play Downstream or Upstream	*/
extern void socle_scu_usb_tranceiver_downstream (void);		//UHC
extern void socle_scu_usb_tranceiver_upstream (void);		//UDC
	/*	select fast IRQ polarity	*/
extern void socle_scu_fast_irq_active_high (void);
extern void socle_scu_fast_irq_active_low (void);
	/*	select USB port over current function polarity	*/
extern void socle_scu_usb_port_over_current_active_high (void);
extern void socle_scu_usb_port_over_current_active_low (void);
	/*	select fast IRQ and USB port over current function	*/
extern void socle_scu_fast_irq_enable (void);
extern void socle_scu_usb_port_over_current_enable (void);
	
/*	SCU_CHIPCFG_B	*/
	/*	ADC clock duty period	*/
extern void socle_scu_adc_clock_duty_period (int period);
	
/*	SCU_CLKCFG	*/
	/*	pclk enable/disable	*/
extern int socle_scu_pclk_enable (int ip);
extern int socle_scu_pclk_disable (int ip);

/*	SCU_REMAP	*/
	/*	SDRAM data bus width status	*/
extern int socle_scu_sdram_bus_width_status (void);
	/*	MAC Tx process stop status	*/
extern int socle_scu_mac_tx_stop_status (void);
	/*	MAC Rx process stop status	*/
extern int socle_scu_mac_rx_stop_status (void);
	/*	UCFG Mode6 status	*/
extern int socle_scu_ucfg_mode6_status (void);
	/*	UCFG Mode5 status	*/
extern int socle_scu_ucfg_mode5_status(void);
	/*	UCFG Mode4 status	*/
extern int socle_scu_ucfg_mode4_status (void);
	/*	UCFG Mode3 status	*/
extern int socle_scu_ucfg_mode3_status (void);
	/*	UCFG Mode2 status	*/
extern int socle_scu_ucfg_mode2_status (void);
	/*	UCFG Mode1 status	*/
extern int socle_scu_ucfg_mode1_status (void);
	/*	Boot source selection status	*/
extern int socle_scu_boot_source_status (void);
	/*	FIQDIS from ARM7 status	*/
extern int socle_scu_fiq_dis_status (void);
	/*	IRQDIS from ARM7 status	*/
extern int socle_scu_irq_dis_status (void);
	/*	auto boot fail indicator from NFC status	*/
extern int socle_scu_nand_boot_fail_status (void);
	/*	pll lock status	*/
extern int socle_scu_pll_lock_status (void);
	/*	stop mode -- systen clock	*/
extern void socle_scu_stop_mode_enable (void);
extern void socle_scu_stop_mode_disable (void);
	/*	sleep mode -- cpu clock	*/
extern void socle_scu_sleep_mode_enable (void);
extern void socle_scu_sleep_mode_disable (void);
	/*	normal mode -- use PLL clock or Base clock	*/
extern void socle_scu_normal_mode_enable (void);
extern void socle_scu_normal_mode_disable (void);
	/*	decoder remap function	*/
extern void socle_scu_remap (void);


extern unsigned long socle_get_cpu_clock (void);		//return CPU clock (Hz)
extern unsigned long socle_get_ahb_clock (void);		//return AHB clock (Hz)
extern unsigned long socle_get_apb_clock (void);		//return APB clock (Hz)
extern unsigned long socle_get_uart_clock (void);		//return UART clock (Hz)

#endif	//_irqs_h_included_
