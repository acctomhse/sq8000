// unmark USE_USBPRINT_SYS can enable the WinXP to enumerate
// this device as a printer class device, thus use "usbprint.sys"
// as the windows driver, else we use "bulkusb.sys" as the driver

// #define SUPPORT_REMOTE_WAKEUP
// #define USE_USBPRINT_SYS
//#define SUPPORT_DEBUG_OUT
//#define USB11

#include "platform.h"
#include "genlib.h"
#include "usbctrl.h"
#include "GPIO/gpioctrl.h"
#include "Timer/timerctrl.h"
#include "Timer/regs-timer.h"

typedef struct ChannelSetStructure
{
	UCHAR	*pBulkOutBuffer;			// Bulk_OUT Buffer
	UCHAR	*pBulkInBuffer;				// Bulk_IN Buffer
	UCHAR	*pIntrInBuffer;				// Intr_IN Buffer
	UINT	uCurrentBulkOutBlockNum;	// number of Bulk_OUT
	UINT	uCurrentBulkInBlockNum;		// number of Bulk_IN
	UINT	uCurrentIntrInBlockNum;		// number of Intr_IN
	short	sBulkBlockSizeBuffer[1024];	// each Bulk block size record buffer

	UINT	uLoopbackCount;			// count of need to loopback
	UINT	uIntrINCount;			// count of need to transfer with Intr_IN

	UINT	uBulk_OUT;				// Bulk_OUT register base address
	UINT	uBulk_IN;				// Bulk_IN register base address
	UINT	uIntr_IN;				// Intr_IN register base address

	UINT	uStatus;				// status for Bulk_IN
	bool	bBulkDMAOnGoing;		// Bulk_IN DMA status flag
	bool	bIntrDMAOnGoing;		// Intr_IN DMA status flag
	bool	bBufferAvailable;		// buffer available flag
	bool	bTransmitError;			// transmit error flag
} ChannelSetType;

//#define TEST_SEG	0x80080000
//#define TEST_SEG	0x800a0000
#define TEST_SEG	0xa0200000

//#if	(END_POINTER_NUM == 4)
//
//#define END_TEST_SEG		(TEST_SEG + CTRL_BUF_SIZE + BK_BUF_SIZE + INTR_BUF_SIZE)
//#define	NUM_OF_USB_CHANNEL	1
//
//#elif (END_POINTER_NUM == 16)

#define END_TEST_SEG		(TEST_SEG + CTRL_BUF_SIZE + (BK_BUF_SIZE + INTR_BUF_SIZE) * 5)
#define	NUM_OF_USB_CHANNEL	5

//#else
//#error "END_POINTER_NUM not well defined!"
//#endif	/* END_POINTER_NUM */

char	*CtrlInBuffer  = (char *)  TEST_SEG;	// control in buffer

ChannelSetType	ChannelSet[NUM_OF_USB_CHANNEL];			// Bulk OUT, IN & Intr IN buffer pointer
UINT	uBulkBlockSize  = BULK_BLOCK_SIZE;
UINT	uMaxBlockNumber = (BK_BUF_SIZE / BULK_BLOCK_SIZE);

int		*debug		= (INT *)END_TEST_SEG;

setup_request_t		SetupRequestData;

bool	bIsConnected;	// stable status of connect/disconnect
bool	bIsFullSpeed;	// bus connected speed status, true -> Full Speed (USB 1.1), false -> High Speed (USB 2.0)
bool	bIsSuspend;		// bus suspend status, true -> in suspend state

UINT	uCurrentBulklPacketSize;	// current bulk packet size, 2.0 is 512, 1.1 is 64

UINT	uCurrentCtrlPacketSize;		// current control in packet size
UINT	uControlDataSize;			// control data need to trasmit to host
bool	bRequestError;				// get descriptor error flag

UINT	uCurrentCtrlBlockNum;		// current index nmuber for Control Endpoint buffer
UINT	uNeedDoProcCtrlBlockNum;	// there have control request, need to response
UINT	uControlStage;				// stage for Control Endpoint

int 	ResetFlag;
int	isr_waiting_time;
int	isr_time_value;

#include "usbdescp.h"

#define         DEVICE_ID       "DOT4\\Vid_03f0&Pid_0011&Rev_0100&SCAN"

