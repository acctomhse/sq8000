#include <global.h>
#include <test_item.h>
#include "pwmt-regs.h"
#include "dependency.h"
#if defined (CONFIG_PC7210) || defined (CONFIG_PDK)
#include <scu.h>
#endif


static struct socle_pwmt *p_pwmt;
static int socle_pwmt_count;
static int socle_pwmt_isr_flag;

static void socle_adc_ch_test_init_pwmt(int hrc, int lrc);


extern struct test_item_container adc_pwm_main_container;

extern int
pwm_adc_test(int autotest)
{
	int ret = 0;

	ret = test_item_ctrl(&adc_pwm_main_container, autotest);

	return ret;
}


extern struct test_item_container adc_converse_select_container;

extern int
socle_adc_test(int autotest)
{
	int ret = 0;

	//20080123 leonid add for ADC clock 
#if defined (CONFIG_PC7210)
	socle_scu_adc_clock_duty_period(0x8);
#elif defined (CONFIG_PDK)
	socle_scu_adc_clock_duty_period(0x1);
#endif	

	p_pwmt = get_socle_pwmt_structure(USE_PWM_NUM);
	if (NULL == p_pwmt) {
		printf("ADC can't get PWMT structure!!\n");
		return -1;
	}

	ret = test_item_ctrl(&adc_converse_select_container, autotest);

	release_socle_pwmt_structure(USE_PWM_NUM);

	return ret;
}

extern struct test_item_container adc_main_container;
extern int (*adc_wait_for_conversion)(void);

extern int
socle_adc_conv_w_poll(int autotest)
{
	int ret = 0;
	extern int adc_wait_for_conversion_by_poll(void);

	adc_wait_for_conversion = adc_wait_for_conversion_by_poll;

	ret = test_item_ctrl(&adc_main_container, autotest);

	return ret;
}

extern int
socle_adc_conv_w_int(int autotest)
{
	int ret = 0;
	extern int adc_wait_for_conversion_by_int(void);
	extern void adc_init_conversion_by_int(void);
	extern void adc_release_conversion_by_int(void);

	adc_wait_for_conversion = adc_wait_for_conversion_by_int;

	adc_init_conversion_by_int();

	ret = test_item_ctrl(&adc_main_container, autotest);
	
	adc_release_conversion_by_int();

	return ret;
}


extern int socle_adc_ch_test_sub(int v_max);

extern int
socle_adc_ch_test_w_pwmt(int autotest)
{
	int ret = 0, hrc = DEFAULT_HRC, lrc = DEFAULT_LRC, v_ref;
	extern void socle_adc_init(void);
	extern int socle_adc_read(int ch);

	printf("test with 100 duty cycle\n");
	socle_adc_ch_test_init_pwmt(hrc, lrc);
	MSDELAY(100);		// wait for capacitance charge or discharge (option)
	socle_adc_init();
	v_ref = socle_adc_read(0);		// read ch[0] value as reference
	printf("v_ref = 0x%x\n", v_ref);

	printf("test with 50 duty cycle\n");
	v_ref /= 2;
	hrc /= 2;
	socle_adc_ch_test_init_pwmt(hrc, lrc);
	MSDELAY(100);		// wait for capacitance charge or discharge (option)
	ret |= socle_adc_ch_test_sub(v_ref);
	
	printf("test with 25 duty cycle\n");
	v_ref /= 2;
	hrc /= 2;
	socle_adc_ch_test_init_pwmt(hrc, lrc);
	MSDELAY(100);		// wait for capacitance charge or discharge (option)
	ret |= socle_adc_ch_test_sub(v_ref);

	return ret;
}


extern int
socle_adc_ch_test_w_board(int autotest)
{
	int ret = 0;
	
#if defined(CONFIG_PC9002)
	ret = socle_adc_ch_test_sub(BIT_MASK(7));
#elif defined(CONFIG_PC7210)
	ret = socle_adc_ch_test_sub(0xc5);
#else
	ret = socle_adc_ch_test_sub(BIT_MASK(10));
#endif

	return ret;
}

