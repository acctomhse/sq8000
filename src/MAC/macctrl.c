#include <genlib.h>
#include <irqs.h>
#include <interrupt.h>

//MAC include
#include "if_socle.h"			/* device description header */
#include "sMACdev.h"			/* device struct */
#include "macif.h"
#include "macctrl.h"
#include <test_item.h>

#if defined (CONFIG_PC9220)
#include <scu.h>
#include "../../platform/arch/scu-reg.h"
#endif


//When Disable SemiHost Function the Default Testing item
#define DEFAULT_MAC_TESTING_ITEM	'z'

typedef	enum	{
	PERFECT		,
	PERFECT_FAIL	,
	INVERSE_PERFECT	,
	HASH		,
	HASH_ONLY	,
	ALL_MULTI	,
	PROMISCUOUS	,
	ALL_BAD
}	FILTER_TYPE;

const 	ULONG	Filter_OPMode_TABLE[]	=
{
		0				,	//PERFECT
		CSR6_RA				,   //PERFECT_FAIL
		CSR6_IF				,	//INVERSE_PERFECT
		CSR6_HP				,	//HASH
		CSR6_HP|CSR6_HO			,	//HASH_ONLY
		CSR6_PM				,	//ALL_MULTI
		CSR6_PR				,	//PROMISCUOUS
		CSR6_PB|CSR6_PR			//ALL_BAD
};

const 	ULONG	Filter_TableType_TABLE[] =
{
		TDESC1_PERFECT		,	//PERFECT
		TDESC1_PERFECT		,   //PERFECT_FAIL
		TDESC1_INVERSE		,	//INVERSE_PERFECT
		TDESC1_HASH			,	//HASH
		TDESC1_HASH_ONLY	,	//HASH_ONLY
		TDESC1_PERFECT		,	//ALL_MULTI
		TDESC1_PERFECT		,	//PROMISCUOUS
		TDESC1_PERFECT			//ALL_BAD
};

//////////////////////////////////////////////////////////////////////////////
//		Veriable Definition
//////////////////////////////////////////////////////////////////////////////


DRV_CTRL	socle_MacDevice;
DRV_CTRL	*Testing_DevicePt;

#define     DES_RING_SKIP	1
typedef struct rDescShip
    {
    ULONG	rDesc0;		/* status and ownership */
    ULONG	rDesc1;		/* control & buffer count */
    ULONG	rDesc2;		/* buffer address 1 */
    ULONG	rDesc3;		/* buffer address 2 */
    ULONG   skip[DES_RING_SKIP];
    } SMAC_RDE_SKIP;

typedef struct tDescSkip
    {
    ULONG	tDesc0;		/* status and ownership */
    ULONG	tDesc1;		/* control & buffer count */
    ULONG	tDesc2;		/* buffer address 1 */
    ULONG	tDesc3;		/* buffer address 2 */
    ULONG   skip[DES_RING_SKIP];
    } SMAC_TDE_SKIP;

SMAC_RDE		Rx_Desc[NUM_RDS];
SMAC_TDE		Tx_Desc[NUM_TDS];
SMAC_RDE_SKIP	Rx_Desc_Skip[NUM_RDS];
SMAC_TDE_SKIP	Tx_Desc_Skip[NUM_TDS];
ULONG 			Test_FltrFrm[FLTR_FRM_SIZE_ULONGS];
const char		Test_Perfect_MAC_Addr[16][6]=
{
	{0x00, 0x45, 0x4C, 0x43, 0x4f ,0x53},	//"SOCLE"-00
	{0x02, 0x45, 0x4C, 0x43, 0x4f ,0x53},	//"SOCLE"-02
	{0x04, 0x45, 0x4C, 0x43, 0x4f ,0x53},	//"SOCLE"-04
	{0x06, 0x45, 0x4C, 0x43, 0x4f ,0x53},	//"SOCLE"-06
	{0x08, 0x45, 0x4C, 0x43, 0x4f ,0x53},	//"SOCLE"-08
	{0x0a, 0x45, 0x4C, 0x43, 0x4f ,0x53},	//"SOCLE"-0a
	{0x0c, 0x45, 0x4C, 0x43, 0x4f ,0x53},	//"SOCLE"-0c
	{0x0e, 0x45, 0x4C, 0x43, 0x4f ,0x53},	//"SOCLE"-0e
	{0x10, 0x45, 0x4C, 0x43, 0x4f ,0x53},	//"SOCLE"-10
	{0x12, 0x45, 0x4C, 0x43, 0x4f ,0x53},	//"SOCLE"-12
	{0x14, 0x45, 0x4C, 0x43, 0x4f ,0x53},	//"SOCLE"-14
	{0x16, 0x45, 0x4C, 0x43, 0x4f ,0x53},	//"SOCLE"-16
	{0x18, 0x45, 0x4C, 0x43, 0x4f ,0x53},	//"SOCLE"-18
	{0x1a, 0x45, 0x4C, 0x43, 0x4f ,0x53},	//"SOCLE"-1a
	{0x1c, 0x45, 0x4C, 0x43, 0x4f ,0x53},	//"SOCLE"-1c
	{0x1e, 0x45, 0x4C, 0x43, 0x4f ,0x53}	//"SOCLE"-1e
};
const char		Test_Hash_MAC_Addr[10][6]=
{
	{0x25, 0x00, 0x25, 0x00, 0x27 ,0x00},	//"Mutli Patern1"
	{0xA3, 0xC5, 0x62, 0x3F, 0x25 ,0x87},	//"Mutli Patern2"
	{0xD9, 0xC2, 0xC0, 0x99, 0x0B ,0x82},	//"Mutli Patern3"
	{0x7D, 0x48, 0x4D, 0xFD, 0xCC ,0x0A},	//"Mutli Patern4"
	{0xE7, 0xC1, 0x96, 0x36, 0x89 ,0xDD},	//"Mutli Patern5"
	{0x61, 0xCC, 0x28, 0x55, 0xD3 ,0xC7},	//"Mutli Patern6"
	{0x6B, 0x46, 0x0A, 0x55, 0x2D ,0x7E},	//"Mutli Patern7"
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF ,0xFF},	//"Mutli Patern8"
	{0x00, 0x45, 0x4C, 0x43, 0x4f ,0x53},	//"SOCLE"-00
	{0x11, 0x11, 0x11, 0x11, 0x11 ,0x11}	//Filter out Mutli Address
};

// Data Buff veriable frame size from 128 to 1518
#define TRX1_DATA_SEG		32
#define	TRX1_DATA_SIZE		FLTR_FRM_SIZE		// Minmun is Filter Size
#define TRX2_DATA_SEG		16
#define	TRX2_DATA_SIZE		256
#define TRX3_DATA_SEG		8
#define	TRX3_DATA_SIZE		512
#define TRX4_DATA_SEG		8
#define	TRX4_DATA_SIZE		1024

char			Tx_DataBuff_1	[TRX1_DATA_SEG][TRX1_DATA_SIZE] __attribute__ ((section (".MAC_BUFF"))); //[TRX1_DATA_SIZE];
char			Tx_DataBuff_2	[TRX2_DATA_SEG][TRX2_DATA_SIZE] __attribute__ ((section (".MAC_BUFF"))); //[TRX2_DATA_SIZE];
char			Tx_DataBuff_3	[TRX3_DATA_SEG][TRX3_DATA_SIZE] __attribute__ ((section (".MAC_BUFF"))); //[TRX3_DATA_SIZE];
char			Tx_DataBuff_4	[TRX4_DATA_SEG][TRX4_DATA_SIZE] __attribute__ ((section (".MAC_BUFF"))); //[TRX4_DATA_SIZE];
char			Rx_DataBuff_1	[TRX1_DATA_SEG][TRX1_DATA_SIZE] __attribute__ ((section (".MAC_BUFF"))); //[TRX1_DATA_SIZE];
char			Rx_DataBuff_2	[TRX2_DATA_SEG][TRX2_DATA_SIZE] __attribute__ ((section (".MAC_BUFF"))); //[TRX2_DATA_SIZE];
char			Rx_DataBuff_3	[TRX3_DATA_SEG][TRX3_DATA_SIZE] __attribute__ ((section (".MAC_BUFF"))); //[TRX3_DATA_SIZE];
char			Rx_DataBuff_4	[TRX4_DATA_SEG][TRX4_DATA_SIZE] __attribute__ ((section (".MAC_BUFF"))); //[TRX4_DATA_SIZE];

RBUFF_VSIZE		rBuffSize[NUM_RDS];
TBUFF_VSIZE		tBuffSize[NUM_TDS];

///////////////////////////////////////////////////////////////////
//			Test Control
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//		Default Test Setting
///////////////////////////////////////////////////////////////////
#define 	DEFAULT_ENABLE_LOG_HIF	1

#define		DEFAULT_FILTER			PERFECT		//ALL_BAD
#define		DEFAULT_PERFECT_SIZE	1
#define		DEFAULT_HASH_SIZE		8
#define		DEFAULT_FILTER_SIZE		DEFAULT_PERFECT_SIZE
#define		DEFAULT_TAP				0				// Auto polling Tx
#define		DEFAULT_PBL				16				// Burst length 16
#define		DEFAULT_BAR				0  				// Equal Prioity
#define     DEFAULT_SPEED_100M		1				// 100M
#define		DEFAULT_FULL_DUPLEX		1				// Full duplex
#define     DEFAULT_TX_TR			4				// Store & Forward // 1024 bytes Threshold
#define		RXWAIT_CLOCK			100				// wait *2ms
#define		DEFAULT_SINGLEBUFF1		1				// Don't use SINGLEBUFF1 2 == 1 1
#define		DEFAULT_SINGLEBUFF2		0				// if both SINGLEBUFF1 2 == 0 0 means 2 buffer
#define		DEFAULT_IGNOREERROR		1

#define		TEST_MIN_MTU			SMAC_MIN_MTU
#define		TEST_MAX_MTU			SMAC_MAX_MTU

#define		DEFAULT_REMOTE_PACKET	10

int				Test_Ethernet_Frame;
int				Test_Speed_100M;
int				Test_Full_Duplex;
int				Test_DMA_Burst;
int				Test_Tx_TR;
int				Test_Bus_Arbitration;
FILTER_TYPE		Test_FilterMode;
int 			Test_FilterSize;
int 			Test_ChainDes;
int				Test_SkipDes;
int				Test_SingleBuff1;
int				Test_SingleBuff2;
int				Test_Auto_Polling_Tx;
int 			Test_LoopBack;
int 			Test_ForceEnable;
char			Test_FilterAddr[16][6];
int				Test_PollRx_En;
int				Test_PollTx_En;
int				Test_DisPadding;
int				Test_DisCRC;
int				Test_IgnoreError;
int				Test_Tx_Underflow;
int				Test_No_Phy;

char			Test_Tx_DataPool[2048];
RCVDATAPOOL		Test_Rx_DataPool[32];

int				Test_Error_TraceLen[3];

int				MAC_Auto_Test_ItemNum;
int				MAC_Auto_Test_CurrentNum;

const char		MAC_Auto_Test_Item[] =
{ '1', '2', '3'
, '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
, 'g'
, 'h'
, 'i'
, 'j'
, 'k', 'l'
, 'm'
, 'n'
//, '@', '=', 'o'	//for 7230
, '!', '=', 'o'
, 'p'
, 't'
, 'u'
//, '\\'
, '@', '#', '=', '4'
//, '@', '$', '=', '4'
//, '!', '$', '=', '4'
, '!', '#', '=', '4'
, '%', '&', '=', '4'
, '%', '*', '=', '4'
, '^', '&', '=', '4'
, '^', '*', '=', '4'

, '(', '=', '4'	//Equal Aribitration
, '}', '=', '4'
, '}', '=', '4'
, '}', '=', '4'
, '}', '=', '4'
, '}', '=', '4'
, '}', '=', '4'
, '}', '=', '4'

, ')', '=', '4'	//Rx High Aribitration
, '}', '=', '4'
, '}', '=', '4'
, '}', '=', '4'
, '}', '=', '4'
, '}', '=', '4'
, '}', '=', '4'
, '}', '=', '4'

, '@'	//Beacause the bus speed we only could test TR at 10M
, '+', '=', '4'
, '+', '=', '4'
, '+', '=', '4'
, '+', '=', '4'
, '+', '=', '4'
, '!'
, '>', '=', '4'
, '>', '=', '4'
, '>', '=', '4'
, '>', '=', '4'
, '>', '=', '4'
, '>', '=', '4'
, '>', '=', '4'
, '>', '=', '4'
, ':', '=', '4'
, '"', '=', '4'
, '!', '='//, 'q'
, '@', '='//, 'q'
, '@', '='//, 'y'
, '!', '='//, 'y'
, 'x' };

void	MAC_RealEtherTesting(int auto_reply);
void	Display_RealEtherMenu(void);
ULONG 	Random_Gen(void);
void 	MacTestRandomTx(int *PacketLen);
void 	ShowDataPacket(char * Pool, int len);
void	MacTestReplyPacket(void);
int		Paser_ParaSetting(char cmd);

//void	MAC_SROMTesting(int autotest);
void	Display_SROMMenu(void);


extern	int sMacEnableDataShow;

int		Enable_Log_HIF;
int     Forever_Test;

int		RealEtherPacketLen;
int		RealEtherPacketID;

					
extern struct test_item_container mac_main_container;

int MACTesting(int autotest)
{
	int ret=0;
	
	ret=test_item_ctrl(&mac_main_container,autotest);
	
	return ret;
	
}

extern struct test_item_container mac_phy_container;

int mac_phy_clock_test(int autotest)
{
	int ret=0;
	
	Testing_DevicePt=&socle_MacDevice;
	Testing_DevicePt->devAdrs=MAC_REG_BASE;
	Testing_DevicePt->MII_PhyAD=MAC_MAX_PHY;
		
	ret=test_item_ctrl(&mac_phy_container,autotest);
	
	return ret;
	
}	

int mac_phy_clock_on_test(int autotest)
{
	int ret;
	u32 cr;
				
	printf(" PHY clock on test ...........\n");
	ret=sMacMiiInit(Testing_DevicePt);
	cr = sMacMiiPhyRead(Testing_DevicePt, Testing_DevicePt->MII_PhyAD, MII_PHY_CR);
	sMacMiiPhyWrite(Testing_DevicePt, Testing_DevicePt->MII_PhyAD, MII_PHY_CR, (cr & ~MII_PHY_CR_OFF));	
	sMacMiiPhyReadAll(Testing_DevicePt);				
			
	return 0;
}

int mac_phy_clock_off_test(int autotest)
{
	int ret;
	u32 cr;
		
	printf(" PHY clock off test ...........\n");
	ret=sMacMiiInit(Testing_DevicePt);
	cr = sMacMiiPhyRead(Testing_DevicePt, Testing_DevicePt->MII_PhyAD, MII_PHY_CR);
	sMacMiiPhyWrite(Testing_DevicePt, Testing_DevicePt->MII_PhyAD, MII_PHY_CR, (cr | MII_PHY_CR_OFF));
	sMacMiiPhyReadAll(Testing_DevicePt);
						
	return 0;
}