// called on first h/w init or reset
void initUDCController()
{
#ifdef	USB11
 	// set UDC is Full-Speed mode, default is High-Speed mode
 	writew( readw(UDC_DEVCTL)|DEV_FULL_SPD, UDC_DEVCTL );
#endif

	// init control endpoint
	writew( EN_SETUP_INTR | EN_IN0_INTR | EN_OUT0_INTR | EN_BOUTALL_INTR |
			EN_BINALL_INTR | EN_IINALL_INTR | EN_USBRST_INTR | EN_RSUME_INTR |
			EN_SUSP_INTR, UDC_ENINT );

	// INT 0,1,2 enable, level-trigger(default), high-active
//	writew( INT0EN | INT1EN | INT2EN | INT0HIGH_ACT | INT1HIGH_ACT | INT2HIGH_ACT , UDC_INTCON );
	writew( UDC_INTEN | UDC_INTHIGH_ACT, UDC_INTCON);

	writew( TxACKINTEN | TxNAK  , UDC_TX0CON );
	writew( RxACKINTEN | RxNAK |  RxEPEN, UDC_RX0CON );

	// UDC Channel Set 0
	ChannelSet[0].uBulk_OUT = UDC_RX1STAT;			// Bulk_OUT control register
	ChannelSet[0].uBulk_IN  = UDC_TX2STAT;			// Bulk_IN control register
	ChannelSet[0].uIntr_IN  = UDC_TX3STAT;			// Intr_IN control register
//#if END_POINTER_NUM == 16
	// UDC Channel Set 1
	ChannelSet[1].uBulk_OUT = UDC_RX4STAT;			// Bulk_OUT control register
	ChannelSet[1].uBulk_IN  = UDC_TX5STAT;			// Bulk_IN control register
	ChannelSet[1].uIntr_IN  = UDC_TX6STAT;			// Intr_IN control register
	// UDC Channel Set 2
	ChannelSet[2].uBulk_OUT = UDC_RX7STAT;			// Bulk_OUT control register
	ChannelSet[2].uBulk_IN  = UDC_TX8STAT;			// Bulk_IN control register
	ChannelSet[2].uIntr_IN  = UDC_TX9STAT;			// Intr_IN control register
	// UDC Channel Set 3
	ChannelSet[3].uBulk_OUT = UDC_RX10STAT;			// Bulk_OUT control register
	ChannelSet[3].uBulk_IN  = UDC_TX11STAT;			// Bulk_IN control register
	ChannelSet[3].uIntr_IN  = UDC_TX12STAT;			// Intr_IN control register
	// UDC Channel Set 4
	ChannelSet[4].uBulk_OUT = UDC_RX13STAT;			// Bulk_OUT control register
	ChannelSet[4].uBulk_IN  = UDC_TX14STAT;			// Bulk_IN control register
	ChannelSet[4].uIntr_IN  = UDC_TX15STAT;			// Intr_IN control register
//#endif	/* END_POINTER_NUM */

	writew( (0x1 << 8) | RxACKINTEN  | RxEPEN, UDC_RXCON(ChannelSet[0].uBulk_OUT));
	writew( (0x2 << 8) | TxDMADN_EN  | TxEPEN, UDC_TXCON(ChannelSet[0].uBulk_IN));
	writew( (0x3 << 8) | TxACKINTEN  | TxVOIDINTEN  | TxERRINTEN  | TxEPEN,  UDC_TXCON(ChannelSet[0].uIntr_IN));
                       
//#if END_POINTER_NUM == 16
	writew( (0x4 << 8) | RxACKINTEN  | RxEPEN, UDC_RXCON(ChannelSet[1].uBulk_OUT));
	writew( (0x5 << 8) | TxDMADN_EN  | TxEPEN, UDC_TXCON(ChannelSet[1].uBulk_IN));
	writew( (0x6 << 8) | TxACKINTEN  | TxVOIDINTEN  | TxERRINTEN  | TxEPEN,  UDC_TXCON(ChannelSet[1].uIntr_IN));
                       
	writew( (0x7 << 8) | RxACKINTEN  | RxEPEN, UDC_RXCON(ChannelSet[2].uBulk_OUT));
	writew( (0x8 << 8) | TxDMADN_EN  | TxEPEN, UDC_TXCON(ChannelSet[2].uBulk_IN));
	writew( (0x9 << 8) | TxACKINTEN  | TxVOIDINTEN  | TxERRINTEN  | TxEPEN,  UDC_TXCON(ChannelSet[2].uIntr_IN));
                       
	writew( (0xa << 8) | RxACKINTEN  | RxEPEN, UDC_RXCON(ChannelSet[3].uBulk_OUT));
	writew( (0xb << 8) | TxDMADN_EN  | TxEPEN, UDC_TXCON(ChannelSet[3].uBulk_IN));
	writew( (0xc << 8) | TxACKINTEN  | TxVOIDINTEN  | TxERRINTEN  | TxEPEN,  UDC_TXCON(ChannelSet[3].uIntr_IN));
                       
	writew( (0xd << 8) | RxACKINTEN  | RxEPEN, UDC_RXCON(ChannelSet[4].uBulk_OUT));
	writew( (0xe << 8) | TxDMADN_EN  | TxEPEN, UDC_TXCON(ChannelSet[4].uBulk_IN));
	writew( (0xf << 8) | TxACKINTEN  | TxVOIDINTEN  | TxERRINTEN  | TxEPEN,  UDC_TXCON(ChannelSet[4].uIntr_IN));
//#endif	/* END_POINTER_NUM */
    
    //writew( readw(UDC_DEVCTL)|CSR_DONE, UDC_DEVCTL );
}

void initUDCTestingEnv()
{
	UINT	i, j, *q;
	UINT	bufAddr;

	uControlDataSize = uCurrentCtrlBlockNum = 0;
	// after VBus connect interrupt occurred, initUDCController() will be called, thus we are not bIsSuspend status
	bIsSuspend = FALSE;

	// clear all buffer's content
	for (q = (UINT *)TEST_SEG; q < (UINT *)END_TEST_SEG; *q++ = 0);

	// initialize Bulk_OUT, Bulk_IN & Intr_IN buffer pointer
	for (i = 0, bufAddr = TEST_SEG; i < NUM_OF_USB_CHANNEL; i++) {
		ChannelSet[i].pBulkOutBuffer = (UCHAR *)bufAddr;
		ChannelSet[i].pBulkInBuffer  = (UCHAR *)bufAddr;
		ChannelSet[i].pIntrInBuffer  = (UCHAR *)bufAddr + BK_BUF_SIZE;
		bufAddr += (BK_BUF_SIZE + INTR_BUF_SIZE);
		ChannelSet[i].uCurrentBulkInBlockNum = ChannelSet[i].uCurrentBulkOutBlockNum = ChannelSet[i].uCurrentIntrInBlockNum = 0;

		// initialize Intr_IN buffer with testing pattern
		for (q = (UINT *)ChannelSet[i].pIntrInBuffer, j = 0; j < MAX_INTR_NUM; j++, q += (INTR_BLOCK_SIZE / 4))
			*q = j;

		ChannelSet[i].bBulkDMAOnGoing = false;
		ChannelSet[i].bIntrDMAOnGoing = false;
	}

	CtrlInBuffer  = (CHAR *)  bufAddr;	// control in buffer

}

