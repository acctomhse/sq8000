OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS
{
        . = 0x00000000;
        . = ALIGN(4);
        .text : {
          *(.init.text)
          *(.text)
         }
        .rodata : {
          *(.rodata)
         }
        . = ALIGN(4);
        .data : {
          *(.data)
         }
        . = ALIGN(4);
        .got : {
          *(.got)
         }

        .tlb_map : {
         *(.tlb_map)
         }

        . = ALIGN(4);
        __bss_start = .;
        .bss : { *(.bss) }
        __bss_end = .;


        . = ALIGN(4);
	.stack	:
	{
        . += 0x20000;
        __stack_start = ABSOLUTE(.);
        . += 12;
/*      . += CONFIG_STACKSIZE_FIQ  */
        . += 0x1000;
        __fiq_stack_start = ABSOLUTE(.);
/*      . += CONFIG_STACKSIZE_IRQ  */
        . += 0x1000;
/*        __irq_stack_start= .; */
        __irq_stack_start = ABSOLUTE(.);
/*      . += CFG_GBL_DATA_SIZE  */
        . += 0x80;
/*      . += CFG_MALLOC_LEN     */
        . += 0x40000;
        . += 0x20000;

	}

        . = ALIGN(4);
        _end = .;
}



