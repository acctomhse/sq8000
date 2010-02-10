#include <test_item.h>
#include <genlib.h>
#include <io.h>
#include <type.h>
#include "i2c-regs.h"
#include "dependency.h"
#include "i2c.h"

#if defined (CONFIG_PC9220)
#include <scu.h>
#include "../../platform/arch/scu-reg.h"
#endif

#include "tps62353.h"


#define I2C_SLAVE_ADDR_7_BITS 0x46
#define I2C_SLAVE_ADDR_10_BITS ((0x7 << 7) | I2C_SLAVE_ADDR_7_BITS)
#define I2C_GENERAL_CALL_ADDR 0x00
#define I2C_MASTER_ADDR 0x64

#define AT24C02B_EEPROM_ADDR 0x50
#define AT24C02B_EEPROM_PAGE_SIZE 8
#define AT24C02B_EEPROM_SIZE 2048

static u32 socle_i2c_base;
static int socle_i2c_irq;		//20080701 leonid+ 
static struct i2c_client socle_i2c_client;

extern struct test_item_container socle_i2c_pre_test_container;

extern int
I2CTesting(int autotest)
{
	int ret = 0;

#ifdef CONFIG_PC9220
	//socle_scu_dev_enable(SCU_DEVCON_I2C_GPIO);
	socle_scu_dev_enable(SOCLE_DEVCON_I2C);
#endif

	ret = test_item_ctrl(&socle_i2c_pre_test_container, autotest);

#ifdef CONFIG_PC9220
	//socle_scu_dev_disable(SCU_DEVCON_I2C_GPIO);
	socle_scu_dev_disable(SOCLE_DEVCON_I2C);
#endif
	
	return ret;
}

extern struct test_item_container socle_i2c_main_test_container;

extern int
socle_i2c0_test(int autotest)
{
	int ret = 0;

	socle_i2c_base = SOCLE_APB0_I2C0;
	socle_i2c_irq = SOCLE_INTC_I2C0;
	i2c_master_initialize(socle_i2c_base, socle_i2c_irq);
	
#ifdef CONFIG_MSMV
	tps62353_set_type(TPS62353_TYPE0);
#endif
	
	ret = test_item_ctrl(&socle_i2c_main_test_container, autotest);

	return ret;
}

extern int
socle_i2c1_test(int autotest)
{
	int ret = 0;

       socle_i2c_base = SOCLE_APB0_I2C1;		
	socle_i2c_irq = SOCLE_INTC_I2C1;
        i2c_master_initialize(socle_i2c_base, socle_i2c_irq);
			
#ifdef CONFIG_MSMV
	tps62353_set_type(TPS62353_TYPE1);
#endif

        ret = test_item_ctrl(&socle_i2c_main_test_container, autotest);

	return ret;
}

extern int
socle_i2c2_test(int autotest)
{
	int ret = 0;
        
	socle_i2c_base = SOCLE_APB0_I2C2;
	socle_i2c_irq = SOCLE_INTC_I2C2;
        i2c_master_initialize(socle_i2c_base, socle_i2c_irq);
			
#ifdef CONFIG_MSMV
	tps62353_set_type(TPS62353_TYPE2);
#endif

        ret = test_item_ctrl(&socle_i2c_main_test_container, autotest);

	return ret;
}

extern struct test_item_container socle_i2c_address_test_container;

extern int
socle_i2c_internal_loopback_test(int autotest)
{
	int ret = 0;

	/* Enable the slave port */
	socle_i2c_write(SOCLE_I2C_CONR,
			socle_i2c_read(SOCLE_I2C_CONR, socle_i2c_base) |
			SOCLE_I2C_SLAVE_PORT_EN,
			socle_i2c_base);

	/* Enable the interrupt for slave port */
	socle_i2c_write(SOCLE_I2C_IER,
			socle_i2c_read(SOCLE_I2C_IER, socle_i2c_base) |
			SOCLE_I2C_ABNORMAL_STP_INT_EN |
			SOCLE_I2C_BROADCAST_ADDR_INT_EN |
			SOCLE_I2C_SLAVE_ADDR_INT_EN |
			SOCLE_I2C_SLAVE_ACK_PERIOD_INT_EN |
			SOCLE_I2C_SLAVE_RECV_ACK_INT_EN,
			socle_i2c_base);

	ret = test_item_ctrl(&socle_i2c_address_test_container, autotest);
	return ret;
}

extern struct test_item_container socle_i2c_gen_call_addr_test_container;

