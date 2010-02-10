#include <test_item.h>
#include <genlib.h>
#include <global.h>
#include "rscode-1.0_socle/ecc.h"
#include <dma/dma.h>
#include "nand-flsh-regs.h"
#include "dependency.h"

#ifdef CONFIG_CACHE
#include "../CACHE/cache.h"
#endif

#if defined (CONFIG_PC9220)
#include <scu.h>
#include "../../platform/arch/scu-reg.h"
#endif


#define TO_THRESHOLD_CNT (1024 * 1024)
#define SCU_AHB_MODE (0x1<<6)

/*
 * Standard NAND flash commands
 */
#define NAND_CMD_READ0		0
#define NAND_CMD_READ1		1
#define NAND_CMD_PAGEPROG	0x10
#define NAND_CMD_READOOB	0x50
#define NAND_CMD_ERASE1		0x60
#define NAND_CMD_STATUS		0x70
#define NAND_CMD_STATUS_MULTI	0x71
#define NAND_CMD_SEQIN		0x80
#define NAND_CMD_READID		0x90
#define NAND_CMD_ERASE2		0xd0
#define NAND_CMD_RESET		0xff

/* Extended commands for large page devices */
#define NAND_CMD_READSTART	0x30
#define NAND_CMD_CACHEDPROG	0x15

#define SCU_NAND_MODE (0x1<<7)


struct nand_chip {
	u32 writesize;
	u32 ppblock;
	u32 oobfree_pos;
	u32 oobfree_len;
	int (*erase_block)(u32 page_addr);
	int (*read_page)(u32 column, u32 page_addr, u32 len, int ecc);
	int (*program_page)(u32 column, u32 page_addr, u32 len, int ecc);
	int (*cache_program_page)(u32 column, u32 page_addr, u32 len, int ecc);
	int (*copyback_page)(u32 column_s, u32 page_addr_s, u32 column_d, u32 page_addr_d, u32 len);
};

static inline void
socle_nand_flsh_write(u32 reg, u32 val, u32 base)
{
	base = base + reg;
	iowrite32(val, base);
}

static inline u32
socle_nand_flsh_read(u32 reg, u32 base)
{
	u32 val;

	base = base + reg;
	val = ioread32(base);
	return val;
}

static int socle_nand_flsh_erase_block(u32 page_addr);
static int socle_nand_flsh_read_page_l(u32 column, u32 page_addr, u32 len, int ecc);
static int socle_nand_flsh_program_page_l(u32 column, u32 page_addr, u32 len, int ecc);
static int socle_nand_flsh_cache_program_page_l(u32 column, u32 page_addr, u32 len, int ecc);
static int socle_nand_flsh_copyback_page_l(u32 column_s, u32 page_addr_s, u32 column_d, u32 page_addr_d, u32 len);
static int socle_nand_flsh_read_page_s(u32 column, u32 page_addr, u32 len, int ecc);
static int socle_nand_flsh_program_page_s(u32 column, u32 page_addr, u32 len, int ecc);
static int socle_nand_flsh_copyback_page_s(u32 column_s, u32 page_addr_s, u32 column_d, u32 page_addr_d, u32 len);
static void socle_nand_flsh_mark_ecc_err(void);
static void socle_nand_flsh_mark_err(u32 err_pos);
static int socle_nand_flsh_ecc_program(u32 page_addr);
static int socle_nand_flsh_ecc_read(u32 page_addr);
static int socle_nand_flsh_ecc(int num_err);
static int socle_nand_flsh_autoread(u32 read_pos, int writing);
static void socle_nand_flsh_setup_chip(int large_page);
static void socle_nand_flsh_select_chip(int chipnr);
static void socle_nand_flsh_fill_testpattern(u8 *pattern_addr, u32 base, u32 len);
static void socle_nand_flsh_buf_transfer(u8 *buf, u32 len, int writing);
/* 20080321 leonid+ for ecc correct data */
static void socle_nand_flsh_buf_transfer2(u8 *buf, u32 len, int writing, u32 offset);
static void socle_nand_flsh_clear_buffer(void);
static int socle_nand_flsh_verify_pages(struct nand_chip *chip, u32 startpage, u32 numpages,
					int skip_cmpr_result);
static int socle_nand_flsh_verify_buffer(u8 *buf, u32 len, int skip_cmpr_result);
static int socle_nand_flsh_compare_memory(u32 *mem, u32 *mem_compare, u32 size,
					  int skip_cmpr_result);
static int socle_nand_flsh_sw_dma_transfer(int autotest);
static int socle_nand_flsh_sw_dma_verify_pages(struct nand_chip *chip, u32 startpage, u32 numpage,
					       int skip_cmpr_result);
static void socle_nand_flsh_dma_complete(void *data);
static void socle_nand_flsh_isr(void *data);

/*20080321 leonid+ for workaround correct data function*/
static int socle_nand_flsh_correct_data(int status, u32 page);	
/*20080321 leonid+ for normal correct data function */
static int socle_nand_flsh_correct_data2(int status, u32 page);

static struct nand_chip chip;
static u32 socle_nand_flsh_base;
static volatile u32 *socle_nand_flsh_buffer;
static volatile u32 *socle_nand_flsh_prom;
static volatile u32 *socle_nand_ecc_test_buffer;
static u32 socle_nand_flsh_dma_channel_num;
static u32 socle_nand_flsh_dma_burst_type;
static volatile int socle_nand_flsh_dma_complete_flag = 0;
static volatile int socle_nand_flsh_int_flag = 0;
static volatile u32 socle_nand_flsh_int_stat;

static struct socle_dma_notifier socle_nand_flsh_dma_notifier = {
	.complete = socle_nand_flsh_dma_complete,
};

int npar;
static u32 ml;	/* data size + npar */
unsigned short codeword[512 + 12] = {0};
static int ecc_init_flag = 0;
static u8 *socle_nand_flsh_ecc_correct_buffer = (u8 *)0x00b00000;
static u8 *socle_nand_flsh_pattern_addr = (u8 *)0x00a00000;
static u8 *socle_nand_flsh_comare_addr = (u8 *)0x00a08000;

extern struct test_item_container socle_nand_flsh_main_test_container;


extern int
nand_test(int autotest)
{
	int ret = 0;

#ifdef CONFIG_SCDK
	if(ioread32(SOCLE_APB0_SCU + 0x28) & SCU_AHB_MODE) {
		printf("\nCan not test NAND in AMBA mode\n\n");
		return -1;
	}
#endif

#ifdef CONFIG_PC9220
	socle_scu_dev_enable(SCU_DEVCON_NFC_GPIO);
#endif

#if defined(CONFIG_CDK) || defined(CONFIG_PC9002) || defined(CONFIG_SCDK)
  	iowrite32(ioread32(SOCLE_APB0_SCU + 0x28) | SCU_NAND_MODE, SOCLE_APB0_SCU + 0x28);
#endif

	//20080321 leonid+ for NAND reset
	socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_EN, socle_nand_flsh_base);

	socle_nand_flsh_base = SOCLE_AHB0_NAND0;
	request_irq(SOCLE_INTC_NAND0, socle_nand_flsh_isr, NULL);

//	iowrite32(0xc0, 0x1804006c);	//20080115 leonid+ for nand level triggle

	/* Initialize the environment */
	socle_nand_flsh_buffer = (u32 *)SOCLE_NAND_FLSH_BUF_ADDR(socle_nand_flsh_base);
	socle_nand_flsh_prom = (u32 *)socle_nand_flsh_base;
	socle_nand_ecc_test_buffer = (u32 *)(SOCLE_NAND_FLSH_BUF_ADDR(socle_nand_flsh_base) + 512);

	socle_nand_flsh_setup_chip(1);
	ret = test_item_ctrl(&socle_nand_flsh_main_test_container, autotest);
	free_irq(SOCLE_INTC_NAND0);

#ifdef CONFIG_PC9220
	socle_scu_dev_disable(SCU_DEVCON_NFC_GPIO);
#endif

#if defined(CONFIG_CDK) || defined(CONFIG_PC9002) || defined(CONFIG_SCDK)
  	iowrite32(ioread32(SOCLE_APB0_SCU + 0x28) & ~SCU_NAND_MODE, SOCLE_APB0_SCU + 0x28);
#endif

	return ret;
}

extern struct test_item_container socle_nand_flsh_parity_test_container;

extern int
socle_nand_flsh_parity_8bytes(int autotest)
{
	int ret = 0;

	npar=6;
	ml=512+npar;
	ecc_init_flag=0;
	
	ret = test_item_ctrl(&socle_nand_flsh_parity_test_container, autotest);
	return ret;
}

extern int
socle_nand_flsh_parity_15bytes(int autotest)
{
	int ret = 0;
		
	npar=12;
	ml=512+npar;
	ecc_init_flag=0;

	ret = test_item_ctrl(&socle_nand_flsh_parity_test_container, autotest);
	return ret;
}

extern struct test_item_container socle_nand_flsh_parity_switch_container;

extern int
socle_nand_flsh_parity_switch(int autotest)
{
	int err_flag = 0;

#ifdef SOCLE_NAND_FLSH_FIRST4_LEN_15B
	npar = 6;
#else
	npar = 12;
#endif	
	ml=512+npar;
	ecc_init_flag=0;

	/* Select the chip */
	socle_nand_flsh_select_chip(0);

	/* Erase the 0th block for following test*/
	if (chip.erase_block(0*chip.ppblock)) {	
		err_flag |= -1;
		printf("\nSocle NAND Flash host: erasing 0th block is failed\n");
		goto out;
	}

	/* program 0th page */
	socle_nand_flsh_fill_testpattern(socle_nand_flsh_pattern_addr, 0, 2048);	
	socle_nand_flsh_buf_transfer(socle_nand_flsh_pattern_addr, 2048, 1); 
	if (chip.program_page(0, 0, 2048, 1)) {	
			printf("\nSocle NAND Flash host: programming page 0 is failed\n");	
			err_flag |= -1;
			goto out;
	}

	/* program 1th page */
	socle_nand_flsh_fill_testpattern(socle_nand_flsh_pattern_addr, 0, 2048);	
	socle_nand_flsh_buf_transfer(socle_nand_flsh_pattern_addr, 2048, 1); 
	if (chip.program_page(0, 1, 2048, 1)) {	
			printf("\nSocle NAND Flash host: programming page 1 is failed\n");
			err_flag |= -1;
			goto out;
	}

	/* program 8th page */
	socle_nand_flsh_fill_testpattern(socle_nand_flsh_pattern_addr, 0, 2048);	
	socle_nand_flsh_buf_transfer(socle_nand_flsh_pattern_addr, 2048, 1); 
	if (chip.program_page(0, 8, 2048, 1)) {	
			printf("\nSocle NAND Flash host: programming page 8 is failed\n");	
			err_flag |= -1;
			goto out;
	}

	/* program 9th page */
	socle_nand_flsh_fill_testpattern(socle_nand_flsh_pattern_addr, 0, 2048);	
	socle_nand_flsh_buf_transfer(socle_nand_flsh_pattern_addr, 2048, 1); 
	if (chip.program_page(0, 9, 2048, 1)) {	
			printf("\nSocle NAND Flash host: programming page 9 is failed\n");
			err_flag |= -1;
			goto out;
	}
	
out:
	/* De-select the chip */
	socle_nand_flsh_select_chip(-1);

	if (!err_flag) 
		/* Verify by switch mode */
		return test_item_ctrl(&socle_nand_flsh_parity_switch_container, autotest);
	else
		return err_flag;
}

extern int socle_nand_flsh_parity_switch_verify(u32 page_addr1, u32 page_addr2, int autotest);

extern int socle_nand_flsh_parity_switch_8to8(int autotest)
{

#ifdef SOCLE_NAND_FLSH_FIRST4_LEN_15B
	return socle_nand_flsh_parity_switch_verify(8, 9, autotest);
#else	
	return socle_nand_flsh_parity_switch_verify(0, 1, autotest);
#endif

}

extern int socle_nand_flsh_parity_switch_8to15(int autotest)
{

#ifdef SOCLE_NAND_FLSH_FIRST4_LEN_15B
	return socle_nand_flsh_parity_switch_verify(8, 0, autotest);
#else	
	return socle_nand_flsh_parity_switch_verify(0, 8, autotest);
#endif

}

extern int socle_nand_flsh_parity_switch_15to8(int autotest)
{

#ifdef SOCLE_NAND_FLSH_FIRST4_LEN_15B
	return socle_nand_flsh_parity_switch_verify(0, 8, autotest);
#else	
	return socle_nand_flsh_parity_switch_verify(8, 0, autotest);
#endif

}

extern int socle_nand_flsh_parity_switch_15to15(int autotest)
{

#ifdef SOCLE_NAND_FLSH_FIRST4_LEN_15B
	return socle_nand_flsh_parity_switch_verify(0, 1, autotest);
#else	
	return socle_nand_flsh_parity_switch_verify(8, 9, autotest);
#endif

}

extern int
socle_nand_flsh_parity_switch_verify(u32 page_addr1, u32 page_addr2, int autotest)
{
	int err_flag = 0;

	err_flag = socle_nand_flsh_verify_pages(&chip, page_addr1, 1, autotest);
	if(err_flag)
		return err_flag;
	else
		return socle_nand_flsh_verify_pages(&chip, page_addr2, 1, autotest);	
}

extern struct test_item_container socle_nand_flsh_transfer_test_container;

extern int
socle_nand_flsh_transfer_test(int autotest)
{
	int ret = 0;

	ret = test_item_ctrl(&socle_nand_flsh_transfer_test_container, autotest);
	return ret;
}

extern struct test_item_container socle_nand_flsh_ecc8_test_container;
extern struct test_item_container socle_nand_flsh_ecc15_test_container;

extern int 
socle_nand_flsh_ecc_test(int autotest)
{
	int ret = 0;
	
	if(npar == 6)
		ret = test_item_ctrl(&socle_nand_flsh_ecc8_test_container, autotest);
	else
		ret = test_item_ctrl(&socle_nand_flsh_ecc15_test_container, autotest);
	return ret;
}


extern struct test_item_container socle_nand_flsh_autoread_test_container;

extern int
socle_nand_flsh_autoread_test(int autotest)
{
	int ret = 0;
	ret = test_item_ctrl(&socle_nand_flsh_autoread_test_container, autotest);
	return ret;
}

extern struct test_item_container socle_nand_flsh_swdma_burst_type_test_container;

extern int
socle_nand_flsh_a2a_burst_type_test(int autotest)
{
	int ret = 0;

	socle_nand_flsh_dma_channel_num = SOCLE_A2A_CH_0;
	socle_request_dma(socle_nand_flsh_dma_channel_num, &socle_nand_flsh_dma_notifier);
	ret = test_item_ctrl(&socle_nand_flsh_swdma_burst_type_test_container, autotest);
	socle_disable_dma(socle_nand_flsh_dma_channel_num);
	socle_free_dma(socle_nand_flsh_dma_channel_num);
	return ret;
}

extern int
socle_nand_flsh_hdma_burst_type_test(int autotest)
{
	int ret = 0;

	socle_nand_flsh_dma_channel_num = SOCLE_HDMA_CH_0;
	socle_request_dma(socle_nand_flsh_dma_channel_num, &socle_nand_flsh_dma_notifier);
	ret = test_item_ctrl(&socle_nand_flsh_swdma_burst_type_test_container, autotest);
	socle_disable_dma(socle_nand_flsh_dma_channel_num);
	socle_free_dma(socle_nand_flsh_dma_channel_num);
	return ret;
}

extern int
socle_nand_flsh_panther7_hdma_burst_type_test(int autotest)
{
	int ret = 0;

	socle_nand_flsh_dma_channel_num = PANTHER7_HDMA_CH_0;
	socle_request_dma(socle_nand_flsh_dma_channel_num, &socle_nand_flsh_dma_notifier);
	ret = test_item_ctrl(&socle_nand_flsh_swdma_burst_type_test_container, autotest);
	socle_disable_dma(socle_nand_flsh_dma_channel_num);
	socle_free_dma(socle_nand_flsh_dma_channel_num);
	return ret;
}

