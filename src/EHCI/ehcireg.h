#ifndef	_uhci_h_
#define	_uhci_h_

#include "type.h"


//======================HCD.h=================================
//bmREquestType	bRequest wvalue wIndex Wlength
//	1			1		2		2		2

/* (shifted) direction/type/recipient from the USB 2.0 spec, table 9.2 */
#define DeviceRequest \
	((USB_DIR_IN|USB_TYPE_STANDARD|USB_RECIP_DEVICE)<<8)
#define DeviceOutRequest \
	((USB_DIR_OUT|USB_TYPE_STANDARD|USB_RECIP_DEVICE)<<8)

#define InterfaceRequest \
	((USB_DIR_IN|USB_TYPE_STANDARD|USB_RECIP_INTERFACE)<<8)

#define EndpointRequest \
	((USB_DIR_IN|USB_TYPE_STANDARD|USB_RECIP_INTERFACE)<<8)
#define EndpointOutRequest \
	((USB_DIR_OUT|USB_TYPE_STANDARD|USB_RECIP_INTERFACE)<<8)

/* table 9.6 standard features */
/*
#define DEVICE_REMOTE_WAKEUP	1
#define ENDPOINT_HALT		0
*/
/* class requests from the USB 2.0 hub spec, table 11-15 */
/* GetBusState and SetHubDescriptor are optional, omitted */
/* redefine
#define ClearHubFeature					(0x2000 | USB_REQ_CLEAR_FEATURE)
#define ClearPortFeature					(0x2300 | USB_REQ_CLEAR_FEATURE)
#define GetHubDescriptor					(0xa000 | USB_REQ_GET_DESCRIPTOR)
#define GetHubStatus						(0xa000 | USB_REQ_GET_STATUS)
#define GetPortStatus						(0xa300 | USB_REQ_GET_STATUS)
#define SetHubFeature					(0x2000 | USB_REQ_SET_FEATURE)
#define SetPortFeature					(0x2300 | USB_REQ_SET_FEATURE)
*/
//===========================================================
/*
//Operational Register Offsets
#define USBCMD			 0
#define USBSTS			 1
#define USBINTR			 2
#define FRINDEX			 3
#define CTRLDSSEGMENT	 4
#define PERIODICLISTBASE 5
#define ASYNCLISTADDR	 6
#define CONFIGFLAG		 16
#define PORTSC1			 17
#define PORTSC2			 18
#define PORTSC3			 19
#define PORTSC4		     		20
#define PORTSC5          	21
#define MAX_PACKET_SIZE  512
*/

/*-------------------------------------------------------------------------*/
/* EHCI Registers Offset Define */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* Capability Register Define */
/*-------------------------------------------------------------------------*/

#define	  CAPREG_OFFSET		0x100
	#define   CAPLENGTH(base)					(base + CAPREG_OFFSET + 0x00)
	/*0x01 Reserved*/
	#define   HCIVERSION(base)					(base + CAPREG_OFFSET + 0x02)
	#define   HCSPARAMS(base)					(base + CAPREG_OFFSET + 0x04)
#if 0
		#define 	HCS_DEBUG_PORT(hcsparams)			((readw(HCSPARAMS)>>20)&0xf)	/* bits 23:20, debug port? */
		#define 	HCS_INDICATOR				(readw(HCSPARAMS)&(1 << 16))	/* true: has port indicators */
		#define 	HCS_N_CC					((readw(HCSPARAMS)>>12)&0xf)	/* bits 15:12, #companion HCs */
		#define 	HCS_N_PCC					((readw(HCSPARAMS)>>8)&0xf)	/* bits 11:8, ports per CC */
		#define 	HCS_PORTROUTED			(readw(HCSPARAMS)&(1 << 7))	/* true: port routing */ 
		#define 	HCS_PPC					(readw(HCSPARAMS)&(1 << 4))	/* true: port power control */ 
		#define 	HCS_N_PORTS				((readw(HCSPARAMS)>>0)&0xf)	/* bits 3:0, ports on HC */
#endif
	#define   HCCPARAMS(base)					(base + CAPREG_OFFSET + 0x08)
#if 0
		#define 	HCC_EXT_CAPS				((readw(HCCPARAMS)>>8)&0xff)	/* for pci extended caps */
		#define 	HCC_ISOC_CACHE       		(readw(HCCPARAMS)&(1 << 7))  /* true: can cache isoc frame */
		#define 	HCC_ISOC_THRES       		((readw(HCCPARAMS)>>4)&0x7)  /* bits 6:4, uframes cached */
		#define 	HCC_CANPARK				(readw(HCCPARAMS)&(1 << 2))  /* true: can park on async qh */
		#define 	HCC_PGM_FRAMELISTLEN 	(readw(HCCPARAMS)&(1 << 1))  /* true: periodic_size changes*/
		#define 	HCC_64BIT_ADDR       		(readw(HCCPARAMS)&(1))       /* true: can use 64-bit addr */