int mac_normal_test(int autotest)
{
	int 	result;
	int		auto_ret=0;
	int		clock;
	char 	cmd_buf[20] = {DEFAULT_MAC_TESTING_ITEM};

#ifdef CONFIG_PC9220
	socle_scu_dev_enable(SCU_DEVCON_MAC_GPIO);
#endif

	MAC_Auto_Test_ItemNum=0;
	#ifdef	SMAC_DEBUG
	#ifdef SMAC_MII_DEBUG
	sMacEnableMiiPhyShow=1;
	#else
	sMacEnableMiiPhyShow=0;
	#endif
	#ifdef SMAC_CSR_DEBUG
	sMacEnableCsrShow=1;
	#else
	sMacEnableCsrShow=0;
	#endif
	#ifdef SMAC_DEVICE_DEBUG
	sMacEnableDevShow=1;
	#else
	sMacEnableDevShow=0;
	#endif
	#ifdef	SMAC_DATA_DEBUG
	sMacEnableDataShow=1;
	#else
	sMacEnableDataShow=0;
	#endif
	#endif

	Enable_Log_HIF=DEFAULT_ENABLE_LOG_HIF;
	Forever_Test=0;

	sMAC_Des_Byte_Order=CPU_BYTE_ORDER;
	sMAC_DataB_Byte_Order=CPU_BYTE_ORDER;

	// Inital Test Control
	Test_Ethernet_Frame=0;
	Test_Speed_100M=DEFAULT_SPEED_100M;
	Test_Full_Duplex=DEFAULT_FULL_DUPLEX;
	Test_DMA_Burst=DEFAULT_PBL;
	Test_Tx_TR=DEFAULT_TX_TR;
	Test_Bus_Arbitration=DEFAULT_BAR;
	Test_FilterMode=DEFAULT_FILTER;
	Test_FilterSize=DEFAULT_FILTER_SIZE;
 	Test_ChainDes=0;
	Test_SkipDes=0;
	Test_SingleBuff1=DEFAULT_SINGLEBUFF1;
	Test_SingleBuff2=DEFAULT_SINGLEBUFF2;
	Test_Auto_Polling_Tx=DEFAULT_TAP;
	Test_LoopBack=1;
	Test_ForceEnable=1;
	Test_PollRx_En=1;
	Test_PollTx_En=1;
	Test_DisPadding=0;
	Test_DisCRC=0;
	Test_IgnoreError=DEFAULT_IGNOREERROR;
	Test_Tx_Underflow=0;
	Test_No_Phy=0;

	//Inital the Device Struct, Descriptor and Data Buffer
	Testing_DevicePt=&socle_MacDevice;
	Testing_DevicePt->devAdrs=MAC_REG_BASE;
	Testing_DevicePt->ivec=INTC_CONNECT_SYS_INT;	//MIPS IP vect 					(level 1)
	Testing_DevicePt->ilevel=MAC_INT;				//Interrupt Controller ilevel	(level 2)
	Testing_DevicePt->pFltrFrm=Test_FltrFrm;

	Testing_DevicePt->rx_PoolPt=&Test_Rx_DataPool[0];
	Testing_DevicePt->rx_PooliMax=32;

	Testing_DevicePt->MII_PhyAD=MAC_MAX_PHY;	//Unconnect PHY
	//Testing_DevicePt->MII_PhyAD=1;	//Unconnect PHY

	MacTestDevCrtlInit();

	Display_MacMenu();

	while(1){
MacMain_break:

		result = 0;
		Test_LoopBack = 1;
		MSDELAY(100);
		if (autotest){
			autotest = 0;
			cmd_buf[0] = 'z';
		} 
		else if (MAC_Auto_Test_ItemNum != 0){
			cmd_buf[0]=MAC_Auto_Test_Item[MAC_Auto_Test_CurrentNum];
			MAC_Auto_Test_CurrentNum++;
			MAC_Auto_Test_ItemNum--;
			printf("\n");
		}
		else{
			//Display_MacMenu();
			printf("MacTP_Shell>");
			cmd_buf[0] = getchar();
		}

		printf("Execute MAC Test Command (%c)...",cmd_buf[0]);

		if( Paser_ParaSetting(cmd_buf[0]) ){
			goto MacMain_break;
		}
		else{
			switch(cmd_buf[0])
			{

//-----------------------------------------------------------------------------
				case '1' :
					printf(" Scan MII PHY Chip\n");
					Testing_DevicePt->MII_PhyAD=MAC_MAX_PHY;	//Unconnect PHY
					result=sMacMiiScan(Testing_DevicePt);
					//result=TRUE;
					sMacMiiPhyShow(Testing_DevicePt);
					if(result!=TRUE){
					printf("MAC_MII_NOTFIND\n");
					result=-1;
					}
    			else{
    				result=0;
    			}
				break;
				case '2' :
					printf("MII PHY Initial\n");
					MacTestDevCrtlInit();
					result=sMacMiiInit(Testing_DevicePt);
					//result=TRUE;
					sMacMiiPhyShow(Testing_DevicePt);
    			if(result!=TRUE){
				printf("MAC_MII_INITFAIL\n");
				result=-1;
    			}
    			else{
    				result=0;
    			}
				break;
				case '3' :
					printf(" Initial MAC\n");
					MacTestDevCrtlInit();
					MacTestFilterInit();

					result=sMacInit(Testing_DevicePt);
					sMacCsrShow(Testing_DevicePt);

					if(result!=TRUE){
						printf("MAC_MAC_INITFAIL\n");
						result=-1;
						break;
					}

    			//Initial Filter
					result=sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);
					if(result!=TRUE){
						printf("MAC_FILTERSET_FAIL\n");
						result=-1;
					}
    			else{
    				result=0;
    			}
    			sMacCsrShow(Testing_DevicePt);
				break;

				case '4' :
					printf(" Normal Loopback Test (rc2)\n");
					if(Test_No_Phy == 1){
                                                Testing_DevicePt->MII_PhyAD=1;
                                                printf("NO Phy Test\n");
                                        }
					MacTestDevCrtlInit();
					MacTestFilterInit();

				//Initial MAC
					result=sMacInit(Testing_DevicePt);
					if(result!=TRUE){
						printf("MAC_MAC_INITFAIL\n");
						result=-1;
						break;
					}
					
				//Initial Filter
					result=sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);
					if(result!=TRUE){
						printf("MAC_FILTERSET_FAIL\n");
						result=-1;
						break;
					}

				//Start Tx to test
				// Sherlock God
					//sMacMiiPhyReadAll(Testing_DevicePt);	//leonid+
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					MacTestPolling();
				//result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],256,2);
					MacTestPolling();
				//result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],256,2);

					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],512,3);
					MacTestPolling();
				//result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],512,3);

					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],1024,4);
					MacTestPolling();
				//result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],1024,4);

					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,5);
					MacTestPolling();
				//result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,5);

					MacTestPrepareTx(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,6);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,7);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,8);
					MacTestPolling();
				//result=TestCheckRxPattern(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,8);

					MacTestPrepareTx(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,9);
					MacTestPolling();
				//result=TestCheckRxPattern(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,9);

					for(clock=0;clock<RXWAIT_CLOCK;clock++){
						MacTestPolling();
						if(TestCheckRxNum(7))	break;
						MSDELAY(2);
					}
					sMacDevShow(Testing_DevicePt);

					if(clock==RXWAIT_CLOCK){
						printf("MAC_RXWAIT_TIMEOUT\n");
						result=-1;
						break;
					}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					if(result!=0)	break;
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],256,2);
					if(result!=0)	break;
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],512,3);
					if(result!=0)	break;
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],1024,4);
					if(result!=0)	break;
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,5);
					if(result!=0)	break;
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,8);
					if(result!=0)	break;
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,9);
					if(result!=0)	break;

				break;
				case '5' :
					printf(" Long Frame (rc3)\n");
					Test_FilterMode=ALL_BAD;

					MacTestDevCrtlInit();
					MacTestFilterInit();
					sMacInit(Testing_DevicePt);
					sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);

				//Start Tx to test
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],SMAC_MAX_MTU+1,1);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],SMAC_MAX_MTU+256,2);
					MacTestPolling();
					for(clock=0;clock<RXWAIT_CLOCK;clock++){
						MacTestPolling();
						if(TestCheckRxNum(2))	break;
						MSDELAY(2);
					}
					sMacDevShow(Testing_DevicePt);
					if(Testing_DevicePt->errStats.rxTooLong!=2){
						printf("MAC_TESTITEM_FAIL\n");
						result=-1;
					}

					Test_FilterMode=DEFAULT_FILTER;
				break;
				case '6' :
					printf(" Run Frame (rc4)\n");
					Test_DisPadding=1;
					Test_FilterMode=ALL_BAD;

					MacTestDevCrtlInit();
					MacTestFilterInit();
					sMacInit(Testing_DevicePt);
					sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);

				//Start Tx to test
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],45,1);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],4,2);
					MacTestPolling();
					for(clock=0;clock<RXWAIT_CLOCK;clock++){
						MacTestPolling();
						if(TestCheckRxNum(2))	break;
						MSDELAY(2);
					}
					sMacDevShow(Testing_DevicePt);
					if(Testing_DevicePt->errStats.rxRuntFrm!=2){
						printf("MAC_TESTITEM_FAIL\n");
						result=-1;
					}

					Test_DisPadding=0;
					Test_FilterMode=DEFAULT_FILTER;
				break;
				case '7' :
					printf(" Frame with CRC Error (rc6)\n");
					Test_DisCRC=1;
					Test_FilterMode=ALL_BAD;

					MacTestDevCrtlInit();
					MacTestFilterInit();
					sMacInit(Testing_DevicePt);
					sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);

				//Start Tx to test
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU+4,1);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU+4,2);
					MacTestPolling();
					for(clock=0;clock<RXWAIT_CLOCK;clock++){
						MacTestPolling();
						if(TestCheckRxNum(2))	break;
						MSDELAY(2);
					}
					sMacDevShow(Testing_DevicePt);
					if(Testing_DevicePt->errStats.rxCrcErr!=2){
						printf("MAC_TESTITEM_FAIL\n");
						result=-1;
					}

					Test_DisCRC=0;
					Test_FilterMode=DEFAULT_FILTER;
				break;
				case '8' :
					printf(" Receiving in suspended (rc8)\n");
				{
					SMAC_RDE *		desPt;

					MacTestDevCrtlInit();
				//Change Des Owner
					desPt = (SMAC_RDE *)( (int)&Rx_Desc[1] );
					sMAC_RESET_DESC(&desPt->rDesc0,(RDESC0_OWN));

					MacTestFilterInit();
					sMacInit(Testing_DevicePt);
					sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);

					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);

					MSDELAY(100);

				//Now Rx Should be suspended, check it
					if( (sMAC_READ_CSR(Testing_DevicePt->devAdrs, CSR5) & CSR5_RS_MSK) != CSR5_RS_SURB ){
						printf("MAC_TESTITEM_FAIL\n");
						result=-1;
						break;
					}

				//Give the owner
					sMAC_UPDATA_DESC(&desPt->rDesc0,(RDESC0_OWN));

				//Start Tx to test
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,2);
					MacTestPolling();
					for(clock=0;clock<RXWAIT_CLOCK;clock++){
						MacTestPolling();
						if(TestCheckRxNum(2))	break;
						MSDELAY(2);
					}
					sMacDevShow(Testing_DevicePt);
					if(clock==RXWAIT_CLOCK){
						printf("MAC_RXWAIT_TIMEOUT\n");
						result=-1;
						break;
					}

					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					if(result!=0)	break;
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,2);
					if(result!=0)	break;
				}
				break;
				case '9' :
					printf(" Filtering Fail (All (no Filter)) (raf2)\n");
					Test_FilterMode=PERFECT_FAIL;

					MacTestDevCrtlInit();
					MacTestFilterInit();
					sMacInit(Testing_DevicePt);
					result=sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);
					if(result!=TRUE){
						printf("MAC_FILTERSET_FAIL\n");
						result=-1;
						goto case_9_break;
					}
					else{
						result=0;
					}
				//Start Tx to test
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,2);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,3);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,4);
					MacTestPolling();
					for(clock=0;clock<RXWAIT_CLOCK;clock++){
						MacTestPolling();
						if(TestCheckRxNum(4))	break;
						MSDELAY(2);
					}
					sMacDevShow(Testing_DevicePt);
					if(clock==RXWAIT_CLOCK){
						printf("MAC_RXWAIT_TIMEOUT\n");
						result=-1;
						goto case_9_break;
					}

					if(Testing_DevicePt->errStats.rxFiltrErr!=2){
						printf("MAC_TESTITEM_FAIL\n");
						result=-1;
					}

case_9_break:
					Test_FilterMode=DEFAULT_FILTER;
				break;
				case 'A' :	case 'a' :
					printf(" Inverse Perfect Filtering (raf3)\n");
					Test_FilterMode=INVERSE_PERFECT;

					MacTestDevCrtlInit();
					MacTestFilterInit();
					sMacInit(Testing_DevicePt);
					result=sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);
					if(result!=TRUE){
						printf("MAC_FILTERSET_FAIL\n");
						result=-1;
						goto case_a_break;
					}
				//Start Tx to test
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,2);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,3);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,4);
					MacTestPolling();
					for(clock=0;clock<RXWAIT_CLOCK;clock++){
						MacTestPolling();
						if(TestCheckRxNum(2))	break;
						MSDELAY(2);
					}
					sMacDevShow(Testing_DevicePt);
					if(clock==RXWAIT_CLOCK){
						printf("MAC_RXWAIT_TIMEOUT\n");
						result=-1;
						goto case_a_break;
					}

					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,2);
					if(result!=0)	{goto case_a_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,4);
					if(result!=0)	{goto case_a_break;}

case_a_break:
					Test_FilterMode=DEFAULT_FILTER;
				break;
				case 'B' :	case 'b' :
					printf(" Hash Filtering (raf4)\n");
					Test_FilterMode=HASH;
					Test_FilterSize=DEFAULT_HASH_SIZE+DEFAULT_PERFECT_SIZE;

					MacTestDevCrtlInit();
					MacTestFilterInit();
					sMacInit(Testing_DevicePt);
					result=sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);
					if(result!=TRUE){
						printf("MAC_FILTERSET_FAIL\n");
						result=-1;
						goto case_b_break;
					}
				//Start Tx to test
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,12);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,2);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,3);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[2],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,4);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[3],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,5);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[4],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,6);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[5],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,7);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[6],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,8);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[7],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,9);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[9],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,10);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,13);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,11);
					MacTestPolling();
					for(clock=0;clock<RXWAIT_CLOCK;clock++){
						MacTestPolling();
						if(TestCheckRxNum(10))	break;
						MSDELAY(2);
					}
					sMacDevShow(Testing_DevicePt);
					if(clock==RXWAIT_CLOCK){
						printf("MAC_RXWAIT_TIMEOUT\n");
						result=-1;
						goto case_b_break;
					}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					if(result!=0)	{goto case_b_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,2);
					if(result!=0)	{goto case_b_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,3);
					if(result!=0)	{goto case_b_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[2],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,4);
					if(result!=0)	{goto case_b_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[3],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,5);
					if(result!=0)	{goto case_b_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[4],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,6);
					if(result!=0)	{goto case_b_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[5],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,7);
					if(result!=0)	{goto case_b_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[6],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,8);
					if(result!=0)	{goto case_b_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[7],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,9);
					if(result!=0)	{goto case_b_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,11);
					if(result!=0)	{goto case_b_break;}
					if(Testing_DevicePt->rxMultiFCount!=8){
						printf("MAC_TESTITEM_FAIL\n");
						result=-1;
					}
case_b_break:
					Test_FilterSize=DEFAULT_FILTER_SIZE;
					Test_FilterMode=DEFAULT_FILTER;
				break;

				case 'C' :	case 'c' :
					printf(" Hash Only Filtering (raf5)\n");
					Test_FilterMode=HASH_ONLY;
					Test_FilterSize=DEFAULT_HASH_SIZE+1;	//Include My address

					MacTestDevCrtlInit();
					MacTestFilterInit();
					sMacInit(Testing_DevicePt);
					result=sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);
					if(result!=TRUE){
						printf("MAC_FILTERSET_FAIL\n");
						result=-1;
						goto case_c_break;
					}
				//Start Tx to test
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[2],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,12);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,2);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,3);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[2],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,4);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[3],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,5);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[4],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,6);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[5],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,7);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[6],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,8);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[7],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,9);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[9],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,10);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[2],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,13);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,11);
					MacTestPolling();
					for(clock=0;clock<RXWAIT_CLOCK;clock++){
						MacTestPolling();
						if(TestCheckRxNum(10))	break;
						MSDELAY(2);
					}
					sMacDevShow(Testing_DevicePt);
					if(clock==RXWAIT_CLOCK){
						printf("MAC_RXWAIT_TIMEOUT\n");
						result=-1;
						goto case_c_break;
					}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					if(result!=0)	{goto case_c_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,2);
					if(result!=0)	{goto case_c_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,3);
					if(result!=0)	{goto case_c_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[2],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,4);
					if(result!=0)	{goto case_c_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[3],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,5);
					if(result!=0)	{goto case_c_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[4],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,6);
					if(result!=0)	{goto case_c_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[5],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,7);
					if(result!=0)	{goto case_c_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[6],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,8);
					if(result!=0)	{goto case_c_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[7],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,9);
					if(result!=0)	{goto case_c_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,11);
					if(result!=0)	{goto case_c_break;}
					if(Testing_DevicePt->rxMultiFCount!=8){
						printf("MAC_TESTITEM_FAIL\n");
						result=-1;
					}
case_c_break:
					Test_FilterSize=DEFAULT_FILTER_SIZE;
					Test_FilterMode=DEFAULT_FILTER;
				break;
				case 'D' :	case 'd' :
					printf(" Receive Bad packet (raf6)\n");
					Test_FilterMode=ALL_BAD;
					Test_IgnoreError=1;
					Test_DisPadding=1;

					MacTestDevCrtlInit();
					MacTestFilterInit();
					sMacInit(Testing_DevicePt);
					result=sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);
					if(result!=TRUE){
						printf("MAC_FILTERSET_FAIL\n");
						result=-1;
						goto case_d_break;
					}
				//Start Tx to test
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],40,2);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],4,3);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,4);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,5);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[2],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,6);
					MacTestPolling();
					for(clock=0;clock<RXWAIT_CLOCK;clock++){
						MacTestPolling();
						if(TestCheckRxNum(6))	break;
						MSDELAY(2);
					}
					sMacDevShow(Testing_DevicePt);
					if(clock==RXWAIT_CLOCK){
						printf("MAC_RXWAIT_TIMEOUT\n");
						result=-1;
						goto case_d_break;
					}

					if(Testing_DevicePt->errStats.rxRuntFrm!=2){
						printf("MAC_TESTITEM_FAIL\n");
						result=-1;
						goto case_d_break;
					}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					if(result!=0)	{goto case_d_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],40,2);
					if(result!=0)	{goto case_d_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],4,3);
					if(result!=0)	{goto case_d_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,4);
					if(result!=0)	{goto case_d_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,5);
					if(result!=0)	{goto case_d_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[2],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,6);
					if(result!=0)	{goto case_d_break;}
case_d_break:
					Test_FilterMode=DEFAULT_FILTER;
					Test_IgnoreError=DEFAULT_IGNOREERROR;
					Test_DisPadding=0;
				break;

				case 'E' :	case 'e' :
					printf(" Promiscuous Filtering (raf7)\n");
					Test_FilterMode=PROMISCUOUS;

					MacTestDevCrtlInit();
					MacTestFilterInit();
					sMacInit(Testing_DevicePt);
					result=sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);
					if(result!=TRUE){
						printf("MAC_FILTERSET_FAIL\n");
						result=-1;
						goto case_e_break;
					}
				//Start Tx to test
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,2);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,3);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,4);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[2],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,5);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[2],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,6);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,7);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,8);
					MacTestPolling();

					for(clock=0;clock<RXWAIT_CLOCK;clock++){
						MacTestPolling();
						if(TestCheckRxNum(8))	break;
						MSDELAY(2);
					}
					sMacDevShow(Testing_DevicePt);
					if(clock==RXWAIT_CLOCK){
						printf("MAC_RXWAIT_TIMEOUT\n");
						result=-1;
						goto case_e_break;
					}

					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					if(result!=0)	{goto case_e_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,2);
					if(result!=0)	{goto case_e_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,3);
					if(result!=0)	{goto case_e_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,4);
					if(result!=0)	{goto case_e_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[2],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,5);
					if(result!=0)	{goto case_e_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[2],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,6);
					if(result!=0)	{goto case_e_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,7);
					if(result!=0)	{goto case_e_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,8);
					if(result!=0)	{goto case_e_break;}
					if(Testing_DevicePt->rxMultiFCount!=2){
						printf("MAC_TESTITEM_FAIL\n");
						result=-1;
					}