extern int 
socle_nand_flsh_transfer_ecc(int autotest)
{
	int i;
	int err_flag = 0;

	/* Select the chip */
	socle_nand_flsh_select_chip(0);

	/* Erase the 1th block for following test*/
	if (chip.erase_block(1*chip.ppblock)) {	
		err_flag |= -1;
		printf("\nSocle NAND Flash host: erasing 1th block is failed\n");
		goto out;
	}

	/* Loop until the total pages of a block have been programmed */
	for (i = 0; i < chip.ppblock; i++) {
		socle_nand_flsh_fill_testpattern(socle_nand_flsh_pattern_addr, i*chip.writesize, chip.writesize);		
		socle_nand_flsh_buf_transfer(socle_nand_flsh_pattern_addr, chip.writesize, 1); 
		if (chip.program_page(0, (i+chip.ppblock), chip.writesize, 1)) {	
			printf("\nSocle NAND Flash host: programming page #%d is failed\n", (i+chip.ppblock));	
			err_flag |= -1;
			goto out;
		}
	}
	
out:
	/* De-select the chip */
	socle_nand_flsh_select_chip(-1);

	if (!err_flag) 
		/* Verify all pages that have been written */
		return socle_nand_flsh_verify_pages(&chip, chip.ppblock, chip.ppblock, autotest);		
	else
		return err_flag;
}

extern int 
socle_nand_flsh_transfer_ecc_cached(int autotest)
{
	int i;
	int err_flag = 0;

	/* Select the chip */
	socle_nand_flsh_select_chip(0);

	/* Erase the 1th block for following test*/	
	if (chip.erase_block(1*chip.ppblock)) {	
		err_flag |= -1;
		printf("\nSocle NAND Flash host: erasing 1th block is failed\n");
		goto out;
	}

	/* Loop until the total pages of a block have been programmed */
	for (i = 0; i < chip.ppblock; i++) {
		socle_nand_flsh_fill_testpattern(socle_nand_flsh_pattern_addr, i*chip.writesize, chip.writesize);		
		socle_nand_flsh_buf_transfer(socle_nand_flsh_pattern_addr, chip.writesize, 1);
		if (i == (chip.ppblock - 1)) {
			/* Use real program */
			if (chip.program_page(0, (i+chip.ppblock), chip.writesize, 1)) {	
				printf("\nSocle NAND Flash host: programming %d page is failed\n", (i+chip.ppblock));	
				err_flag |= -1;
				goto out;
			}
		} else {
			if (chip.cache_program_page(0, (i+chip.ppblock), chip.writesize, 1)) {	
				printf("\nSocle NAND Flash host: cached programming %d page is failed\n", (i+chip.ppblock));	
				err_flag |= -1;
				goto out;
			}
		}
	}

out:
	/* De-select the chip */
	socle_nand_flsh_select_chip(-1);

	if (!err_flag) 
		/* Verify all pages that have been written */
		return socle_nand_flsh_verify_pages(&chip, chip.ppblock, chip.ppblock, autotest);	
	else
		return err_flag;
}

extern int 
socle_nand_flsh_transfer_raw(int autotest)
{
	int i;
	int err_flag = 0;

	/* Select the chip */
	socle_nand_flsh_select_chip(0);

	/* Erase the 1th block for following test*/
	if (chip.erase_block(1*chip.ppblock)) {	
		err_flag |= -1;
		printf("\nSocle NAND Flash host: erasing 1th block is failed\n");
		goto out;
	}

	/* Loop until the total pages of a block have been programmed */
	for (i = 0; i < chip.ppblock; i++) {
		socle_nand_flsh_fill_testpattern(socle_nand_flsh_pattern_addr, i*chip.writesize, chip.oobfree_len);		
		socle_nand_flsh_buf_transfer(socle_nand_flsh_pattern_addr, chip.oobfree_len, 1);
		if (chip.program_page(chip.writesize+chip.oobfree_pos, (i+chip.ppblock), chip.oobfree_len, 0)) {	
			printf("\nSocle NAND Flash host: programming %d page is failed\n", (i+chip.ppblock));	
			err_flag |= -1;
			goto out;
		}
	}

out:
	if (!err_flag) {
		/* Verify all pages that have been written */
		for (i = 0; i < chip.ppblock; i++) {
			socle_nand_flsh_fill_testpattern(socle_nand_flsh_pattern_addr, i*chip.writesize, chip.oobfree_len);	
			socle_nand_flsh_clear_buffer();
			chip.read_page(chip.writesize+chip.oobfree_pos, (i+chip.ppblock), chip.oobfree_len, 0);
			if (socle_nand_flsh_verify_buffer(socle_nand_flsh_pattern_addr, chip.oobfree_len, autotest)) {
				err_flag |= -1;
				printf("\nSocle NAND Flash host: comparing page #%d......fail\n", (i+chip.ppblock));
			} else {
				if(!autotest)
					printf("\nSocle NAND Flash host: comparing page #%d......pass\n", (i+chip.ppblock));
			}
			if (err_flag)
				break;
		}
	}

	/* De-select the chip */
	socle_nand_flsh_select_chip(-1);

	return err_flag;
}

extern int 
socle_nand_flsh_transfer_copyback(int autotest)
{
	int err_flag = 0;

	/* Select the chip */
	socle_nand_flsh_select_chip(0);

	/* Erase the 1th block for following test*/
	if (chip.erase_block(1*chip.ppblock)) {
		err_flag |= -1;
		printf("\nSocle NAND Flash host: erasing 1th block is failed\n");	
		goto out;
	}

	/* Erase the 4th block for following test*/
	if (chip.erase_block(4*chip.ppblock)) {
		err_flag |= -1;
		printf("\nSocle NAND Flash host: erasing 4th block is failed\n");
		goto out;
	}

	socle_nand_flsh_fill_testpattern(socle_nand_flsh_pattern_addr, 0, chip.writesize);
	socle_nand_flsh_buf_transfer(socle_nand_flsh_pattern_addr, chip.writesize, 1);

	/* Program test pattern into the 0th page of 1th block */
	if (chip.program_page(0, chip.ppblock, chip.writesize, 1)) {	
		err_flag |= -1;
		printf("\nSocle NAND Flash host: programming 0th page is failed\n");
		goto out;
	}

	/*
	 *  Move 0th page to 1st page using copy-back operation	
	 	Move 0th page of block1 to 0th page of block4
	 *  */
	if (chip.copyback_page(0, chip.ppblock, 0, 4*chip.ppblock, chip.writesize)) {
		err_flag |= -1;
		printf("\nSocle NAND Flash host: copy-back 0th page of 1th block to 0th page of 4th block is failed\n");
		goto out;
	}

	/* Read the 1st page, and compare with 0th page */
	if (chip.read_page(0, 4*chip.ppblock, chip.writesize, 1)) {
		err_flag |= -1;
		printf("\nSocle NAND Flash host: reading 0th page of 4th block cause a ecc error\n");
		goto out;
	}

	if (socle_nand_flsh_verify_buffer(socle_nand_flsh_pattern_addr, chip.writesize, autotest))
		err_flag |= -1;

out:
	/* De-select the chip */
	socle_nand_flsh_select_chip(-1);

	return err_flag;     
}

extern int
socle_nand_flsh_ecc_error_0(int autotest)
{
	return socle_nand_flsh_ecc(0);
}

extern int
socle_nand_flsh_ecc_error_1(int autotest)
{
	return socle_nand_flsh_ecc(1);
}

extern int
socle_nand_flsh_ecc_error_2(int autotest)
{
	return socle_nand_flsh_ecc(2);
}

extern int
socle_nand_flsh_ecc_error_3(int autotest)
{
	return socle_nand_flsh_ecc(3);
}

extern int
socle_nand_flsh_ecc_error_4(int autotest)
{
	return socle_nand_flsh_ecc(4);
}

extern int
socle_nand_flsh_ecc_error_5(int autotest)
{
	return socle_nand_flsh_ecc(5);
}

extern int
socle_nand_flsh_ecc_error_6(int autotest)
{
	return socle_nand_flsh_ecc(6);
}

extern int
socle_nand_flsh_ecc_error_7(int autotest)
{
	return socle_nand_flsh_ecc(7);
}

static int 
socle_nand_flsh_ecc(int num_err)
{
	int i, err_pos, err_flag = 0;
	u32 hw_pBytes[MAXNPAR] = {0};
	u32 hw_synBytes[MAXNPAR] = {0};
	u32 tmp;

	if (!ecc_init_flag) {
		initialize_ecc();
		ecc_init_flag = 1;
	}
     
	/* Fill the test pattern for ecc module test */
	for (i = 0; i < 511; i++)	
		socle_nand_flsh_pattern_addr[i] = i+1;
	socle_nand_flsh_pattern_addr[511] = 0xff;

	/* Encode data using C-Model */
	encode_data(socle_nand_flsh_pattern_addr, 512, codeword);

	/*
	 *  Encode data using hw ecc moudle
	 *  */
	socle_nand_flsh_buf_transfer(socle_nand_flsh_pattern_addr, 512, 1);

	/* Program using ecc test mode */
	socle_nand_flsh_ecc_program(chip.ppblock);
     
	/* Assign hw ecc code to hw_pBytes[] */
	tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_ECC_1, socle_nand_flsh_base);
	hw_pBytes[0] = SOCLE_NAND_FLSH_ECC_CODE_1(tmp);
	hw_pBytes[1] = SOCLE_NAND_FLSH_ECC_CODE_2(tmp);
	hw_pBytes[2] = SOCLE_NAND_FLSH_ECC_CODE_3(tmp);
	tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_ECC_2, socle_nand_flsh_base);
	hw_pBytes[3] = SOCLE_NAND_FLSH_ECC_CODE_1(tmp);
	hw_pBytes[4] = SOCLE_NAND_FLSH_ECC_CODE_2(tmp);
	hw_pBytes[5] = SOCLE_NAND_FLSH_ECC_CODE_3(tmp);
	if (npar == 12){
		tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_ECC_3, socle_nand_flsh_base);
		hw_pBytes[6] = SOCLE_NAND_FLSH_ECC_CODE_1(tmp);
		hw_pBytes[7] = SOCLE_NAND_FLSH_ECC_CODE_2(tmp);
		hw_pBytes[8] = SOCLE_NAND_FLSH_ECC_CODE_3(tmp);
		tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_ECC_4, socle_nand_flsh_base);
		hw_pBytes[9] = SOCLE_NAND_FLSH_ECC_CODE_1(tmp);
		hw_pBytes[10] = SOCLE_NAND_FLSH_ECC_CODE_2(tmp);
		hw_pBytes[11] = SOCLE_NAND_FLSH_ECC_CODE_3(tmp);
	}
	/* Compare ecc code generated by C-Model and hw ecc module */
	for (i = 0; i < npar; i++) {
		if (hw_pBytes[i] != pBytes[i]) {
			printf("\nSocle NAND Flash host: ecc codes are mismatch at number %d\n", i);
			printf("\nSocle NAND Flash host: HW ecc code: 0x%x, C-Module ecc code: 0x%x\n",
			       hw_pBytes[i], pBytes[i]);
			err_flag |= -1;
		}
	}
	if (err_flag)
		return -1;

	/* Make errors */
	for (i = 0; i < num_err; i++) {
		/* We mark error on parity code area first */
		if (0 == i) {
			socle_nand_flsh_mark_ecc_err();
			continue;
		}
	       
	input_err_pos:
		printf("\nSocle NAND Flash host: input the error position you want to make (0 ~ 511):");
		scanf("%d", &err_pos);
		printf("%d\n", err_pos);
		if ((err_pos < 0) || (err_pos > 511)) {
			printf("\nSocle NAND Flash host: unacceptable position (%d), please input again\n", err_pos);
			goto input_err_pos;
		}
		socle_nand_flsh_mark_err(err_pos);
	}

	/* Decode data using C-Module */
	decode_data(codeword, ml);

	/*
	 *  Decode data using hw ecc moudle
	 *  */
	/* Read using ecc test mode */
	socle_nand_flsh_ecc_read(chip.ppblock);
     
	/* Reading from nand buffer to buffer in ram to correct data */
	socle_nand_flsh_buf_transfer(socle_nand_flsh_ecc_correct_buffer, 512, 0);

	/* Assign syndrome value to ecc correct moudle */
	tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_SYNDR_1, socle_nand_flsh_base);
	hw_synBytes[0] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
	hw_synBytes[1] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
	hw_synBytes[2] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
	tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_SYNDR_2, socle_nand_flsh_base);
	hw_synBytes[3] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
	hw_synBytes[4] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
	hw_synBytes[5] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
	if (npar == 12){
		tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_SYNDR_3, socle_nand_flsh_base);
		hw_synBytes[6] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
		hw_synBytes[7] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
		hw_synBytes[8] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
		tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_SYNDR_4, socle_nand_flsh_base);
		hw_synBytes[9] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
		hw_synBytes[10] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
		hw_synBytes[11] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
	}
	/* Compare the syndrome generated by C-Module with hw ecc module */
	for (i = 0; i < npar; i++) {
		if (hw_synBytes[i] != synBytes[i]) {
			printf("\nSocle NAND Flash host: syndrome are mismatch at number %d\n", i);
			printf("\nSocle NAND Flash host: HW syndrome: 0x%x, C-Module syndrome: 0x%x\n",
			       hw_synBytes[i], synBytes[i]);
			err_flag |= -1;
		}
	}
	if (err_flag)
		return -1;

	/* Do the correction process */
	(void)correct_errors_hw_ecc(socle_nand_flsh_ecc_correct_buffer, ml);

	/* Compare the corrected data with original data */
	for (i = 0; i < 512; i++) {
		if (socle_nand_flsh_pattern_addr[i] != socle_nand_flsh_ecc_correct_buffer[i])
			return -1;
	}
	return 0;
}

extern int
socle_nand_flsh_autoread_0(int autotest)
{
	return socle_nand_flsh_autoread(0, 1);
}

extern int
socle_nand_flsh_autoread_2048(int autotest)
{
	return socle_nand_flsh_autoread(1, 1);
}

extern int 
socle_nand_flsh_autoread_4096(int autotest)
{
	return socle_nand_flsh_autoread(2, 1);
}

extern int
socle_nand_flsh_autoread_6144(int autotest)
{
	return socle_nand_flsh_autoread(3, 1);
}

extern int
socle_nand_flsh_autoread_erase(int autotest)
{
	return socle_nand_flsh_autoread(0, -1);
}

extern int
socle_nand_flsh_autoread_compare(int autotest)
{
	return socle_nand_flsh_autoread(0, 0);
}

extern int 
socle_nand_flsh_autoread(u32 read_pos, int writing)
{
	int err_flag = 0;

	socle_nand_flsh_fill_testpattern(socle_nand_flsh_pattern_addr, 0, 2048);
	if (writing) {
		u32 i;
		u32 page_addr = (2048 * read_pos) / chip.writesize;
		u32 len = 2048 / chip.writesize;

		/* Select the chip */
		socle_nand_flsh_select_chip(0);

		if (chip.erase_block(0)) {
			err_flag |= -1;
			printf("\nSocle NAND Flash host: erasing 0th block is failed\n");
			goto out;
		}

		if ((-1) == writing)
			goto out;

		for (i = 0; i < len; i++) {
			/* Program data into desired page */
			socle_nand_flsh_buf_transfer(socle_nand_flsh_pattern_addr+(i*chip.writesize), chip.writesize, 1);
			if (chip.program_page(0, page_addr+i, chip.writesize, 1)) {
				err_flag |= -1;
				printf("\nSocle NAND Flash host: programming %d page is failed\n", i);
				goto out;
			}

			/* Read back the data to verify it*/
			if (chip.read_page(0, page_addr+i, chip.writesize, 1)) {
				err_flag |= -1;
				printf("\nSocle NAND Flash host: reading %d page cause a ecc error\n");
			}
			if (socle_nand_flsh_verify_buffer(socle_nand_flsh_pattern_addr+(i*chip.writesize), chip.writesize, 0))
				err_flag |= -1;
			if (err_flag)
				goto out;
		}
		socle_nand_flsh_clear_buffer();
		printf("\nSocle NAND Flash host: data writing is done\n");
	} else {
		if (socle_nand_flsh_verify_buffer(socle_nand_flsh_pattern_addr, 2048, 0)) {
			err_flag |= -1;
			printf("\nSocle NAND Flash host: comparing data is failed at 0x%x\n", socle_nand_flsh_pattern_addr);
			goto out;
		}
		printf("\nSocle NAND Flash host: compare data is pass at 0x%x\n", socle_nand_flsh_pattern_addr);
		if (socle_nand_flsh_verify_buffer((u8 *)socle_nand_flsh_prom, 2048, 0)) {
			err_flag |= -1;
			printf("\nSocle NAND Flash host: comparing data is failed at 0x%x\n", socle_nand_flsh_prom);
			goto out;
		}
		printf("\nSocle NAND Flash host: compare data is pass at 0x%x\n", socle_nand_flsh_prom);
	}

out:
	/* De-select the chip */
	socle_nand_flsh_select_chip(-1);

	return err_flag;
}