int USBTesting()
{
	UINT	ch;		// channel number
	UINT	TX_Status;
	CHAR	*dma_src_addr;
	char	Continue_test = 1;
	
//cyli++ 01/17/07
	if (IS_16_ENDPT()) {
		printf("16 end-points\n");
	} else if (IS_10_ENDPT()) {
		printf("10 end-points\n");
	} else if (IS_4_ENDPT()) {
		printf("4 end-points\n");
	} else {
		printf("Error end-point number!\n");
		printf("DEV_INFO = 0x%08x", readw(UDC_DEVINFO) & ENDPT_NUM_MASK);
	}

// CY+ to fix program reload Socle phy hang up problem
#ifdef	UDC_SOCLE_PHY
		ASSERT_SOFT_POR();
#if 0
		isr_time_value   = 0;
		isr_waiting_time = 1;
		setup_1ms_timer(1);
#else
		MSDELAY(1);
#endif

		while (isr_time_value != isr_waiting_time);

		DEASSERT_SOFT_POR();
#endif

#ifdef	UDC_SOCLE_PHY
#if 0
		isr_time_value   = 0;
		isr_waiting_time = 8;
		setup_1ms_timer(8);
#else
		MSDELAY(8);
#endif

		while (isr_time_value != isr_waiting_time);

//		SET_PHY_16_BIT();
#endif

	ResetFlag = 0;
	bIsConnected = VBUS_OK();
//	if (bIsConnected)
//		printf(" -> USB VBus connect\n");
//	else
//		printf(" -> USB VBus disconnect");

	// enable interrupt
	INT0_ENABLE(LDK_INTC_UDC);

	// setup interrupt handler
	connectInterrupt( LDK_INTC_UDC, UDCIintHandler, NULL);

	INT0_SET_MASK(LDK_INTC_UDC);

	while (1) {
//		;printf("\n USB cable disconnected!\n");

		SOFT_DISCONNECT();

		// wait for plugin stable on vbus
		printf("\nWait for USB Host connect...");
//		while ( !bIsConnected );
//CY++ for checking USB Host connect waiting
		int wait=0;
		while ( !bIsConnected ){
          		MSDELAY(60);
			printf("%4d", wait);			
          		wait++;
          		if(wait > 100)
             			return USB_NO_CONNECT_ERROR;
			printf("\b\b\b\b");
        	}

#ifdef	UDC_SOCLE_PHY
		ASSERT_SOFT_POR();
#if 0
		isr_time_value   = 0;
		isr_waiting_time = 1;
		setup_1ms_timer(1);
#else
		MSDELAY(1);
#endif

		while (isr_time_value != isr_waiting_time);

		DEASSERT_SOFT_POR();
#endif

#ifdef	UDC_SOCLE_PHY
#if 0
		isr_time_value   = 0;
		isr_waiting_time = 8;
		setup_1ms_timer(8);
#else
		MSDELAY(8);
#endif

		while (isr_time_value != isr_waiting_time);

//		SET_PHY_16_BIT();
#endif
		initUDCController();	// initialize UDC Controller
		initUDCTestingEnv();	// initialize UDC testing environment
		SOFT_CONNECT();
		;//printf("\n USB cable connected!\n");
		uNeedDoProcCtrlBlockNum = 0;

		for (ch = 0; ch < NUM_OF_USB_CHANNEL; ch++) {
			ChannelSet[ch].uLoopbackCount = 0;
			ChannelSet[ch].uIntrINCount   = 0;

			// start receive from Bulk_OUT pipe
			writew(virt_to_phy( (u32_t)ChannelSet[ch].pBulkOutBuffer), UDC_DMALM_OADDR(ChannelSet[ch].uBulk_OUT));
			writew( ENP_DMA_START, UDC_DMACTRLO(ChannelSet[ch].uBulk_OUT));
		}


		while ( bIsConnected ) {
			// check ENP 0 transmit request
			if ( uNeedDoProcCtrlBlockNum > 0 ) {
				int	dma_size;
				
				// dma done now, polling for empty data set
				if (!( readw(UDC_TX0BUF) & TxFULL)) {

					// critical section
					DisableInterrupt();
					uNeedDoProcCtrlBlockNum--;
					dma_src_addr = CtrlInBuffer + uCurrentCtrlBlockNum++ * uCurrentCtrlPacketSize;

					if ( uControlDataSize > uCurrentCtrlPacketSize ) {
						dma_size = uCurrentCtrlPacketSize;
					}
					else {
						dma_size = uControlDataSize;
					}

					// calculate remaining bytes need transfer
					uControlDataSize -= dma_size;

					writew( dma_size, UDC_TX0STAT );
					writew( virt_to_phy((u32_t)dma_src_addr) , UDC_DMA0LM_IADDR );
					writew( ENP_DMA_START , UDC_DMA0CTLI );
					EnableInterrupt();
					
					// set ACK--after we have overwritten the previously incorrect data
					writew( readw( UDC_TX0CON ) & ~TxNAK, UDC_TX0CON );
				
					
				}
			}
		    if (Continue_test){
				printf("Done\n");
		            	printf("USB initialize...Done\n");
		            	printf("HandShaking...Done\n");
				printf("Waiting for Bulk Read/Write test...\n");
				printf("Remove Socle USB Bulk device Hardware from Windows and USB cable to Exit !!!\n");
				Continue_test = 0;
	    		}
			// check channel group transmit request
			for (ch = 0; ch < NUM_OF_USB_CHANNEL; ch++) {

				// check each Bulk_IN End Point transmit request
				if ( ChannelSet[ch].uLoopbackCount > 0 ) {
					if (ChannelSet[ch].bBulkDMAOnGoing == false) {

						// check buffer is available for transmit data
						TX_Status = readw(UDC_TXBUF(ChannelSet[ch].uBulk_IN));	// get buffer status

						if ((TX_Status & TxFULL) == 0) {
#if 0
							if ((TX_Status & TxDS0) == 0)
								DEBUG_OUT('0');
							if ((TX_Status & TxDS1) == 0)
								DEBUG_OUT('1');
#endif

							//DEBUG_OUT('i');

							// enter critical section, protect share variable
							DisableInterrupt();

							//DEBUG_OUT('+');

							ChannelSet[ch].uLoopbackCount--;
							// get transmit buffer address & move buffer to next position
							dma_src_addr = ChannelSet[ch].pBulkInBuffer + ChannelSet[ch].uCurrentBulkInBlockNum * uBulkBlockSize;

							// setup DMA
							writew( ChannelSet[ch].sBulkBlockSizeBuffer[ChannelSet[ch].uCurrentBulkInBlockNum],
									UDC_TXSTAT(ChannelSet[ch].uBulk_IN) );	// write transmit count
							writew( virt_to_phy((u32_t)dma_src_addr),	// set transmit buffer pointer
									UDC_DMALM_IADDR(ChannelSet[ch].uBulk_IN) );
							writew( ENP_DMA_START , UDC_DMACTRLI(ChannelSet[ch].uBulk_IN) );			// start transmit DMA

							// move to next buffer
							ChannelSet[ch].uCurrentBulkInBlockNum = ++ChannelSet[ch].uCurrentBulkInBlockNum % uMaxBlockNumber;

#if 0
							WAIT_DMA_DONE(UDC_TXSTAT(ChannelSet[ch].uBulk_IN));
#endif
#if 0
							for (delay = 0; delay < 1000; delay++) {
								TX2_DMAOnGoing = true;
							}
#endif

							ChannelSet[ch].bBulkDMAOnGoing = true;

							//DEBUG_OUT('-');

							EnableInterrupt();

							// leave critical section
						}
					}
				}

				// check each Intr_IN End Point transmit request
				if ( ChannelSet[ch].uIntrINCount > 0 ) {
//					if (!( readw(UDC_TXBUF(ChannelSet[ch].uIntr_IN)) & TxFULL)) {
					if (ChannelSet[ch].bIntrDMAOnGoing == false) {

						// critical section
						DisableInterrupt();
						ChannelSet[ch].uCurrentIntrInBlockNum--;
						dma_src_addr = ChannelSet[ch].pIntrInBuffer + ChannelSet[ch].uCurrentIntrInBlockNum * INTR_BLOCK_SIZE;
						EnableInterrupt();

						writew( INTR_BLOCK_SIZE, UDC_TXSTAT(ChannelSet[ch].uIntr_IN) );
						writew( virt_to_phy((u32_t)dma_src_addr), UDC_DMALM_IADDR(ChannelSet[ch].uIntr_IN) );
						writew( ENP_DMA_START, UDC_DMACTRLI(ChannelSet[ch].uIntr_IN) );

						ChannelSet[ch].bIntrDMAOnGoing = true;
#if 0
						WAIT_DMA_DONE(UDC_DMACTRLI(ChannelSet[ch].uIntr_IN) );
#endif
						ChannelSet[ch].uCurrentBulkInBlockNum = ++ChannelSet[ch].uCurrentBulkInBlockNum % MAX_INTR_NUM;
					}
				}
			}
		}
 		// CY+ for disconnect detect and exit testing program	
		printf ("USB Host disconnected !!!\n");
		printf ("Press Y to continue and any other Key to exit? ");
		char	i;	
		i=getchar();
		if((i!='Y')&&(i!='y'))
		{
			printf ("\n");		
			return UPF_TEST_SUCCESS;
		}
		else
			Continue_test = 1;
	}

	// leave usb test now, should never been here
	return USB_UNKONW_ERROR;
}