static void
socle_adc_ch_test_init_pwmt(int hrc, int lrc)
{
	struct socle_pwmt_driver *pwmt_drv = p_pwmt->drv;
	
	pwmt_drv->claim_pwm_lock();

	pwmt_drv->reset(p_pwmt);
	pwmt_drv->write_prescale_factor(p_pwmt, DEFAULT_PRE_SCL);
	pwmt_drv->write_hrc(p_pwmt, hrc);
	pwmt_drv->write_lrc(p_pwmt, lrc);
	pwmt_drv->output_enable(p_pwmt, 1);
	pwmt_drv->enable(p_pwmt, 1);

	pwmt_drv->release_pwm_lock();
}


extern struct test_item_container pwmt_main_container;

extern int
socle_pwm_test(int autotest)
{
	int ret = 0;

	ret = test_item_ctrl(&pwmt_main_container, autotest);

	return ret;
}


extern struct test_item_container pwmt_test_container;

extern int
socle_pwmt_timer_mode(int autotest)
{
	int ret = 0;

	ret = test_item_ctrl(&pwmt_test_container, autotest);

	return ret;
}


extern struct test_item_container pwmt_counter_mode_container;

extern int
socle_pwmt_timer_0(int autotest)
{
	int ret = 0;

	p_pwmt = get_socle_pwmt_structure(0);
	if (NULL == p_pwmt) {
		printf("ADC can't get PWMT structure!!\n");
		return -1;
	}

	ret = test_item_ctrl(&pwmt_counter_mode_container, autotest);

	release_socle_pwmt_structure(0);

	return ret;
}

extern int
socle_pwmt_timer_1(int autotest)
{
	int ret = 0;

	p_pwmt = get_socle_pwmt_structure(1);
	if (NULL == p_pwmt) {
		printf("ADC can't get PWMT structure!!\n");
		return -1;
	}

	ret = test_item_ctrl(&pwmt_counter_mode_container, autotest);

	release_socle_pwmt_structure(1);

	return ret;
}

extern int
socle_pwmt_timer_2(int autotest)
{
	int ret = 0;

	p_pwmt = get_socle_pwmt_structure(2);
	if (NULL == p_pwmt) {
		printf("ADC can't get PWMT structure!!\n");
		return -1;
	}

	ret = test_item_ctrl(&pwmt_counter_mode_container, autotest);

	release_socle_pwmt_structure(2);

	return ret;
}

extern int
socle_pwmt_timer_3(int autotest)
{
	int ret = 0;

	p_pwmt = get_socle_pwmt_structure(3);
	if (NULL == p_pwmt) {
		printf("ADC can't get PWMT structure!!\n");
		return -1;
	}

	ret = test_item_ctrl(&pwmt_counter_mode_container, autotest);

	release_socle_pwmt_structure(3);

	return ret;
}

static void
socle_pwm_timer_isr(void *pparam)
{
	struct socle_pwmt_driver *pwmt_drv = p_pwmt->drv;

	pwmt_drv->claim_pwm_lock();

	socle_pwmt_isr_flag = 1;
	socle_pwmt_count++;
	printf("pwm timer irs [%d]\n", socle_pwmt_count);

	// let single counter mode to exit
	if (socle_pwmt_count >= 5)
		pwmt_drv->set_counter(p_pwmt, socle_get_apb_clock()+1);

	// clear isr
	pwmt_drv->clear_interrupt(p_pwmt);

	pwmt_drv->release_pwm_lock();
}

static void
socle_pwmt_timer_mode_test_init(int single_mode)
{
	struct socle_pwmt_driver *pwmt_drv = p_pwmt->drv;

	socle_pwmt_isr_flag = 0;
	socle_pwmt_count = 0;

	pwmt_drv->claim_pwm_lock();

	request_irq(p_pwmt->irq, socle_pwm_timer_isr, NULL);
	
	pwmt_drv->reset(p_pwmt);
	pwmt_drv->write_prescale_factor(p_pwmt, DEFAULT_PRE_SCL);
	pwmt_drv->write_hrc(p_pwmt, socle_get_apb_clock());
	pwmt_drv->write_lrc(p_pwmt, socle_get_apb_clock());
	pwmt_drv->single_counter_mode_enable(p_pwmt, single_mode);
	pwmt_drv->enable_interrupt(p_pwmt, 1);
	pwmt_drv->output_enable(p_pwmt, 1);
	pwmt_drv->enable(p_pwmt, 1);

	pwmt_drv->release_pwm_lock();
}