#define SOCLE_NAND_FLSH_SYN_ERR_MASK (SOCLE_NAND_FLSH_SYN_ERR_LOC_1 |	\
				      SOCLE_NAND_FLSH_SYN_ERR_LOC_2 |	\
				      SOCLE_NAND_FLSH_SYN_ERR_LOC_3 |	\
				      SOCLE_NAND_FLSH_SYN_ERR_LOC_4)

extern int
socle_nand_flsh_check_syndrome_bit(int autotest)
{
	int err_flag = 0;
	u32 status, loc;
	
	/* Select the chip */
	socle_nand_flsh_select_chip(0);
#if 0
	/* Erase the 1th block for following test */
	if (chip.erase_block(1*chip.ppblock)) {	
		err_flag |= -1;
		printf("\nSocle NAND Flash host: erasing 1th block is failed\n");	
		goto out;
	}

	status = chip.read_page(0, chip.ppblock, chip.writesize, 1);	
	printf("\nSocle NAND Flash host: status = 0x%08x\n", status);

	if (SOCLE_NAND_FLSH_SYN_ERR_MASK != (status & (SOCLE_NAND_FLSH_SYN_ERR_MASK)))
		err_flag |= -1;

#else
	/*	20080321 leonid+ for nand syndrome bug sw work around	*/
	/* Erase the 1th block for following test */
	if (chip.erase_block(1*chip.ppblock)) {	
		err_flag |= -1;
		printf("\nSocle NAND Flash host: erasing 1th block is failed\n");	
		goto out;
	}
	
	/* Loop until the total pages of a block have been programmed */
	socle_nand_flsh_fill_testpattern(socle_nand_flsh_pattern_addr,  chip.writesize, chip.writesize);	
	socle_nand_flsh_buf_transfer(socle_nand_flsh_pattern_addr, chip.writesize, 1); 
	if (chip.program_page(0, chip.ppblock, chip.writesize, 1)) {	
		printf("\nSocle NAND Flash host: programming page #%d is failed\n", chip.ppblock);	
		err_flag |= -1;
		goto out;
	}

	/*	test for 1st sync bit		*/		
	/*  write 1st block 0st page 0~2047 bytes to 1st block 1st page without ecc	*/
	if (chip.read_page(0, chip.ppblock, chip.writesize, 0)){
		printf("\nSocle NAND Flash host: reading page #%d cause a ecc error\n", chip.ppblock);
		err_flag |= -1;
		goto out;
	}
	/*	make 1st 512 data fail to 0xffffffff		*/
	loc = 0;
	socle_nand_flsh_buffer[loc] = 0xffffffff;
	if (chip.program_page(0, (chip.ppblock + 1), chip.writesize, 0)){
		printf("\nSocle NAND Flash host: programming page #%d is failed\n", (chip.ppblock + 1));	
		err_flag |= -1;
		goto out;
	}	
	/*  write 1st block 0st page 2048~2111 bytes to 1st block 1st page without ecc	*/
	if (chip.read_page(chip.writesize, chip.ppblock, (chip.oobfree_pos + chip.oobfree_len), 0)){
		printf("\nSocle NAND Flash host: reading page #%d cause a ecc error\n", chip.ppblock);
		err_flag |= -1;
		goto out;
	}
	if (chip.program_page(chip.writesize, (chip.ppblock + 1), (chip.oobfree_pos + chip.oobfree_len), 0)){
		printf("\nSocle NAND Flash host: programming page #%d is failed\n", (chip.ppblock + 1));	
		err_flag |= -1;
		goto out;
	}
	status = chip.read_page(0, (chip.ppblock + 1), chip.writesize, 1);	
	printf("\nSocle NAND Flash host: status = 0x%08x\n", status);
	if (SOCLE_NAND_FLSH_SYN_ERR_LOC_1 != (status & (SOCLE_NAND_FLSH_SYN_ERR_MASK))){
		err_flag |= -1;
		printf("test for 1st syndrome_bit error\n");
		goto out;
	}
	printf("test for 1st syndrome_bit pass\n");

	
	/*	test for 2nd sync bit		*/		
	/*  write 1st block 0st page 0~2047 bytes to 1st block 2nd page without ecc	*/
	if (chip.read_page(0, chip.ppblock, chip.writesize, 0)){
		printf("\nSocle NAND Flash host: reading page #%d cause a ecc error\n", chip.ppblock);
		err_flag |= -1;
		goto out;
	}
	/*	make 2nd 512 data fail to 0xffffffff		*/
	loc += (512/4 + 8);	//be sure the location is 2nd 512 data
	socle_nand_flsh_buffer[loc] = 0xffffffff;
	if (chip.program_page(0, (chip.ppblock + 2), chip.writesize, 0)){
		printf("\nSocle NAND Flash host: programming page #%d is failed\n", (chip.ppblock + 2));	
		err_flag |= -1;
		goto out;
	}	
	/*  write 1st block 0st page 2048~2111 bytes to 1st block 2nd page without ecc	*/
	if (chip.read_page(chip.writesize, chip.ppblock, (chip.oobfree_pos + chip.oobfree_len), 0)){
		printf("\nSocle NAND Flash host: reading page #%d cause a ecc error\n", chip.ppblock);
		err_flag |= -1;
		goto out;
	}
	if (chip.program_page(chip.writesize, (chip.ppblock + 2), (chip.oobfree_pos + chip.oobfree_len), 0)){
		printf("\nSocle NAND Flash host: programming page #%d is failed\n", (chip.ppblock + 2));	
		err_flag |= -1;
		goto out;
	}
	status = chip.read_page(0, (chip.ppblock + 2), chip.writesize, 1);	
	printf("\nSocle NAND Flash host: status = 0x%08x\n", status);
	if (SOCLE_NAND_FLSH_SYN_ERR_LOC_2 != (status & (SOCLE_NAND_FLSH_SYN_ERR_MASK))){
		err_flag |= -1;
		printf("test for 2nd syndrome_bit error\n");
		goto out;
	}
	printf("test for 2nd syndrome_bit pass\n");
	
	/*	test for 3nd sync bit		*/		
	/*  write 1st block 0st page 0~2047 bytes to 1st block 3nd page without ecc	*/
	if (chip.read_page(0, chip.ppblock, chip.writesize, 0)){
		printf("\nSocle NAND Flash host: reading page #%d cause a ecc error\n", chip.ppblock);
		err_flag |= -1;
		goto out;
	}
	/*	make 3nd 512 data fail to 0xffffffff		*/
	loc += (512/4 + 8);	//be sure the location is 3nd 512 data
	socle_nand_flsh_buffer[loc] = 0xffffffff;
	if (chip.program_page(0, (chip.ppblock + 3), chip.writesize, 0)){
		printf("\nSocle NAND Flash host: programming page #%d is failed\n", (chip.ppblock + 3));	
		err_flag |= -1;
		goto out;
	}	
	/*  write 1st block 0st page 2048~2111 bytes to 1st block 3nd page without ecc	*/
	if (chip.read_page(chip.writesize, chip.ppblock, (chip.oobfree_pos + chip.oobfree_len), 0)){
		printf("\nSocle NAND Flash host: reading page #%d cause a ecc error\n", chip.ppblock);
		err_flag |= -1;
		goto out;
	}
	if (chip.program_page(chip.writesize, (chip.ppblock + 3), (chip.oobfree_pos + chip.oobfree_len), 0)){
		printf("\nSocle NAND Flash host: programming page #%d is failed\n", (chip.ppblock + 3));	
		err_flag |= -1;
		goto out;
	}
	status = chip.read_page(0, (chip.ppblock + 3), chip.writesize, 1);	
	printf("\nSocle NAND Flash host: status = 0x%08x\n", status);
	if (SOCLE_NAND_FLSH_SYN_ERR_LOC_3 != (status & (SOCLE_NAND_FLSH_SYN_ERR_MASK))){
		err_flag |= -1;
		printf("test for 3nd syndrome_bit error\n");
		goto out;
	}
	printf("test for 3nd syndrome_bit pass\n");
	
	/*	test for 4nd sync bit		*/		
	/*  write 1st block 0st page 0~2047 bytes to 1st block 4nd page without ecc	*/
	if (chip.read_page(0, chip.ppblock, chip.writesize, 0)){
		printf("\nSocle NAND Flash host: reading page #%d cause a ecc error\n", chip.ppblock);
		err_flag |= -1;
		goto out;
	}
	/*	make 4nd 512 data fail to 0xffffffff		*/
	loc += (512/4 + 8);	//be sure the location is 4nd 512 data
	socle_nand_flsh_buffer[loc] = 0xffffffff;
	if (chip.program_page(0, (chip.ppblock + 4), chip.writesize, 0)){
		printf("\nSocle NAND Flash host: programming page #%d is failed\n", (chip.ppblock + 4));	
		err_flag |= -1;
		goto out;
	}	
	/*  write 1st block 0st page 2048~2111 bytes to 1st block 4nd page without ecc	*/
	if (chip.read_page(chip.writesize, chip.ppblock, (chip.oobfree_pos + chip.oobfree_len), 0)){
		printf("\nSocle NAND Flash host: reading page #%d cause a ecc error\n", chip.ppblock);
		err_flag |= -1;
		goto out;
	}
	if (chip.program_page(chip.writesize, (chip.ppblock + 4), (chip.oobfree_pos + chip.oobfree_len), 0)){
		printf("\nSocle NAND Flash host: programming page #%d is failed\n", (chip.ppblock + 4));	
		err_flag |= -1;
		goto out;
	}
	status = chip.read_page(0, (chip.ppblock + 4), chip.writesize, 1);	
	printf("\nSocle NAND Flash host: status = 0x%08x\n", status);
	if (SOCLE_NAND_FLSH_SYN_ERR_LOC_4 != (status & (SOCLE_NAND_FLSH_SYN_ERR_MASK))){
		err_flag |= -1;
		printf("test for 4nd syndrome_bit error\n");
		goto out;
	}	
	printf("test for 4nd syndrome_bit pass\n");
	
#endif

out:
	/* De-select the chip */
	socle_nand_flsh_select_chip(-1);

	return err_flag;
}

extern int 
socle_nand_flsh_swdma_burst_type_single(int autotest)
{
	socle_nand_flsh_dma_burst_type = SOCLE_DMA_BURST_SINGLE;
	return socle_nand_flsh_sw_dma_transfer(autotest);
}

extern int 
socle_nand_flsh_swdma_burst_type_incr4(int autotest)
{
	socle_nand_flsh_dma_burst_type = SOCLE_DMA_BURST_INCR4;
	return socle_nand_flsh_sw_dma_transfer(autotest);
}

extern int 
socle_nand_flsh_swdma_burst_type_incr8(int autotest)
{
	socle_nand_flsh_dma_burst_type = SOCLE_DMA_BURST_INCR8;
	return socle_nand_flsh_sw_dma_transfer(autotest);
}

extern int 
socle_nand_flsh_swdma_burst_type_incr16(int autotest)
{
	socle_nand_flsh_dma_burst_type = SOCLE_DMA_BURST_INCR16;
	return socle_nand_flsh_sw_dma_transfer(autotest);
}

static void socle_nand_flsh_dma_complete(void *data)
{
	socle_nand_flsh_dma_complete_flag = 1;
}

static int
socle_nand_flsh_sw_dma_transfer(int autotest)
{
	int i;
	int err_flag = 0;

	/* Select the chip */
	socle_nand_flsh_select_chip(0);

	/* Erase the 1th block for following test */
	if (chip.erase_block(1*chip.ppblock)) {
		err_flag |= -1;
		printf("\nSocle NAND Flash host: erasing 1th block is failed\n");	
		goto out;
	}

	/* Loop until the total pages of a block have been programmed */
	for (i = 0; i < chip.ppblock; i++) {
		socle_nand_flsh_fill_testpattern(socle_nand_flsh_pattern_addr, i*chip.writesize, chip.writesize);		

		/* set dma */
		socle_disable_dma(socle_nand_flsh_dma_channel_num);
		socle_set_dma_mode(socle_nand_flsh_dma_channel_num, SOCLE_DMA_MODE_SW);
		socle_set_dma_source_address(socle_nand_flsh_dma_channel_num, (u32)socle_nand_flsh_pattern_addr);
		socle_set_dma_destination_address(socle_nand_flsh_dma_channel_num, (u32)socle_nand_flsh_buffer);
		socle_set_dma_source_direction(socle_nand_flsh_dma_channel_num, SOCLE_DMA_DIR_INCR);
		socle_set_dma_destination_direction(socle_nand_flsh_dma_channel_num, SOCLE_DMA_DIR_INCR);
		socle_set_dma_burst_type(socle_nand_flsh_dma_channel_num, socle_nand_flsh_dma_burst_type);
		socle_set_dma_data_size(socle_nand_flsh_dma_channel_num, SOCLE_DMA_DATA_WORD);
		socle_set_dma_transfer_count(socle_nand_flsh_dma_channel_num, chip.writesize);
		socle_nand_flsh_dma_complete_flag = 0;
		socle_enable_dma(socle_nand_flsh_dma_channel_num);
	  
		/* Wait for dma to be complete */
		if (socle_wait_for_int(&socle_nand_flsh_dma_complete_flag, 30)) {
			printf("\nSocle NAND Flsh host: software dma transmission is timeout\n");
			return -1;
		}
			
		if (chip.program_page(0, (i+chip.ppblock), chip.writesize, 1)) {
			printf("\nSocle NAND Flash host: programming page #%d is failed\n", (i+chip.ppblock));	
			err_flag |= -1;
			goto out;
		}
	}

out:
	/* De-select the chip */
	socle_nand_flsh_select_chip(-1);

	if (!err_flag)
		/* Verify all pages that have been written using hdma */
		return socle_nand_flsh_sw_dma_verify_pages(&chip, chip.ppblock, chip.ppblock,
							   autotest);
		
	else
		return err_flag;
}

static int 
socle_nand_flsh_sw_dma_verify_pages(struct nand_chip *chip, u32 startpage, u32 numpage,
				    int autotest)
{
	u32 i; 
	int err_flag = 0;

	/* Select the chip */
	socle_nand_flsh_select_chip(0);

	for (i = 0; i < chip->ppblock; i++) {
		socle_nand_flsh_fill_testpattern(socle_nand_flsh_pattern_addr, i*chip->writesize, chip->writesize);		
		socle_nand_flsh_clear_buffer();
		if (chip->read_page(0, startpage+i, chip->writesize, 1)) {
			printf("\nSocle NAND Flash host: reading %d page cause a ecc error\n");
			err_flag |= -1;
		}

		/* set dma */
		socle_disable_dma(socle_nand_flsh_dma_channel_num);
		socle_set_dma_mode(socle_nand_flsh_dma_channel_num, SOCLE_DMA_MODE_SW);
		socle_set_dma_source_address(socle_nand_flsh_dma_channel_num, (u32)socle_nand_flsh_buffer);
		socle_set_dma_destination_address(socle_nand_flsh_dma_channel_num, (u32)socle_nand_flsh_comare_addr);
		socle_set_dma_source_direction(socle_nand_flsh_dma_channel_num, SOCLE_DMA_DIR_INCR);
		socle_set_dma_destination_direction(socle_nand_flsh_dma_channel_num, SOCLE_DMA_DIR_INCR);
		socle_set_dma_burst_type(socle_nand_flsh_dma_channel_num, socle_nand_flsh_dma_burst_type);
		socle_set_dma_data_size(socle_nand_flsh_dma_channel_num, SOCLE_DMA_DATA_WORD);
		socle_set_dma_transfer_count(socle_nand_flsh_dma_channel_num, chip->writesize);
		socle_nand_flsh_dma_complete_flag = 0;
		socle_enable_dma(socle_nand_flsh_dma_channel_num);

		/* Wait for dma to be complete */
		if (socle_wait_for_int(&socle_nand_flsh_dma_complete_flag, 30)) {
			printf("\nSocle NAND Flash host: software dma transmisson is timeout\n");
			return -1;
		}

		if (socle_nand_flsh_compare_memory((u32 *)socle_nand_flsh_pattern_addr, (u32 *)socle_nand_flsh_comare_addr, chip->writesize, autotest)) {
			err_flag |= 1;
			printf("\nSocle NAND Flash host: comparing page #%d......fail\n", (i+chip->ppblock));
		} else{
			if(!autotest)
				printf("\nSocle NAND Flash host: comparing page #%d......pass\n", (i+chip->ppblock));
		}
		if (err_flag)
			goto out;
	}

out:
	/* De-select the chip */
	socle_nand_flsh_select_chip(-1);

	return err_flag;
}

