#include <platform.h>
#include <global.h>
#include <genlib.h>
#include <test_item.h>
#include "mem-ctrl.h"
#include "memctrl-regs.h"
#include "dependency.h"
#if defined(CONFIG_PDK) || defined(CONFIG_PC7210)
#include <scu.h>
#endif

#define SCU_AHB_MODE (0x1<<6)

static u32 mem_pattern[]={
	MEM_PATTERN0,
	MEM_PATTERN1,
	MEM_PATTERN2,
	MEM_PATTERN3,
	MEM_PATTERN4,
	MEM_PATTERN5,
	MEM_PATTERN6,
	MEM_PATTERN7,
};

static u32	addr_start[SOCLE_MEMORY_BANKS];					
static u32	bank_size[SOCLE_MEMORY_BANKS];
static u32	mem_start;
static u32	mem_end;
static u32	mem_size;	
static u32	test_start_addr;
static u32	test_size;

extern struct test_item_container mem_main_container;

extern int
MemoryTesting(int autotest)
{
	int ret = 0, size;

	size = SOCLE_MEMORY_ADDR_SIZE;

	#if defined(CONFIG_PDK) || defined(CONFIG_PC7210)
		if(SOCLE_SCU_SDRAM_BUS_WIDTH_32 == socle_scu_sdram_bus_width_status())
			size = size * 2;
	#endif

	mem_start = (u32)SOCLE_MEMORY_ADDR_START | (u32)_end;
	mem_end = (u32)SOCLE_MEMORY_ADDR_START + size;
	mem_size = (u32)mem_end - (u32)mem_start;
	
#ifdef CONFIG_SCDK
	{
		extern struct test_item mem_main_test_items[];
		//read scu to get amba mode
		if(ioread32(SOCLE_APB0_SCU + 0x28) & SCU_AHB_MODE)
			mem_main_test_items[4].enable=1;
		else 
			mem_main_test_items[4].enable=0;
			
	}
#endif	
	ret = test_item_ctrl(&mem_main_container, autotest);
	 
	return ret;
}

extern struct test_item_container mem_range_test_container;

extern int
mem_range_test(int autotest)
{
	int ret=0,tmp, burn, cnt=1;

	printf("test all range from 0x%08x to 0x%08x\n", mem_start, (mem_end-1));
	test_start_addr = mem_start;
	test_size = mem_size;
	
	if(!autotest){
		printf("test range : 1.all range 2.set range : ");
		scanf("%d\n", &tmp);
		if(tmp == 2){
			printf("mem_start : ");
			scanf("%x", &test_start_addr);
			printf("mem_size : ");
			scanf("%x", &test_size);
		}
		printf("all pattern burn in(y/n)");
		scanf("%c\n", &burn);
		if(burn == 'y' || burn == 'Y'){
			printf("loop : ");
			scanf("%d\n", &burn);
			printf("Start test from 0x%08x to 0x%08x\n", test_start_addr, (test_start_addr+test_size-1));
			while(burn--){
				printf("loop : %d\n", cnt);
				ret = test_item_ctrl(&mem_range_test_container, 1);
				cnt ++;
				if(ret == -1)
					return ret;
			}
			return ret;
		}		
	}
	
	printf("Start test from 0x%08x to 0x%08x\n", test_start_addr, (test_start_addr+test_size-1));
	ret = test_item_ctrl(&mem_range_test_container, autotest);
	
	return ret;
}