case_e_break:
					Test_FilterMode=DEFAULT_FILTER;
				break;
				case 'F' :	case 'f' :
					printf(" Pass All Multicast (raf8)\n");
					Test_FilterMode=ALL_MULTI;

					MacTestDevCrtlInit();
					MacTestFilterInit();
					sMacInit(Testing_DevicePt);
					result=sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);
					if(result!=TRUE){
						printf("MAC_FILTERSET_FAIL\n");
						result=-1;
						goto case_d_break;
					}
				//Start Tx to test
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,2);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,3);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,4);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,5);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,6);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,7);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[2],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,8);
					MacTestPolling();

					for(clock=0;clock<RXWAIT_CLOCK;clock++){
						MacTestPolling();
						if(TestCheckRxNum(6))	break;
						MSDELAY(2);
					}
					sMacDevShow(Testing_DevicePt);
					if(clock==RXWAIT_CLOCK){
						printf("MAC_RXWAIT_TIMEOUT\n");
						result=-1;
						goto case_f_break;
					}

					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					if(result!=0)	{goto case_f_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,2);
					if(result!=0)	{goto case_f_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,5);
					if(result!=0)	{goto case_f_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,6);
					if(result!=0)	{goto case_f_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,7);
					if(result!=0)	{goto case_f_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[2],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,8);
					if(result!=0)	{goto case_f_break;}
					if(Testing_DevicePt->rxMultiFCount!=4){
						printf("MAC_TESTITEM_FAIL\n");
						result=-1;
					}
case_f_break:
					Test_FilterMode=DEFAULT_FILTER;
				break;
				case 'G': case 'g' :
					printf(" Rx Overflow (rfifo2)\n");
				{
					SMAC_RDE *		desPt;
					int				i,j;

					for(j=1;j<8;j++){
						MacTestDevCrtlInit();
					//Reset All Des Owner form 1
						for(i=1;i<NUM_RDS;i++){
							desPt = (SMAC_RDE *)( (int)&Rx_Desc[i] );
							sMAC_RESET_DESC(&desPt->rDesc0,(RDESC0_OWN));
						}

						MacTestFilterInit();
						sMacInit(Testing_DevicePt);
						sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);

					// Use 1
						MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);

					// Ocuppy FIFO
						MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,2);

					//Start Tx to test
						for(i=0;i<j;i++){
						//Overflow Pattern
							MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,i+2);
						}

						MSDELAY(100);
#ifdef CONFIG_PC7210
						printf(".");	//leonid+ for 7210 20080123
#endif

						if( (sMAC_READ_CSR(Testing_DevicePt->devAdrs, CSR5) & CSR5_RS_MSK) != CSR5_RS_SURB ){
							printf("MAC_TESTITEM_FAIL\n");
							result=-1;
							goto case_g_break;
						}

					//Give the Owner back
						for(i=1;i<NUM_RDS;i++){
							desPt = (SMAC_RDE *)( (int)&Rx_Desc[i] );
							desPt->rDesc0 = DESSWAP(RDESC0_OWN);
						}

						sMAC_WRITE_CSR( Testing_DevicePt->devAdrs, CSR2, CSR2_RPD);

						for(clock=0;clock<RXWAIT_CLOCK;clock++){
							MacTestPolling();
							if(TestCheckRxNum(2))	break;
							MSDELAY(2);
						}
						sMacDevShow(Testing_DevicePt);

						if(clock==RXWAIT_CLOCK){
							printf("MAC_RXWAIT_TIMEOUT\n");
							result=-1;
							goto case_g_break;
						}

						result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
						if(result!=0){
							goto case_g_break;
						}
						result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,2);
						if(result!=0){
							goto case_g_break;
						}

						if(Testing_DevicePt->errStats.rxOverflow != j){
							printf("MAC_TESTITEM_FAIL\n");
							result=-1;
							printf(" Error get overflow = %x Should be = %x\n", Testing_DevicePt->errStats.rxOverflow, j);
							goto case_g_break;
						}
					}
				}
case_g_break:
				break;
				case 'H' : case 'h' :
					printf(" Single Buffer2 (rlsm2/tlsm2)\n");
					Test_SingleBuff1=0;
					Test_SingleBuff2=1;
	
					MacTestDevCrtlInit();
					MacTestFilterInit();
					sMacInit(Testing_DevicePt);

				//Initial Filter
					sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);

				//Start Tx to test
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],256,2);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],512,3);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],1024,4);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,5);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,6);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,7);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,8);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,9);
					MacTestPolling();

					for(clock=0;clock<RXWAIT_CLOCK;clock++){
						MacTestPolling();
						if(TestCheckRxNum(7))	break;
						MSDELAY(2);
					}
					sMacDevShow(Testing_DevicePt);
					if(clock==RXWAIT_CLOCK){
						printf("MAC_RXWAIT_TIMEOUT\n");
						result=-1;
						goto case_h_break;
					}

					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					if(result!=0)	{goto case_h_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],256,2);
					if(result!=0)	{goto case_h_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],512,3);
					if(result!=0)	{goto case_h_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],1024,4);
					if(result!=0)	{goto case_h_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,5);
					if(result!=0)	{goto case_h_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,8);
					if(result!=0)	{goto case_h_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,9);
					if(result!=0)	{goto case_h_break;}

case_h_break:
					Test_SingleBuff1=DEFAULT_SINGLEBUFF1;
					Test_SingleBuff2=DEFAULT_SINGLEBUFF2;
				break;
				case 'I' : case 'i' :
					printf(" Chain Mode (rlsm9/tlsm9)\n");
					Test_ChainDes=1;
	
					MacTestDevCrtlInit();
					MacTestFilterInit();
					sMacInit(Testing_DevicePt);

				//Initial Filter
					sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);

				//Start Tx to test
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],256,2);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],512,3);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],1024,4);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,5);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,6);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,7);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,8);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,9);
					MacTestPolling();

					for(clock=0;clock<RXWAIT_CLOCK;clock++){
						MacTestPolling();
						if(TestCheckRxNum(7))	break;
						MSDELAY(2);
					}
					sMacDevShow(Testing_DevicePt);
					if(clock==RXWAIT_CLOCK){
						printf("MAC_RXWAIT_TIMEOUT\n");
						result=-1;
						goto case_i_break;
					}

					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					if(result!=0)	{goto case_i_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],256,2);
					if(result!=0)	{goto case_i_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],512,3);
					if(result!=0)	{goto case_i_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],1024,4);
					if(result!=0)	{goto case_i_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,5);
					if(result!=0)	{goto case_i_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,8);
					if(result!=0)	{goto case_i_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,9);
					if(result!=0)	{goto case_i_break;}

case_i_break:
					Test_ChainDes=0;
				break;
				case 'J' : case 'j' :
					printf(" Empty Descriptors for Rx(rlsm10)\n");
				{
					SMAC_RDE *		desPt;
	
					MacTestDevCrtlInit();
	
					//Change 1 Des to Empty
					rBuffSize[1].Buffer1_Size=0;
					rBuffSize[1].Buffer2_Size=0;
					desPt = (SMAC_RDE *)( (int)&Rx_Desc[1] );
					sMAC_RESET_DESC(&desPt->rDesc1,(RDESC1_RBS1_MSK|RDESC1_RBS2_MSK));
	
					MacTestFilterInit();
					sMacInit(Testing_DevicePt);
	
					sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);
	
					//Start Tx to test
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],1024,1);
					MacTestPolling();
	
					for(clock=0;clock<RXWAIT_CLOCK;clock++){
						MacTestPolling();
						if(TestCheckRxNum(1))
							break;
						MSDELAY(2);
					}
					sMacDevShow(Testing_DevicePt);
					if(clock==RXWAIT_CLOCK){
						printf("MAC_RXWAIT_TIMEOUT\n");
						result=-1;
						goto case_j_break;
					}
				/* HW ISSUE 6 */
				// if(Testing_DevicePt->rxIndex != ((int)(1024/(TRX1_DATA_SIZE*2))+1+1+1))
					if(Testing_DevicePt->rxIndex != ((int)(1024/(TRX1_DATA_SIZE))+1+1)){
						printf("MAC_TESTITEM_FAIL\n");
						result=-1;
						goto case_j_break;
					}
	
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],1024,1);
					if(result!=0){
						goto case_j_break;
					}
				}
case_j_break:
				break;
				case 'K' : case 'k' :
					printf(" Descriptors Unavailable (rlsm12)\n");
				{
					SMAC_RDE *		desPt;
	
					MacTestDevCrtlInit();
					//Change Second Des Owner
					desPt = (SMAC_RDE *)( (int)&Rx_Desc[1] );
					sMAC_RESET_DESC(&desPt->rDesc0,(RDESC0_OWN));
	
					MacTestFilterInit();
					sMacInit(Testing_DevicePt);
	
					sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);
	
					//Start Tx to test
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],1024,1);
					MacTestPolling();
	
					for(clock=0;clock<RXWAIT_CLOCK;clock++)
					{
						MacTestPolling();
						if(TestCheckRxNum(7))	break;
						MSDELAY(2);
					}
					sMacDevShow(Testing_DevicePt);
					if(Testing_DevicePt->int_counter.int_ru!=1)
					{
						printf("MAC_TESTITEM_FAIL\n");
						result=-1;
					}
					if(Testing_DevicePt->errStats.rxDescErr!=1)
					{
						printf("MAC_TESTITEM_FAIL\n");
						result=-1;
					}
				}
				break;
				case 'L' : case 'l' :
					printf(" Descriptors Skip Ring Mode (rlsm11/tlsm11)\n");
					Test_SkipDes=1;
	
					MacTestDevCrtlInit();
					MacTestFilterInit();
					sMacInit(Testing_DevicePt);
	
					//Initial Filter
					sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);
	
					//Start Tx to test
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],256,2);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],512,3);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],1024,4);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,5);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,6);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,7);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,8);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,9);
					MacTestPolling();
	
					for(clock=0;clock<RXWAIT_CLOCK;clock++){
						MacTestPolling();
						if(TestCheckRxNum(7))
							break;
						MSDELAY(2);
					}
					sMacDevShow(Testing_DevicePt);
					if(clock==RXWAIT_CLOCK){
						printf("MAC_RXWAIT_TIMEOUT\n");
						result=-1;
						goto case_l_break;
					}
	
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					if(result!=0)	{goto case_l_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],256,2);
					if(result!=0)	{goto case_l_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],512,3);
					if(result!=0)	{goto case_l_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],1024,4);
					if(result!=0)	{goto case_l_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,5);
					if(result!=0)	{goto case_l_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,8);
					if(result!=0)	{goto case_l_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,9);
					if(result!=0)	{goto case_l_break;}

case_l_break:
					Test_SkipDes=0;
				break;
				case 'M' : case 'm' :
					printf(" Dual Buffer (rlsm3/tlsm3)\n");
					Test_SingleBuff2=0;
					Test_SingleBuff1=0;
	
					MacTestDevCrtlInit();
					MacTestFilterInit();
					sMacInit(Testing_DevicePt);
	
					//Initial Filter
					sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);
	
					//Start Tx to test
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],256,2);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],512,3);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],1024,4);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,5);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,6);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,7);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,8);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,9);
					MacTestPolling();
	
					for(clock=0;clock<RXWAIT_CLOCK;clock++){
						MacTestPolling();
						if(TestCheckRxNum(7))	break;
						MSDELAY(2);
					}
					sMacDevShow(Testing_DevicePt);
					if(clock==RXWAIT_CLOCK){
						printf("MAC_RXWAIT_TIMEOUT\n");
						result=-1;
						goto case_m_break;
					}
	
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					if(result!=0)	{goto case_m_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],256,2);
					if(result!=0)	{goto case_m_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],512,3);
					if(result!=0)	{goto case_m_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],1024,4);
					if(result!=0)	{goto case_m_break;}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,5);
					if(result!=0)	{goto case_m_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,8);
					if(result!=0)	{goto case_m_break;}
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,9);
					if(result!=0)	{goto case_m_break;}

case_m_break:
					Test_SingleBuff2=DEFAULT_SINGLEBUFF2;
					Test_SingleBuff1=DEFAULT_SINGLEBUFF1;
				break;
				case 'N' : case 'n' :
					printf(" Interrupt Mitigation All NRP & NTP (rim/tim)\n");
				{
					int	nrtp;
					int	i;
					int j;
					int	intnum;
					int intpadding;
					int save_TAP;
	
					save_TAP=Test_Auto_Polling_Tx;
					Test_Auto_Polling_Tx=0;
					for(nrtp=7;nrtp>1;nrtp--){
						printf("NRP/NTP Test Case N = %x", nrtp);
						MacTestDevCrtlInit();
						MacTestFilterInit();
						sMacInit(Testing_DevicePt);
	
						Testing_DevicePt->tx_merge_counter=nrtp-1;
						Testing_DevicePt->tx_merge_number=nrtp-1;

					//Initial Filter
						sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);

					//Disable other Interrupt (only open TI and RI)
					//Otherwise We read interrupt will clear NRTP
						sMAC_WRITE_CSR( Testing_DevicePt->devAdrs, CSR7, (CSR7_NIS|CSR7_RI|CSR7_TI) );
	
						sMAC_WRITE_CSR( Testing_DevicePt->devAdrs, CSR11
										, (CSR11_NTP_VAL(nrtp)|CSR11_NRP_VAL(nrtp)) );
						intnum=0;
						intpadding=nrtp;
						for(i=0;i<28;i++){
						//Test 28 Packet
							MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,i);
							intpadding--;
							if(intpadding==0){
								intpadding=nrtp;
								intnum++;
								//Delay to finish loopback
								MSDELAY(100);
#ifdef CONFIG_PC7210
						printf(".");	//leonid+ for 7210 20080123
#endif
								MacTestPolling();
								//Return to Mask
								sMAC_WRITE_CSR( Testing_DevicePt->devAdrs, CSR7, (CSR7_NIS|CSR7_RI|CSR7_TI) );
							}
							//Delay to finish loopback
							MSDELAY(100);
							MacTestPolling();
							if(intpadding==nrtp){
								for(j=(i+1-nrtp);j<=i;j++){
									result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,j);
									if(result!=0)	{goto case_n_break;}
								}
								sMacDevShow(Testing_DevicePt);
							}
						}
						if( (Testing_DevicePt->int_counter.int_ri!=intnum)
						  ||(Testing_DevicePt->int_counter.int_ti!=intnum) ){
							printf("    Test Fail interrupt(%x) ri(%x) ti(%x)", 
								intnum, Testing_DevicePt->int_counter.int_ri, Testing_DevicePt->int_counter.int_ti);
							printf("MAC_TESTITEM_FAIL\n");
							result=-1;
							goto case_n_break;
						}
						printf("........OK\n");
					}
case_n_break:
					Testing_DevicePt->tx_merge_counter=0;
					Testing_DevicePt->tx_merge_number=0;
					Test_Auto_Polling_Tx=save_TAP;
				}
				break;
				case 'O' : case'o' :
					printf(" Continue Tx 60~1500 Packet ... \n");
				{
					int		i;
	
					MacTestDevCrtlInit();
					MacTestFilterInit();
					sMacInit(Testing_DevicePt);
	
					//Initial Filter
					sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);
	
					for(i=60;i<1501;i++){
					//Test 100
					//Test 2*NUM_RDS Packet (Could Try Rx Ring back function)
						MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],i,(i&0x7f));
						for(clock=0;clock<RXWAIT_CLOCK;clock++){
							MacTestPolling();
							if(TestCheckRxNum(1))	break;
							MSDELAY(2);
						}
						if(clock==RXWAIT_CLOCK){
							printf("MAC_RXWAIT_TIMEOUT\n");
							result=-1;
							goto case_o_break;
						}
						result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],i,(i&0x7f));
						if(result!=0)	{goto case_o_break;}
					}
				}
case_o_break:
				sMacDevShow(Testing_DevicePt);
				break;
				case 'P' : case'p' :
				{
					int	save_TR;
	
					save_TR=Test_Tx_TR;
	
					//Underflow could not use store and forward
					Test_Tx_TR=1;
					printf(" Tx Underflow Test (tfifo9)\n");
					Test_Tx_Underflow=1;
	
					MacTestDevCrtlInit();
					MacTestFilterInit();
					sMacInit(Testing_DevicePt);
	
					//Initial Filter
					sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);
	
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],SMAC_MAX_MTU,1);
					MSDELAY(100);
					sMacDevShow(Testing_DevicePt);

					if(Testing_DevicePt->int_counter.int_unf!=1){
						printf("MAC_TESTITEM_FAIL\n");
						result=-1;
						goto case_p_break;
					}

case_p_break:
					Test_Tx_TR=save_TR;
					Test_Tx_Underflow=0;
				}
				break;
				case 'R' : case 'r' :
					printf(" Interrupt Mitigation TT & RT (rim/tim)\n");
				{
					int	nrtp;
					int trt;
					int cs;
					volatile int *pt;
	
					for(nrtp=2;nrtp<3;nrtp++){
						printf("NRP/NTP Test Case N = %x\n", nrtp);
						for(cs=0;cs<2;cs++)	{
							printf("..... CS Test Case N = %x\n", cs);
							for(trt=1;trt<16;trt++){
								printf(".......... TT/RT Test Case N = %x    Enter any key when LA Ready\n", trt);
								scanf("%c", &cmd_buf[0]);
	
								MacTestDevCrtlInit();
								MacTestFilterInit();
								sMacInit(Testing_DevicePt);
	
							//Initial Filter
								sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);
	
							//Disable other Interrupt (only open TI and RI)
							//Otherwise We read interrupt will clear NRTP
								sMAC_WRITE_CSR( Testing_DevicePt->devAdrs, CSR7, (CSR7_NIS|CSR7_RI|CSR7_TI) );
	
								sMAC_WRITE_CSR( Testing_DevicePt->devAdrs, CSR11
											, (CSR11_NTP_VAL(nrtp)|CSR11_NRP_VAL(nrtp)|CSR11_RT_VAL(trt)|CSR11_TT_VAL(trt)|CSR11_CS_VAL(cs)) );
	
								MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
								pt=&Testing_DevicePt->int_counter.int_ri;
								while(*pt==0){ }
							}
						}
					}
				}
				break;
				case 't' :
				case 'T' :
					printf("Miss Frames & FIFO Test\n");
				{
					SMAC_RDE *		desPt;
					int 			i,j;
					int				pending_rx;
	
					// how many pending
					for(j=1;j<16;j++){
						printf(" Test unavailable descriptor at (%x) times Rx\n", j);
	
						MacTestDevCrtlInit();
	
						//Reset All Des Owner
						for(i=1;i<NUM_RDS;i++){
							desPt = (SMAC_RDE *)( (int)&Rx_Desc[i] );
							sMAC_RESET_DESC(&desPt->rDesc0,(RDESC0_OWN));
						}
	
						MacTestFilterInit();
						sMacInit(Testing_DevicePt);
						sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);
	
						//Use one to let RU on
						MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],0xA0,0x7f);
	
						//Start Tx to test
						for(i=0;i<j;i++){
							MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU+i,i);
						}
	
						MSDELAY(100);
	
						//Reset All Des Owner
						for(i=1;i<NUM_RDS;i++){
							desPt = (SMAC_RDE *)( (int)&Rx_Desc[i] );
							sMAC_UPDATA_DESC(&desPt->rDesc0,(RDESC0_OWN));
						}
						sMAC_WRITE_CSR( Testing_DevicePt->devAdrs, CSR2, CSR2_RPD);
						MSDELAY(100);
						for(i=0;i<10;i++){
							MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU+NUM_RDS+i,NUM_RDS+i);
						}
	
						MSDELAY(100);
						MacTestPolling();
	
						if(j>3){
							pending_rx=3;
						}
						else{
							pending_rx=j;
						}
						result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],0xA0,0x7f);
						if(result!=0)	{goto case_t_break;}
	
						for(i=0;i<pending_rx;i++){
							result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU+i,i);
							if(result!=0)	{goto case_t_break;}
						}
	
						for(i=0;i<10;i++){
							result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU+NUM_RDS+i,NUM_RDS+i);
							if(result!=0)	{goto case_t_break;}
						}
	
						sMacDevShow(Testing_DevicePt);
	
						if(Testing_DevicePt->errStats.rxMissed!=(j-pending_rx)){
							printf("MAC_TESTITEM_FAIL\n");
							result=-1;
							printf(" Error get missed frame = %x\n", Testing_DevicePt->errStats.rxMissed);
							goto case_t_break;
						}
					}
				}