#ifdef CONFIG_CACHE
extern int
socle_nand_flsh_transfer_cache(int autotest)
{
	/* Enable the Cache Controller */
	set_cache_enable();  

	socle_nand_flsh_fill_testpattern(socle_nand_flsh_pattern_addr, 0, 2048);
	socle_nand_flsh_buf_transfer(socle_nand_flsh_pattern_addr, 2048, 1);
	printf("\nSocle NAND Flash host: comparing...");
	if (socle_nand_flsh_compare_memory((u32 *)socle_nand_flsh_pattern_addr, (u32 *)socle_nand_flsh_buffer, 2048,
					   autotest)) {
		printf("...fail\n");
		return -1;
	}
	printf("...pass\n");

	set_cache_disable();
	
	return 0;
}
#endif

/*	20080321 leonid+ for nand ecc correct test	*/
extern int
socle_nand_flsh_ecc_correct_test(int autotest)
{	
	int err_flag = 0;
	u32 status, loc;
	int (*socle_correct_data)(int s, u32 p);
	int i;

#ifdef CONFIG_PC9220
	if(autotest == 1)
		i = 1;
	else{
		printf("0. work around corretc function\n");
		printf("1. normal corretc function\n");
		printf("nand>");
		scanf("%d\n", &i);
	}
#else
	i = 0;
#endif

	if(i == 0)
		socle_correct_data = socle_nand_flsh_correct_data;
	else		
		socle_correct_data = socle_nand_flsh_correct_data2;

	if (!ecc_init_flag) {
		initialize_ecc();
		ecc_init_flag = 1;
	}
	
	/* Select the chip */
	socle_nand_flsh_select_chip(0);
	
	/* Erase the 1th block for following test */
	if (chip.erase_block(1*chip.ppblock)) {	
		err_flag |= -1;
		printf("\nSocle NAND Flash host: erasing 1th block is failed\n");	
		goto out;
	}
	
	/* Loop until the total pages of a block have been programmed */
	socle_nand_flsh_fill_testpattern(socle_nand_flsh_pattern_addr,  chip.writesize, chip.writesize);		
	socle_nand_flsh_buf_transfer(socle_nand_flsh_pattern_addr, chip.writesize, 1); 
	if (chip.program_page(0, chip.ppblock, chip.writesize, 1)) {	
		printf("\nSocle NAND Flash host: programming page #%d is failed\n", chip.ppblock);	
		err_flag |= -1;
		goto out;
	}

	/*	test for 1st sync bit		*/		
	/*  write 1st block 0st page 0~2047 bytes to 1st block 1st page without ecc	*/
	if (chip.read_page(0, chip.ppblock, chip.writesize, 0)){
		printf("\nSocle NAND Flash host: reading page #%d cause a ecc error\n", chip.ppblock);
		err_flag |= -1;
		goto out;
	}
	/*	make 1st 512 data fail to 0xffffffff		*/
	loc = 0;
	socle_nand_flsh_buffer[loc] = socle_nand_flsh_buffer[loc] + 1;
	if (chip.program_page(0, (chip.ppblock + 1), chip.writesize, 0)){
		printf("\nSocle NAND Flash host: programming page #%d is failed\n", (chip.ppblock + 1));	
		err_flag |= -1;
		goto out;
	}	
	/*  write 1st block 0st page 2048~2111 bytes to 1st block 1st page without ecc	*/
	if (chip.read_page(chip.writesize, chip.ppblock, (chip.oobfree_pos + chip.oobfree_len), 0)){
		printf("\nSocle NAND Flash host: reading page #%d cause a ecc error\n", chip.ppblock);
		err_flag |= -1;
		goto out;
	}
	if (chip.program_page(chip.writesize, (chip.ppblock + 1), (chip.oobfree_pos + chip.oobfree_len), 0)){
		printf("\nSocle NAND Flash host: programming page #%d is failed\n", (chip.ppblock + 1));	
		err_flag |= -1;
		goto out;
	}
	status = chip.read_page(0, (chip.ppblock + 1), chip.writesize, 1);	
	printf("\nSocle NAND Flash host: status = 0x%08x\n", status);
	
	if (socle_correct_data(status, (chip.ppblock + 1))){		
		printf("test for 1st syndrome_bit correct fail\n");
		err_flag |= -1;
		goto out;
	}else{
		if (socle_nand_flsh_compare_memory((u32 *)socle_nand_flsh_pattern_addr, (u32 *)socle_nand_flsh_buffer, 2048,
					   autotest)){					   
			printf("test for 1st syndrome_bit compare data fail\n");
			err_flag |= -1;
		}else					   
			printf("test for 1st syndrome_bit pass\n");			
	}
	
	/*	test for 2nd sync bit		*/		
	/*  write 1st block 0st page 0~2047 bytes to 1st block 2nd page without ecc	*/
	if (chip.read_page(0, chip.ppblock, chip.writesize, 0)){
		printf("\nSocle NAND Flash host: reading page #%d cause a ecc error\n", chip.ppblock);
		err_flag |= -1;
		goto out;
	}
	/*	make 2nd 512 data fail to 0xffffffff		*/
	loc += (512/4 + 8);	//be sure the location is 2nd 512 data
	socle_nand_flsh_buffer[loc] = socle_nand_flsh_buffer[loc] + 1;
	if (chip.program_page(0, (chip.ppblock + 2), chip.writesize, 0)){
		printf("\nSocle NAND Flash host: programming page #%d is failed\n", (chip.ppblock + 2));	
		err_flag |= -1;
		goto out;
	}	
	/*  write 1st block 0st page 2048~2111 bytes to 1st block 2nd page without ecc	*/
	if (chip.read_page(chip.writesize, chip.ppblock, (chip.oobfree_pos + chip.oobfree_len), 0)){
		printf("\nSocle NAND Flash host: reading page #%d cause a ecc error\n", chip.ppblock);
		err_flag |= -1;
		goto out;
	}
	if (chip.program_page(chip.writesize, (chip.ppblock + 2), (chip.oobfree_pos + chip.oobfree_len), 0)){
		printf("\nSocle NAND Flash host: programming page #%d is failed\n", (chip.ppblock + 2));	
		err_flag |= -1;
		goto out;
	}
	status = chip.read_page(0, (chip.ppblock + 2), chip.writesize, 1);	
	printf("\nSocle NAND Flash host: status = 0x%08x\n", status);
	
	if (socle_correct_data(status, (chip.ppblock + 2))){		
		printf("test for 2nd syndrome_bit fail\n");
		err_flag |= -1;
		goto out;
	}else{
		if (socle_nand_flsh_compare_memory((u32 *)socle_nand_flsh_pattern_addr, (u32 *)socle_nand_flsh_buffer, 2048,
					   autotest)){					   
			printf("test for 2nd syndrome_bit compare data fail\n");
			err_flag |= -1;
		}else					   
			printf("test for 2nd syndrome_bit pass\n");			
	}
	
	
	/*	test for 3nd sync bit		*/		
	/*  write 1st block 0st page 0~2047 bytes to 1st block 3nd page without ecc	*/
	if (chip.read_page(0, chip.ppblock, chip.writesize, 0)){
		printf("\nSocle NAND Flash host: reading page #%d cause a ecc error\n", chip.ppblock);
		err_flag |= -1;
		goto out;
	}
	/*	make 3nd 512 data fail to 0xffffffff		*/
	loc += (512/4 + 8);	//be sure the location is 3nd 512 data
	socle_nand_flsh_buffer[loc] = socle_nand_flsh_buffer[loc] + 1;
	if (chip.program_page(0, (chip.ppblock + 3), chip.writesize, 0)){
		printf("\nSocle NAND Flash host: programming page #%d is failed\n", (chip.ppblock + 3));	
		err_flag |= -1;
		goto out;
	}	
	/*  write 1st block 0st page 2048~2111 bytes to 1st block 3nd page without ecc	*/
	if (chip.read_page(chip.writesize, chip.ppblock, (chip.oobfree_pos + chip.oobfree_len), 0)){
		printf("\nSocle NAND Flash host: reading page #%d cause a ecc error\n", chip.ppblock);
		err_flag |= -1;
		goto out;
	}
	if (chip.program_page(chip.writesize, (chip.ppblock + 3), (chip.oobfree_pos + chip.oobfree_len), 0)){
		printf("\nSocle NAND Flash host: programming page #%d is failed\n", (chip.ppblock + 3));	
		err_flag |= -1;
		goto out;
	}
	status = chip.read_page(0, (chip.ppblock + 3), chip.writesize, 1);	
	printf("\nSocle NAND Flash host: status = 0x%08x\n", status);
	
	if (socle_correct_data(status, (chip.ppblock + 3))){		
		printf("test for 3nd syndrome_bit fail\n");
		err_flag |= -1;
		goto out;
	}else{
		if (socle_nand_flsh_compare_memory((u32 *)socle_nand_flsh_pattern_addr, (u32 *)socle_nand_flsh_buffer, 2048,
					   autotest)){					   
			printf("test for 3nd syndrome_bit compare data fail\n");
			err_flag |= -1;
		}else					   
			printf("test for 3nd syndrome_bit pass\n");			
	}
		
	
	/*	test for 4nd sync bit		*/		
	/*  write 1st block 0st page 0~2047 bytes to 1st block 4nd page without ecc	*/
	if (chip.read_page(0, chip.ppblock, chip.writesize, 0)){
		printf("\nSocle NAND Flash host: reading page #%d cause a ecc error\n", chip.ppblock);
		err_flag |= -1;
		goto out;
	}
	/*	make 4nd 512 data fail to 0xffffffff		*/
	loc += (512/4 + 8);	//be sure the location is 4nd 512 data
	socle_nand_flsh_buffer[loc] = socle_nand_flsh_buffer[loc] + 1;
	if (chip.program_page(0, (chip.ppblock + 4), chip.writesize, 0)){
		printf("\nSocle NAND Flash host: programming page #%d is failed\n", (chip.ppblock + 4));	
		err_flag |= -1;
		goto out;
	}	
	/*  write 1st block 0st page 2048~2111 bytes to 1st block 4nd page without ecc	*/
	if (chip.read_page(chip.writesize, chip.ppblock, (chip.oobfree_pos + chip.oobfree_len), 0)){
		printf("\nSocle NAND Flash host: reading page #%d cause a ecc error\n", chip.ppblock);
		err_flag |= -1;
		goto out;
	}
	if (chip.program_page(chip.writesize, (chip.ppblock + 4), (chip.oobfree_pos + chip.oobfree_len), 0)){
		printf("\nSocle NAND Flash host: programming page #%d is failed\n", (chip.ppblock + 4));	
		err_flag |= -1;
		goto out;
	}
	status = chip.read_page(0, (chip.ppblock + 4), chip.writesize, 1);	
	printf("\nSocle NAND Flash host: status = 0x%08x\n", status);
	
	if (socle_correct_data(status, (chip.ppblock + 4))){		
		printf("test for 4nd syndrome_bit fail\n");
		err_flag |= -1;
		goto out;
	}else{
		if (socle_nand_flsh_compare_memory((u32 *)socle_nand_flsh_pattern_addr, (u32 *)socle_nand_flsh_buffer, 2048,
					   autotest)){					   
			printf("test for 4nd syndrome_bit compare data fail\n");
			err_flag |= -1;
		}else					   
			printf("test for 4nd syndrome_bit pass\n");			
	}
		
	
out:
	/* De-select the chip */
	socle_nand_flsh_select_chip(-1);

	return err_flag;
	
}

