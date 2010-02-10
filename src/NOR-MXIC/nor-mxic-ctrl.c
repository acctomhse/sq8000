#include <genlib.h>
#include <type.h>
#include <io.h>
#include <test_item.h>
#include <global.h>
#include "nor-mxic-ctrl.h"
#include "nor-mxic-regs.h"

static short	flash_total_sec_num;
static unsigned long	flash_start_addr;	/*the absolute start address of flash memory*/

static unsigned long flashsize;
static unsigned int secsize;
static int mode;
						
extern struct test_item_container nor_mxic_main_container;

extern int
NOR_MXIC_Test (int autotest)
{
	int ret=0;

#if defined(CONFIG_PC7210) || defined(CONFIG_PDK)
	iowrite32(0x1, 0x18058320);	// for 2nd nor 16bit
#endif

// 20080714 cyli add for INR
#ifdef CONFIG_INR_PC7230
#include <GPIO/gpio.h>
	// set CS0 chip selection to NOR
	socle_gpio_set_value_with_mask(PA, SHIFT_MASK(5), SHIFT_MASK(5));				// PA5 = 1
#endif

	mode=ioread32(MODE_BASE)&0x1;
	if(mode==1)
		printf("\n Flash run in word mode \n");
	else
		printf("\n Flash run in byte mode \n");
	flashsize=FLASH_SIZE;
	secsize=FLASH_SECTOR_SIZE;
	flash_total_sec_num = FLASH_TOTAL_SEC_NUM;
	ret=test_item_ctrl(&nor_mxic_main_container,autotest);
	return ret;
}


extern struct test_item_container nor_mxic_test_container;

extern int
nor_mxic_test_0 (int autotest)
{
	int ret=0;
			
	flash_start_addr = SOCLE_NOR_BASE0;
	
	ret=test_item_ctrl(&nor_mxic_test_container,autotest);
	return ret;
}

extern int
nor_mxic_test_1 (int autotest)
{
	int ret=0;
	
	flash_start_addr = SOCLE_NOR_BASE1;

	ret=test_item_ctrl(&nor_mxic_test_container,autotest);
	return ret;
}

extern int 
nor_mxic_sector_rw(int autotest)
{
	
	int all=0;
	int start;
	int end;
							
	if(autotest){
		start = flash_total_sec_num-1;
		end = flash_total_sec_num-1;
	}else{
block_break :
		printf("0. set sector range\n");
		printf("1. test all sector\n");
		printf("sector>");
		scanf("%d", &all);
		if(all == 0){
			do{
				printf("set start sector (0~%d): ", (flash_total_sec_num-1));
				scanf("%d\n", &start);
				printf("set end sector (%d~%d): ", start, (flash_total_sec_num-1));
				scanf("%d\n", &end);								
			}while((end > flash_total_sec_num) || (start < 0) || (start > end));
		}else if(all == 1){	
			start = 0;
			end = flash_total_sec_num-1;
		}else{
			printf("input error!!\n");
			goto  block_break;
		}
	}	
	for(; start <= end ; start++){	
		printf("Sector %d r/w testing......\n", start);			
		if(mxic_sector_write(start) == FLASH_NOERROR){
				printf("Pass!!\n");	
		}
		else{
			printf("Fail!!\n");					
			return -1;
		}
	}
	
	return 0;
}

extern int nor_mxic_read(int autotest)
{
	int data;
	unsigned long	flashoffset;
	
	while(1){
		
		printf("address (offset:0~%x) : ",flashsize-1);
		scanf("%x\n", &flashoffset);
		if(flashoffset >= flashsize || flashoffset<0)
			printf("address over the range\n");
		else
			break;
	}
	if(mode==0) {
		data = readb(flash_start_addr+ flashoffset);
		data&=0xff;
  	printf("0x%2x\n", data);
  }
  else {
  	data = ioread16(flash_start_addr+ flashoffset);
  	data&=0xffff;
  	printf("0x%4x\n", data);
  }
	return 0;
}

