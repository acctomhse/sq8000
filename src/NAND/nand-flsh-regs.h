#ifndef __SOCLE_NAND_REGS_H_INCLUDED
#define __SOCLE_NAND_REGS_H_INCLUDED

#include <type.h>
#include <io.h>

/*
 *  Registers for NAND flash host controller
 *  */
#define SOCLE_NAND_FLSH_CONF 0xC000 /* Flash type configuration register */
#define SOCLE_NAND_FLSH_COMM_1 0xC004 /* First Command input register */
#define SOCLE_NAND_FLSH_COMM_2 0xC008 /* Second Command input register */
#define SOCLE_NAND_FLSH_STATE_COMM 0xC00C /* Second Command input register */
#define SOCLE_NAND_FLSH_ADDRESS_1 0xC010 /* 1st Cycle address input register */
#define SOCLE_NAND_FLSH_ADDRESS_2 0xC014 /* 2nd Cycle address input register */
#define SOCLE_NAND_FLSH_ADDRESS_3 0xC018 /* 3rd Cycle address input regiater */
#define SOCLE_NAND_FLSH_ADDRESS_4 0xC01C /* 4th Cycle address input register */
#define SOCLE_NAND_FLSH_ADDRESS_5 0xC020 /* 5th Cycle address input register */
#define SOCLE_NAND_FLSH_DATA 0xC024 /* Access Data register */
#define SOCLE_NAND_FLSH_BUFF_STADDR 0xC028 /* Internal buffer start address register */
#define SOCLE_NAND_FLSH_BUFF_CNT 0xC02C	/* Internal bufer data count register */
#define SOCLE_NAND_FLSH_BUFF_STATE 0xC030 /* Internal buffer data count register */
#define SOCLE_NAND_FLSH_DMA_SET 0xC034 /* DMA operation setting register */
#define SOCLE_NAND_FLSH_CE_WP 0xC038 /* Chip enable/Write protect register */
#define SOCLE_NAND_FLSH_CONTROL 0xC03C /* NAND Flash Controller control register */
#define SOCLE_NAND_FLSH_RESET 0xC040 /* NAND Flash Controller control register */
#define SOCLE_NAND_FLSH_STATE 0xC044 /* NAND Flash Controller status register */
#define SOCLE_NAND_FLSH_INT_MASK 0xC048	/* NAND Flash Controller interrupt mask register */
#define SOCLE_NAND_FLSH_INT_STATE 0xC04C /* NAND Flash Controller interrupt status register */
#define SOCLE_NAND_FLSH_GPIO 0xC050 /* GPIO control register */
#define SOCLE_NAND_FLSH_S_NUM 0xC054 /* Flash data serial number register */
#define SOCLE_NAND_FLSH_1ST_ECC_1 0xC058 /* 1st 512 byte ECC_1 code register */
#define SOCLE_NAND_FLSH_1ST_ECC_2 0xC05C /* 1st 512 byte ECC_2 code register */
#define SOCLE_NAND_FLSH_1ST_ECC_3 0xC060 /* 1st 512 byte ECC_3 code register */
#define SOCLE_NAND_FLSH_1ST_ECC_4 0xC064 /* 1st 512 byte ECC_4 code register */
#define SOCLE_NAND_FLSH_2ND_ECC_1 0xC068 /* 2ND 512 byte ECC_1 code register */
#define SOCLE_NAND_FLSH_2ND_ECC_2 0xC06C /* 2ND 512 byte ECC_2 code register */
#define SOCLE_NAND_FLSH_2ND_ECC_3 0xC070 /* 2ND 512 byte ECC_3 code register */
#define SOCLE_NAND_FLSH_2ND_ECC_4 0xC074 /* 2ND 512 byte ECC_4 code register */
#define SOCLE_NAND_FLSH_3RD_ECC_1 0xC078 /* 3RD 512 byte ECC_1 code register */
#define SOCLE_NAND_FLSH_3RD_ECC_2 0xC07C /* 3RD 512 byte ECC_2 code register */
#define SOCLE_NAND_FLSH_3RD_ECC_3 0xC080 /* 3RD 512 byte ECC_3 code register */
#define SOCLE_NAND_FLSH_3RD_ECC_4 0xC084 /* 3RD 512 byte ECC_4 code register */
#define SOCLE_NAND_FLSH_4TH_ECC_1 0xC088 /* 4TH 512 byte ECC_1 code register */
#define SOCLE_NAND_FLSH_4TH_ECC_2 0xC08C /* 4TH 512 byte ECC_2 code register */
#define SOCLE_NAND_FLSH_4TH_ECC_3 0xC090 /* 4TH 512 byte ECC_3 code register */
#define SOCLE_NAND_FLSH_4TH_ECC_4 0xC094 /* 4TH 512 byte ECC_4 code register */
#define SOCLE_NAND_FLSH_1ST_SYNDR_1 0xC098 /* 1st 512 byte Syndrome_1 code register. (Big page only */
#define SOCLE_NAND_FLSH_1ST_SYNDR_2 0xC09C /* 1st 512 byte Syndrome_2 code register */
#define SOCLE_NAND_FLSH_1ST_SYNDR_3 0xC0A0 /* 1st 512 byte Syndrome_3 code register */
#define SOCLE_NAND_FLSH_1ST_SYNDR_4 0xC0A4 /* 1st 512 byte Syndrom3_4 code register */
#define SOCLE_NAND_FLSH_2ND_SYNDR_1 0xC0A8 /* 1st 512 byte Syndrome_1 code register. (Big page only */
#define SOCLE_NAND_FLSH_2ND_SYNDR_2 0xC0AC /* 1st 512 byte Syndrome_2 code register */
#define SOCLE_NAND_FLSH_2ND_SYNDR_3 0xC0B0 /* 1st 512 byte Syndrome_3 code register */
#define SOCLE_NAND_FLSH_2ND_SYNDR_4 0xC0B4 /* 1st 512 byte Syndrom3_4 code register */
#define SOCLE_NAND_FLSH_3RD_SYNDR_1 0xC0B8 /* 1st 512 byte Syndrome_1 code register. (Big page only */
#define SOCLE_NAND_FLSH_3RD_SYNDR_2 0xC0BC /* 1st 512 byte Syndrome_2 code register */
#define SOCLE_NAND_FLSH_3RD_SYNDR_3 0xC0C0 /* 1st 512 byte Syndrome_3 code register */
#define SOCLE_NAND_FLSH_3RD_SYNDR_4 0xC0C4 /* 1st 512 byte Syndrom3_4 code register */
#define SOCLE_NAND_FLSH_4TH_SYNDR_1 0xC0C8 /* 1st 512 byte Syndrome_1 code register. (Big page only */
#define SOCLE_NAND_FLSH_4TH_SYNDR_2 0xC0CC /* 1st 512 byte Syndrome_2 code register */
#define SOCLE_NAND_FLSH_4TH_SYNDR_3 0xC0D0 /* 1st 512 byte Syndrome_3 code register */
#define SOCLE_NAND_FLSH_4TH_SYNDR_4 0xC0D4 /* 1st 512 byte Syndrom3_4 code register */

