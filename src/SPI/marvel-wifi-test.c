#include "spi-master.h"
#include "if_gspi.h"

#define MARVEL_MAX_NUM	64
#define MARVEL_CHAR_LEN	16
#define MARVEL_CMD_WR	0x8000

static u8 *socle_spi_pattern_buf = (u8 *)PATTERN_BUF_ADDR;
static u8 *socle_spi_cmpr_buf = (u8 *)(PATTERN_BUF_ADDR + PATTERN_BUF_SIZE);

extern int
socle_spi_marvel_wifi(int autotest)
{
	u8 divisor;
	u16 tx_buf[MARVEL_MAX_NUM + 1] = {0};
	u16 rx_buf[1] = {0};
	u32 result = 0, i;

	/* Reset SPI controller */
	socle_spi_write(
			socle_spi_read(SOCLE_SPI_FWCR) | 
			SOCLE_SPI_SOFT_RST,
			SOCLE_SPI_FWCR);

	/* Configure SPI controller */
	socle_spi_write(
#ifdef CONFIG_PC9220
			SOCLE_SPI_MASTER_SIGNAL_CTL_HW |
			SOCLE_SPI_MASTER_SIGNAL_ACT_NO |		
			SOCLE_SPI_MODE_MASTER |
#endif
			SOCLE_SPI_SOFT_N_RST |
			SOCLE_SPI_TXRX_N_RUN |
			SOCLE_SPI_CLK_IDLE_AST |
			SOCLE_SPI_TXRX_SIMULT_DIS |
			SOCLE_SPI_CPOL_0 |
			SOCLE_SPI_CPHA_0 |
			SOCLE_SPI_TX_MSB_FIRST |
			SOCLE_SPI_OP_NORMAL,
			SOCLE_SPI_FWCR);
			
	/* Enable SPI interrupt */
	socle_spi_write(
			SOCLE_SPI_IER_RXFIFO_INT_EN |
			//SOCLE_SPI_IER_RXFIFO_OVR_INT_EN |
			SOCLE_SPI_IER_RX_COMPLETE_INT_EN,
			SOCLE_SPI_IER);

	/* Configure FIFO and clear Tx & Rx FIFO */
	socle_spi_write(
			SOCLE_SPI_RXFIFO_INT_TRIGGER_LEVEL_4 |
			SOCLE_SPI_TXFIFO_INT_TRIGGER_LEVEL_4 |
			SOCLE_SPI_RXFIFO_CLR |
			SOCLE_SPI_TXFIFO_CLR,
			SOCLE_SPI_FCR);
     
	
	divisor = socle_spi_calculate_divisor(1000000); /* 1.0 MHz clock rate */
	/* Set the SPI slaves select and characteristic control register */
	socle_spi_write(
			SOCLE_SPI_CHAR_LEN_16 |
			SOCLE_SPI_SLAVE_SEL_0 |
			SOCLE_SPI_CLK_DIV(divisor),
			//SOCLE_SPI_CLK_DIV(0x3f),
			SOCLE_SPI_SSCR);

	/* Config SPI clock delay */
	socle_spi_write(
			SOCLE_SPI_PBTXRX_DELAY_NONE |
			SOCLE_SPI_PBCT_DELAY_NONE |
			SOCLE_SPI_PBCA_DELAY_1_2,
			SOCLE_SPI_DLYCR);

	/* Set per char length */
	socle_spi_set_current_mode(MODE_CHAR_LEN_16);

	/* Clear pattern buffer and compare buffer */
	memset(socle_spi_pattern_buf, 0x0, MARVEL_MAX_NUM * MARVEL_CHAR_LEN / 8);
	memset(socle_spi_cmpr_buf, 0x0, MARVEL_MAX_NUM * MARVEL_CHAR_LEN / 8);

	/* Set as no delay mode */
	tx_buf[0] = MARVEL_CMD_WR | SPU_BUS_MODE_REG;
	tx_buf[1] = BUS_MODE_16_NO_DELAY;
	if(socle_spi_transfer(tx_buf, NULL, SET_TX_RX_LEN(2, 0)))
		return -1;

	/* Read Chip ID */
	tx_buf[0] = CHIPREV_REG;
	if(socle_spi_transfer(tx_buf, rx_buf, SET_TX_RX_LEN(1, 2)))
		return -1;

	if (0xb != rx_buf[0]) {
		printf("Chip ID [0x%02x] != 0x0b fail!!\n", rx_buf[0]);
		return -1;
	} else {
		printf("Read Chip ID = 0x%02x success\n", rx_buf[0]);
	}

	/* Bulk data R/W test */
	printf("Burn in loop: ");
	for (i = 0; i < 10; i++) {

		/* Write data */
		tx_buf[0] = MARVEL_CMD_WR | DATA_RDWRPORT_REG;

		socle_spi_make_test_pattern(socle_spi_pattern_buf, MARVEL_MAX_NUM * MARVEL_CHAR_LEN / 8);
		memcpy((char *)&tx_buf[1], socle_spi_pattern_buf, MARVEL_MAX_NUM * MARVEL_CHAR_LEN / 8);
		if(socle_spi_transfer(tx_buf, NULL, SET_TX_RX_LEN(MARVEL_MAX_NUM+1, 0)))
			return -1;

		/* Read data */
		tx_buf[0] = DATA_RDWRPORT_REG;

		if(socle_spi_transfer(tx_buf, socle_spi_cmpr_buf, SET_TX_RX_LEN(1, MARVEL_MAX_NUM)))
			return -1;

		result = socle_spi_compare_memory(socle_spi_pattern_buf, socle_spi_cmpr_buf, MARVEL_MAX_NUM * MARVEL_CHAR_LEN / 8,
						autotest);
		printf("%d ", i);

		if (result)
			break;
	}
	printf("\n");

	if (result)
		printf("Burn in fail[%d]!!\n", i);

	return result;
}
