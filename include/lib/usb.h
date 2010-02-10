#ifndef  _usb_h_
#define  _usb_h_


/////////////////////////////////////////////////////////////////////////////
//USB Data Structure Definitions
/////////////////////////////////////////////////////////////////////////////

/*
 * Device and/or Interface Class codes
 */
#define USB_CLASS_PER_INTERFACE		0	/* for DeviceClass */
#define USB_CLASS_AUDIO				1
#define USB_CLASS_COMM					2
#define USB_CLASS_HID					3
#define USB_CLASS_PHYSICAL				5
#define USB_CLASS_STILL_IMAGE			6
#define USB_CLASS_PRINTER				7
#define USB_CLASS_MASS_STORAGE		8
#define USB_CLASS_HUB					9
#define USB_CLASS_CDC_DATA			0x0a
#define USB_CLASS_CSCID				0x0b	/* chip+ smart card */
#define USB_CLASS_CONTENT_SEC			0x0d	/* content security */
#define USB_CLASS_APP_SPEC				0xfe
#define USB_CLASS_VENDOR_SPEC			0xff

/*
 * USB types
 */
#define USB_TYPE_MASK					(0x03 << 5)
#define USB_TYPE_STANDARD				(0x00 << 5)
#define USB_TYPE_CLASS					(0x01 << 5)
#define USB_TYPE_VENDOR				(0x02 << 5)
#define USB_TYPE_RESERVED				(0x03 << 5)

/*
 * USB recipients
 */
#define USB_RECIP_MASK					0x1f
#define USB_RECIP_DEVICE				0x00
#define USB_RECIP_INTERFACE			0x01
#define USB_RECIP_ENDPOINT				0x02
#define USB_RECIP_OTHER				0x03

/*
 * USB directions
 */
#define USB_DIR_OUT						0		/* to device */
#define USB_DIR_IN						0x80		/* to host */

/*
 * Descriptor types
 */
#define USB_DT_DEVICE					0x01
#define USB_DT_CONFIG					0x02
#define USB_DT_STRING					0x03
#define USB_DT_INTERFACE				0x04
#define USB_DT_ENDPOINT				0x05

#define USB_DT_HID						(USB_TYPE_CLASS | 0x01)
#define USB_DT_REPORT					(USB_TYPE_CLASS | 0x02)
#define USB_DT_PHYSICAL				(USB_TYPE_CLASS | 0x03)
#define USB_DT_HUB						(USB_TYPE_CLASS | 0x09)

/*
 * Descriptor sizes per descriptor type
 */
#define USB_DT_DEVICE_SIZE				18
#define USB_DT_CONFIG_SIZE				9
#define USB_DT_INTERFACE_SIZE			9
#define USB_DT_ENDPOINT_SIZE			7
#define USB_DT_ENDPOINT_AUDIO_SIZE	9	/* Audio extension */
#define USB_DT_HUB_NONVAR_SIZE		7
#define USB_DT_HID_SIZE				9

/*
 * Endpoints
 */
#define USB_ENDPOINT_NUMBER_MASK		0x0f	/* in bEndpointAddress */
#define USB_ENDPOINT_DIR_MASK			0x80

#define USB_ENDPOINT_XFERTYPE_MASK	0x03	/* in bmAttributes */
#define USB_ENDPOINT_XFER_CONTROL	0
#define USB_ENDPOINT_XFER_ISOC		1
#define USB_ENDPOINT_XFER_BULK		2
#define USB_ENDPOINT_XFER_INT			3

/*
 * USB Packet IDs (PIDs)
 */
#define USB_PID_UNDEF_0				0xf0
#define USB_PID_OUT						0xe1
#define USB_PID_ACK						0xd2
#define USB_PID_DATA0					0xc3
#define USB_PID_PING					0xb4	/* USB 2.0 */
#define USB_PID_SOF						0xa5
#define USB_PID_NYET					0x96	/* USB 2.0 */
#define USB_PID_DATA2					0x87	/* USB 2.0 */
#define USB_PID_SPLIT					0x78	/* USB 2.0 */
#define USB_PID_IN						0x69
#define USB_PID_NAK						0x5a
#define USB_PID_DATA1					0x4b
#define USB_PID_PREAMBLE				0x3c	/* Token mode */
#define USB_PID_ERR						0x3c	/* USB 2.0: handshake mode */
#define USB_PID_SETUP					0x2d
#define USB_PID_STALL					0x1e
#define USB_PID_MDATA					0x0f	/* USB 2.0 */