/*20080321 leonid+ for workaround correct data function*/
static int 
socle_nand_flsh_correct_data(int status, u32 page)
{
	u8 *codeword = (u8 *)socle_nand_flsh_ecc_correct_buffer;
	u32 tmp;	
	int   i, loc;

	//printf("socle_nand_flsh_correct_data\n");
	
	if(npar == 12)
		loc = 512 + 15;
	else		
		loc = 512 + 8;
	
	/* Check if it is a erased range */
	if ((SOCLE_NAND_FLSH_SYN_ERR_MASK == (status & SOCLE_NAND_FLSH_SYN_ERR_MASK)) ||
	    (512 == chip.writesize)) {
	    	for (i = 0; i < (chip.writesize >> 2); i++) {
			if (*((u32 *)socle_nand_flsh_buffer + i) != 0xffffffff)
				goto data_correct;
		}
		return 0;
	}

data_correct:
	
	/*	20080321 leonid+ for nand syndrome bug sw work around	*/
	
	if(2048 == chip.writesize){		
		/* Second 512 bytes */
		if(SOCLE_NAND_FLSH_SYN_ERR_LOC_1 == 
					(chip.read_page(loc, page, 512, 1) & SOCLE_NAND_FLSH_SYN_ERR_LOC_1)){
			socle_nand_flsh_buf_transfer(socle_nand_flsh_ecc_correct_buffer, 512, 0);			
			tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_SYNDR_1, socle_nand_flsh_base);
			synBytes[0] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
			synBytes[1] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
			synBytes[2] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
			tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_SYNDR_2, socle_nand_flsh_base);
			synBytes[3] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
			synBytes[4] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
			synBytes[5] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
			if (npar == 12){	
				tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_SYNDR_3, socle_nand_flsh_base);
				synBytes[6] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
				synBytes[7] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
				synBytes[8] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
				tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_SYNDR_4, socle_nand_flsh_base);
				synBytes[9] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
				synBytes[10] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
				synBytes[11] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
			}
			if (correct_errors_hw_ecc(codeword, ml)) {
				printf("\nsocle_nand_host: data can not be corrected between 512 and 1023 bytes in page %d\n", page);
				return -1;
			} else 
				printf("\nsocle_nand_host: data is corrected between 512 and 1023 bytes in page %d\n", page);
			
			socle_nand_flsh_buf_transfer2(socle_nand_flsh_ecc_correct_buffer, 512, 1, 512);
		}
		/* Third 512 bytes */
		if(SOCLE_NAND_FLSH_SYN_ERR_LOC_1 == 
					(chip.read_page(loc*2, page, 512, 1) & SOCLE_NAND_FLSH_SYN_ERR_LOC_1)){
			socle_nand_flsh_buf_transfer(socle_nand_flsh_ecc_correct_buffer, 512, 0);			
			tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_SYNDR_1, socle_nand_flsh_base);
			synBytes[0] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
			synBytes[1] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
			synBytes[2] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
			tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_SYNDR_2, socle_nand_flsh_base);
			synBytes[3] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
			synBytes[4] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
			synBytes[5] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
			if (npar == 12){	
				tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_SYNDR_3, socle_nand_flsh_base);
				synBytes[6] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
				synBytes[7] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
				synBytes[8] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
				tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_SYNDR_4, socle_nand_flsh_base);
				synBytes[9] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
				synBytes[10] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
				synBytes[11] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
			}
			if (correct_errors_hw_ecc(codeword, ml)) {
				printf("\nsocle_nand_host: data can not be corrected between 1024 and 1535 bytes in page %d\n", page);
				return -1;
			} else 
				printf("\nsocle_nand_host: data is corrected between 1024 and 1535 bytes in page %d\n", page);
			
			socle_nand_flsh_buf_transfer2(socle_nand_flsh_ecc_correct_buffer, 512, 1, 1024);	
		}
		/* Forth 512 bytes */
		if(SOCLE_NAND_FLSH_SYN_ERR_LOC_1 == 
					(chip.read_page(loc*3, page, 512, 1) & SOCLE_NAND_FLSH_SYN_ERR_LOC_1)){
			socle_nand_flsh_buf_transfer(socle_nand_flsh_ecc_correct_buffer, 512, 0);			
			tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_SYNDR_1, socle_nand_flsh_base);
			synBytes[0] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
			synBytes[1] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
			synBytes[2] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
			tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_SYNDR_2, socle_nand_flsh_base);
			synBytes[3] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
			synBytes[4] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
			synBytes[5] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
			if (npar == 12){	
				tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_SYNDR_3, socle_nand_flsh_base);
				synBytes[6] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
				synBytes[7] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
				synBytes[8] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
				tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_SYNDR_4, socle_nand_flsh_base);
				synBytes[9] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
				synBytes[10] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
				synBytes[11] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
			}
			if (correct_errors_hw_ecc(codeword, ml)) {
				printf("\nsocle_nand_host: data can not be corrected between 1536 and 2047 bytes in page %d\n", page);
				return -1;
			} else 
				printf("\nsocle_nand_host: data is corrected between 1536 and 2047 bytes in page %d\n", page);
			
			socle_nand_flsh_buf_transfer2(socle_nand_flsh_ecc_correct_buffer, 512, 1, 1536);
		}
	}
	
	/* First 512 bytes */
	if(SOCLE_NAND_FLSH_SYN_ERR_LOC_1 == 
				(chip.read_page(0, page, 512, 1) & SOCLE_NAND_FLSH_SYN_ERR_LOC_1)){
		socle_nand_flsh_buf_transfer(socle_nand_flsh_ecc_correct_buffer, 512, 0);			
		tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_SYNDR_1, socle_nand_flsh_base);
		synBytes[0] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
		synBytes[1] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
		synBytes[2] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
		tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_SYNDR_2, socle_nand_flsh_base);
		synBytes[3] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
		synBytes[4] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
		synBytes[5] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
		if (npar == 12){	
			tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_SYNDR_3, socle_nand_flsh_base);
			synBytes[6] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
			synBytes[7] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
			synBytes[8] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
			tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_SYNDR_4, socle_nand_flsh_base);
			synBytes[9] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
			synBytes[10] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
			synBytes[11] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
		}
		if (correct_errors_hw_ecc(codeword, ml)) {
			printf("\nsocle_nand_host: data can not be corrected between 0 and 511 bytes in page %d\n", page);
			return -1;
		} else 
			printf("\nsocle_nand_host: data is corrected between 0 and 511 bytes in page %d\n", page);
			
			socle_nand_flsh_buf_transfer2(socle_nand_flsh_ecc_correct_buffer, 512, 1, 0);	
	}
	
	return 0;

}

	
/*20080321 leonid+ for normal correct data function */
static int 
socle_nand_flsh_correct_data2(int status, u32 page)
{
	u8 *codeword = (u8 *)socle_nand_flsh_ecc_correct_buffer;
	u32 tmp;	
	int   i;


	printf("socle_nand_flsh_correct_data\n");
	
	/* Check if it is a erased range */
	if ((SOCLE_NAND_FLSH_SYN_ERR_MASK == (status & SOCLE_NAND_FLSH_SYN_ERR_MASK)) ||
	    (512 == chip.writesize)) {
	    	for (i = 0; i < (chip.writesize >> 2); i++) {
			if (*((u32 *)socle_nand_flsh_buffer + i) != 0xffffffff)
				goto data_correct;
		}
		return 0;
	}

data_correct:
		
	if(2048 == chip.writesize){
		/* Second 512 bytes */	
		if(SOCLE_NAND_FLSH_SYN_ERR_LOC_2 == (status & SOCLE_NAND_FLSH_SYN_ERR_LOC_2)){
			socle_nand_flsh_buf_transfer2(socle_nand_flsh_ecc_correct_buffer, 512, 0, 512);			
			tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_2ND_SYNDR_1, socle_nand_flsh_base);
			synBytes[0] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
			synBytes[1] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
			synBytes[2] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
			tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_2ND_SYNDR_2, socle_nand_flsh_base);
			synBytes[3] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
			synBytes[4] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
			synBytes[5] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
			if (npar == 12){	
				tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_2ND_SYNDR_3, socle_nand_flsh_base);
				synBytes[6] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
				synBytes[7] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
				synBytes[8] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
				tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_2ND_SYNDR_4, socle_nand_flsh_base);
				synBytes[9] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
				synBytes[10] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
				synBytes[11] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
			}
			if (correct_errors_hw_ecc(codeword, ml)) {
				printf("\nsocle_nand_host: data can not be corrected between 512 and 1023bytes in page %d\n", page);
				return -1;
			} else 
				printf("\nsocle_nand_host: data is corrected between 512 and 1023bytes bytes in page %d\n", page);
			
			socle_nand_flsh_buf_transfer2(socle_nand_flsh_ecc_correct_buffer, 512, 1, 512);
		}
		/* Third 512 bytes */
		if(SOCLE_NAND_FLSH_SYN_ERR_LOC_3 == (status & SOCLE_NAND_FLSH_SYN_ERR_LOC_3)){
			socle_nand_flsh_buf_transfer2(socle_nand_flsh_ecc_correct_buffer, 512, 0, 1024);			
			tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_3RD_SYNDR_1, socle_nand_flsh_base);
			synBytes[0] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
			synBytes[1] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
			synBytes[2] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
			tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_3RD_SYNDR_2, socle_nand_flsh_base);
			synBytes[3] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
			synBytes[4] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
			synBytes[5] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
			if (npar == 12){	
				tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_3RD_SYNDR_3, socle_nand_flsh_base);
				synBytes[6] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
				synBytes[7] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
				synBytes[8] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
				tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_3RD_SYNDR_4, socle_nand_flsh_base);
				synBytes[9] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
				synBytes[10] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
				synBytes[11] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
			}
			if (correct_errors_hw_ecc(codeword, ml)) {
				printf("\nsocle_nand_host: data can not be corrected between 1024 and 1535 bytes in page %d\n", page);
				return -1;
			} else 
				printf("\nsocle_nand_host: data is corrected between 1024 and 1535 bytes in page %d\n", page);
			
			socle_nand_flsh_buf_transfer2(socle_nand_flsh_ecc_correct_buffer, 512, 1, 1024);	
		}
		/* Forth 512 bytes */
		if(SOCLE_NAND_FLSH_SYN_ERR_LOC_4 == (status & SOCLE_NAND_FLSH_SYN_ERR_LOC_4)){
			socle_nand_flsh_buf_transfer2(socle_nand_flsh_ecc_correct_buffer, 512, 0, 1536);			
			tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_4TH_SYNDR_1, socle_nand_flsh_base);
			synBytes[0] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
			synBytes[1] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
			synBytes[2] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
			tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_4TH_SYNDR_2, socle_nand_flsh_base);
			synBytes[3] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
			synBytes[4] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
			synBytes[5] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
			if (npar == 12){	
				tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_4TH_SYNDR_3, socle_nand_flsh_base);
				synBytes[6] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
				synBytes[7] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
				synBytes[8] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
				tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_4TH_SYNDR_4, socle_nand_flsh_base);
				synBytes[9] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
				synBytes[10] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
				synBytes[11] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
			}
			if (correct_errors_hw_ecc(codeword, ml)) {
				printf("\nsocle_nand_host: data can not be corrected between 1536 and 2047 bytes in page %d\n", page);
				return -1;
			} else 
				printf("\nsocle_nand_host: data is corrected between 1536 and 2047 bytes in page %d\n", page);
			
			socle_nand_flsh_buf_transfer2(socle_nand_flsh_ecc_correct_buffer, 512, 1, 1536);
		}
	}
	
	/* First 512 bytes */
		if(SOCLE_NAND_FLSH_SYN_ERR_LOC_1 == (status & SOCLE_NAND_FLSH_SYN_ERR_LOC_1)){
		socle_nand_flsh_buf_transfer2(socle_nand_flsh_ecc_correct_buffer, 512, 0, 0);			
		tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_SYNDR_1, socle_nand_flsh_base);
		synBytes[0] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
		synBytes[1] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
		synBytes[2] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
		tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_SYNDR_2, socle_nand_flsh_base);
		synBytes[3] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
		synBytes[4] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
		synBytes[5] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
		if (npar == 12){	
			tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_SYNDR_3, socle_nand_flsh_base);
			synBytes[6] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
			synBytes[7] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
			synBytes[8] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
			tmp = socle_nand_flsh_read(SOCLE_NAND_FLSH_1ST_SYNDR_4, socle_nand_flsh_base);
			synBytes[9] = SOCLE_NAND_FLSH_SYN_CODE_1(tmp);
			synBytes[10] = SOCLE_NAND_FLSH_SYN_CODE_2(tmp);
			synBytes[11] = SOCLE_NAND_FLSH_SYN_CODE_3(tmp);
		}
		if (correct_errors_hw_ecc(codeword, ml)) {
			printf("\nsocle_nand_host: data can not be corrected between 0 and 511 bytes in page %d\n", page);
			return -1;
		} else 
			printf("\nsocle_nand_host: data is corrected between 0 and 511 bytes in page %d\n", page);
			
			socle_nand_flsh_buf_transfer2(socle_nand_flsh_ecc_correct_buffer, 512, 1, 0);	
	}
	
	return 0;

}



extern void 
socle_nand_flsh_fill_testpattern(u8 *pattern_addr, u32 base, u32 len)
{
	u32 i;

	for (i = 0; i < len; i++)
		pattern_addr[i] = (u8)((base + i) & 0xff);
}

extern void 
socle_nand_flsh_buf_transfer(u8 *buf, u32 len, int writing)	 
{
	u32 i;
	u32 num_words = (len >> 2);
	u8 num_rem_bytes = len & (3);
	u8 rem_byte[4] = {0};

	/* Must use word access in writing data into nand buffer */     
	if (writing) {
		for (i = 0; i < num_words; i++)
			socle_nand_flsh_buffer[i] = *((u32 *)buf + i);
		if (num_rem_bytes) {
			for (i = 0; i < num_rem_bytes; i++)
				rem_byte[i] = buf[4*num_words+i];
			socle_nand_flsh_buffer[num_words] = *((u32 *)rem_byte);
		}
	} else {
		for (i = 0; i < num_words; i++)
			*((u32 *)buf + i) = socle_nand_flsh_buffer[i];
		if (num_rem_bytes) {
			*((u32 *)rem_byte) = socle_nand_flsh_buffer[num_words];
			for (i = 0; i < num_rem_bytes; i++)
				buf[4*num_words+i] = rem_byte[i];
		}
	}
}

extern void 
socle_nand_flsh_buf_transfer2(u8 *buf, u32 len, int writing, u32 offset)	 
{
	u32 i;
	u32 num_words = (len >> 2);
	u8 num_rem_bytes = len & (3);
	u8 rem_byte[4] = {0};
	u32 *nand_buffer = (u32 *)((u32)socle_nand_flsh_buffer + offset);

	/* Must use word access in writing data into nand buffer */     
	if (writing) {
		for (i = 0; i < num_words; i++)
			nand_buffer[i] = *((u32 *)buf + i);
		if (num_rem_bytes) {
			for (i = 0; i < num_rem_bytes; i++)
				rem_byte[i] = buf[4*num_words+i];
			nand_buffer[num_words] = *((u32 *)rem_byte);
		}
	} else {
		for (i = 0; i < num_words; i++)
			*((u32 *)buf + i) = nand_buffer[i];
		if (num_rem_bytes) {
			*((u32 *)rem_byte) = nand_buffer[num_words];
			for (i = 0; i < num_rem_bytes; i++)
				buf[4*num_words+i] = rem_byte[i];
		}
	}
}


extern void
socle_nand_flsh_select_chip(int chipnr)
{
	u32 data;

	data = socle_nand_flsh_read(SOCLE_NAND_FLSH_CE_WP, socle_nand_flsh_base);
	if (-1 == chipnr)
		/* Clear all NAND flash device chip enable */
		data &= ~(0xf);
	else
		data |= 1 << (chipnr & 0xf);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CE_WP, data, socle_nand_flsh_base);
}

extern int 
socle_nand_flsh_verify_pages(struct nand_chip *chip, u32 startpage, u32 numpages,
			     int skip_cmpr_result)
{
	u32 i; 
	int err_flag = 0;

	/* Select the chip */
	socle_nand_flsh_select_chip(0);

	for (i = 0; i < numpages; i++) {
		socle_nand_flsh_fill_testpattern(socle_nand_flsh_pattern_addr, i*chip->writesize, chip->writesize);		
		socle_nand_flsh_clear_buffer();
		if (chip->read_page(0, startpage+i, chip->writesize, 1)) {
			printf("\nSocle NAND Flash host: reading page #%d cause a ecc error\n", (startpage+i));
			err_flag |= -1;
		}
		if (socle_nand_flsh_verify_buffer(socle_nand_flsh_pattern_addr, chip->writesize,
						  skip_cmpr_result)) {
			err_flag |= -1;
			printf("\nSocle NAND Flash host: comparing page #%d......fail\n", (startpage+i));
		} else {
			if(!skip_cmpr_result)
				printf("Socle NAND Flash host: comparing page #%d......pass\n", (startpage+i));
		}
		if (err_flag)
			goto out;
	}

out:
	/* De-select the chip */
	socle_nand_flsh_select_chip(-1);

	return err_flag;
}

extern int 
socle_nand_flsh_verify_buffer(u8 *buf, u32 len, int skip_cmpr_result)
{
	u32 i;
	u32 num_words = (len >> 2);
	u8 num_rem_bytes = len & (3);
	u8 rem_byte[4] = {0};
	int err_flag = 0;
	u32 *buf32 = (u32 *)buf;

	for (i = 0; i < num_words; i++) {
		if (buf32[i] != socle_nand_flsh_buffer[i]) {
			err_flag |= -1;
			if (!skip_cmpr_result)
				printf("\nWord %d: RAM: 0x%08x, NAND: 0x%08x", i, buf32[i], socle_nand_flsh_buffer[i]);
		}
	}

	if (num_rem_bytes) {
		*((u32 *)rem_byte) = socle_nand_flsh_buffer[num_words];
		for (i = 0; i < num_rem_bytes; i++) {
			if (rem_byte[i] != buf[4*num_words+i]) {
				err_flag |= -1;
				if (!skip_cmpr_result)
					printf("\nByte %d: RAM: 0x%02x, NAND: 0x%02x", buf[4*num_words+i], rem_byte[i]);	       
			}
		}
	}
	return err_flag;
}

extern int
socle_nand_flsh_compare_memory(u32 *mem, u32 *mem_compare, u32 size,
			       int skip_cmpr_result) 
{
	int i;
	int err_flag = 0;

	for (i = 0; i < (size >> 2); i++) {
		if (mem[i] != mem_compare[i]) {
			err_flag |= -1;
			if (!skip_cmpr_result)
				printf("\nWord #%d 0x%08x != 0x%08x", i, mem[i], mem_compare[i]); 
		}
	}
	return err_flag;
}

extern void
socle_nand_flsh_clear_buffer(void)
{
	u32 i;

	for (i = 0; i < (2048 >> 2); i++)
		socle_nand_flsh_buffer[i] = 0xffffffff;
}

static void
socle_nand_flsh_mark_ecc_err(void)
{
	u32 data;
	u32 word_pos = 512 >> 2;

	/* Mark error on parity code on codeword*/
	data = codeword[512];
	data &= ~(0xff << 2);
	codeword[512] = data;
     
	/* Mark error on parity code on nand buffer */
	data = socle_nand_ecc_test_buffer[word_pos];
	data &= ~(0xff << 24);
	socle_nand_ecc_test_buffer[word_pos] = data;
}

static void
socle_nand_flsh_mark_err(u32 err_pos)
{
	u32 word_pos = err_pos >> 2;
	u32 byte_pos = (err_pos & 3) << 3;
	u32 data;

	/* Mark error on codeword */
	codeword[err_pos] = 0x0000;

	/* Mark error on nand buffer */
	data = socle_nand_ecc_test_buffer[word_pos];
	data &= ~(0xff << byte_pos);
	socle_nand_ecc_test_buffer[word_pos] = data;
}

static void
socle_nand_flsh_isr(void *data)
{
	socle_nand_flsh_int_flag = 1;
	socle_nand_flsh_int_stat = socle_nand_flsh_read(SOCLE_NAND_FLSH_INT_STATE, socle_nand_flsh_base);
}

