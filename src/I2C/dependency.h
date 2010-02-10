#include <platform.h>
#include <irqs.h>

#ifndef SOCLE_APB0_I2C0
#ifdef SOCLE_APB0_I2C
#define SOCLE_APB0_I2C0 SOCLE_APB0_I2C
#else
#error "I2C IP base address is not defined"
#endif
#endif

#ifndef SOCLE_APB0_I2C1
#define SOCLE_APB0_I2C1 SOCLE_APB0_I2C0
#define SOCLE_I2C1_TEST 0
#else
#define SOCLE_I2C1_TEST 1
#endif

#ifndef SOCLE_APB0_I2C2
#define SOCLE_APB0_I2C2 SOCLE_APB0_I2C0
#define SOCLE_I2C2_TEST 0
#else
#define SOCLE_I2C2_TEST 1
#endif

#ifndef SOCLE_INTC_I2C0
#ifdef SOCLE_INTC_I2C
#define SOCLE_INTC_I2C0 SOCLE_INTC_I2C
#else
#error "I2C IRQ is not defined"
#endif
#endif

#ifndef SOCLE_INTC_I2C1
#define SOCLE_INTC_I2C1 SOCLE_INTC_I2C0
#endif

#ifndef SOCLE_INTC_I2C2
#define SOCLE_INTC_I2C2 SOCLE_INTC_I2C0
#endif

#if defined(CONFIG_PC7210) || defined(CONFIG_PC9002)
#define SOCLE_I2C_AT24C02B_EEPROM_TEST 0
#else
#define SOCLE_I2C_AT24C02B_EEPROM_TEST 1
#endif

#if defined(CONFIG_MSMV)
#define SOCLE_I2C_TPS62353_TEST 1

#define TPS62353_IP0	1
#define TPS62353_IP1	1
#define TPS62353_IP2	1

#define TPS62353_TYPE0	0		//0:VSEL PIN LOW (Change valtage from register VSEL0)	: default valtage 1.0V
#define TPS62353_TYPE1	0
#define TPS62353_TYPE2	1		//1:VSEL PIN HIGH (Change valtage from register VSEL1)	: default valtage 1.2V
#else
#define SOCLE_I2C_TPS62353_TEST 0

#define TPS62353_IP0	0	
#define TPS62353_IP1	0
#define TPS62353_IP2	0

#define TPS62353_TYPE0	0
#define TPS62353_TYPE1	0
#define TPS62353_TYPE2	0
#endif


