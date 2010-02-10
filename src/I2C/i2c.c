#include <genlib.h>
#include <io.h>
#include <type.h>
#include <interrupt.h>
#include "dependency.h"
#include "i2c-regs.h"
#include "i2c.h"

static void socle_i2c_isr(void *data);
static u32 socle_i2c_calculate_divisor(u32 clk);
static int socle_i2c_do_transfer(struct i2c_msg *msgs, int num);
static int socle_i2c_do_address(struct i2c_msg *msgs);
static int socle_i2c_read_bytes(struct i2c_msg *msgs);
static int socle_i2c_send_bytes(struct i2c_msg *msgs);
static void socle_i2c_ack(void);
static void socle_i2c_nak(void);
static void socle_i2c_start(void);
static void socle_i2c_stop(void);
static void socle_i2c_resume(void);
static void socle_i2c_master_write_byte(u8 data);
static int socle_i2c_master_read_byte(void);
static int socle_i2c_master_check_ack(void);

static u32 socle_i2c_base;
static u8 socle_i2c_slave_buf[2048] = {0};
static u32 socle_i2c_slave_buf_idx = 0;
static int int_master_ack_period = 0, int_master_recv_ack = 0;

/*
 *  The functional interface to the i2c buses
 *  */

extern void
i2c_master_initialize(u32 ip_mem_base, u32 ip_irq)
{
	u32 i2c_cdvr = 0;

	socle_i2c_base = ip_mem_base;

	request_irq(ip_irq, socle_i2c_isr, NULL);

	/* Enable the I2C controller */
	socle_i2c_write(SOCLE_I2C_OPR,
			socle_i2c_read(SOCLE_I2C_OPR, socle_i2c_base) |
			SOCLE_I2C_CORE_EN,
			socle_i2c_base);

	/* Enable the master port */
	socle_i2c_write(SOCLE_I2C_CONR,
			socle_i2c_read(SOCLE_I2C_CONR, socle_i2c_base) |
			SOCLE_I2C_MASTER_PORT_EN,
			socle_i2c_base);

	/* Reset I2C state machine of both master and slave */
	socle_i2c_write(SOCLE_I2C_OPR,
			socle_i2c_read(SOCLE_I2C_OPR, socle_i2c_base) |
			SOCLE_I2C_RST,
			socle_i2c_base);

	/* Restore the reset bit to 0  */
	socle_i2c_write(SOCLE_I2C_OPR,
			socle_i2c_read(SOCLE_I2C_OPR, socle_i2c_base) &
			~SOCLE_I2C_RST,
			socle_i2c_base);

	i2c_cdvr = socle_i2c_calculate_divisor(400000);
	
	/* Set the divisor */
	socle_i2c_write(SOCLE_I2C_OPR,
			(socle_i2c_read(SOCLE_I2C_OPR, socle_i2c_base) & 
			 ~SOCLE_I2C_CLK_DIVISOR(0x3f)) |
			SOCLE_I2C_CLK_DIVISOR(i2c_cdvr),
			socle_i2c_base);

	/* Set interrupt generation of I2C controller */
	socle_i2c_write(SOCLE_I2C_IER,
			SOCLE_I2C_ARBIT_LOSE_INT_EN |
			SOCLE_I2C_ABNORMAL_STP_INT_DIS |
			SOCLE_I2C_BROADCAST_ADDR_INT_DIS |
			SOCLE_I2C_SLAVE_ADDR_INT_DIS |
			SOCLE_I2C_SLAVE_ACK_PERIOD_INT_DIS |
			SOCLE_I2C_SLAVE_RECV_ACK_INT_DIS |
			SOCLE_I2C_MASTER_ACK_PERIOD_INT_EN |
			SOCLE_I2C_MASTER_RECV_ACK_INT_EN,
			socle_i2c_base);

	/* Clear the slave buffer */
	memset(socle_i2c_slave_buf, 0x00, 2048);
}

