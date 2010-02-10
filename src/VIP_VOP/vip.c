#include <platform.h>
#include <irqs.h>
#include <io.h>
#include <type.h>
#include "vip-regs.h"
#include "vip.h"

static u32 int_sts;		
static u32 frame_sts;
static void vipOneFrameIsr(void *pparam);
static void vipTwoFrameIsr(void *pparam);

static inline void
socle_vip_write(u32 val, u32 reg)
{
	iowrite32(val, SOCLE_VIP_BASE+reg);
}

static inline u32
socle_vip_read(u32 reg)
{
	return ioread32(SOCLE_VIP_BASE+reg);
}

extern int
vipSetOutFormat(int format)
{
	if(format==FORMAT_NTSC)
		socle_vip_write(socle_vip_read(SOCLE_VIP_CTRL) & ~VIP_CTRL_FORMAT_PAL, SOCLE_VIP_CTRL);
	else if(format==FORMAT_PAL)
		socle_vip_write(socle_vip_read(SOCLE_VIP_CTRL) | VIP_CTRL_FORMAT_PAL, SOCLE_VIP_CTRL);
	else {
		printf("VIP : Error Output Format!!\n");
		return -1;
	}
	return 0;	
}

extern int
vipSet422Output(int en)
{
	if(en)
		socle_vip_write(socle_vip_read(SOCLE_VIP_CTRL) | VIP_CTRL_422_OUTPUT, SOCLE_VIP_CTRL);
	else if(en==0)
		socle_vip_write(socle_vip_read(SOCLE_VIP_CTRL) & ~VIP_CTRL_422_OUTPUT, SOCLE_VIP_CTRL);
	else {
		printf("VIP : Error Output Data Format!!\n");
		return -1;
	}
	return 0;
	
}

extern int
vipSetFrameSize(int framesize)
{
	switch(framesize)
  {
  	case FRAMESIZE_QCIF:
    	socle_vip_write(0x00B00090, SOCLE_VIP_FS);
    	break;
    case FRAMESIZE_CIF:
      socle_vip_write(0x01600120, SOCLE_VIP_FS);
      break;
    case FRAMESIZE_QVGA:
      socle_vip_write(0x014000F0, SOCLE_VIP_FS);
      break;
    case FRAMESIZE_VGA:
      socle_vip_write(0x028001E0, SOCLE_VIP_FS);
      break;
    case FRAMESIZE_D1:
      socle_vip_write(0x02D001E0, SOCLE_VIP_FS);
      break;
    default:
    	printf("VIP : Error Frame Size!!\n");
    	return -1;
	}
	return 0;
}

extern int 
vipSetFrameCaptureAddr(int frameNum, u32 Y_pt, u32 Cb_pt, u32 Cr_pt)
{
	if(frameNum==FRAME1) {
		socle_vip_write(Y_pt, SOCLE_VIP_CAPTURE_F1SA_Y);
		socle_vip_write(Cb_pt, SOCLE_VIP_CAPTURE_F1SA_Cb);
		socle_vip_write(Cr_pt, SOCLE_VIP_CAPTURE_F1SA_Cr);
	}
	else if(frameNum==FRAME2) {
		socle_vip_write(Y_pt, SOCLE_VIP_CAPTURE_F2SA_Y);
		socle_vip_write(Cb_pt, SOCLE_VIP_CAPTURE_F2SA_Cb);
		socle_vip_write(Cr_pt, SOCLE_VIP_CAPTURE_F2SA_Cr);
	}
	else {
		printf("VIP : Error Frame Number!!\n");
    return -1;
	}
	return 0;
}

extern int
vipSetFrameMode(int frame_mode)
{
	switch(frame_mode) {
		case ONE_FRAME:
			socle_vip_write(socle_vip_read(SOCLE_VIP_CTRL) | VIP_CTRL_ONE_FRAME_STOP, SOCLE_VIP_CTRL);
			request_irq(SOCLE_INTC_VIP,vipOneFrameIsr,NULL);
			break;
		case TWO_FRAME:
			socle_vip_write((socle_vip_read(SOCLE_VIP_CTRL) & ~VIP_CTRL_ONE_FRAME_STOP) | VIP_CTRL_PING_PONG_MODE, SOCLE_VIP_CTRL);
			request_irq(SOCLE_INTC_VIP,vipTwoFrameIsr,NULL);
			break;
		case CONTINUOUS:
			socle_vip_write(socle_vip_read(SOCLE_VIP_CTRL) & ~(VIP_CTRL_ONE_FRAME_STOP | VIP_CTRL_PING_PONG_MODE), SOCLE_VIP_CTRL);
			break;
		default:
    	printf("VIP : Error Frame Mode!!\n");
    	return -1;
	}
	return 0;
}