extern int nor_mxic_write(int autotest)
{
	unsigned int data;
	unsigned long	flashoffset;
	
	while(1){
		printf("address (offset:0~%x) : ",flashsize-1);
		scanf("%x\n", &flashoffset);
		if(flashoffset > flashsize || flashoffset<0)
			printf("address over the range\n");
		else if(mode==1 && flashoffset%2!=0)
			printf("must input an even address at word mode\n");	
		break;
	}
	
	if(mode==0) {
		printf("data (0~255 or 0x..): ");
		scanf("%d\n", &data);
		data &= 0xff;
	
		if(mxic_flash_write(flashoffset,data)==FLASH_NOERROR){
			if(readb(flash_start_addr + flashoffset) == data){
				printf("Write Data Success\n");
				return 0;
			}
			else{
				printf("Write Data Fail\n");
				return -6;
			}
		}
		else{
			printf("---Write Error\n");					
			return 1;	
		}
	}
	else {
		printf("data (0~65536 or 0x....): ");
		scanf("%d\n", &data);
		data &= 0xffff;
	
		if(mxic_flash_write(flashoffset,data)==FLASH_NOERROR){
			printf("Write Data Success\n");
			return 0;
		}
		else{
			printf("Write Error\n");					
			return 1;	
		}
	}
}


extern int nor_mxic_erase_sector(int autotest)
{
	int sector=0;
	int tmp=0;
	unsigned long flashoffset;
	
erase_break :	
	printf("choose the type 0.address (offset) 1.sector number : ");
	scanf("%d\n", &tmp);
	if(tmp == 0){
    	while(1){
			printf("address (offset:0~%x) : ",flashsize-1);
        	scanf("%x\n", &flashoffset);
			if(flashoffset >flashsize)
				printf("address over the range\n");
			else
				break;
		}
    if(flashoffset<secsize)
    	sector = flashoffset/(secsize/8);
    else 
    	sector = (flashoffset/secsize+7);
    	
	}
	else if(tmp == 1){	
		while(1){
			printf("Sector (0~%d): ", (FLASH_TOTAL_SEC_NUM-1));
			scanf("%d\n", &sector);
			if(sector >= FLASH_TOTAL_SEC_NUM || sector<0)
				printf("sector over the range\n");
			else
				break;
		}
	}
	else{
		printf("input error\n");
		goto erase_break;
	}
								
	if(mxic_flash_erase_sector(sector)!=FLASH_NOERROR){
		return -5;
	}
	return 0;
}

extern int nor_mxic_erase_all(int autotest)
{
  if(mode==0) {  	
		writeb(0xAA,flash_start_addr+0xAAA);
		writeb(0x55,flash_start_addr+0x555);
		writeb(0x80,flash_start_addr+0xAAA);
		writeb(0xAA,flash_start_addr+0xAAA);
		writeb(0x55,flash_start_addr+0x555);
		writeb(0x10,flash_start_addr+0xAAA);
		
	}
	else {
		iowrite16(0xAA,flash_start_addr+0xAAA);
		iowrite16(0x55,flash_start_addr+0x554);
		iowrite16(0x80,flash_start_addr+0xAAA);
		iowrite16(0xAA,flash_start_addr+0xAAA);
		iowrite16(0x55,flash_start_addr+0x554);
		iowrite16(0x10,flash_start_addr+0xAAA);
	}
	printf("erasing....\n");
	MSDELAY(38000);
	printf("checking....\n");
	unsigned long i;
	for(i=0;i<flashsize;i++) {
		if(readb(flash_start_addr+i)!=0xFF) {
			printf("Erase Error at 0x%x\n",flash_start_addr+i);
			return -5;
		}
	}	
	printf("Erase All Success\n");
	return 0;
}


int mxic_flash_write(unsigned long flashoffset, unsigned int Data)
{
	unsigned long	startAddr;
	unsigned int	i;
	int	sector;
	unsigned int sector_size;
	
	
	if(flashoffset<secsize) {
		sector_size=secsize/8;
		sector = flashoffset/sector_size;
		startAddr = sector*(sector_size);	
	}
  else {   
  	sector_size=secsize;
   	sector = (flashoffset/secsize+7);
		startAddr = secsize*(sector-7);
	}
	unsigned long addr=flash_start_addr+startAddr;
	
		
	if(mode==0) {
		unsigned int temp[sector_size];
		for(i=0;i<sector_size;i++)
			temp[i] = readb(addr+i);
		temp[flashoffset % sector_size] = Data;
		if(mxic_flash_erase_sector(sector)!=FLASH_NOERROR)
			return (FLASH_ERROR);
		printf("writing...\n");
		for(i=0;i<sector_size;i++) {
			if(mxic_program((startAddr+i), temp[i])!=FLASH_NOERROR)
				return (FLASH_ERROR);
		}				
	}
	else {
		unsigned int temp[sector_size/2];
		for(i=0;i<(sector_size/2);i++)
			temp[i] = ioread16(addr+i*2);
		temp[(flashoffset % sector_size)/2] = Data;
		if(mxic_flash_erase_sector(sector)!=FLASH_NOERROR) 
			return (FLASH_ERROR);
		printf("progaming...\n");
		for(i=0;i<(sector_size/2);i++) {
			if(mxic_program(startAddr+i*2, temp[i])!=FLASH_NOERROR)
				return (FLASH_ERROR);
		}				
	}
	return FLASH_NOERROR;
	
	
	
}