case_t_break:
				break;
	
				case 'u' :
				case 'U' :
					printf(" Rx Overflow Robust Test\n");
				{
					SMAC_RDE *		desPt;
					int				i,j;
	
					for(i=1;i<8;i++){
						MacTestDevCrtlInit();
						//Reset All Des Owner for 1
						for(j=1;j<NUM_RDS;j++){
							desPt = (SMAC_RDE *)( (int)&Rx_Desc[j] );
							sMAC_RESET_DESC(&desPt->rDesc0,(RDESC0_OWN));
						}
	
						MacTestFilterInit();
						sMacInit(Testing_DevicePt);
						sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);
	
						//First use use 1 des (0)
						MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,i*16);
	
						//Overflow Pattern
						for(j=0;j<i+1;j++){
							MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,(i*16)+j+1);
						}
	
						MSDELAY(200);
#ifdef CONFIG_PC7210
						printf(".");	//leonid+ for 7210 20080123
#endif
	
						for(j=1;j<NUM_RDS;j++){
							desPt = (SMAC_RDE *)( (int)&Rx_Desc[j] );
							desPt->rDesc0 = DESSWAP(RDESC0_OWN);
						}
	
						//sMAC_WRITE_CSR( Testing_DevicePt->devAdrs, CSR2, CSR2_RPD);
	
						for(j=0;j<8;j++){
							MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,(i*32)+j+1);
						}
	
						for(clock=0;clock<RXWAIT_CLOCK;clock++){
							MacTestPolling();	//Inside the Polling have Poll CSR2_RPD
							if(TestCheckRxNum(2))	break;
							MSDELAY(2);
						}
						sMacDevShow(Testing_DevicePt);
	
						if(clock==RXWAIT_CLOCK){
							printf("MAC_RXWAIT_TIMEOUT\n");
							result=-1;
							goto case_u_break;
						}
	
						result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,i*16);
						if(result!=0)	{goto case_u_break;}
						result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,(i*16)+0+1);
						if(result!=0)	{goto case_u_break;}
	
						/*
						if(Testing_DevicePt->errStats.rxOverflow != (i+8)){
							result=MAC_TESTITEM_FAIL;
							printf("Error get overflow = %x  Should be = %x\n", Testing_DevicePt->errStats.rxOverflow, i)
							goto case_u_break;
						}
						*/
						//MSDELAY(1000);
					}
				}
case_u_break:
				break;
	
				case 'v' :
				case 'V' :
				{
					int i;
					int preDummy;
	
					do{
						printf(" Test the Packet Size = %x\n", RealEtherPacketLen);
						for(preDummy=0;preDummy<NUM_RDS;preDummy++){
							printf(" Test at Pre-dummy = %x\n", preDummy);
	
							while(Testing_DevicePt->rxIndex!=preDummy){
								MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,0x7f);
								for(clock=0;clock<RXWAIT_CLOCK;clock++){
									MacTestPolling();
									if(TestCheckRxNum(1))	break;
									MSDELAY(2);
								}
								if(clock==RXWAIT_CLOCK){
									printf("MAC_RXWAIT_TIMEOUT\n");
									result=-1;
									goto case_v_break;
								}
							//Discard
								Testing_DevicePt->rx_PooliRead++;
								if((Testing_DevicePt->rx_PooliRead)==(Testing_DevicePt->rx_PooliMax))
									Testing_DevicePt->rx_PooliRead=0;
							}
	
							for(i=0;i<2;i++){
								//RealEtherPacketLen=0x131;
								//RealEtherPacketID=RealEtherPacketLen&0x7F;
								MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],RealEtherPacketLen,RealEtherPacketID);
								for(clock=0;clock<RXWAIT_CLOCK;clock++){
									MacTestPolling();
									if(TestCheckRxNum(1))	break;
									MSDELAY(2);
								}
								if(clock==RXWAIT_CLOCK){
									sMacMiiPhyReadAll(Testing_DevicePt);
									printf(" Could not Rx Error !!! Pre-dummy = %x rxIndex = %x\n", preDummy, Testing_DevicePt->rxIndex);
	
									printf("MAC_RXWAIT_TIMEOUT\n");
									result=-1;
									goto case_v_break;
								}
								result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],RealEtherPacketLen,RealEtherPacketID);
								if(result!=0){
									sMacMiiPhyReadAll(Testing_DevicePt);
									printf(" Check Pattern Error !!! Pre-dummy = %x rxIndex = %x\n", preDummy, Testing_DevicePt->rxIndex);
									goto case_v_break;
								}
							}
						}
					}while(Forever_Test);
				}
case_v_break:
				break;
				
				case 'y' :	//leonid+
				case 'Y' :
					printf(" Normal external loopback Test \n");
					Test_LoopBack=0;
					Test_Ethernet_Frame=1;
					MacTestDevCrtlInit();
					MacTestFilterInit();

				//Initial MAC
					result=sMacInit(Testing_DevicePt);
					if(result!=TRUE){
						printf("MAC_MAC_INITFAIL\n");
						result=-1;
						break;
					}

				//Initial Filter
					result=sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);
					if(result!=TRUE){
						printf("MAC_FILTERSET_FAIL\n");
						result=-1;
						break;
					}

				//Start Tx to test
				// Sherlock God
					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					MacTestPolling();
				//result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);

					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],256,2);
					MacTestPolling();
				//result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],256,2);

					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],512,3);
					MacTestPolling();
				//result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],512,3);

					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],1024,4);
					MacTestPolling();
				//result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],1024,4);

					MacTestPrepareTx(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,5);
					MacTestPolling();
				//result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,5);

					MacTestPrepareTx(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,6);
					MacTestPolling();
					MacTestPrepareTx(Test_Perfect_MAC_Addr[1],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,7);
					MacTestPolling();
					MacTestPrepareTx(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,8);
					MacTestPolling();
				//result=TestCheckRxPattern(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,8);

					MacTestPrepareTx(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,9);
					MacTestPolling();
				//result=TestCheckRxPattern(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,9);


					for(clock=0;clock<RXWAIT_CLOCK;clock++){
						MacTestPolling();
						if(TestCheckRxNum(7))	break;
						MSDELAY(2);
					}
					sMacDevShow(Testing_DevicePt);

					if(clock==RXWAIT_CLOCK){
						printf("MAC_RXWAIT_TIMEOUT\n");
						result=-1;
						break;
					}
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,1);
					if(result!=0)	break;
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],256,2);
					if(result!=0)	break;
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],512,3);
					if(result!=0)	break;
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],1024,4);
					if(result!=0)	break;
					result=TestCheckRxPattern(Test_Perfect_MAC_Addr[0],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,5);
					if(result!=0)	break;
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MIN_MTU,8);
					if(result!=0)	break;
					result=TestCheckRxPattern(Test_Hash_MAC_Addr[8],Test_Perfect_MAC_Addr[0],TEST_MAX_MTU,9);
					if(result!=0)	break;

				break;
				
//-----------------------------------------------------------------------------
				case 'w' :
				case 'W' :
					printf(" Dead Lock Test...........\n");
					{
						int deadlock = 1;
						volatile int *pt;
						int temp;
						pt=(volatile int *)0xa0000800;
						while(deadlock) { temp = *pt; }
					}
				break;
				case 'z' :
				case 'Z' :
					printf("!!! Start Fully MAC Item AutoTesting\n");
					MAC_Auto_Test_ItemNum = sizeof(MAC_Auto_Test_Item);
					MAC_Auto_Test_CurrentNum = 0;
					goto MacMain_break;
				break;
				case '~' :
					printf("!!!Real Ethernet Data Testing (Non Loopback)\n");
					MAC_RealEtherTesting(0);
				break;
				case '`' :
					printf("!!!Real Ethernet Auto Data Testing (Non Loopback)\n");
					MAC_RealEtherTesting(1);
				break;	
							
//-----------------------------------------------------------------------------
				case '?' :
					printf("Display MAC Menu\n");
					Display_MacMenu();					
					goto MacMain_break;
				break;
	
				case 'x' : //X. Exit MAC Part Back to Main Menu
				case 'X' :
					printf("Bye Bye ..........\n");
					return(auto_ret);
				break;
				default :
					printf("??? Un-Implement MAC Testing Feature ???\n");
					printf("Please ReTry .........\n");
					goto MacMain_break;
				break;
			}
		}
		if(result != 0){
//			printf("!!!! Testing Error ... Error Code = %x", result);
			printf("!!!! Testing Error");
			auto_ret = -1;
#if 0
			if(result == MAC_MII_NOTFIND	) printf("MAC_MII_NOTFIND   ");
			if(result == MAC_MII_INITFAIL	) printf("MAC_MII_INITFAIL  ");
			if(result == MAC_MAC_INITFAIL	) printf("MAC_MAC_INITFAIL  ");
			if(result == MAC_FILTERSET_FAIL	) printf("MAC_FILTERSET_FAIL");
			if(result == MAC_RXDADDR_FAIL	) printf("MAC_RXDADDR_FAIL  ");
			if(result == MAC_RXSADDR_FAIL	) printf("MAC_RXSADDR_FAIL  ");
			if(result == MAC_RXLEN_FAIL	) printf("MAC_RXLEN_FAIL	  ");
			if(result == MAC_RXDATA_FAIL	) printf("MAC_RXDATA_FAIL	  ");
			if(result == MAC_RXWAIT_TIMEOUT	) printf("MAC_RXWAIT_TIMEOUT");
			if(result == MAC_TESTITEM_FAIL	) printf("MAC_TESTITEM_FAIL ");
			printf("\n");
#endif
		}
		else{
			printf("Pass !!! Test Finish");
		}
		printf(".......MAC Item (%c)\n", cmd_buf[0]);

	}

#ifdef CONFIG_PC9220
	socle_scu_dev_disable(SCU_DEVCON_MAC_GPIO);
#endif
	
	return(result);
}

int	Paser_ParaSetting(char cmd)
{
int	result;

	result=TRUE;

	switch(cmd)
	{
			case '!' : //!.Set 100M (Default)
				Test_Speed_100M=1;
				printf("... 100M Seting Done.\n");
			break;
			case '@' : //@.Set 10M
				Test_Speed_100M=0;
				printf("...  10M Seting Done.\n");
			break;
			case '#' : //#.Set Full Duplex (Default)
				Test_Full_Duplex=1;
				printf("...  Full Duplex Seting Done.\n");
			break;
			case '$' : //$.Set Half Duplex
				Test_Full_Duplex=0;
				printf("...  Half Duplex Seting Done.\n");
			break;

			#ifdef	VERIABLE_BYTE_ORDER
			case '%' : //%. Set Descriptor Big Endian\n");
				sMAC_Des_Byte_Order=F_BIG_ENDIAN;
				printf("...  Descriptor Big Endian Seting Done.\n");
			break;
			case '^' : //^. Set Descriptor Little Endian\n");
				sMAC_Des_Byte_Order=F_LITTLE_ENDIAN;
				printf("...  Descriptor Little Endian Seting Done.\n");
			break;
			case '&' : //&. Set Data Buffer Big Endian\n");
				sMAC_DataB_Byte_Order=F_BIG_ENDIAN;
				printf("...  Data Buffer Big Endian Seting Done.\n");
			break;
			case '*' : //*. Set Data Buffer Little Endian\n");
				sMAC_DataB_Byte_Order=F_LITTLE_ENDIAN;
				printf("...  Data Buffer Little Endian Seting Done.\n");
			break;
			#endif	/* VERIABLE_BYTE_ORDER */

			case '(' : //(. Set Equal Aribitration
				Test_Bus_Arbitration=1;
				printf("...  Equal Aribitration Seting Done.\n");
			break;
			case ')' : //). Set Rx High Aribitration
				Test_Bus_Arbitration=0;
				printf("...  Rx High Aribitration Seting Done.\n");
			break;
			case '}' : //}. Increase the Burst Length Setting
				if(Test_DMA_Burst==0)	Test_DMA_Burst=1;
				else					Test_DMA_Burst=Test_DMA_Burst<<1;
				if(Test_DMA_Burst>32)	Test_DMA_Burst=0;
				switch(Test_DMA_Burst)
				{
					case 0 :  	printf("...  Burst Length unlimited Seting Done.\n");	break;
					case 1 :  	printf("...  Burst Length 1 Seting Done.\n");			break;
					case 2 :  	printf("...  Burst Length 2 Seting Done.\n");			break;
					case 4 :  	printf("...  Burst Length 4 Seting Done.\n");			break;
					case 8 :  	printf("...  Burst Length 8 Seting Done.\n");			break;
					case 16 :  	printf("...  Burst Length 16 Seting Done.\n");		break;
					case 32 :  	printf("...  Burst Length 32 Seting Done.\n");		break;
				}
			break;
			case '{' : // Decrease the Burst Length Setting
				if(Test_DMA_Burst==0)	Test_DMA_Burst=32;
				else					Test_DMA_Burst=Test_DMA_Burst>>1;
				switch(Test_DMA_Burst)
				{
					case 0 :  	printf("...  Burst Length unlimited Seting Done.\n");	break;
					case 1 :  	printf("...  Burst Length 1 Seting Done.\n");			break;
					case 2 :  	printf("...  Burst Length 2 Seting Done.\n");			break;
					case 4 :  	printf("...  Burst Length 4 Seting Done.\n");			break;
					case 8 :  	printf("...  Burst Length 8 Seting Done.\n");			break;
					case 16 :  	printf("...  Burst Length 16 Seting Done.\n");		break;
					case 32 :  	printf("...  Burst Length 32 Seting Done.\n");		break;
				}
			break;
			case '+' : //+. Increase the Transmit Threshold
				Test_Tx_TR++;
				if(Test_Tx_TR>4)	Test_Tx_TR=0;
				if(Test_Tx_TR==4)
				{
					printf("...  Store & Forward Seting Done.\n");
				}
				else
				{
					printf("...  Transmit Threshold %d Seting Done.\n", Test_Tx_TR);
				}
			break;
			case '_' : //_. Decrease the Transmit Threshold
				Test_Tx_TR--;
				if(Test_Tx_TR<0)	Test_Tx_TR=4;
				if(Test_Tx_TR==4)
				{
					printf("...  Store & Forward Seting Done.\n");
				}
				else
				{
					printf("...  Transmit Threshold %d Seting Done.\n", Test_Tx_TR);
				}
			break;
			case '>' : //>. Increase the Transmit Auto Polling Setting
				Test_Auto_Polling_Tx++;
				if(Test_Auto_Polling_Tx>7)	Test_Auto_Polling_Tx=0;
				if(Test_Auto_Polling_Tx==0)
				{
					printf("...  Disable Transmit Auto Polling Seting Done.\n");
				}
				else
				{
					printf("...  Transmit Auto Polling %d Seting Done.\n", Test_Auto_Polling_Tx);
				}
			break;
			case '<' : //<. Decrease the Transmit Auto Polling Setting
				Test_Auto_Polling_Tx--;
				if(Test_Auto_Polling_Tx<0)	Test_Auto_Polling_Tx=7;
				if(Test_Auto_Polling_Tx==0)
				{
					printf("...  Disable Transmit Auto Polling Seting Done.\n");
				}
				else
				{
					printf("...  Transmit Auto Polling %d Seting Done.\n", Test_Auto_Polling_Tx);
				}
			break;
			case ':' : //:. Ethernet Frame
				Test_Ethernet_Frame=1;
				printf("...  Ethernet Frame Seting Done.\n");
			break;
			case '"' : //". IEEE 802.3 Frame
				Test_Ethernet_Frame=0;
				printf("...  IEEE 802.3 Frame Seting Done.\n");
			break;
			case ',' :
				printf("Turn on all the show setting Done\n");
				sMacEnableMiiPhyShow=1;
				sMacEnableCsrShow=1;
				sMacEnableDevShow=1;
				sMacEnableDataShow=1;
				Enable_Log_HIF=1;
			break;
			case '.' :
				printf("Turn off all the show setting Done\n");
				sMacEnableMiiPhyShow=0;
				sMacEnableCsrShow=0;
				sMacEnableDevShow=0;
				sMacEnableDataShow=0;
				Enable_Log_HIF=0;
			break;
			case '[' :
				printf("Forever Set\n");
				Forever_Test=1;
			break;
			case ']' :
				printf("Forever Reset\n");
				Forever_Test=0;
			break;
			/*  leonid+ for translation without phy 	*/
			case '~' :
				printf("Translate without PHY test\n");
				Test_No_Phy=1;
			break;
			case '`' :
				printf("Translate with PHY test\n");
				Test_No_Phy=0;
			break;

			case '=' : //=.Echo Setting
				#if	(CPU_BYTE_ORDER	== F_BIG_ENDIAN)
					printf("\nSetting ...  CPU Byte Order - Big (Non-changeable)\n");
				#else
					printf("\nSetting ...  CPU Byte Order - Little (Non-changeable)\n");
				#endif
				#ifndef	VERIABLE_BYTE_ORDER
				#if	(DES_BYTE_ORDER	== F_BIG_ENDIAN)
					printf("        ...  Descriptor Byte Order - Big (Non-changeable)\n");
				#else
					printf("        ...  Descriptor Byte Order - Little (Non-changeable)\n");
				#endif
				#if	(DES_BYTE_ORDER	== F_BIG_ENDIAN)
					printf("        ...  Data Buffer Byte Order - Big (Non-changeable)\n");
				#else
					printf("        ...  Data Buffer Byte Order - Little (Non-changeable)\n");
				#endif
				#else
				if	(sMAC_Des_Byte_Order	== F_BIG_ENDIAN)
					printf("        ...  Descriptor Byte Order - Big\n");
				else
					printf("        ...  Descriptor Byte Order - Little\n");

				if	(sMAC_DataB_Byte_Order	== F_BIG_ENDIAN)
					printf("        ...  Data Buffer Byte Order - Big\n");
				else
					printf("        ...  Data Buffer Byte Order - Little\n");
				#endif	/* VERIABLE_BYTE_ORDER */
				if(Test_Speed_100M)
				{
					printf("        ...  100M\n");
				}
				else
				{
					printf("        ...  10M\n");
				}
				if(Test_Full_Duplex)
				{
					printf("        ...  Full Duplex\n");
				}
				else
				{
					printf("        ...  Half Duplex\n");
				}
				if(Test_Bus_Arbitration)
				{
					printf("        ...  Equal Aribitration\n");
				}
				else
				{
					printf("        ...  Rx High Aribitration\n");
				}
				switch(Test_DMA_Burst)
				{
					case 0 :  	printf("        ...  Burst Length unlimited\n");	break;
					case 1 :  	printf("        ...  Burst Length 1\n");			break;
					case 2 :  	printf("        ...  Burst Length 2\n");			break;
					case 4 :  	printf("        ...  Burst Length 4\n");			break;
					case 8 :  	printf("        ...  Burst Length 8\n");			break;
					case 16 :  	printf("        ...  Burst Length 16\n");			break;
					case 32 :  	printf("        ...  Burst Length 32\n");			break;
				}
				if(Test_Tx_TR==4)
				{
					printf("        ...  Store & Forward.\n");
				}
				else
				{
					printf("        ...  Transmit Threshold %d\n", Test_Tx_TR);
				}
				if(Test_Auto_Polling_Tx==0)
				{
					printf("        ...  Disable Transmit Auto Polling\n");
				}
				else
				{
					printf("        ...  Transmit Auto Polling %d\n", Test_Auto_Polling_Tx);
				}
				if(Test_Ethernet_Frame)
				{
					printf("        ...  Ethernet Frame\n");
				}
				else
				{
					printf("        ...  IEEE 802.3 Frame\n");
				}
			break;
			case 's' :
			case 'S' :
			{
				int 	save_temp;

				printf("Show All Debug Information\n");

				#ifdef	SMAC_DEBUG
				save_temp=sMacEnableMiiPhyShow;
				sMacEnableMiiPhyShow=1;
				sMacMiiPhyShow(Testing_DevicePt);
				sMacEnableMiiPhyShow=save_temp;

				save_temp=sMacEnableCsrShow;
				sMacEnableCsrShow=1;
				sMacCsrShow(Testing_DevicePt);
				sMacEnableCsrShow=save_temp;

				save_temp=sMacEnableDevShow;
				sMacEnableDevShow=1;
				sMacDevShow(Testing_DevicePt);
				sMacEnableDevShow=save_temp;
				#endif
			}
			break;

		default :
			result=FALSE;
		break;
	}
	return(result);
}