/* SOCLE_NAND buffer */
#define SOCLE_NAND_FLSH_BUF_ADDR(x) ((x) + 0x8000)

/*
 *  SOCLE_NAND_FLSH_CONF
 *  */
/* tWC: Write Cycle time */
#define SOCLE_NAND_FLSH_tWC_CYC_0 (0x0 << 17) /* 0cycle */
#define SOCLE_NAND_FLSH_tWC_CYC_1 (0x1 << 17) /* 1cycle */
#define SOCLE_NAND_FLSH_tWC_CYC_2 (0x2 << 17) /* 2cycle */
#define SOCLE_NAND_FLSH_tWC_CYC_3 (0x3 << 17) /* 3cycle */
#define SOCLE_NAND_FLSH_tWC_CYC_4 (0x4 << 17) /* 4cycle */
#define SOCLE_NAND_FLSH_tWC_CYC_5 (0x5 << 17) /* 5cycle */
#define SOCLE_NAND_FLSH_tWC_CYC_6 (0x6 << 17) /* 6cycle */
#define SOCLE_NAND_FLSH_tWC_CYC_7 (0x7 << 17) /* 7cycle */
#define SOCLE_NAND_FLSH_tWC_CYC_8 (0x8 << 17) /* 8cycle */
#define SOCLE_NAND_FLSH_tWC_CYC_9 (0x9 << 17) /* 9cycle */
#define SOCLE_NAND_FLSH_tWC_CYC_10 (0xA << 17) /* 10cycle */
#define SOCLE_NAND_FLSH_tWC_CYC_11 (0xB << 17) /* 11cycle */
#define SOCLE_NAND_FLSH_tWC_CYC_12 (0xC << 17) /* 12cycle */
#define SOCLE_NAND_FLSH_tWC_CYC_13 (0xD << 17) /* 13cycle */
#define SOCLE_NAND_FLSH_tWC_CYC_14 (0xE << 17) /* 14cycle */
#define SOCLE_NAND_FLSH_tWC_CYC_15 (0xF << 17) /* 15cycle (default)*/