extern int 
mem_pattern0_test(int autotest)
{
	int ret;

	printf("PATTERN0 (0x%08x) test", MEM_PATTERN0);
	ret = mem_rw_test (test_start_addr, test_size, mem_pattern[0], 0);
	return ret;
}
extern int 
mem_pattern1_test(int autotest)
{
	int ret;
	
	printf("PATTERN1 (0x%08x) test", MEM_PATTERN1);
	ret = mem_rw_test (test_start_addr, test_size, mem_pattern[1], 0);
	return ret;
}
extern int 
mem_pattern2_test(int autotest)
{
	int ret;
	
	printf("PATTERN2 (0x%08x) test", MEM_PATTERN2);
	ret = mem_rw_test (test_start_addr, test_size, mem_pattern[2], 0);
	return ret;
}
extern int 
mem_pattern3_test(int autotest)
{
	int ret;
	
	printf("PATTERN3 (0x%08x) test", MEM_PATTERN3);
	ret = mem_rw_test (test_start_addr, test_size, mem_pattern[3], 0);
	return ret;
}
extern int 
mem_pattern4_test(int autotest)
{
	int ret;
	
	printf("PATTERN4 (0x%08x) test", MEM_PATTERN4);
	ret = mem_rw_test (test_start_addr, test_size, mem_pattern[4], 0);
	return ret;
}
extern int 
mem_pattern5_test(int autotest)
{
	int ret;
	
	printf("PATTERN5 (0x%08x) test", MEM_PATTERN5);
	ret = mem_rw_test (test_start_addr, test_size, mem_pattern[5], 0);
	return ret;
}
extern int 
mem_pattern6_test(int autotest)
{
	int ret;
	
	printf("PATTERN6 (0x%08x) test", MEM_PATTERN6);
	ret = mem_rw_test (test_start_addr, test_size, mem_pattern[6], 0);
	return ret;
}
extern int 
mem_pattern7_test(int autotest)
{
	int ret;
	
	printf("PATTERN7 (0x%08x) test", MEM_PATTERN7);
	ret = mem_rw_test (test_start_addr, test_size, mem_pattern[7], 0);
	return ret;
}
extern int 
mem_pattern8_test(int autotest)
{
	int ret;
	
	printf("PATTERN8 (addr) test");
	ret = mem_rw_test (test_start_addr, test_size, 0,1);
	return ret;
}

extern struct test_item_container mem_bank_test_container;

extern int
mem_bank_test(int autotest)
{
	int count, ret;
		
	for(count=0;count<SOCLE_MEMORY_BANKS;count++){
		addr_start[count] = BANK_ADDRESS[count] + 0x200000;
		bank_size[count] = 0x10000;
	}
	
	ret = test_item_ctrl(&mem_bank_test_container, autotest);

	return ret;
}

extern int
mem_align_test(int autotest)
{
	u32 *addr;
	
	addr = (u32 *)(mem_start & 0xfffe0000) + 0x8000;
	while ((u32)addr < mem_end){
		*addr = (u32) addr;
		addr += 0x8000;
	}
	
	addr = (u32 *)(mem_start & 0xfffe0000) + 0x8000;
	while((u32)addr < mem_end){
		if(*addr != (u32)addr){			
			printf("error occur : addr : 0x%08x (0x%08x), value : 0x%08x\n", addr, addr, *addr);
			return -1;
		}
		addr += 0x8000;
	}
	
	return 0;	
}

extern int
repeat_write_test(int autotest)
{
	u32 *addr;
	u32 data;
	int loop, tmp;

	printf("repeat write someone address\n");
	printf("address : ");
	scanf("%x\n", &addr);
	printf("dara : ");
	scanf("%x", &data);
	printf("loop : ");
	scanf("%x", &loop);
       		
	for(tmp=0;tmp<loop;tmp++){
		*addr = data;
		if(*addr != data)
			return -1;
	}

	return 0;
}

extern int
mem_bank_normal_test(int autotest)
{
	return 	mem_bank_pattern_test(0);
}

extern int
mem_bank_burn_in_test(int autotest)
{
	return 	mem_bank_pattern_test(1);
}