static void
socle_pwmt_timer_mode_test_release(void)
{
	struct socle_pwmt_driver *pwmt_drv = p_pwmt->drv;

	pwmt_drv->claim_pwm_lock();

	free_irq(p_pwmt->irq);
	pwmt_drv->enable_interrupt(p_pwmt, 0);

	pwmt_drv->release_pwm_lock();
}

extern int
socle_pwmt_periodical_counter_mode_test(int autotest)
{
	int ret = 0;

	printf("PWM Timer will interrupt 2 times.\n");

	socle_pwmt_timer_mode_test_init(0);

	if (socle_wait_for_int(&socle_pwmt_isr_flag, 3)) {
		printf("Timeout!\n");
		ret = -1;
	} else {
		if (socle_wait_for_int(&socle_pwmt_isr_flag, 3)) {
			printf("Timeout!\n");
			ret = -1;
		} else {
			if (2 != socle_pwmt_count) {
				printf("Error! ISR is not 2 times!\n");
				ret = -1;
			}
		}
	}

	socle_pwmt_timer_mode_test_release();

	return ret;
}

extern int
socle_pwmt_single_counter_mode_test(int autotest)
{
	int ret = 0;

	printf("PWM Timer will interrupt 5 times.\n");

	socle_pwmt_timer_mode_test_init(1);

	if (socle_wait_for_int(&socle_pwmt_isr_flag, 3)) {
		printf("Timeout!\n");
		ret = -1;
	}

	if (socle_pwmt_count < 5) {
		printf("socle_pwmt_count = %d does not equal and greater than 5\n", socle_pwmt_count);
		ret = -1;
	}

	socle_pwmt_timer_mode_test_release();

	return ret;
}




extern struct test_item_container pwmt_capture_mode_container;

extern int
socle_pwmt_capture_mode(int autotest)
{
	int ret = 0;

	ret = test_item_ctrl(&pwmt_capture_mode_container, autotest);

	return ret;
}


static void
socle_pwmt_capture_mode_init(struct socle_pwmt *p_pwmt, int enable, int hrc, int lrc, int irq)
{
	struct socle_pwmt_driver *pwmt_drv = p_pwmt->drv;
	
	pwmt_drv->claim_pwm_lock();

	pwmt_drv->reset(p_pwmt);
	//pwmt_drv->write_prescale_factor(p_pwmt, DEFAULT_PRE_SCL);
#ifdef CONFIG_MSMV
	pwmt_drv->write_prescale_factor(p_pwmt, 4);
#else
	pwmt_drv->write_prescale_factor(p_pwmt, 16);
#endif
	
	if (enable) {
		pwmt_drv->write_hrc(p_pwmt, 0);
		pwmt_drv->write_lrc(p_pwmt, 0);
		pwmt_drv->capture_mode_enable(p_pwmt, 1);
		if(irq)
			pwmt_drv->enable_interrupt(p_pwmt, 1);
		pwmt_drv->output_enable(p_pwmt, 0);
	} else {
		pwmt_drv->write_hrc(p_pwmt, hrc);
		pwmt_drv->write_lrc(p_pwmt, lrc);
		pwmt_drv->capture_mode_enable(p_pwmt, 0);
		pwmt_drv->output_enable(p_pwmt, 1);
	}

	pwmt_drv->enable(p_pwmt, 1);

	pwmt_drv->release_pwm_lock();
}

int mode_irq;

extern struct test_item_container pwmt_capture_mode_item_container;

extern int
socle_pwmt_capture_mode_irq(int autotest)
{
	int ret = 0;

	mode_irq=1;
	ret = test_item_ctrl(&pwmt_capture_mode_item_container, autotest);

	return ret;
}

extern int
socle_pwmt_capture_mode_pull(int autotest)
{
	int ret = 0;

	mode_irq=0;
	ret = test_item_ctrl(&pwmt_capture_mode_item_container, autotest);

	return ret;
}


extern int socle_pwmt_capture_mode_loopback(int autotest);
extern int socle_pwmt_capture_mode_current_sense(int autotest);


extern struct test_item_container pwmt_capture_mode_loop_container;

extern int
socle_pwmt_capture_mode_loopback (int autotest)
{
	int ret = 0;

	ret = test_item_ctrl(&pwmt_capture_mode_loop_container, autotest);

	return ret;
}