/* tWP: Write Pulse time, duration of write pulse */
#define SOCLE_NAND_FLSH_tWP_CYC_0 (0x0 << 14) /* 0cycle */
#define SOCLE_NAND_FLSH_tWP_CYC_1 (0x1 << 14) /* 1cycle */
#define SOCLE_NAND_FLSH_tWP_CYC_2 (0x2 << 14) /* 2cycle */
#define SOCLE_NAND_FLSH_tWP_CYC_3 (0x3 << 14) /* 3cycle */
#define SOCLE_NAND_FLSH_tWP_CYC_4 (0x4 << 14) /* 4cycle */
#define SOCLE_NAND_FLSH_tWP_CYC_5 (0x5 << 14) /* 5cycle */
#define SOCLE_NAND_FLSH_tWP_CYC_6 (0x6 << 14) /* 6cycle */
#define SOCLE_NAND_FLSH_tWP_CYC_7 (0x7 << 14) /* 7cycle (default)*/

/* tRC: Read Cycle time */
#define SOCLE_NAND_FLSH_tRC_CYC_0 (0x0 << 10) /* 0cycle */
#define SOCLE_NAND_FLSH_tRC_CYC_1 (0x1 << 10) /* 1cycle */
#define SOCLE_NAND_FLSH_tRC_CYC_2 (0x2 << 10) /* 2cycle */
#define SOCLE_NAND_FLSH_tRC_CYC_3 (0x3 << 10) /* 3cycle */
#define SOCLE_NAND_FLSH_tRC_CYC_4 (0x4 << 10) /* 4cycle */
#define SOCLE_NAND_FLSH_tRC_CYC_5 (0x5 << 10) /* 5cycle */
#define SOCLE_NAND_FLSH_tRC_CYC_6 (0x6 << 10) /* 6cycle */
#define SOCLE_NAND_FLSH_tRC_CYC_7 (0x7 << 10) /* 7cycle */
#define SOCLE_NAND_FLSH_tRC_CYC_8 (0x8 << 10) /* 8cycle */
#define SOCLE_NAND_FLSH_tRC_CYC_9 (0x9 << 10) /* 9cycle */
#define SOCLE_NAND_FLSH_tRC_CYC_10 (0xA << 10) /* 10cycle */
#define SOCLE_NAND_FLSH_tRC_CYC_11 (0xB << 10) /* 11cycle */
#define SOCLE_NAND_FLSH_tRC_CYC_12 (0xC << 10) /* 12cycle */
#define SOCLE_NAND_FLSH_tRC_CYC_13 (0xD << 10) /* 13cycle */
#define SOCLE_NAND_FLSH_tRC_CYC_14 (0xE << 10) /* 14cycle */
#define SOCLE_NAND_FLSH_tRC_CYC_15 (0xF << 10) /* 15cycle (default)*/

/* tRP: Read Pulse time, duration of read pulse */
#define SOCLE_NAND_FLSH_tRP_CYC_0 (0x0 << 7) /* 0cycle */
#define SOCLE_NAND_FLSH_tRP_CYC_1 (0x1 << 7) /* 1cycle */
#define SOCLE_NAND_FLSH_tRP_CYC_2 (0x2 << 7) /* 2cycle */
#define SOCLE_NAND_FLSH_tRP_CYC_3 (0x3 << 7) /* 3cycle */
#define SOCLE_NAND_FLSH_tRP_CYC_4 (0x4 << 7) /* 4cycle */
#define SOCLE_NAND_FLSH_tRP_CYC_5 (0x5 << 7) /* 5cycle */
#define SOCLE_NAND_FLSH_tRP_CYC_6 (0x6 << 7) /* 6cycle */
#define SOCLE_NAND_FLSH_tRP_CYC_7 (0x7 << 7) /* 7cycle (default)*/

/* NAND-Flash memory page size */
#define SOCLE_NAND_FLSH_PAGE_512B (0x0 << 3) /* 512 bytes (default) */
#define SOCLE_NAND_FLSH_PAGE_2KB (0x1 << 3) /* 2K bytes */

/*
 *  SOCLE_NAND_FLSH_COMM
 *  */
