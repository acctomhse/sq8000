#include <platform.h>
#include <irqs.h>

#ifndef SOCLE_APB0_SDMMC0
#ifdef SOCLE_APB0_SDMMC
#define SOCLE_APB0_SDMMC0 SOCLE_APB0_SDMMC
#else
#error "SD/MMC IP base address is not defined"
#endif
#endif

#ifndef SOCLE_INTC_SDMMC0
#ifdef SOCLE_INTC_SDMMC
#define SOCLE_INTC_SDMMC0 SOCLE_INTC_SDMMC
#else
#error "SD/MMC IRQ is not defined"
#endif
#endif

#ifdef CONFIG_A2ADMA
#define SOCLE_SDMMC_SWDMA_A2A_TEST 1
#else
#define SOCLE_SDMMC_SWDMA_A2A_TEST 0
#endif

#ifdef CONFIG_HDMA
#define SOCLE_SDMMC_SWDMA_HDMA_TEST 1
#else
#define SOCLE_SDMMC_SWDMA_HDMA_TEST 0
#endif

#if defined(CONFIG_PANTHER7_HDMA) && (defined(CONFIG_PDK) || defined(CONFIG_PC7210) || defined(CONFIG_PC9002) || defined(CONFIG_SCDK))
#define SOCLE_SDMMC_SWDMA_PANTHER7_HDMA_TEST 1
#else
#define SOCLE_SDMMC_SWDMA_PANTHER7_HDMA_TEST 0
#endif

#if defined(CONFIG_CDK) || defined(CONFIG_PC9220) || defined(CONFIG_PC9002) || defined(CONFIG_SCDK)
#define SOCLE_SDMMC_MUTLIPLE_PARTIAL_READ_TEST 0
#else
#define SOCLE_SDMMC_MUTLIPLE_PARTIAL_READ_TEST 0 
#endif
