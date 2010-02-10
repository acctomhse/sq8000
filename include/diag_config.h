#ifndef __SOCLE_DIAG_CONFIG_H
#define __SOCLE_DIAG_CONFIG_H
/*
 * Automatically generated header file: don't edit
 */


/*
 * Socle Platforms
 */
#define CONFIG_PC9220 1
#define ENABLE_PC9220 1
#define USE_PC9220(...)  __VA_ARGS__
#define SKIP_PC9220(...)


/*
 * Socle CPUs
 */
#undef CONFIG_MIPS
#define ENABLE_MIPS 0
#define USE_MIPS(...)
#define SKIP_MIPS(...)  __VA_ARGS__

#undef CONFIG_ARM7
#define ENABLE_ARM7 0
#define USE_ARM7(...)
#define SKIP_ARM7(...)  __VA_ARGS__

#undef CONFIG_ARM7_HI
#define ENABLE_ARM7_HI 0
#define USE_ARM7_HI(...)
#define SKIP_ARM7_HI(...)  __VA_ARGS__

#define CONFIG_ARM9 1
#define ENABLE_ARM9 1
#define USE_ARM9(...)  __VA_ARGS__
#define SKIP_ARM9(...)


/*
 * Socle CPU Endians
 */
#undef CONFIG_BIG_ENDIAN
#define ENABLE_BIG_ENDIAN 0
#define USE_BIG_ENDIAN(...)
#define SKIP_BIG_ENDIAN(...)  __VA_ARGS__

#define CONFIG_LITTLE_ENDIAN 1
#define ENABLE_LITTLE_ENDIAN 1
#define USE_LITTLE_ENDIAN(...)  __VA_ARGS__
#define SKIP_LITTLE_ENDIAN(...)


/*
 * Socle IPs Test Items
 */
#define CONFIG_GPIO 1
#define ENABLE_GPIO 1
#define USE_GPIO(...)  __VA_ARGS__
#define SKIP_GPIO(...)

#define CONFIG_MEMORY 1
#define ENABLE_MEMORY 1
#define USE_MEMORY(...)  __VA_ARGS__
#define SKIP_MEMORY(...)

#define CONFIG_INTERRUPT 1
#define ENABLE_INTERRUPT 1
#define USE_INTERRUPT(...)  __VA_ARGS__
#define SKIP_INTERRUPT(...)

#define CONFIG_UART 1
#define ENABLE_UART 1
#define USE_UART(...)  __VA_ARGS__
#define SKIP_UART(...)

#define CONFIG_TIMER 1
#define ENABLE_TIMER 1
#define USE_TIMER(...)  __VA_ARGS__
#define SKIP_TIMER(...)

#define CONFIG_WDT 1
#define ENABLE_WDT 1
#define USE_WDT(...)  __VA_ARGS__
#define SKIP_WDT(...)

#define CONFIG_RTC 1
#define ENABLE_RTC 1
#define USE_RTC(...)  __VA_ARGS__
#define SKIP_RTC(...)

#define CONFIG_SPI 1
#define ENABLE_SPI 1
#define USE_SPI(...)  __VA_ARGS__
#define SKIP_SPI(...)

#define CONFIG_UDC 1
#define ENABLE_UDC 1
#define USE_UDC(...)  __VA_ARGS__
#define SKIP_UDC(...)

#define CONFIG_MAC 1
#define ENABLE_MAC 1
#define USE_MAC(...)  __VA_ARGS__
#define SKIP_MAC(...)

#define CONFIG_PANTHER7_HDMA 1
#define ENABLE_PANTHER7_HDMA 1
#define USE_PANTHER7_HDMA(...)  __VA_ARGS__
#define SKIP_PANTHER7_HDMA(...)

#define CONFIG_I2S 1
#define ENABLE_I2S 1
#define USE_I2S(...)  __VA_ARGS__
#define SKIP_I2S(...)

#define CONFIG_MS6335 1
#define ENABLE_MS6335 1
#define USE_MS6335(...)  __VA_ARGS__
#define SKIP_MS6335(...)

#undef CONFIG_UDA1342TS
#define ENABLE_UDA1342TS 0
#define USE_UDA1342TS(...)
#define SKIP_UDA1342TS(...)  __VA_ARGS__

#define CONFIG_I2C 1
#define ENABLE_I2C 1
#define USE_I2C(...)  __VA_ARGS__
#define SKIP_I2C(...)

#define CONFIG_SDMMC 1
#define ENABLE_SDMMC 1
#define USE_SDMMC(...)  __VA_ARGS__
#define SKIP_SDMMC(...)

#define CONFIG_NOR_MXIC 1
#define ENABLE_NOR_MXIC 1
#define USE_NOR_MXIC(...)  __VA_ARGS__
#define SKIP_NOR_MXIC(...)

#define CONFIG_NAND 1
#define ENABLE_NAND 1
#define USE_NAND(...)  __VA_ARGS__
#define SKIP_NAND(...)

#define CONFIG_EHCI 1
#define ENABLE_EHCI 1
#define USE_EHCI(...)  __VA_ARGS__
#define SKIP_EHCI(...)

#define CONFIG_OHCI 1
#define ENABLE_OHCI 1
#define USE_OHCI(...)  __VA_ARGS__
#define SKIP_OHCI(...)

#define CONFIG_SCU 1
#define ENABLE_SCU 1
#define USE_SCU(...)  __VA_ARGS__
#define SKIP_SCU(...)

#define CONFIG_SOCLE_LCD 1
#define ENABLE_SOCLE_LCD 1
#define USE_SOCLE_LCD(...)  __VA_ARGS__
#define SKIP_SOCLE_LCD(...)

#define CONFIG_ADC_PWM 1
#define ENABLE_ADC_PWM 1
#define USE_ADC_PWM(...)  __VA_ARGS__
#define SKIP_ADC_PWM(...)

#define CONFIG_VIOP 1
#define ENABLE_VIOP 1
#define USE_VIOP(...)  __VA_ARGS__
#define SKIP_VIOP(...)

#define CONFIG_LCM 1
#define ENABLE_LCM 1
#define USE_LCM(...)  __VA_ARGS__
#define SKIP_LCM(...)


/*
 * Socle Debugers
 */
#define CONFIG_SEMIHOST 1
#define ENABLE_SEMIHOST 1
#define USE_SEMIHOST(...)  __VA_ARGS__
#define SKIP_SEMIHOST(...)

#undef CONFIG_GDB
#define ENABLE_GDB 0
#define USE_GDB(...)
#define SKIP_GDB(...)  __VA_ARGS__


/*
 * Socle UART Display
 */
#define CONFIG_UART_DISP 1
#define ENABLE_UART_DISP 1
#define USE_UART_DISP(...)  __VA_ARGS__
#define SKIP_UART_DISP(...)

#endif /* __SOCLE_DIAG_CONFIG_H */
