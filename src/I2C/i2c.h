#ifndef __I2C_H_INCLUDED
#define __I2C_H_INCLUDED

#include <type.h>
#include <io.h>
#define I2C_NAME_SIZE 50

/*
 *  i2c_client identifies a single device (i.e. chip) that is connected to an
 *  i2c bus. The behaviour is defined by the routines of the driver. This
 *  function is mainly used for lookup & other admin. functions
 *  */
struct i2c_client {
	unsigned int flags;	/* div., see below */
	unsigned short addr;	/* chip address - NOTE: 7bit
				 * addresses are stored in the
				 * _LOWER_ 7 bits*/
};

/*
 *  I2C Message - used for pure i2c transaction, also from /dev interface
 *  */
struct i2c_msg {
	u16 addr;		/* slave address */
	u16 flags;
#define I2C_M_TEN 0x10		/* we have a ten bit chip address */
#define I2C_M_RD 0x01
#define I2C_M_NOSTART 0x40000
#define I2C_M_REV_DIR_ADDR 0x2000
#define I2C_M_IGNORE_NAK 0x1000
#define I2C_M_NO_RD_ACK 0x0800
	u16 len;		/* msg length */
	u8 *buf;		/* pointer to msg data */
};

/*
 *  The master routines are the ones normally used to transmit data to devices
 *  on a bus (or read from them). Apart from two basic transfer functions to
 *  transmit one message at a time, a more complex version can be used to
 *  transmit an arbitrary number of messages without interruption
 *  */

extern void i2c_master_initialize(u32 ip_mem_base, u32 ip_irq);
extern int i2c_master_send(struct i2c_client *client, const char *buf, int count);
extern int i2c_master_recv(struct i2c_client *client, char *buf, int count);

/*
 *  Transfer num messages
 *  */
extern int i2c_transfer(struct i2c_msg *msgs, int num);

static void inline
socle_i2c_write(u32 reg, u32 val, u32 base)
{
	base += reg;
	iowrite32(val, base);
}

static u32 inline
socle_i2c_read(u32 reg, u32 base)
{
	u32 val;

	base += reg;
	val = ioread32(base);
	return val;
}

#endif