extern int
i2c_transfer(struct i2c_msg *msgs, int num)
{
	int ret, i;

#ifdef DEBUG
	for (ret = 0; ret < num; ret++)
		printf("master_xfer[%d] %c, addr=0x%02x, "
		       "len=%d\n", ret, msgs[ret].flags & I2C_M_RD ?
		       'R' : 'W', msgs[ret].addr. msgs[ret].len);

#endif

	for (i = 0; i < 5; i++) {
		ret = socle_i2c_do_transfer(msgs, num);
		if (ret == num)
			return ret;
		printf("Socle I2C master: Retrying transmission (%d)\n", i);
		USDELAY(100);
	}
	return -1;
}

static int
socle_i2c_do_transfer(struct i2c_msg *msgs, int num)
{
	int ret, i, nak_ok;
	struct i2c_msg *pmsgs;
	u32 bus_stat;

	for (i = 0; i < num; i++) {
		pmsgs = &msgs[i];
		nak_ok = pmsgs->flags & I2C_M_IGNORE_NAK;
		if (!(!pmsgs->flags & I2C_M_NOSTART)) {
			if (0 == i) {
				bus_stat = socle_i2c_read(SOCLE_I2C_LSR, socle_i2c_base);
				if (SOCLE_I2C_AFTER_STR_COND_DET == (bus_stat & SOCLE_I2C_AFTER_STR_COND_DET)) {
					printf("Socle I2C master: bus is busy\n");
					ret = -1;
					goto out;
				}
			}
			ret = socle_i2c_do_address(pmsgs);
			if ((ret != 0) && !nak_ok) {
				printf("Socle I2C master: NAK from device address %2.2x msg #%d\n", msgs[i].addr, i);
				ret = -1;
				goto out;
			}
		}
		if (pmsgs->flags & I2C_M_RD) {
			/* Change the master to be as the receiver */
			socle_i2c_write(SOCLE_I2C_CONR,
					socle_i2c_read(SOCLE_I2C_CONR, socle_i2c_base) &
					~SOCLE_I2C_MASTER_TRAN_SEL,
					socle_i2c_base);

			/* Read bytes into buffer */
			ret = socle_i2c_read_bytes(pmsgs);

			if (ret < pmsgs->len)
				goto out;
		} else {
			/* Set the master to be the transmitter */
			socle_i2c_write(SOCLE_I2C_CONR,
					socle_i2c_read(SOCLE_I2C_CONR, socle_i2c_base) |
					SOCLE_I2C_MASTER_TRAN_SEL,
					socle_i2c_base);

			/* Write bytes from buffer */
			ret = socle_i2c_send_bytes(pmsgs);
			if (ret < pmsgs->len)
				goto out;
		}
	}
	ret = num;
out:
	socle_i2c_stop();
	return ret;
}

extern int
i2c_master_send(struct i2c_client *client, const char *buf, int count)
{
	int ret;
	struct i2c_msg msgs;

	msgs.addr = client->addr;
	msgs.flags = client->flags & I2C_M_TEN;
	msgs.len = count;
	msgs.buf = (char *)buf;
	ret = i2c_transfer(&msgs, 1);

	/* If everything went ok (i.e. 1 msg transmitted), return #bytes
	 * transmitted, else error code*/
	return (1 == ret)? count : ret;
}

extern int
i2c_master_recv(struct i2c_client *client, char *buf, int count)
{
	struct i2c_msg msgs;
	int ret;

	msgs.addr = client->addr;
	msgs.flags = client->flags &I2C_M_TEN;
	msgs.flags |= I2C_M_RD;
	msgs.len = count;
	msgs.buf = buf;
	ret = i2c_transfer(&msgs, 1);

	/* If everthing went ok (i.e. 1 msg transmitted), return #bytes
	 * transmitted, else error code*/
	return (1 == ret) ? count : ret;
}

