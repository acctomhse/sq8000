#include <platform.h>
#include <genlib.h>
#include <irqs.h>
#include <test_item.h>
#include <dma/dma.h>

#define PATTERN_ADDR 0x00a00000
#define COMPARE_ADDR 0x00a00800
#define PATTERN_SIZE 2048
#define BURN_IN_SIZE 512
#define SCU_AHB_MODE (0x1<<6)

static void panther7_hdma_make_test_pattern(u8 *mem, u32 cnt);
static int panther7_hdma_compare_memory(u8 *mem, u8 *cmpr_mem, u32 cnt, 
					u32 data_size, u32 src_dir, u32 dst_dir);
static void panther7_hdma_notifier_complete(void *data);

static int panther7_complete_flag = 0;
static u32 panther7_tran_size = PATTERN_SIZE;
static u8 *panther7_hdma_pattern_addr = (u8 *)PATTERN_ADDR;
static u8 *panther7_hdma_cmpr_addr = (u8 *)COMPARE_ADDR;
static u32 panther7_hdma_channel_num, panther7_hdma_burst_type, panther7_hdma_data_size, panther7_hdma_src_dir, panther7_hdma_dst_dir;

static struct socle_dma_notifier panther7_hdma_notifier = {
	.complete = panther7_hdma_notifier_complete,
};

extern struct test_item_container panther7_hdma_main_test_container;

extern int panther7_hdma_fly_disable(int autotest)
{
	return 0;
}

extern int panther7_hdma_fly_read(int autotest)
{
	return 0;
}

extern int panther7_hdma_fly_write(int autotest)
{
	return 0;
}


extern int 
PANTHER7_HDMATesting(int autotest)
{
	int ret=0;
#ifdef CONFIG_SCDK
	{
		extern struct test_item panther7_hdma_main_test_items[];
		//read scu to get amba mode
		if(ioread32(SOCLE_APB0_SCU + 0x28) & SCU_AHB_MODE) {
			panther7_hdma_main_test_items[0].enable=0;
			panther7_hdma_main_test_items[1].enable=1;
		}
		else {
			panther7_hdma_main_test_items[0].enable=1;
			panther7_hdma_main_test_items[1].enable=0;
		}
			
	}
#endif	    
	ret = test_item_ctrl(&panther7_hdma_main_test_container, autotest);
	return ret;
}

extern struct test_item_container panther7_hdma_ch_test_container;

extern int
panther7_hdma_onboard_test(int autotest)
{
	int ret=0;
	socle_set_dma_base_irq(PANTHER7_HDMA_CH_0, PANTHER7_AHB_0_HDMA_0, PANTHER7_INTC_HDMA_0);
	socle_set_dma_base_irq(PANTHER7_HDMA_CH_1, PANTHER7_AHB_0_HDMA_0, PANTHER7_INTC_HDMA_0);
	ret = test_item_ctrl(&panther7_hdma_ch_test_container, autotest);
	return ret;
}

extern int
panther7_hdma_fpga_test(int autotest)
{
	int ret=0;
#ifdef CONFIG_SCDK
	socle_set_dma_base_irq(PANTHER7_HDMA_CH_0, SOCLE_AHB1_MP, SOCLE_INTC_MPS1);
	socle_set_dma_base_irq(PANTHER7_HDMA_CH_1, SOCLE_AHB1_MP, SOCLE_INTC_MPS1);
#endif
	ret = test_item_ctrl(&panther7_hdma_ch_test_container, autotest);
	
	return ret;
}

extern struct test_item_container panther7_hdma_src_dir_test_container;

extern int 
panther7_hdma_channel_0_test(int autotest)
{
	int ret = 0;

	panther7_hdma_channel_num = PANTHER7_HDMA_CH_0;
	socle_request_dma(panther7_hdma_channel_num, &panther7_hdma_notifier);
	ret = test_item_ctrl(&panther7_hdma_src_dir_test_container, autotest);
	socle_disable_dma(panther7_hdma_channel_num);
	socle_free_dma(panther7_hdma_channel_num);
	return ret;
}

extern int 
panther7_hdma_channel_1_test(int autotest)
{
	int ret = 0;

	panther7_hdma_channel_num = PANTHER7_HDMA_CH_1;
	socle_request_dma(panther7_hdma_channel_num, &panther7_hdma_notifier);
	ret = test_item_ctrl(&panther7_hdma_src_dir_test_container, autotest);
	socle_disable_dma(panther7_hdma_channel_num);
	socle_free_dma(panther7_hdma_channel_num);
	return ret;
}

struct test_item_container panther7_hdma_dst_dir_test_container;

extern int 
panther7_hdma_source_direction_fixed(int autotest)
{
	int ret = 0;

	panther7_hdma_src_dir = SOCLE_DMA_DIR_FIXED;
	ret = test_item_ctrl(&panther7_hdma_dst_dir_test_container, autotest);
	return ret;
}

