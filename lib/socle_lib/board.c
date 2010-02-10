#include <genlib.h>
#include <clock.h>
#include <io.h>

//#define CONFIG_SOCLE_WAIT_DEBUG
#ifdef CONFIG_SOCLE_WAIT_DEBUG
	#define WAIT_DBG(fmt, args...) printf("SOCLE_WAIT: " fmt, ## args)
#else
	#define WAIT_DBG(fmt, args...)
#endif


extern int
socle_wait_for_int(volatile int *flag, int sec)
{
	int t = 0, threshold = socle_get_cpu_clock() / 95 * sec;

#if defined(CONFIG_ARM7) || (CONFIG_ARM7_HI)
	threshold /= 2;
#endif

	WAIT_DBG("flag = %d, sec = %d, threshold = 0x%08x\n", *flag, sec, threshold);

	while (1 != *flag) {
		if (t >= threshold)
			return -1;
		t++;
	}
	*flag = 0;

	WAIT_DBG("t = %x\n", t);

	return 0;
}

extern int
socle_wait_by_poll(int addr, int mask, int exp_val, int sec)
{
	int t = 0, val, threshold = socle_get_cpu_clock() / 170 * sec;

#if defined(CONFIG_ARM7) || (CONFIG_ARM7_HI)
	threshold /= 2;
#endif

	exp_val &= mask;
	WAIT_DBG("addr = 0x%08x, mask = 0x%08x, exp_val = 0x%08x, sec = %d, threshold = 0x%08x\n", addr, mask, exp_val, sec, threshold);

	do {
		val = ioread32(addr);
		val &= mask;

		if (t >= threshold)
			return -1;
		t++;
	} while (exp_val != val);

	WAIT_DBG("t = %x\n", t);

	return 0;
}