extern int 
socle_i2c_general_call_address_test(int autotest)
{
	int ret;

	/* Enable the slave port */
	socle_i2c_write(SOCLE_I2C_CONR,
			socle_i2c_read(SOCLE_I2C_CONR, socle_i2c_base) |
			SOCLE_I2C_SLAVE_PORT_EN,
			socle_i2c_base);

	/* Enable the interrupt for slave port */
	socle_i2c_write(SOCLE_I2C_IER,
			socle_i2c_read(SOCLE_I2C_IER, socle_i2c_base) |
			SOCLE_I2C_ABNORMAL_STP_INT_EN |
			SOCLE_I2C_BROADCAST_ADDR_INT_EN |
			SOCLE_I2C_SLAVE_ADDR_INT_EN |
			SOCLE_I2C_SLAVE_ACK_PERIOD_INT_EN |
			SOCLE_I2C_SLAVE_RECV_ACK_INT_EN,
			socle_i2c_base);

	/* Set the slave to be as a receiver */
	socle_i2c_write(SOCLE_I2C_CONR,
			socle_i2c_read(SOCLE_I2C_CONR, socle_i2c_base) &
			~SOCLE_I2C_SLAVE_TRAN_SEL,
			socle_i2c_base);

	memset((void *)&socle_i2c_client, 0x00, sizeof(struct i2c_client));
	socle_i2c_client.addr = I2C_GENERAL_CALL_ADDR;
	ret = test_item_ctrl(&socle_i2c_gen_call_addr_test_container, autotest);
	return ret;
}

extern struct test_item_container socle_i2c_xfer_test_container;

extern int 
socle_i2c_address_7_test(int autotest)
{
	int ret = 0;

	/* Set the slave address */
	socle_i2c_write(SOCLE_I2C_SADDR, I2C_SLAVE_ADDR_7_BITS, socle_i2c_base);

	/* Set the I2C slave address mode */
	socle_i2c_write(SOCLE_I2C_OPR,
			socle_i2c_read(SOCLE_I2C_OPR, socle_i2c_base) &
			(~SOCLE_I2C_ADDR_10_BITS),
			socle_i2c_base);

	memset((void *)&socle_i2c_client, 0x00, sizeof(struct i2c_client));
	socle_i2c_client.addr = I2C_SLAVE_ADDR_7_BITS;

	ret = test_item_ctrl(&socle_i2c_xfer_test_container, autotest);
	return ret;
}

extern int
socle_i2c_address_10_test(int autotest)
{
	int ret = 0;

	/* Set the slave address */
	socle_i2c_write(SOCLE_I2C_SADDR, I2C_SLAVE_ADDR_10_BITS, socle_i2c_base);

	/* Set the I2C slave address mode */
	socle_i2c_write(SOCLE_I2C_OPR,
			socle_i2c_read(SOCLE_I2C_OPR, socle_i2c_base) |
			SOCLE_I2C_ADDR_10_BITS,
			socle_i2c_base);

	memset((void *)&socle_i2c_client, 0x00, sizeof(struct i2c_client));
	socle_i2c_client.addr = I2C_SLAVE_ADDR_10_BITS;
	socle_i2c_client.flags = I2C_M_TEN;

	ret = test_item_ctrl(&socle_i2c_xfer_test_container, autotest);
	return ret;
}

extern int 
socle_i2c_general_call_address_reset_test(int autotest)
{
	u8 buf = 0x06;
	int ret;

	/*
	 *  Master write data to slave
	 *  */
	ret = i2c_master_send(&socle_i2c_client, &buf, 1);
	if (ret != 1)
		return -1;

	return 0;
}

extern int 
socle_i2c_general_call_address_write_test(int autotest)
{
	u8 buf = 0x04;
	int ret;

	/*
	 *  Master write data to slave
	 *  */
	ret = i2c_master_send(&socle_i2c_client, &buf, 1);
	if (ret != 1)
		return -1;

	return 0;
}

extern int socle_i2c_hardware_general_call_address_test(int autotest)
{
	u8 buf = I2C_MASTER_ADDR << 1 | 0x01;
	int ret;

	/*
	 *  Master write data to slave
	 *  */
	ret = i2c_master_send(&socle_i2c_client, &buf, 1);
	if (ret != 1)
		return -1;

	return 0;
}

static void socle_i2c_make_test_pattern(u8 *buf, u32 size);
static int socle_i2c_compare_memory(u8 *mem, u8 *cmpr_mem, u32 size,
				    int skip_cmpr_result);

#define PATTERN_BUF_ADDR 0x00a00000
#define PATTERN_BUF_SIZE 2048

static u8 *socle_i2c_pattern_buf = (u8 *)PATTERN_BUF_ADDR;
static u8 *socle_i2c_cmpr_buf = (u8 *)(PATTERN_BUF_ADDR + PATTERN_BUF_SIZE);