void process_reset()
{
	// wait until usbReset signal down

	u32_t int2flgReg;

	;//printf(" USB Bus reset!\n");

	do {
		int2flgReg = readw( UDC_INTFLG );
	} while( int2flgReg & USBRST_INTR );

	initUDCController();

	ResetFlag = 1;	
}

void process_setup_req()
{
	u32_t* p;

	u32_t request_type;

	UINT	ch;		// channel number
	int i;
//	DEBUG_OUT('s');

	bRequestError   = NO;
	uCurrentCtrlBlockNum    = 0;
	uNeedDoProcCtrlBlockNum = 0;

	uControlStage = SETUP_STAGE;



	if (ResetFlag == 1)
	{
		if ( IS_HIGH_SPEED() ) {
			;//printf(" USB connect with HIGH Speed!\n");
			bIsFullSpeed = 0;
			uBulkBlockSize  = HI_SPEED_BULK_PACKET_SIZE;
			uCurrentCtrlPacketSize  = HI_SPEED_CTRL_PACKET_SIZE;
			uCurrentBulklPacketSize = HI_SPEED_BULK_PACKET_SIZE;
		} else {
			;//printf(" USB connect with FULL Speed!\n");
			bIsFullSpeed = 1;
			uBulkBlockSize  = FULL_SPEED_BULK_PACKET_SIZE;
			uCurrentCtrlPacketSize  = FULL_SPEED_CTRL_PACKET_SIZE;
			uCurrentBulklPacketSize = FULL_SPEED_BULK_PACKET_SIZE;
		}
		uMaxBlockNumber = (BK_BUF_SIZE / uBulkBlockSize);
	
		// change device MaxPacketSize of descriptor
		scusbDscr[7] = uCurrentCtrlPacketSize;		
		ResetFlag = 0;
		for (ch = 0; ch < NUM_OF_USB_CHANNEL; ch++) {
			ChannelSet[ch].uLoopbackCount = 0;
			ChannelSet[ch].uIntrINCount   = 0;

			// start receive from Bulk_OUT pipe
			writew(virt_to_phy( (u32_t)ChannelSet[ch].pBulkOutBuffer), UDC_DMALM_OADDR(ChannelSet[ch].uBulk_OUT));
			writew( ENP_DMA_START, UDC_DMACTRLO(ChannelSet[ch].uBulk_OUT));
		}
		for (i = 0; i < NUM_OF_USB_CHANNEL; i++)
			ChannelSet[i].uCurrentBulkInBlockNum = ChannelSet[i].uCurrentBulkOutBlockNum = ChannelSet[i].uCurrentIntrInBlockNum = 0;		

	}
	// clear previous buffer
	/*
	writew( readw( UDC_TX0CON ) | TxCLR,  UDC_TX0CON );
	writew( readw( UDC_TX0CON ) & ~TxCLR, UDC_TX0CON );
	writew( readw( UDC_RX0CON ) | RxCLR,  UDC_RX0CON );
	writew( readw( UDC_RX0CON ) & ~RxCLR, UDC_RX0CON );
*/
	// read setup data

	p = (u32_t*)&SetupRequestData;
	*p++ = *(volatile u32_t *)UDC_SETUP1;
	*p   = *(volatile u32_t *)UDC_SETUP2;

	// identify standard request
	request_type = GET_REQ_TYPE(SetupRequestData.bmRequestType);
	if (request_type == REQ_TYPE_STANDARD) {
		if (SetupRequestData.bRequest == GET_DESCRIPTOR ) {
			fn_get_descriptor();
		}
	} else if (request_type == REQ_TYPE_CLASS ) {
		if (SetupRequestData.bRequest == GET_DEVICE_ID) {
			;//printf("process_setup_req() - get device id!\n");
			fn_get_device_id();
		}
	} else if (request_type == REQ_TYPE_VENDOR ) {
		if (SetupRequestData.bRequest == VENDOR_TEST_OUT0 ) {
			;//printf("process_setup_req() - vendor test out!\n");
			fn_vendor_test_out0();
		} else if ( SetupRequestData.bRequest == VENDOR_TEST_IN0 ) {
			;//printf("process_setup_req() - vendor test in!\n");
			fn_vendor_test_in0();
		}
	}

	uControlStage = DATA_STAGE ;
}

