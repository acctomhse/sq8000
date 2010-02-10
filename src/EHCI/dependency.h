#include <platform.h>
#include <irqs.h>


#ifndef SOCLE_AHB0_UHC0
#ifdef SOCLE_AHB0_UHC
#define SOCLE_AHB0_UHC0 SOCLE_AHB0_UHC
#else
#error "EHCI IP base address is not defined"
#endif
#endif

#ifndef SOCLE_AHB0_UHC1
#define SOCLE_AHB0_UHC1 SOCLE_AHB0_UHC0
#define SOCLE_EHCI1_TEST 0
#else
#define SOCLE_EHCI1_TEST 1
#endif


#ifndef SOCLE_INTC_UHC0
#ifdef SOCLE_INTC_UHC
#define SOCLE_INTC_UHC0 SOCLE_INTC_UHC
#else
#error "EHCI IRQ is not defined"
#endif
#endif

#ifndef SOCLE_INTC_UHC1
#define SOCLE_INTC_UHC1 SOCLE_INTC_UHC0
#endif