//extern int socle_pwm_capture_duty_irq (void);
////static void socle_pwm_capture_isr(void *pparam);
extern int socle_pwm_capture_duty_pull (void);
int current_sense_calc(int *hrc, int *lrc, int count);
int current_duty_conver_table(int duty);

static int socle_pwmt_capture_mode_test(int pwmt_num_tx, int pwmt_num_rx);

struct socle_pwmt *p_pwmt_cur;
int cur_init=0;


#define MAX_CNT 100

u32 hrc_val[MAX_CNT];
u32 lrc_val[MAX_CNT];

int cur_cnt;
int sen_cnt;
u32 hrc_tx, lrc_tx;
u32 hrc_rx, lrc_rx;
int duty;

extern int
socle_pwmt_capture_mode_current_sense (int autotest)
{
	struct socle_pwmt *p_pwmt_rx;
	int duty, ma;
	char tmp;
	
	p_pwmt_rx = get_socle_pwmt_structure(USE_PWM_NUM_1);
	
	if (NULL == p_pwmt_rx) {
		printf("ADC can't get PWMT structure!!\n");
		return -1;
	}
	
	p_pwmt_cur = p_pwmt_rx;

	while(1){
	//if(mode_irq == 1)
	//	duty = socle_pwm_capture_duty_irq();
	//else
		duty = socle_pwm_capture_duty_pull();
	
		cur_init=1;
	
		if(duty < 0){
			printf("sense duty error\n");
			release_socle_pwmt_structure(USE_PWM_NUM_1);
			cur_init=0;
			return -1;
		}else
			printf("duty cycle = %d.%02d	", (duty/100), (duty%100));
		
		ma = current_duty_conver_table(duty);
		if(ma > 1000){
			printf("convert duty error\n");
			release_socle_pwmt_structure(USE_PWM_NUM_1);
			cur_init=0;
			return -1;
		}else
			printf("current sense is %4d mA\n", ma);
	
		printf("exit(x) or sense again(any key) : ");
		scanf("%c\n", &tmp);
		if((tmp=='x')||(tmp=='X'))
			break;
	}

	release_socle_pwmt_structure(USE_PWM_NUM_1);
	cur_init=0;

	return 0;

}

#if 0	
extern int
socle_pwm_capture_duty_irq (void)
{
	int duty;
	int tmp;

	printf("socle_current_sense_enable_irq\n");

	cur_cnt = 0;
	hrc_val[0]=0;
	lrc_val[0]=0;
	sen_cnt = 0;
	socle_pwmt_isr_flag=0;
	
	request_irq(p_pwmt_cur->irq, socle_pwm_capture_isr, NULL);
	
	// rx
	socle_pwmt_capture_mode_init(p_pwmt_cur, 1, 0, 0, 1);

	while(1){	
		if(socle_pwmt_isr_flag){
			socle_pwmt_isr_flag = 0;
			//printf(" hrc_val[cur_cnt] = %x, lrc_val[cur_cnt] = %x, cur_cnt = %d\n",
			//		hrc_val[cur_cnt], lrc_val[cur_cnt], cur_cnt);
			duty = current_sense_calc(hrc_val, lrc_val, sen_cnt);
			if(duty == -1){
				cur_cnt = 0;			
				p_pwmt_cur->drv->set_counter (p_pwmt_cur, 0);
				p_pwmt_cur->drv->enable_interrupt (p_pwmt_cur, 1);
				printf("duty = -1\n");
				continue;
			}else{
				break;
			}					
		}else if(cur_cnt > MAX_CNT){				
			duty = -1;
			break;
		}
	}
		
	p_pwmt_cur->drv->enable(p_pwmt_cur, 0);
	
	free_irq(p_pwmt_cur->irq);

	//for(tmp=0;tmp<100;tmp++)
	//	printf("hrc_val[%d]=%d, lrc_val[%d]=%d\n", tmp, hrc_val[tmp], tmp, lrc_val[tmp]);
	
	return duty;	
	
}