u32_t fn_vendor_test_out0()
{

	// ctrl-out via control pipe
	uControlDataSize = ___swab16 ( SetupRequestData.wLength );
	writew( virt_to_phy( (u32_t)CtrlInBuffer ), UDC_DMA0LM_OADDR );
	writew( ENP_DMA_START ,  UDC_DMA0CTLO );

	// deassert RX0NAK --- let host data rush into DMA buffer
	writew( readw( UDC_RX0CON ) & ~RxNAK, UDC_RX0CON
 );

	return 0;
}

u32_t fn_vendor_test_in0()
{

	// ctrl-in via control pipe
	uControlDataSize = ___swab16 ( SetupRequestData.wLength );
	uNeedDoProcCtrlBlockNum = 1;

	return 0;
}

u32_t fn_get_device_id()
{
	u32_t  total_len = sizeof(DEVICE_ID);

	memcpy( CtrlInBuffer, DEVICE_ID, total_len);
	uNeedDoProcCtrlBlockNum = 1;

	return (uControlDataSize = total_len);
}

u32_t fn_get_descriptor()
{
	u32_t	 total_len, req_len;
	u16_t	 type;
	u16_t	 index;

#if END_POINTER_NUM == 4
	u8_t	*str_ptr       = scusbDscr + 18 + 10 + (9 + 9 + 7*3) * 2;
	u8_t	*fuspd_cfg_ptr = scusbDscr + 18 + 10 + 9 + 9 + 7*3;
#elif END_POINTER_NUM == 16
	u8_t	*str_ptr       = scusbDscr + 18 + 10 + (9 + 9 + 7*15) * 2;
	u8_t	*fuspd_cfg_ptr = scusbDscr + 18 + 10 + 9 + 9 + 7*15;
#else
#error "****** END_POINTER_NUM is not well defined!"
#endif

	u8_t	*hispd_cfg_ptr = scusbDscr + 18 + 10;
	u8_t	*edp_ptr;

#ifdef BIG_ENDIAN
	type  = ( SetupRequestData.wValue & 0x00ff );
	index = ( SetupRequestData.wValue & 0xff00 )>>8;
#else
	type  = ( SetupRequestData.wValue & 0xff00 )>>8;
	index = ( SetupRequestData.wValue & 0x00ff );
#endif
	total_len = 0;

	switch ( type ) {
	case DEV_DSCR_TYPE:
		;//printf("Device descriptor!\n");
		total_len = 18;
		memcpy( CtrlInBuffer, scusbDscr, total_len);
		break;

	case CFG_DSCR_TYPE:
#if END_POINTER_NUM == 4
		total_len = 9 + 9 + 7*3;
#elif END_POINTER_NUM == 16
		total_len = 9 + 9 + 7*15;
#else
#error "****** END_POINTER_NUM is not well defined!"
#endif
		if ( bIsFullSpeed ) {
			;//printf("Full speed config descriptor!\n");
			memcpy( CtrlInBuffer, fuspd_cfg_ptr, total_len);
		} else {
			;//printf("High speed config descriptor!\n");
			memcpy( CtrlInBuffer, hispd_cfg_ptr, total_len);
		}
		break;

	case STR_DSCR_TYPE:
		switch( index ) {
		case 0:
			;//printf("String decscripotr 0!\n");
			total_len = 4;
			memcpy( CtrlInBuffer, str_ptr, total_len );
			break;
		case 1:
			;//printf("String decscripotr 1!\n");
			total_len = 20;
			memcpy( CtrlInBuffer, str_ptr + 4, total_len );
			break;
		case 2:
			;//printf("String decscripotr 2!\n");
			total_len = 22;
			memcpy( CtrlInBuffer, str_ptr + 4 + 20, total_len );
			break;
		default:
			;//printf("String decscripotr 3!\n");
			bRequestError = YES;
			break;
		}
		break;

	case INTF_DSCR_TYPE:
		total_len = 9;
		if( bIsFullSpeed ) {
			;//printf("Full speed interface descriptor!\n");
			memcpy( CtrlInBuffer, fuspd_cfg_ptr + 9, total_len );
		} else {
			;//printf("High speed interface descriptor!\n");
			memcpy( CtrlInBuffer, hispd_cfg_ptr + 9, total_len );
		}
		break;

	case EDP_DSCR_TYPE:
		;//printf("EndPoiint descriptor!\n");
		total_len = 7;

		if ( bIsFullSpeed )
			edp_ptr = fuspd_cfg_ptr + 9 + 9;
		else
			edp_ptr = hispd_cfg_ptr + 9 + 9;

		memcpy( CtrlInBuffer, edp_ptr + (index * 7), total_len );
		break;

	case QUAL_DSCR_TYPE:
		;//printf("Qual descriptor!\n");
		total_len = 10;
		memcpy( CtrlInBuffer, scusbDscr + 18, total_len );
		break;

	case OTHERSPD_DSCR_TYPE:
		;//printf("Other speed descriptor!\n");
#if END_POINTER_NUM == 4
		total_len = 9 + 9 + 7*3;
#elif END_POINTER_NUM == 16
		total_len = 9 + 9 + 7*15;
#else
#error "****** END_POINTER_NUM is not well defined!"
#endif
		if ( bIsFullSpeed )
			memcpy( CtrlInBuffer, hispd_cfg_ptr, total_len );
		else
			memcpy( CtrlInBuffer, fuspd_cfg_ptr, total_len );

		CtrlInBuffer[1] = OTHERSPD_DSCR_TYPE;
		break;

	default:
		;//printf("Get decsriptor : request error!\n");
		bRequestError = YES;
		break;
	}

	if ( bRequestError )
		// write stall
		writew( readw( UDC_TX0CON ) | TxSTALL , UDC_TX0CON );
	else {
		// start ctrl-in transfer
#ifdef BIG_ENDIAN
		req_len = ___swab16 ( SetupRequestData.wLength );
#else
		req_len =  SetupRequestData.wLength;
#endif
		if ( req_len < total_len )
			total_len = req_len;
		uNeedDoProcCtrlBlockNum = 1;
	}

	return (uControlDataSize = total_len);
}