void     CheckRxInfoPrint(int length, int ID)
{
	char	*pDataPool;

	pDataPool=&Test_Rx_DataPool[Testing_DevicePt->rx_PooliRead].DataPool[0];
	printf("Data Pool Addr : 0x%08x length : 0x%08x ID : 0x%08x\n",&pDataPool,length,ID);
}


int		TestCheckRxPattern(const char *DesMacAddr, const char *SourceMacAddr, int length, int ID)
{
	char	*pDataPool;
	int		i;
	int 	pattern;
	char	ipattern=0;

				// Sherlock God
/*
				int clock;

				for(clock=0;clock<RXWAIT_CLOCK;clock++)
				{
					if(TestCheckRxNum(1))	break;
					MSDELAY(100);
					MacTestPolling();
				}
				sMacDevShow(Testing_DevicePt);
*/
	pDataPool=&Test_Rx_DataPool[Testing_DevicePt->rx_PooliRead].DataPool[0];
	if(Test_Rx_DataPool[Testing_DevicePt->rx_PooliRead].Frame_Length != (length+6+6+2))
	{
		CheckRxInfoPrint(length,ID);
		printf("Get Error Length = %x", (int *)Test_Rx_DataPool[Testing_DevicePt->rx_PooliRead].Frame_Length);
		printf("\n");
		printf("MAC_RXLEN_FAIL\n");
		return -1;
	}
	Testing_DevicePt->rx_PooliRead++;
	if((Testing_DevicePt->rx_PooliRead)==(Testing_DevicePt->rx_PooliMax))	Testing_DevicePt->rx_PooliRead=0;

	if (memcmp(DesMacAddr, pDataPool, SMAC_DES_ADDRSIZ))
	{
		CheckRxInfoPrint(length,ID);
		printf("D. ADDR ERROR : 0x%2x Get 0x%2x\n", *DesMacAddr, *pDataPool);
		printf("MAC_RXDADDR_FAIL\n");
		return -1;
	}
	pDataPool += SMAC_DES_ADDRSIZ;

	if (memcmp(SourceMacAddr, pDataPool, SMAC_SOURCE_ADDRSIZ))
	{
		CheckRxInfoPrint(length,ID);
		printf("S. ADDR ERROR : 0x%2x Get 0x%2x\n", *SourceMacAddr, *pDataPool);
		printf("MAC_RXSADDR_FAIL\n");
		return -1;
	}
	pDataPool += SMAC_SOURCE_ADDRSIZ;

	if(Test_Ethernet_Frame)
	{
		pattern=0x8000;		//IP protocol
	}
	else
	{
		if(length>1500)
		{
			pattern=0;		//Special Code for long Frame Test
		}
		else
		{
			if(Test_DisCRC)
			{
				pattern=length-4;
			}
			else
			{
				pattern=length;
			}
		}
	}
	if(*pDataPool != (char)(pattern>>8))
	{
		CheckRxInfoPrint(length,ID);
		printf("L/T pattern : %0x%2xGet Error L/T pattern : 0x%2x\n", pattern, *pDataPool);
		printf("MAC_RXDATA_FAIL\n");
		return -1;
	}
	pDataPool++;

	if(*pDataPool != (char)(pattern))
	{
		CheckRxInfoPrint(length,ID);
		printf("L/T pattern : 0x%2xGet Error L/T pattern : 0x%2x\n", pattern, *pDataPool);
		printf("MAC_RXDATA_FAIL\n");
		return -1;
	}
	pDataPool++;

	pattern=0;
	for(i=0;i<(length-4);i++)
	{
		switch(i%4)
		{
			case 0 : ipattern=ID; break;
			case 1 : ipattern=(pattern>>16); break;
			case 2 : ipattern=(pattern>>8); break;
			case 3 : ipattern=(pattern); pattern++; break;
		}
		if(*pDataPool != ipattern)
		{
			CheckRxInfoPrint(length,ID);
			printf("ipattern : 0x%2xGet Error ipattern : 0x%2x\n", ipattern, *pDataPool);
			printf("MAC_RXDATA_FAIL\n");
			return -1;
		}
		pDataPool++;
	}

	if(*pDataPool != ID)
	{
		CheckRxInfoPrint(length,ID);
		printf("Get Error ID pattern : 0x%2x\n", *pDataPool);
		printf("MAC_RXDATA_FAIL\n");
		return -1;
	}
	pDataPool++;
	if(*pDataPool != 'E')
	{
		CheckRxInfoPrint(length,ID);
		printf("Get Error eND pattern : 0x%2x\n", *pDataPool);
		printf("MAC_RXDATA_FAIL\n");
		return -1;
	}
	pDataPool++;
	if(*pDataPool != 'N')
	{
		CheckRxInfoPrint(length,ID);
		printf("Get Error EnD pattern : 0x%2x\n", *pDataPool);
		printf("MAC_RXDATA_FAIL\n");
		return -1;
	}
	pDataPool++;
	if(*pDataPool != 'D')
	{
		CheckRxInfoPrint(length,ID);
		printf("Get Error ENd pattern : 0x%2x\n", *pDataPool);
		printf("MAC_RXDATA_FAIL\n");
		return -1;
	}

	return (0);
}

void	MacTestDevCrtlInit(void)
{
	ULONG	val1;

	if(Test_SkipDes)
	{
		Testing_DevicePt->DesSkipSize=DES_RING_SKIP*sizeof(ULONG);
		val1=CSR0_DSL_VAL(DES_RING_SKIP);
	}
	else
	{
		val1=0;
		Testing_DevicePt->DesSkipSize=0;
	}

	Testing_DevicePt->AutoPollTx=Test_Auto_Polling_Tx;
	val1=val1|CSR0_TAP_VAL(Testing_DevicePt->AutoPollTx);

	#ifndef	VERIABLE_BYTE_ORDER
	val1 = val1
		| ( DES_BYTE_ORDER << 20)
		| ( CSR0_PBL_VAL(Test_DMA_Burst))
		| ( DATAB_BYTE_ORDER << 7)
		| ( CSR0_BAR_VAL(Test_Bus_Arbitration))
		;
	#else
	val1 = val1
		| ( sMAC_Des_Byte_Order << 20)
		| ( CSR0_PBL_VAL(Test_DMA_Burst))
		| ( sMAC_DataB_Byte_Order << 7)
		| ( CSR0_BAR_VAL(Test_Bus_Arbitration))
		;
	#endif	/* VERIABLE_BYTE_ORDER */

	Testing_DevicePt->BusMode = val1;

	val1=0;
	if(Test_Tx_TR > 0x3)	val1=val1|CSR6_SF;
	else					val1=val1|CSR6_TR_VAL(Test_Tx_TR);
	if(Test_Full_Duplex)	val1=val1|CSR6_FD;

	Testing_DevicePt->OpMode = val1;

	Testing_DevicePt->MII_Loopback_Enable=Test_LoopBack;
	Testing_DevicePt->MII_Force_Enable=Test_ForceEnable;
	if(Test_LoopBack || Test_ForceEnable)
	{
		if(Test_Speed_100M)		Testing_DevicePt->MII_Force_Speed=MII_PHY_CR_100M;
		else					Testing_DevicePt->MII_Force_Speed=MII_PHY_CR_10M;
		if(Test_Full_Duplex)	Testing_DevicePt->MII_Force_Duplex=MII_PHY_CR_FDX;
		else					Testing_DevicePt->MII_Force_Duplex=MII_PHY_CR_HDX;
	}

	Testing_DevicePt->MII_ANA_Setting = MII_PHY_ANA_CSMA;
	if(Test_Speed_100M)		Testing_DevicePt->MII_ANA_Setting |= (MII_PHY_ANA_100M |MII_PHY_ANA_10M);
	else					Testing_DevicePt->MII_ANA_Setting |= (MII_PHY_ANA_10M);
	if(Test_Full_Duplex)	{}
	else					Testing_DevicePt->MII_ANA_Setting &= (~MII_PHY_ANA_FDAM);

	//Inital Descriptor and data buff (include data pattern)
	MacTestDesInit();

}

void	MacTestDesInit(void)
{
	#ifdef	SIMULATION_ONLY
	//First Initail Tx Data Buffer Pattern
	Init_Tx_DataBuff();
	//Clear Rx Buff
	Clear_Rx_DataBuff();
	#endif

	if(Test_ChainDes)		Testing_DevicePt->DesChainMode=1;
	else					Testing_DevicePt->DesChainMode=0;
	if(Test_IgnoreError)	Testing_DevicePt->rx_ignore_error=1;
	else					Testing_DevicePt->rx_ignore_error=0;
	if(Test_Tx_Underflow)	Testing_DevicePt->Tx_Underflow_Test=1;
	else					Testing_DevicePt->Tx_Underflow_Test=0;
	if(Test_SingleBuff1)	Testing_DevicePt->DesSingleBuff1=1;
	else					Testing_DevicePt->DesSingleBuff1=0;
	if(Test_SingleBuff2)	Testing_DevicePt->DesSingleBuff2=1;
	else					Testing_DevicePt->DesSingleBuff2=0;

	Testing_DevicePt->tx_merge_counter=0;
	Testing_DevicePt->tx_merge_number=0;

	//Initail Rx Des
	Init_Rx_Des();

	//Initail Tx Des
	Init_Tx_Des();
}


