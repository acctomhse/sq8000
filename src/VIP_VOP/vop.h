#include <platform.h>

#ifdef CONFIG_PC9220
#define SOCLE_VOP_BASE SOCLE_AHB0_VOP
#else
#define SOCLE_VOP_BASE 0xB8080000
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

#define ONE_FRAME 1
#define TWO_FRAME 2
extern void vopReset(void);
extern void vopStart(void);
extern void vopStop(void);
extern int vopSetOutFormat(int format);
extern int vopSetFrameSize(int framesize);
extern int vopSetFrameMode(int frame_mode);
extern int vopSetFrameDisplayAddr(int frameNum, u32 Y_pt, u32 Cb_pt, u32 Cr_pt);