extern int 
panther7_hdma_source_direction_increment(int autotest)
{
	int ret = 0;

	panther7_hdma_src_dir = SOCLE_DMA_DIR_INCR;
	ret = test_item_ctrl(&panther7_hdma_dst_dir_test_container, autotest);
	return ret;
}

struct test_item_container panther7_hdma_burst_test_container;

extern int panther7_hdma_destination_direction_fixed(int autotest)
{
	int ret = 0;

	panther7_hdma_dst_dir = SOCLE_DMA_DIR_FIXED;
	ret = test_item_ctrl(&panther7_hdma_burst_test_container, autotest);
	return ret;
}

extern int panther7_hdma_destination_direction_increment(int autotest)
{
	int ret = 0;

	panther7_hdma_dst_dir = SOCLE_DMA_DIR_INCR;
	ret = test_item_ctrl(&panther7_hdma_burst_test_container, autotest);
	return ret;
}

struct test_item_container panther7_hdma_data_test_container;

extern int 
panther7_hdma_burst_single(int autotest)
{
	int ret = 0;

	panther7_hdma_burst_type = SOCLE_DMA_BURST_SINGLE;
	ret = test_item_ctrl(&panther7_hdma_data_test_container, autotest);
	return ret;
}

extern int panther7_hdma_burst_incr4(int autotest)
{
	int ret = 0;

	panther7_hdma_burst_type = SOCLE_DMA_BURST_INCR4;
	ret = test_item_ctrl(&panther7_hdma_data_test_container, autotest);
	return ret;
}

extern int panther7_hdma_burst_incr8(int autotest)
{
	int ret = 0;

	panther7_hdma_burst_type = SOCLE_DMA_BURST_INCR8;
	ret = test_item_ctrl(&panther7_hdma_data_test_container, autotest);
	return ret;
}

extern int panther7_hdma_burst_incr16(int autotest)
{
	int ret = 0;

	panther7_hdma_burst_type = SOCLE_DMA_BURST_INCR16;
	ret = test_item_ctrl(&panther7_hdma_data_test_container, autotest);
	return ret;
}

struct test_item_container panther7_hdma_run_test_container;

extern int 
panther7_hdma_data_byte(int autotest)
{
	int ret = 0;	

	panther7_hdma_data_size = SOCLE_DMA_DATA_BYTE;
	ret = test_item_ctrl(&panther7_hdma_run_test_container, autotest);
	return ret;
}

extern int 
panther7_hdma_data_halfword(int autotest)
{
	int ret = 0;	
	
	panther7_hdma_data_size = SOCLE_DMA_DATA_HALFWORD;
	ret = test_item_ctrl(&panther7_hdma_run_test_container, autotest);
        return ret;

}

extern int 
panther7_hdma_data_word(int autotest)
{
	int ret = 0;	

	panther7_hdma_data_size = SOCLE_DMA_DATA_WORD;
	ret = test_item_ctrl(&panther7_hdma_run_test_container, autotest);
        return ret;

}

static int panther7_hdma_software_dma_tranfer(int autotest);
static int panther7_hdma_burn_in_tranfer(int autotest);

extern int
panther7_hdma_run(int autotest)
{
	int ret = 0;	
	
	panther7_tran_size = PATTERN_SIZE;
	ret = panther7_hdma_software_dma_tranfer(autotest);
	return ret;
}

extern int
panther7_hdma_burn_in(int autotest)
{
	int ret = 0;        

	panther7_tran_size = BURN_IN_SIZE;
	
	ret = panther7_hdma_burn_in_tranfer(autotest);
        return ret;
}


static int 
panther7_hdma_software_dma_tranfer(int autotest)
{
	socle_disable_dma(panther7_hdma_channel_num);
	socle_set_dma_mode(panther7_hdma_channel_num, SOCLE_DMA_MODE_SW);
	socle_set_dma_source_address(panther7_hdma_channel_num, (u32)panther7_hdma_pattern_addr);
	socle_set_dma_destination_address(panther7_hdma_channel_num, (u32)panther7_hdma_cmpr_addr);
	socle_set_dma_source_direction(panther7_hdma_channel_num, panther7_hdma_src_dir);
	socle_set_dma_destination_direction(panther7_hdma_channel_num, panther7_hdma_dst_dir);
	socle_set_dma_burst_type(panther7_hdma_channel_num, panther7_hdma_burst_type);
	socle_set_dma_data_size(panther7_hdma_channel_num, panther7_hdma_data_size);
	socle_set_dma_transfer_count(panther7_hdma_channel_num, panther7_tran_size);

	/* Clear pattern buffer and compare buffer */
	memset(panther7_hdma_pattern_addr, 0x0, panther7_tran_size);
	memset(panther7_hdma_cmpr_addr, 0x0, panther7_tran_size);

	panther7_hdma_make_test_pattern(panther7_hdma_pattern_addr, panther7_tran_size);
	panther7_complete_flag = 0;
	socle_enable_dma(panther7_hdma_channel_num);
	
	if (socle_wait_for_int(&panther7_complete_flag, 30)) {
		printf("Timeout\n");
		return -1;
	}
	return panther7_hdma_compare_memory(panther7_hdma_pattern_addr, panther7_hdma_cmpr_addr, panther7_tran_size, 
					    panther7_hdma_data_size, panther7_hdma_src_dir, panther7_hdma_dst_dir);
}

