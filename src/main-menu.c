#include "test_item.h"

//Test entry for all IPs
extern int gpio_test(int autotest);
extern int socle_mp_gpio_test(int autotest);
extern int MemoryTesting(int autotest);
extern int CacheTesting(int autotest);
extern int INTRTesting(int autotest);
extern int UARTTesting(int autotest);
extern int TIMERTesting(int autotest);
extern int wdt_watching(int autotest);
extern int RTCTesting(int autotest);
extern int SPITesting(int autotest);
extern int UDCTesting(int autotest);
extern int PCITesting(int autotest);
extern int MACTesting(int autotest);
extern int ExtendBoardTesting(int autotest);
extern int Extend_HDMA_Testing(int autotest);
extern int HDMATesting(int autotest);
extern int PANTHER7_HDMATesting(int autotest);
extern int MULTIMEM_Testing(int autotest);
extern int I2CTesting(int autotest);
extern int MPEG4Testing (int autotest);
extern int SDTesting(int autotest);
extern int A2ATesting(int autotest);
extern int NORFlashTesting(int autotest);
extern int NOR_MXIC_Test(int autotest);
extern int nand_test(int autotest);
extern int EHCITesting(int autotest);
extern int OHCITesting(int autotest);
extern int IDETesting(int autotest);
extern int MailboxTesting(int autotest);
extern int I2STesting(int autotest);
extern int scu_test(int autotest);
extern int LCD_PWM_Testing(int autotest);
extern int SOCLE_LCD_Testing (int autotest);
extern int pwm_adc_test(int autotest);
extern int VIP_VOP_Testing(int autotest);
extern int lcm_test(int autotest);
extern int power_save_test(int autotest);
extern int sram_test(int autotest);

