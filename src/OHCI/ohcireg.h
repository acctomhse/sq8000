#ifndef	_ohci_h_
#define	_ohci_h_

//OHCI Register================================================
#define OHCI_ED_SKIP	(1 << 14)

/*
 * The HCCA (Host Controller Communications Area) is a 256 byte
 * structure defined in the OHCI spec. that the host controller is
 * told the base address of.  It must be 256-byte aligned.
 */
 
#define NUM_INTS 32	/* part of the OHCI standard */

//typedef _ohci_hcca ohci_hcca;  
/*
 * Maximum number of root hub ports.  
 */
#define MAX_ROOT_PORTS	15	/* maximum OHCI root hub ports */

/*
 * This is the structure of the OHCI controller's memory mapped I/O
 * region.  This is Memory Mapped I/O.  You must use the readl() and
 * writel() macros defined in asm/io.h to access these!!
 */
 /*-------------------------------------------------------------------------*/
/* Operational Register Define */
/*-------------------------------------------------------------------------*/
#define  OHCI_BASE 	SOCLE_AHB0_UHC	/*0x180a4000*//*0x1e6e0000*/

#define   OhciReg							(OHCI_BASE)
	#define 	HcRevision(base)					(base + 0x00)
	#define 	HcControl(base)						(base + 0x04)
	#define 	HcCMDSTS(base)						(base + 0x08)
	#define 	HcINTRSTS(base)						(base + 0x0C)
	#define 	HcINTREN(base)						(base + 0x10)
	#define 	HcINTRDISEN(base)					(base + 0x14)
	/* memory pointers */
	#define 	HcHCCA(base)						(base + 0x18)
	#define 	HcPeriodCurrED(base)					(base + 0x1C)
	#define 	HcCtrlHeadED(base)					(base + 0x20)
	#define 	HcCtrlCurrED(base)					(base + 0x24)
	#define 	HcBulkHeadED(base)					(base + 0x28)
	#define 	HcBulkCurrED(base)					(base + 0x2C)
	#define 	HcDoneHead(base)					(base + 0x30)
	/* frame counters */	
	#define 	HcFmIntr(base)						(base + 0x34)
	#define		HcFmremaining(base)					(base + 0x38)	
	#define		Hcfmnumber(base)					(base + 0x3C)	
	#define		Hcperiodicstart(base)					(base + 0x40)	
	#define		Hclsthresh(base)					(base + 0x44)	
	/* Root Hub  & Port Status registers */
	#define		HcRhDscrA(base)						(base + 0x48)	
	#define		HcRhDscrB(base)						(base + 0x4c)	
	#define		HcRhStatus(base)					(base + 0x50)	
	#define		HcRhPortStatus(base)					(base + 0x54)	




/* OHCI CONTROL AND STATUS REGISTER MASKS */

/*
 * HcControl (control) register masks
 */
#define OHCI_CTRL_CBSR	(3 << 0)	/* control/bulk service ratio */
#define OHCI_CTRL_PLE	(1 << 2)	/* periodic list enable */
#define OHCI_CTRL_IE	(1 << 3)	/* isochronous enable */
#define OHCI_CTRL_CLE	(1 << 4)	/* control list enable */
#define OHCI_CTRL_BLE	(1 << 5)	/* bulk list enable */
#define OHCI_CTRL_HCFS	(3 << 6)	/* host controller functional state */
#define OHCI_CTRL_IR	(1 << 8)	/* interrupt routing */
#define OHCI_CTRL_RWC	(1 << 9)	/* remote wakeup connected */
#define OHCI_CTRL_RWE	(1 << 10)	/* remote wakeup enable */

/* pre-shifted values for HCFS */
#define OHCI_USB_RESET	(0 << 6)
#define OHCI_USB_RESUME	(1 << 6)
#define OHCI_USB_OPER	(2 << 6)
#define OHCI_USB_SUSPEND	(3 << 6)

/*
 * HcCommandStatus (cmdstatus) register masks
 */
#define OHCI_HCR	(1 << 0)	/* host controller reset */
#define OHCI_CLF  	(1 << 1)	/* control list filled */
#define OHCI_BLF  	(1 << 2)	/* bulk list filled */
#define OHCI_OCR  	(1 << 3)	/* ownership change request */
#define OHCI_SOC  	(3 << 16)	/* scheduling overrun count */

/*
 * masks used with interrupt registers:
 * HcInterruptStatus (intrstatus)
 * HcInterruptEnable (intrenable)
 * HcInterruptDisable (intrdisable)
 */
#define OHCI_INTR_SO	(1 << 0)	/* scheduling overrun */
#define OHCI_INTR_WDH	(1 << 1)	/* writeback of done_head */
#define OHCI_INTR_SF	(1 << 2)	/* start frame */
#define OHCI_INTR_RD	(1 << 3)	/* resume detect */
#define OHCI_INTR_UE	(1 << 4)	/* unrecoverable error */
#define OHCI_INTR_FNO	(1 << 5)	/* frame number overflow */
#define OHCI_INTR_RHSC	(1 << 6)	/* root hub status change */
#define OHCI_INTR_OC	(1 << 30)	/* ownership change */
#define OHCI_INTR_MIE	(1 << 31)	/* master interrupt enable */


#endif /* _ohcireg_h_ */
