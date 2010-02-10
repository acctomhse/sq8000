#include <platform.h>
#include <irqs.h>
#include "../../src/Interrupt/interrupt.h"

struct intr_handler
{
	void (*vec)(void *pparam);				// interrupt vector
	void *pparam;							// interrupt handler parameter
};

static struct intr_handler irq_vector_table[INTC_MAX_IRQ];		// IRQ controller interrupt handle vector table
//static struct intr_handler fiq_vector_table[INTC_MAX_FIQ];		// FIQ controller interrupt handle vector table

static inline void
int_irq_set_type (int i, int type)
{
	iowrite32((ioread32(SOCLE_INTC_IRQ_SCR(INTC_REG_BASE, i)) & ~SRCTYPE_MASK) | type,
				SOCLE_INTC_IRQ_SCR(INTC_REG_BASE, i));
}

static inline void
int_fiq_set_type (int i, int type)
{
	iowrite32((ioread32(SOCLE_INTC_FIQ_SCR(INTC_REG_BASE, i)) & ~SRCTYPE_MASK) | type,
				SOCLE_INTC_FIQ_SCR(INTC_REG_BASE, i));
}

// connectInterrupt()
//   note : pparam must point to an 'exist' data when interrupt occur
//   usually this should be a 'static' vars
//   'auto' vars can be used iff we know the interrupt occurred in the function
extern void
connectInterrupt (int irq, void (*routine)(void*), void* pparam)
{
	irq_vector_table[irq].vec     = routine;
	irq_vector_table[irq].pparam  = pparam;
}

extern void
request_irq (int irq, void (*routine)(void*), void* pparam)
{
	irq_vector_table[irq].vec     = routine;
	irq_vector_table[irq].pparam  = pparam;

	// irq enable
	iowrite32(ioread32(SOCLE_INTC_IRQ_IECR(INTC_REG_BASE)) | (0x1 << irq),
				SOCLE_INTC_IRQ_IECR(INTC_REG_BASE));

	// irq set mask
	iowrite32(ioread32(SOCLE_INTC_IRQ_IMR(INTC_REG_BASE)) | (0x1 << irq),
				SOCLE_INTC_IRQ_IMR(INTC_REG_BASE));
}

extern void
request_fiq (int fiq, void (*routine)(void*), void* pparam)
{
	return;
}

extern void
free_irq (int irq)
{
	irq_vector_table[irq].vec     = NULL;
	irq_vector_table[irq].pparam  = NULL;

	// irq disable
	iowrite32(ioread32(SOCLE_INTC_IRQ_IECR(INTC_REG_BASE)) & ~(0x1 << irq),
				SOCLE_INTC_IRQ_IECR(INTC_REG_BASE));

	// irq clear mask
	iowrite32(ioread32(SOCLE_INTC_IRQ_IMR(INTC_REG_BASE)) & ~(0x1 << irq),
				SOCLE_INTC_IRQ_IMR(INTC_REG_BASE));
}


extern void
free_fiq (int fiq)
{
	return;
}

////////////////////////////////////
//
// normal interrupt dispatch routine
//
////////////////////////////////////
extern void
irq_dispatch (void)
{
	u32_t v = ioread32(SOCLE_INTC_IRQ_ISR(INTC_REG_BASE));		// index for interrupt source

#if 0  //for debug
	u32_t w = ioread32(SOCLE_INTC_IRQ_IPR(INTC_REG_BASE));
	if (w != (1 << v)) {
		printf("IRQ dispatch: IPR = 0x%08x\n", w);
		printf("IRQ dispatch: ISR = 0x%08x\n", v);
	}
	printf("IRQ dispatch: ISR = %d\n", v);
#endif

	if (NULL == irq_vector_table[v].vec) {
//		printf("Warning!! irq = %d but isr is NULL\n", v);
		return;
	}

	// branch to the fn saved in irq_vector_table[v]
	irq_vector_table[v].vec(irq_vector_table[v].pparam);

	// clear interrupt
	iowrite32(1 << v, SOCLE_INTC_IRQ_ICCR(INTC_REG_BASE));
}

extern void
init_irq_controller (void)
{
	int i;

	// disable all interrupt
	iowrite32(0, SOCLE_INTC_IRQ_IECR(INTC_REG_BASE));

	// clear all interrupt
	iowrite32(0xFFFFFFFF, SOCLE_INTC_IRQ_ICCR(INTC_REG_BASE));

	for (i = 0; i < INTC_MAX_IRQ; i++) {
		//if (i == 19)
//			int_irq_set_type(i, LO_LEVEL);
		//else if (i == 27)
		if (i == 27)
			int_irq_set_type(i, POSITIVE_EDGE);
		else if (i == 29)
			int_irq_set_type(i, LO_LEVEL);
		else
			int_irq_set_type(i, HI_LEVEL);
	}
}

//++cyli 02/01/07
extern void
fiq_dispatch (void)
{
	return;
}

//++cyli 02/01/07
extern void
init_fiq_controller (void)
{
	return;
}

#ifdef CPU_ARCH_ARM
/* enable IRQ interrupts */
extern void
enable_irq (void)
{
	unsigned long temp;
	__asm__ __volatile__("mrs %0, cpsr\n"
			"bic %0, %0, #0x80\n"
			"msr cpsr_c, %0"
			: "=r" (temp)
			:
			: "memory");
}


/* enable FIQ interrupts */
extern void
enable_fiq (void)
{
	unsigned long temp;
	__asm__ __volatile__("mrs %0, cpsr\n"
			"bic %0, %0, #0x40\n"
			"msr cpsr_c, %0"
			: "=r" (temp)
			:
			: "memory");
}
/*
 * disable IRQ/FIQ interrupts
 * returns true if interrupts had been enabled before we disabled them
 */
extern int
disable_interrupts (void)
{
	unsigned long old,temp;
	__asm__ __volatile__("mrs %0, cpsr\n"
			"orr %1, %0, #0xc0\n"
			"msr cpsr_c, %1"
			: "=r" (old), "=r" (temp)
			:
			: "memory");
	return (old & 0x80) == 0;
}
#endif
