#ifndef _interrupt_regs_H_
#define _interrupt_regs_H_

#if defined(CONFIG_LDK5) && defined(CONFIG_ARM7_HI)
#define INTC_REG_BASE		SOCLE_AHB1_INTC
#endif

#ifndef INTC_REG_BASE
#define INTC_REG_BASE		SOCLE_AHB0_INTC
#endif

//IRQ
#define	SOCLE_INTC_IRQ_SCR(x, y)  ((x) + 4 * (y))
#define	SOCLE_INTC_IRQ_SVR(x, y)  ((x) + 0x0080 + 4 * (y))
#define	SOCLE_INTC_IRQ_ISR(x)     ((x) + 0x0104)
#define	SOCLE_INTC_IRQ_IPR(x)     ((x) + 0x0108)
#define	SOCLE_INTC_IRQ_IMR(x)     ((x) + 0x010C)
#define	SOCLE_INTC_IRQ_IECR(x)    ((x) + 0x0114)
#define	SOCLE_INTC_IRQ_ICCR(x)    ((x) + 0x0118)
#define	SOCLE_INTC_IRQ_ISCR(x)    ((x) + 0x011C)
#define	SOCLE_INTC_IRQ_TEST(x)    ((x) + 0x0124)

//FIQ
#define FIQ_BASE_OFFSET		0x1000
#define	SOCLE_INTC_FIQ_SCR(x, y)  ((x) + FIQ_BASE_OFFSET + 4 * (y))
#define	SOCLE_INTC_FIQ_SVR(x, y)  ((x) + FIQ_BASE_OFFSET + 0x0080 + 4 * (y))
#define	SOCLE_INTC_FIQ_ISR(x)     ((x) + FIQ_BASE_OFFSET + 0x0104)
#define	SOCLE_INTC_FIQ_IPR(x)     ((x) + FIQ_BASE_OFFSET + 0x0108)
#define	SOCLE_INTC_FIQ_IMR(x)     ((x) + FIQ_BASE_OFFSET + 0x010C)
#define	SOCLE_INTC_FIQ_IECR(x)    ((x) + FIQ_BASE_OFFSET + 0x0114)
#define	SOCLE_INTC_FIQ_ICCR(x)    ((x) + FIQ_BASE_OFFSET + 0x0118)
#define	SOCLE_INTC_FIQ_ISCR(x)    ((x) + FIQ_BASE_OFFSET + 0x011C)
#define	SOCLE_INTC_FIQ_TEST(x)    ((x) + FIQ_BASE_OFFSET + 0x0124)


#define	LO_LEVEL		 		0x00000000
#define	HI_LEVEL		 		0x00000040
#define	NEGATIVE_EDGE	 	0x00000080
#define	POSITIVE_EDGE	 	0x000000C0
#define	SRCTYPE_MASK    0x000000C0


#endif	//_interrupt_regs_H_