// process Intr_IN interrupt
void process_intr_req(int ch)
{
	u32_t status = readw( UDC_TXSTAT(ChannelSet[ch].uIntr_IN) );

	// transmit error
	if ( status & TxERR ) {
		ChannelSet[ch].bIntrDMAOnGoing = false;

		status = readw( UDC_TXBUF(ChannelSet[ch].uIntr_IN) );

		if ( status & TxURF ) {
			// uncheck TxNAK, TxSTALL
			writew( readw( UDC_TXCON(ChannelSet[ch].uIntr_IN) ) & ~( TxSTALL | TxNAK ),
			        UDC_TXCON(ChannelSet[ch].uIntr_IN) );
		}

		return;
	}

	// transmit complete
	if ( status & TxACK ) {
		ChannelSet[ch].uIntrINCount++;
		ChannelSet[ch].bIntrDMAOnGoing = false;

		return;
	}

	return;
}

// tx0 handler
void process_in0()
{
    readw( UDC_TX0STAT ); //added by Ivan Chiang 040805
	if ( uControlStage == DATA_STAGE ) {
		// data stage

        //;//printf("tx0 handler uControlDataSize = %x\n",uControlDataSize); 
		if ( uControlDataSize > 0 )
			uNeedDoProcCtrlBlockNum++;
		else {
			// open OUT gate
			writew( readw( UDC_RX0CON ) & ~RxNAK, UDC_RX0CON );
			writew( virt_to_phy((u32_t)CtrlInBuffer), UDC_DMA0LM_OADDR );
			writew( ENP_DMA_START ,  UDC_DMA0CTLO );
			uControlStage = STATUS_STAGE;
		}

	}
	else if ( uControlStage == STATUS_STAGE ) {
		// status stage, close gates

		writew( readw( UDC_TX0CON ) | TxNAK, UDC_TX0CON );
		writew( readw( UDC_RX0CON ) | RxNAK, UDC_RX0CON );
	}

	//DEBUG_OUT(uControlDataSize);
	return;
}

// rx0 handler
void process_out0()
{
	int len;

	if ( uControlStage == STATUS_STAGE ) {
		// status stage

		// read clear RX0ACK interrupt
		readw( UDC_RX0STAT );

		// assert both TX0NAK, RX0NAK at status stage
		writew( readw( UDC_TX0CON ) | TxNAK, UDC_TX0CON );
		writew( readw( UDC_RX0CON ) | RxNAK, UDC_RX0CON );

	} else if ( uControlStage == DATA_STAGE ) {
		// data stage

		// read length from H/W
		len = readw( UDC_RX0STAT ) & RxCNT;

		// record received result
		uControlDataSize -= len;
		//DEBUG_OUT(uControlDataSize);

		// step next dma block
		uCurrentCtrlBlockNum = ++uCurrentCtrlBlockNum % MAX_CTRL_NUM;
		writew( virt_to_phy((u32_t)( CtrlInBuffer + uCurrentCtrlBlockNum * uCurrentCtrlPacketSize)), UDC_DMA0LM_OADDR );
		writew( ENP_DMA_START ,  UDC_DMA0CTLO );
/*
		if ( uControlDataSize <= 0 ) {
			// open IN gate
			writew( readw( UDC_TX0CON ) & ~TX0NAK, UDC_TX0CON );
			writew( 0, UDC_TX0STAT );
			writew( virt_to_phy((u32_t)CtrlInBuffer) , UDC_DMA0LM_IADDR );
			writew( DMA0INSTA , UDC_DMA0CTLI );
	   		uControlStage = STATUS_STAGE;
		}
*/

	}

	return;
}

