#ifndef __CLOCK_H_INCLUDED
#define __CLOCK_H_INCLUDED

#if defined(CONFIG_PC7210) || defined(CONFIG_PDK) || defined(CONFIG_MSMV)
#include <scu.h>
#else
extern unsigned long socle_get_cpu_clock (void);
extern unsigned long socle_get_ahb_clock (void);
extern unsigned long socle_get_apb_clock (void);
#endif

#endif