/* Command valid bit (set by SW, clear by H/W) */
#define SOCLE_NAND_FLSH_CMD_N_VALID 0x0	/* command not valid */
#define SOCLE_NAND_FLSH_CMD_VALID (0x1 << 8) /* command is valid */
#define SOCLE_NAND_FLSH_CMD_INPUT(x) ((x) & 0x0000000FF) /* NAND flash memory commmand input port */

/*
 *  SOCLE_NAND_FLSH_ADDRESS
 *  */
/* Address valid Bit (set by S/W, clear by H/W) */
#define SOCLE_NAND_FLSH_ADDR_N_VALID 0x0 /* Address not valid */
#define SOCLE_NAND_FLSH_ADDR_VALID (0x1 << 8) /* Address is valid */
#define SOCLE_NAND_FLSH_ADDR_INPUT(x) ((x) & 0x000000FF) /* NAND Flash memory address input port */

/*
 *  SOCLE_NAND_FLSH_BUFF_STADDR
 *  */
#define SOCLE_NAND_FLSH_INTER_BUF_STR_ADDR(x) ((x) & 0x0000FFFF) /* internal buffer start address */

/*
 *  SOCLE_NAND_FLSH_BUFF_CNT
 *  */
/* Data access with/without ECC code */
#define SOCLE_NAND_FLSH_RW_WITHOUT_ECC 0x0 /* access without ecc code */
#define SOCLE_NAND_FLSH_RW_WITH_ECC (0x1 << 16) /* access with ecc code */
#define SOCLE_NAND_FLSH_INTER_BUF_DATA_NUM(x) ((x) & 0x0000FFFF) /* internal buffer read/write data number */
//leonid+ for support 8/15 bytes parity length
#define SOCLE_NAND_FLSH_NPAR_LEN_8B	0x0			/* ECC Parity length 8 bytes	*/
#define SOCLE_NAND_FLSH_NPAR_LEN_15B	(0x1 << 17)	/* ECC Parity length 15 bytes	*/
//#define SOCLE_NAND_FLSH_FIRST4_LEN_15B			/*define first 4 pages ECC Parity length 15B, or 8B	*/


/*
 *  SOCLE_NAND_FLSH_DMA_SET
 *  */
/* Endan mode set */
#define SOCLE_NAND_FLSH_LITTLE_ENDING 0x0 /* little ending */
#define SOCLE_NAND_FLSH_BIG_ENDING (0x1 << 11) /* big ending */

/* ECC test mode */
#define SOCLE_NAND_FLSH_ECC_TESTMODE_DIS 0x0 /* disable ecc codec test mode, data path is normal */
#define SOCLE_NAND_FLSH_ECC_TESTMODE_EN (0x1 << 10) /* enable ecc codec test mode, data path not to NAND Flash */

/* Buffer Read/Write Enable */
#define SOCLE_NAND_FLSH_BUF_RW_DIS 0x0 /* disable buffer read/write */
#define SOCLE_NAND_FLSH_BUF_RW_EN (0x1 << 9) /* enable buffer read/write */

/* Second Command Input */
#define SOCLE_NAND_FLSH_SEC_CMD_N 0x0 /* no second command input */
#define SOCLE_NAND_FLSH_SEC_CMD_Y (0x1 << 8) /* second command input */

/* Auto Status Check Enable */
#define SOCLE_NAND_FLSH_AUTO_STAT_CHK_DIS 0x0 /* disable auto status check */
#define SOCLE_NAND_FLSH_AUTO_STAT_CHK_EN (0x1 << 7) /* enable auto status check */

/* Access Data Path Select */
#define SOCLE_NAND_FLSH_ACCESS_DATA_INTER_BUF 0x0 /* access data from/to internal buffer */
#define SOCLE_NAND_FLSH_ACCESS_DATA_REG_PORT (0x1 << 6) /* access data from/to register data port */

/* DMA Operation Type */
#define SOCLE_NAND_FLSH_DMA_NOP 0x0	/* no data access */
#define SOCLE_NAND_FLSH_DMA_READ (0x1 << 4) /* read operation */
#define SOCLE_NAND_FLSH_DMA_WRITE (0x2 << 4) /* write operation */

/* Device Wait R/B Enable */
#define SOCLE_NAND_FLSH_DEV_N_WAIT_RB 0x0 /* not wait R/B */
#define SOCLE_NAND_FLSH_DEV_WAIT_RB 0x1 /* wait R/B */


/*
 *  SOCLE_NAND_FLSH_CE_WP
 *  */