static int
socle_i2c_do_address(struct i2c_msg *msgs)
{
	u16 flags = msgs->flags;
	u16 nak_ok = msgs->flags & I2C_M_IGNORE_NAK;
	u8 addr;
	int ret;

	/* Set the master to be the transmitter */
	socle_i2c_write(SOCLE_I2C_CONR,
			socle_i2c_read(SOCLE_I2C_CONR, socle_i2c_base) |
			SOCLE_I2C_MASTER_TRAN_SEL,
			socle_i2c_base);

	if (flags & I2C_M_TEN) { /* A ten bits address */
		addr = 0xf0 | ((msgs->addr >> 7) & 0x06);
		
		/* Try extended address code... */
		socle_i2c_master_write_byte(addr);
		socle_i2c_start();
		ret = socle_i2c_master_check_ack();
		if ((ret != 0) && !nak_ok)
			goto out;
			
		/* The remaining 8 bit address */
		socle_i2c_master_write_byte(msgs->addr & 0x7f);
		socle_i2c_resume();
		ret = socle_i2c_master_check_ack();
		if ((ret != 0) && !nak_ok)
			goto out;

		if (flags & I2C_M_RD) {
			/* Okay, now switch into reading mode */
			addr |= 0x01;
			socle_i2c_master_write_byte(addr);
			socle_i2c_start();
			ret = socle_i2c_master_check_ack();
			if ((ret != 0) && !nak_ok)
				goto out;
		}
	} else {		/* normal 7 bits address */
		addr = msgs->addr << 1;
		if (flags & I2C_M_RD)
			addr |= 1;
		if (flags & I2C_M_REV_DIR_ADDR)
			addr ^= 1;
		socle_i2c_master_write_byte(addr);
		socle_i2c_start();
		ret = socle_i2c_master_check_ack();
		if ((ret != 0) && !nak_ok)
			goto out;
			
	}
	return 0;
out:
	return -1;
}

static int
socle_i2c_read_bytes(struct i2c_msg *msgs)
{
	int inval;
	u16 flags = msgs->flags;
	int rdcnt = 0;
	u8 *tmp = msgs->buf;
	int cnt = msgs->len;

	while (cnt > 0) {
		socle_i2c_resume();
		inval = socle_i2c_master_read_byte();
		*tmp = inval;
		rdcnt++;
		tmp++;
		cnt--;
		if (flags & I2C_M_NO_RD_ACK)
			continue;
		if (cnt > 0) {
			socle_i2c_ack();
		} else {
			/* Neg. ack on last byte */
			socle_i2c_nak();
		}
	}
	return rdcnt;
}

static int
socle_i2c_send_bytes(struct i2c_msg *msgs)
{
	char c;
	const char *temp = msgs->buf;
	int cnt = msgs->len;
	u16 nak_ok = msgs->flags & I2C_M_IGNORE_NAK;
	int ret;
	int wrcnt = 0;

	while (cnt > 0) {
		c = *temp;
/* 		printf("Socle I2C master: sending %2.2x\n", c&0xff); */
		socle_i2c_master_write_byte(c);
		socle_i2c_resume();
		ret = socle_i2c_master_check_ack();
		if ((ret != 0) && !nak_ok) {
			printf("Socle I2C master: error - bailout\n");
			return -1;
		} else {
			cnt--;
			temp++;
			wrcnt++;
		}
	}
	return wrcnt;
}

static int addr_10_bits_str = 0;
static int gen_call_addr_str = 0;

