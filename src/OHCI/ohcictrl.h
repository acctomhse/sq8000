#ifndef  _ohcictrl_h_
#define  _ohcictrl_h_

#include "ohcireg.h"
#include "Interrupt/interrupt.h"
#include "type.h"

/////////////////////////////////////////////////////////////////////////////
//OHCI Data Structure Definitions
/////////////////////////////////////////////////////////////////////////////


/* ED States */

#define ED_NEW 		0x00
#define ED_UNLINK 	0x01
#define ED_OPER		0x02
#define ED_DEL		0x04
#define ED_URB_DEL  	0x08


typedef struct _OHCI_HCCA {
	__u32	int_table[NUM_INTS];	/* Interrupt ED table */
	__u16	frame_no;		/* current frame number */
	__u16	pad1;			/* set to 0 on each frame_no change */
	__u32	done_head;		/* info returned for an interrupt */
	unsigned char 	reserved_for_hc[116];		// changed by viswa from u8 to unsigned char 
} __attribute((aligned(16)))OHCI_HCCA,*POHCI_HCCA;

typedef struct _ED
{
	ULONG EDdata;
	ULONG TailP;    //physical address of tail td to be filled
	ULONG HeadP;	//physical address of first td to be filled
	ULONG NextED;	//physical address of next ed to be filled
} __attribute((aligned(16))) ED,*PED;

/* ED info field */
#define ISOFMT (1 << 15)
#define EPINDIR (2 << 11)
#define EPOUTDIR (1 << 11)
/* TD info field */
#define TD_CC       0xf0000000
#define TD_CC_GET(td_p) ((td_p >>28) & 0x0f)
#define TD_CC_SET(td_p, cc) (td_p) = ((td_p) & 0x0fffffff) | (((cc) & 0x0f) << 28)
#define TD_EC       0x0C000000
#define TD_T        0x03000000
#define TD_T_DATA0  0x02000000
#define TD_T_DATA1  0x03000000
#define TD_T_TOGGLE 0x00000000
#define TD_R        0x00040000
#define TD_DI       0x00E00000
#define TD_DI_SET(X) (((X) & 0x07)<< 21)
#define TD_DP       0x00180000
#define TD_DP_SETUP 0x00000000
#define TD_DP_IN    0x00100000
#define TD_DP_OUT   0x00080000

#define TD_ISO	    0x00010000
#define TD_DEL      0x00020000

/* CC Codes */
#define TD_CC_NOERROR      0x00
#define TD_CC_CRC          0x01
#define TD_CC_BITSTUFFING  0x02
#define TD_CC_DATATOGGLEM  0x03
#define TD_CC_STALL        0x04
#define TD_DEVNOTRESP      0x05
#define TD_PIDCHECKFAIL    0x06
#define TD_UNEXPECTEDPID   0x07
#define TD_DATAOVERRUN     0x08
#define TD_DATAUNDERRUN    0x09
#define TD_BUFFEROVERRUN   0x0C
#define TD_BUFFERUNDERRUN  0x0D
#define TD_NOTACCESSED     0x0F


#define MAXPSW 1

//#define virt_to_phy(va)			((va) & 0x1fffffff)
#define PUCHAR 	unsigned char *
#define UCHAR 	unsigned char 
#define PULONG 	unsigned long *
#define ULONG 	unsigned long

typedef struct _GENTD
{
	ULONG TDdata;
	ULONG Cbp;		//physical address of buffer to be filled
	ULONG NextTD;	//physical address of next td to be filled
	ULONG Bend;		//physical address of buffer end to be filled	
} __attribute((aligned(16))) GENTD,*PGENTD;

typedef struct _ISOTD
{
	ULONG TDdata;
	ULONG Cbp;		//physical address of buffer to be filled
	ULONG NextTD;	//physical address of next td to be filled
	ULONG Bend;		//physical address of buffer end to be filled	
	ULONG offset10;	//offset0-1
	ULONG offset32;	//offset2-3
	ULONG offset54;	//offset4-5
	ULONG offset76;	//offset6-7
} ISOTD,*PISOTD;

