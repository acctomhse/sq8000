#include <platform.h>
#include <genlib.h>
#include <global.h>
#include <test_item.h>
#include "cdk-scu-regs.h"

//#define CONFIG_CDK_SCU_DEBUG

#ifdef CONFIG_CDK_SCU_DEBUG
	#define SCU_DBG(fmt, args...) printf("CDK_SCU : " fmt, ## args)
#else
	#define SCU_DBG(fmt, args...)
#endif

static int cdk_scu_set_main_clock_frequence(int freq);
static int cdk_scu_set_main_clock_ratio(int mode);
static int cdk_scu_set_main_power_mode(int mode);
static void cdk_scu_check_pll_lock(void);
static int cdk_scu_mem_test(void);

static inline void
cdk_scu_read(unsigned int offset, unsigned int *data)
{
	*data = ioread32(CDK_SCU_REG_BASE + offset);

	SCU_DBG("cdk_scu_read(): base:0x%08x, offset:0x%08x, data:0x%08x\n", CDK_SCU_REG_BASE, offset, *data);
}

static inline void
cdk_scu_write(unsigned int offset, unsigned int data)
{
	iowrite32(data, CDK_SCU_REG_BASE + offset);

	SCU_DBG("cdk_scu_write(): base:0x%08x, offset:0x%08x, data:0x%08x\n", CDK_SCU_REG_BASE, offset, data);
}


extern struct test_item_container cdk_scu_frequence_test_container;

extern int
cdk_scu_test(int autotest)
{
	int ret = 0;

	ret = test_item_ctrl(&cdk_scu_frequence_test_container, autotest);
	return ret;
}


extern struct test_item_container cdk_scu_mode_test_container;

extern int
cdk_scu_frequence_88M(int autotest)
{
	int ret = 0;

	cdk_scu_set_main_clock_frequence(88);
	
	ret = test_item_ctrl(&cdk_scu_mode_test_container, autotest);

	return ret;
}

extern int
cdk_scu_frequence_133M(int autotest)
{
	int ret = 0;

	cdk_scu_set_main_clock_frequence(133);
	
	ret = test_item_ctrl(&cdk_scu_mode_test_container, autotest);

	return ret;
}

extern int
cdk_scu_frequence_266M(int autotest)
{
	int ret = 0;

	cdk_scu_set_main_clock_frequence(266);
	
	ret = test_item_ctrl(&cdk_scu_mode_test_container, autotest);

	return ret;
}


extern struct test_item_container cdk_scu_test_container;

extern int
cdk_scu_mode_1vs1(int autotest)
{
	int ret = 0;

	cdk_scu_set_main_clock_ratio(1);
	
	ret = test_item_ctrl(&cdk_scu_test_container, autotest);

	return ret;
}

extern int
cdk_scu_mode_2vs1(int autotest)
{
	int ret = 0;

	cdk_scu_set_main_clock_ratio(2);
	
	ret = test_item_ctrl(&cdk_scu_test_container, autotest);

	return ret;
}

extern int
cdk_scu_mode_3vs1(int autotest)
{
	int ret = 0;

	cdk_scu_set_main_clock_ratio(3);
	
	ret = test_item_ctrl(&cdk_scu_test_container, autotest);

	return ret;
}

extern int
cdk_scu_mode_4vs1(int autotest)
{
	int ret = 0;

	cdk_scu_set_main_clock_ratio(4);
	
	ret = test_item_ctrl(&cdk_scu_test_container, autotest);

	return ret;
}

extern int
cdk_scu_mode_8vs1(int autotest)
{
	int ret = 0;

	cdk_scu_set_main_clock_ratio(8);
	
	ret = test_item_ctrl(&cdk_scu_test_container, autotest);

	return ret;
}