extern void
vipReset()
{
#ifdef CONFIG_PC9220
	socle_vip_write(VIP_RESET, SOCLE_VIP_RESET);
#else
	socle_vip_write(VIP_CTRL_RESET, SOCLE_VIP_CTRL);
#endif
	MSDELAY(260);
	socle_vip_write(VIP_INT_MASK_DISABLE, SOCLE_VIP_INT_MASK);
	socle_vip_write(VIP_CTRL_CAPTURE_DIS, SOCLE_VIP_CTRL);
}

extern void
vipStart()
{
	socle_vip_write(VIP_AHBR_CTRL_INCR16, SOCLE_VIP_AHBR_CTRL);
	socle_vip_write(socle_vip_read(SOCLE_VIP_FB_SR) & ~VIP_FBS_FRAME1_DATA_READY, SOCLE_VIP_FB_SR);
	socle_vip_write(socle_vip_read(SOCLE_VIP_FB_SR) & ~VIP_FBS_FRAME2_DATA_READY, SOCLE_VIP_FB_SR);
	socle_vip_write(VIP_INT_MASK_CAPTURE_COMPLETE | VIP_INT_MASK_CAPTURE_FRAME_LOSS, SOCLE_VIP_INT_MASK);
	socle_vip_write(socle_vip_read(SOCLE_VIP_CTRL) | VIP_CTRL_CAPTURE_EN, SOCLE_VIP_CTRL);
}

extern void
vipStop ()
{  
  socle_vip_write(VIP_INT_MASK_DISABLE, SOCLE_VIP_INT_MASK);
  socle_vip_write(VIP_CTRL_CAPTURE_DIS, SOCLE_VIP_CTRL);
  free_irq(SOCLE_INTC_VIP);
}

static void
vipOneFrameIsr(void *pparam)
{
	int_sts = socle_vip_read(SOCLE_VIP_INT_STS);  
  frame_sts = socle_vip_read(SOCLE_VIP_FB_SR);
  
  if (int_sts & VIP_INT_STS_CAPTURE_COMPLETE) {    
  	if (frame_sts & VIP_FBS_FRAME1_DATA_READY)
  		socle_vip_write(frame_sts & ~VIP_FBS_FRAME1_DATA_READY, SOCLE_VIP_FB_SR);
  } 
  //if (int_sts & VIP_INT_STS_CAPTURE_FRAME_LOSS)
	//	printf("VIP : Capture frame loss!!\n");
}


static void
vipTwoFrameIsr(void *pparam)
{
  int_sts = socle_vip_read(SOCLE_VIP_INT_STS); 
 
  frame_sts = socle_vip_read(SOCLE_VIP_FB_SR);
  
  if (int_sts & VIP_INT_STS_CAPTURE_COMPLETE) {    
  	if (frame_sts & VIP_FBS_FRAME1_DATA_READY) {
  		socle_vip_write(socle_vip_read(SOCLE_VIP_CAPTURE_F1SA_Y),SOCLE_VIP_CAPTURE_F1SA_Y);
  		socle_vip_write(socle_vip_read(SOCLE_VIP_CAPTURE_F1SA_Cb),SOCLE_VIP_CAPTURE_F1SA_Cb);
  		socle_vip_write(socle_vip_read(SOCLE_VIP_CAPTURE_F1SA_Cr),SOCLE_VIP_CAPTURE_F1SA_Cr);
  		socle_vip_write(frame_sts & ~VIP_FBS_FRAME1_DATA_READY, SOCLE_VIP_FB_SR);
  	}
  	if (frame_sts & VIP_FBS_FRAME2_DATA_READY) {
  		socle_vip_write(socle_vip_read(SOCLE_VIP_CAPTURE_F2SA_Y),SOCLE_VIP_CAPTURE_F2SA_Y);
  		socle_vip_write(socle_vip_read(SOCLE_VIP_CAPTURE_F2SA_Cb),SOCLE_VIP_CAPTURE_F2SA_Cb);
  		socle_vip_write(socle_vip_read(SOCLE_VIP_CAPTURE_F2SA_Cr),SOCLE_VIP_CAPTURE_F2SA_Cr);
  		socle_vip_write(frame_sts & ~VIP_FBS_FRAME2_DATA_READY, SOCLE_VIP_FB_SR);
  	}
  }
  //if (int_sts & VIP_INT_STS_CAPTURE_FRAME_LOSS)
	//	printf("VIP : Capture frame loss!!\n");
}