typedef struct _OHCSTAT
{
	unsigned char DevAddress;
	unsigned char DevConfig;
	unsigned char DevIface;
	unsigned char DevFeature;
	unsigned int DevMPS;
}OHCSTAT,*POHCSTAT;

typedef struct _EP_CHAR
{
	unsigned char EpNum;
	unsigned char EpDir;
	unsigned char EpType;
	unsigned int EpMps;
	unsigned int EpPollRate;
}EP_CHAR,*PEP_CHAR;

int OHCITesting(int value);
int OHCI_Auto_TestItem(void);
int OHCI_Base_TestItem(void);
int OHCI_Ctrl_TestItem(void);
int OHCI_HUB_TestItem();
int OHCI_Bulk_TestItem();
int OHCI_Isochronous_TestItem();
int OHCI_Intr_TestItem();
void	initOHCIController();
void	initOHCITestingEnv();
int OHCI_Intr_Disable(void);
int OHCI_Intr_Enable(void);
int OHCI_Intr_Threshold_Ctrl(BYTE Value);
void OHCIIntrHandler(void* pparam);
void OHCI_ControlList_Enable(BYTE Enable);
void OHCI_BulkList_Enable(BYTE Enable);
void OHCI_PeriodicList_Enable(BYTE Enable);
void OHCITransactionComplete();
void OHCIInitMemory();
//void SetCtrlSetupDataFun(pCtrlRequest pCtrlReq,BYTE bRequest);
int OHCI_ResetDevice();
int HostCtrlReset_OHCI();
int SetAddress_OHCI(BYTE DevAddress);
int SetConfiguration_OHCI(BYTE DevConfig);
int SetFeature_OHCI(BYTE DevRequest,BYTE DevFeature,BYTE EPNO,BYTE EpDir);
int ClearFeature_OHCI(BYTE DevRequest,BYTE DevFeature,BYTE EPNO,BYTE EpDir);
int SetInterface_OHCI(BYTE DevAltSet,BYTE InterfaceNo);
int GetDescriptor_OHCI(BYTE DevDescType,BYTE DevDescIndex,int DevDescLength);
int GetDescriptorN1_OHCI(BYTE DevDescType,BYTE DevDescIndex,BYTE DevDescLength);
int GetInterface_OHCI();
int GetStatus_OHCI(BYTE DevRequest,BYTE DevDescIndex);
int GetConfiguration_OHCI();
int OHCI_SendBulkData(int MPS,char EpNum,int Length);
int OHCI_ReceiveBulkData(int MPS,char EpNum,int Length);
int OHCI_SendIsoData(int MPS, char EpNum, int Framecount, PUCHAR OutDataBuf);
int OHCI_ReceiveIsoData(int MPS, char EpNum, int Framecount, PUCHAR InDataBuf);
int OHCI_BulkLoopData(int MPS,char EpOutNum,char EpInNum,int Length,PUCHAR OutDataBuf,PUCHAR InDataBuf);
void OHCI_Start();
void OHCI_Stop();
int OHCI_Halt(void);
void OHCI_Reset(void);
int ClearHubFeature_OHCI(BYTE DevFeature);
int ClearPortFeature_OHCI(BYTE DevFeature, BYTE Portnum );
int GetHubStatus_OHCI();
int GetPortStatus_OHCI(BYTE Portnum);
int Success_OHCI(ULONG trans);
int SetHubFeature_OHCI(BYTE DevFeature);
int GetHubDescriptor_OHCI(BYTE DevDescType,BYTE DevDescIndex,BYTE DevDescLength);
void FrameIndexIncreTest(void);
void PrintOHCIRegs(void);
void Validate_OHCIRegs(void);
void	Display_OHCIMenu(void);
void Display_OHCIAutoTestMenu(void);
void Display_OHCIBASETestMenu(void);
void Display_OHCICtrlMenu(void);
void Display_OHCIBulkMenu(void);
void Display_OHCIIsoMenu(void);
void Display_OHCIIntrMenu(void);
void Display_OHCIHubMenu(void);
//void ehci_time_up(void* pparam);
//void Set_1ms_timer(int tmr_times);


#endif // _ohcictrl_h_