static int
mem_rw_test (u32 start, u32 bank_size, u32 pattern, u8 type)
{
	register u32	end = start + bank_size;
	register u32	*p;
	u32 count = 0;
	int ret = 0;

	p = (u32 *)start;
	if ( bank_size <= 0 ){
		printf("error bank size\n");
		return -1;
	}	if ( bank_size <= 0 ){
		printf("error bank size\n");
		return -1;
	}
	if(type==0){
		while ((u32)p < end) {
			*p = pattern;
			if((count & 0x1fffff) == 0)
				printf(".");
			p ++;		
			count ++;
		}
		p = (u32 *)start;
		while ((u32)p < end) {
			if (pattern != *((volatile u32 *)p)) {
				printf("!!!!!!!!!!!! Test Error at address(%08X) (%08x) Get(%08x)\n", (u32)p, pattern, *p);
				ret = -1;
			}
			if((count & 0x1fffff) == 0)
				printf(".");
			p ++;		
			count ++;
		}
	}else{
		while ((u32)p < end) {
			*p = (u32)p;
			if((count & 0x1fffff) == 0)
				printf(".");
			p ++;		
			count ++;
		}
		p = (u32 *)start;
		while ((u32)p < end) {
			if ((u32)p != *((volatile u32 *)p)) {
				printf("!!!!!!!!!!!! Test Error at address(%08X) (%08x) Get(%08x)\n", (u32)p, (u32)p, *p);
				return -1;
			}
			if((count & 0x1fffff) == 0)
				printf(".");
			p ++;		
			count ++;
		}
	}
	printf("\n");

	return ret;
}

static int
mem_bank_pattern_test(int burn)
{	
	int ret, pat, bank;
	
	do{
		for (bank = 0; bank < SOCLE_MEMORY_BANKS; bank++) {                
			printf("Start Test Special Memory Address (%08x), Size(%08x)\n", addr_start[bank], bank_size[bank]);
			for(pat=0;pat<sizeof(mem_pattern);pat++)
				printf("    ---- Test Pattern : %08x\n", mem_pattern[pat]);
				ret = mem_rw_test(addr_start[bank], bank_size[bank], mem_pattern[pat], 0);
				if (ret != 0)
					return -1;
		}
	}while(burn);
	
	return 0;
}

extern int
scdk_sram_test(int autotest)
{
	register u32 sram_start = 0x1cb20000;
	register u32 sram_size = 0x00200000;
	register u32 sram_end = sram_start + sram_size;
	register u32	*p;
	iowrite32(0x11,0x1cd28020);
	u32 count = 0;
	int ret = 0;

	p = (u32 *)sram_start;

	
	while ((u32)p < sram_end) {
		*p = (u32)p;
		if ((u32)p != *((volatile u32 *)p)) {
			printf("!!!!!!!!!!!! Test Error at address(%08X) (%08x) Get(%08x)\n", (u32)p, (u32)p, *p);
			return -1;
		}
		if((count & 0x7ffff) == 0)
			printf(".");
		p++;		
		count++;
	}
	printf("\n");

	return ret;
}

extern struct test_item_container msdr_test_container;

extern int
msdr_rw_test(int autotest)
{
	int ret;
	ret = test_item_ctrl(&msdr_test_container, autotest);	 
	return ret;
}

extern int
msdr_write_test(int autotest)
{
	int ret = 0;
	//register u32	end = mem_end;
	register u32	end =(u32)SOCLE_MEMORY_ADDR_START+0x400000;
	register u32	*p;
	
	p = (u32 *)mem_start;
	
	while ((u32)p < end) {
			*p = (u32)p;
			//if((count & 0x1fffff) == 0)
			//	printf(".");
			p ++;		
			//count ++;
	}
	
	return ret;
}

extern int
msdr_compare_test(int autotest)
{
	int ret = 0;
	//register u32	end = mem_end;
	register u32	end =(u32)SOCLE_MEMORY_ADDR_START+0x400000;
	register u32	*p;
	
	p = (u32 *)mem_start;
	
	while ((u32)p < end) {
			if ((u32)p != *((volatile u32 *)p)) {
				printf("!!!!!!!!!!!! Test Error at address(%08X) (%08x) Get(%08x)\n", (u32)p, (u32)p, *p);
				return -1;
			}
			//if((count & 0x1fffff) == 0)
			//	printf(".");
			p ++;		
			//count ++;
	}
	
	return ret;
}
