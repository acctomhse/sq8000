#include <global.h>
#include <irqs.h>
#include "adc-regs.h"
#include "dependency.h"

//#define CONFIG_SOCLE_ADC_DEBUG
#ifdef CONFIG_SOCLE_ADC_DEBUG
	#define ADC_DBG(fmt, args...) printf("SOCLE_ADC: " fmt, ## args)
#else
	#define ADC_DBG(fmt, args...)
#endif


int (*adc_wait_for_conversion)(void);

static u32 socle_adc_base = SOCLE_APB0_ADC;;
static u32 socle_adc_irq = SOCLE_INTC_ADC;
static int socle_adc_ready;

static void adc_conversion_isr(void *pparam);

extern void socle_adc_init(void);
extern int socle_adc_read(int ch);
extern int adc_wait_for_conversion_by_poll(void);
extern int adc_wait_for_conversion_by_int(void);

static inline void
adc_read(u32 offset, u32 *data, u32 base)
{
	MSDELAY(10);

	*data = ioread32(base + offset);

	ADC_DBG("adc_read(): base:0x%08x, offset:0x%08x, data:0x%08x\n", base, offset, *data);
}

static inline void
adc_write(u32 offset, u32 data, u32 base)
{
	MSDELAY(10);
	
	iowrite32(data, base + offset);

	ADC_DBG("adc_write(): base:0x%08x, offset:0x%08x, data:0x%08x\n", base, offset, data);
}


extern int
socle_adc_ch_test_sub(int v_max)
{
	int ch, val, sum = 0, bnd_hi, bnd_lo, err;

	if (0 == v_max) {
		printf("Error! v_max = 0, No input source!! => PWMT fail\n");
		return -1;
	}

	socle_adc_init();

	for (ch = 0; ch < SUPT_CH; ch++) {	
		val = socle_adc_read(ch);
		if (-1 == val)
			return -1;
			
		printf("ch[%d] = 0x%x\n", ch, val);
		sum += val;
	}

	// without divided voltage
	//err = 1;
	//bnd_hi = v_max * SUPT_CH + SUPT_CH * err;
	//bnd_lo = v_max * SUPT_CH - SUPT_CH * err;

	// with divided voltage
	err = 2;
	bnd_hi = v_max * (SUPT_CH + 1) / 2 + SUPT_CH * err;
	bnd_lo = v_max * (SUPT_CH + 1) / 2 - SUPT_CH * err;

	if (0 == sum) {
		printf("Error! sum = 0, No input source!! => PWMT fail\n");
		return -1;
	}

	if ((sum > bnd_hi) || (sum < bnd_lo)) {
		printf("v_max = 0x%x, sum = 0x%x must between 0x%x and 0x%x!!\n", v_max, sum, bnd_lo, bnd_hi);
		return -1;
	} else
		printf("v_max = 0x%x, sum = 0x%x\n", v_max, sum);

	return 0;
}

extern void
socle_adc_init(void)
{
	int data;

#ifndef CONFIG_PC9220
	// adc power up and reset
	adc_read(ADC_CTRL, &data, socle_adc_base);
	adc_write(ADC_CTRL, ADC_PWR_UP | data, socle_adc_base);
#endif

}

extern int
socle_adc_read(int ch)
{
	int data, val;
	
	if (SUPT_CH <= ch) {
		printf("Select ch[%d], but only support %d channels!!\n", ch, SUPT_CH);
		return -1;
	}
	
	// start converse
	adc_read(ADC_CTRL, &data, socle_adc_base);
	data = data & ~ADC_CH_MSK;
	adc_write(ADC_CTRL, ADC_STR_CONV | data | ch , socle_adc_base);
	
	// wait for conversion
	val = adc_wait_for_conversion();
	if (-1 == val)
		return -1;

	return (val & ADC_CONV_VAL_MSK);
}

extern int
adc_wait_for_conversion_by_poll(void)
{
	int val;

	if (socle_wait_by_poll(socle_adc_base + ADC_STAS, ADC_CONV_STAS, ACD_STAS_STOP, 3)) {
		printf("Timeout!!\n");
		return -1;
	}

	// read value
	adc_read(ADC_DATA, &val, socle_adc_base);

	return val;
}

extern int
adc_wait_for_conversion_by_int(void)
{
	int val;

	if (socle_wait_for_int(&socle_adc_ready, 3)) {
		printf("Timeout!!\n");
		return -1;
	}

	// read value
	adc_read(ADC_DATA, &val, socle_adc_base);

	return val;
}

extern void
adc_init_conversion_by_int(void)
{
	int data;

	ADC_DBG("adc irq = %d\n", socle_adc_irq);

	// adc interrupt enable
	adc_read(ADC_CTRL, &data, socle_adc_base);
	adc_write(ADC_CTRL, ADC_INT_EN | data, socle_adc_base);

	// interrupt enable
	request_irq(socle_adc_irq, adc_conversion_isr, NULL);

	socle_adc_ready = 0;
}

extern void
adc_release_conversion_by_int(void)
{
	int data;

	// adc interrupt disable
	adc_read(ADC_CTRL, &data, socle_adc_base);
	adc_write(ADC_CTRL, ~ADC_INT_EN & data, socle_adc_base);

	// disable interrupt
	free_irq(socle_adc_irq);

	socle_adc_ready = 0;
}

static void
adc_conversion_isr(void *pparam)
{
	int data;

	ADC_DBG("adc irs\n");

	// clear isr
	adc_read(ADC_CTRL, &data, socle_adc_base);
	adc_write(ADC_CTRL, ~ADC_INT_STAS & data, socle_adc_base);

	socle_adc_ready = 1;
}

