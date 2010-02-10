#ifndef _MEMCTRL_H_
#define _MEMCTRL_H_

extern int MemoryTesting(int autotest);
static int mem_rw_test (u32 start, u32 bank_size, u32 pattern, u8 type);
static int mem_bank_pattern_test(int burn);

#endif // _MEMCTRL_H_