static void
socle_pwm_capture_isr(void *pparam)
{
        p_pwmt_cur->drv->clear_interrupt (p_pwmt_cur);

        hrc_val[cur_cnt] = p_pwmt_cur->drv->read_hrc(p_pwmt_cur);
        lrc_val[cur_cnt] = p_pwmt_cur->drv->read_lrc(p_pwmt_cur);

        //printf(" hrc_val[%d] = %x, lrc_val[%d] = %x\n", cur_cnt, hrc_val[cur_cnt], cur_cnt, lrc_val[cur_cnt]);

        if(cur_cnt>2){
                if((hrc_val[cur_cnt] == hrc_val[cur_cnt-1]) || (lrc_val[cur_cnt] != lrc_val[cur_cnt-1])){
                        sen_cnt = cur_cnt;
                        p_pwmt_cur->drv->enable_interrupt (p_pwmt_cur, 0);
                        //printk(" cur_cnt = %d\n", cur_cnt);
                        cur_cnt++;
                        socle_pwmt_isr_flag = 1;
                        return ;
                }
        }

        cur_cnt++;

        return;
}


#endif

extern int
socle_pwm_capture_duty_pull(void)
{
	int duty;
	int t=0;	

	//printf("socle_current_sense_enable_pull\n");	

	if(cur_init==0)
		// rx
		socle_pwmt_capture_mode_init(p_pwmt_cur, 1, 0, 0, 0);
	
	//p_pwmt_cur->drv->set_counter (p_pwmt_cur, 0);
	cur_cnt = 0;
	hrc_val[0]=0;
	lrc_val[0]=0;
	
	while(1){	
			t++;				
			if((cur_cnt > MAX_CNT)||(t>0x100000)){
				printf("PWMT_TIMEOUT\n");
				p_pwmt_cur->drv->enable(p_pwmt_cur, 0);
				return -1;
			}
			
			hrc_rx = p_pwmt_cur->drv->read_hrc(p_pwmt_cur);
			lrc_rx = p_pwmt_cur->drv->read_lrc(p_pwmt_cur);
			//printf("hrc = %x, lrc = %x\n", hrc, lrc);
			if(hrc_rx != hrc_val[cur_cnt]){
                                cur_cnt++;
                                hrc_val[cur_cnt] = hrc_rx;
                                lrc_val[cur_cnt] = lrc_rx;
                                if((hrc_val[cur_cnt-1]==0) || (lrc_val[cur_cnt] != lrc_val[cur_cnt-1]))
                                        continue;
                        } else if (lrc_rx != lrc_val[cur_cnt]) {
                                cur_cnt++;
                                hrc_val[cur_cnt] = hrc_rx;
                                lrc_val[cur_cnt] = lrc_rx;
                                if((lrc_val[cur_cnt-1]==0) || (hrc_val[cur_cnt] != hrc_val[cur_cnt-1]))
                                        continue;
                        }else
                                continue;
		#if 0	
			printf("hrc_val[%d]=%x, lrc_val[%d]=%x\n", 
						cur_cnt-1, hrc_val[cur_cnt-1], cur_cnt-1, lrc_val[cur_cnt-1]);
			printf("hrc_val[%d]=%x, lrc_val[%d]=%x\n", 
						cur_cnt, hrc_val[cur_cnt], cur_cnt, lrc_val[cur_cnt]);
		#endif
			duty = current_sense_calc(hrc_val, lrc_val, cur_cnt);
			//printf("duty = %d\n", duty);
			if(duty == -1){
				cur_cnt = 0;			
				p_pwmt_cur->drv->set_counter (p_pwmt_cur, 0);
				printf("duty = -1\n");
				continue;
			}else{
				break;
			}	
	}
	
	
	//p_pwmt_cur->drv->enable(p_pwmt_cur, 0);
		
	return duty;	
}

int
current_sense_calc(int hrc_cal[5], int lrc_cal[5], int cnt)
{
	u32 val1, val2, val3;
	int duty;


	if(hrc_cal[cnt] == hrc_cal[cnt-1]){
		val1 = lrc_cal[cnt-1]; 
		val2 = hrc_cal[cnt]; 
		val3 = lrc_cal[cnt]; 
		duty = ((val3-val2)*10000) / (val3-val1) ;
	}else{
		val1 = hrc_cal[cnt-1]; 
		val2 = lrc_cal[cnt]; 
		val3 = hrc_cal[cnt]; 
		duty = ((val2-val1)*10000) / (val3-val1) ;
	}

	//printf("val1=%x, val2=%x, val3=%x\n", val1, val2, val3);
	//printf("count = %d\n", cnt);

	if((duty<0) || (duty>10000))
		duty =-1;
	
	return duty; 
}


