#include <platform.h>

#ifndef SOCLE_VIP_BASE
#define SOCLE_VIP_BASE 0xB8060000
#endif

#ifndef SOCLE_INTC_VIP
#define SOCLE_INTC_VIP 18
#endif

#define FRAMESIZE_QCIF 0   //176x144
#define FRAMESIZE_CIF  1   //352x288
#define FRAMESIZE_QVGA 2   //320x240
#define FRAMESIZE_VGA  3   //640x480
#define FRAMESIZE_D1   4   //720x480 or 720*576

#define FORMAT_NTSC 0
#define FORMAT_PAL 1

#define FRAME1 1
#define FRAME2 2

#define CONTINUOUS 0
#define ONE_FRAME 1
#define TWO_FRAME 2
extern void vipReset(void);
extern void vipStart(void);
extern void vipStop(void);
extern int vipSetOutFormat(int format);
extern int vipSetFrameSize(int framesize);
extern int vipSetFrameMode(int frame_mode);
extern int vipSetFrameCaptureAddr(int frameNum, u32 Y_pt, u32 Cb_pt, u32 Cr_pt);
extern int vipSet422Output(int en);