static void 
socle_i2c_isr(void *param)
{
	u32 tmp;
	u8 ack_stat, data;

	tmp = socle_i2c_read(SOCLE_I2C_ISR, socle_i2c_base);

	if (SOCLE_I2C_ARBIT_LOSE_INT == (SOCLE_I2C_ARBIT_LOSE_INT & tmp)) {
		printf("Socle I2C master: arbitration lose occurs\n");

		/* Clear interrupt */
		socle_i2c_write(SOCLE_I2C_ISR, tmp & (~SOCLE_I2C_ARBIT_LOSE_INT), socle_i2c_base);
	}
	if (SOCLE_I2C_ABNORMAL_STP_INT == (SOCLE_I2C_ABNORMAL_STP_INT & tmp)) {
		printf("Socle I2C slave: abnormal stop occurs\n");

		/* Clear interrupt */
		socle_i2c_write(SOCLE_I2C_ISR, tmp & (~SOCLE_I2C_ABNORMAL_STP_INT), socle_i2c_base);
	}
	if (SOCLE_I2C_BROADCAST_ADDR_INT == (SOCLE_I2C_BROADCAST_ADDR_INT & tmp)) {
/* 		printf("Socle I2C slave: broadcast address matches\n"); */

		/* Clear interrupt */
		socle_i2c_write(SOCLE_I2C_ISR, tmp & (~SOCLE_I2C_BROADCAST_ADDR_INT), socle_i2c_base);

		gen_call_addr_str = 1;

		/* Reply ACK */
		socle_i2c_ack();

		socle_i2c_resume();		
	}
	if (SOCLE_I2C_SLAVE_ADDR_INT == (SOCLE_I2C_SLAVE_ADDR_INT & tmp)) {
		u32 opr;

/* 		printf("Socle I2C slave: slave address matches\n"); */

		socle_i2c_slave_buf_idx = 0;		
	
		/* Clear the interrupt */
		socle_i2c_write(SOCLE_I2C_ISR, tmp & (~SOCLE_I2C_SLAVE_ADDR_INT), socle_i2c_base);

		data = socle_i2c_read(SOCLE_I2C_SRXR, socle_i2c_base);
/* 		printf("Socle I2C slave: data is 0x%02x\n", data); */

		opr = socle_i2c_read(SOCLE_I2C_OPR, socle_i2c_base);
		if ((SOCLE_I2C_ADDR_10_BITS == (opr & SOCLE_I2C_ADDR_10_BITS)) && 
		    !(data & 0x01))
			addr_10_bits_str = 1;

		if (data & 0x01) {
			/* Change the slave to be as the transmitter */
			socle_i2c_write(SOCLE_I2C_CONR,
					socle_i2c_read(SOCLE_I2C_CONR, socle_i2c_base) |
					SOCLE_I2C_SLAVE_TRAN_SEL,
					socle_i2c_base);

			/* Write next data */
			socle_i2c_write(SOCLE_I2C_STXR, 
					socle_i2c_slave_buf[socle_i2c_slave_buf_idx++],
					socle_i2c_base);

/* 			printf("Socle I2C slave: data is 0x%02x\n", socle_i2c_slave_buf[socle_i2c_slave_buf_idx-1]); */
		}

		/* Reply ACK */
		socle_i2c_ack();

		socle_i2c_resume();		
	}
	if (SOCLE_I2C_SLAVE_ACK_PERIOD_INT == (SOCLE_I2C_SLAVE_ACK_PERIOD_INT & tmp)) {
		u32 opr;

/* 		printf("Socle I2C slave: ACK period interrupt generation\n"); */

		/* Clear interrupt */
		socle_i2c_write(SOCLE_I2C_ISR, 
				tmp & ~SOCLE_I2C_SLAVE_ACK_PERIOD_INT, 
				socle_i2c_base);

		opr = socle_i2c_read(SOCLE_I2C_OPR, socle_i2c_base);
		if ((SOCLE_I2C_ADDR_10_BITS == (opr & SOCLE_I2C_ADDR_10_BITS)) && 
		    addr_10_bits_str) {
			u32 recv_addr;
			u32 slave_addr;

			recv_addr= socle_i2c_read(SOCLE_I2C_SRXR, socle_i2c_base);
			slave_addr = socle_i2c_read(SOCLE_I2C_SADDR, socle_i2c_base);
			slave_addr &= 0x7f;
			if (recv_addr == slave_addr)
				/* Reply ACK */
				socle_i2c_ack();
			else
				/* Reply NAK */
				socle_i2c_nak();
			addr_10_bits_str = 0;
		} else if (gen_call_addr_str) {
			u32 buf;

			buf = socle_i2c_read(SOCLE_I2C_SRXR, socle_i2c_base);
			if (0x06 == buf)
				printf("Socle I2C slave: Reset and write programmable part of slave address by hardware\n");
			else if (0x04 == buf)
				printf("Socle I2C slave: write programmable part of slave address by hardware\n");
			else if (buf & 0x01)
				printf("Socle I2C slave: the address of master is 0x%02x\n", (buf>>1)&0x7f);
			else
				printf("Socle I2C slave: unknown value 0x%02x\n", buf);
			gen_call_addr_str = 0;

			/* Reply ACK */
			socle_i2c_ack();

		} else {
			socle_i2c_slave_buf[socle_i2c_slave_buf_idx++] = socle_i2c_read(SOCLE_I2C_SRXR, socle_i2c_base);
			
			/* Reply ACK */
			socle_i2c_ack();
		}

/* 		printf("Socle I2C slave: data is 0x%02x\n", socle_i2c_slave_buf[socle_i2c_slave_buf_idx-1]); */

		socle_i2c_resume();
	}
	if (SOCLE_I2C_SLAVE_RECV_ACK_INT == (SOCLE_I2C_SLAVE_RECV_ACK_INT & tmp)) {
/* 		printf("Socle I2C slave: receive ACK interrupt generation\n"); */

		/* Clear interrupt */
		socle_i2c_write(SOCLE_I2C_ISR, tmp & (~SOCLE_I2C_SLAVE_RECV_ACK_INT), socle_i2c_base);

		/* Write next data */
		socle_i2c_write(SOCLE_I2C_STXR, 
				socle_i2c_slave_buf[socle_i2c_slave_buf_idx++],
				socle_i2c_base);

/* 		printf("Socle I2C slave: data is 0x%02x\n", socle_i2c_slave_buf[socle_i2c_slave_buf_idx-1]); */

		/* Check ACK status */
		ack_stat = socle_i2c_read(SOCLE_I2C_LSR, socle_i2c_base);
		if (SOCLE_I2C_RECV_STAT_NAK == (ack_stat & SOCLE_I2C_RECV_STAT_NAK)) {
			socle_i2c_slave_buf_idx = 0; /* restore the buffer idx because master terminate the transfer */

			/* Set the slave to be as a receiver */
			socle_i2c_write(SOCLE_I2C_CONR,
					socle_i2c_read(SOCLE_I2C_CONR, socle_i2c_base) &
					~SOCLE_I2C_SLAVE_TRAN_SEL,
					socle_i2c_base);
		}

		socle_i2c_resume();
	}
	if (SOCLE_I2C_MASTER_ACK_PERIOD_INT == (SOCLE_I2C_MASTER_ACK_PERIOD_INT & tmp)) {
/* 		printf("Socle I2C master: ACK period interrupt generation\n"); */

		/* Clear interrupt */
		socle_i2c_write(SOCLE_I2C_ISR, tmp & (~SOCLE_I2C_MASTER_ACK_PERIOD_INT), socle_i2c_base);

		int_master_ack_period = 1;
	}
	if (SOCLE_I2C_MASTER_RECV_ACK_INT == (SOCLE_I2C_MASTER_RECV_ACK_INT & tmp)) {
/* 		printf("Socle I2C master: receive ACK interrupt generation\n"); */

		/* Clear interrupt */
		socle_i2c_write(SOCLE_I2C_ISR, tmp & (~SOCLE_I2C_MASTER_RECV_ACK_INT), socle_i2c_base);

		int_master_recv_ack = 1;
	}
}