struct test_item main_test_items [] = {
	
#ifdef	CONFIG_GPIO
	{
		"GPIO Testing",
		gpio_test,
		1,
#ifdef	CONFIG_INR_PC7230
		0
#else
		1
#endif
	},
#endif

#ifdef CONFIG_MP_GPIO
	{
		"MP GPIO Testing",
		socle_mp_gpio_test,
		1,
		1
	},
#endif

#ifdef	CONFIG_MEMORY
	{
		"Memory Testing",
		MemoryTesting,
		1,
		1
	},
#endif

#ifdef	CONFIG_CACHE
	{
		"CACHE Testing",
		CacheTesting,
		1,
		1
	},
#endif

#ifdef	CONFIG_INTERRUPT
	{
		"Interrupt Testing",
		INTRTesting,
		1,
		1
	},
#endif
	
#ifdef	CONFIG_UART
	{
		"UART Testing",
		UARTTesting,
		1,
		1
	},
#endif

#ifdef	CONFIG_TIMER
	{
		"Timer Testing",
		TIMERTesting,
		1,
		1
	},
#endif

#ifdef	CONFIG_RTC
	{
		"RTC Testing",
		RTCTesting,
		1,
		1
	},
#endif

#ifdef CONFIG_SPI
	{
		"SPI Testing",
		SPITesting,
		1,
		1
	},
#endif

#ifdef	CONFIG_UDC
	{	
		"UDC Testing",
		UDCTesting,
		0,
		1
	},
#endif

#ifdef	CONFIG_PCI
	{
		"PCI Testing",
		PCITesting,
		1,
		1
	},
#endif

#ifdef	CONFIG_MAC
	{
		"MAC Testing",
		MACTesting,
		1,
		1
	},
#endif

#ifdef	CONFIG_PSEUDO_EXTBOARD
	{
		"Ext Board Pseudo Device Testing",
		ExtendBoardTesting,
		1,
		1
	},
#endif

#ifdef  CONFIG_HDMA_EXTBOARD
	{
		"Ext Board HDMA Testing",
		Extend_HDMA_Testing,
		1,
		1
	},
#endif

#ifdef	CONFIG_HDMA
	{
		"HDMA Testing",
		HDMATesting,
		1,
		1
	},
#endif

#ifdef CONFIG_PANTHER7_HDMA
	{
		"HDMA Testing for Panther7",
		PANTHER7_HDMATesting,
		1,
		1
	},
#endif

#ifdef  CONFIG_MULTI_MEM
	{
		"Multi-port MEM Testing",
		MULTIMEM_Testing,
		1,
		1
	},
#endif

#ifdef  CONFIG_I2S
        {
                "I2S Testing",
                I2STesting,
                1,
                1
        },
#endif


#ifdef	CONFIG_I2C
	{
		"I2C Testing",
		I2CTesting,
		1,
		1
	},
#endif

#ifdef CONFIG_MPEG4
	{
		"MPEG4 Testing",
		MPEG4Testing,
		1,
		1
	},
#endif

#ifdef CONFIG_SDMMC
	{
		"SDMMC Testing",
		SDTesting,
		1,
		1
	},
#endif

#ifdef CONFIG_A2ADMA
	{
		"A2A DMA Testing",
		A2ATesting,
		1,
		1
	},
#endif

#ifdef CONFIG_NOR
	{
		"NOR-FLASH Testing",
		NORFlashTesting,
		1,
		1
	},
#endif

#ifdef CONFIG_NOR_MXIC
	{
		"NOR-FLASH-MXIC Testing",
		NOR_MXIC_Test,
		1,
		1
	},
#endif

#ifdef CONFIG_NAND
	{
		"NAND-FLASH Testing",
		nand_test,
		1,
		1
	},
#endif

#ifdef CONFIG_EHCI
	{
		"EHCI Testing",
		EHCITesting,
		1,
		1
	},
#endif

#ifdef CONFIG_OHCI
	{
		"OHCI Testing",
		OHCITesting,
		1,
		1
	},
#endif

#ifdef CONFIG_IDE
	{
		"IDE Testing",
		IDETesting,
		1,
		1
	},
#endif

#ifdef CONFIG_MAILBOX
	{
		"MailBox Testing",
		MailboxTesting,
		1,
		1
	},
#endif

#ifdef CONFIG_SCU
	{
		"SCU Testing",
		scu_test,
		1,
		1
	},
#endif

#ifdef CONFIG_LCD_PWM
	{
		"LCD & PWM Testing",
		LCD_PWM_Testing,
		1,
		1
	},
#endif

#ifdef CONFIG_SOCLE_LCD
        {
                "SOCLE LCD Testing",
                SOCLE_LCD_Testing,
                1,
                1
        },
#endif
#ifdef CONFIG_ADC_PWM
	{
		"ADC & PWM Testing",
		pwm_adc_test,
		1,
		1
	},
#endif

#ifdef	CONFIG_VIOP
	{
		"VIOP Testing",
		VIP_VOP_Testing,
		1,
		1
	},
#endif

#ifdef	CONFIG_LCM
	{
		"LCM Testing",
		lcm_test,
		1,
		1
	},
#endif

#ifdef CONFIG_7230_PS
	{
		"7230 POWER SAVING Testing",
		power_save_test,
		1,
		1
	},
#endif

#ifdef CONFIG_SRAM
	{
		"SRAM Testing",
		sram_test,
		1,
		1
	},
#endif

///////////////////////////////////////////////
// WDT must be put last to reboot the board!!
#ifdef	CONFIG_WDT
	{
		"WDT Testing",
		wdt_watching,
		1,
		1
	},
#endif

};

struct test_item_container main_test_item_container = {

#if defined (CONFIG_LDK3V21)
	"Leopard Development Kit III.A Test Program",
	"LDK3V21",
#elif defined (CONFIG_LDK5)
	"Leopard Development Kit V Test Program",
	"LDK5",
#elif defined (CONFIG_SEDK)
	"Soc Enable Development Kit Test Program",
	"SEDK",
#elif defined (CONFIG_CDK)
	"Cheetah Development Kit Test Program",
	"CDK",
#elif defined (CONFIG_PDK)
	"Panterh7 Development Kit Test Program",
	"PDK",
#elif defined (CONFIG_PC7210)
	"PDK-PC7210 Development Kit Test Program",
	"PDK-PC7210",
#elif defined (CONFIG_PC9002)
	"PDK-PC9002 Development Kit Test Program",
	"PDK-PC9002",
#elif defined (CONFIG_PC9220)
	"PDK-PC9220 Development Kit Test Program",
	"PDK-PC9220",
#elif defined (CONFIG_SCDK)
	"SCDK Development Kit Test Program",
	"SCDK",
#elif defined (CONFIG_MSMV)
	"MSMV Development Kit Test Program",
	"MSMV",
#else
#error "PLATFORM is not well defined!"
#endif

	.items = main_test_items,
	.test_item_size = sizeof(main_test_items)
};