void	Init_Rx_Des(void)
{
	ULONG	init_des0;
	ULONG	init_des1;
	ULONG	temp_des1;
	int		i;
	int		buff1_index;
	int		buff2_index;
	SMAC_RDE *		desPt;
	SMAC_RDE_SKIP *	desSkipPt;

	desPt = (SMAC_RDE *)( (int)&Rx_Desc[0] );
	desSkipPt = (SMAC_RDE_SKIP *)( (int)&Rx_Desc_Skip[0] );

	init_des0=RDESC0_OWN;	//OWN by MAC for Rx
	if(Test_ChainDes)	init_des1=RDESC1_RCH;
	else				init_des1=0;

	//Buffer size Config 1
	if(Test_SingleBuff1)		temp_des1=init_des1 | (RDESC1_RBS1_VAL(TRX1_DATA_SIZE));
	else if(Test_SingleBuff2)	temp_des1=init_des1 | (RDESC1_RBS2_VAL(TRX1_DATA_SIZE));
	else						temp_des1=init_des1 | (RDESC1_RBS2_VAL(TRX1_DATA_SIZE)) | (RDESC1_RBS1_VAL(TRX1_DATA_SIZE));

	buff1_index=0;
	buff2_index=1;
	for(i=0;i<(TRX1_DATA_SEG/2);i++)
	{
		sMAC_WRITE_DESC(&desPt->rDesc0,init_des0);
		sMAC_WRITE_DESC(&desSkipPt->rDesc0,init_des0);
		sMAC_WRITE_DESC(&desPt->rDesc1,temp_des1);
		sMAC_WRITE_DESC(&desSkipPt->rDesc1,temp_des1);
		sMAC_WRITE_DESC(&desPt->rDesc2,KSEG_TO_PHY(&Rx_DataBuff_1[buff1_index]));
		sMAC_WRITE_DESC(&desSkipPt->rDesc2,KSEG_TO_PHY(&Rx_DataBuff_1[buff1_index]));
		if(Test_ChainDes)
		{
			sMAC_WRITE_DESC(&desPt->rDesc3,KSEG_TO_PHY(&Rx_Desc[i+1]));
			sMAC_WRITE_DESC(&desSkipPt->rDesc3,KSEG_TO_PHY(&Rx_Desc_Skip[i+1]));
		}
		else
		{
			sMAC_WRITE_DESC(&desPt->rDesc3,KSEG_TO_PHY(&Rx_DataBuff_1[buff2_index]));
			sMAC_WRITE_DESC(&desSkipPt->rDesc3,KSEG_TO_PHY(&Rx_DataBuff_1[buff2_index]));
		}
		rBuffSize[i].Buffer1_Size=TRX1_DATA_SIZE;
		rBuffSize[i].Buffer2_Size=TRX1_DATA_SIZE;
		buff1_index=buff1_index+2;
		buff2_index=buff2_index+2;
		desPt++;
		desSkipPt++;
	}

	//Buffer size Config 2
	if(Test_SingleBuff1)		temp_des1=init_des1 | (RDESC1_RBS1_VAL(TRX2_DATA_SIZE));
	else if(Test_SingleBuff2)	temp_des1=init_des1 | (RDESC1_RBS2_VAL(TRX2_DATA_SIZE));
	else						temp_des1=init_des1 | (RDESC1_RBS2_VAL(TRX2_DATA_SIZE)) | (RDESC1_RBS1_VAL(TRX2_DATA_SIZE));

	buff1_index=0;
	buff2_index=1;
	for(i=(TRX1_DATA_SEG/2);i<((TRX1_DATA_SEG+TRX2_DATA_SEG)/2);i++)
	{
		sMAC_WRITE_DESC(&desPt->rDesc0,init_des0);
		sMAC_WRITE_DESC(&desSkipPt->rDesc0,init_des0);
		sMAC_WRITE_DESC(&desPt->rDesc1,temp_des1);
		sMAC_WRITE_DESC(&desSkipPt->rDesc1,temp_des1);
		sMAC_WRITE_DESC(&desPt->rDesc2,KSEG_TO_PHY(&Rx_DataBuff_2[buff1_index]));
		sMAC_WRITE_DESC(&desSkipPt->rDesc2,KSEG_TO_PHY(&Rx_DataBuff_2[buff1_index]));
		if(Test_ChainDes)
		{
			sMAC_WRITE_DESC(&desPt->rDesc3,KSEG_TO_PHY(&Rx_Desc[i+1]));
			sMAC_WRITE_DESC(&desSkipPt->rDesc3,KSEG_TO_PHY(&Rx_Desc_Skip[i+1]));
		}
		else
		{
			sMAC_WRITE_DESC(&desPt->rDesc3,KSEG_TO_PHY(&Rx_DataBuff_2[buff2_index]));
			sMAC_WRITE_DESC(&desSkipPt->rDesc3,KSEG_TO_PHY(&Rx_DataBuff_2[buff2_index]));
		}
		rBuffSize[i].Buffer1_Size=TRX2_DATA_SIZE;
		rBuffSize[i].Buffer2_Size=TRX2_DATA_SIZE;
		buff1_index=buff1_index+2;
		buff2_index=buff2_index+2;
		desPt++;
		desSkipPt++;
	}

	//Buffer size Config 3
	if(Test_SingleBuff1)		temp_des1=init_des1 | (RDESC1_RBS1_VAL(TRX3_DATA_SIZE));
	else if(Test_SingleBuff2)	temp_des1=init_des1 | (RDESC1_RBS2_VAL(TRX3_DATA_SIZE));
	else						temp_des1=init_des1 | (RDESC1_RBS2_VAL(TRX3_DATA_SIZE)) | (RDESC1_RBS1_VAL(TRX3_DATA_SIZE));

	buff1_index=0;
	buff2_index=1;
	for(i=((TRX1_DATA_SEG+TRX2_DATA_SEG)/2);i<((TRX1_DATA_SEG+TRX2_DATA_SEG+TRX3_DATA_SEG)/2);i++)
	{
		sMAC_WRITE_DESC(&desPt->rDesc0,init_des0);
		sMAC_WRITE_DESC(&desSkipPt->rDesc0,init_des0);
		sMAC_WRITE_DESC(&desPt->rDesc1,temp_des1);
		sMAC_WRITE_DESC(&desSkipPt->rDesc1,temp_des1);
		sMAC_WRITE_DESC(&desPt->rDesc2,KSEG_TO_PHY(&Rx_DataBuff_3[buff1_index]));
		sMAC_WRITE_DESC(&desSkipPt->rDesc2,KSEG_TO_PHY(&Rx_DataBuff_3[buff1_index]));
		if(Test_ChainDes)
		{
			sMAC_WRITE_DESC(&desPt->rDesc3,KSEG_TO_PHY(&Rx_Desc[i+1]));
			sMAC_WRITE_DESC(&desSkipPt->rDesc3,KSEG_TO_PHY(&Rx_Desc_Skip[i+1]));
		}
		else
		{
			sMAC_WRITE_DESC(&desPt->rDesc3,KSEG_TO_PHY(&Rx_DataBuff_3[buff2_index]));
			sMAC_WRITE_DESC(&desSkipPt->rDesc3,KSEG_TO_PHY(&Rx_DataBuff_3[buff2_index]));
		}
		rBuffSize[i].Buffer1_Size=TRX3_DATA_SIZE;
		rBuffSize[i].Buffer2_Size=TRX3_DATA_SIZE;
		buff1_index=buff1_index+2;
		buff2_index=buff2_index+2;
		desPt++;
		desSkipPt++;
	}
	//Buffer size Config 4
	if(Test_SingleBuff1)		temp_des1=init_des1 | (RDESC1_RBS1_VAL(TRX4_DATA_SIZE));
	else if(Test_SingleBuff2)	temp_des1=init_des1 | (RDESC1_RBS2_VAL(TRX4_DATA_SIZE));
	else						temp_des1=init_des1 | (RDESC1_RBS2_VAL(TRX4_DATA_SIZE)) | (RDESC1_RBS1_VAL(TRX4_DATA_SIZE));

	buff1_index=0;
	buff2_index=1;
	for(i=((TRX1_DATA_SEG+TRX2_DATA_SEG+TRX3_DATA_SEG)/2);i<((TRX1_DATA_SEG+TRX2_DATA_SEG+TRX3_DATA_SEG+TRX4_DATA_SEG)/2);i++)
	{
		sMAC_WRITE_DESC(&desPt->rDesc0,init_des0);
		sMAC_WRITE_DESC(&desSkipPt->rDesc0,init_des0);
		sMAC_WRITE_DESC(&desPt->rDesc1,temp_des1);
		sMAC_WRITE_DESC(&desSkipPt->rDesc1,temp_des1);
		sMAC_WRITE_DESC(&desPt->rDesc2,KSEG_TO_PHY(&Rx_DataBuff_4[buff1_index]));
		sMAC_WRITE_DESC(&desSkipPt->rDesc2,KSEG_TO_PHY(&Rx_DataBuff_4[buff1_index]));
		if(Test_ChainDes)
		{
			sMAC_WRITE_DESC(&desPt->rDesc3,KSEG_TO_PHY(&Rx_Desc[i+1]));
			sMAC_WRITE_DESC(&desSkipPt->rDesc3,KSEG_TO_PHY(&Rx_Desc_Skip[i+1]));
		}
		else
		{
			sMAC_WRITE_DESC(&desPt->rDesc3,KSEG_TO_PHY(&Rx_DataBuff_4[buff2_index]));
			sMAC_WRITE_DESC(&desSkipPt->rDesc3,KSEG_TO_PHY(&Rx_DataBuff_4[buff2_index]));
		}
		rBuffSize[i].Buffer1_Size=TRX4_DATA_SIZE;
		rBuffSize[i].Buffer2_Size=TRX4_DATA_SIZE;
		buff1_index=buff1_index+2;
		buff2_index=buff2_index+2;
		desPt++;
		desSkipPt++;
	}

	if(Test_ChainDes)
	{
		desPt--;
		desSkipPt--;
		sMAC_WRITE_DESC(&desPt->rDesc3,KSEG_TO_PHY(&Rx_Desc[0]));
		sMAC_WRITE_DESC(&desSkipPt->rDesc3,KSEG_TO_PHY(&Rx_Desc_Skip[0]));

		Testing_DevicePt->rxRing=(SMAC_RDE *)(&Rx_Desc_Skip[0]);
	}
	else
	{
		desPt--;
		desSkipPt--;
		sMAC_WRITE_DESC(&desPt->rDesc1,sMAC_READ_DESC(&desPt->rDesc1)|RDESC1_RER);
		sMAC_WRITE_DESC(&desSkipPt->rDesc1,sMAC_READ_DESC(&desSkipPt->rDesc1)|RDESC1_RER);
		if(Test_SkipDes)
		{
			Testing_DevicePt->rxRing=(SMAC_RDE *)(&Rx_Desc_Skip[0]);
		}
		else
		{
			Testing_DevicePt->rxRing=(SMAC_RDE *)(&Rx_Desc[0]);
		}
	}
	Testing_DevicePt->NumRds=NUM_RDS;
	Testing_DevicePt->rBuffSizePt=&rBuffSize[0];
}

void	Init_Tx_Des(void)
{
	ULONG	init_des0;
	ULONG	init_des1;
	ULONG	temp_des1;
	int		i;
	int		buff1_index;
	int		buff2_index;
	SMAC_TDE *		desPt;
	SMAC_TDE_SKIP *	desSkipPt;

	desPt = (SMAC_TDE *) ( (int)&Tx_Desc[0] );
	desSkipPt = (SMAC_TDE_SKIP *) ( (int)&Tx_Desc_Skip[0] );

	init_des0=0;			//OWN by Host

	/* SW ISSUE Don't use TDESC1_IC on normal opeartion
		This bit will make the Mitigation interrupt no function */
	//init_des1=TDESC1_IC;	//For Tx Interrupt
	init_des1=0;
	if(Test_ChainDes)		init_des1 |= TDESC1_TCH;
	if(Test_DisPadding)		init_des1 |= TDESC1_DPD;
	if(Test_DisCRC)			init_des1 |= TDESC1_AC;

	//Buffer size Config 1
	if(Test_SingleBuff1)		temp_des1=init_des1 | (TDESC1_TBS1_VAL(TRX1_DATA_SIZE));
	else if(Test_SingleBuff2)	temp_des1=init_des1 | (TDESC1_TBS2_VAL(TRX1_DATA_SIZE));
	else						temp_des1=init_des1 | (TDESC1_TBS2_VAL(TRX1_DATA_SIZE))	| (TDESC1_TBS1_VAL(TRX1_DATA_SIZE));

	buff1_index=0;
	buff2_index=1;
	for(i=0;i<(TRX1_DATA_SEG/2);i++)
	{
		sMAC_WRITE_DESC(&desPt->tDesc0,init_des0);
		sMAC_WRITE_DESC(&desSkipPt->tDesc0,init_des0);
		sMAC_WRITE_DESC(&desPt->tDesc1,temp_des1);
		sMAC_WRITE_DESC(&desSkipPt->tDesc1,temp_des1);
		sMAC_WRITE_DESC(&desPt->tDesc2,KSEG_TO_PHY(&Tx_DataBuff_1[buff1_index]));
		sMAC_WRITE_DESC(&desSkipPt->tDesc2,KSEG_TO_PHY(&Tx_DataBuff_1[buff1_index]));
		if(Test_ChainDes)
		{
			sMAC_WRITE_DESC(&desPt->tDesc3,KSEG_TO_PHY(&Tx_Desc[i+1]));
			sMAC_WRITE_DESC(&desSkipPt->tDesc3,KSEG_TO_PHY(&Tx_Desc_Skip[i+1]));
		}
		else
		{
			sMAC_WRITE_DESC(&desPt->tDesc3,KSEG_TO_PHY(&Tx_DataBuff_1[buff2_index]));
			sMAC_WRITE_DESC(&desSkipPt->tDesc3,KSEG_TO_PHY(&Tx_DataBuff_1[buff2_index]));
		}
		tBuffSize[i].Buffer1_Size=TRX1_DATA_SIZE;
		tBuffSize[i].Buffer2_Size=TRX1_DATA_SIZE;
		buff1_index=buff1_index+2;
		buff2_index=buff2_index+2;
		desPt++;
		desSkipPt++;
	}

	//Buffer size Config 2
	if(Test_SingleBuff1)		temp_des1=init_des1 | (TDESC1_TBS1_VAL(TRX2_DATA_SIZE));
	else if(Test_SingleBuff2)	temp_des1=init_des1 | (TDESC1_TBS2_VAL(TRX2_DATA_SIZE));
	else						temp_des1=init_des1 | (TDESC1_TBS2_VAL(TRX2_DATA_SIZE))	| (TDESC1_TBS1_VAL(TRX2_DATA_SIZE));

	buff1_index=0;
	buff2_index=1;
	for(i=(TRX1_DATA_SEG/2);i<((TRX1_DATA_SEG+TRX2_DATA_SEG)/2);i++)
	{
		sMAC_WRITE_DESC(&desPt->tDesc0,init_des0);
		sMAC_WRITE_DESC(&desSkipPt->tDesc0,init_des0);
		sMAC_WRITE_DESC(&desPt->tDesc1,temp_des1);
		sMAC_WRITE_DESC(&desSkipPt->tDesc1,temp_des1);
		sMAC_WRITE_DESC(&desPt->tDesc2,KSEG_TO_PHY(&Tx_DataBuff_2[buff1_index]));
		sMAC_WRITE_DESC(&desSkipPt->tDesc2,KSEG_TO_PHY(&Tx_DataBuff_2[buff1_index]));
		if(Test_ChainDes)
		{
			sMAC_WRITE_DESC(&desPt->tDesc3,KSEG_TO_PHY(&Tx_Desc[i+1]));
			sMAC_WRITE_DESC(&desSkipPt->tDesc3,KSEG_TO_PHY(&Tx_Desc_Skip[i+1]));
		}
		else
		{
			sMAC_WRITE_DESC(&desPt->tDesc3,KSEG_TO_PHY(&Tx_DataBuff_2[buff2_index]));
			sMAC_WRITE_DESC(&desSkipPt->tDesc3,KSEG_TO_PHY(&Tx_DataBuff_2[buff2_index]));
		}
		tBuffSize[i].Buffer1_Size=TRX2_DATA_SIZE;
		tBuffSize[i].Buffer2_Size=TRX2_DATA_SIZE;
		buff1_index=buff1_index+2;
		buff2_index=buff2_index+2;
		desPt++;
		desSkipPt++;
	}

	//Buffer size Config 3
	if(Test_SingleBuff1)		temp_des1=init_des1 | (TDESC1_TBS1_VAL(TRX3_DATA_SIZE));
	else if(Test_SingleBuff2)	temp_des1=init_des1 | (TDESC1_TBS2_VAL(TRX3_DATA_SIZE));
	else						temp_des1=init_des1 | (TDESC1_TBS2_VAL(TRX3_DATA_SIZE))	| (TDESC1_TBS1_VAL(TRX3_DATA_SIZE));

	buff1_index=0;
	buff2_index=1;
	for(i=((TRX1_DATA_SEG+TRX2_DATA_SEG)/2);i<((TRX1_DATA_SEG+TRX2_DATA_SEG+TRX3_DATA_SEG)/2);i++)
	{
		sMAC_WRITE_DESC(&desPt->tDesc0,init_des0);
		sMAC_WRITE_DESC(&desSkipPt->tDesc0,init_des0);
		sMAC_WRITE_DESC(&desPt->tDesc1,temp_des1);
		sMAC_WRITE_DESC(&desSkipPt->tDesc1,temp_des1);
		sMAC_WRITE_DESC(&desPt->tDesc2,KSEG_TO_PHY(&Tx_DataBuff_3[buff1_index]));
		sMAC_WRITE_DESC(&desSkipPt->tDesc2,KSEG_TO_PHY(&Tx_DataBuff_3[buff1_index]));
		if(Test_ChainDes)
		{
			sMAC_WRITE_DESC(&desPt->tDesc3,KSEG_TO_PHY(&Tx_Desc[i+1]));
			sMAC_WRITE_DESC(&desSkipPt->tDesc3,KSEG_TO_PHY(&Tx_Desc_Skip[i+1]));
		}
		else
		{
			sMAC_WRITE_DESC(&desPt->tDesc3,KSEG_TO_PHY(&Tx_DataBuff_3[buff2_index]));
			sMAC_WRITE_DESC(&desSkipPt->tDesc3,KSEG_TO_PHY(&Tx_DataBuff_3[buff2_index]));
		}
		tBuffSize[i].Buffer1_Size=TRX3_DATA_SIZE;
		tBuffSize[i].Buffer2_Size=TRX3_DATA_SIZE;
		buff1_index=buff1_index+2;
		buff2_index=buff2_index+2;
		desPt++;
		desSkipPt++;
	}

	//Buffer size Config 4
	if(Test_SingleBuff1)		temp_des1=init_des1 | (TDESC1_TBS1_VAL(TRX4_DATA_SIZE));
	else if(Test_SingleBuff2)	temp_des1=init_des1 | (TDESC1_TBS2_VAL(TRX4_DATA_SIZE));
	else						temp_des1=init_des1 | (TDESC1_TBS2_VAL(TRX4_DATA_SIZE))	| (TDESC1_TBS1_VAL(TRX4_DATA_SIZE));

	buff1_index=0;
	buff2_index=1;
	for(i=((TRX1_DATA_SEG+TRX2_DATA_SEG+TRX3_DATA_SEG)/2);i<((TRX1_DATA_SEG+TRX2_DATA_SEG+TRX3_DATA_SEG+TRX4_DATA_SEG)/2);i++)
	{
		sMAC_WRITE_DESC(&desPt->tDesc0,init_des0);
		sMAC_WRITE_DESC(&desSkipPt->tDesc0,init_des0);
		sMAC_WRITE_DESC(&desPt->tDesc1,temp_des1);
		sMAC_WRITE_DESC(&desSkipPt->tDesc1,temp_des1);
		sMAC_WRITE_DESC(&desPt->tDesc2,KSEG_TO_PHY(&Tx_DataBuff_4[buff1_index]));
		sMAC_WRITE_DESC(&desSkipPt->tDesc2,KSEG_TO_PHY(&Tx_DataBuff_4[buff1_index]));
		if(Test_ChainDes)
		{
			sMAC_WRITE_DESC(&desPt->tDesc3,KSEG_TO_PHY(&Tx_Desc[i+1]));
			sMAC_WRITE_DESC(&desSkipPt->tDesc3,KSEG_TO_PHY(&Tx_Desc_Skip[i+1]));
		}
		else
		{
			sMAC_WRITE_DESC(&desPt->tDesc3,KSEG_TO_PHY(&Tx_DataBuff_4[buff2_index]));
			sMAC_WRITE_DESC(&desSkipPt->tDesc3,KSEG_TO_PHY(&Tx_DataBuff_4[buff2_index]));
		}
		tBuffSize[i].Buffer1_Size=TRX4_DATA_SIZE;
		tBuffSize[i].Buffer2_Size=TRX4_DATA_SIZE;
		buff1_index=buff1_index+2;
		buff2_index=buff2_index+2;
		desPt++;
		desSkipPt++;
	}

	if(Test_ChainDes)
	{
		desPt--;
		desSkipPt--;
		sMAC_WRITE_DESC(&desPt->tDesc3,KSEG_TO_PHY(&Tx_Desc[0]));
		sMAC_WRITE_DESC(&desSkipPt->tDesc3,KSEG_TO_PHY(&Tx_Desc_Skip[0]));

		Testing_DevicePt->txRing=(SMAC_TDE *)(&Tx_Desc_Skip[0]);
	}
	else
	{
		desPt--;
		desSkipPt--;
		sMAC_WRITE_DESC(&desPt->tDesc1,sMAC_READ_DESC(&desPt->tDesc1)|TDESC1_TER);
		sMAC_WRITE_DESC(&desSkipPt->tDesc1,sMAC_READ_DESC(&desSkipPt->tDesc1)|TDESC1_TER);
		if(Test_SkipDes)
		{
			Testing_DevicePt->txRing=(SMAC_TDE *)(&Tx_Desc_Skip[0]);
		}
		else
		{
			Testing_DevicePt->txRing=(SMAC_TDE *)(&Tx_Desc[0]);
		}
	}
	Testing_DevicePt->NumTds=NUM_TDS;
	Testing_DevicePt->tBuffSizePt=&tBuffSize[0];
}

#ifdef	SIMULATION_ONLY
void	Init_Tx_DataBuff(void)
{
	int	i;
	int	j;

	//Buffer size Config 1
	for(i=0;i<TRX1_DATA_SEG;i++)
	{
		for(j=0;j<TRX1_DATA_SIZE;j=j+2)
		{
			Tx_DataBuff_1[i][j]=1;
			Tx_DataBuff_1[i][j+1]=i;
		}
	}
	//Buffer size Config 2
	for(i=0;i<TRX2_DATA_SEG;i++)
	{
		for(j=0;j<TRX2_DATA_SIZE;j=j+2)
		{
			Tx_DataBuff_2[i][j]=2;
			Tx_DataBuff_2[i][j+1]=i;
		}
	}
	//Buffer size Config 3
	for(i=0;i<TRX3_DATA_SEG;i++)
	{
		for(j=0;j<TRX3_DATA_SIZE;j=j+2)
		{
			Tx_DataBuff_3[i][j]=3;
			Tx_DataBuff_3[i][j+1]=i;
		}
	}
	//Buffer size Config 4
	for(i=0;i<TRX4_DATA_SEG;i++)
	{
		for(j=0;j<TRX4_DATA_SIZE;j=j+2)
		{
			Tx_DataBuff_4[i][j]=4;
			Tx_DataBuff_4[i][j+1]=i;
		}
	}
}

