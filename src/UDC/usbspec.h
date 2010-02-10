#ifndef _usb_spec_h_
#define _usb_spec_h_

/* spec defined standard command value --- only GET_DESCRIPTOR used here */
#define GET_DESCRIPTOR					6
/* printer class command */
#define GET_DEVICE_ID					0
#define GET_PORT_STATUS					1
#define SOFT_RESET						2
/* spec defined const */

#define FULL_SPEED_CTRL_PACKET_SIZE	8
#define FULL_SPEED_BULK_PACKET_SIZE	64
#define HI_SPEED_CTRL_PACKET_SIZE	64
#define HI_SPEED_BULK_PACKET_SIZE	512
#define MAX_STR_DES_SIZE			128

#include "type.h"

typedef struct _std_setup_format
{
	u8_t	bmRequestType;
	u8_t	bRequest;
	u16_t	wValue;
	u16_t	wIndex;
	u16_t	wLength;
}setup_request_t;

typedef struct _std_dev_dscr
{
	u8_t	bLength;
	u8_t	bDescriptorType;
	u16_t	bcdUSB;
	u8_t	bDeviceClass;
	u8_t	bDeviceSubClass;
	u8_t	bDeviceProtocal;
	u8_t	bMaxPacketSize;
	u16_t	idVendor;
	u16_t	idProduct;
	u16_t	bcdDevice;
	u8_t	iManufacturer;
	u8_t	iProduct;
	u8_t	iSerialNumber;
	u8_t	bNumConfigurations;
}std_dev_dscr, *pstd_dev_dscr;

typedef struct _std_dev_qual_dscr
{
	u8_t	bLength;
	u8_t	bDescriptorType;
	u16_t	bcdUSB;
	u8_t	bDeviceClass;
	u8_t	bDeviceSubClass;
	u8_t	bDeviceProtocal;
	u8_t	bMaxPacketSize;
	u8_t	bNumConfigurations;
	u8_t	bReserved;
}std_dev_qual_dscr, *pstd_dev_qual_dscr;

typedef struct _std_cfg_dscr
{
	u8_t	bLength;
	u8_t	bDescriptorType;
	u16_t	wTotalLength;
	u8_t	bNumInterfaces;
	u8_t	bConfigurationValue;
	u8_t	iConfiguration;
	u8_t	bmAttributes;
	u8_t	MaxPower;
}std_cfg_dscr, *pstd_cfg_dscr;

typedef struct _std_if_dscr
{
	u8_t	bLength;
	u8_t	bDescriptorType;
	u8_t	bInterfaceNumber;
	u8_t	bAlternateSetting;
	u8_t	bNumEndpoints;
	u8_t	bInterfaceClass;
	u8_t	bInterfaceSubClass;
	u8_t	bInterfaceProtocol;
	u8_t	iInterface;
}std_if_dscr, *pstd_if_dscr;

typedef struct _std_edp_dscr
{
	u8_t	bLength;
	u8_t	bDescriptorType;
	u8_t	bEndpointAddress;
	u8_t	bmAttributes;
	u16_t	wMaxPacketSize;
	u8_t	bInterval;
}std_edp_dscr, *pstd_edp_dscr;

typedef struct _std_str_dscr_h
{
	u8_t	bLength;
	u8_t	bDescriptorType;
	u8_t	bString[1];

}std_str_dscr_h, *pstd_str_dscr_h;

/* descriptor type */			
#define DEV_DSCR_TYPE			1
#define CFG_DSCR_TYPE			2
#define STR_DSCR_TYPE			3
#define INTF_DSCR_TYPE 			4
#define EDP_DSCR_TYPE			5
#define QUAL_DSCR_TYPE			6
#define OTHERSPD_DSCR_TYPE		7

/* bmRequestType Mask */
#define REQ_MASK_DIR			0x80
#define REQ_MASK_TYPE			0x60
#define REQ_MASK_RECP			0x1f

#define REQ_DIR_HOST_TO_DEV	    (0x0 << 7)
#define REQ_DIR_DEV_TO_HOST	    (0x1 << 7)
#define GET_REQ_DIR(x)			((x) & REQ_MASK_DIR)

#define REQ_TYPE_STANDARD		(0x0 << 5)
#define REQ_TYPE_CLASS			(0x1 << 5)
#define REQ_TYPE_VENDOR			(0x2 << 5)
#define GET_REQ_TYPE(x)			((x) & REQ_MASK_TYPE)

#define REQ_RECP_DEVICE			(0x0)
#define REQ_RECP_INTERFACE		(0x1)
#define REQ_RECP_ENDPOINT		(0x2)
#define GET_REQ_RECP(x)			((x) & REQ_MASK_RECP)

#endif /* _usb_spec_h_ */
