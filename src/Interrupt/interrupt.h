#ifndef	_interrupt_h_included_
#define	_interrupt_h_included_

#include "regs-interrupt.h"
#include "interrupt-regs.h"
#include <platform.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Default macro definitions for BSP interface.
 * These macros can be redefined in a wrapper file, to generate
 * a new module with an optimized interface.
 */
#ifndef SYS_INT_CONNECT
#define	SYS_INT_CONNECT(pDrvCtrl, rtn, arg, pResult)                    \
    {                                                                   \
    request_irq(pDrvCtrl->ilevel, (void *)(rtn) ,(void *)(arg));			\
    *pResult = TRUE;													\
    }
#endif /* SYS_INT_CONNECT */

#ifndef SYS_INT_DISCONNECT
#define SYS_INT_DISCONNECT(pDrvCtrl, rtn, arg, pResult)                 \
    {                                                                   \
    *pResult = TRUE;                                                    \
    }
#endif /* SYS_INT_DISCONNECT */

#ifndef SYS_INT_ENABLE
#define	SYS_INT_ENABLE(pDrvCtrl)                                        \
    {                                                                   \
 	iowrite32(ioread32(SOCLE_INTC_IRQ_IECR(INTC_REG_BASE)) | (0x1 << pDrvCtrl->ilevel),		\
 				SOCLE_INTC_IRQ_IECR(INTC_REG_BASE));					\
 	iowrite32(ioread32(SOCLE_INTC_IRQ_IMR(INTC_REG_BASE)) | (0x1 << pDrvCtrl->ilevel),		\
 				SOCLE_INTC_IRQ_IMR(INTC_REG_BASE));						\
    }
#endif /* SYS_INT_ENABLE */

#ifndef SYS_INT_DISABLE
#define SYS_INT_DISABLE(pDrvCtrl)                                       \
    {                                                                   \
 	iowrite32(ioread32(SOCLE_INTC_IRQ_IECR(INTC_REG_BASE)) & ~(0x1 << pDrvCtrl->ilevel),	\
 				SOCLE_INTC_IRQ_IECR(INTC_REG_BASE));					\
 	iowrite32(ioread32(SOCLE_INTC_IRQ_IMR(INTC_REG_BASE)) & ~(0x1 << pDrvCtrl->ilevel),		\
 				SOCLE_INTC_IRQ_IMR(INTC_REG_BASE));						\
    }
#endif /* SYS_INT_DISABLE */

#ifndef SYS_INT_SET_TYPE
#define SYS_INT_SET_TYPE(pDrvCtrl,type)									\
	{																	\
 	iowrite32((ioread32(SOCLE_INTC_IRQ_SCR(INTC_REG_BASE, pDrvCtrl->ilevel)) & ~SRCTYPE_MASK) | type ,		\
 				SOCLE_INTC_IRQ_SCR(INTC_REG_BASE, pDrvCtrl->ilevel));		\
	}
#endif /* SYS_INT_SET_TYPE */

#ifdef __cplusplus
}
#endif

#endif //_interrupt_h_included_
