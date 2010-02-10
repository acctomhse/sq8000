#ifndef _io_h_included_
#define _io_h_included_

#include "type.h"
#include "swab.h"

#if defined (SYS_CPU_BIG)

#define readb(addr) (*(volatile u8_t *)(addr))
#define readh(addr) swab16(*(volatile u16_t *)(addr))
#define readw(addr) swab32(*(volatile u32_t *)(addr))
#define ioread8(addr) readb(addr)
#define ioread16(addr) readh(addr)
#define ioread32(addr) readw(addr)

#define writeb(data, addr) (*(volatile u8_t  *)(addr) = (data))
#define writeh(data, addr) (*(volatile u16_t *)(addr) = swab16(data))
#define writew(data, addr) (*(volatile u32_t *)(addr) = swab32(data))
#define iowrite8(data, addr) writeb(data, addr)
#define iowrite16(data, addr) writeh(data, addr)
#define iowrite32(data, addr) writew(data, addr)

#elif defined (SYS_CPU_LITTLE)

#define readb(addr) (*(volatile u8_t *)(addr))
#define readh(addr) (*(volatile u16_t *)(addr))
#define readw(addr) (*(volatile u32_t *)(addr))
#define readl(addr) (*(volatile u32_t *)(addr))
#define ioread8(addr) readb(addr)
#define ioread16(addr) readh(addr)
#define ioread32(addr) readw(addr)

#define writeb(data, addr) (*(volatile u8_t  *)(addr) = data)
#define writeh(data, addr) (*(volatile u16_t *)(addr) = data)
#define writew(data, addr) (*(volatile u32_t *)(addr) = data)
#define writel(data, addr) (*(volatile u32_t *)(addr) = data)
#define iowrite8(data, addr) writeb(data, addr)
#define iowrite16(data, addr) writeh(data, addr)
#define iowrite32(data, addr) writew(data, addr)

#else
#error "System  Endian not well defined!"
#endif

#endif // _io_h_included_