#endif
	#define   HCSP_PORTROUTE(base)			(base + CAPREG_OFFSET + 0x0C)
/*-------------------------------------------------------------------------*/
/* Operational Register Define */
/*-------------------------------------------------------------------------*/
#define   OPERREG_OFFSET	(0x120)
	#define   USBCMD(base)						(base + OPERREG_OFFSET + 0x00)
		/* 23:16 is r/w intr rate, in microframes; default "8" == 1/msec */
		#define 	CMD_PARK					(1<<11)		/* enable "park" on async qh */
		#define 	CMD_PARK_CNT_M					(3<<8)		/* how many transfers to park for */
		#define 	CMD_PARK_CNT_S					8		/* how many transfers to park for */
		#define 	CMD_LRESET					(1<<7)		/* partial reset (no ports, etc) */
		#define 	CMD_IAAD					(1<<6)		/* "doorbell" interrupt async advance */
		#define 	CMD_ASE						(1<<5)		/* async schedule enable */
		#define 	CMD_PSE  					(1<<4)		/* periodic schedule enable */
		/* 3:2 is periodic frame list size */
		#define 	CMD_RESET					(1<<1)		/* reset HC not bus */
		#define 	CMD_RUN						(1<<0)		/* 1:run, 0:stop start/stop HC */
	#define   USBSTS(base)						(base + OPERREG_OFFSET + 0x04)
		#define 	STS_ASS						(1<<15)		/* Async Schedule Status */
		#define 	STS_PSS						(1<<14)		/* Periodic Schedule Status */
		#define 	STS_RECL					(1<<13)		/* Reclamation */
		#define 	STS_HALT					(1<<12)		/* Not running (any reason) */
		/* some bits reserved */
		/* these STS_* flags are also intr_enable bits (USBINTR) */
		#define 	STS_IAA						(1<<5)		/* Interrupted on async advance */
		#define 	STS_FATAL					(1<<4)		/* such as some PCI access errors */
		#define 	STS_FLR						(1<<3)		/* frame list rolled over */
		#define 	STS_PCD						(1<<2)		/* port change detect */
		#define 	STS_ERR						(1<<1)		/* "error" completion (overflow, ...) */
		#define 	STS_INT						(1<<0)		/* "normal" completion (short, ...) */
	#define   USBINTR(base)					(base + OPERREG_OFFSET + 0x08)
	#define   FRINDEX(base)					(base + OPERREG_OFFSET  + 0x0C)
	#define   CTRLDSSEGMENT(base)				(base + OPERREG_OFFSET  + 0x10)
	#define   PERIODICLISTBASE(base)			(base + OPERREG_OFFSET  + 0x14)
	#define   ASYNCICLISTADDR(base)				(base + OPERREG_OFFSET  + 0x18)
	/*1C ~ 3F Reserved*/
	#define   CONFIGFLAG(base)				(base + OPERREG_OFFSET  + 0x40)
		#define 	FLAG_CF					(1<<0)		/* true: we'll support "high speed" */
	#define   PORTSC(base)					(base + OPERREG_OFFSET  + 0x44)
		/* 31:23 reserved */
		#define 	PORT_WKOC_E				(1<<22)		/* wake on overcurrent (enable) */
		#define 	PORT_WKDISC_E				(1<<21)		/* wake on disconnect (enable) */
		#define 	PORT_WKCONN_E				(1<<20)		/* wake on connect (enable) */
		/* 19:16 for port testing */
		#define   PORT_TEST_CTRL				(0xF<<16)		/*0101 enable*/
			#define PORT_J_TEST				(1<<16)
			#define PORT_K_TEST				(2<<16)
			#define PORT_SE0_NAK_TEST			(3<<16)
			#define PORT_PACKET_TEST			(4<<16)
			#define PORT_PORCEEN_TEST			(5<<16)
		/* 15:14 for using port indicator leds (if HCS_INDICATOR allows) */
		#define 	PORT_OWNER				(1<<13)		/* true: companion hc owns this port */
		#define 	PORT_POWER				(1<<12)		/* true: has power (see PPC) */
		#define 	PORT_LINE_STS_M 			(3<<10)		/* for USB 1.1 device */
		#define 	PORT_LINE_STS_S 			(10		/* for USB 1.1 device */
		/* 11:10 for detecting lowspeed devices (reset vs release ownership) */
		/* 9 reserved */
		#define 	PORT_RESET				(1<<8)		/* reset port */
		#define 	PORT_SUSPEND				(1<<7)		/* suspend port */
		#define 	PORT_RESUME				(1<<6)		/* resume it */
		#define 	PORT_OCC				(1<<5)		/* over current change */
		#define 	PORT_OC					(1<<4)		/* over current active */
		#define 	PORT_PEC				(1<<3)		/* port enable change */
		#define 	PORT_PE					(1<<2)		/* port enable */
		#define 	PORT_CSC				(1<<1)		/* connect status change */
		#define 	PORT_CONNECT				(1<<0)		/* device connected */
// ==============================================================


#endif /* _uhci_h_ */