static u32
socle_i2c_power(u32 base, u32 exp)
{
	u32 i;
	u32 val = 1;
	
	if (0 == exp)
		return 1;
	else {
		for (i = 0; i < exp; i++)
			val *= base;
		return val;
	}
}

static u32
socle_i2c_calculate_divisor(u32 clk)
{
	u8 div_high_3 = 0, div_low_3 = 0, i2c_cdvr = 0;
	u32 sclk_divisor, sclk, pclk, power;

	/*
	 *  SCL Divisor = (I2CCDVR[5:3] + 1) * 2 power(I2CCDVR[2:0] + 1
	 *  SCL = PCLK / 5 * SCLK Divisor
	 *  */

	pclk = socle_get_apb_clock();
	while (1) {
		power = socle_i2c_power(2, div_low_3+1);
		for (div_high_3 = 0; div_high_3 < 8; div_high_3++) {
			sclk_divisor = (div_high_3 + 1) * power;
			sclk = pclk / (5 * sclk_divisor);
			if (sclk < clk)
				goto out;
		}
		div_low_3++;
	}
out:
	i2c_cdvr = (div_high_3 << 3) | div_low_3;
	printf("sclk is %d, divisor is 0x%08x\n", sclk, i2c_cdvr);
	return i2c_cdvr;
}

