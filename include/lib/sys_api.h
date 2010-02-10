#ifndef _SYS_API_H_
#define _SYS_API_H_


#define SYS_TLB_OK                      0
#define SYS_TLB_NOTFOUND                1
#define SYS_TLB_NOTVALID                2
#define SYS_TLB_WP                      3

#define KUSEG_MSK		  0x80000000
#define KSEG_MSK		  0xE0000000
#define KUSEGBASE		  0x00000000
#define KSEG0BASE		  0x80000000
#define KSEG1BASE		  0xA0000000
#define KSSEGBASE		  0xC0000000
#define KSEG3BASE		  0xE0000000


#define SET_MIPS3() 	.##set mips3
#define KSEG1A(reg) 	and reg, ~KSEG_MSK; or reg, KSEG1BASE
#define KSEG0(addr)     (((addr) & ~KSEG_MSK)  | KSEG0BASE)
#define KSEG1(addr)     (((addr) & ~KSEG_MSK)  | KSEG1BASE)

#ifndef _ASSEMBLER_

#include "type.h"

/************************************************************************
 *
 *                          sys_dcache_flush_all
 *  Description :
 *  -------------
 *
 *  Flush entire DCACHE.
 *
 *  We bypass the cache operations if CPU is running uncached.
 *  (We assume Config.k0 == K_CacheAttrU is the
 *  only uncached mode used).
 *
 *  Return values :
 *  ---------------
 *
 *  None
 *
 ************************************************************************/
void
sys_dcache_flush_all( void );


/************************************************************************
 *
 *                          sys_icache_invalidate_all
 *  Description :
 *  -------------
 *
 *  Invalidate entire ICACHE.
 *
 *  We bypass the cache operations if CPU is running uncached.
 *  (We assume Config.k0 == K_CacheAttrU is the
 *  only uncached mode used).
 *
 *  Return values :
 *  ---------------
 *
 *  None
 *
 ************************************************************************/
void
sys_icache_invalidate_all( void );

/************************************************************************
 *
 *                          sys_dcache_flush_index
 *  Description :
 *  -------------
 *
 *  Flush D cache line containing specified index
 *
 *  Return values :
 *  ---------------
 *
 *  None
 *
 ************************************************************************/
void
sys_dcache_flush_index( u32_t index );

/************************************************************************
 *
 *                          sys_icache_invalidate_index
 *  Description :
 *  -------------
 *
 *  Fill I cache line containing specified index
 *
 *  Return values :
 *  ---------------
 *
 *  None
 *
 ************************************************************************/
void
sys_icache_invalidate_index( u32_t index );

/************************************************************************
 *
 *                          sys_flush_pipeline
 *  Description :
 *  -------------
 *
 *  Flush pipeline
 *
 *  Return values :
 *  ---------------
 *
 *  None
 *
 ************************************************************************/
void
sys_flush_pipeline( void );

/************************************************************************
 *
 *                          sys_cp0_read32
 *  Description :
 *  -------------
 *
 *  Read 32 bit CP0 register
 *
 *  Return values :
 *  ---------------
 *
 *  Value read
 *
 ************************************************************************/
u32_t
sys_cp0_read32(
    u32_t number,      /* Register number (0..31) */
    u32_t sel );       /* sel field (0..7)        */
/************************************************************************
 *
 *                          sys_tlb_lookup
 *  Description :
 *  -------------
 *
 *  Probe TLB for matching entry
 *
 *  Return values :
 *  ---------------
 *
 *  SYS_TLB_NOTFOUND : No match
 *  SYS_TLB_NOTVALID : Match with valid bit cleared, i.e. not valid
 *  SYS_TLB_WP	     : Match with dirty bit cleared, i.e. write-protected
 *  SYS_TLB_OK       : Valid and Dirty entry found
 *
 ************************************************************************/
u32_t
sys_tlb_lookup(
    u32_t vaddr,		/* Virtual address			*/
    u32_t *phys,		/* OUT : Physical address		*/
    u32_t *pagesize );		/* OUT : Pagesize (byte count)		*/


/************************************************************************
 *
 *                          sys_tlb_write
 *  Description :
 *  -------------
 *
 *  Write TLB
 *
 *  data = pointer to array of 5 words
 * 
 *  array[0] = index
 *  array[1] = pagemask
 *  array[2] = entryhi
 *  array[3] = entrylo0
 *  array[4] = entrylo1
 *	
 *  Return values :
 *  ---------------
 *
 *  None
 *
 ************************************************************************/
void
sys_tlb_write(
    u32_t *data );
/************************************************************************
 *
 *                          sys_tlb_read
 *  Description :
 *  -------------
 *
 *  Read TLB
 *
 *  data = pointer to array of 4 words. They will be filled with  the
 *         following data :
 *
 *  array[0] = pagemask
 *  array[1] = entryhi
 *  array[2] = entrylo0
 *  array[3] = entrylo1
 *	
 *  Return values :
 *  ---------------
 *
 *  None
 *
 ************************************************************************/
void
sys_tlb_read(
    u32_t index,
    u32_t *data );
/************************************************************************	
 *
 *                          sys_cpu_k0_config
 *  Description :
 *  -------------
 *
 *  Configure K0 field of CP0 CONFIG register
 *
 *  Return values :
 *  ---------------
 *
 *  None
 *
 ************************************************************************/
void 
sys_cpu_k0_config( 
    u32_t k0 );		/* K0 field to be written		*/
/************************************************************************
 *
 *                          sys_cp0_read32
 *  Description :
 *  -------------
 *
 *  Read 32 bit CP0 register
 *
 *  Return values :
 *  ---------------
 *
 *  Value read
 *
 ************************************************************************/
u32_t
sys_cp0_read32(
    u32_t number,	/* Register number (0..31) */
    u32_t sel );	/* sel field (0..7)	   */
/************************************************************************
 *
 *                          sys_cp0_write32
 *  Description :
 *  -------------
 *
 *  Write 32 bit CP0 register
 *
 *  Return values :
 *  ---------------
 *
 *  None
 *
 ************************************************************************/
void
sys_cp0_write32(
    u32_t number,	/* Register number (0..31) */
    u32_t sel,		/* sel field (0..7)	   */
    u32_t value );	/* Value to be written	   */
    
    
/************************************************************************
 *
 *                          sys_enable_int
 *  Description :
 *  -------------
 *
 *  Enable interrupt: set IE in CP0-status.
 *  
 *  Return values :
 *  ---------------
 *
 *  None
 *
 ************************************************************************/
void 
sys_enable_int( void );


/************************************************************************
 *
 *                          sys_enable_int_mask
 *  Description :
 *  -------------
 *
 *  Enable specific CPU interrupt.
 *  
 *  Return values :
 *  ---------------
 *
 *  None
 *
 ************************************************************************/
void 
sys_enable_int_mask( 
    u32_t cpu_int );		/* CPU interrupt 0..7			*/


/************************************************************************
 *
 *                          sys_disable_int_mask
 *  Description :
 *  -------------
 *
 *  Disable specific CPU interrupt.
 *  
 *  Return values :
 *  ---------------
 *
 *  None
 *
 ************************************************************************/
void 
sys_disable_int_mask( 
    u32_t cpu_int );		/* CPU interrupt 0..7			*/


/************************************************************************
 *
 *                          sys_disable_int
 *  Description :
 *  -------------
 *
 *  Disable interrupt: clear IE in CP0-status.
 *  
 *  Return values :
 *  ---------------
 *
 *  Old IE bit
 *
 ************************************************************************/
u32_t 
sys_disable_int( void );

    
#endif // _ASSEMBLER_
#endif // _SYS_API_H_
