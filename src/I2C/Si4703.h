#ifndef _SI4703_H_
#define _SI4703_H_

#include <type.h>

#define SI4703_SLAVE_ADDR		0x10

#define SI4703_DEVICEID			0x1242
#define SI4703_CHIPID			0x1253

/* Register */
#define SI4703_REG_DEVICEID		0x00
#define SI4703_REG_CHIPID			0x01
#define SI4703_REG_POWERCFG		0x02
#define SI4703_REG_CHANNEL			0x03
#define SI4703_REG_SYSCONFIG1		0x04
#define SI4703_REG_SYSCONFIG2		0x05
#define SI4703_REG_SYSCONFIG3		0x06
#define SI4703_REG_TEST1			0x07
#define SI4703_REG_TEST2			0x08
#define SI4703_REG_BOOTCONFIG		0x09
#define SI4703_REG_STATUSRSSI		0x0a
#define SI4703_REG_READCHAN		0x0b
#define SI4703_REG_RDSA			0x0c
#define SI4703_REG_RDSB			0x0d
#define SI4703_REG_RDSC			0x0e
#define SI4703_REG_RDSD			0x0f

// SI4703_REG_POWERCFG (0x02)
#define POWERCFG_DSMUTE		(1 << 15)
#define POWERCFG_DMUTE		(1 << 14)
#define POWERCFG_MONO			(1 << 13)
#define POWERCFG_RDSM			(1 << 11)
#define POWERCFG_SKMODE		(1 << 10)
#define POWERCFG_SEEKUP		(1 << 9)
#define POWERCFG_SEEK			(1 << 8)
#define POWERCFG_DISABLE		(1 << 6)
#define POWERCFG_ENABLE		(1 << 0)

// SI4703_REG_CHANNEL (0x03)
#define CHANNEL_TUNE			(1 << 15)
#define CHANNEL_CHAN(x)		((x) & 0x3ff)

// SI4703_REG_SYSCONFIG2 (0x05)
#define SYSCONFIG2_SEEKTH(x)			(((x) & 0xff) << 8)
#define SYSCONFIG2_BAND(x)				(((x) & 0x3) << 6)
#define SYSCONFIG2_SPACE(x)			(((x) & 0x3) << 4)
#define SYSCONFIG2_VOLUME(x)			((x) & 0xf)

// SI4703_REG_SYSCONFIG3 (0x06)
#define SYSCONFIG3_SMUTER(x)			(((x) & 0x3) << 14)
#define SYSCONFIG3_SMUTEA(x)			(((x) & 0x3) << 12)
#define SYSCONFIG3_VOLEXT				(1 << 8)
#define SYSCONFIG3_SKSNR(x)			(((x) & 0xf) << 4)
#define SYSCONFIG3_SKCNT(x)			((x) & 0xf)

// SI4703_REG_READCHAN (0x0b)
#define READCHAN_BLERB				(3 << 14)
#define READCHAN_BLERC				(3 << 12)
#define READCHAN_BLERD				(3 << 10)
#define READCHAN_READCHAN(x)		((x) & 0x3ff)

// SI4703_REG_STATUSRSSI (0x0a)
#define STATUSRSSI_RDSR			(1 << 15)
#define STATUSRSSI_STC				(1 << 14)
#define STATUSRSSI_SF_BL			(1 << 13)
#define STATUSRSSI_AFCRL			(1 << 12)
#define STATUSRSSI_RDSS			(1 << 11)
#define STATUSRSSI_BLERA			(3 << 9)
#define STATUSRSSI_ST				(1 << 8)
#define STATUSRSSI_RSSI(x)			((x) & 0xff)

struct si4703_struct {
	u8 reg_02_h;
	u8 reg_02_l;
	u8 reg_03_h;
	u8 reg_03_l;
	u8 reg_04_h;
	u8 reg_04_l;
	u8 reg_05_h;
	u8 reg_05_l;
	u8 reg_06_h;
	u8 reg_06_l;
	u8 reg_07_h;
	u8 reg_07_l;
	u8 reg_08_h;
	u8 reg_08_l;
	u8 reg_09_h;
	u8 reg_09_l;
	u8 reg_0a_h;
	u8 reg_0a_l;
	u8 reg_0b_h;
	u8 reg_0b_l;
	u8 reg_0c_h;	// arg0, resp0
	u8 reg_0c_l;		// arg1, resp1
	u8 reg_0d_h;	// arg2, resp2
	u8 reg_0d_l;		// arg3, resp3
	u8 reg_0e_h;	// arg4, resp4
	u8 reg_0e_l;		// arg5, resp5
	u8 reg_0f_h;		// arg6, resp6
	u8 reg_0f_l;		// cmd, status
	u8 reg_00_h;
	u8 reg_00_l;
	u8 reg_01_h;
	u8 reg_01_l;
};

#define TO_REG_16(h, l)		((h) << 8 | (l))
#define TO_REG_8_l(x)		((x) & 0xff)
#define TO_REG_8_h(x)		((x) >> 8)

#endif //_SI4703_H_
