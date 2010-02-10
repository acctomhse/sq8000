#ifndef  _usbctrl_h_
#define  _usbctrl_h_

#include "usbspec.h"
#include "udcreg.h"

#define END_POINTER_NUM		4
//#define END_POINTER_NUM		16

#define WAIT_TX_BUF_AV(reg)		do{ }while( readw( reg ) & TxFULL   )
#define WAIT_DMA_DONE(reg)		do{ }while( readw( reg ) & ENP_DMASTATUS )

//#define virt_to_phy(va)			((va) & 0x7fffffff)
#define virt_to_phy(va)			((va) & 0x1fffffff)

#define ASSERT_SOFT_POR()		do { writew( (readw(UDC_DEVCTL) | SOFT_POR),     UDC_DEVCTL ); } while(0)
#define DEASSERT_SOFT_POR()		do { writew( (readw(UDC_DEVCTL) & ~SOFT_POR),    UDC_DEVCTL ); } while(0)
#define SET_PHY_16_BIT()		do { writew( (readw(UDC_DEVCTL) | DEV_PHY16BIT), UDC_DEVCTL ); } while(0)

#ifdef SUPPORT_REMOTE_WAKEUP
  #define SOFT_CONNECT()		do { writew( (readw(UDC_DEVCTL)|DEV_SELF_PWR|DEV_SOFT_CN|CSR_DONE|DEV_RMTWKP), UDC_DEVCTL ); } while(0)
#else
  #define SOFT_CONNECT()		do { writew( (readw(UDC_DEVCTL)|DEV_SELF_PWR|DEV_SOFT_CN|CSR_DONE), UDC_DEVCTL );} while(0)
#endif // SUPPORT_REMOTE_WAKEUP

#ifdef SUPPORT_REMOTE_WAKEUP
  #define SOFT_DISCONNECT()		do { writew( (readw(UDC_DEVCTL)|DEV_RMTWKP|DEV_SELF_PWR), UDC_DEVCTL ); } while(0)
#else
  #define SOFT_DISCONNECT()		do { writew( (readw(UDC_DEVCTL)|DEV_SELF_PWR), UDC_DEVCTL ); } while(0)
#endif // SUPPORT_REMOTE_WAKEUP

#ifdef SUPPORT_DEBUG_OUT
  #define DEBUG_OUT(x)			do { *debug++ = (x); } while(0);
#else
  #define DEBUG_OUT(x)
#endif // SUPPORT_DEBUG_OUT

#define IS_FULL_SPEED()			( (readw( UDC_DEVINFO ) & ENUM_SPEED_MASK)==ENUM_FULL_SPEED )
#define IS_HIGH_SPEED()			( (readw( UDC_DEVINFO ) & ENUM_SPEED_MASK)==ENUM_HIGH_SPEED )
#define ASSIGNED_ADDRESS()		( readw( UDC_DEVINFO ) & DEV_ADDR )
#define VBUS_OK()				(( readw( UDC_DEVINFO ) & VBUS_STS ) ? true : false)

//cyli++ 01/17/07
#define IS_16_ENDPT()			(ENDPT_NUM_16  == (readw(UDC_DEVINFO) & ENDPT_NUM_MASK))
#define IS_10_ENDPT()			(ENDPT_NUM_10  == (readw(UDC_DEVINFO) & ENDPT_NUM_MASK))
#define IS_4_ENDPT()			(ENDPT_NUM_4  == (readw(UDC_DEVINFO) & ENDPT_NUM_MASK))

#define YES						1
#define NO						0
#define SETUP_STAGE				11
#define DATA_STAGE				22
#define STATUS_STAGE			33

#define BK_BUF_SIZE				0x10000
#define BULK_BLOCK_SIZE			0x200
#define MAX_BK_NUM				(BK_BUF_SIZE/BULK_BLOCK_SIZE)

#define INTR_BUF_SIZE			0x10000
#define INTR_BLOCK_SIZE			0x20
#define MAX_INTR_NUM			(INTR_BUF_SIZE/INTR_BLOCK_SIZE)

#define CTRL_BUF_SIZE			0x10000
#define CTRL_BLOCK_SIZE			0x200
#define MAX_CTRL_NUM			(CTRL_BUF_SIZE/CTRL_BLOCK_SIZE)

#define VENDOR_TEST_OUT0		(0x1)
#define VENDOR_TEST_IN0			(0x2)
#define VENDOR_TEST_OUT1		(0x3)
#define VENDOR_TEST_IN2			(0x4)

#ifdef __cplusplus
extern "C" {
#endif

int		USBTesting();
void	initUDCController();
void	initUDCTestingEnv();
void	UDCIintHandler(void* pparam);

u32_t	fn_get_descriptor();
u32_t	fn_get_device_id();
void	process_reset();
void	process_setup_req();
void	process_in0();
void	process_out0();
void	process_intr_req(int ch);	// Interrupt IN request hanlder
void	process_bulk_out(int ch);	// Bulk Out request handler
void	process_bulk_in(int ch);	// Bulk In request handler
void	process_suspend();
void	process_resume();
void	process_vbus_intr();
u32_t	fn_vendor_test_out0();
u32_t	fn_vendor_test_in0();

void	setup_resume_timer();
void	usb_time_up_for_resume( void* pparam );
void	setup_1ms_timer(int);
void	usb_time_up_for_1ms( void* pparam );

#ifdef __cplusplus
}
#endif

#endif // _usbctrl_h_
