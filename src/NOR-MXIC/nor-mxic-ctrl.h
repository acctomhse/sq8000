#ifndef _flash_h_included_
#define _flash_h_included_

int NOR_MXIC_Test();
int mxic_flash_write(unsigned long flashoffset, unsigned int Data);
int mxic_program(unsigned long offsetaddr, unsigned int data );
int mxic_flash_erase_sector(int sector_num);
int mxic_sector_write(int sector);

#endif // _flash_h_included_
