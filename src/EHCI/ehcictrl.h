#ifndef  _ehcictrl_h_
#define  _ehcictrl_h_


#include "Interrupt/interrupt.h"
#include "type.h"

#define	DEFAULT_I_TDPS		1024		/* some HCs can do less */
#define	QH_HEAD		0x00008000

#define NO_FRAME ((unsigned short)~0)			/* pick new start */

#define PUCHAR 	unsigned char *
#define UCHAR 	unsigned char 
#define PULONG 	unsigned long *
#define ULONG 	unsigned long

//Queue Element Transfer Descriptor (QTD)
//qTD Token
#define	QTD_TOGGLE	(1 << 31)	/* data toggle */
#define	QTD_LENGTH(tok)	(((tok)>>16) & 0x7fff)
#define	QTD_IOC		(1 << 15)	/* interrupt on complete */
#define	QTD_CERR(tok)	(((tok)>>10) & 0x3)
#define	QTD_PID(tok)	(((tok)>>8) & 0x3)
#define	QTD_STS_ACTIVE	(1 << 7)	/* HC may execute this */
#define	QTD_STS_HALT	(1 << 6)	/* halted on error */
#define	QTD_STS_DBE	(1 << 5)	/* data buffer error (in HC) */
#define	QTD_STS_BABBLE	(1 << 4)	/* device was babbling (qtd halted) */
#define	QTD_STS_XACT	(1 << 3)	/* device gave illegal response */
#define	QTD_STS_MMF	(1 << 2)	/* incomplete split transaction */
#define	QTD_STS_STS	(1 << 1)	/* split transaction state */
#define	QTD_STS_PING	(1 << 0)	/* issue PING? */

typedef struct _QHEAD
{
	ULONG QHEADLinkPtr;
	ULONG EpChar;
	ULONG EpCap;
	ULONG CurrQTD;
	ULONG NextQTD;
	ULONG AltQTd;
	ULONG QTDChar;
	ULONG QHEADBuffPtr0;
	ULONG QHEADBuffPtr1;
	ULONG QHEADBuffPtr2;
	ULONG QHEADBuffPtr3;
	ULONG QHEADBuffPtr4;
} QHEAD, *PQHEAD;

typedef struct _QTD
{
	ULONG NextQTD;
	ULONG AltQTD;
	ULONG QTDChar;
	ULONG QTDBuffPrt0;
	ULONG QTDBuffPrt1;
	ULONG QTDBuffPrt2;
	ULONG QTDBuffPrt3;
	ULONG QTDBuffPrt4;
} QTD, *PQTD;

typedef struct _iTD
{
	ULONG iTDNextLinkPtr;
	ULONG iTDStatCon0;
	ULONG iTDStatCon1;
	ULONG iTDStatCon2;
	ULONG iTDStatCon3;
	ULONG iTDStatCon4;
	ULONG iTDStatCon5;
	ULONG iTDStatCon6;
	ULONG iTDStatCon7;
	ULONG iTDBuffPtr0;
	ULONG iTDBuffPtr1;
	ULONG iTDBuffPtr2;
	ULONG iTDBuffPtr3;
	ULONG iTDBuffPtr4;
	ULONG iTDBuffPtr5;
	ULONG iTDBuffPtr6;
}iTD,*PiTD;

typedef struct _siTD
{
	ULONG siTDNextLinkPtr;
	ULONG siTDEpCap;
	ULONG siTDEpChar;
	ULONG siTDEpStatCon;
	ULONG siTDPagePtr0;
	ULONG siTDPagePtr1;
	ULONG siTDBackLnkPtr;
}siTD,*PsiTD;

typedef struct _EHCSTAT
{
	unsigned char DevAddress;
	unsigned char DevConfig;
	unsigned char DevIface;
	unsigned char DevFeature;
	unsigned char DevAddress_Hub;
	unsigned char Portnum;
	unsigned int  CepMps;
	unsigned int  CepMps_Hub;
}EHCSTAT,*PEHCSTAT;

typedef struct _CtrlRequest {
	u8_t		bRequestType;
	u8_t		bRequest;
	u16_t	wValue;
	u16_t 	wIndex;
	u16_t 	wLength;
} CtrlRequest,*pCtrlRequest;

//Linux
#define INIT_LIST_HEAD(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

#define virt_to_phy(va)			((va) & 0x1fffffff)

