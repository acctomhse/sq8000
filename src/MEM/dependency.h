
#define MEM_PATTERN0	0x55aa55aa
#define MEM_PATTERN1 	0xaa55aa55
#define MEM_PATTERN2 	0x55555555
#define MEM_PATTERN3 	0xaaaaaaaa
#define MEM_PATTERN4 	0xffffffff
#define MEM_PATTERN5 	0x00000000
#define MEM_PATTERN6 	0xffff0000
#define MEM_PATTERN7 	0x0000ffff

#ifdef CONFIG_SCDK
#define SCDK_FPGA_TEST 1
#else
#define SCDK_FPGA_TEST 0
#endif

#ifdef CONFIG_PC9220
#define MSDR_TEST 1
#else
#define MSDR_TEST 0
#endif
