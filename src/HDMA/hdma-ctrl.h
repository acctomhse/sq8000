#ifndef _hdmactrl_h_included_
#define _hdmactrl_h_included_

extern int		HDMATesting (int autotest);
static void		socle_hdma_make_test_pattern(u8 *mem, u32 cnt);
static void		socle_hdma_clear_memory(u8 *mem, u32 cnt);	
static int		socle_hdma_compare_memory(u8 *mem, u8 *cmpr_mem, u32 cnt, 
			     u32 data_size, u32 src_dir, u32 dst_dir);
extern int		socle_hdma_software_dma_tranfer(int autotest);	
extern int 		socle_hdma_show_setting(int autotest);

static void socle_hdma_notifier_complete(void *data);
				

#endif // _hdmactrl_h_included_
