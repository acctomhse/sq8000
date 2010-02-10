#ifndef __DEPENDENCY_H_INCLUDED
#define __DEPENDENCY_H_INCLUDED

#include <platform.h>
#include <irqs.h>
#include <dma/dma.h>

#ifndef SOCLE_APB0_I2S0
#ifdef SOCLE_APB0_I2S
#define SOCLE_I2S_BASE SOCLE_APB0_I2S
#else
#error "I2S IP base address is not defined"
#endif
#endif

#ifndef SOCLE_INTC_I2S0
#ifdef SOCLE_INTC_I2S 
#define SOCLE_I2S_IRQ SOCLE_INTC_I2S
#else
#error "I2S IRQ is not defined"
#endif
#endif

//20080123 Ryan Define MSB/LSB to 0
#define SOCLE_I2S_BUS_LSB_TEST 0
#define SOCLE_I2S_BUS_MSB_TEST 0

#ifdef CONFIG_PANTHER7_HDMA
#define SOCLE_I2S_SUPPORT_HWDMA_PANTHER7_HDMA_TEST 1
#else
#define SOCLE_I2S_SUPPORT_HWDMA_PANTHER7_HDMA_TEST 0
#endif

#if defined(CONFIG_PDK) || defined(CONFIG_PC7210) || defined(CONFIG_MSMV) || defined(CONFIG_PC9220)
#define TX_DMA_EXT_HDREQ 6
#define RX_DMA_EXT_HDREQ 7
#elif defined(CONFIG_CDK) || defined(CONFIG_PC9002) || defined(CONFIG_SCDK)
#define TX_DMA_EXT_HDREQ 0
#define RX_DMA_EXT_HDREQ 1
#else
#define TX_DMA_EXT_HDREQ 0
#define RX_DMA_EXT_HDREQ 0
#endif

#if defined(CONFIG_PDK) ||defined(CONFIG_PC7210)|| defined(CONFIG_PC9220)
#define SOCLE_I2S_HDMA_AUTO_TEST	0
#define SOCLE_I2S_HWDMA_BURST_INCR8	0
#define SOCLE_I2S_HWDMA_BURST_INCR16	0
#else
#define SOCLE_I2S_HDMA_AUTO_TEST        1
#define SOCLE_I2S_HWDMA_BURST_INCR8	1
#define SOCLE_I2S_HWDMA_BURST_INCR16	1
#endif

#endif
