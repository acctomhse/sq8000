#ifndef	_interrupt_h_
#define	_interrupt_h_

#include <global.h>

// for MIPS
#define	SYS_INT_0		0
#define	SYS_INT_1		1
#define	SYS_INT_2		2	// SI_INT[0]
#define	SYS_INT_3		3	// SI_INT[1]
#define	SYS_INT_4		4	// SI_INT[2]
#define	SYS_INT_5		5	// SI_INT[3]
#define	SYS_INT_6		6	// SI_INT[4]
#define	SYS_INT_7		7	// SI_INT[5]


// interrupt function
extern void request_irq (int irq, void (*routine)(void*), void* pparam);
extern void request_fiq (int fiq, void (*routine)(void*), void* pparam);
extern void free_irq (int irq);
extern void free_fiq (int fiq);
extern void init_irq_controller (void);
extern void init_fiq_controller (void);
extern void connectInterrupt (int irq, void (*routine)(void*), void* pparam);

extern int disable_interrupts (void);
extern void enable_irq (void);
extern void enable_fiq (void);

#endif	//_interrupt_h_