//============QH struct================
//============QH Pointer===============
#define ITD_TYPE			(0x0 << 1)  //0:iTD 1:QH 2:siTD 3:FSTN
#define QH_TYPE 			(0x1 << 1)
#define SITD_TYPE		(0x2 << 1)
#define FSTN_TYPE		(0x3 << 1)
#define T_BIT			(0x01)
//=======pQHEAD1->EpChar QH Edp Characteristic 3.6.2====
#define RL				(0x1 << 28)	//Nak Count Load
#define H_BIT 			(0x1 << 15)	
#define DTC 				(0x1 << 14)
#define EPS 				(0x2 << 12) //0:FS 1:LS 2:HS
//=======pQHEAD1->EpCap QH Edp Capability 3.6.2=======
#define MULT				(0x1 << 30) //0 1 2 3 transactions to be issued for this endpoint per micro-frame
//============QTD struct================
//============QTD Token===============
#define DT				(0x1 << 31)	
#define TOTAL_LENGTH	(0x1 << 16)	//30:16
#define IOC				(0x1 << 15)	//15	Interrupt on Complete
#define C_PAGE			(0x1 << 12)  	//14:11
#define C_ERR			(0x3 << 10)	//11:10
#define PID_OUT			(0x0 << 8) 	//PID 9:8
#define PID_IN			(0x1 << 8)
#define PID_SETUP		(0x2 << 8)
#define ACTIVE			(0x1 << 7) 	//Status 7:0
//===================================



#define Bulk_Mps (0x200 << 16)
#define Intr_Mps (0x400 << 16)
#define Nak_Rl (0xf <<28)

#define TBT (0x7fff0000)

#define TwentyKB 20480
#define No_Tds_In_Grp 4
#define MPS_BULK_HS 512

#define Tbt (0x7fff0000)

#define ISO_MULT  	 	(0x1)
#define ISO_IN			(0x1 << 11)


#define USB_INTERRUPT 			0x1
#define USB_DISABLE_ALL_INTR 	0x0
#define ENDPOINT_HALT 			0x0

#define CLEAR_HUB_FEATURE 		1
#define CLEAR_PORT_FEATURE 	2
#define CLEAR_TT_BUFFER 		3
#define GET_HUB_DESCRIPTOR 	4
#define GET_HUB_STATUS 		5
#define GET_PORT_STATUS 		6
#define RESET_TT 				7
#define SET_HUB_DESCRIPTOR 	8
#define SET_HUB_FEATURE 		9
#define SET_PORT_FEATURE 		10
#define GET_TT_STATE 			11
#define STOP_TT 					12

extern BYTE		portconnect;	// flag set when device is connected and reset to zero when device is disconnected.
extern BYTE		TransferComplete;// flag set when an interrupt occured due to completion of an transfer.
extern BYTE 		ErrorComplete;	// flag set when an interrupt occured due to some error on usb.
extern BYTE		MpsKnown;	// initially this variable is zero and we have to perform get device descriptor to know the MPS of control endpoint of the connected device and after successfull completion of get device descriptor this variable is initialzed to correct value as specified in the descriptor. 
extern BYTE 		BulkInTog;	// used to store the bulk in endpoint toggle value to be used for next transaction.
extern BYTE 		IntrInTog;	// used to store the intr in endpoint toggle value to be used for next transaction.
extern BYTE		BulkOutTog;	// used to store the bulk out endpoint toggle value to be used for next transaction.
extern BYTE 		MultipleTT;	// used to know whether the connected hub contains multiple TT or single TT
extern BYTE		C_Bit;		// flag set when the newly connected device under hub is a full speed device.
extern BYTE		Device_Hub_Speed;// flag used to specify the speed of the newly connected device under a hub.


#define QHADDR 				TEST_PATTERN_START//0x00400000		//0x30*5= 0xf0			// 0x80400000 to 0xA0400000
#define QTDADDR 			(TEST_PATTERN_START + 0x200)//0x00400200		//0x20*5= 0xa0		//0x80400100 to 0xA0400100 to 0xA0400200 to accomodate for qhead alignement problem
#define iTDADDR				(TEST_PATTERN_START + 0x400)//0x00400400		// for itd  api_new

//For Test Tools
#define SETUP_BUFF_ADDR					(TEST_PATTERN_START + 0x20000) //0x00200000		//0x1000 4096 = 1024*4		// 0x80200000 to 0xA0200000
#define DATA_IN_ADDR						(TEST_PATTERN_START + 0x40000) //0x00800000		//0x5000 20480
#define DATA_OUT_ADDR						(TEST_PATTERN_START + 0x60000) //0x00A00000		//0x5000 20480	
#define PERIOD_FRAMELIST_ADDR				(TEST_PATTERN_START + 0x80000) //0x00204000		//0x1000 4096
#define EHCIOHCI_Hc_HCCA					(TEST_PATTERN_START + 0xa0000) //0x00400C00
#define INTR_OUT_ADDR						(TEST_PATTERN_START + 0x100000) //0x00220000		//0x100000 1048576
#define INTR_IN_ADDR						(TEST_PATTERN_START + 0x200000) //0x00320000		//0x100000 1048576


#define	INTR_MASK 				(STS_IAA | STS_FATAL | STS_ERR | STS_INT)
/* next async queue entry, or pointer to interrupt/periodic QH */
//#define	QH_NEXT(dma)			(dma&~0x01f)|Q_TYPE_QH

/* values for that type tag */
#define Q_TYPE_ITD	 	(0 << 1)
#define Q_TYPE_QH		(1 << 1)
#define Q_TYPE_SITD 	(2 << 1)
#define Q_TYPE_FSTN 	(3 << 1)

