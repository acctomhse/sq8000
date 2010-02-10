#include <platform.h>
#include <irqs.h>

/* Set the IP's base address */
#ifndef SOCLE_APB0_UART0
#ifdef SOCLE_APB0_UART
#define SOCLE_APB0_UART0 SOCLE_APB0_UART
#else
#error "UART IP base address is not defined"
#endif
#endif

#ifndef SOCLE_APB0_UART0
#define SOCLE_APB0_UART0 SOCLE_APB0_UART0
#define SOCLE_UART_IP_0_TEST 0
#else
#define SOCLE_UART_IP_0_TEST 1
#endif

#ifndef SOCLE_APB0_UART1
#define SOCLE_APB0_UART1 SOCLE_APB0_UART0
#define SOCLE_UART_IP_1_TEST 0
#else
#define SOCLE_UART_IP_1_TEST 1
#endif

#ifndef SOCLE_APB0_UART2
#define SOCLE_APB0_UART2 SOCLE_APB0_UART0
#define SOCLE_UART_IP_2_TEST 0
#else
#define SOCLE_UART_IP_2_TEST 1
#endif

#ifndef SOCLE_APB0_UART3
#define SOCLE_APB0_UART3 SOCLE_APB0_UART0
#define SOCLE_UART_IP_3_TEST 0
#else
#define SOCLE_UART_IP_3_TEST 1
#endif

/* Set the IP's irq */
#ifndef SOCLE_INTC_UART0
#ifdef SOCLE_INTC_UART
#define SCOLE_INTC_UART0 SOCLE_INTC_UART
#else
#error "UART IRQ is not defined"
#endif
#endif

#ifndef SOCLE_INTC_UART1
#define SOCLE_INTC_UART1 SOCLE_INTC_UART0
#endif

#ifndef SOCLE_INTC_UART2
#define SOCLE_INTC_UART2 SOCLE_INTC_UART0
#endif

#ifndef SOCLE_INTC_UART3
#define SOCLE_INTC_UART3 SOCLE_INTC_UART0
#endif

/* Configure the test menu */
#if defined(CONFIG_PANTHER7_HDMA) && (defined(CONFIG_PDK) || defined(CONFIG_PC7210) || defined(CONFIG_PC9220))
#define SOCLE_UART_HWDMA_PANTHER7_HDMA_TEST 1
#else
#define SOCLE_UART_HWDMA_PANTHER7_HDMA_TEST 0
#endif

#ifdef CONFIG_UART_DISP
#if defined(CONFIG_PC7210) || defined(CONFIG_PDK)
#undef SOCLE_UART_IP_1_TEST
#define SOCLE_UART_IP_1_TEST 0
#else
#undef SOCLE_UART_IP_0_TEST
#define SOCLE_UART_IP_0_TEST 0
#endif
#endif

#if defined(CONFIG_PC9002) || defined(CONFIG_PC9220)
#define SOCLE_UART_GPS_0_TEST 0 
#define SOCLE_UART_GPS_1_TEST 1
#define SOCLE_UART_GPS_2_TEST 0
#define SOCLE_UART_GPS_3_TEST 0
#else
#define SOCLE_UART_GPS_0_TEST 0 
#define SOCLE_UART_GPS_1_TEST 0
#define SOCLE_UART_GPS_2_TEST 0
#define SOCLE_UART_GPS_3_TEST 0
#endif

#if defined(CONFIG_PC9002) || defined(CONFIG_PC9220)
#define SOCLE_UART_EXT_0_TEST 1
#define SOCLE_UART_EXT_1_TEST 0
#define SOCLE_UART_EXT_2_TEST 0
#define SOCLE_UART_EXT_3_TEST 0
#elif defined(CONFIG_PC7210)
#define SOCLE_UART_EXT_0_TEST 0
#define SOCLE_UART_EXT_1_TEST 1
#define SOCLE_UART_EXT_2_TEST 0
#define SOCLE_UART_EXT_3_TEST 0
#else
#define SOCLE_UART_EXT_0_TEST 1 
#define SOCLE_UART_EXT_1_TEST 1
#define SOCLE_UART_EXT_2_TEST 0
#define SOCLE_UART_EXT_3_TEST 0
#endif