static int
socle_nand_flsh_ecc_read(u32 page_addr)
{
	/* Reset the NAND Flash controller */
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_EN, socle_nand_flsh_base);
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_DIS, socle_nand_flsh_base);

	/* Clear all interrupt flags */
	//printf("INT_STATE : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_STATE, 0, socle_nand_flsh_base);

	/* Set the interrupt mask */
	//printf("INT_MASK : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_MASK,
			      ~(SOCLE_NAND_FLSH_MASK_READ_DATA_DONE |
				SOCLE_NAND_FLSH_MASK_SYN_BIT),
			      socle_nand_flsh_base);

	/* Config setting */
	//printf("FLSH_CONF : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONF,
			      SOCLE_NAND_FLSH_tWC_CYC_15 |
			      SOCLE_NAND_FLSH_tWP_CYC_7 |
			      SOCLE_NAND_FLSH_tRC_CYC_15 |
			      SOCLE_NAND_FLSH_tRP_CYC_7 |
			      SOCLE_NAND_FLSH_PAGE_2KB,
			      socle_nand_flsh_base);

	/* Command */
	//printf("COMM_1 : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_1,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_READ0),
			      socle_nand_flsh_base);
	//printf("COMM_2 : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_2,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_READSTART),
			      socle_nand_flsh_base);
	//printf("STATE_COMM : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_STATE_COMM,
			      SOCLE_NAND_FLSH_CMD_N_VALID,
			      socle_nand_flsh_base);

	/* Address */
	//printf("ADDRESS_1 : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_1,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(0),
			      socle_nand_flsh_base);
	//printf("ADDRESS_2 : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_2,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(0),
			      socle_nand_flsh_base);
	//printf("ADDRESS_3 : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_3,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr),
			      socle_nand_flsh_base);
	//printf("ADDRESS_4 : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_4,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr >> 8),
			      socle_nand_flsh_base);
	//printf("ADDRESS_5 : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_5,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr >> 16),
			      socle_nand_flsh_base);

	/* DMA setting */
	//printf("DMA_SET : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_DMA_SET,
			      SOCLE_NAND_FLSH_LITTLE_ENDING |
			      SOCLE_NAND_FLSH_ECC_TESTMODE_EN |
			      SOCLE_NAND_FLSH_BUF_RW_EN |
			      SOCLE_NAND_FLSH_SEC_CMD_Y |
			      SOCLE_NAND_FLSH_AUTO_STAT_CHK_DIS |
			      SOCLE_NAND_FLSH_ACCESS_DATA_INTER_BUF |
			      SOCLE_NAND_FLSH_DMA_READ |
			      SOCLE_NAND_FLSH_DEV_WAIT_RB,
			      socle_nand_flsh_base);

	/* Set the start address of buffer */
	//printf("BUFF_STADDR : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_STADDR,
			      SOCLE_NAND_FLSH_INTER_BUF_STR_ADDR(0),
			      socle_nand_flsh_base);

	/* Set buffer count and ecc mode */
	//printf("BUFF_CNT : ");
#ifdef CONFIG_NAND_TWO_ECC_MODEL
	if(npar == 6)
		socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
			      SOCLE_NAND_FLSH_RW_WITH_ECC | SOCLE_NAND_FLSH_NPAR_LEN_8B |
			      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(511),
			      socle_nand_flsh_base);
	else			
		socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
			      SOCLE_NAND_FLSH_RW_WITH_ECC | SOCLE_NAND_FLSH_NPAR_LEN_15B |
			      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(511),
			      socle_nand_flsh_base);
#else
		socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
			      SOCLE_NAND_FLSH_RW_WITH_ECC |
			      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(511),
			      socle_nand_flsh_base);
#endif

	/* Start operation */
	//printf("FLSH_CONTROL : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONTROL,
			      SOCLE_NAND_FLSH_DMA_EN |
			      SOCLE_NAND_FLSH_DATA_RW_EN |
			      SOCLE_NAND_FLSH_SEND_CMD_ADDR_EN,
			      socle_nand_flsh_base);

	socle_nand_flsh_int_flag = 0;
	socle_nand_flsh_int_stat = 0;
	if (socle_wait_for_int(&socle_nand_flsh_int_flag, 30)) {
		printf("Socle NAND Flash host: socle_nand_flsh_ecc_read() is timeout\n");
		return -1;
	} else {
		if (socle_nand_flsh_int_stat & SOCLE_NAND_FLSH_READ_DATA_DONE)
			return 0;
		else
			return -1;
	}
}

static int
socle_nand_flsh_ecc_program(u32 page_addr)
{
	/* Reset the NAND Flash controller */
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_EN, socle_nand_flsh_base);
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_DIS, socle_nand_flsh_base);

	/* Clear all interrupt flags */
	//printf("INT_STATE : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_STATE, 0, socle_nand_flsh_base);

	/* Set the interrupt mask */
	//printf("INT_MASK : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_MASK,
			      ~(SOCLE_NAND_FLSH_MASK_WRITE_DATA_DONE |
				SOCLE_NAND_FLSH_MASK_WRITE_DATA_ERR),
			      socle_nand_flsh_base);

	/* Config setting and chip enable */
	//printf("FLSH_CONF : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONF,
			      SOCLE_NAND_FLSH_tWC_CYC_15 |
			      SOCLE_NAND_FLSH_tWP_CYC_7 |
			      SOCLE_NAND_FLSH_tRC_CYC_15 |
			      SOCLE_NAND_FLSH_tRP_CYC_7 |
			      SOCLE_NAND_FLSH_PAGE_2KB,
			      socle_nand_flsh_base);

	/* Command */
	//printf("COMM_1 : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_1,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_SEQIN),
			      socle_nand_flsh_base);
	//printf("COMM_2 : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_2,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_PAGEPROG),
			      socle_nand_flsh_base);
	/*
	printf("STATE_COMM : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_STATE_COMM,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_STATUS),
			      socle_nand_flsh_base);
	*/
	/* Address */
	//printf("ADDRESS_1 : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_1,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(0),
			      socle_nand_flsh_base);
	//printf("ADDRESS_2 : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_2,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(0),
			      socle_nand_flsh_base);
	//printf("ADDRESS_3 : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_3,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr),
			      socle_nand_flsh_base);
	//printf("ADDRESS_4 : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_4,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr >> 8),
			      socle_nand_flsh_base);
	//printf("ADDRESS_5 : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_5,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr >> 16),
			      socle_nand_flsh_base);

	/* DMA setting */
	//printf("DMA_SET : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_DMA_SET,
			      SOCLE_NAND_FLSH_LITTLE_ENDING |
			      SOCLE_NAND_FLSH_ECC_TESTMODE_EN |
			      SOCLE_NAND_FLSH_BUF_RW_EN |
			      SOCLE_NAND_FLSH_SEC_CMD_Y |
			      SOCLE_NAND_FLSH_AUTO_STAT_CHK_DIS |
			      SOCLE_NAND_FLSH_ACCESS_DATA_INTER_BUF |
			      SOCLE_NAND_FLSH_DMA_WRITE |
			      SOCLE_NAND_FLSH_DEV_WAIT_RB,
			      socle_nand_flsh_base);

	/* Set the start address of buffer */
	//printf("BUFF_STADDR : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_STADDR,
			      SOCLE_NAND_FLSH_INTER_BUF_STR_ADDR(0),
			      socle_nand_flsh_base);

	/* Set buffer count and ecc mode */
	//printf("BUFF_CNT : ");
#ifdef CONFIG_NAND_TWO_ECC_MODEL
	if(npar == 6)
		socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
			      SOCLE_NAND_FLSH_RW_WITH_ECC | SOCLE_NAND_FLSH_NPAR_LEN_8B |
			      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(511),
			      socle_nand_flsh_base);
	else			
		socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
			      SOCLE_NAND_FLSH_RW_WITH_ECC | SOCLE_NAND_FLSH_NPAR_LEN_15B |
			      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(511),
			      socle_nand_flsh_base);
#else
		socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
			      SOCLE_NAND_FLSH_RW_WITH_ECC |
			      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(511),
			      socle_nand_flsh_base);
#endif

	/* Start operation */
	//printf("FLSH_CONTROL : ");
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONTROL,
			      SOCLE_NAND_FLSH_DMA_EN |
			      SOCLE_NAND_FLSH_DATA_RW_EN |
			      SOCLE_NAND_FLSH_SEND_CMD_ADDR_EN,
			      socle_nand_flsh_base);
	
	socle_nand_flsh_int_flag = 0;
	socle_nand_flsh_int_stat = 0;
	if (socle_wait_for_int(&socle_nand_flsh_int_flag, 30)) {
		printf("Socle NAND Flash host: socle_nand_flsh_ecc_program() is timeout\n");
		return -1;
	} else {
		if (socle_nand_flsh_int_stat & SOCLE_NAND_FLSH_WRITE_DATA_DONE)
			return 0;
		else
			return -1;
	}
}


extern void
socle_nand_flsh_setup_chip(int is_large_page)
{
	/* Setup the chip */
	if (is_large_page) {
		chip.writesize = 2048;
		chip.ppblock = 64;
		chip.oobfree_pos = 60;
		chip.oobfree_len = 4;
		chip.erase_block = socle_nand_flsh_erase_block;
		chip.read_page = socle_nand_flsh_read_page_l;
		chip.program_page = socle_nand_flsh_program_page_l;
		chip.cache_program_page = socle_nand_flsh_cache_program_page_l;
		chip.copyback_page = socle_nand_flsh_copyback_page_l;
	} else {
		chip.writesize = 512;
		chip.ppblock = 32;
		chip.oobfree_pos = 15;
		chip.oobfree_len = 1;
		chip.erase_block = socle_nand_flsh_erase_block;
		chip.read_page = socle_nand_flsh_read_page_s;
		chip.program_page = socle_nand_flsh_program_page_s;
		chip.cache_program_page = NULL;
		chip.copyback_page = socle_nand_flsh_copyback_page_s;
	}
}

static int 
socle_nand_flsh_erase_block(u32 page_addr)
{
	/* Reset the NAND Flash controller */
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_EN, socle_nand_flsh_base);
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_DIS, socle_nand_flsh_base);

	/* Clear all interrupt flags */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_STATE, 0, socle_nand_flsh_base);

	/* Set the interrupt mask */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_MASK,
			      ~(SOCLE_NAND_FLSH_MASK_BLK_ERASE_DONE |
				SOCLE_NAND_FLSH_MASK_BLK_ERASE_FAIL),
			      socle_nand_flsh_base);

	/* Config setting */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONF,
			      SOCLE_NAND_FLSH_tWC_CYC_15 |
			      SOCLE_NAND_FLSH_tWP_CYC_7 |
			      SOCLE_NAND_FLSH_tRC_CYC_15 |
			      SOCLE_NAND_FLSH_tRP_CYC_7 |
			      SOCLE_NAND_FLSH_PAGE_512B,
			      socle_nand_flsh_base);

	/* Command */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_1,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_ERASE1),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_2,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_ERASE2),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_STATE_COMM,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_STATUS),
			      socle_nand_flsh_base);

	/* Address */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_1,
			      SOCLE_NAND_FLSH_ADDR_N_VALID,
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_2,
			      SOCLE_NAND_FLSH_ADDR_VALID,
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_3,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_4,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr >> 8),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_5,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr >> 16),
			      socle_nand_flsh_base);

	/* DMA setting */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_DMA_SET,
			      SOCLE_NAND_FLSH_LITTLE_ENDING |
			      SOCLE_NAND_FLSH_ECC_TESTMODE_DIS |
			      SOCLE_NAND_FLSH_BUF_RW_DIS |
			      SOCLE_NAND_FLSH_SEC_CMD_Y |
			      SOCLE_NAND_FLSH_AUTO_STAT_CHK_EN |
			      SOCLE_NAND_FLSH_ACCESS_DATA_INTER_BUF |
			      SOCLE_NAND_FLSH_DMA_NOP |
			      SOCLE_NAND_FLSH_DEV_WAIT_RB,
			      socle_nand_flsh_base);

	socle_nand_flsh_int_flag = 0;
	socle_nand_flsh_int_stat = 0;

	/* Start operation */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONTROL,
			      SOCLE_NAND_FLSH_DMA_EN |
			      SOCLE_NAND_FLSH_DATA_RW_DIS |
			      SOCLE_NAND_FLSH_SEND_CMD_ADDR_EN,
			      socle_nand_flsh_base);

	if (socle_wait_for_int(&socle_nand_flsh_int_flag, 30)) {
		printf("Socle NAND Flash host: socle_nand_flsh_erase_block() is timeout\n");
		return -1;
	} else {
		if (socle_nand_flsh_int_stat & SOCLE_NAND_FLSH_BLK_ERASE_DONE)
			return 0;
		else
			return -1;
	}
}

static int
socle_nand_flsh_read_page_l(u32 column, u32 page_addr, u32 len, int ecc)
{
	/* Reset the NAND Flash controller */
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_EN, socle_nand_flsh_base);
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_DIS, socle_nand_flsh_base);

	/* Clear all interrupt flags */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_STATE, 0, socle_nand_flsh_base);

	/* Set the interrupt mask */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_MASK,
			      ~(SOCLE_NAND_FLSH_MASK_READ_DATA_DONE |
				SOCLE_NAND_FLSH_MASK_SYN_BIT),
			      socle_nand_flsh_base);

	/* Config setting */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONF,
			      SOCLE_NAND_FLSH_tWC_CYC_15 |
			      SOCLE_NAND_FLSH_tWP_CYC_7 |
			      SOCLE_NAND_FLSH_tRC_CYC_15 |
			      SOCLE_NAND_FLSH_tRP_CYC_7 |
			      SOCLE_NAND_FLSH_PAGE_2KB,
			      socle_nand_flsh_base);

	/* Command */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_1,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_READ0),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_2,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_READSTART),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_STATE_COMM,
			      SOCLE_NAND_FLSH_CMD_N_VALID,
			      socle_nand_flsh_base);

	/* Address */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_1,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(column),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_2,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(column >> 8),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_3,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_4,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr >> 8),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_5,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr >> 16),
			      socle_nand_flsh_base);

	/* DMA setting */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_DMA_SET,
			      SOCLE_NAND_FLSH_LITTLE_ENDING |
			      SOCLE_NAND_FLSH_ECC_TESTMODE_DIS |
			      SOCLE_NAND_FLSH_BUF_RW_EN |
			      SOCLE_NAND_FLSH_SEC_CMD_Y |
			      SOCLE_NAND_FLSH_AUTO_STAT_CHK_DIS |
			      SOCLE_NAND_FLSH_ACCESS_DATA_INTER_BUF |
			      SOCLE_NAND_FLSH_DMA_READ |
			      SOCLE_NAND_FLSH_DEV_WAIT_RB,
			      socle_nand_flsh_base);

	/* Set the start address of buffer */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_STADDR,
			      SOCLE_NAND_FLSH_INTER_BUF_STR_ADDR(0),
			      socle_nand_flsh_base);

	/* Set buffer count and ecc mode */
	if (ecc){
#ifdef CONFIG_NAND_TWO_ECC_MODEL
		if(npar == 6)
			socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
				      SOCLE_NAND_FLSH_RW_WITH_ECC | SOCLE_NAND_FLSH_NPAR_LEN_8B |
				      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(len-1),
				      socle_nand_flsh_base);
		else			
			socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
				      SOCLE_NAND_FLSH_RW_WITH_ECC | SOCLE_NAND_FLSH_NPAR_LEN_15B |
				      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(len-1),
				      socle_nand_flsh_base);
#else
			socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
				      SOCLE_NAND_FLSH_RW_WITH_ECC |
				      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(len-1),
				      socle_nand_flsh_base);
#endif
	}else
		socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
				      SOCLE_NAND_FLSH_RW_WITHOUT_ECC |
				      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(len-1),
				      socle_nand_flsh_base);


	socle_nand_flsh_int_flag = 0;
	socle_nand_flsh_int_stat = 0;
	 
	/* Start operation */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONTROL,
			      SOCLE_NAND_FLSH_DMA_EN |
			      SOCLE_NAND_FLSH_DATA_RW_EN |
			      SOCLE_NAND_FLSH_SEND_CMD_ADDR_EN,
			      socle_nand_flsh_base);

	if (socle_wait_for_int(&socle_nand_flsh_int_flag, 30)) {
		printf("Socle NAND Flash host: socle_nand_flsh_read_page_l() is timeout\n");
		return -1;
	} else {
		if (socle_nand_flsh_int_stat & SOCLE_NAND_FLSH_READ_DATA_DONE)
			goto out;
		else
			return -1;
	}
out:
	//20080321 leonid+ for check syn error status
	if (socle_nand_flsh_int_stat & SOCLE_NAND_FLSH_SYN_ERR_LOC_MASK)	
		return socle_nand_flsh_int_stat;
	return 0;
}