static int
panther7_hdma_burn_in_tranfer(int autotest)
{
	int cnt =100, cur_cnt, ret;

        printf("set burn count (default 100): ");
        scanf("%d\n", &cnt);

        for(cur_cnt=0;cur_cnt<cnt;cur_cnt++){
		if((cur_cnt & 0x7) == 0){
			panther7_hdma_pattern_addr = (u8 *)PATTERN_ADDR;
                	panther7_hdma_cmpr_addr = (u8 *)COMPARE_ADDR;
		}else{
        		panther7_hdma_pattern_addr += panther7_tran_size; 
			panther7_hdma_cmpr_addr += panther7_tran_size; 
		}
		ret = panther7_hdma_software_dma_tranfer(autotest);
		if(ret != 0){
			printf("error occur at count %d\n", cur_cnt);
			return -1;
			break;
		}
	}
        return 0;
}

static void
panther7_hdma_make_test_pattern(u8 *mem, u32 cnt)
{
	int i;
     
	for (i = 0; i < cnt; i++)
		mem[i] = i;
}

static int 
panther7_hdma_compare_memory(u8 *mem, u8 *cmpr_mem, u32 cnt, 
			     u32 data_size, u32 src_dir, u32 dst_dir)
{
	u32 i;
	u8 *mem_8 = mem;
	u8 *cmpr_mem_8 = cmpr_mem;
	u16 *mem_16 = (u16 *)mem;
	u16 *cmpr_mem_16 = (u16 *)cmpr_mem;
	u32 *mem_32 = (u32 *)mem;
	u32 *cmpr_mem_32 = (u32 *)cmpr_mem;
	int err_flag = 0;
		
	switch (data_size) {
	case SOCLE_DMA_DATA_BYTE:
		if ((SOCLE_DMA_DIR_INCR == src_dir) &&
		    (SOCLE_DMA_DIR_INCR == dst_dir)) {
			for (i = 0; i < cnt; i++) {
				if (mem_8[i] != cmpr_mem_8[i]) {
					err_flag |= -1;
					printf("\nByte %d, 0x%02x (0x%08x) != 0x%02x (0x%08x)", i, mem_8[i], &mem_8[i], 
					       cmpr_mem_8[i], &cmpr_mem_8[i]);
				}
			}
		} else if ((SOCLE_DMA_DIR_INCR == src_dir) &&
			   (SOCLE_DMA_DIR_FIXED == dst_dir)) {
			if (mem_8[cnt-1] != cmpr_mem_8[0]) {
				err_flag |= -1;
				printf("\nByte %d, 0x%02x (0x%08x) != 0x%02x (0x%08x)", cnt-1, mem_8[cnt-1], &mem_8[cnt-1], 
				       cmpr_mem_8[0], &cmpr_mem_8[0]);
			}

		} else if ((SOCLE_DMA_DIR_FIXED == src_dir) &&
			   (SOCLE_DMA_DIR_INCR == dst_dir)) {
			for (i = 0; i < cnt; i++) {
				if (mem_8[0] != cmpr_mem_8[i]) {
					err_flag |= -1;
					printf("\nByte %d, 0x%02x (0x%08x) != 0x%02x (0x%08x)", i, mem_8[0], &mem_8[0], 
					       cmpr_mem_8[i], &cmpr_mem_8[i]);
				}
			}
		} else if ((SOCLE_DMA_DIR_FIXED == src_dir) &&
			   (SOCLE_DMA_DIR_FIXED == dst_dir)) {
			if (mem_8[0] != cmpr_mem_8[0]) {
				err_flag |= -1;
				printf("\nByte 0, 0x%02x (0x%08x) != 0x%02x (0x%08x)", mem_8[0], &mem_8[0], 
				       cmpr_mem_8[0], &cmpr_mem_8[0]);
			}
		} else {
			err_flag |= -1;
			printf("Panther7 HDMA: unknown direction for source and destination\n");
		}
		break;
	case SOCLE_DMA_DATA_HALFWORD:
		if ((SOCLE_DMA_DIR_INCR == src_dir) &&
		    (SOCLE_DMA_DIR_INCR == dst_dir)) {
			for (i = 0; i < (cnt >> 1); i++) {
				if (mem_16[i] != cmpr_mem_16[i]) {
					err_flag |= -1;
					printf("\nHalfword %d, 0x%04x (0x%08x) != 0x%04x (0x%08x)", i, mem_16[i], &mem_16[i], 
					       cmpr_mem_16[i], &cmpr_mem_16[i]);
				}
			}
		} else if ((SOCLE_DMA_DIR_INCR == src_dir) &&
			   (SOCLE_DMA_DIR_FIXED == dst_dir)) {
			if (mem_16[(cnt>>1)-1] != cmpr_mem_16[0]) {
				err_flag |= -1;
				printf("\nHalfword %d, 0x%04x (0x%08x) != 0x%04x (0x%08x)", (cnt>>1)-1, mem_16[(cnt>>1)-1], &mem_16[(cnt>>1)-1], 
				       cmpr_mem_16[0], &cmpr_mem_16[0]);
			}

		} else if ((SOCLE_DMA_DIR_FIXED == src_dir) &&
			   (SOCLE_DMA_DIR_INCR == dst_dir)) {
			for (i = 0; i < (cnt >> 1); i++) {
				if (mem_16[0] != cmpr_mem_16[i]) {
					err_flag |= -1;
					printf("\nHalfword %d, 0x%04x (0x%08x) != 0x%04x (0x%08x)", i, mem_16[0], &mem_16[0], 
					       cmpr_mem_16[i], &cmpr_mem_16[i]);
				}
			}
		} else if ((SOCLE_DMA_DIR_FIXED == src_dir) &&
			   (SOCLE_DMA_DIR_FIXED == dst_dir)) {
			if (mem_16[0] != cmpr_mem_16[0]) {
				err_flag |= -1;
				printf("\nHalfword 0, 0x%04x (0x%08x) != 0x%04x (0x%08x)", mem_16[0], &mem_16[0], 
				       cmpr_mem_16[0], &cmpr_mem_16[0]);
			}
		} else {
			err_flag |= -1;
			printf("Panther7 HDMA: unknown direction for source and destination\n");
		}
		break;
	case SOCLE_DMA_DATA_WORD:
		if ((SOCLE_DMA_DIR_INCR == src_dir) &&
		    (SOCLE_DMA_DIR_INCR == dst_dir)) {
			for (i = 0; i < (cnt >> 2); i++) {
				if (mem_32[i] != cmpr_mem_32[i]) {
					err_flag |= -1;
					printf("\nWord %d, 0x%08x (0x%08x) != 0x%08x (0x%08x)", i, mem_32[i], &mem_32[i], 
					       cmpr_mem_32[i], &cmpr_mem_32[i]);
				}
			}
		} else if ((SOCLE_DMA_DIR_INCR == src_dir) &&
			   (SOCLE_DMA_DIR_FIXED == dst_dir)) {
			if (mem_32[(cnt>>2)-1] != cmpr_mem_32[0]) {
				err_flag |= -1;
				printf("\nWord %d, 0x%08x (0x%08x) != 0x%08x (0x%08x)", (cnt>>2)-1, mem_32[(cnt>>2)-1], &mem_32[(cnt>>2)-1], 
				       cmpr_mem_32[0], &cmpr_mem_32[0]);
			}

		} else if ((SOCLE_DMA_DIR_FIXED == src_dir) &&
			   (SOCLE_DMA_DIR_INCR == dst_dir)) {
			for (i = 0; i < (cnt >> 2); i++) {
				if (mem_32[0] != cmpr_mem_32[i]) {
					err_flag |= -1;
					printf("\nWord %d, 0x%08x (0x%08x) != 0x%08x (0x%08x)", i, mem_32[0], &mem_32[0], 
					       cmpr_mem_32[i], &cmpr_mem_32[i]);
				}
			}
		} else if ((SOCLE_DMA_DIR_FIXED == src_dir) &&
			   (SOCLE_DMA_DIR_FIXED == dst_dir)) {
			if (mem_32[0] != cmpr_mem_32[0]) {
				err_flag |= -1;
				printf("\nWord 0, 0x%08x (0x%08x) != 0x%08x (0x%08x)", mem_32[0], &mem_32[0], 
				       cmpr_mem_32[0], &cmpr_mem_32[0]);
			}
		} else {
			err_flag |= -1;
			printf("Panther7 HDMA: unknown direction for source and destination\n");
		}
		break;
	default:
		err_flag |= -1;
		printf("Panther7 HDMA: unknown data size\n");
	}

	return err_flag;
}

static void 
panther7_hdma_notifier_complete(void *data)
{
	panther7_complete_flag = 1;
}