void	Clear_Rx_DataBuff(void)
{
	int	i;
	int	j;

	//Buffer size Config 1
	for(i=0;i<TRX1_DATA_SEG;i++)
	{
		for(j=0;j<TRX1_DATA_SIZE;j=j+2)
		{
			Rx_DataBuff_1[i][j]=1;
			Rx_DataBuff_1[i][j+1]=i;
		}
	}
	//Buffer size Config 2
	for(i=0;i<TRX2_DATA_SEG;i++)
	{
		for(j=0;j<TRX2_DATA_SIZE;j=j+2)
		{
			Rx_DataBuff_2[i][j]=2;
			Rx_DataBuff_2[i][j+1]=i;
		}
	}
	//Buffer size Config 3
	for(i=0;i<TRX3_DATA_SEG;i++)
	{
		for(j=0;j<TRX3_DATA_SIZE;j=j+2)
		{
			Rx_DataBuff_3[i][j]=3;
			Rx_DataBuff_3[i][j+1]=i;
		}
	}
	//Buffer size Config 4
	for(i=0;i<TRX4_DATA_SEG;i++)
	{
		for(j=0;j<TRX4_DATA_SIZE;j=j+2)
		{
			Rx_DataBuff_4[i][j]=4;
			Rx_DataBuff_4[i][j+1]=i;
		}
	}
}
#endif	/* SIMULATION_ONLY */

void	MacTestFilterInit(void)
{
	int		i;
	int 	j;
	char			*to_pt;
	const	char	*from_pt;

	Testing_DevicePt->FilterTableType=Filter_TableType_TABLE[Test_FilterMode];
	Testing_DevicePt->OpMode |= Filter_OPMode_TABLE[Test_FilterMode];

	if(Testing_DevicePt->FilterTableType==TDESC1_HASH)
	{
		to_pt=&Test_FilterAddr[0][0];
		from_pt=&Test_Perfect_MAC_Addr[0][0];
		//Set 1 perfect
		for(j=0;j<FLTR_FRM_ADRS_SIZE;j++)
		{
			*to_pt=*from_pt;
			to_pt++;
			from_pt++;
		}
		from_pt=&Test_Hash_MAC_Addr[0][0];
		//set Mutil
		for(i=1;i<Test_FilterSize;i++)
		{
			for(j=0;j<FLTR_FRM_ADRS_SIZE;j++)
			{
				*to_pt=*from_pt;
				to_pt++;
				from_pt++;
			}
		}
	}
	else if(Testing_DevicePt->FilterTableType==TDESC1_HASH_ONLY)
	{
		to_pt=&Test_FilterAddr[0][0];
		from_pt=&Test_Hash_MAC_Addr[0][0];
		//set Mutil
		for(i=0;i<Test_FilterSize;i++)
		{
			for(j=0;j<FLTR_FRM_ADRS_SIZE;j++)
			{
				*to_pt=*from_pt;
				to_pt++;
				from_pt++;
			}
		}
	}
	else
	//FilterTableType==TDESC1_PERFECT
	//FilterTableType==TDESC1_INVERSE
	{
		to_pt=&Test_FilterAddr[0][0];
		from_pt=&Test_Perfect_MAC_Addr[0][0];
		//set Mutil
		for(i=0;i<Test_FilterSize;i++)
		{
			for(j=0;j<FLTR_FRM_ADRS_SIZE;j++)
			{
				*to_pt=*from_pt;
				to_pt++;
				from_pt++;
			}
		}
	}
}

void	MacTestPrepareTx(const char *DesMacAddr, const char *SourceMacAddr, int length, int ID)
{
	char	*pDataPool;
	int		i;
	int 	pattern;

	pDataPool=Test_Tx_DataPool;
	for(i=0;i<SMAC_DES_ADDRSIZ;i++)
	{
		*pDataPool=*DesMacAddr;
		pDataPool++;
		DesMacAddr++;
	}
	for(i=0;i<SMAC_SOURCE_ADDRSIZ;i++)
	{
		*pDataPool=*SourceMacAddr;
		pDataPool++;
		SourceMacAddr++;
	}

	if(Test_Ethernet_Frame)
	{
		pattern=0x8000;		//IP protocol
	}
	else
	{
		if(length>1500)
		{
			pattern=0;		//Special Code for long Frame Test
		}
		else
		{
			if(Test_DisCRC)
			{
				pattern=length-4;
			}
			else
			{
				pattern=length;
			}
		}
	}
	*pDataPool=(char)(pattern>>8);	//first octet is the most significant octet of this field.
	pDataPool++;
	*pDataPool=(char)(pattern);
	pDataPool++;

	pattern=0;
	for(i=0;i<(length-4);i++)
	{
		switch(i%4)
		{
			case 0 : *pDataPool=ID; pDataPool++; break;
			case 1 : *pDataPool=(pattern>>16); pDataPool++; break;
			case 2 : *pDataPool=(pattern>>8); pDataPool++; break;
			case 3 : *pDataPool=(pattern); pattern++; pDataPool++; break;
		}
	}

	*pDataPool=ID; pDataPool++;
	*pDataPool='E'; pDataPool++;
	*pDataPool='N'; pDataPool++;
	*pDataPool='D';

	sMacPacketSend(Testing_DevicePt,Test_Tx_DataPool,length+SMAC_HEADERSIZ);
}

void	MacTestPolling(void)
{
	//Polling Rx
	if(Test_PollRx_En)
	{
		if(Testing_DevicePt->rxScheduled)
		{
			sMacRxSchedule(Testing_DevicePt);
		}
	}

	//Polling Tx
	if(Test_PollTx_En)
	{
		if(Testing_DevicePt->txScheduled)
		{
			sMacTxSchedule(Testing_DevicePt);
		}
	}
}

int		TestCheckTxFinsih(void)
{
	if(Testing_DevicePt->txIndex == Testing_DevicePt->txDiIndex)
		return(TRUE);
	else
		return(FALSE);
}

int		TestCheckRxNum(int rxnum)
{
int i;

	i = Testing_DevicePt->rx_PooliWrite;
	if( i < (Testing_DevicePt->rx_PooliRead) )
		i = i + Testing_DevicePt->rx_PooliMax;

	if((i-Testing_DevicePt->rx_PooliRead)>=rxnum)
		return(TRUE);
	else
		return(FALSE);
}
void Display_ParaMenu(void)
{
		printf("!. Set 100M (Default)\n");
		printf("@. Set 10M \n");
		printf("#. Set Full Duplex (Default)\n");
		printf("$. Set Half Duplex\n");
		#ifdef	VERIABLE_BYTE_ORDER
		printf("%. Set Descriptor Big Endian\n");
		printf("^. Set Descriptor Little Endian\n");
		printf("&. Set Data Buffer Big Endian\n");
		printf("*. Set Data Buffer Little Endian\n");
		#endif	/* VERIABLE_BYTE_ORDER */
		printf("(. Set Equal Aribitration\n");
		printf("). Set Rx High Aribitration\n");
		printf("+. Increase the Transmit Threshold\n");
		printf("_. Decrease the Transmit Threshold\n");
		printf("}. Increase the Burst Length Setting\n");
		printf("{. Decrease the Burst Length Setting\n");
		printf(">. Increase the Transmit Auto Polling Setting\n");
		printf("<. Decrease the Transmit Auto Polling Setting\n");
		printf(":. Ethernet Frame\n");
		printf("\". IEEE 802.3 Frame\n");

		printf(",. Turn on all the show setting\n");
		printf(".. Turn off all the show setting\n");
		printf("[. Turn on Forever setting\n");
		printf("]. Turn off Forever setting\n");
		/*  leonid+ for translation without phy         */
                printf("~. Translate without PHY test\n");
                printf("`. Translate with PHY test\n");

		printf("\n=. Echo Setting\n");
		printf("S. Show All Debug Information\n");
		printf("--------------------------------------------------\n");
}


void	Display_MacMenu(void)
{

		printf("LDK MAC Testing.\n");
		printf("==================================================\n");

		Display_ParaMenu();

		printf("1. Scan MII PHY Chip\n");
		printf("2. MII PHY Initial\n");
		printf("3. Initial MAC\n");

		printf("4. Normal Loopback Test (rc2)\n");
		printf("5. Long Frame (rc3)\n");
		printf("6. Runt Frame (rc4)\n");
		printf("7. Frame with CRC Error (rc6)\n");
		printf("8. Receiving in suspended (rc8)\n");

		printf("9. Filtering Fail (All (no Filter)) (raf2)\n");
		printf("A. Inverse Perfect Filtering (raf3)\n");
		printf("B. Hash Filtering (raf4)\n");
		printf("C. Hash Only Filtering (raf5)\n");
		printf("D. Receive Bad packet (raf6)\n");
		printf("E. Promiscuous Filtering (raf7)\n");
		printf("F. Pass All Multicast (raf8)\n");

		printf("G. Rx Overflow (rfifo2)\n");

		printf("H. Single Buffer2 (rlsm2/tlsm2)\n");
		printf("I. Chain Mode (rlsm9/tlsm9)\n");
		printf("J. Empty Descriptors for Rx(rlsm10)\n");
		printf("K. Descriptors Unavailable (rlsm12/tlsm12)\n");
		printf("L. Descriptors Skip Ring Mode (rlsm11/tlsm11)\n");
		printf("M. Dual Buffer (rlsm3/tlsm3)\n");

		printf("N. Interrupt Mitigation All NRP & NTP (rim/tim)\n");

		printf("O. Continue Tx 60~1500 Packet\n");
		printf("P. Tx Underflow Test (tfifo9)\n");

//mask by arthur		printf("Q. General Purpose Timer (gpt)\n");
		printf("R. Interrupt Mitigation TT & RT (rim/tim)\n");

		printf("T. Miss Frames & FIFO Test\n");
		printf("U. Rx Overflow Robust Test\n");
		printf("V. Tx/Rx Special size packet Test\n");
//mask by arthur		printf("Y. Tx/Rx Random Packet (Continues 1,048,576 1M)\n");

		printf("Y. Normal external loopback Test \n");

		printf("--------------------------------------------------\n");
		printf("W. Dead Lock Test\n");
		printf("Z. Fully Item AutoTesting\n");
/* mask by leonid
		printf("~. Real Ethernet Data Testing (Non Loopback)\n");
		printf("`. Real Ethernet Auto Data Testing (Non Loopback)\n");
*/
/*mask by arthur
		printf("|. SROM Data Testing\n");
		printf("\\. SROM Auto Data Testing\n");
*/		
		printf("--------------------------------------------------\n");
		printf("?. Display this Menu\n");

		printf("\nX. Exit MAC Part Back to Main Menu\n");
		printf("==================================================\n");
}


const char		RealEther_Auto_Reply_Item[] =
{
	  '@', '#', '.', '=', '0', '2', 'b'
	, '@', '$', '.', '=', '0', '2', 'b'
	, '!', '$', '.', '=', '0', '2', 'b'
	, '!', '#', '.', '=', '0', '2', 'b'
	, '%', '&', '.', '=', '0', '2', 'b'
	, '%', '*', '.', '=', '0', '2', 'b'
	, '^', '&', '.', '=', '0', '2', 'b'
	, '^', '*', '.', '=', '0', '2', 'b'
	, 'x'
};

void	MAC_RealEtherTesting(int auto_reply)
{
	int 	result;
	char 	cmd_buf[128];
	int     clock;
	int     ForceOut;
//	char	buffs[24];

	Test_LoopBack=0;
	Test_Ethernet_Frame=1;

	memcpy(&Testing_DevicePt->LocalMacAddr[0],&Test_Perfect_MAC_Addr[0][0],6);
	memcpy(&Testing_DevicePt->DestinationMacAddr[0],&Test_Perfect_MAC_Addr[1][0],6);

	RealEtherPacketLen=TEST_MIN_MTU;
	RealEtherPacketID=1;
	Forever_Test=0;
	ForceOut=0;

	MacTestDevCrtlInit();

	Display_RealEtherMenu();
	if(auto_reply){
		auto_reply=sizeof(RealEther_Auto_Reply_Item);
	}
//-----------------------------------------------------------------------------
	while(1){
RealEther_break:
		result = 0;
		printf("RealEtherTP_Shell>");
		if(auto_reply)		{
			cmd_buf[0]=RealEther_Auto_Reply_Item[sizeof(RealEther_Auto_Reply_Item)-auto_reply];
			auto_reply--;
		}
		else		{
			scanf("%c", &cmd_buf[0]);
		}

		if( Paser_ParaSetting(cmd_buf[0]) )		{
			goto RealEther_break;
		}
		else{
		switch(cmd_buf[0])
		{
			case '0' :
				printf(" Force MII Setting of Speed and Duplex\n");
				if(Test_Speed_100M){
					printf("        ...  100M\n");
				}
				else{
					printf("        ...  10M\n");
				}
				if(Test_Full_Duplex){
					printf("        ...  Full Duplex\n");
				}
				else{
					printf("        ...  Half Duplex\n");
				}
				printf("Enter any key when above remote setting Done : \n");
				scanf("%c", &cmd_buf[0]);
				Test_ForceEnable = 1;
			break;
			case '1' :
				printf(" MII PHY Initial\n");
				result=sMacMiiInit(Testing_DevicePt);
				sMacMiiPhyShow(Testing_DevicePt);
    			if(result!=TRUE){
				printf("MAC_MII_INITFAIL\n");
    				result=-1;
    			}
    			else{
    				result=0;
    			}
			break;
			case '2' :
				printf(" Initial MAC & Setup\n");

				MacTestDevCrtlInit();

				//Initial MAC
				result=sMacInit(Testing_DevicePt);
				sMacCsrShow(Testing_DevicePt);
				if(result!=TRUE){
					printf("MAC_MAC_INITFAIL\n");
					result=-1;
					break;
				}
    		else{
    				result=0;
    		}

				//Initial Filter
				result=sMacFltrFrmXmit(Testing_DevicePt,(char *)Testing_DevicePt->LocalMacAddr,1);
				if(result!=TRUE){
					printf("MAC_FILTERSET_FAIL\n");
					result=-1;
				}
    		else{
    				result=0;
    		}
    		sMacDevShow(Testing_DevicePt);
			break;
			case '3' :
				printf("Transmit Fixed Pattern Packet ... \n");
				MacTestPrepareTx(Testing_DevicePt->DestinationMacAddr
								,Testing_DevicePt->LocalMacAddr
								,RealEtherPacketLen
								,RealEtherPacketID);
				ShowDataPacket(Test_Tx_DataPool,RealEtherPacketLen);
				sMacDevShow(Testing_DevicePt);

				for(clock=0;clock<RXWAIT_CLOCK;clock++){
					MacTestPolling();
					if(TestCheckTxFinsih())	break;
					MSDELAY(2);
				}
				RealEtherPacketLen++;
				if(RealEtherPacketLen>TEST_MAX_MTU)
					RealEtherPacketLen=TEST_MIN_MTU;
				RealEtherPacketID++;
			break;
			case '4' :
				printf("Transmit Random Pattern Packet ... \n");
				MacTestRandomTx(&RealEtherPacketLen);
				ShowDataPacket(Test_Tx_DataPool,RealEtherPacketLen);
				sMacDevShow(Testing_DevicePt);

				for(clock=0;clock<RXWAIT_CLOCK;clock++){
					MacTestPolling();
					if(TestCheckTxFinsih())	break;
					MSDELAY(2);
				}
			break;

			case '5' :
				printf("Wait until receive Packet (One shot) ... \n");
				while(!(TestCheckRxNum(1))){
					MacTestPolling();
					if(ForceOut)	
						goto RealEther_break;
					// DeadLock until get one
				}
				printf("  Received Packet --- \n");
				RealEtherPacketLen=Test_Rx_DataPool[Testing_DevicePt->rx_PooliRead].Frame_Length-14;
				ShowDataPacket(Test_Rx_DataPool[Testing_DevicePt->rx_PooliRead].DataPool
								,RealEtherPacketLen);
				Testing_DevicePt->rx_PooliRead++;
				if((Testing_DevicePt->rx_PooliRead)==(Testing_DevicePt->rx_PooliMax))	Testing_DevicePt->rx_PooliRead=0;
			break;
			case '6' :
				printf("6. Ether reply received Packet (One shot)\n");
				sMacDevShow(Testing_DevicePt);
				while(!(TestCheckRxNum(1))){
					MacTestPolling();
					if(ForceOut)	
						goto RealEther_break;
					// DeadLock until get one
				}
				printf("  Received Packet --- \n");
				RealEtherPacketLen=Test_Rx_DataPool[Testing_DevicePt->rx_PooliRead].Frame_Length-14;
				ShowDataPacket(Test_Rx_DataPool[Testing_DevicePt->rx_PooliRead].DataPool
								,RealEtherPacketLen);
				if(Enable_Log_HIF){
					printf("Replay Packet ....\n");
				}
				MacTestReplyPacket();
				//Wait Tx Finish
				for(clock=0;clock<RXWAIT_CLOCK;clock++){
					MacTestPolling();
					if(TestCheckTxFinsih())	break;
					MSDELAY(2);
				}
				Testing_DevicePt->rx_PooliRead++;
				if((Testing_DevicePt->rx_PooliRead)==(Testing_DevicePt->rx_PooliMax))	Testing_DevicePt->rx_PooliRead=0;
			break;
//-----------------------------------------------------------------------------
			case 'a' : case 'A' :
			{
				int 	packet_num;

				packet_num=DEFAULT_REMOTE_PACKET;
				RealEtherPacketID=0;

				while(packet_num){
					sMacDevShow(Testing_DevicePt);

					//DeadLock Forever
					while(!(TestCheckRxNum(1))){
						MacTestPolling();
						if(ForceOut)
							goto RealEther_break;
						// DeadLock until get one
					}
					if(Enable_Log_HIF){
						printf("  Received Packet(%d)\n", RealEtherPacketID);

						RealEtherPacketLen=Test_Rx_DataPool[Testing_DevicePt->rx_PooliRead].Frame_Length-14;
						ShowDataPacket(Test_Rx_DataPool[Testing_DevicePt->rx_PooliRead].DataPool
										,RealEtherPacketLen);
					}
					Testing_DevicePt->rx_PooliRead++;
					if((Testing_DevicePt->rx_PooliRead)==(Testing_DevicePt->rx_PooliMax))	Testing_DevicePt->rx_PooliRead=0;
					RealEtherPacketID++;
					if(Forever_Test==0)
						packet_num--;
				}
			}
			break;

			case 'b' : case 'B' :
			{
				int 	packet_num;

				printf("Ether reply received Packet (Forever) ... \n");
				packet_num=DEFAULT_REMOTE_PACKET;
				RealEtherPacketID=0;
				while(packet_num)
				{
					sMacDevShow(Testing_DevicePt);

					while(!(TestCheckRxNum(1)))
					{
						MacTestPolling();
						if(ForceOut)	goto RealEther_break;
						// DeadLock until get one
					}
					RealEtherPacketLen=Test_Rx_DataPool[Testing_DevicePt->rx_PooliRead].Frame_Length-14;
					if(Enable_Log_HIF)
					{
						printf("  Received Packet(%d) Len(%d)\n", RealEtherPacketID, RealEtherPacketLen);
						ShowDataPacket(Test_Rx_DataPool[Testing_DevicePt->rx_PooliRead].DataPool
										,RealEtherPacketLen);
					}
					if(Enable_Log_HIF)
					{
						printf("Replay Packet ....\n");
					}
					MacTestReplyPacket();

					Testing_DevicePt->rx_PooliRead++;
					if((Testing_DevicePt->rx_PooliRead)==(Testing_DevicePt->rx_PooliMax))	Testing_DevicePt->rx_PooliRead=0;
					RealEtherPacketID++;
					if(Forever_Test==0)
						packet_num--;
				}
				//Wait Tx Finish
				for(clock=0;clock<RXWAIT_CLOCK;clock++)
				{
					MacTestPolling();
					if(TestCheckTxFinsih())	break;
					MSDELAY(2);
				}
			}
			break;
			case 'c' : case 'C' :
			{
				int 	packet_num;

				printf("Transmit Fixed Pattern Packet (Forever) ... \n");
				RealEtherPacketLen=1024;
				packet_num=DEFAULT_REMOTE_PACKET;
				RealEtherPacketID=0;
				while(packet_num)
				{
					if(ForceOut)	goto RealEther_break;

					MacTestPrepareTx(Testing_DevicePt->DestinationMacAddr
									,Testing_DevicePt->LocalMacAddr
									,RealEtherPacketLen
									,RealEtherPacketID);
					if(Enable_Log_HIF)
					{
						ShowDataPacket(Test_Tx_DataPool,RealEtherPacketLen);
						sMacDevShow(Testing_DevicePt);
					}
					for(clock=0;clock<RXWAIT_CLOCK;clock++)
					{
						MacTestPolling();
						if(TestCheckTxFinsih())	break;
						MSDELAY(2);
					}
					RealEtherPacketID++;
					if(Forever_Test==0)
						packet_num--;
				}
			}
			break;
			case 'd' :
			case 'D' :
				printf("Overflow Test\n");
			{
				int 	packet_num;

				SMAC_RDE *		desPt;
				int 			i;

				MacTestDevCrtlInit();

				//Reset All Des Owner for 1
				for(i=1;i<NUM_RDS;i++)
				{
					desPt = (SMAC_RDE *)( (int)&Rx_Desc[i] );
					sMAC_RESET_DESC(&desPt->rDesc0,(RDESC0_OWN));
				}

				MacTestFilterInit();
				sMacInit(Testing_DevicePt);
				sMacFltrFrmXmit(Testing_DevicePt,(char *)Test_FilterAddr,Test_FilterSize);

				printf("Wait receive Packet (Forever) ... \n");
				packet_num=DEFAULT_REMOTE_PACKET;
				RealEtherPacketID=0;

				while(packet_num)
				{
					sMacDevShow(Testing_DevicePt);

					//DeadLock Forever
					while(!(TestCheckRxNum(1)))
					{
						//MacTestPolling();
						if(ForceOut)	goto RealEther_break;
						// DeadLock until get one
					}
					Testing_DevicePt->rx_PooliRead++;
					if((Testing_DevicePt->rx_PooliRead)==(Testing_DevicePt->rx_PooliMax))	Testing_DevicePt->rx_PooliRead=0;
					RealEtherPacketID++;
					if(Forever_Test==0)
						packet_num--;
				}
			}
			break;


			case 'n' :
			case 'N' :
				printf("Filter Set to Normal mode\n");
				Test_FilterMode=DEFAULT_FILTER;
				goto RealEther_break;
			break;
			case 'm' :
			case 'M' :
				printf("Filter Set to Receive All Bad Pack\n");
				Test_FilterMode=ALL_BAD;
				goto RealEther_break;
			break;

//-----------------------------------------------------------------------------
			break;
			case '?' :
				printf(" Display MAC Ether Real Menu\n");
				Display_RealEtherMenu();
				goto RealEther_break;
			break;

			case 'x' : //X. Exit MAC Part Back to Main Menu
			case 'X' :
				Test_ForceEnable=0;
				Test_LoopBack=1;
				printf("Bye Bye ..........\n");
				return;
			break;

			default :
				printf("??? Un-Implement Command ???\n");
				printf("Please ReTry .........\n");
				goto RealEther_break;
			break;
		}
		}
		if(result != 0)
		{
//			printf("!!!! Testing Error ... Error Code = %0x%8x", result);
			printf("!!!! Testing Error!!\n");

#if 0
			if(result == MAC_MII_NOTFIND	) printf("MAC_MII_NOTFIND   ");
			if(result == MAC_MII_INITFAIL	) printf("MAC_MII_INITFAIL  ");
			if(result == MAC_MAC_INITFAIL	) printf("MAC_MAC_INITFAIL  ");
			if(result == MAC_FILTERSET_FAIL	) printf("MAC_FILTERSET_FAIL");
			if(result == MAC_RXDADDR_FAIL	) printf("MAC_RXDADDR_FAIL  ");
			if(result == MAC_RXSADDR_FAIL	) printf("MAC_RXSADDR_FAIL  ");
			if(result == MAC_RXLEN_FAIL	) printf("MAC_RXLEN_FAIL	  ");
			if(result == MAC_RXDATA_FAIL	) printf("MAC_RXDATA_FAIL	  ");
			if(result == MAC_RXWAIT_TIMEOUT	) printf("MAC_RXWAIT_TIMEOUT");
			if(result == MAC_TESTITEM_FAIL	) printf("MAC_TESTITEM_FAIL ");
			printf("\n");
#endif
		}
		else{
			printf("Pass !!!\n");
		}

	}
}