/* NAND Flash Device 3 Write Protect */
#define SOCLE_NAND_FLSH_DEV_3_WP_DIS 0x0 /* disable write protect */
#define SOCLE_NAND_FLSH_DEV_3_WP_EN (0x1 << 7) /* enable write protect */

/* NAND Flash Device 2 Write Protect */
#define SOCLE_NAND_FLSH_DEV_2_WP_DIS 0x0 /* disable write protect */
#define SOCLE_NAND_FLSH_DEV_2_WP_EN (0x1 << 6) /* enable write protect */

/* NAND Flash Device 1 Write Protect */
#define SOCLE_NAND_FLSH_DEV_1_WP_DIS 0x0 /* disable write protect */
#define SOCLE_NAND_FLSH_DEV_1_WP_EN (0x1 << 5) /* enable write protect */

/* NAND Flash Device 0 Write Protect */
#define SOCLE_NAND_FLSH_DEV_0_WP_DIS 0x0 /* disable write protect */
#define SOCLE_NAND_FLSH_DEV_0_WP_EN (0x1 << 4) /* enable write protect */

/* NAND Flash Device 3 Chip Enable */
#define SOCLE_NAND_FLSH_DEV_3_CHIP_DIS 0x0 /* chip disable */
#define SOCLE_NAND_FLSH_DEV_3_CHIP_EN (0x1 << 3) /* chip enable */

/* NAND Flash Device 2 Chip Enable */
#define SOCLE_NAND_FLSH_DEV_2_CHIP_DIS 0x0 /* chip disable */
#define SOCLE_NAND_FLSH_DEV_2_CHIP_EN (0x1 << 2) /* chip enable */

/* NAND Flash Device 1 Chip Enable */
#define SOCLE_NAND_FLSH_DEV_1_CHIP_DIS 0x0 /* chip disable */
#define SOCLE_NAND_FLSH_DEV_1_CHIP_EN (0x1 << 1) /* chip enable */

/* NAND Flash Device 0 Chip Enable */
#define SOCLE_NAND_FLSH_DEV_0_CHIP_DIS 0x0 /* chip disable */
#define SOCLE_NAND_FLSH_DEV_0_CHIP_EN 0x1 /* chip enable */

/*
 *  SOCLE_NAND_FLSH_CONTROL
 *  */
/* Flash DMA Enable */
#define SOCLE_NAND_FLSH_DMA_DIS 0x0	/* disable flash DMA */
#define SOCLE_NAND_FLSH_DMA_EN (0x1 << 2) /* enable flash DMA */

/* Access Data Enable */
#define SOCLE_NAND_FLSH_DATA_RW_DIS 0x0 /* disable to Read/Write Data */
#define SOCLE_NAND_FLSH_DATA_RW_EN (0x1 << 1) /* enable to Read/Write Data */

/* Flash Command/Address Input Enable */
#define SOCLE_NAND_FLSH_SEND_CMD_ADDR_DIS 0x0 /* disable flash send command and address */
#define SOCLE_NAND_FLSH_SEND_CMD_ADDR_EN 0x1 /* enable flash send command and address */

/*
 *  SOCLE_NAND_FLSH_RESET
 *  */
/* NAND Flash Controller Reset Enable */
#define SOCLE_NAND_FLSH_RST_DIS 0x0	/* disable reset */
#define SOCLE_NAND_FLSH_RST_EN 0x1	/* enable reset */

/*
 *  SOCLE_NAND_FLSH_STATE
 *  */
/* NAND Flash Device 3 Ready/Busy */
#define SOCLE_NAND_FLSH_DEV_3_RB_LOW 0x0 /* device 3 R/B = 1'b0 */
#define SOCLE_NAND_FLSH_DEV_3_RB_HIGH (0x1 << 3) /* device 3 R/B = 1'b1 */

/* NAND Flash Device 2 Ready/Busy */
#define SOCLE_NAND_FLSH_DEV_2_RB_LOW 0x0 /* device 2 R/B = 1'b0 */
#define SOCLE_NAND_FLSH_DEV_2_RB_HIGH (0x1 << 2) /* device 2 R/B = 1'b1 */

/* NAND Flash Device 1 Ready/Busy */
#define SOCLE_NAND_FLSH_DEV_1_RB_LOW 0x0 /* device 1 R/B = 1'b0 */
#define SOCLE_NAND_FLSH_DEV_1_RB_HIGH (0x1 << 1) /* device 1 R/B = 1'b1 */