/*
 * Standard requests
 */
#define USB_REQ_GET_STATUS			0x00
#define USB_REQ_CLEAR_FEATURE			0x01
#define USB_REQ_SET_FEATURE			0x03
#define USB_REQ_SET_ADDRESS			0x05
#define USB_REQ_GET_DESCRIPTOR		0x06
#define USB_REQ_SET_DESCRIPTOR		0x07
#define USB_REQ_GET_CONFIGURATION	0x08
#define USB_REQ_SET_CONFIGURATION	0x09
#define USB_REQ_GET_INTERFACE			0x0A
#define USB_REQ_SET_INTERFACE			0x0B
#define USB_REQ_SYNCH_FRAME			0x0C

/* HUB CLASS SPECIFIC REQUEST*/
#define HUB_CLASS_CLEAR_TT_BUFFER		0x08
#define HUB_CLASS_RESET_TT				0x09
#define HUB_CLASS_GET_TT_STATE 		0x0A
#define HUB_CLASS_STOP_TT				0x0B
/*
 * HID requests
 */
#define USB_REQ_GET_REPORT			0x01
#define USB_REQ_GET_IDLE				0x02
#define USB_REQ_GET_PROTOCOL			0x03
#define USB_REQ_SET_REPORT				0x09
#define USB_REQ_SET_IDLE				0x0A
#define USB_REQ_SET_PROTOCOL			0x0B

/* All standard descriptors have these 2 fields in common */
struct usb_descriptor_header {
	u8_t		bLength;
	u8_t  	bDescriptorType;
} __attribute__ ((packed));

/* Device descriptor */
struct usb_device_descriptor {
	u8_t  	bLength;
	u8_t  	bDescriptorType;
	u16_t 	bcdUSB;
	u8_t  	bDeviceClass;
	u8_t  	bDeviceSubClass;
	u8_t  	bDeviceProtocol;
	u8_t  	bMaxPacketSize0;
	u16_t 	idVendor;
	u16_t 	idProduct;
	u16_t 	bcdDevice;
	u8_t  	iManufacturer;
	u8_t  	iProduct;
	u8_t  	iSerialNumber;
	u8_t  	bNumConfigurations;
} __attribute__ ((packed));

/* Configuration descriptor information.. */
struct usb_config_descriptor {
	u8_t  bLength		__attribute__ ((packed));
	u8_t  bDescriptorType	__attribute__ ((packed));
	u16_t wTotalLength	__attribute__ ((packed));
	u8_t  bNumInterfaces	__attribute__ ((packed));
	u8_t  bConfigurationValue __attribute__ ((packed));
	u8_t  iConfiguration	__attribute__ ((packed));
	u8_t  bmAttributes	__attribute__ ((packed));
	u8_t  MaxPower		__attribute__ ((packed));

	struct usb_interface *interface;

   	unsigned char *extra;   /* Extra descriptors */
	int extralen;
};

/* String descriptor */
struct usb_string_descriptor {
	u8_t  bLength;
	u8_t  bDescriptorType;
	u16_t wData[1];
} __attribute__ ((packed));


/* This is arbitrary.
 * From USB 2.0 spec Table 11-13, offset 7, a hub can
 * have up to 255 ports. The most yet reported is 10.
 */

struct usb_device {
	int		devnum;		/* Address on USB bus */

	unsigned int toggle[2];		/* one bit for each endpoint ([0] = IN, [1] = OUT) */
	unsigned int halted[2];		/* endpoint halts; one bit per endpoint # & direction; */
							/* [0] = IN, [1] = OUT */
	int epmaxpacketin[16];		/* INput endpoint specific maximums */
	int epmaxpacketout[16];		/* OUTput endpoint specific maximums */

	struct usb_device_descriptor 	descriptor;/* Descriptor */
	struct usb_config_descriptor 	*config;	/* All of the configs */
	struct usb_config_descriptor 	*actconfig;/* the active configuration */
};


#endif // _usb_h_