static void
socle_i2c_ack(void)
{
	u32 tmp;

	tmp = socle_i2c_read(SOCLE_I2C_CONR, socle_i2c_base);
	tmp &= ~SOCLE_I2C_BUS_ACK_DIS;
	socle_i2c_write(SOCLE_I2C_CONR, tmp, socle_i2c_base);
}

static void
socle_i2c_nak(void)
{
	u32 tmp;
	
	tmp = socle_i2c_read(SOCLE_I2C_CONR, socle_i2c_base);
	tmp |= SOCLE_I2C_BUS_ACK_DIS;
	socle_i2c_write(SOCLE_I2C_CONR, tmp, socle_i2c_base);
}

static void
socle_i2c_master_write_byte(u8 data)
{
	socle_i2c_write(SOCLE_I2C_MTXR, data, socle_i2c_base);
}

static int 
socle_i2c_master_read_byte(void)
{
	int data;

	/* Wait for the master ack period interrupt occuring */
	while (!int_master_ack_period)
		/* NOP */;
	data = socle_i2c_read(SOCLE_I2C_MRXR, socle_i2c_base);
	int_master_ack_period = 0;
	return data;
}

static int
socle_i2c_master_check_ack(void)
{
	u32 ack_stat;

	/* Wait for the master receive ack interrupt occuring */
	while (!int_master_recv_ack)
		/* NOP */;
	int_master_recv_ack = 0;
	
	/* Check the ack status */
	ack_stat = socle_i2c_read(SOCLE_I2C_LSR, socle_i2c_base);
	if (SOCLE_I2C_RECV_STAT_NAK == (ack_stat & SOCLE_I2C_RECV_STAT_NAK))
		return -1;
	else
		return 0;
}

static void
socle_i2c_start(void)
{
	socle_i2c_write(SOCLE_I2C_LCMR,
			SOCLE_I2C_RESUME_COND_GEN_EN |
			SOCLE_I2C_STP_COND_GEN_DIS |
			SOCLE_I2C_STR_COND_GEN_EN,
			socle_i2c_base);
}

static void
socle_i2c_stop(void)
{
	socle_i2c_write(SOCLE_I2C_LCMR,
			SOCLE_I2C_RESUME_COND_GEN_EN |
			SOCLE_I2C_STP_COND_GEN_EN |
			SOCLE_I2C_STR_COND_GEN_DIS,
			socle_i2c_base);
}

static void
socle_i2c_resume(void)
{
	socle_i2c_write(SOCLE_I2C_LCMR,
			SOCLE_I2C_RESUME_COND_GEN_EN |
			SOCLE_I2C_STP_COND_GEN_DIS |
			SOCLE_I2C_STR_COND_GEN_DIS,
			socle_i2c_base);

}