/* NAND Flash Device 0 Ready/Busy */
#define SOCLE_NAND_FLSH_DEV_0_RB_LOW 0x0 /* device 0 R/B = 1'b0 */
#define SOCLE_NAND_FLSH_DEV_0_RB_HIGH 0x1 /* device 0 R/B = 1'b1 */

/*
 *  SOCLE_NAND_FLSH_INT_MASK
 *  */
#define SOCLE_NAND_FLSH_MASK_SYN_ERR_LOC_4 (0x1 << 11)	/* syndrome bit for 4th 512 data */
#define SOCLE_NAND_FLSH_MASK_SYN_ERR_LOC_3 (0x1 << 10)	/* syndrome bit for 3rd 512 data */
#define SOCLE_NAND_FLSH_MASK_SYN_ERR_LOC_2 (0x1 << 9) /* syndrome bit for 2nd 512 data */
#define SOCLE_NAND_FLSH_MASK_SYN_ERR_LOC_1 (0x1 << 8) /* syndrome bit for 1st 512 data */
#define SOCLE_NAND_FLSH_MASK_BLK_IS_ERASE (0x1 << 7) /* blcok data is erase */
#define SOCLE_NAND_FLSH_MASK_BLK_ERASE_DONE (0x1 << 6) /* block erase done */
#define SOCLE_NAND_FLSH_MASK_BLK_ERASE_FAIL (0x1 << 5) /* block erase fail */
#define SOCLE_NAND_FLSH_MASK_READ_DATA_DONE (0x1 << 4) /* read data done */
#define SOCLE_NAND_FLSH_MASK_SYN_BIT (0x1 << 3) /* syndrome bit */
#define SOCLE_NAND_FLSH_MASK_TO (0x1 << 2) /* time out */
#define SOCLE_NAND_FLSH_MASK_WRITE_DATA_DONE (0x1 << 1) /* write data done */
#define SOCLE_NAND_FLSH_MASK_WRITE_DATA_ERR 0x1 /* write data error */

/*
 *  SOCLE_NAND_FLSH_INT_STATE
 *  */
#define SOCLE_NAND_FLSH_SYN_ERR_LOC_MASK (0xf << 8) /* syndrome bit mask */	//20080326 leonid+ for  check syn error status
#define SOCLE_NAND_FLSH_SYN_ERR_LOC_4 (0x1 << 11) /* syndrome bit for 4th 512 data */
#define SOCLE_NAND_FLSH_SYN_ERR_LOC_3 (0x1 << 10) /* syndrome bit for 3rd 512 data */
#define SOCLE_NAND_FLSH_SYN_ERR_LOC_2 (0x1 << 9) /* syndrome bit for 2nd 512 data */
#define SOCLE_NAND_FLSH_SYN_ERR_LOC_1 (0x1 << 8) /* syndrome bit for 1st 512 data */
#define SOCLE_NAND_FLSH_BLK_IS_ERASE (0x1 << 7) /* blcok data is erase */
#define SOCLE_NAND_FLSH_BLK_ERASE_DONE (0x1 << 6) /* block erase done */
#define SOCLE_NAND_FLSH_BLK_ERASE_FAIL (0x1 << 5) /* block erase fail */
#define SOCLE_NAND_FLSH_READ_DATA_DONE (0x1 << 4) /* read data done */
#define SOCLE_NAND_FLSH_SYN_BIT (0x1 << 3) /* syndrome bit */
#define SOCLE_NAND_FLSH_TO (0x1 << 2)	/* time out */
#define SOCLE_NAND_FLSH_WRITE_DATA_DONE (0x1 << 1) /* write data done */
#define SOCLE_NAND_FLSH_WRITE_DATA_ERR 0x1 /* write data error */

/*
 *  SOCLE_NAND_FLSH_ECC
 *  */
#define SOCLE_NAND_FLSH_ECC_CODE_1(x) (((x) & (0x3ff << 20)) >> 20)
#define SOCLE_NAND_FLSH_ECC_CODE_2(x) (((x) & (0x3ff << 10)) >> 10)
#define SOCLE_NAND_FLSH_ECC_CODE_3(x) ((x) & 0x3ff)

/*
 *  SOCLE_NAND_FLSH_SYNDR
 *  */
#define SOCLE_NAND_FLSH_SYN_CODE_1(x) (((x) & (0x3ff << 20)) >> 20)
#define SOCLE_NAND_FLSH_SYN_CODE_2(x) (((x) & (0x3ff << 10)) >> 10)
#define SOCLE_NAND_FLSH_SYN_CODE_3(x) ((x) & 0x3ff)

#endif


