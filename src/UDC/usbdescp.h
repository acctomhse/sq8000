#ifndef	_udc_descriptor_h_
#define	_udc_descriptor_h_

/* scusb standard descriptor */
u8_t scusbDscr[] =
{
	/* Device Descriptor */
	18,					/* Descriptor length                   */
	DEV_DSCR_TYPE,		/* Descriptor type 	                   */
	00,02,				/* Specification Version (BCD)         */
	00,					/* Device class 	                   */
	00,					/* Device sub-class 	               */
	00,					/* Device sub-sub-class                */
	CTL_MAX_PKT,		/* Maximum packet size 	               */
	0xcb,0x05,	        /* Vendor ID 		                   */
	0x83,0x14,	        /* Product ID (Sample Device)          */
	0x00,0x00,	        /* Product version ID 	               */
	1,					/* Manufacturer string index           */
	2,					/* Product string index                */
	0,					/* Serial number string index          */
	1,					/* Number of configurations            */

	/* DeviceQualDscr */
	10,					/* Descriptor length          	       */
	QUAL_DSCR_TYPE,		/* Descriptor type 	      	           */
	0x00,0x02,	        /* Specification Version (BCD)	       */
	00,					/* Device class 	      	           */
	00,					/* Device sub-class 	      	       */
	00,					/* Device sub-sub-class       	       */
	CTL_MAX_PKT,		/* Maximum packet size 	      	       */
	1,					/* Number of configurations   	       */
	0,					/* Reserved                   	       */

	/* HighSpeedConfigDscr */
	9,					/* Descriptor length                   */
	CFG_DSCR_TYPE,		/* Descriptor type 				       */
#if END_POINTER_NUM == 4
	39,					/* Total Length (LSB) 				   */
#elif END_POINTER_NUM == 16
	123,				/* Total Length (LSB) 				   */
#else
#error "****** END_POINTER_NUM is not well defined!"
#endif
	0,					/* Total Length (MSB) 				   */
	1,					/* Number of interfaces 			   */
	1,					/* Configuration number 			   */
	0,					/* Configuration string 			   */

#ifdef SUPPORT_REMOTE_WAKEUP
	0xE0,		        /* Attributes (b7 : 1, b6 - selfpwr, b5 - with rwu)  */
#else
	0xC0,		        /* Attributes (b7 : 1, b6 - selfpwr, b5 - no rwu)    */
#endif

	50,					/* Power requirement (div 2 ma)                      */

	/* Interface Descriptor */
	9,					/* Descriptor length                   */
	INTF_DSCR_TYPE,		/* Descriptor type 		               */
	0,					/* Zero-based index of this interface  */
	0,					/* Alternate setting 		           */
#if END_POINTER_NUM == 4
	3,					/* Number of end points  	           */
#elif END_POINTER_NUM == 16
	15,					/* Number of end points  	           */
#else
#error "****** END_POINTER_NUM is not well defined!"
#endif
#ifdef USE_USBPRINT_SYS
	0x07,		        /* Interface class: printer            */
	0x01,		        /* Interface sub class: printer	       */
	0x02,		        /* Interface protocol: bidirectional   */
#else   /* USE BULKUSB.SYS */
	0xff,
	0x00,
	0x00,
#endif

	0,					/* Interface descriptor string index   */
#if END_POINTER_NUM == 4
	/* Endpoint Descriptor */
	7,					/* Descriptor length                   */
	EDP_DSCR_TYPE,		/* Descriptor type 		               */
	BLKOUT_ENDP_NUM_SET1,      /* Endpoint number, and direction: OUT */
	2,					/* Endpoint type : Bulk		           */
	BULK_PKT_LSB,       /* Maximum packet size (LSB) 	       */
	BULK_PKT_MSB,		/* Max packet size (MSB) 	           */
	0x00,				/* Polling interval                    */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                   */
	EDP_DSCR_TYPE,		/* Descriptor type 		               */
	BLKIN_ENDP_NUM_SET1,      /* Endpoint number, and direction: IN  */
	2,					/* Endpoint type : Bulk		           */
	BULK_PKT_LSB,		/* Maximum packet size (LSB) 	       */
	BULK_PKT_MSB,		/* Max packet size (MSB) 	           */
	0x00,				/* Polling interval                    */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                   */
	EDP_DSCR_TYPE,		/* Descriptor type 		               */
	INTRIN_ENDP_NUM_SET1,     /* Endpoint number, and direction: IN  */
	3,					/* Endpoint type : Interrupt	       */
	INTRIN_PKT_LSB,		/* Maximum packet size (LSB) 	       */
	INTRIN_PKT_MSB,		/* Max packet size (MSB) 	           */
	0x0b,				/* Polling interval                    */

#elif END_POINTER_NUM == 16
	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 		                */
	BLKOUT_ENDP_NUM_SET1,     /* Endpoint number, and direction : OUT */
	2,					/* Endpoint type 		                */
	BULK_PKT_LSB,		/* Maximum packet size (LSB) 	        */
	BULK_PKT_MSB,		/* Max packet size (MSB) 	            */
	0x00,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 			            */
	BLKIN_ENDP_NUM_SET1,      /* Endpoint number, and direction : IN	*/
	2,					/* Endpoint type 			            */
	BULK_PKT_LSB,		/* Maximum packet size (LSB) 		    */
	BULK_PKT_MSB,		/* Max packet size (MSB) 		        */
	0x00,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 			            */
	INTRIN_ENDP_NUM_SET1,     /* Endpoint number, and direction : IN	*/
	3,					/* Endpoint type 			            */
	INTRIN_PKT_LSB,		/* Maximum packet size (LSB) 		    */
	INTRIN_PKT_MSB,		/* Max packet size (MSB) 		        */
	0x0b,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 		                */
	BLKOUT_ENDP_NUM_SET2,     /* Endpoint number, and direction : OUT */
	2,					/* Endpoint type 		                */
	BULK_PKT_LSB,		/* Maximum packet size (LSB) 	        */
	BULK_PKT_MSB,		/* Max packet size (MSB) 	            */
	0x00,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 			            */
	BLKIN_ENDP_NUM_SET2,      /* Endpoint number, and direction : IN	*/
	2,					/* Endpoint type 			            */
	BULK_PKT_LSB,		/* Maximum packet size (LSB) 		    */
	BULK_PKT_MSB,		/* Max packet size (MSB) 		        */
	0x00,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 			            */
	INTRIN_ENDP_NUM_SET2,     /* Endpoint number, and direction : IN	*/
	3,					/* Endpoint type 			            */
	INTRIN_PKT_LSB,		/* Maximum packet size (LSB) 		    */
	INTRIN_PKT_MSB,		/* Max packet size (MSB) 		        */
	0x0b,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 		                */
	BLKOUT_ENDP_NUM_SET3,     /* Endpoint number, and direction : OUT */
	2,					/* Endpoint type 		                */
	BULK_PKT_LSB,		/* Maximum packet size (LSB) 	        */
	BULK_PKT_MSB,		/* Max packet size (MSB) 	            */
	0x00,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 			            */
	BLKIN_ENDP_NUM_SET3,      /* Endpoint number, and direction : IN	*/
	2,					/* Endpoint type 			            */
	BULK_PKT_LSB,		/* Maximum packet size (LSB) 		    */
	BULK_PKT_MSB,		/* Max packet size (MSB) 		        */
	0x00,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 			            */
	INTRIN_ENDP_NUM_SET3,     /* Endpoint number, and direction : IN	*/
	3,					/* Endpoint type 			            */
	INTRIN_PKT_LSB,		/* Maximum packet size (LSB) 		    */
	INTRIN_PKT_MSB,		/* Max packet size (MSB) 		        */
	0x0b,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 		                */
	BLKOUT_ENDP_NUM_SET4,     /* Endpoint number, and direction : OUT */
	2,					/* Endpoint type 		                */
	BULK_PKT_LSB,		/* Maximum packet size (LSB) 	        */
	BULK_PKT_MSB,		/* Max packet size (MSB) 	            */
	0x00,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 			            */
	BLKIN_ENDP_NUM_SET4,      /* Endpoint number, and direction : IN	*/
	2,					/* Endpoint type 			            */
	BULK_PKT_LSB,		/* Maximum packet size (LSB) 		    */
	BULK_PKT_MSB,		/* Max packet size (MSB) 		        */
	0x00,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 			            */
	INTRIN_ENDP_NUM_SET4,     /* Endpoint number, and direction : IN	*/
	3,					/* Endpoint type 			            */
	INTRIN_PKT_LSB,		/* Maximum packet size (LSB) 		    */
	INTRIN_PKT_MSB,		/* Max packet size (MSB) 		        */
	0x0b,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 		                */
	BLKOUT_ENDP_NUM_SET5,     /* Endpoint number, and direction : OUT */
	2,					/* Endpoint type 		                */
	BULK_PKT_LSB,		/* Maximum packet size (LSB) 	        */
	BULK_PKT_MSB,		/* Max packet size (MSB) 	            */
	0x00,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 			            */
	BLKIN_ENDP_NUM_SET5,      /* Endpoint number, and direction : IN	*/
	2,					/* Endpoint type 			            */
	BULK_PKT_LSB,		/* Maximum packet size (LSB) 		    */
	BULK_PKT_MSB,		/* Max packet size (MSB) 		        */
	0x00,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 			            */
	INTRIN_ENDP_NUM_SET5,     /* Endpoint number, and direction : IN	*/
	3,					/* Endpoint type 			            */
	INTRIN_PKT_LSB,		/* Maximum packet size (LSB) 		    */
	INTRIN_PKT_MSB,		/* Max packet size (MSB) 		        */
	0x0b,				/* Polling interval                     */
#else
#error "****** END_POINTER_NUM is not well defined!"
#endif

	/* FullSpeedConfigDscr */
	9,					/* Descriptor length                   */
	CFG_DSCR_TYPE,		/* Descriptor type 				       */
#if END_POINTER_NUM == 4
	39,					/* Total Length (LSB) 				   */
#elif END_POINTER_NUM == 16
	123,				/* Total Length (LSB) 				   */
#else
#error "****** END_POINTER_NUM is not well defined!"
#endif
	0,					/* Total Length (MSB) 				   */
	1,					/* Number of interfaces 			   */
	1,					/* Configuration number 			   */
	0,					/* Configuration string 			   */

#ifdef SUPPORT_REMOTE_WAKEUP
	0xE0,		        /* Attributes (b7 : 1, b6 - selfpwr, b5 - with rwu)  */
#else
	0xC0,		        /* Attributes (b7 : 1, b6 - selfpwr, b5 - no rwu)    */
#endif

	50,					/* Power requirement (div 2 ma)                      */

	/* Interface Descriptor */
	9,					/* Descriptor length                   */
	INTF_DSCR_TYPE,		/* Descriptor type 		               */
	0,					/* Zero-based index of this interface  */
	0,					/* Alternate setting 		           */
#if END_POINTER_NUM == 4
	3,					/* Number of end points  	           */
#elif END_POINTER_NUM == 16
	15,					/* Number of end points  	           */
#else
#error "****** END_POINTER_NUM is not well defined!"
#endif
#ifdef USE_USBPRINT_SYS
	0x07,		        /* Interface class: printer            */
	0x01,		        /* Interface sub class: printer	       */
	0x02,		        /* Interface protocol: bidirectional   */
#else   /* USE BULKUSB.SYS */
	0xff,		        /* Interface class: printer            */
	0x00,		        /* Interface sub class: printer	       */
	0x00,		        /* Interface protocol: bidirectional   */
#endif

	0,					/* Interface descriptor string index   */
#if END_POINTER_NUM == 4
	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 		                */
	BLKOUT_ENDP_NUM_SET1,     /* Endpoint number, and direction : OUT */
	2,					/* Endpoint type 		                */
	FS_BULK_PKT_LSB,    /* Maximum packet size (LSB) 	        */
	FS_BULK_PKT_MSB,	/* Max packet size (MSB) 	            */
	0x00,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 			            */
	BLKIN_ENDP_NUM_SET1,      /* Endpoint number, and direction : IN	*/
	2,					/* Endpoint type 			            */
	FS_BULK_PKT_LSB,    /* Maximum packet size (LSB) 		    */
	FS_BULK_PKT_MSB,	/* Max packet size (MSB) 		        */
	0x00,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 			            */
	INTRIN_ENDP_NUM_SET1,     /* Endpoint number, and direction : IN	*/
	3,					/* Endpoint type 			            */
	INTRIN_PKT_LSB,		/* Maximum packet size (LSB) 		    */
	INTRIN_PKT_MSB,		/* Max packet size (MSB) 		        */
	0x0b,				/* Polling interval                     */

#elif END_POINTER_NUM == 16
	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 		                */
	BLKOUT_ENDP_NUM_SET1,     /* Endpoint number, and direction : OUT */
	2,					/* Endpoint type 		                */
	FS_BULK_PKT_LSB,    /* Maximum packet size (LSB) 	        */
	FS_BULK_PKT_MSB,	/* Max packet size (MSB) 	            */
	0x00,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 			            */
	BLKIN_ENDP_NUM_SET1,      /* Endpoint number, and direction : IN	*/
	2,					/* Endpoint type 			            */
	FS_BULK_PKT_LSB,    /* Maximum packet size (LSB) 		    */
	FS_BULK_PKT_MSB,	/* Max packet size (MSB) 		        */
	0x00,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 			            */
	INTRIN_ENDP_NUM_SET1,     /* Endpoint number, and direction : IN	*/
	3,					/* Endpoint type 			            */
	INTRIN_PKT_LSB,		/* Maximum packet size (LSB) 		    */
	INTRIN_PKT_MSB,		/* Max packet size (MSB) 		        */
	0x0b,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 		                */
	BLKOUT_ENDP_NUM_SET2,     /* Endpoint number, and direction : OUT */
	2,					/* Endpoint type 		                */
	FS_BULK_PKT_LSB,    /* Maximum packet size (LSB) 	        */
	FS_BULK_PKT_MSB,	/* Max packet size (MSB) 	            */
	0x00,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 			            */
	BLKIN_ENDP_NUM_SET2,      /* Endpoint number, and direction : IN	*/
	2,					/* Endpoint type 			            */
	FS_BULK_PKT_LSB,    /* Maximum packet size (LSB) 		    */
	FS_BULK_PKT_MSB,	/* Max packet size (MSB) 		        */
	0x00,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 			            */
	INTRIN_ENDP_NUM_SET2,     /* Endpoint number, and direction : IN	*/
	3,					/* Endpoint type 			            */
	INTRIN_PKT_LSB,		/* Maximum packet size (LSB) 		    */
	INTRIN_PKT_MSB,		/* Max packet size (MSB) 		        */
	0x0b,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 		                */
	BLKOUT_ENDP_NUM_SET3,     /* Endpoint number, and direction : OUT */
	2,					/* Endpoint type 		                */
	FS_BULK_PKT_LSB,	/* Maximum packet size (LSB) 	        */
	FS_BULK_PKT_MSB,	/* Max packet size (MSB) 	            */
	0x00,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 			            */
	BLKIN_ENDP_NUM_SET3,      /* Endpoint number, and direction : IN	*/
	2,					/* Endpoint type 			            */
	FS_BULK_PKT_LSB,	/* Maximum packet size (LSB) 		    */
	FS_BULK_PKT_MSB,	/* Max packet size (MSB) 		        */
	0x00,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 			            */
	INTRIN_ENDP_NUM_SET3,     /* Endpoint number, and direction : IN	*/
	3,					/* Endpoint type 			            */
	INTRIN_PKT_LSB,		/* Maximum packet size (LSB) 		    */
	INTRIN_PKT_MSB,		/* Max packet size (MSB) 		        */
	0x0b,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 		                */
	BLKOUT_ENDP_NUM_SET4,     /* Endpoint number, and direction : OUT */
	2,					/* Endpoint type 		                */
	FS_BULK_PKT_LSB,	/* Maximum packet size (LSB) 	        */
	FS_BULK_PKT_MSB,	/* Max packet size (MSB) 	            */
	0x00,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 			            */
	BLKIN_ENDP_NUM_SET4,      /* Endpoint number, and direction : IN	*/
	2,					/* Endpoint type 			            */
	FS_BULK_PKT_LSB,	/* Maximum packet size (LSB) 		    */
	FS_BULK_PKT_MSB,	/* Max packet size (MSB) 		        */
	0x00,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 			            */
	INTRIN_ENDP_NUM_SET4,     /* Endpoint number, and direction : IN	*/
	3,					/* Endpoint type 			            */
	INTRIN_PKT_LSB,		/* Maximum packet size (LSB) 		    */
	INTRIN_PKT_MSB,		/* Max packet size (MSB) 		        */
	0x0b,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 		                */
	BLKOUT_ENDP_NUM_SET5,     /* Endpoint number, and direction : OUT */
	2,					/* Endpoint type 		                */
	FS_BULK_PKT_LSB,	/* Maximum packet size (LSB) 	        */
	FS_BULK_PKT_MSB,	/* Max packet size (MSB) 	            */
	0x00,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 			            */
	BLKIN_ENDP_NUM_SET5,      /* Endpoint number, and direction : IN	*/
	2,					/* Endpoint type 			            */
	FS_BULK_PKT_LSB,	/* Maximum packet size (LSB) 		    */
	FS_BULK_PKT_MSB,	/* Max packet size (MSB) 		        */
	0x00,				/* Polling interval                     */

	/* Endpoint Descriptor */
	7,					/* Descriptor length                    */
	EDP_DSCR_TYPE,		/* Descriptor type 			            */
	INTRIN_ENDP_NUM_SET5,     /* Endpoint number, and direction : IN	*/
	3,					/* Endpoint type 			            */
	INTRIN_PKT_LSB,		/* Maximum packet size (LSB) 		    */
	INTRIN_PKT_MSB,		/* Max packet size (MSB) 		        */
	0x0b,				/* Polling interval                     */
#else
#error "****** END_POINTER_NUM is not well defined!"
#endif

	/* StringDscr0 */
	4,					/* String descriptor length   */
	STR_DSCR_TYPE,		/* String Descriptor          */
	0x09,0x04,

	/* StringDscr1 */
	20,					/* String descriptor length   */
	STR_DSCR_TYPE,		/* String Descriptor Type     */
	'S',00,
	'o',00,
	'c',00,
	'l',00,
	'e',00,
	'T',00,
	'e',00,
	'c',00,
	'h',00,

	/* StringDscr2 */
	22,					/* String descriptor length   */
	STR_DSCR_TYPE,		/* String Descriptor Type     */
	'B',00,
	'0',00,
	'1',00,
	'U',00,
	'S',00,
	'B',00,
	' ',00,
	'U',00,
	'D',00,
	'C',00
};

#endif /* _udc_descriptor_h_ */