static int
socle_nand_flsh_program_page_l(u32 column, u32 page_addr, u32 len, int ecc)
{
	/* Reset the NAND Flash controller */
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_EN, socle_nand_flsh_base);
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_DIS, socle_nand_flsh_base);

	/* Clear all interrupt flags */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_STATE, 0, socle_nand_flsh_base);

	/* Set the interrupt mask */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_MASK,
			      ~(SOCLE_NAND_FLSH_MASK_WRITE_DATA_DONE |
				SOCLE_NAND_FLSH_MASK_WRITE_DATA_ERR),
			      socle_nand_flsh_base);

	/* Config setting and chip enable */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONF,
			      SOCLE_NAND_FLSH_tWC_CYC_15 |
			      SOCLE_NAND_FLSH_tWP_CYC_7 |
			      SOCLE_NAND_FLSH_tRC_CYC_15 |
			      SOCLE_NAND_FLSH_tRP_CYC_7 |
			      SOCLE_NAND_FLSH_PAGE_2KB,
			      socle_nand_flsh_base);

	/* Command */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_1,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_SEQIN),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_2,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_PAGEPROG),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_STATE_COMM,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_STATUS),
			      socle_nand_flsh_base);

	/* Address */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_1,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(column),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_2,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(column >> 8),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_3,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_4,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr >> 8),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_5,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr >> 16),
			      socle_nand_flsh_base);

	/* DMA setting */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_DMA_SET,
			      SOCLE_NAND_FLSH_LITTLE_ENDING |
			      SOCLE_NAND_FLSH_ECC_TESTMODE_DIS |
			      SOCLE_NAND_FLSH_BUF_RW_EN |
			      SOCLE_NAND_FLSH_SEC_CMD_Y |
			      SOCLE_NAND_FLSH_AUTO_STAT_CHK_EN |
			      SOCLE_NAND_FLSH_ACCESS_DATA_INTER_BUF |
			      SOCLE_NAND_FLSH_DMA_WRITE |
			      SOCLE_NAND_FLSH_DEV_WAIT_RB,
			      socle_nand_flsh_base);

	/* Set the start address of buffer */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_STADDR,
			      SOCLE_NAND_FLSH_INTER_BUF_STR_ADDR(0),
			      socle_nand_flsh_base);

	/* Set buffer count and ecc mode */
	if (ecc){
#ifdef CONFIG_NAND_TWO_ECC_MODEL
		if(npar == 6)
			socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
				      SOCLE_NAND_FLSH_RW_WITH_ECC | SOCLE_NAND_FLSH_NPAR_LEN_8B |
				      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(len-1),
				      socle_nand_flsh_base);
		else			
			socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
				      SOCLE_NAND_FLSH_RW_WITH_ECC | SOCLE_NAND_FLSH_NPAR_LEN_15B |
				      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(len-1),
				      socle_nand_flsh_base);
#else
			socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
				      SOCLE_NAND_FLSH_RW_WITH_ECC |
				      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(len-1),
				      socle_nand_flsh_base);
#endif
	}else
		socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
				      SOCLE_NAND_FLSH_RW_WITHOUT_ECC |
				      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(len-1),
				      socle_nand_flsh_base);


	socle_nand_flsh_int_flag = 0;
	socle_nand_flsh_int_stat = 0;

	/* Start operation */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONTROL,
			      SOCLE_NAND_FLSH_DMA_EN |
			      SOCLE_NAND_FLSH_DATA_RW_EN |
			      SOCLE_NAND_FLSH_SEND_CMD_ADDR_EN,
			      socle_nand_flsh_base);

	if (socle_wait_for_int(&socle_nand_flsh_int_flag, 30)) {
		printf("Socle NAND Flash host: socle_nand_flsh_program_page_l() is timeout\n");
		return -1;
	} else {
		if (socle_nand_flsh_int_stat & SOCLE_NAND_FLSH_WRITE_DATA_DONE)
			return 0;
		else
			return -1;
	}
}

static int 
socle_nand_flsh_cache_program_page_l(u32 column, u32 page_addr, u32 len, int ecc)
{
	/* Reset the NAND Flash controller */
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_EN, socle_nand_flsh_base);
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_DIS, socle_nand_flsh_base);

	/* Clear all interrupt flags */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_STATE, 0, socle_nand_flsh_base);

	/* Set the interrupt mask */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_MASK,
			      ~(SOCLE_NAND_FLSH_MASK_WRITE_DATA_DONE),
			      socle_nand_flsh_base);

	/* Config setting */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONF,
			      SOCLE_NAND_FLSH_tWC_CYC_15 |
			      SOCLE_NAND_FLSH_tWP_CYC_7 |
			      SOCLE_NAND_FLSH_tRC_CYC_15 |
			      SOCLE_NAND_FLSH_tRP_CYC_7 |
			      SOCLE_NAND_FLSH_PAGE_2KB,
			      socle_nand_flsh_base);

	/* Command */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_1,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_SEQIN),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_2,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_CACHEDPROG),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_STATE_COMM,
			      SOCLE_NAND_FLSH_CMD_N_VALID,
			      socle_nand_flsh_base);

	/* Address */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_1,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(column),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_2,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(column >> 8),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_3,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_4,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr >> 8),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_5,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr >> 16),
			      socle_nand_flsh_base);

	/* DMA setting */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_DMA_SET,
			      SOCLE_NAND_FLSH_LITTLE_ENDING |
			      SOCLE_NAND_FLSH_ECC_TESTMODE_DIS |
			      SOCLE_NAND_FLSH_BUF_RW_EN |
			      SOCLE_NAND_FLSH_SEC_CMD_Y |
			      SOCLE_NAND_FLSH_AUTO_STAT_CHK_DIS |
			      SOCLE_NAND_FLSH_ACCESS_DATA_INTER_BUF |
			      SOCLE_NAND_FLSH_DMA_WRITE |
			      SOCLE_NAND_FLSH_DEV_WAIT_RB,
			      socle_nand_flsh_base);

	/* Set the start address of buffer */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_STADDR,
			      SOCLE_NAND_FLSH_INTER_BUF_STR_ADDR(0),
			      socle_nand_flsh_base);

	/* Set buffer count and ecc mode */
	if (ecc){
#ifdef CONFIG_NAND_TWO_ECC_MODEL
		if(npar == 6)
			socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
				      SOCLE_NAND_FLSH_RW_WITH_ECC | SOCLE_NAND_FLSH_NPAR_LEN_8B |
				      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(len-1),
				      socle_nand_flsh_base);
		else			
			socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
				      SOCLE_NAND_FLSH_RW_WITH_ECC | SOCLE_NAND_FLSH_NPAR_LEN_15B |
				      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(len-1),
				      socle_nand_flsh_base);
#else
			socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
				      SOCLE_NAND_FLSH_RW_WITH_ECC |
				      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(len-1),
				      socle_nand_flsh_base);
#endif
	}else
		socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
				      SOCLE_NAND_FLSH_RW_WITHOUT_ECC |
				      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(len-1),
				      socle_nand_flsh_base);


	socle_nand_flsh_int_flag = 0;
	socle_nand_flsh_int_stat = 0;

	/* Start operation */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONTROL,
			      SOCLE_NAND_FLSH_DMA_EN |
			      SOCLE_NAND_FLSH_DATA_RW_EN |
			      SOCLE_NAND_FLSH_SEND_CMD_ADDR_EN,
			      socle_nand_flsh_base);

	if (socle_wait_for_int(&socle_nand_flsh_int_flag, 30)) {
		printf("Socle NAND Flash host: socle_nand_flsh_cache_program_page_l() is timeout\n");
		return -1;
	} else {
		if (socle_nand_flsh_int_stat & SOCLE_NAND_FLSH_WRITE_DATA_DONE)
			return 0;
		else
			return -1;
	}
}

#define NAND_CMD_COPYBACK_READ_L 0x00
#define NAND_CMD_COPYBACK_READSTART_L 0x35
#define NAND_CMD_COPYBACK_SEQIN_L 0x85
#define NAND_CMD_COPYBACK_PAGEPROG_L 0x10

static int 
socle_nand_flsh_copyback_page_l(u32 column_s, u32 page_addr_s, u32 column_d, u32 page_addr_d, u32 len)
{
	/*
	 *  Reading for copy-back operation
	 *  */
	/* Reset the NAND Flash controller */
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_EN, socle_nand_flsh_base);
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_DIS, socle_nand_flsh_base);

	/* Clear all interrupt flags */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_STATE, 0, socle_nand_flsh_base);

	/* Set the interrupt mask */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_MASK,
			      ~(SOCLE_NAND_FLSH_MASK_READ_DATA_DONE |
				SOCLE_NAND_FLSH_MASK_SYN_BIT),
			      socle_nand_flsh_base);

	/* Config setting */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONF,
			      SOCLE_NAND_FLSH_tWC_CYC_15 |
			      SOCLE_NAND_FLSH_tWP_CYC_7 |
			      SOCLE_NAND_FLSH_tRC_CYC_15 |
			      SOCLE_NAND_FLSH_tRP_CYC_7 |
			      SOCLE_NAND_FLSH_PAGE_2KB,
			      socle_nand_flsh_base);

	/* Command */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_1,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_COPYBACK_READ_L),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_2,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_COPYBACK_READSTART_L),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_STATE_COMM,
			      SOCLE_NAND_FLSH_CMD_N_VALID,
			      socle_nand_flsh_base);

	/* Address */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_1,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(column_s),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_2,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(column_s >> 8),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_3,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr_s),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_4,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr_s >> 8),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_5,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr_s >> 16),
			      socle_nand_flsh_base);


	/* DMA setting */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_DMA_SET,
			      SOCLE_NAND_FLSH_LITTLE_ENDING |
			      SOCLE_NAND_FLSH_ECC_TESTMODE_DIS |
			      SOCLE_NAND_FLSH_BUF_RW_DIS |
			      SOCLE_NAND_FLSH_SEC_CMD_Y |
			      SOCLE_NAND_FLSH_AUTO_STAT_CHK_DIS |
			      SOCLE_NAND_FLSH_ACCESS_DATA_INTER_BUF |
			      SOCLE_NAND_FLSH_DMA_READ |
			      SOCLE_NAND_FLSH_DEV_WAIT_RB,
			      socle_nand_flsh_base);

	socle_nand_flsh_int_flag = 0;
	socle_nand_flsh_int_stat = 0;

	/* Start operation */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONTROL,
			      SOCLE_NAND_FLSH_DMA_EN |
			      SOCLE_NAND_FLSH_DATA_RW_EN |
			      SOCLE_NAND_FLSH_SEND_CMD_ADDR_EN,
			      socle_nand_flsh_base);

	if (socle_wait_for_int(&socle_nand_flsh_int_flag, 30)) {
		printf("Socle NAND Flash host: socle_nand_flsh_copyback_page_l()(read) is timeout\n");
		return -1;
	} else {
		if (socle_nand_flsh_int_stat & SOCLE_NAND_FLSH_READ_DATA_DONE)
			goto write;
		else
			return -1;
	}

write:

	/*
	 *  Writing for copy-back operation
	 *  */
	/* Reset the NAND Flash controller */
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_EN, socle_nand_flsh_base);
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_DIS, socle_nand_flsh_base);

	/* Clear all interrupt flags */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_STATE, 0, socle_nand_flsh_base);

	/* Set the interrupt mask */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_MASK,
			      ~(SOCLE_NAND_FLSH_MASK_WRITE_DATA_DONE |
				SOCLE_NAND_FLSH_MASK_WRITE_DATA_ERR),
			      socle_nand_flsh_base);

	/* Config setting */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONF,
			      SOCLE_NAND_FLSH_tWC_CYC_15 |
			      SOCLE_NAND_FLSH_tWP_CYC_7 |
			      SOCLE_NAND_FLSH_tRC_CYC_15 |
			      SOCLE_NAND_FLSH_tRP_CYC_7 |
			      SOCLE_NAND_FLSH_PAGE_2KB,
			      socle_nand_flsh_base);

	/* Command */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_1,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_COPYBACK_SEQIN_L),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_2,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_COPYBACK_PAGEPROG_L),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_STATE_COMM,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_STATUS),
			      socle_nand_flsh_base);

	/* Address */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_1,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(column_d),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_2,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(column_d >> 8),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_3,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr_d),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_4,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr_d >> 8),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_5,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr_d >> 16),
			      socle_nand_flsh_base);

	/* DMA setting */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_DMA_SET,
			      SOCLE_NAND_FLSH_LITTLE_ENDING |
			      SOCLE_NAND_FLSH_ECC_TESTMODE_DIS |
			      SOCLE_NAND_FLSH_BUF_RW_DIS |
			      SOCLE_NAND_FLSH_SEC_CMD_Y |
			      SOCLE_NAND_FLSH_AUTO_STAT_CHK_EN |
			      SOCLE_NAND_FLSH_ACCESS_DATA_INTER_BUF |
			      SOCLE_NAND_FLSH_DMA_WRITE |
			      SOCLE_NAND_FLSH_DEV_WAIT_RB,
			      socle_nand_flsh_base);

	socle_nand_flsh_int_flag = 0;
	socle_nand_flsh_int_stat = 0;

	/* Start operation */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONTROL,
			      SOCLE_NAND_FLSH_DMA_EN |
			      SOCLE_NAND_FLSH_DATA_RW_EN |
			      SOCLE_NAND_FLSH_SEND_CMD_ADDR_EN,
			      socle_nand_flsh_base);

	if (socle_wait_for_int(&socle_nand_flsh_int_flag, 30)) {
		printf("Socle NAND Flash host: socle_nand_flsh_copyback_page_l()(write) is timeout\n");
		return -1;
	} else {
		if (socle_nand_flsh_int_stat & SOCLE_NAND_FLSH_WRITE_DATA_DONE)
			return 0;
		else
			return -1;
	}
}

static int
socle_nand_flsh_read_page_s(u32 column, u32 page_addr, u32 len, int ecc)
{
	/* Reset the NAND Flash controller */
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_EN, socle_nand_flsh_base);
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_DIS, socle_nand_flsh_base);

	/* Clear all interrupt flags */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_STATE, 0, socle_nand_flsh_base);

	/* Set the interrupt mask */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_MASK,
			      ~(SOCLE_NAND_FLSH_MASK_READ_DATA_DONE |
				SOCLE_NAND_FLSH_MASK_SYN_BIT),
			      socle_nand_flsh_base);

	/* Config setting */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONF,
			      SOCLE_NAND_FLSH_tWC_CYC_15 |
			      SOCLE_NAND_FLSH_tWP_CYC_7 |
			      SOCLE_NAND_FLSH_tRC_CYC_15 |
			      SOCLE_NAND_FLSH_tRP_CYC_7 |
			      SOCLE_NAND_FLSH_PAGE_512B,
			      socle_nand_flsh_base);

	/* Command */
	if (column >= 512) {
		/* OOB area */
		column -= 512;
		socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_1,
				      SOCLE_NAND_FLSH_CMD_VALID |
				      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_READOOB),
				      socle_nand_flsh_base);
	} else if(column < 256) {
		/* First 256 bytes --> READ0 */
		socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_1,
				      SOCLE_NAND_FLSH_CMD_VALID |
				      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_READ0),
				      socle_nand_flsh_base);

	} else {
		column -= 256;
		socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_1,
				      SOCLE_NAND_FLSH_CMD_VALID |
				      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_READ1),
				      socle_nand_flsh_base);

	}
	socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_2,
			      SOCLE_NAND_FLSH_CMD_N_VALID,
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_STATE_COMM,
			      SOCLE_NAND_FLSH_CMD_N_VALID,
			      socle_nand_flsh_base);

	/* Address */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_1,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(column),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_2,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(column >> 8),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_3,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_4,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr >> 8),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_5,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr >> 16),
			      socle_nand_flsh_base);

	/* DMA setting */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_DMA_SET,
			      SOCLE_NAND_FLSH_LITTLE_ENDING |
			      SOCLE_NAND_FLSH_ECC_TESTMODE_DIS |
			      SOCLE_NAND_FLSH_BUF_RW_EN |
			      SOCLE_NAND_FLSH_SEC_CMD_N |
			      SOCLE_NAND_FLSH_AUTO_STAT_CHK_DIS |
			      SOCLE_NAND_FLSH_ACCESS_DATA_INTER_BUF |
			      SOCLE_NAND_FLSH_DMA_READ |
			      SOCLE_NAND_FLSH_DEV_WAIT_RB,
			      socle_nand_flsh_base);

	/* Set the start address of buffer */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_STADDR,
			      SOCLE_NAND_FLSH_INTER_BUF_STR_ADDR(0),
			      socle_nand_flsh_base);

	/* Set buffer count and ecc mode */
	if (ecc){
#ifdef CONFIG_NAND_TWO_ECC_MODEL
		if(npar == 6)
			socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
				      SOCLE_NAND_FLSH_RW_WITH_ECC | SOCLE_NAND_FLSH_NPAR_LEN_8B |
				      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(len-1),
				      socle_nand_flsh_base);
		else			
			socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
				      SOCLE_NAND_FLSH_RW_WITH_ECC | SOCLE_NAND_FLSH_NPAR_LEN_15B |
				      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(len-1),
				      socle_nand_flsh_base);