void process_bulk_out(int ch)
{
	// bulk out the nth block
	UINT rxvalue = readw( UDC_RXSTAT(ChannelSet[ch].uBulk_OUT) );

	DEBUG_OUT('R');
    
#if 0
	if ( (rxvalue & RxVOID  ) != 0 )
	if ( (rxvalue & RxERR   ) != 0 )
	if ( (rxvalue & RxURF   ) != 0 )
#endif

	if ( (rxvalue & RxACK   ) != 0 ) {
		// get Bulk Out byte count
		ChannelSet[ch].sBulkBlockSizeBuffer[ChannelSet[ch].uCurrentBulkOutBlockNum] = rxvalue & RxCNT;

		// step next DMA block
		ChannelSet[ch].uCurrentBulkOutBlockNum = ++ChannelSet[ch].uCurrentBulkOutBlockNum % uMaxBlockNumber;
		writew( virt_to_phy((u32_t)(ChannelSet[ch].pBulkOutBuffer + ChannelSet[ch].uCurrentBulkOutBlockNum * uBulkBlockSize)),
				UDC_DMALM_OADDR(ChannelSet[ch].uBulk_OUT) );
		writew( ENP_DMA_START, UDC_DMACTRLO(ChannelSet[ch].uBulk_OUT) );
//		printf("0x%08x addr=0x%08x\n", virt_to_phy((u32_t)(ChannelSet[ch].pBulkOutBuffer + ChannelSet[ch].uCurrentBulkOutBlockNum * uBulkBlockSize)), UDC_DMALM_OADDR(ChannelSet[ch].uBulk_OUT));
		// loop back to Bulk Out transfer
		ChannelSet[ch].uLoopbackCount++;
	}
}

void process_bulk_in(int ch)
{
	// remove interrupt cause
	UINT	TX_Status = readw( UDC_TXSTAT(ChannelSet[ch].uBulk_IN) );

	// get interrupt status
#if 0
	if ( (TX_Status & (TxVOID)  ) != 0 )
	if ( (TX_Status & (TxURF)   ) != 0 )
#endif

#if	1
	// is one packet transcation complete
	if ( (TX_Status & (TxDMADN)) != 0 )	{
		DEBUG_OUT('D');

		ChannelSet[ch].bBulkDMAOnGoing = false;

		return;
	}
#endif

#if	0	// is USB bus get ACK
	if ( (TX_Status & (TxACK)   ) != 0 ) {
//		DEBUG_OUT('a');

		return;
	}
#endif

	// is transmit error
	if ( (TX_Status & (TxERR)   ) != 0 )	{
//		DEBUG_OUT('e');

		ChannelSet[ch].bBulkDMAOnGoing = false;
		ChannelSet[ch].bTransmitError  = true;
		ChannelSet[ch].uLoopbackCount++;

		// clear TX2 FIFO
		writew( (readw(UDC_TXCON(ChannelSet[ch].uBulk_IN)) |  TxCLR), UDC_TXCON(ChannelSet[ch].uBulk_IN) );
		writew( (readw(UDC_TXCON(ChannelSet[ch].uBulk_IN)) & ~TxCLR), UDC_TXCON(ChannelSet[ch].uBulk_IN) );

		return;
	}

//	ChannelSet[ch].bBulkDMAOnGoing = false;
//	TX_Status = readw(ChannelSet[ch].Bulk_IN.uStatReg);
	// get buffer status
//	TX_BufferAvailable = (TX_Status & TXFULL)? false: true;

#if 0
//	TX_Status = readw(ChannelSet[ch].Bulk_IN.uStatReg);
	// get buffer status
	if ((TX_Status & TXDS0) == 0)
//		DEBUG_OUT('0');
	if ((TX_Status & TX2S1) == 0)
//		DEBUG_OUT('1');
#endif

#if 0
	if (((TX_Status & TXDS0) == 0) || ((TX_Status & TXDS1) == 0)) {
//		DEBUG_OUT('E');
		TX_BufferAvailable = true;
	} else {
//	DEBUG_OUT('F');
	TX_BufferAvailable = false;
	}
#endif
}