int mxic_program(unsigned long OffsetAddr, unsigned int data )
{		
	int flag=0;
  int cnt=0;
	unsigned long address;
	
	address = flash_start_addr+OffsetAddr;
	
	if(mode==0) {
		writeb(0xAA,flash_start_addr+0xAAA);
		writeb(0x55,flash_start_addr+0x555);
		writeb(0xA0,flash_start_addr+0xAAA);
		writeb(data,address);
		//writeb(0xF0,flash_start_addr);
	  //MSDELAY(10);
		while(flag!=1) {
			cnt++;
			unsigned int tmp = readb(address);
			if(tmp==data) {
				flag=1;
			}
			else { 
				if(cnt>150) {
					printf("write fail!!   address:0x%8x , data:0x%2x\n", address, tmp);
					return (FLASH_ERROR);
				}
			}
		}
	}
	else {
		iowrite16(0xAA,flash_start_addr+0xAAA);
		iowrite16(0x55,flash_start_addr+0x554);
		iowrite16(0xA0,flash_start_addr+0xAAA);
		iowrite16(data,address);
		//iowrite16(0xF0,flash_start_addr);
		//MSDELAY(10);
		while(flag!=1) {
			cnt++;
			unsigned int tmp = ioread16(address);
			if(tmp==data) {
				flag=1;
			}
			else { 
				
				if(cnt>150) {
					printf("write fail!!   address:0x%8x , data:0x%4x\n", address, tmp);
					return (FLASH_ERROR);
				}
			}
		}
	}

  
	return FLASH_NOERROR;
}

/* flash_num and sector_num are 0 based. */
int mxic_flash_erase_sector(int sector_num)
{
	
	int i,tmp;
	unsigned long	startaddr;
	unsigned int sector_size;
	if(sector_num<8) {
		sector_size=secsize/8;
		startaddr = flash_start_addr + sector_size * sector_num;
		
	}
	else {
		sector_size=secsize;
		startaddr = flash_start_addr + secsize*(sector_num-7);
	}
	if(mode==0) { 
		writeb(0xAA,flash_start_addr+0xAAA);
		writeb(0x55,flash_start_addr+0x555);
		writeb(0x80,flash_start_addr+0xAAA);
		writeb(0xAA,flash_start_addr+0xAAA);
		writeb(0x55,flash_start_addr+0x555);
		writeb(0x30,startaddr);
		MSDELAY(3000);
		writeb(0xF0,flash_start_addr);
	}		
		
  else {
		iowrite16(0xAA,flash_start_addr+0xAAA);
		iowrite16(0x55,flash_start_addr+0x554);
		iowrite16(0x80,flash_start_addr+0xAAA);
		iowrite16(0xAA,flash_start_addr+0xAAA);
		iowrite16(0x55,flash_start_addr+0x554);
		iowrite16(0x30,startaddr);
		MSDELAY(3000);
		iowrite16(0xF0,flash_start_addr);
	}

	

	for(i=0;i<sector_size;i++) {
		tmp=readb(startaddr+i);
		if(tmp!=0xFF) {
			printf("Erase Error at 0x%x : 0x%x\n",startaddr, tmp);
			return -5;
		}
	}
	
	printf("Erase Sector %d Success\n",sector_num);
	return FLASH_NOERROR;
}



int mxic_sector_write(int sector)
{
	unsigned long	offaddr;
	unsigned int	i,j;
	unsigned int sector_size;
	
	if(sector<8) {
		sector_size=secsize/8 ;
		offaddr =sector*sector_size ;
	}
	else {
		sector_size=secsize ;
		offaddr = secsize*(sector-7);
	}	


	if(mxic_flash_erase_sector(sector)!=FLASH_NOERROR)
		return (FLASH_ERROR);	
	
	
	if(mode==0) {
		for(i=0;i<sector_size;i++)
			if(mxic_program((offaddr+i), (i%256))!=FLASH_NOERROR)
				return (FLASH_ERROR);
	}
	else {
		j=0;
		for(i=0;i<sector_size;i+=2) {
			if(mxic_program((offaddr+i), (j%65536))!=FLASH_NOERROR)
				return (FLASH_ERROR);
			j++;
		}
	}

	return FLASH_NOERROR;
}