#else
			socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
				      SOCLE_NAND_FLSH_RW_WITH_ECC |
				      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(len-1),
				      socle_nand_flsh_base);
#endif
	}else
		socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
				      SOCLE_NAND_FLSH_RW_WITHOUT_ECC |
				      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(len-1),
				      socle_nand_flsh_base);

     
	socle_nand_flsh_int_flag = 0;
	socle_nand_flsh_int_stat = 0;

	/* Start operation */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONTROL,
			      SOCLE_NAND_FLSH_DMA_EN |
			      SOCLE_NAND_FLSH_DATA_RW_EN |
			      SOCLE_NAND_FLSH_SEND_CMD_ADDR_EN,
			      socle_nand_flsh_base);

	if (socle_wait_for_int(&socle_nand_flsh_int_flag, 30)) {
		printf("Socle NAND Flash host: socle_nand_flsh_read_page_s() is timeout\n");
		return -1;
	} else {
		if (socle_nand_flsh_int_stat & SOCLE_NAND_FLSH_READ_DATA_DONE)
			goto out;
		else
			return -1;
	}
out:
	//20080321 leonid+ for check syn error status
	if (socle_nand_flsh_int_stat & SOCLE_NAND_FLSH_SYN_ERR_LOC_MASK)	
		return socle_nand_flsh_int_stat;
	return 0;
}

static int 
socle_nand_flsh_program_page_s(u32 column, u32 page_addr, u32 len, int ecc)
{

	/*
	 *  Pointer Operation
	 *  */
	/* Reset the NAND Flash controller */
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_EN, socle_nand_flsh_base);
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_DIS, socle_nand_flsh_base);

	/* Clear all interrupt flags */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_STATE, 0, socle_nand_flsh_base);

	/* Set the interrupt mask */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_MASK,
			      ~(SOCLE_NAND_FLSH_MASK_READ_DATA_DONE |
				SOCLE_NAND_FLSH_MASK_SYN_BIT),
			      socle_nand_flsh_base);

	/* Config setting */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONF,
			      SOCLE_NAND_FLSH_tWC_CYC_15 |
			      SOCLE_NAND_FLSH_tWP_CYC_7 |
			      SOCLE_NAND_FLSH_tRC_CYC_15 |
			      SOCLE_NAND_FLSH_tRP_CYC_7 |
			      SOCLE_NAND_FLSH_PAGE_512B,
			      socle_nand_flsh_base);

	/* Command */
	if (column >= 512) {
		/* OOB area */
		column -= 512;
		socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_1,
				      SOCLE_NAND_FLSH_CMD_VALID |
				      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_READOOB),
				      socle_nand_flsh_base);
	} else if(column < 256) {
		/* First 256 bytes --> READ0 */
		socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_1,
				      SOCLE_NAND_FLSH_CMD_VALID |
				      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_READ0),
				      socle_nand_flsh_base);
	} else {
		column -= 256;
		socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_1,
				      SOCLE_NAND_FLSH_CMD_VALID |
				      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_READ1),
				      socle_nand_flsh_base);
	}
	socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_2,
			      SOCLE_NAND_FLSH_CMD_N_VALID,
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_STATE_COMM,
			      SOCLE_NAND_FLSH_CMD_N_VALID,
			      socle_nand_flsh_base);

	/* Disable all address cycles */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_1,
			      SOCLE_NAND_FLSH_ADDR_N_VALID,
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_2,
			      SOCLE_NAND_FLSH_ADDR_N_VALID,
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_3,
			      SOCLE_NAND_FLSH_ADDR_N_VALID,
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_4,
			      SOCLE_NAND_FLSH_ADDR_N_VALID,
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_5,
			      SOCLE_NAND_FLSH_ADDR_VALID,
			      socle_nand_flsh_base);

	/* DMA setting */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_DMA_SET,
			      SOCLE_NAND_FLSH_LITTLE_ENDING |
			      SOCLE_NAND_FLSH_ECC_TESTMODE_DIS |
			      SOCLE_NAND_FLSH_BUF_RW_DIS |
			      SOCLE_NAND_FLSH_SEC_CMD_N |
			      SOCLE_NAND_FLSH_AUTO_STAT_CHK_DIS |
			      SOCLE_NAND_FLSH_ACCESS_DATA_INTER_BUF |
			      SOCLE_NAND_FLSH_DMA_READ |
			      SOCLE_NAND_FLSH_DEV_WAIT_RB,
			      socle_nand_flsh_base);

	socle_nand_flsh_int_flag = 0;
	socle_nand_flsh_int_stat = 0;

	/* Start operation */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONTROL,
			      SOCLE_NAND_FLSH_DMA_EN |
			      SOCLE_NAND_FLSH_DATA_RW_EN |
			      SOCLE_NAND_FLSH_SEND_CMD_ADDR_EN,
			      socle_nand_flsh_base);

	if (socle_wait_for_int(&socle_nand_flsh_int_flag, 30)) {
		printf("Socle NAND Flash host: socle_nand_flsh_program_page_s()(pre-point)\n");
		return -1;
	} else {
		if (socle_nand_flsh_int_stat & SOCLE_NAND_FLSH_READ_DATA_DONE)
			goto real_page_program;
		else
			return -1;
	}

real_page_program:

	/*
	 *  Real page program sequence
	 *  */
	/* Reset the NAND Flash controller */
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_EN, socle_nand_flsh_base);
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_DIS, socle_nand_flsh_base);

	/* Clear all interrupt flags */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_STATE, 0, socle_nand_flsh_base);

	/* Set the interrupt mask */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_MASK,
			      ~(SOCLE_NAND_FLSH_MASK_WRITE_DATA_DONE |
				SOCLE_NAND_FLSH_MASK_WRITE_DATA_ERR),
			      socle_nand_flsh_base);

	/* Config setting */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONF,
			      SOCLE_NAND_FLSH_tWC_CYC_15 |
			      SOCLE_NAND_FLSH_tWP_CYC_7 |
			      SOCLE_NAND_FLSH_tRC_CYC_15 |
			      SOCLE_NAND_FLSH_tRP_CYC_7 |
			      SOCLE_NAND_FLSH_PAGE_512B,
			      socle_nand_flsh_base);

	/* Command */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_1,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_SEQIN),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_2,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_PAGEPROG),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_STATE_COMM,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_STATUS),
			      socle_nand_flsh_base);

	/* Address */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_1,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(column),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_2,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(column >> 8),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_3,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_4,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr >> 8),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_5,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr >> 16),
			      socle_nand_flsh_base);

	/* DMA setting */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_DMA_SET,
			      SOCLE_NAND_FLSH_LITTLE_ENDING |
			      SOCLE_NAND_FLSH_ECC_TESTMODE_DIS |
			      SOCLE_NAND_FLSH_BUF_RW_EN |
			      SOCLE_NAND_FLSH_SEC_CMD_Y |
			      SOCLE_NAND_FLSH_AUTO_STAT_CHK_EN |
			      SOCLE_NAND_FLSH_ACCESS_DATA_INTER_BUF |
			      SOCLE_NAND_FLSH_DMA_WRITE |
			      SOCLE_NAND_FLSH_DEV_WAIT_RB,
			      socle_nand_flsh_base);

	/* Set the start address of buffer */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_STADDR,
			      SOCLE_NAND_FLSH_INTER_BUF_STR_ADDR(0),
			      socle_nand_flsh_base);

	/* Set buffer count and ecc mode */
	if (ecc){
#ifdef CONFIG_NAND_TWO_ECC_MODEL
		if(npar == 6)
			socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
				      SOCLE_NAND_FLSH_RW_WITH_ECC | SOCLE_NAND_FLSH_NPAR_LEN_8B |
				      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(len-1),
				      socle_nand_flsh_base);
		else			
			socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
				      SOCLE_NAND_FLSH_RW_WITH_ECC | SOCLE_NAND_FLSH_NPAR_LEN_15B |
				      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(len-1),
				      socle_nand_flsh_base);
#else
			socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
				      SOCLE_NAND_FLSH_RW_WITH_ECC |
				      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(len-1),
				      socle_nand_flsh_base);
#endif
	}else
		socle_nand_flsh_write(SOCLE_NAND_FLSH_BUFF_CNT,
				      SOCLE_NAND_FLSH_RW_WITHOUT_ECC |
				      SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(len-1),
				      socle_nand_flsh_base);


	socle_nand_flsh_int_flag = 0;
	socle_nand_flsh_int_stat = 0;

	/* Start operation */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONTROL,
			      SOCLE_NAND_FLSH_DMA_EN |
			      SOCLE_NAND_FLSH_DATA_RW_EN |
			      SOCLE_NAND_FLSH_SEND_CMD_ADDR_EN,
			      socle_nand_flsh_base);

	if (socle_wait_for_int(&socle_nand_flsh_int_flag, 30)) {
		printf("Socle NAND Flash host: socle_nand_flsh_program_page_s()\n");
		return -1;
	} else {
		if (socle_nand_flsh_int_stat & SOCLE_NAND_FLSH_WRITE_DATA_DONE)
			return 0;
		else 
			return -1;
	}
}

#define NAND_CMD_COPYBACK_READ_S 0x00
#define NAND_CMD_COPYBACK_SEQIN_S 0x8A
#define NAND_CMD_COPYBACK_PAGEPROG_S 0x10

static int 
socle_nand_flsh_copyback_page_s(u32 column_s, u32 page_addr_s, u32 column_d, u32 page_addr_d, u32 len)
{
	/*
	 *  Reading for copy-back operation
	 *  */
	/* Reset the NAND Flash controller */
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_EN, socle_nand_flsh_base);
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_DIS, socle_nand_flsh_base);

	/* Clear all interrupt flags */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_STATE, 0, socle_nand_flsh_base);

	/* Set the interrupt mask */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_MASK,
			      ~(SOCLE_NAND_FLSH_MASK_READ_DATA_DONE |
				SOCLE_NAND_FLSH_MASK_SYN_BIT),
			      socle_nand_flsh_base);

	/* Config setting */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONF,
			      SOCLE_NAND_FLSH_tWC_CYC_15 |
			      SOCLE_NAND_FLSH_tWP_CYC_7 |
			      SOCLE_NAND_FLSH_tRC_CYC_15 |
			      SOCLE_NAND_FLSH_tRP_CYC_7 |
			      SOCLE_NAND_FLSH_PAGE_512B,
			      socle_nand_flsh_base);

	/* Command */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_1,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_COPYBACK_READ_S),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_2,
			      SOCLE_NAND_FLSH_CMD_N_VALID,
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_STATE_COMM,
			      SOCLE_NAND_FLSH_CMD_N_VALID,
			      socle_nand_flsh_base);

	/* Address */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_1,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(column_s),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_2,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(column_s >> 8),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_3,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr_s),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_4,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr_s >> 8),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_5,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr_s >> 16),
			      socle_nand_flsh_base);

	/* DMA setting */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_DMA_SET,
			      SOCLE_NAND_FLSH_LITTLE_ENDING |
			      SOCLE_NAND_FLSH_ECC_TESTMODE_DIS |
			      SOCLE_NAND_FLSH_BUF_RW_DIS |
			      SOCLE_NAND_FLSH_SEC_CMD_N |
			      SOCLE_NAND_FLSH_AUTO_STAT_CHK_DIS |
			      SOCLE_NAND_FLSH_ACCESS_DATA_INTER_BUF |
			      SOCLE_NAND_FLSH_DMA_READ |
			      SOCLE_NAND_FLSH_DEV_WAIT_RB,
			      socle_nand_flsh_base);

	socle_nand_flsh_int_flag = 0;
	socle_nand_flsh_int_stat = 0;

	/* Start operation */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONTROL,
			      SOCLE_NAND_FLSH_DMA_EN |
			      SOCLE_NAND_FLSH_DATA_RW_EN |
			      SOCLE_NAND_FLSH_SEND_CMD_ADDR_EN,
			      socle_nand_flsh_base);

	if (socle_wait_for_int(&socle_nand_flsh_int_flag, 30)) {
		printf("Socle NAND Flash host: socle_nand_flsh_copyback_page_s() is timeout\n");
		return -1;
	} else {
		if (socle_nand_flsh_int_stat & SOCLE_NAND_FLSH_READ_DATA_DONE)
			goto write;
		else
			return -1;
	}

write:

	/*
	 *  Writing for copy-back operation
	 *  */
	/* Reset the NAND Flash controller */
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_EN, socle_nand_flsh_base);
	//socle_nand_flsh_write(SOCLE_NAND_FLSH_RESET, SOCLE_NAND_FLSH_RST_DIS, socle_nand_flsh_base);

	/* Clear all interrupt flags */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_STATE, 0, socle_nand_flsh_base);

	/* Set the interrupt mask */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_INT_MASK,
			      ~(SOCLE_NAND_FLSH_MASK_WRITE_DATA_DONE |
				SOCLE_NAND_FLSH_MASK_WRITE_DATA_ERR),
			      socle_nand_flsh_base);

	/* Config setting */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONF,
			      SOCLE_NAND_FLSH_tWC_CYC_15 |
			      SOCLE_NAND_FLSH_tWP_CYC_7 |
			      SOCLE_NAND_FLSH_tRC_CYC_15 |
			      SOCLE_NAND_FLSH_tRP_CYC_7 |
			      SOCLE_NAND_FLSH_PAGE_512B,
			      socle_nand_flsh_base);

	/* Command */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_1,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_COPYBACK_SEQIN_S),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_COMM_2,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_COPYBACK_PAGEPROG_S),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_STATE_COMM,
			      SOCLE_NAND_FLSH_CMD_VALID |
			      SOCLE_NAND_FLSH_CMD_INPUT(NAND_CMD_STATUS),
			      socle_nand_flsh_base);

	/* Address */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_1,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(column_d),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_2,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(column_d >> 8),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_3,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr_d),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_4,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr_d >> 8),
			      socle_nand_flsh_base);
	socle_nand_flsh_write(SOCLE_NAND_FLSH_ADDRESS_5,
			      SOCLE_NAND_FLSH_ADDR_VALID |
			      SOCLE_NAND_FLSH_ADDR_INPUT(page_addr_d >> 16),
			      socle_nand_flsh_base);

	/* DMA setting */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_DMA_SET,
			      SOCLE_NAND_FLSH_LITTLE_ENDING |
			      SOCLE_NAND_FLSH_ECC_TESTMODE_DIS |
			      SOCLE_NAND_FLSH_BUF_RW_DIS |
			      SOCLE_NAND_FLSH_SEC_CMD_Y |
			      SOCLE_NAND_FLSH_AUTO_STAT_CHK_EN |
			      SOCLE_NAND_FLSH_ACCESS_DATA_INTER_BUF |
			      SOCLE_NAND_FLSH_DMA_WRITE |
			      SOCLE_NAND_FLSH_DEV_WAIT_RB,
			      socle_nand_flsh_base);

	socle_nand_flsh_int_flag = 0;
	socle_nand_flsh_int_stat = 0;

	/* Start operation */
	socle_nand_flsh_write(SOCLE_NAND_FLSH_CONTROL,
			      SOCLE_NAND_FLSH_DMA_EN |
			      SOCLE_NAND_FLSH_DATA_RW_EN |
			      SOCLE_NAND_FLSH_SEND_CMD_ADDR_EN,
			      socle_nand_flsh_base);

	if (socle_wait_for_int(&socle_nand_flsh_int_flag, 30)) {
		printf("Socle NAND Flash host: socle_nand_flsh_ecc_read() is timeout\n");
		return -1;
	} else {
		if (socle_nand_flsh_int_stat & SOCLE_NAND_FLSH_WRITE_DATA_DONE)
			return 0;
		else
			return -1;
	}
}