/* magic numbers that can affect system performance */
#define	EHCI_TUNE_CERR		3	/* 0-3 qtd retries; 0 == don't stop */
#define	EHCI_TUNE_RL_HS		4	/* nak throttle; see 4.9 */
#define	EHCI_TUNE_RL_TT		0
#define	EHCI_TUNE_MULT_HS	1	/* 1-3 transactions/uframe; 4.10.3 */
#define	EHCI_TUNE_MULT_TT		1
#define	EHCI_TUNE_FLS			2	/* (small) 256 frame schedule */

//=============================================================
//For Test Tools
//=============================================================
int EHCITesting(int value);
int EHCI_Auto_TestItem(void);
int EHCI_Base_TestItem(void);
int EHCI_Ctrl_TestItem(void);
int EHCI_HUB_TestItem();
int EHCI_Bulk_TestItem();
int EHCI_Intr_TestItem();
void	initEHCIController();
void	initEHCITestingEnv();
int EHCI_Intr_Disable(void);
int EHCI_Intr_Enable(void);
int EHCI_Intr_Threshold_Ctrl(BYTE Value);
void EHCIIntrHandler(void* pparam);
void EHCI_Asynchronous_Enable(BYTE Enable);
void EHCI_Periodic_Enable(BYTE Enable);
void TransactionComplete();
void InitMemory();
void SetCtrlSetupDataFun(pCtrlRequest pCtrlReq,BYTE bRequest);
int ResetDevice();
int HostCtrlReset();
int SetAddress(BYTE DevAddress);
int SetConfiguration(BYTE DevConfig);
int SetFeature(BYTE DevRequest,BYTE DevFeature,BYTE DevDescIndex,BYTE EpDir);
int ClearFeature(BYTE DevRequest,BYTE DevFeature,BYTE DevDescIndex,BYTE EpDir);
int SetInterface(BYTE DevAltSet);
int GetDescriptor(BYTE DevDescType,BYTE DevDescIndex,int DevDescLength);
int GetDescriptorN1(BYTE DevDescType,BYTE DevDescIndex,BYTE DevDescLength);
int GetHubDescriptor(BYTE DevDescType,BYTE DevDescIndex,int DevDescLength);
int GetInterface(BYTE DevIface);
int GetStatus(BYTE DevRequest,BYTE DevDescIndex);
int GetConfiguration();
int ReceiveBulkData(int Mps,BYTE EpNum, int  Length);
int SendBulkData(int Mps,BYTE EpNum, int  Length);
int BulkLoopData(int Mps,BYTE EpOutNum,BYTE EpInNum,int TdMaxbufLength,PUCHAR DataOutBuf,PUCHAR DataInBuf,BYTE UnderHub);
int IsoLoopData(int Mps,BYTE EpOutNum,BYTE EpInNum,int TdMaxbufLength,PUCHAR DataOutBuf,PUCHAR DataInBuf);


extern void EHCI_Start();
void EHCIOHCI_Start();
int EHCIOHCI_Intr_Disable(void);
int EHCIOHCI_Intr_Enable(void);
int EHCIOHCI_Reset(void);

void EHCI_Stop();
int EHCI_Halt(void);
int EHCI_Reset(void);
void PortReset();
void PortResume();
int VendorRequest();
int SendIntrData();
int ReceiveIntrData(int Mps,BYTE EpNum,int Length);
int GetPortStatus(BYTE Portnum);
int GetHubStatus();
int ClearPortFeature(BYTE DevFeature, BYTE Portnum);
int SetPortFeature(BYTE DevFeature, BYTE Portnum );
int ClearTTBuffer(unsigned short EpChar, BYTE TTPort);
int GetTTState(BYTE TTPort);//api_new
int ResetTT(BYTE TTPort);
int Success(ULONG trans);
void FrameIndexIncreTest(void);
void PrintRegs(void);
void ValidateRegs(void);
void	Display_EHCIMenu(void);
void Display_EHCIAutoTestMenu(void);
void Display_EHCIBASETestMenu(void);
void Display_EHCICtrlMenu(void);
void Display_EHCIBulkMenu(void);
void Display_EHCIIsoMenu(void);
void Display_EHCIIntrMenu(void);
void Display_EHCIHubMenu(void);
void ehci_time_up(void* pparam);
void Set_1ms_timer(int tmr_times);

//====Test Mode Function=============================================
void Suspend_Resume_auto();
void Erumate_Device();
void Display_SuspendResumeMenu();
int Suspend_Resume();
int Chirp_Timing();
void Auto_Erumation();
//=============================================================

//=============================================================
//For Linux Driver
//=============================================================
int ehci_suspend ();
int ehci_resume ();
int ehci_get_frame (struct usb_hcd *hcd);
//memory

//=================
//For Test Mode
//=================
int USB_TestMode();


#endif // _ehcictrl_h_
