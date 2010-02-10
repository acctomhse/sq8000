#include <platform.h>
#include <irqs.h>

#ifdef SOCLE_GPIO_WITH_INT
#define SOCLE_GPIO_SUPPORT_INT 1
#else
#define SOCLE_GPIO_SUPPORT_INT 0
#endif

/* Set the IP's base address */
#ifndef SOCLE_APB0_GPIO0
#ifdef SOCLE_APB0_GPIO
#define SOCLE_APB0_GPIO0	SOCLE_APB0_GPIO
#else
#error "GPIO IP base address is not defined"
#endif
#endif

#ifndef SOCLE_APB0_GPIO1
#define SOCLE_APB0_GPIO1 SOCLE_APB0_GPIO0
#define SOCLE_GPIO_GP1	0
#else
#define SOCLE_GPIO_GP1	1
#endif

#ifndef SOCLE_APB0_GPIO2
#define SOCLE_APB0_GPIO2 SOCLE_APB0_GPIO0
#define SOCLE_GPIO_GP2	0
#else
#define SOCLE_GPIO_GP2	1
#endif

#ifndef SOCLE_APB0_GPIO3
#define SOCLE_APB0_GPIO3 SOCLE_APB0_GPIO0
#define SOCLE_GPIO_GP3	0
#else
#define SOCLE_GPIO_GP3	1
#endif

/* Set the IP's irq */
#ifdef SOCLE_GPIO_WITH_INT

#ifndef SOCLE_INTC_GPIO0
#ifdef SOCLE_INTC_GPIO
#define SOCLE_INTC_GPIO0	SOCLE_INTC_GPIO
#else
#error "GPIO IRQ is not defined"
#endif
#endif

#ifndef SOCLE_INTC_GPIO1
#define SOCLE_INTC_GPIO1		SOCLE_INTC_GPIO0
#endif

#endif //SOCLE_GPIO_WITH_INT

#if defined(CONFIG_PDK) || defined(CONFIG_PC7210) || defined(CONFIG_PC9220)
#define SOCLE_GPIO_KPD_TEST		1
#else
#define SOCLE_GPIO_KPD_TEST		0
#endif

#ifdef CONFIG_SCDK
#define SCDK_FPGA_TEST 1
#else
#define SCDK_FPGA_TEST 0
#endif