void UDCIintHandler(void* pparam)
{
	u32_t int2flgReg = readw( UDC_INTFLG );

	// USB Bus Status Interrupt
	if ( int2flgReg & VBUS_INTR ) {			// VBUS interrupt
		process_vbus_intr();
	}

	if ( int2flgReg & USBRST_INTR ) {		// USB Bus reset
		process_reset();
	}

	if ( int2flgReg & SETUP_INTR ) {		// Setup request inetrrupt
		process_setup_req();
	}

	if ( int2flgReg & SUSP_INTR ) {			// Bus suspend interrupt
		process_suspend();
	}

	if ( int2flgReg & RSUME_INTR ) {		// Bus resume interrupt
		process_resume();
	}

	// Control Endpoint Interrupt
	if ( int2flgReg & IN0_INTR ) {
		process_in0();
	}
	if ( int2flgReg & OUT0_INTR ) {
		process_out0();
	}

	// Bulk_OUT Interrupt
	if ( int2flgReg & ENP1_INTR ) {
		process_bulk_out(0);
	}
//#if END_POINTER_NUM == 16
	if ( int2flgReg & ENP4_INTR ) {
		process_bulk_out(1);
	}
	if ( int2flgReg & ENP7_INTR ) {
		process_bulk_out(2);
	}
	if ( int2flgReg & ENP10_INTR ) {
		process_bulk_out(3);
	}
	if ( int2flgReg & ENP13_INTR ) {
		process_bulk_out(4);
	}
//#endif	/* END_POINTER_NUM */

	// Bulk_IN Interrupt
	if ( int2flgReg & ENP2_INTR ) {
		process_bulk_in(0);
	}
//#if END_POINTER_NUM == 16
	if ( int2flgReg & ENP5_INTR ) {
		process_bulk_in(1);
	}
	if ( int2flgReg & ENP8_INTR ) {
		process_bulk_in(2);
	}
	if ( int2flgReg & ENP11_INTR ) {
		process_bulk_in(3);
	}
	if ( int2flgReg & ENP14_INTR ) {
		process_bulk_in(4);
	}
//#endif	/* END_POINTER_NUM */

	// Intr_IN Interrupt
	if ( int2flgReg & ENP3_INTR ) {
		process_intr_req(0);
	}
//#if END_POINTER_NUM == 16
	if ( int2flgReg & ENP6_INTR ) {
		process_intr_req(1);
	}
	if ( int2flgReg & ENP9_INTR ) {
		process_intr_req(2);
	}
	if ( int2flgReg & ENP12_INTR ) {
		process_intr_req(3);
	}
	if ( int2flgReg & ENP15_INTR ) {
		process_intr_req(4);
	}
//#endif	/* END_POINTER_NUM */
}

void process_vbus_intr()
{
	;//printf("\n USB VBus Interrupt");

	bIsConnected = VBUS_OK();

	if (bIsConnected)
		;//printf(" -> USB VBus connect\n");
	else
		;//printf(" -> USB VBus disconnect");
}

void process_suspend()
{
	;//printf(" USB Bus suspend!\n");

	bIsSuspend = YES;

	if (*(debug-1) != 'u') {
//		DEBUG_OUT('u');
	}
}

void process_resume()
{
	;//printf(" USB Bus resume!\n");

	bIsSuspend = NO;

	if (*(debug-1) != 'z')
//		DEBUG_OUT('z');

        // clear all buffer
	writew( readw( UDC_TX0CON ) | TxCLR,  UDC_TX0CON );
	writew( readw( UDC_TX0CON ) & ~TxCLR, UDC_TX0CON );
	writew( readw( UDC_RX0CON ) | RxCLR,  UDC_RX0CON );
	writew( readw( UDC_RX0CON ) & ~RxCLR, UDC_RX0CON );
}

void setup_resume_timer()
{
	TMR0_CLR_INT();

//	writew(0x11, GPIO_B);

	// disable timer 0
	TMR0_DIS();
	// set reload reg, PCLK ~= 5MHz, reload reg = 60000, prescaler = 256 ====> ~ 3 sec.
	writew(60000, TMR0LR);
	// set interrupt handler
	connectInterrupt(LDK_INTC_TMR0_0, usb_time_up_for_resume, 0);

	// set control flag
	TMR0_PERIOD_MODE();
	TMR0_PRESCALE(PRESCALE_256);

	// enable TIMER 0 interrupt source
	INT0_ENABLE(LDK_INTC_TMR0_0);
	INT0_SET_MASK(LDK_INTC_TMR0_0);

	// enable TIMER 0
	TMR0_EN();
}

void usb_time_up_for_resume( void* pparam )
{
//	DEBUG_OUT('T');
//	DEBUG_OUT(readw( UDC_DEVCTL ));

//	writew(0xff, GPIO_B);

	// write a long resume signal
	writew( readw( UDC_DEVCTL ) | DEV_RESUME,  UDC_DEVCTL );
	writew( readw( UDC_DEVCTL ) | DEV_RESUME,  UDC_DEVCTL );
	writew( readw( UDC_DEVCTL ) | DEV_RESUME,  UDC_DEVCTL );
	writew( readw( UDC_DEVCTL ) | DEV_RESUME,  UDC_DEVCTL );
	writew( readw( UDC_DEVCTL ) | DEV_RESUME,  UDC_DEVCTL );
	writew( readw( UDC_DEVCTL ) | DEV_RESUME,  UDC_DEVCTL );
 	writew( readw( UDC_DEVCTL ) & ~DEV_RESUME, UDC_DEVCTL );

	TMR0_DIS();
}

void setup_1ms_timer(int tmr_times)
{
    // disable timer 0
	TMR1_DIS();
	// set reload reg, PCLK ~= 5MHz, reload reg = 20, prescaler = 256 ====> ~ 1ms.
//	writew(20  * tmr_times, TMR1LR);	// for APB clock = 5  MHz

	writew(264 * tmr_times, TMR1LR);	// for APB clock = 66 MHz
//	writew(512 * tmr_times, TMR1LR);	// for APB clock = 66 MHz

	// set interrupt handler
	connectInterrupt(LDK_INTC_TMR0_1, usb_time_up_for_1ms, 0);

	// set control flag
	TMR1_PERIOD_MODE();
	TMR1_PRESCALE(PRESCALE_256);

	// enable TIMER 1 interrupt source
	INT0_ENABLE(LDK_INTC_TMR0_1);
	INT0_SET_MASK(LDK_INTC_TMR0_1);

	// enable TIMER 1
	TMR1_EN();
}

void usb_time_up_for_1ms( void* pparam )
{
	TMR1_CLR_INT();
	isr_time_value++;

	if (isr_time_value == isr_waiting_time) {
		TMR1_DIS();
	}
}