extern int 
socle_i2c_at24c02b_eeprom_test(int autotest)
{
	int ret = 0;
	int i;
	u32 write_cnt = AT24C02B_EEPROM_SIZE / AT24C02B_EEPROM_PAGE_SIZE;
	u32 write_idx = 0;
	u8 addr = 0;
	u8 buf[9];

	memset((void *)&socle_i2c_client, 0x00, sizeof(struct i2c_client));
	socle_i2c_client.addr = AT24C02B_EEPROM_ADDR;

	/* Clear pattern buffer and compare buffer */
	memset(socle_i2c_pattern_buf, 0x00, AT24C02B_EEPROM_SIZE);
	memset(socle_i2c_cmpr_buf, 0x00, AT24C02B_EEPROM_SIZE);

	/* Make the pattern */
	socle_i2c_make_test_pattern(socle_i2c_pattern_buf, AT24C02B_EEPROM_SIZE);

	/*
	 *  Master write data to the slave
	 *  */
	for (i = 0; i < write_cnt; i++) {
		int j;

		buf[0] = addr;
		for (j = 0 ; j < 8; j++)
			buf[j+1] = socle_i2c_pattern_buf[write_idx+j];
		ret = i2c_master_send(&socle_i2c_client, buf, AT24C02B_EEPROM_PAGE_SIZE+1);
		if (ret != AT24C02B_EEPROM_PAGE_SIZE+1) {
			printf("Writing to AT24C02B eeprom is fail, address is 0x%02x\n", addr);
			return -1;
		}
		addr += 8;
		write_idx += 8;
		MSDELAY(5);
	}

	/* Reset the address of eeprom */
	buf[0] = 0;
	ret = i2c_master_send(&socle_i2c_client, buf, 1);
	if (ret != 1)
		return -1;
	MSDELAY(5);

	/*
	 *  Master read data from the slave
	 *  */
	ret = i2c_master_recv(&socle_i2c_client, socle_i2c_cmpr_buf, AT24C02B_EEPROM_SIZE);
	if (ret != AT24C02B_EEPROM_SIZE) {
		printf("Reading from  AT24C02B eeprom is fail");
		return -1;
	}

	return socle_i2c_compare_memory(socle_i2c_pattern_buf, socle_i2c_cmpr_buf, PATTERN_BUF_SIZE,
					autotest);
}

extern int 
socle_i2c_transfer_normal_format_test(int autotest)
{
	int ret;

	/* Clear pattern buffer and compare buffer */
	memset(socle_i2c_pattern_buf, 0x00, PATTERN_BUF_SIZE);
	memset(socle_i2c_cmpr_buf, 0x00, PATTERN_BUF_SIZE);

	/* Make the pattern */
	socle_i2c_make_test_pattern(socle_i2c_pattern_buf, PATTERN_BUF_SIZE);

	/*
	 *  Master write data to slave
	 *  */
	ret = i2c_master_send(&socle_i2c_client, socle_i2c_pattern_buf, PATTERN_BUF_SIZE);
	if (ret != PATTERN_BUF_SIZE)
		return -1;

	/*
	 *  Master read data from slave
	 *  */
	ret = i2c_master_recv(&socle_i2c_client, socle_i2c_cmpr_buf, PATTERN_BUF_SIZE);
	if (ret != PATTERN_BUF_SIZE)
		return -1;

	return socle_i2c_compare_memory(socle_i2c_pattern_buf, socle_i2c_cmpr_buf, PATTERN_BUF_SIZE,
					autotest);

}

extern int 
socle_i2c_transfer_combined_format_test(int autotest)
{
	int ret, i;
	struct i2c_msg msg[2];

	/* Clear pattern buffer and compare buffer */
	memset(socle_i2c_pattern_buf, 0x0, PATTERN_BUF_SIZE);
	memset(socle_i2c_cmpr_buf, 0x0, PATTERN_BUF_SIZE);

	/* Make the pattern */
	socle_i2c_make_test_pattern(socle_i2c_pattern_buf, PATTERN_BUF_SIZE);

	for (i = 0; i < 2; i++) {
		msg[i].addr = socle_i2c_client.addr;
		msg[i].flags = socle_i2c_client.flags;
		msg[i].len = PATTERN_BUF_SIZE;
	}
	msg[0].buf = socle_i2c_pattern_buf;
	msg[1].buf = socle_i2c_cmpr_buf;
	msg[1].flags |= I2C_M_RD;

	ret = i2c_transfer(msg, 2);
	if (ret != 2)
		return -1;

	return socle_i2c_compare_memory(socle_i2c_pattern_buf, socle_i2c_cmpr_buf, PATTERN_BUF_SIZE,
					autotest);

}

static void
socle_i2c_make_test_pattern(u8 *buf, u32 size)
{
	int i;

	for (i = 0; i < size; i++)
		buf[i] = i;
}

static int
socle_i2c_compare_memory(u8 *mem, u8 *cmpr_mem, u32 size,
			 int skip_cmpr_result)
{
	int i;
	int err_flag = 0;

	printf("\n");
	for (i = 0; i < size; i++) {
		if (mem[i] != cmpr_mem[i]) {
			err_flag = -1;
			if (!skip_cmpr_result)
				printf("Byte %d, 0x%02x (0x%08x) != 0x%02x (0x%08x)\n", i, mem[i], &mem[i],
				       cmpr_mem[i], &cmpr_mem[i]);
		}
	}
	return err_flag;
}