void	Display_RealEtherMenu(void)
{
		printf("RealEther Testing.\n");
		printf("==================================================\n");
		printf(" Local Mac Address       : 0x%2x\n", *Testing_DevicePt->LocalMacAddr);
		printf(" Destination Mac Address : 0x%2x\n", *Testing_DevicePt->DestinationMacAddr);
		printf("--------------------------------------------------\n");

		Display_ParaMenu();

		printf("0. Force MII Setting of Speed and Duplex\n");
		printf("1. MII PHY Initial\n");
		printf("2. Initial MAC & Setup\n");
		printf("3. Transmit Fixed Pattern Packet\n");
		printf("4. Transmit Random Pattern Packet\n");
		printf("5. Wait until received Packet (One shot)\n");
		printf("6. Ether reply received Packet (One shot)\n");
		printf("--------------------------------------------------\n");
		printf("A. Wait receive Packet (Forever)\n");
		printf("B. Ether reply received Packet (Forever)\n");
		printf("C. Transmit Fixed Pattern Packet (Forever)\n");
		printf("D. Overflow Test(Forever)\n");

		printf("\nN. Filter Set to Normal mode\n");
		printf("M. Filter Set to Receive All Bad Pack\n");
		printf("--------------------------------------------------\n");
		printf("?. Display this Menu\n");

		printf("\nX. Exit MAC EtherReal Part Back to MAC main Menu\n");
		printf("==================================================\n");
}

unsigned const 	crc_polynomial = 0xedb88320U;
ULONG			crc_seed=0xffffffff;	/* Initial value. */

ULONG Random_Gen(void)
{
	if(crc_seed & 1)
	{
		crc_seed >>= 1;
		crc_seed ^= crc_polynomial;
	}
	else
	{
		crc_seed >>= 1;
	}
	return crc_seed;
}

void MacTestRandomTx(int *PacketLen)
{
	char	*pDataPool;
	int		i;
	int 	len;
	int		pattern;

	pDataPool=Test_Tx_DataPool;
	for(i=0;i<SMAC_DES_ADDRSIZ;i++)
	{
		*pDataPool=Testing_DevicePt->DestinationMacAddr[i];
		pDataPool++;
	}
	for(i=0;i<SMAC_SOURCE_ADDRSIZ;i++)
	{
		*pDataPool=Testing_DevicePt->LocalMacAddr[i];
		pDataPool++;
	}
	len = Random_Gen() & 0x7ff;
	if(len>1500)	len -= 1500;
	if(len<60)		len += 60;

	*PacketLen = len;

	if(Test_Ethernet_Frame)		pattern=0x8000;		//IP protocol
	else						pattern=len;

	*pDataPool=(char)(pattern>>8);	//first octet is the most significant octet of this field.
	pDataPool++;
	*pDataPool=(char)(pattern);
	pDataPool++;

	while(len)
	{
		*pDataPool = Random_Gen(); pDataPool++;
		len--;
	}
	sMacPacketSend(Testing_DevicePt,Test_Tx_DataPool,(*PacketLen)+SMAC_HEADERSIZ);
}

int  sMacEnableDataShow;
void ShowDataPacket(char * Pool, int len)
{
char	buff[4];
char	Txbuff[128+10+1];
char	*Txbuff_pt;
int		i;
unsigned char cdata;

if(sMacEnableDataShow)
{
	printf("Ethernet Packe ---\n");
	printf("    DA = 0x%2x\n", *Pool);
	Pool += 6;
	printf("    SA = 0x%02x\n", *Pool);
	Pool += 6;
	printf("    Type/Len = 0x%04x (%d)\n", *Pool, len);
	Pool += 2;
	for(i=0;i<len;i++)
	{
		if( (i%16) == 0)		{Txbuff_pt=Txbuff;	*Txbuff_pt = 0 ;	strcat( Txbuff_pt, "    ");	}
		cdata=*Pool;
		
		Pool++;
		strcat( Txbuff_pt, buff);

		if( (i%16) == 7)		strcat( Txbuff_pt, "    ");
		else if( (i%16) == 15)	{strcat( Txbuff_pt, "\n");	printf("%s", Txbuff_pt); }	
		else					strcat( Txbuff_pt, " ");
	}
	if( (i%16) != 0)	{strcat( Txbuff_pt, "\n");	printf("%s", Txbuff_pt); }
}
}

void	MacTestReplyPacket(void)
{
	int 	Len;
	char	*pDataPool;

	pDataPool=Test_Tx_DataPool;
	Len=Test_Rx_DataPool[Testing_DevicePt->rx_PooliRead].Frame_Length-14;
	// Swap Des and Souce
	memcpy( pDataPool
		   ,&(Test_Rx_DataPool[Testing_DevicePt->rx_PooliRead].DataPool[SMAC_DES_ADDRSIZ])
		   ,SMAC_DES_ADDRSIZ);
	pDataPool += SMAC_DES_ADDRSIZ;
	memcpy( pDataPool
		   ,&(Test_Rx_DataPool[Testing_DevicePt->rx_PooliRead].DataPool[0])
		   ,SMAC_DES_ADDRSIZ);
	pDataPool += SMAC_DES_ADDRSIZ;

	// include len/type+2
	memcpy( pDataPool
		   ,&(Test_Rx_DataPool[Testing_DevicePt->rx_PooliRead].DataPool[12])
		   ,Len+2);
	sMacPacketSend(Testing_DevicePt,Test_Tx_DataPool,Len+SMAC_HEADERSIZ);
}



const char		MAC_SROM_Auto_Item[] =
{
	'1', '2', '3', '4', 'x'
};


#if 0	//leonid-
void	MAC_SROMTesting(int autotest)
{
	int 	result;
	char 	cmd_buf[128];
	
	Display_SROMMenu();
	
	sMacInit(Testing_DevicePt);
	
	if(autotest)
	{
		autotest=sizeof(MAC_SROM_Auto_Item);
	}
//-----------------------------------------------------------------------------
	while(1)
	{
MACSROM_break:
		result = 0;
		printf("SROMTP_Shell>");
		if(autotest){
			cmd_buf[0]=MAC_SROM_Auto_Item[sizeof(MAC_SROM_Auto_Item)-autotest];
			autotest--;
		}
		else{
			scanf("%c", &cmd_buf[0]);
		}
		switch(cmd_buf[0])
		{
			case '1' :
				printf(" Write Diable\n");
				EEP25_I0_WRDI(Testing_DevicePt->eepdevpt);
				EEP25_I0_RDSR(Testing_DevicePt->eepdevpt, result);
				printf(" SROM Status Register = 0x%8x\n", result);	
				if(result & EEP25_STATUS_WEN){
					printf("MAC_TESTITEM_FAIL\n");
					result = -1;
				}
				else{
					result = 0;
				}
				break;
			case '2' :
				printf(" Write Enable\n");
				EEP25_I0_WREN(Testing_DevicePt->eepdevpt);
				EEP25_I0_RDSR(Testing_DevicePt->eepdevpt, result);
				printf(" SROM Status Register = 0x%8x\n", result);	
				if(result & EEP25_STATUS_WEN){
					result = 0;
				}
				else{
					printf("MAC_TESTITEM_FAIL\n");
					result = -1;
				}
				break;				
			case '3' :
			{
				int BP;
				
				printf(" Write Status Register (BP0, BP1)\n");				
				for(BP=3; BP>=0; BP--){
					// After We write the status register we need to do 
					// Disable then re-enable write
					EEP25_I0_WRDI(Testing_DevicePt->eepdevpt);
					EEP25_I0_WREN(Testing_DevicePt->eepdevpt);	
										
					EEP25_I0_RDSR(Testing_DevicePt->eepdevpt, result);
					printf(" Before Setting Status Register = 0x%8x", result);	
					printf("......");
					EEP25_I0_WRSR(Testing_DevicePt->eepdevpt, (BP<<2));
					EEP25_I0_RDSR(Testing_DevicePt->eepdevpt, result);
					printf(" After Setting SROM Status Register = 0x%8x\n", result);
					result = result & (EEP25_STATUS_BP0|EEP25_STATUS_BP1);
					if(result != (BP<<2)){
						printf("MAC_TESTITEM_FAIL\n");
						result = -1;
						goto SROM_case_3_break;
					}
					else{
						result = 0;
					}						
				}							
SROM_case_3_break:
				// After We write the status register we need to do 
				// Disable then re-enable write
				EEP25_I0_WRDI(Testing_DevicePt->eepdevpt);
				EEP25_I0_WREN(Testing_DevicePt->eepdevpt);
 			}
 				break;				
 			case '4' :
			{
				char	outdatabuf[512];
				char	indatabuf[512];
				int		i;
				int		pattern;				
				
				printf(" Write Data Test\n");	
				for(pattern=0; pattern<5; pattern++){
					switch(pattern)
					{
						case 0 : //00 test
							printf(" ... Pattern 0x00 Test\n");	
							for(i=0;i<512;i++)
								outdatabuf[i]=0;
						break;
						case 1 : //FF test
							printf(" ... Pattern 0xFF Test\n");	
							for(i=0;i<512;i++)
								outdatabuf[i]=0xFF;
						break;
						case 2 : //55 test
							printf(" ... Pattern 0x55 Test\n");	
							for(i=0;i<512;i++)
								outdatabuf[i]=0x55;
						break;
						case 3 : //AA test
							printf(" ... Pattern 0xAA Test\n");	
							for(i=0;i<512;i++)
								outdatabuf[i]=0xAA;
						break;
						case 4 : //Address offset test
							printf(" ... Pattern Address + 0x55 Test\n");	
							for(i=0;i<512;i++)
								outdatabuf[i]=i+0x55;
						break;
					}
					for(i=0;i<512;i++){
						// After We write need to  
						// Disable then re-enable write
						EEP25_I0_WRDI(Testing_DevicePt->eepdevpt);
						EEP25_I0_WREN(Testing_DevicePt->eepdevpt);
						EEP25_I0_WRITE(Testing_DevicePt->eepdevpt,i,outdatabuf[i]);
						EEP25_I0_READ(Testing_DevicePt->eepdevpt,i,indatabuf[i]);
						if(outdatabuf[i]!=indatabuf[i]){
							printf(" !!!!!! Test Pattern Error at Address(0x%8x ), Write(0x%2x ), Read(0x%2x)\n", i, outdatabuf[i], indatabuf[i]);
							printf("MAC_TESTITEM_FAIL\n");
							result = -1;
							goto  SROM_case_4_break;
						}
					}					
				}
 			}
SROM_case_4_break:
 			break;	

			case 'z' :
			case 'Z' :
				printf("!!! Start Fully MAC SROM Item AutoTesting\n");
				autotest=sizeof(MAC_SROM_Auto_Item);
				goto MACSROM_break;
			break;
		
			case '?' :
				printf(" Display MAC SROM Menu\n");
				Display_SROMMenu();
				goto MACSROM_break;
			break;

			case 'x' : //X. Exit MAC Part Back to Main Menu
			case 'X' :
				Test_ForceEnable=0;
				Test_LoopBack=1;
				printf("Bye Bye ..........\n");
				return;
			break;

			default :
				printf("??? Un-Implement Command ???\n");
				printf("Please ReTry .........\n");
				goto MACSROM_break;
			break;
		}
		if(result != 0){
#if defined(SEMI_HOST_FUNCTION)||defined(SEMI_HOST_FUNCTION_ARM)
			printf("!!!! Testing Error ... Error Code = 0x%8x\n", result);
#else
			test_fail();
#endif
		}
		else{
			printf("Pass !!!\n");
		}
	}
}
#endif


void	Display_SROMMenu(void)
{
		printf("Serial EEPROM Testing.\n");
		printf("==================================================\n");
		printf("1. Write Disable\n");
		printf("2. Write Enable\n");
		printf("3. Write Status Register (BP0, BP1)\n");
		printf("4. Write Data Test\n");
		
		printf("\nZ. Fully Item AutoTesting\n");
		printf("--------------------------------------------------\n");
		printf("?. Display this Menu\n");

		printf("\nX. Exit MAC SROM Part Back to MAC main Menu\n");
		printf("==================================================\n");
}