extern int
cdk_scu_stability_test(int autotest)
{
	int ret = 0, input, i;

	if (autotest) {
		input = 5;
	} else {
		printf("Input test number = ");
		scanf("%d", &input);
	}
	
	for (i = 0; i < input; i++) {
		printf("test number[%d]\n", i);
	
		cdk_scu_set_main_power_mode(1);		// Normal mode
		MSDELAY(20);
		
		ret |= cdk_scu_mem_test();
		
		cdk_scu_set_main_power_mode(0);		// Slow mode
		MSDELAY(20);
		
		ret |= cdk_scu_mem_test();
		
		if (ret)
			return ret;
	}

	return ret;
}

static void
cdk_scu_check_pll_lock(void)
{
	unsigned int tmp;
	int i = 0, t = 1000;
	
	// wait for PLL Locked
	do {
		cdk_scu_read(CDK_SCU_MPLLCON, &tmp);
		tmp &= SCU_PLL_LOCK;
		if (!tmp)
			printf("[%d] PLL Non-locked!\n", i);
	} while ((!tmp) && (i < t));

	if (i >= t)
		printf("Timeout!! PLL still Non-locked!\n");
	else
		printf("PLL Locked!\n");
}

static int
cdk_scu_mem_test(void)
{
	int i;
	unsigned int tmp, test_pattern = 0x55aa55aa;
	
	for(i = 0; i < 5; i++) {
		iowrite32(test_pattern, SOCLE_MM_DDR_SDR_BANK1 + 0x600000);
		tmp = ioread32(SOCLE_MM_DDR_SDR_BANK1 + 0x600000);
		if (test_pattern ^ tmp) {	//check test patern
			printf("Compare error!! test_pattern = 0x%08x, rdata=0x%08x\n", test_pattern);
			return -1;
		}
	}
	
	return 0;
}


static int
cdk_scu_set_main_clock_frequence(int freq)
{
	unsigned int freq_set;
	
	printf("main clock frequence = %dMHz\n", freq);

	switch (freq) {
		case 88:
			freq_set = 0x000008b2;
			break;
		
		case 133:
			freq_set = 0x0000090a;
			break;

		case 266:
			freq_set = 0x00000909;
			break;

		default:
			printf("Error main clock frequence setting!\n");
			return -1;
	}
	
	cdk_scu_write(CDK_SCU_MPLLCON, freq_set);
	
	cdk_scu_check_pll_lock();
	
	return 0;
}

static int
cdk_scu_set_main_clock_ratio(int ratio)
{
	unsigned int tmp, ratio_set;

	printf("main clock ratio = %d:1\n", ratio);
	
	switch (ratio) {
		case 1:
			ratio_set = SCU_CLK_RATIO_1VS1;
			break;

		case 2:
			ratio_set = SCU_CLK_RATIO_2VS1;
			break;

		case 3:
			ratio_set = SCU_CLK_RATIO_3VS1;
			break;
			
		case 4:
			ratio_set = SCU_CLK_RATIO_4VS1;
			break;

		case 8:
			ratio_set = SCU_CLK_RATIO_8VS1;
			break;

		default:
			printf("Error main clock ratio setting!\n");
			return -1;
	}

	cdk_scu_read(CDK_SCU_MCLKDIV, &tmp);
	cdk_scu_write(CDK_SCU_MCLKDIV, (tmp & ~SCU_CLK_RATIO_MASK) | ratio_set);

	return 0;
}

static int
cdk_scu_set_main_power_mode(int mode)
{
	unsigned int tmp;

	switch (mode) {
		case 0:		// Slow mode
			printf("Slow mode!\n");
			cdk_scu_write(CDK_SCU_PWMCON, 0x0);
			break;
		
		case 1:		// Normal mode
			printf("Normal mode!\n");
			cdk_scu_write(CDK_SCU_PWMCON, 0x1 << 0);
			break;

		case 2:		// Idle mode
			printf("Idle mode!\n");
			cdk_scu_read(CDK_SCU_PWMCON, &tmp);
			cdk_scu_write(CDK_SCU_PWMCON, tmp | (0x1 << 2));
			break;

		default:
			printf("Error power mode setting!\n");
			return -1;
	}

	return 0;
}