int duty_table[21]={5000, 5230, 5450, 5680, 5910, 6140, 6360, 6590, 6820, 7050, 
				7270, 7500, 7730, 7950, 8180, 8410, 8640, 8860, 9090, 9320, 9550};

int
current_duty_conver_table(int duty)
{
	u32 val;
	int index;

	if (duty < 5000)
		duty = 10000 - duty;
	
	for(index=0; index < 21; index++){
		if(duty <= duty_table[index]){
			val = 50 * (duty -duty_table[index-1]) / (duty_table[index]-duty_table[index-1]);
			val = val + 50*(index-1);
			
			return val;
		}
	}
	return 1001;		//it > max(1000)	
}


extern struct test_item_container pwmt_capture_mode_loop_duty_container;

extern int 
socle_pwmt_capture_mode_loop_duty_75(int autotest)
{
	int ret;

	hrc_tx = DEFAULT_LRC*3;
	lrc_tx = DEFAULT_LRC*4;
	duty = 75;
	
	ret = test_item_ctrl(&pwmt_capture_mode_loop_duty_container, autotest);

	return ret;
}

extern int 
socle_pwmt_capture_mode_loop_duty_50(int autotest)
{
	int ret;

	hrc_tx = DEFAULT_LRC;
	lrc_tx = DEFAULT_LRC*2;
	duty = 50;
	
	ret = test_item_ctrl(&pwmt_capture_mode_loop_duty_container, autotest);

	return ret;
}

extern int 
socle_pwmt_capture_mode_loop_duty_25(int autotest)
{
	int ret;

	hrc_tx = DEFAULT_LRC;
	lrc_tx = DEFAULT_LRC*4;
	duty = 25;
	
	ret = test_item_ctrl(&pwmt_capture_mode_loop_duty_container, autotest);

	return ret;
}

extern int
socle_pwmt_capture_mode_from_0_to_1(int autotest)
{
	int ret = 0;

	ret = socle_pwmt_capture_mode_test(USE_PWM_NUM_0, USE_PWM_NUM_1);

	return ret;
}

extern int
socle_pwmt_capture_mode_from_1_to_0(int autotest)
{
	int ret = 0;

	ret = socle_pwmt_capture_mode_test(USE_PWM_NUM_1, USE_PWM_NUM_0);

	return ret;
}




static int
socle_pwmt_capture_mode_test(int pwmt_num_tx, int pwmt_num_rx)
{
	int ret = 0;
	struct socle_pwmt *p_pwmt_tx, *p_pwmt_rx;
	int duty_loop;
	
	#if 0
	hrc_tx = DEFAULT_LRC*3;
	lrc_tx = DEFAULT_LRC*4;
	duty = 75;
	mode_irq=0;
	#endif
	
	p_pwmt_tx = get_socle_pwmt_structure(pwmt_num_tx);
	if (NULL == p_pwmt_tx) {
		printf("ADC can't get PWMT structure!!\n");
		return -1;
	}

	p_pwmt_rx = get_socle_pwmt_structure(pwmt_num_rx);
	if (NULL == p_pwmt_rx) {
		printf("ADC can't get PWMT structure!!\n");
		return -1;
	}

	p_pwmt_cur = p_pwmt_rx;

	// tx
	socle_pwmt_capture_mode_init(p_pwmt_tx, 0, hrc_tx, lrc_tx, 0);
	
	

	//if(mode_irq == 1){
		// rx
	//	duty_loop = socle_pwm_capture_duty_irq()/100;
	//}else{
		// rx
		duty_loop = socle_pwm_capture_duty_pull()/100;
	//}

	printf("duty sense = %d\n", duty_loop);

	if(duty_loop == duty){
		printf("capture mode loop test pass\n");
		ret = 0;
	}else{
		printf("capture mode loop test fail\n");
		ret = -1;
	}

	
	release_socle_pwmt_structure(pwmt_num_tx);
	release_socle_pwmt_structure(pwmt_num_rx);

	return ret;
}


