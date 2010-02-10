#include "global.h"
#include "genlib.h"
#include "platform.h"
#include "irqs.h"
#include "ohcictrl.h"
#include "ohcimem.h"
#include "ohcireg.h"
#include "usb.h"
#include "Interrupt/interrupt.h"
#include "EHCI/ehcireg.h"
#ifdef CONFIG_PC9002
#include "MP_GPIO/mp-gpio.h"
#include "MP_GPIO/regs-mp-gpio.h"
#endif
#if defined (CONFIG_PC7210) || defined (CONFIG_PDK)
#include <scu.h>
#endif


//For Test Tools
#define SETUP_BUFF_ADDR_OHCI					TEST_PATTERN_START //0x00300000		//0x1000 4096 = 1024*4	changed from 0x80300000 to 0xA0300000
#define DATA_IN_ADDR_OHCI						(TEST_PATTERN_START + 0x1000) //0x00306000		//0x5000 20480    65536 0x10000
#define DATA_OUT_ADDR_OHCI					(TEST_PATTERN_START + 0x1000 + 0x10000)//0x0031B000		//0x5000 20480		0xA030B000  changed to 0xA031B000
#define INTR_OUT_ADDR_OHCI					(TEST_PATTERN_START + 0x1000 + 0x10000 + 0x10000)//0x00320000		//0x100000 1048576
#define INTR_IN_ADDR_OHCI						(TEST_PATTERN_START + 0x1000 + 0x10000 + 0x10000) //0x00320000		//0x100000 1048576

#define Hc_HCCA									(TEST_PATTERN_START + 0x1000 + 0x10000 + 0x10000 + 0x5000) //0x00400400			// viswa defined host controller communication area changed from 0x80400400 t0 0xA0400400

OHCSTAT	ohci_status;		// OHCSTAT structure for Device related details
BYTE	portconnect; // flag set when a device is connected and reset to zero when no device is connected
BYTE 	TransferComplete;// flag set when an interrupt occurs due to transaction completion.
BYTE 	ResetChange;	// flag set when interrupt occured due to reset change in the Rhportsts reg.
BYTE	MpsKnown;	// initially this variable is zero and we have to perform get device descriptor to know the MPS of control endpoint of the connected device and after successfull completion of get device descriptor this variable is initialzed to correct value as specified in the descriptor.
BYTE 	BulkInTog;	// used to store the bulk in endpoint toggle value to be used for next transaction.
BYTE 	IntrInTog;	// used to store the intr in endpoint toggle value to be used for next transaction.
BYTE	BulkOutTog;
POHCI_HCCA	HcHcca;		// instance of HCCA structure.

u32 ohci_base;
int ohci_irq;

extern int ohci_test(int autotest);

extern struct test_item_container ohci_main_container;

extern int OHCITesting(int autotest)
{
        int     ret = 0;

        #ifdef CONFIG_PC9002
                socle_mp_gpio_set_port_num_value(PA,6,0);
        #endif

                //20080123 leonid add for USB Downstream
        #if defined(CONFIG_PC7210) || defined(CONFIG_PDK)
                socle_scu_usb_tranceiver_downstream();
        #endif

	ret = test_item_ctrl(&ohci_main_container,autotest);

	return ret;
}

extern int
ohci_0_test(int autotest)
{
        int ret=0;

        ohci_base = SOCLE_AHB0_UHC0;
        ohci_irq = SOCLE_INTC_UHC0;
        ret = ohci_test(autotest);

        return ret;
}

extern int
ohci_1_test(int autotest)
{
        int ret=0;

        ohci_base = SOCLE_AHB0_UHC1;
        ohci_irq = SOCLE_INTC_UHC1;
        ret = ohci_test(autotest);

        return ret;
}




void PrintOHCIRegs()
{
	printf("####### OHCI Operational Registers ########\n");
	printf("==========================================\n");
	
	printf("HcRevision(ohci_base)                                               :%x\n",readw(HcRevision(ohci_base)));
	printf("HcControl                                                 :%x\n",readw(HcControl(ohci_base)));
	printf("HcCommandStatus                                    :%x\n",readw(HcCMDSTS(ohci_base)));
	printf("HcInterruptStatus                                      :%x\n",readw(HcINTRSTS(ohci_base)));
	printf("HcInterruptEnable                                      :%x\n",readw(HcINTREN(ohci_base)));
	printf("HcInterruptDisable                                     :%x\n",readw(HcINTRDISEN(ohci_base)));
	printf("HcHCCA                                                    :%x\n",readw(HcHCCA(ohci_base)));
	printf("HcPeriodCurrentEd                                     :%x\n",readw(HcPeriodCurrED(ohci_base)));
	printf("HcControlHeadEd                                       :%x\n",readw(HcCtrlHeadED(ohci_base)));
	printf("HcControlCurrentEd                                    :%x\n",readw(HcCtrlCurrED(ohci_base)));
	printf("HcBulkHeadEd                                           :%x\n",readw(HcBulkHeadED(ohci_base)));
	printf("HcBulkCurrentEd                                       :%x\n",readw(HcBulkCurrED(ohci_base)));
	printf("HcDoneHead                                             :%x\n",readw(HcDoneHead(ohci_base)));
	printf("HcFrameInterval                                       :%x\n",readw(HcFmIntr(ohci_base)));
	printf("HcFmRemaining                                        :%x\n",readw(HcFmremaining(ohci_base)));
	printf("HcFmNumber                                           :%x\n",readw(Hcfmnumber(ohci_base)));
	printf("HcPeriodicStart                                        :%x\n",readw(Hcperiodicstart(ohci_base)));
	printf("HcLSThreshold                                         :%x\n",readw(Hclsthresh(ohci_base)));
//	printf("HcRhDescriptorA                                       :%x\n",readw(0x180a4048));
//	printf("HcRhDescriptorb                                       :%x\n",readw(0x180a404C));
//	printf("HcRhStatus                                              :%x\n",readw(0x180a4050));
//	printf("HcRhPortStatus                                        :%x\n",readw(0x180a4054));
//	printf("the value of HcHcca.frame number is 	   :%x\n",HcHcca->frame_no);
	
}

void Validate_OHCIRegs()
{
	int error=0;
	printf(("Validating Registers"));  
	if(readw(HcRevision(ohci_base)) != 0x00000001)
	{
		printf("HcRevision(ohci_base) register not in default value\n");
		error = 1;
	}
	if(readw(HcControl(ohci_base)) != 0x00000000)
	{
		printf("HcControl register not in default value\n");
		error = 1;
	}
	if(readw(HcCMDSTS(ohci_base)) != 0x00000000)
	{
		printf("HcCommand register not in default value\n");
		error = 1;
	}
	if(readw(HcINTRSTS(ohci_base)) != 0x00000000)
	{
		printf("HcInterruptStatus register not in default value\n");
		error = 1;
	}
	if(readw(HcINTREN(ohci_base)) != 0x00000000)
	{
		printf("HcInterruptEnable register not in default value\n");
		error = 1;
	}
	if(readw(HcINTRDISEN(ohci_base)) != 0x00000000)
	{
		printf("HcInterruptDisable register not in default value\n");
		error = 1;
	}
	if(readw(HcHCCA(ohci_base)) != 0x00000000)
	{
		printf("HcHCCA register not in default value\n");
		error = 1;
	}
	if(readw(HcPeriodCurrED(ohci_base)) != 0x00000000)
	{
		printf("HcPeriodCurrentEd register not in default value\n");
		error = 1;
	}
	if(readw(HcCtrlHeadED(ohci_base)) != 0x00000000)
	{
		printf("HcControlHeadEd register not in default value\n");
		error = 1;
	}
	if(readw(HcCtrlCurrED(ohci_base)) != 0x00000000)
	{
		printf("HcControlCurrentEd register not in default value\n");
		error = 1;
	}
	if(readw(HcBulkHeadED(ohci_base)) != 0x00000000)
	{
		printf("HcBulkHeadEd register not in default value\n");
		error = 1;
	}
	if(readw(HcBulkCurrED(ohci_base)) != 0x00000000)
	{
		printf("HcBulkCurrentEd register not in default value\n");
		error = 1;
	}
	if(readw(HcDoneHead(ohci_base)) != 0x00000000)
	{
		printf("HcDoneHead register not in default value\n");
		error = 1;
	}
	if(readw(HcFmIntr(ohci_base)) != 0x00002EDF)
	{
		printf("HcFrameInterval register not in default value\n");
		error = 1;
	}
	if(readw(HcFmremaining(ohci_base)) != 0x00000000)
	{
		printf("HcFrameRemaining register not in default value\n");
		error = 1;
	}
	if(readw(Hcfmnumber(ohci_base)) != 0x00000000)
	{
		printf("HcFrameNumber register not in default value\n");
		error = 1;
	}
	if(readw(Hcperiodicstart(ohci_base)) != 0x00000000)
	{
		printf("HcPeriodicStart register not in default value\n");
		error = 1;
	}
	if(readw(Hclsthresh(ohci_base)) != 0x00000628)
	{
		printf("HcRevision(ohci_base) register not in default value\n");
		error = 1;
	}
	if(error == 0)
		printf(("SUCCESS : Validating Registers\n"));  
}

/*
int OHCI_Halt(void)
{//check
	u32_t		UsbCmd;
	u32_t		UsbSts; 

	UsbSts = readw(USBSTS);
	if ((UsbSts & STS_HALT) != 0)
		return true;

	UsbCmd=readw(USBCMD);
	UsbCmd=UsbCmd & ~CMD_RUN;
	writew(UsbCmd,USBCMD);
//	printf(("EHC Halted Successfully"));	
	return true;

}
*/
void OHCI_Reset(void)
{
	unsigned long HcCommandStatus;

	printf("Inside ohci reset\n");
	HcCommandStatus=(readw(HcCMDSTS(ohci_base)/*0x180a4008*/))|0x00000001;  //HostControllerReset
	writew(HcCommandStatus,HcCMDSTS(ohci_base)/*0x180a4008*/);
	HcCommandStatus = readw(HcCMDSTS(ohci_base)/*0x180a4008*/);
	printf("the value of hccommand status register is %x\n",HcCommandStatus);
	while(HcCommandStatus & 0x00000001)
	{
		HcCommandStatus=readw(HcCMDSTS(ohci_base)/*0x180a4008*/);
		printf("inside loop\n");
	}
	printf("Ohci reset completed\n");
}
/*spec 4.14 */
int OHCI_Port_TestMode()
{


	return 0;
	
}
/*
	0:Disable 1:Enable
*/
void OHCI_ControlList_Enable(BYTE Enable)
{
	ULONG HcControlreg;
	printf("Inside control list enable\n");
	HcControlreg = readw(HcControl(ohci_base));
	if(Enable)
	{
		HcControlreg = HcControlreg | 0x00000010;			// 0x00000030		 control list 
	}
	else
	{
		HcControlreg = HcControlreg & 0xFFFFFFCF;
	}
	writew(HcControlreg,HcControl(ohci_base));
//	printf("the value fo hc control register is %x\n",readw(0x180a4004));	
}

void OHCI_BulkList_Enable(BYTE Enable)
{
	ULONG HcControlreg;
	//printf("Inside bulk list enable\n");
	HcControlreg = readw(HcControl(ohci_base)/*0x180a4004*/);
	if(Enable)
	{
		HcControlreg = HcControlreg | 0x00000020;			// 0x00000020		 only bulk list enable
	}
	else
	{
		HcControlreg = HcControlreg & 0xFFFFFFCF;
	}
	writew(HcControlreg,HcControl(ohci_base)/*0x180a4004*/);
//	printf("the value fo hc control register is %x\n",readw(0x180a4004));	
}

void OHCI_PeriodicList_Enable(BYTE Enable)
{
	ULONG HcControlreg;
	printf("Inside bulk list enable\n");
	HcControlreg = readw(HcControl(ohci_base));
	if(Enable)
	{
		HcControlreg = HcControlreg | 0x00000004;			// 0x00000020		 only periodic list enable
	}
	else
	{
		HcControlreg = HcControlreg & 0xFFFFFFFB;
	}
	writew(HcControlreg,HcControl(ohci_base));
//	printf("the value fo hc control register is %x\n",readw(0x180a4004));	
}


//api_new
void OHCI_IsoList_Enable(BYTE Enable)
{
	ULONG HcControlreg;
	printf("Inside Isochronous list enable\n");
	HcControlreg = readw(HcControl(ohci_base)/*0x180a4004*/);
	if(Enable)
	{
		HcControlreg = HcControlreg | 0x0000000C;			//  Iso list as well as Periodic list enable
		printf("Hc controlREg value is :%x",HcControlreg);
	}
	else
	{
		HcControlreg = HcControlreg & 0xFFFFFFFB;
	}
	writew(HcControlreg,HcControl(ohci_base)/*0x180a4004*/);
//	printf("the value fo hc control register is %x\n",readw(0x180a4004));	
}

/*
	void OHCI_InitMemory()

	This function is to initialize all the memory regions used by OHCI
*/
void OHCI_InitMemory()
{
	//Initialize Memory for  Control transfers
	memset((UCHAR *)SETUP_BUFF_ADDR_OHCI,0,4096);

	//Initialize Memory for OUT Data transfers
	memset((UCHAR *)DATA_OUT_ADDR_OHCI,0,20480);
	
	//Initialize Memory for IN Data transfers
	memset((UCHAR *)DATA_IN_ADDR_OHCI,0,20480);

	//Initialize Memory for Intr in 
	memset((UCHAR *)INTR_IN_ADDR_OHCI,0,20480);	

	//Initialize Memory for Intr Out
	memset((UCHAR *)INTR_OUT_ADDR_OHCI,0,20480);	
}


/*
	void OHCI_Start()
	
	This function perform intialization steps.
	Intialization steps:
	+++++++++++++++++++
	Disables interrupts.
	Moves to USB OPERATIONAL State
	Performs Ohci Reset
	Moves to USB RESET State
	Installing Interrupt Service Routine and programming the platform's interrupt controller.
	Writing the Host controller registers to it default value.
	Moves to USB OPERATIONAL State.
	Applying power to the port.
	Enabling interrupts.
	Initializing the memory used during the testing.	
*/
void OHCI_Start()
{
	unsigned long temphcca,HcControlreg,HcRhPortstatusreg;
	OHCI_Intr_Disable();

	// moving the host controller to operational state
	HcControlreg = readw(HcControl(ohci_base)/*0x180a4004*/);
	HcControlreg= HcControlreg & 0xffffff3f;
	HcControlreg= HcControlreg|0x00000080;  //USB OPERATIONAL
	writew(HcControlreg,HcControl(ohci_base)/*0x180a4004*/);

	//Apply HC Reset
	OHCI_Reset();			//commented for testing

	printf("in ohci start after ohci reset\n");
//	PrintOHCIRegs();
	
	// moving the host controller to USBRESET state
	HcControlreg = readw(HcControl(ohci_base));
	HcControlreg= HcControlreg & 0xffffff3f;
	HcControlreg= HcControlreg|0x00000000;  //USB RESET state
	writew(HcControlreg,HcControl(ohci_base));
	printf("the value of hccontrol register is 		:%x\n",readw(HcControl(ohci_base)));

	// Enabke interrupt
	request_irq( ohci_irq, OHCIIntrHandler, NULL);
	
	// Write to hcPeriodicStart register to specify when periodic transactions should start
	writew(0x00003e67,Hcperiodicstart(ohci_base)); //start number 0x3e67 Ryan
	printf("HcPeriodicStart                                        :%x\n",readw(Hcperiodicstart(ohci_base)));

	// Load the FSMPS 
//	writew(0x27782edf,0x180a4034);
	writew(0x26502edf,HcFmIntr(ohci_base));
//	writew(0x27D72edf,0x180a4034);
	printf("HcFrameInterval                                       :%x\n",readw(HcFmIntr(ohci_base)/*0x180a4034*/));
	
	// Fill the base address of the HCCA into HcHCCA(ohci_base)
	memset((PUCHAR)Hc_HCCA,0,sizeof(OHCI_HCCA));
	printf("the value of Hc_HCCA is 	%x\n",Hc_HCCA);
	printf("the virtual address of Hc_HCCA is 	%x\n",((u32_t)Hc_HCCA));
	writew(((u32_t)Hc_HCCA),HcHCCA(ohci_base)/*0x180a4018*/);
	temphcca=readw(HcHCCA(ohci_base));
	printf("\nHcHcca Physical Address				: %x\n",temphcca);
	HcHcca = (POHCI_HCCA)Hc_HCCA;

	// moving the host controller to operational state
	HcControlreg = readw(HcControl(ohci_base));
	HcControlreg= HcControlreg & 0xffffff3f;
	HcControlreg= HcControlreg|0x00000083;  //USB OPERATIONAL state and CBSR as 3:1
	writew(HcControlreg,HcControl(ohci_base));
	printf("HcControl                                  :%x\n",readw(HcControl(ohci_base)));
	printf("the value of HcHcca.frame number is 	   :%x\n",HcHcca->frame_no);

	// apply port power status to RhPortStatus register
	HcRhPortstatusreg = 0x00000100;
	writew(HcRhPortstatusreg,HcRhPortStatus(ohci_base));
	// Enable device interrupts
	OHCI_Intr_Enable();
	printf("the value of HcHcca.frame number is 	   :%x\n",HcHcca->frame_no);
	
	OHCI_InitMemory();
	printf("the value of HcHcca.frame number is 	   :%x\n",HcHcca->frame_no);
}

/*
void OHCI_StopDevice()
{
	printf(("OHCI_StopDevice"));	
	// Disable device
	OHCI_Intr_Disable();
	
	OHCI_Halt();
	
	// Disconnect from interrupt
	//INT0_DISABLE(ohci_irq);

	//INT0_CLEAR(ohci_irq);	
	//INT0_CLR_MASK(ohci_irq);	

	printf(("============================================================================="));	

}
*/
//=============================================================
//For Test Tools
//=============================================================
/*
void OHCIIntrHandler(void* pparam)
{
	ULONG UsbSts;
	ULONG UsbIntr;
	ULONG PortSc1;
	
	printf("\nEHCIIintHandler");
	
	UsbSts=readw(USBSTS);
	UsbIntr=readw(USBINTR);
	PortSc1=readw(PORTSC);

	if(UsbSts & UsbIntr)
	{
		//Disable all interrupts
		writew(0x00, USBINTR);
		if(UsbSts & STS_INT)
		{
			printf(("USB Transaction Complete Interrupt"));
			//release buffer of Asyn or Period
			//Add_Free_QH_Ring(PQHEAD freeQH)
			
			writew(STS_INT,USBSTS);
		}
		if(UsbSts & STS_ERR)
		{
			printf(("USB Error Interrupt"));
			//release buffer
			writew(STS_ERR,USBSTS);
		}
		if(UsbSts & STS_PCD)
		{
			printf(("USB Port  Change Detect Interrupt"));
			PortSc1=readw(PORTSC);

			if(PortSc1 & PORT_CSC)	//Connect status change
			{
				if(PortSc1 & PORT_CONNECT)
				{
					printf(("Device Connect on Port1"));
					PortSc1=PortSc1|PORT_CSC;
					writew(PortSc1,PORTSC);
					//usb_new_device();
				}
				else
				{
					printf(("Device Disconnected From Port1"));
					PortSc1=PortSc1|PORT_CSC;
					writew(PortSc1,PORTSC);
				}
			}
			else if(PortSc1 &  PORT_PEC) //port enable change
			{
				if(PortSc1 & 0x00000004)
				{
					printf(("Device Enabled on Port1"));
					PortSc1=PortSc1|PORT_PEC;
					writew(PortSc1,PORTSC);
				}
				else
				{
					printf(("Device Disabled From Port1"));
					PortSc1=PortSc1|PORT_PEC;
					writew(PortSc1,PORTSC);
				}
			}
			
			writew(STS_PCD,USBSTS);
		}
		if(UsbSts & STS_FLR)
		{
			printf(("Frame List Rollover Interrupt"));
			writew(STS_FLR,USBSTS);
		}
		if(UsbSts & STS_FATAL)
		{
			printf(("Host System Error Interrupt"));
			writew(STS_FATAL,USBSTS);
		}
		if(UsbSts & STS_IAA)
		{
			printf(("Interrupt on Async Advance"));
			writew(STS_IAA,USBSTS);
		}
		//Reenable all interrupts here
		writew(0x05,USBINTR);
		//return TRUE;
	}
	else
	{
		printf("Other Device Interrupt");
	}
	


}
*/
//void ResumeTimerDpcRoutine(IN PKDPC Dpc,IN PVOID DeferredContext,IN PVOID SystemArgument1,IN PVOID SystemArgument2)

/*
void PortResume()
{

	//PDEVICE_OBJECT DeviceObject=dx->fdo;
	ULONG PortSc1;
	ULONG Status=0;
	ULONG BytesTxd=0;
	
	printf(("Comes to ResumeTimerDpcRoutine after 20 ms"));
	
	//Set Force Port Resume bit to 0
	

	PortSc1=readw(PORTSC);
	PortSc1=PortSc1&0xffffffbf;
	writew(PortSc1,PORTSC);
	
	//Wait until Port Resume bit is set to 0
	PortSc1=readw(PORTSC);
	while(PortSc1 & 0x40)
		
		if(!(PortSc1 & 0x80))
		{
			printf(("Port 1 Resume after Suspend"));
			Status=1;
		}
		else
		{
			printf(("Port 1 Cannot be Resumed after Suspend"));
			Status=0;
		}
		
		BytesTxd=4;
		
}
*/

/*
void PortReset()
{

	ULONG UsbSts,PortSc1;

	UsbSts=readw(USBSTS);
	PortSc1=readw(PORTSC);
	
	//Clear Reset bit
	PortSc1=PortSc1&0xfffffeff;
	writew(PortSc1,PORTSC);
	
}
*/
/*

/////////////////////////////////////////////////////////////////////////////
int OHCI_Intr_Threshold_Ctrl(BYTE Value)
{//check
	u32_t	UsbCmd;
	printf(("\n==================================================================="));
	UsbCmd=readw(USBCMD); 
	UsbCmd=UsbCmd | (Value <<16);   

	//0x40:8ms 	64
	//0x20:4ms 	32	
	//0x10;2ms 	16
	//0x8: 8 micro frame  	//default
	//0x4: 4 micro frame  
	//0x2; 2 micro frame 
	//0x1; 1 micro frame
	writew(UsbCmd,USBCMD);

	printf(("===================================================================\n"));
	return true;
}

*/
/*
	int OHCI_Intr_Enable(void)

	This function is to enable Root hub status change and Writeback donehead interrupt
*/

int OHCI_Intr_Enable(void)
{
	u32_t HcInterruptEnable,Hcintrdis;
	printf("inside interrupt enable\n");
	HcInterruptEnable=readw(HcINTREN(ohci_base));
//	Hcintrdis = 0x00000000;
//	writew(Hcintrdis,0x180a4014);
	printf("\nCurrent value of HcInterruptEnable: 		%x\n",HcInterruptEnable);
	// Enable interrupt due to Root hub status change and Writeback donehead
//	HcInterruptEnable=HcInterruptEnable|0x80000046;		// enabling WDH,SF and RHSC
	HcInterruptEnable=HcInterruptEnable|0x80000042;		// enabling WDH and RHSC
	writew(HcInterruptEnable,HcINTREN(ohci_base));
	Hcintrdis=0x00000000;
	writew(Hcintrdis,HcINTRDISEN(ohci_base));
	printf("ohci interrupt enabled\n");
	return true;
}

/*
	int OHCI_Intr_Disable(void)

	This function is to Disables all interrupts of OHCI
*/
int OHCI_Intr_Disable(void)
{
	u32_t Hcintrdis;

	printf("Disabling Device Interrupts\n");
	// read the Interrupt Disable register first
	Hcintrdis=readw(HcINTRDISEN(ohci_base)/*0x180a4014*/);
	
	//disable all the interrupts from ohc card, writing to Interrupt Disable register
	Hcintrdis=Hcintrdis|0xc000007f;
	writew(Hcintrdis,HcINTRDISEN(ohci_base)/*0x180a4014*/);
	printf("HcInterruptEnable                                      :%x\n",readw(HcINTREN(ohci_base)/*0x180a4010*/));
	printf("\nDisableDeviceInterrupts Completed\n");
	return true;
}

/*
int HostCtrlReset()
{
	ULONG UsbCmd,UsbSts;
	
	UsbSts=readw(USBSTS);
	while(!(UsbSts & CMD_RESET))
	{
		//HC not halted ,Set the Run/Stop bit to 0
		UsbCmd=readw(USBCMD);
		UsbCmd=UsbCmd & (~CMD_RUN);         
		writew(UsbCmd,USBCMD);
		UsbSts=readw(USBSTS);
	}
	printf(("EHC Halted Successfully"));
	
	//Apply HC Reset
	OHCI_Reset();

	return true;

}
*/
/*
	int OHCI_ResetDevice()

	This function issue port reset to the port to which the device is connected and hence device reset.
*/

int OHCI_ResetDevice()
{
	unsigned long HcRhPortStatus1,HcControlreg;
	HcRhPortStatus1=0x00000010;
	writew(HcRhPortStatus1,HcRhPortStatus(ohci_base));
	HcRhPortStatus1 = readw(HcRhPortStatus(ohci_base));
	printf("Port Reset bit is set  and waiting for reset change bit to set \n");
	while(!ResetChange)
	{
	}
	if(HcRhPortStatus1 & 0x00000002)
	{
		printf("port enabled\n");
		if(HcRhPortStatus1 & 0x00000200)
		{
			printf("Low speed device attached\n");
		}

		// enabling control list enable	as per PCI driver flow
		HcControlreg = readw(HcControl(ohci_base)/*0x180a4004*/);
		HcControlreg = HcControlreg | 0x00000010;			// enabling only the control list after device reset and enable bit is checked
		writew(HcControlreg,HcControl(ohci_base));
//		printf("the value of HcHcca.frame number is 	   :%x\n",HcHcca->frame_no);
//		printf("the value fo hc control register is %x\n",readw(0x180a4004));	
	}
	else
	{
		printf("port is disabled\n");
	}
	MpsKnown = 0;
	ohci_status.DevAddress = 0;
	printf("HcRhPortStatus                             :%x\n",readw(HcRhPortStatus(ohci_base)));
//	printf("the value of HcHcca.frame number is 	   :%x\n",HcHcca->frame_no);
	return TRUE;
}

/* ******************************************************************************** *   
Function Name : int SetAddress_OHCI(BYTE DevAddress)
Description   : Set Device  Address 
Params        :  Device Address
Returns       : True:Set Device Address Success  / False: Set Device Address Fail    
Note          :  
******************************************************************************** */
int SetAddress_OHCI(BYTE DevAddress)
{
//	KdPrint(("Inside SetAddress"));
	PED pED0;
	PGENTD pTD1,pTD2,pTD3;
	PUCHAR  Cbp;
	PUCHAR Bend;
	ULONG  PhyspTD1,PhyspTD2,PhyspTD3,PhyspED0,PhysCbp,PhysBend;
	ULONG  HcCommandStatus;//,HcInterruptStatusreg,HcDoneHeadreg,HCCADoneHead;
//	ohci_status.DevAddress = 0;
//	printf(("ENTERED IN TO THE SET ADDRESS\n"));

//	OHCI_ControlList_Enable(0);		// commented as per pci driver flow
		
	// Allocate Buffer for PhysCbp
	Cbp=(PUCHAR)SETUP_BUFF_ADDR_OHCI;//dx->VirAddr+32;
	PhysCbp=((u32_t)(Cbp+0));
	
	//For the 8 bytes of setup stage
	*(Cbp+0)=0x00;
	*(Cbp+1)=USB_REQ_SET_ADDRESS;
	*(Cbp+2)=0x0 |DevAddress;//Get Address from Application.
	*(Cbp+3)=0x00;
	*(Cbp+4)=0x00;
	*(Cbp+5)=0x00;
	*(Cbp+6)=0x00;
	*(Cbp+7)=0x00;
	Bend=(PUCHAR)(Cbp+7);
	PhysBend=(ULONG)((PUCHAR)PhysCbp+7);
	printf("\nCbp Physical Address: %x",PhysCbp);
	printf("\nCbp Virtual Address: %x",Cbp);
	printf("\nPhysBend Physical Address: %x",PhysBend);
	printf("\nBend Virtual Address: %x",Bend);

	//Initialize the next Null TD 
	pTD3 = Get_Free_GTD();
	PhyspTD3=((u32_t) pTD3);

	printf("\npTD3 Physical Address: %x",PhyspTD3);
	printf("\npTD3 Virtual Address: %x",&pTD3);

	pTD3->TDdata=0xF0000000;	// changed from 0x00000000 to 0xF0000000
	pTD3->Cbp=NULL;
	pTD3->NextTD=NULL;
	pTD3->Bend=NULL;

	//Initialize the next TD for SetAddress status stage
	pTD2=Get_Free_GTD();
	PhyspTD2=((u32_t) pTD2);

	printf("\npTD2 Physical Address: %x",PhyspTD2);
	printf("\npTD2 Virtual Address: %x",pTD2);

	pTD2->TDdata=0xf3100000;
	pTD2->Cbp=NULL;
	pTD2->NextTD=PhyspTD3;
//	pTD2->NextTD=pTD3;
	pTD2->Bend=NULL;

	//Initialize the first TD for SetAddress setup stage
	pTD1=Get_Free_GTD();
	PhyspTD1=((u32_t) pTD1);

	printf("\npTD1 Physical Address: %x",PhyspTD1);
	printf("\npTD1 Virtual Address: %x",pTD1);

	pTD1->TDdata=0xf2000000;
	pTD1->Cbp=PhysCbp;
//	pTD1->Cbp=Cbp;
	pTD1->NextTD=PhyspTD2;
//	pTD1->NextTD=pTD2;
	pTD1->Bend=PhysBend;
//	pTD1->Bend=Bend;

	// Form the first control ED in 
	pED0=Get_Free_ED();
	PhyspED0=((u32_t) pED0);
	
	printf("\nED0 Physical Address: %x",PhyspED0);
	printf("\nED0 Virtual Address: %x",pED0);

	//Initialize this first ED
	pED0->EDdata=0x00080000|(ohci_status.DevAddress);			// chnaged from 0x00080000 to 0x00082000 for ls testing
	pED0->TailP=PhyspTD3; 
	pED0->HeadP=PhyspTD1;
	pED0->NextED=NULL; //PhyspED1.LowPart;

	printf("before enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);

	// Write to Control Head ED register the Physical Address of the first ED
	writew(PhyspED0,HcCtrlHeadED(ohci_base));
//	printf("\nControl Head Initialized to		: %x\n",readw(0x180a4020));

	// Write to HcCommandStatus to tell control list filled
	HcCommandStatus=readw(HcCMDSTS(ohci_base));
	HcCommandStatus=HcCommandStatus|0x00000002;			//only control list filled bit is set in hccommandstatus register & bulk list filled
	printf("the value of HcCommandStatus is 		%x\n",HcCommandStatus);
	writew(HcCommandStatus,HcCMDSTS(ohci_base));
	printf("the value of hccommand status register%x\n",readw(HcCMDSTS(ohci_base)));

	// enabling control list enable
	OHCI_ControlList_Enable(1);
	printf("after enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
	
	HcCommandStatus=readw(HcCMDSTS(ohci_base)/*0x180a4008*/);
//	printf("HcHCCA                                                    :%x\n",readw(0x180a4018));
//	printf("the value of HcHcca.frame number is 	   :%x\n",HcHcca->frame_no);
	printf("HcPeriodCurrentEd                                     :%x\n",readw(HcPeriodCurrED(ohci_base)/*0x180a401C*/));
//	printf("HcControlHeadEd                                       :%x\n",readw(0x180a4020));
//	printf("HcControlCurrentEd                                    :%x\n",readw(0x180a4024));
	printf(("wait for transaction done interrupt\n"));
//	while(!(readw(0x180a400C) & 0x00000002))		// checking for the completion fo the qtd
	while(!TransferComplete)
	{
//		printf("the value of HcHcca.frame number is 				 :%x\n",HcHcca->frame_no);
//		printf("HcControl									  :%x\n",readw(0x180a4004));
//		printf("HcCommandStatus 								 :%x\n",readw(0x180a4008));
//		printf("HcInterruptStatus								 :%x\n",readw(0x180a400C));
//		printf("HcHCCA										 :%x\n",readw(0x180a4018));
//		printf("HcControlHeadEd 								 :%x\n",readw(0x180a4020));
//		printf("HcControlCurrentEd								 :%x\n",readw(0x180a4024));
//		printf("HcFmRemaining									:%x\n",readw(0x180a4038));
//		printf("HcFmNumber									:%x\n",readw(0x180a403C));
//		printf("HcRhPortStatus									 :%x\n",readw(0x180a4054));
		printf("\n");
	}
	TransferComplete = 0;
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
	printf("pTD2->TDdata 			:%x\n",pTD2->TDdata);
	printf("pTD2->Cbp 			:%x\n",pTD2->Cbp);
	printf("pTD2->NextTD 			:%x\n",pTD2->NextTD);
	printf("pTD2->Bend 			:%x\n",pTD2->Bend);
	printf("pTD1->TDdata 			:%x\n",pTD1->TDdata);
	printf("pTD1->Cbp 			:%x\n",pTD1->Cbp);
	printf("pTD1->NextTD 			:%x\n",pTD1->NextTD);
	printf("pTD1->Bend 			:%x\n",pTD1->Bend);
//	HcInterruptStatusreg =readw(0x180a400C);
//	writew(HcInterruptStatusreg,0x180a400C);
//	HcDoneHeadreg = readw(0x180a4030);
//	HCCADoneHead = *((unsigned long *)HCCA + 0x21);
//	printf("the value of HcDoneHead char is %x\n",HcDoneHeadreg);
//	printf("the value of HCCADoneHead is		:%x\n",HcHcca->done_head);
//	printf("the value of HCCADoneHead char is %x\n",HCCADoneHead);
	if(HcHcca->done_head == PhyspTD2) 	// checking the status of the completion
	{
		if(!(pTD2->TDdata & 0xF0000000))
		{
			printf("the value of pTD3->TDdata is %x",pTD3->TDdata);
			printf("the value of pTD2->TDdata is %x",pTD2->TDdata);
			ohci_status.DevAddress=DevAddress;
			printf("the value of the device address is %x\n",ohci_status.DevAddress);
			printf("Set Address Passed in Driver");
		}
		else 
		{
			printf("setaddress failed in driver\n");
			printf("the value of pTD2->TDdata is %x",pTD2->TDdata);
		}
	}
	else
	{
		printf("status td not completed yet\n");
	}
	Add_Free_ED_Ring(pED0);
	Add_Free_GTD_Ring(pTD1);		
	Add_Free_GTD_Ring(pTD2);
	Add_Free_GTD_Ring(pTD3);		
	
	return true;
}

/* ******************************************************************************** *   
Function Name : int GetDescriptor_OHCI(BYTE DevDescType,BYTE DevDescIndex,int DevDescLength)
Description   : Get Device Descriptor 
Params        : Descriptor Type,Descriptor Index,Descriptor Length
Returns       : True:Get Device Descriptor Success  / False: Get Device Descriptor Fail    
Note          :  
******************************************************************************** */
int GetDescriptor_OHCI(BYTE DevDescType,BYTE DevDescIndex,int DevDescLength)
{
	unsigned int i;
	PED pED0;
	PGENTD pTD1,pTD2,pTD3,pTD4;
	PUCHAR	Buff;
	PUCHAR  Cbp;
	PUCHAR Bend,BuffEnd;
	ULONG  PhyspTD1,PhyspTD2,PhyspTD3,PhyspTD4,PhyspED0,PhysCbp,PhysBend;
	ULONG  PhyBuff,PhyBuffEnd,IntialInStartBuffAddr,TotalBytesTransfered;	
	ULONG  HcCommandStatus;//,HcInterruptStatusreg,HcDoneHeadreg,HCCADoneHead;
	//ohci_status.DevAddress = 0;
	printf(("ENTERED IN TO THE GetDescriptor_OHCI\n"));

	OHCI_ControlList_Enable(0);
		
	// Allocate Buffer for PhysCbp
	Cbp=(PUCHAR)SETUP_BUFF_ADDR_OHCI;//dx->VirAddr+32;
	PhysCbp=((u32_t)(Cbp+0));
	Buff = (PUCHAR)SETUP_BUFF_ADDR_OHCI+20;
	PhyBuff = ((u32_t)(Buff+0));
		
	//For the 8 bytes of setup stage
	*(Cbp+0)=0x80;
	*(Cbp+1)=USB_REQ_GET_DESCRIPTOR;
	*(Cbp+2)=0x00;
	*(Cbp+3)=DevDescType;
	*(Cbp+4)=0x00;
	*(Cbp+5)=0x00;
	*(Cbp+6)=DevDescLength;
	*(Cbp+7)=DevDescLength >> 8;

	Bend=(PUCHAR)(Cbp+7);
	PhysBend=(ULONG)((PUCHAR)PhysCbp+7);
	BuffEnd = (PUCHAR)(Buff + 63);
//	PhyBuffEnd = ((u32_t)(BuffEnd+0));
//	BuffEnd = (PUCHAR)(Buff + (DevDescLength-1));
	PhyBuffEnd = ((u32_t)(BuffEnd+0));

	pTD4 = Get_Free_GTD();
	PhyspTD4=((u32_t) pTD4);
	printf("\npTD3 Physical Address: %x",PhyspTD4);
	printf("\npTD3 Virtual Address: %x",pTD4);
	pTD4->TDdata=NULL;
	pTD4->Cbp=NULL;
	pTD4->NextTD=NULL;
	pTD4->Bend=NULL;
	//Initialize the next Null TD 
	pTD3 = Get_Free_GTD();
	PhyspTD3=((u32_t) pTD3);
	printf("\npTD3 Physical Address: %x",PhyspTD3);
	printf("\npTD3 Virtual Address: %x",pTD3);
	pTD3->TDdata=0xF3080000;
	pTD3->Cbp=NULL;
	pTD3->NextTD=PhyspTD4;
	pTD3->Bend=NULL;
	//Initialize the next TD for SetAddress status stage
	pTD2=Get_Free_GTD();
	//Get the Physical address for this ED
	PhyspTD2=((u32_t) pTD2);
	printf("\npTD2 Physical Address: %x",PhyspTD2);
	printf("\npTD2 Virtual Address: %x",pTD2);
	pTD2->TDdata=0xf3140000;		// Buffer rounding bit is also set.
	pTD2->Cbp=PhyBuff;
	pTD2->NextTD=PhyspTD3;
	pTD2->Bend=PhyBuffEnd;
	IntialInStartBuffAddr = PhyBuff;
	//Initialize the first TD for SetAddress setup stage
	pTD1=Get_Free_GTD();
	//Get the Physical address for this TD
	PhyspTD1=((u32_t) pTD1);
	printf("\npTD1 Physical Address: %x",PhyspTD1);
	printf("\npTD1 Virtual Address: %x",pTD1);
	pTD1->TDdata=0xf2000000;
	pTD1->Cbp=PhysCbp;
	pTD1->NextTD=PhyspTD2;
	pTD1->Bend=PhysBend;

	pED0=Get_Free_ED();
	PhyspED0=((u32_t) pED0);
	printf("\nED0 Physical Address: %x",PhyspED0);
	printf("\nED0 Virtual Address: %x",pED0);
	//Initialize this first ED
	if(MpsKnown == 0)
	{
		pED0->EDdata=0x00400000|(ohci_status.DevAddress);			//0x00080000 changed to 0x00400000
		printf("MPsunknown\n");
	}
	else
	{
		printf("the value of devmps is %x\n",ohci_status.DevMPS);
		pED0->EDdata=((0x00000000|ohci_status.DevMPS) << 16) |(ohci_status.DevAddress);			//0x00080000 changed to 0x00400000
	}
	pED0->TailP=PhyspTD4; 
	pED0->HeadP=PhyspTD1;
	pED0->NextED=NULL; //PhyspED1.LowPart;

	printf("before enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
	// Write to Control Head ED register the Physical Address of the first ED
	writew(PhyspED0,HcCtrlHeadED(ohci_base));
//	printf("\nControl Head Initialized to: %x",readw(0x180a4020));

	// Write to HcCommandStatus to tell control list filled
	HcCommandStatus=readw(HcCMDSTS(ohci_base));
	HcCommandStatus=HcCommandStatus|0x00000002; 		//only control list filled bit is set in hccommandstatus register & bulk list filled
	printf("the value of HcCommandStatus is 		%x\n",HcCommandStatus);
	writew(HcCommandStatus,HcCMDSTS(ohci_base));
	printf("the value of hccommand status register%x\n",readw(HcCMDSTS(ohci_base)));

	// enabling control list enable
	OHCI_ControlList_Enable(1);
//	printf("after enable\n");
//	printf("pED0->EDdata			:%x\n",pED0->EDdata);
//	printf("pED0->TailP 			:%x\n",pED0->TailP);
//	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
//	printf("pED0->NextED			:%x\n",pED0->NextED);
	HcCommandStatus=readw(HcCMDSTS(ohci_base)/*0x180a4008*/);
//	printf("HcHCCA							:%x\n",readw(0x180a4018));
//	printf("the value of HcHcca.frame number is 	:%x\n",HcHcca->frame_no);
	printf("HcPeriodCurrentEd					:%x\n",readw(HcPeriodCurrED(ohci_base)/*0x180a401C*/));
//	printf("HcControlHeadEd 					:%x\n",readw(0x180a4020));
//	printf("HcControlCurrentEd					:%x\n",readw(0x180a4024));
	printf(("wait for transaction done interrupt\n"));
	while(!TransferComplete)
	{
//		printf("the value of HcHcca.frame number is 				 :%x\n",HcHcca->frame_no);
//		printf("HcControl									  :%x\n",readw(0x180a4004));
//		printf("HcCommandStatus 								 :%x\n",readw(0x180a4008));
//		printf("HcInterruptStatus								 :%x\n",readw(0x180a400C));
//		printf("HcHCCA										 :%x\n",readw(0x180a4018));
//		printf("HcControlHeadEd 								 :%x\n",readw(0x180a4020));
//		printf("HcControlCurrentEd								 :%x\n",readw(0x180a4024));
//		printf("HcFmRemaining									:%x\n",readw(0x180a4038));
//		printf("HcFmNumber									:%x\n",readw(0x180a403C));
//		printf("HcRhPortStatus									 :%x\n",readw(0x180a4054));
		printf("\n");
	}
	TransferComplete = 0;
	printf("pED0->EDdata			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED			:%x\n",pED0->NextED);
//	HcInterruptStatusreg =readw(0x180a400C);
//	writew(HcInterruptStatusreg,0x180a400C);
//	HcDoneHeadreg = readw(0x180a4030);
//	printf("the value of HcDoneHead char is 	:%x\n",HcDoneHeadreg);
//	printf("the value of HCCADoneHead is		:%x\n",HcHcca->done_head);
	if(HcHcca->done_head == PhyspTD3)	// checking the status of the completion
	{
		printf("the value of pTD3->TDdata is (Status)		:%x\n",pTD3->TDdata);
		printf("the value of pTD2->TDdata is (Data)	 	:%x\n",pTD2->TDdata);
		printf("the value of pTD1->TDdata is (Setup)		:%x\n",pTD1->TDdata);
		if(!(pTD3->TDdata & 0xF0000000))
		{
			printf("Get Descriptor Passed in Driver\n");
			printf("pTD2->TDdata 	:%x\n",pTD2->TDdata);
			printf("pTD2->Cbp 	:%x\n",pTD2->Cbp);
			printf("pTD2->NextTD 	:%x\n",pTD2->NextTD);
			printf("pTD2->Bend 	:%x\n",pTD2->Bend);
			TotalBytesTransfered = (pTD2->Cbp - IntialInStartBuffAddr);
			for(i=0;i<TotalBytesTransfered;i++)
			{
				printf("the value of the dscr byte  is	:%x\n",(*(Buff+ i)));
			}
			if(DevDescType == 1)
			{
				ohci_status.DevMPS = *(Buff + 7);
				MpsKnown = 1;
				printf("the value of control endpoint mps is %x\n",ohci_status.DevMPS);
			}
			printf("the value of device control endpoint max pak size is %x\n",ohci_status.DevMPS);
			if(DevDescType == 2 && DevDescLength > 0x09)
			{
				printf("Full configuration descriptor\n");
				
			}
			
		}
		else 
		{
			printf("Get Descriptor failed \n");
		}
	}
	else 
	{
		printf("status td not completed yet\n");
	}
	Add_Free_ED_Ring(pED0);
	Add_Free_GTD_Ring(pTD1);		
	Add_Free_GTD_Ring(pTD2);
	Add_Free_GTD_Ring(pTD3);		
	Add_Free_GTD_Ring(pTD4);		
	return true;
}

/* ******************************************************************************** *   
Function Name : int GetConfiguration_OHCI()
Description   : Get Current Device Configuration value 
Params        : NILL
Returns       : True:GetConfiguration Success  / False: GetConfiguration Fail    
Note          :  
******************************************************************************** */
int GetConfiguration_OHCI()
{
	unsigned char i;
	PED pED0;
	PGENTD pTD1,pTD2,pTD3,pTD4;
	PUCHAR	Buff;
	PUCHAR  Cbp;
	PUCHAR Bend,BuffEnd;
	ULONG  PhyspTD1,PhyspTD2,PhyspTD3,PhyspTD4,PhyspED0,PhysCbp,PhysBend;
	ULONG  PhyBuff,PhyBuffEnd,IntialInStartBuffAddr,TotalBytesTransfered;	
	ULONG  HcCommandStatus;//,HcInterruptStatusreg,HcDoneHeadreg;
	//ohci_status.DevAddress = 0;
	printf(("ENTERED IN TO THE GetConfiguration_OHCI\n"));

	OHCI_ControlList_Enable(0);
		
	// Allocate Buffer for PhysCbp
	Cbp=(PUCHAR)SETUP_BUFF_ADDR_OHCI;//dx->VirAddr+32;
	PhysCbp=((u32_t)(Cbp+0));
	Buff = (PUCHAR)SETUP_BUFF_ADDR_OHCI+0x100;
	PhyBuff = ((u32_t)(Buff+0));
		
	//For the 8 bytes of setup stage
	*(Cbp+0)=0x80;
	*(Cbp+1)=USB_REQ_GET_CONFIGURATION;
	*(Cbp+2)=0x00;
	*(Cbp+3)=0x00;
	*(Cbp+4)=0x00;
	*(Cbp+5)=0x00;
	*(Cbp+6)=0x01;
	*(Cbp+7)=0x00;

	Bend=(PUCHAR)(Cbp+7);
	PhysBend=(ULONG)((PUCHAR)PhysCbp+7);
	BuffEnd = (PUCHAR)(Buff + 0);
	PhyBuffEnd = ((u32_t)(BuffEnd+0));

	pTD4 = Get_Free_GTD();
	PhyspTD4=((u32_t) pTD3);
	printf("\npTD3 Physical Address: %x",PhyspTD4);
	printf("\npTD3 Virtual Address: %x",pTD4);
	pTD4->TDdata=NULL;
	pTD4->Cbp=NULL;
	pTD4->NextTD=NULL;
	pTD4->Bend=NULL;
	//Initialize the next Null TD 
	pTD3 = Get_Free_GTD();
	PhyspTD3=((u32_t) pTD3);
	printf("\npTD3 Physical Address: %x",PhyspTD3);
	printf("\npTD3 Virtual Address: %x",pTD3);
	pTD3->TDdata=0xF3080000;
	pTD3->Cbp=NULL;
	pTD3->NextTD=PhyspTD4;
	pTD3->Bend=NULL;
	//Initialize the next TD for SetAddress status stage
	pTD2=Get_Free_GTD();
	//Get the Physical address for this ED
	PhyspTD2=((u32_t) pTD2);
	printf("\npTD2 Physical Address: %x",PhyspTD2);
	printf("\npTD2 Virtual Address: %x",pTD2);
	pTD2->TDdata=0xf3140000;		// Buffer rounding bit is also set.
	pTD2->Cbp=PhyBuff;
	pTD2->NextTD=PhyspTD3;
	pTD2->Bend=PhyBuffEnd;
	IntialInStartBuffAddr = PhyBuff;
	//Initialize the first TD for SetAddress setup stage
	pTD1=Get_Free_GTD();
	//Get the Physical address for this TD
	PhyspTD1=((u32_t) pTD1);
	printf("\npTD1 Physical Address: %x",PhyspTD1);
	printf("\npTD1 Virtual Address: %x",pTD1);
	pTD1->TDdata=0xf2000000;
	pTD1->Cbp=PhysCbp;
	pTD1->NextTD=PhyspTD2;
	pTD1->Bend=PhysBend;

	pED0=Get_Free_ED();
	PhyspED0=((u32_t) pED0);
	printf("\nED0 Physical Address: %x",PhyspED0);
	printf("\nED0 Virtual Address: %x",pED0);
	//Initialize this first ED
	pED0->EDdata=0x00000000|ohci_status.DevMPS << 16 |(ohci_status.DevAddress);			//0x00080000 changed to 0x00400000
	pED0->TailP=PhyspTD4; 
	pED0->HeadP=PhyspTD1;
	pED0->NextED=NULL; //PhyspED1.LowPart;

	// Write to Control Head ED register the Physical Address of the first ED
	writew(PhyspED0,HcCtrlHeadED(ohci_base)/*0x180a4020*/);
//	printf("\nControl Head Initialized to: %x",readw(0x180a4020));

	// Write to HcCommandStatus to tell control list filled
	HcCommandStatus=readw(HcCMDSTS(ohci_base));
	HcCommandStatus=HcCommandStatus|0x00000002; 		//only control list filled bit is set in hccommandstatus register & bulk list filled
	printf("the value of HcCommandStatus is 		%x\n",HcCommandStatus);
	writew(HcCommandStatus,HcCMDSTS(ohci_base));
	printf("the value of hccommand status register%x\n",readw(HcCMDSTS(ohci_base)));

	// enabling control list enable
	OHCI_ControlList_Enable(1);
	printf("after enable\n");
	printf("pED0->EDdata			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED			:%x\n",pED0->NextED);
	HcCommandStatus=readw(HcCMDSTS(ohci_base));
//	printf("HcHCCA							:%x\n",readw(0x180a4018));
//	printf("the value of HcHcca.frame number is 	:%x\n",HcHcca->frame_no);
	printf("HcPeriodCurrentEd					:%x\n",readw(HcPeriodCurrED(ohci_base)));
//	printf("HcControlHeadEd 					:%x\n",readw(0x180a4020));
//	printf("HcControlCurrentEd					:%x\n",readw(0x180a4024));
	printf(("wait for transaction done interrupt\n"));
	while(!TransferComplete)
	{
//		printf("the value of HcHcca.frame number is 				 :%x\n",HcHcca->frame_no);
//		printf("HcControl									  :%x\n",readw(0x180a4004));
//		printf("HcCommandStatus 								 :%x\n",readw(0x180a4008));
//		printf("HcInterruptStatus								 :%x\n",readw(0x180a400C));
//		printf("HcHCCA										 :%x\n",readw(0x180a4018));
//		printf("HcControlHeadEd 								 :%x\n",readw(0x180a4020));
//		printf("HcControlCurrentEd								 :%x\n",readw(0x180a4024));
//		printf("HcFmRemaining									:%x\n",readw(0x180a4038));
//		printf("HcFmNumber									:%x\n",readw(0x180a403C));
//		printf("HcRhPortStatus									 :%x\n",readw(0x180a4054));
		printf("\n");
	}
	TransferComplete = 0;
	printf("pED0->EDdata			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED			:%x\n",pED0->NextED);
//	HcInterruptStatusreg =readw(0x180a400C);
//	writew(HcInterruptStatusreg,0x180a400C);
//	HcDoneHeadreg = readw(0x180a4030);
//	printf("the value of HcDoneHead char is 	:%x\n",HcDoneHeadreg);
//	printf("the value of HCCADoneHead is		:%x\n",HcHcca->done_head);
	if(HcHcca->done_head == PhyspTD3)	// checking the status of the completion
	{
		printf("the value of pTD3->TDdata is (Status)		:%x\n",pTD3->TDdata);
		printf("the value of pTD2->TDdata is (Data)	 	:%x\n",pTD2->TDdata);
		printf("the value of pTD1->TDdata is (Setup)		:%x\n",pTD1->TDdata);
		if(!(pTD3->TDdata & 0xF0000000))
		{
			printf("Get Configuration Passed in Driver\n");
			TotalBytesTransfered = (pTD2->Cbp - IntialInStartBuffAddr);
			for(i=0;i<=TotalBytesTransfered;i++)
			{
				printf("the value of the dscr byte  is	:%x\n",(*(Buff+ i)));
			}
		}
		else 
		{
			printf("Get Configuration  failed \n");
		}
	}
	else 
	{
		printf("status td not completed yet\n");
	}
	Add_Free_ED_Ring(pED0);
	Add_Free_GTD_Ring(pTD1);		
	Add_Free_GTD_Ring(pTD2);
	Add_Free_GTD_Ring(pTD3);		
	Add_Free_GTD_Ring(pTD4);		
	return true;
}

/* ******************************************************************************** *   
Function Name : int SetConfiguration_OHCI(BYTE DevConfig)
Description   : Setting the device to a configured state 
Params        : Configuration value
Returns       : True:Setconfiguration Success  / False: SetConfiguration Fail    
Note          :  
******************************************************************************** */
int SetConfiguration_OHCI(BYTE DevConfig)
{
	PED pED0;
	PGENTD pTD1,pTD2,pTD3;
	PUCHAR  Cbp;
	PUCHAR Bend;
	ULONG  PhyspTD1,PhyspTD2,PhyspTD3,PhyspED0,PhysCbp,PhysBend;
	ULONG  HcCommandStatus;//,HcInterruptStatusreg,HcDoneHeadreg;
//	ohci_status.DevAddress = 0;
	printf(("ENTERED IN TO THE SET CONFIGURATION\n"));

	OHCI_ControlList_Enable(0);		// commented as per pci driver flow
		
	// Allocate Buffer for PhysCbp
	Cbp=(PUCHAR)SETUP_BUFF_ADDR_OHCI;//dx->VirAddr+32;
	PhysCbp=((u32_t)(Cbp+0));
	
	//For the 8 bytes of setup stage
	*(Cbp+0)=0x00;
	*(Cbp+1)=USB_REQ_SET_CONFIGURATION;
	*(Cbp+2)=0x0 |DevConfig;//Get Address from Application.
	*(Cbp+3)=0x00;
	*(Cbp+4)=0x00;
	*(Cbp+5)=0x00;
	*(Cbp+6)=0x00;
	*(Cbp+7)=0x00;
	Bend=(PUCHAR)(Cbp+7);
	PhysBend=(ULONG)((PUCHAR)PhysCbp+7);
	printf("\nCbp Physical Address: %x",PhysCbp);
	printf("\nCbp Virtual Address: %x",Cbp);
	printf("\nPhysBend Physical Address: %x",PhysBend);
	printf("\nBend Virtual Address: %x",Bend);

	//Initialize the next Null TD 
	pTD3 = Get_Free_GTD();
	PhyspTD3=((u32_t) pTD3);

	printf("\npTD3 Physical Address: %x",PhyspTD3);
	printf("\npTD3 Virtual Address: %x",&pTD3);

	pTD3->TDdata=0xF0000000;	// changed from 0x00000000 to 0xF0000000
	pTD3->Cbp=NULL;
	pTD3->NextTD=NULL;
	pTD3->Bend=NULL;

	//Initialize the next TD for SetAddress status stage
	pTD2=Get_Free_GTD();
	PhyspTD2=((u32_t) pTD2);

	printf("\npTD2 Physical Address: %x",PhyspTD2);
	printf("\npTD2 Virtual Address: %x",pTD2);

	pTD2->TDdata=0xf3100000;
	pTD2->Cbp=NULL;
	pTD2->NextTD=PhyspTD3;
	pTD2->Bend=NULL;

	//Initialize the first TD for SetAddress setup stage
	pTD1=Get_Free_GTD();
	PhyspTD1=((u32_t) pTD1);

	printf("\npTD1 Physical Address: %x",PhyspTD1);
	printf("\npTD1 Virtual Address: %x",pTD1);

	pTD1->TDdata=0xf2000000;
	pTD1->Cbp=PhysCbp;
	pTD1->NextTD=PhyspTD2;
	pTD1->Bend=PhysBend;

	// Form the first control ED in 
	pED0=Get_Free_ED();
	PhyspED0=((u32_t) pED0);
	
	printf("\nED0 Physical Address: %x",PhyspED0);
	printf("\nED0 Virtual Address: %x",pED0);

	//Initialize this first ED
	pED0->EDdata=0x00000000| (ohci_status.DevMPS << 16) |(ohci_status.DevAddress);	// 
	printf("the value of ped0->eddata is %x",pED0->EDdata);
	printf("the value of devaddress is %x\n",ohci_status.DevAddress);
	pED0->TailP=PhyspTD3; 
	pED0->HeadP=PhyspTD1;
	pED0->NextED=NULL; //PhyspED1.LowPart;

	printf("before enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);

	// Write to Control Head ED register the Physical Address of the first ED
	writew(PhyspED0,HcCtrlHeadED(ohci_base));
//	printf("\nControl Head Initialized to		: %x\n",readw(0x180a4020));

	// Write to HcCommandStatus to tell control list filled
	HcCommandStatus=readw(HcCMDSTS(ohci_base));
	HcCommandStatus=HcCommandStatus|0x00000002;			//only control list filled bit is set in hccommandstatus register & bulk list filled
	printf("the value of HcCommandStatus is 		%x\n",HcCommandStatus);
	writew(HcCommandStatus,HcCMDSTS(ohci_base));
	printf("the value of hccommand status register%x\n",readw(HcCMDSTS(ohci_base)));

	// enabling control list enable
	OHCI_ControlList_Enable(1);
	printf("after enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
	
	HcCommandStatus=readw(HcCMDSTS(ohci_base)/*0x180a4008*/);
//	printf("HcHCCA                                                    :%x\n",readw(0x180a4018));
//	printf("the value of HcHcca.frame number is 	   :%x\n",HcHcca->frame_no);
	printf("HcPeriodCurrentEd                                     :%x\n",readw(HcPeriodCurrED(ohci_base)/*0x180a401C*/));
//	printf("HcControlHeadEd                                       :%x\n",readw(0x180a4020));
//	printf("HcControlCurrentEd                                    :%x\n",readw(0x180a4024));
	printf(("wait for transaction done interrupt\n"));
	while(!TransferComplete)
	{
//		printf("the value of HcHcca.frame number is 				 :%x\n",HcHcca->frame_no);
//		printf("HcControl									  :%x\n",readw(0x180a4004));
//		printf("HcCommandStatus 								 :%x\n",readw(0x180a4008));
//		printf("HcInterruptStatus								 :%x\n",readw(0x180a400C));
//		printf("HcHCCA										 :%x\n",readw(0x180a4018));
//		printf("HcControlHeadEd 								 :%x\n",readw(0x180a4020));
//		printf("HcControlCurrentEd								 :%x\n",readw(0x180a4024));
//		printf("HcFmRemaining									:%x\n",readw(0x180a4038));
//		printf("HcFmNumber									:%x\n",readw(0x180a403C));
//		printf("HcRhPortStatus									 :%x\n",readw(0x180a4054));
		printf("\n");
	}
	TransferComplete = 0;
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
//	HcInterruptStatusreg =readw(0x180a400C);
//	writew(HcInterruptStatusreg,0x180a400C);
//	HcDoneHeadreg = readw(0x180a4030);
//	printf("the value of HcDoneHead char is %x\n",HcDoneHeadreg);
//	printf("the value of HCCADoneHead is		:%x\n",HcHcca->done_head);
	if(HcHcca->done_head == PhyspTD2) 	// checking the status of the completion
	{
		if(!(pTD2->TDdata & 0xF0000000))
		{
			printf("the value of pTD2->TDdata is %x\n",pTD2->TDdata);
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
//			ohci_status.DevAddress=DevAddress;
			printf("Set configuration Passed in Driver");
		}
		else 
		{
			printf("set configuration failed \n");
			printf("the value of pTD2->TDdata is %x\n",pTD2->TDdata);
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
		}
	}
	else
	{
		printf("status td not completed yet\n");
	}
	Add_Free_ED_Ring(pED0);
	Add_Free_GTD_Ring(pTD1);		
	Add_Free_GTD_Ring(pTD2);
	Add_Free_GTD_Ring(pTD3);		
	
	return true;
}

/* ******************************************************************************** *   
Function Name : int SetFeature_OHCI(BYTE DevRequest,BYTE DevFeature,BYTE DevDescIndex,BYTE EpDir)
Description   : selecting or enabling a certain feature of the device
Params        : bmRequest value,Feature Selector,Endpoint number(DevDescIndex),Endpoint Direction
Returns       : True:SetFeature_OHCI Success  / False: SetFeature_OHCI Fail    
Note          :  
******************************************************************************** */
int SetFeature_OHCI(BYTE DevRequest,BYTE DevFeature,BYTE DevDescIndex,BYTE EpDir)
{
	PED pED0;
	PGENTD pTD1,pTD2,pTD3;
	PUCHAR  Cbp;
	PUCHAR Bend;
	ULONG  PhyspTD1,PhyspTD2,PhyspTD3,PhyspED0,PhysCbp,PhysBend;
	ULONG  HcCommandStatus;//,HcInterruptStatusreg,HcDoneHeadreg;
//	ohci_status.DevAddress = 0;
	printf(("ENTERED IN TO THE SET ADDRESS\n"));

	OHCI_ControlList_Enable(0);		// commented as per pci driver flow
		
	// Allocate Buffer for PhysCbp
	Cbp=(PUCHAR)SETUP_BUFF_ADDR_OHCI;//dx->VirAddr+32;
	PhysCbp=((u32_t)(Cbp+0));
	
	//For the 8 bytes of setup stage
	*(Cbp+0)=0x00 | DevRequest;
	*(Cbp+1)=USB_REQ_SET_FEATURE;
	*(Cbp+2)=0x0 |DevFeature;//Get Address from Application.
	*(Cbp+3)=0x00;
	*(Cbp+4)=0x00| DevDescIndex |EpDir;
	*(Cbp+5)=0x00 ;
	*(Cbp+6)=0x00;
	*(Cbp+7)=0x00;
	Bend=(PUCHAR)(Cbp+7);
	PhysBend=(ULONG)((PUCHAR)PhysCbp+7);
	printf("\nCbp Physical Address: %x",PhysCbp);
	printf("\nCbp Virtual Address: %x",Cbp);
	printf("\nPhysBend Physical Address: %x",PhysBend);
	printf("\nBend Virtual Address: %x",Bend);

	//Initialize the next Null TD 
	pTD3 = Get_Free_GTD();
	PhyspTD3=((u32_t) pTD3);

	printf("\npTD3 Physical Address: %x",PhyspTD3);
	printf("\npTD3 Virtual Address: %x",&pTD3);

	pTD3->TDdata=0xF0000000;	// changed from 0x00000000 to 0xF0000000
	pTD3->Cbp=NULL;
	pTD3->NextTD=NULL;
	pTD3->Bend=NULL;

	//Initialize the next TD for SetAddress status stage
	pTD2=Get_Free_GTD();
	PhyspTD2=((u32_t) pTD2);

	printf("\npTD2 Physical Address: %x",PhyspTD2);
	printf("\npTD2 Virtual Address: %x",pTD2);

	pTD2->TDdata=0xf3100000;
	pTD2->Cbp=NULL;
	pTD2->NextTD=PhyspTD3;
	pTD2->Bend=NULL;

	//Initialize the first TD for SetAddress setup stage
	pTD1=Get_Free_GTD();
	PhyspTD1=((u32_t) pTD1);

	printf("\npTD1 Physical Address: %x",PhyspTD1);
	printf("\npTD1 Virtual Address: %x",pTD1);

	pTD1->TDdata=0xf2000000;
	pTD1->Cbp=PhysCbp;
	pTD1->NextTD=PhyspTD2;
	pTD1->Bend=PhysBend;

	// Form the first control ED in 
	pED0=Get_Free_ED();
	PhyspED0=((u32_t) pED0);
	
	printf("\nED0 Physical Address: %x",PhyspED0);
	printf("\nED0 Virtual Address: %x",pED0);

	//Initialize this first ED
	pED0->EDdata=0x00000000| ohci_status.DevMPS << 16 |(ohci_status.DevAddress);
	pED0->TailP=PhyspTD3; 
	pED0->HeadP=PhyspTD1;
	pED0->NextED=NULL; //PhyspED1.LowPart;

	printf("before enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);

	// Write to Control Head ED register the Physical Address of the first ED
	writew(PhyspED0,HcCtrlHeadED(ohci_base));
//	printf("\nControl Head Initialized to		: %x\n",readw(0x180a4020));

	// Write to HcCommandStatus to tell control list filled
	HcCommandStatus=readw(HcCMDSTS(ohci_base));
	HcCommandStatus=HcCommandStatus|0x00000002;			//only control list filled bit is set in hccommandstatus register & bulk list filled
	printf("the value of HcCommandStatus is 		%x\n",HcCommandStatus);
	writew(HcCommandStatus,HcCMDSTS(ohci_base));
	printf("the value of hccommand status register%x\n",readw(HcCMDSTS(ohci_base)));

	// enabling control list enable
	OHCI_ControlList_Enable(1);
	printf("after enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
	
	HcCommandStatus=readw(HcCMDSTS(ohci_base)/*0x180a4008*/);
//	printf("HcHCCA                                                    :%x\n",readw(0x180a4018));
//	printf("the value of HcHcca.frame number is 	   :%x\n",HcHcca->frame_no);
	printf("HcPeriodCurrentEd                                     :%x\n",readw(HcPeriodCurrED(ohci_base)/*0x180a401C*/));
//	printf("HcControlHeadEd                                       :%x\n",readw(0x180a4020));
//	printf("HcControlCurrentEd                                    :%x\n",readw(0x180a4024));
	printf(("wait for transaction done interrupt\n"));
	while(!TransferComplete)
	{
//		printf("the value of HcHcca.frame number is 				 :%x\n",HcHcca->frame_no);
//		printf("HcControl									  :%x\n",readw(0x180a4004));
//		printf("HcCommandStatus 								 :%x\n",readw(0x180a4008));
//		printf("HcInterruptStatus								 :%x\n",readw(0x180a400C));
//		printf("HcHCCA										 :%x\n",readw(0x180a4018));
//		printf("HcControlHeadEd 								 :%x\n",readw(0x180a4020));
//		printf("HcControlCurrentEd								 :%x\n",readw(0x180a4024));
//		printf("HcFmRemaining									:%x\n",readw(0x180a4038));
//		printf("HcFmNumber									:%x\n",readw(0x180a403C));
//		printf("HcRhPortStatus									 :%x\n",readw(0x180a4054));
		printf("\n");
	}
	TransferComplete = 0;
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
//	HcInterruptStatusreg =readw(0x180a400C);
//	writew(HcInterruptStatusreg,0x180a400C);
//	HcDoneHeadreg = readw(0x180a4030);
//	printf("the value of HcDoneHead char is %x\n",HcDoneHeadreg);
//	printf("the value of HCCADoneHead is		:%x\n",HcHcca->done_head);
	if(HcHcca->done_head == PhyspTD2) 	// checking the status of the completion
	{
		if(!(pTD2->TDdata & 0xF0000000))
		{
			printf("the value of pTD2->TDdata is %x\n",pTD2->TDdata);
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
//			ohci_status.DevAddress=DevAddress;
			printf("Set Feature Passed in Driver");
		}
		else 
		{
			printf("set Feature failed \n");
			printf("the value of pTD2->TDdata is %x\n",pTD2->TDdata);
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
		}
	}
	else
	{
		printf("status td not completed yet\n");
	}
	Add_Free_ED_Ring(pED0);
	Add_Free_GTD_Ring(pTD1);		
	Add_Free_GTD_Ring(pTD2);
	Add_Free_GTD_Ring(pTD3);		
	
	return true;
}

/* ******************************************************************************** *   
Function Name : int ClearFeature_OHCI(BYTE DevRequest,BYTE DevFeature,BYTE EPNO,BYTE EpDir)
Description   : clearing a certain feature of the device
Params        : bmRequest value,Feature Selector,Endpoint number,Endpoint Direction
Returns       : True:ClearFeature_OHCI Success  / False: ClearFeature_OHCI Fail    
Note          :  
******************************************************************************** */
int ClearFeature_OHCI(BYTE DevRequest,BYTE DevFeature,BYTE EPNO,BYTE EpDir)
{
	PED pED0;
	PGENTD pTD1,pTD2,pTD3;
	PUCHAR  Cbp;
	PUCHAR Bend;
	ULONG  PhyspTD1,PhyspTD2,PhyspTD3,PhyspED0,PhysCbp,PhysBend;
	ULONG  HcCommandStatus;//,HcInterruptStatusreg,HcDoneHeadreg;
//	ohci_status.DevAddress = 0;
	printf(("ENTERED IN TO THE Clear Feature\n"));

	OHCI_ControlList_Enable(0);		// commented as per pci driver flow
		
	// Allocate Buffer for PhysCbp
	Cbp=(PUCHAR)SETUP_BUFF_ADDR_OHCI;//dx->VirAddr+32;
	PhysCbp=((u32_t)(Cbp+0));
	
	//For the 8 bytes of setup stage
	*(Cbp+0)=0x00 | DevRequest;
	*(Cbp+1)=USB_REQ_CLEAR_FEATURE;
	*(Cbp+2)=0x0 |DevFeature;//Get Address from Application.
	*(Cbp+3)=0x00;
	*(Cbp+4)=0x00 | EPNO |EpDir;
	*(Cbp+5)=0x00;
	*(Cbp+6)=0x00;
	*(Cbp+7)=0x00;
	Bend=(PUCHAR)(Cbp+7);
	PhysBend=(ULONG)((PUCHAR)PhysCbp+7);
	printf("\nCbp Physical Address: %x",PhysCbp);
	printf("\nCbp Virtual Address: %x",Cbp);
	printf("\nPhysBend Physical Address: %x",PhysBend);
	printf("\nBend Virtual Address: %x",Bend);

	//Initialize the next Null TD 
	pTD3 = Get_Free_GTD();
	PhyspTD3=((u32_t) pTD3);

	printf("\npTD3 Physical Address: %x",PhyspTD3);
	printf("\npTD3 Virtual Address: %x",&pTD3);

	pTD3->TDdata=0xF0000000;	// changed from 0x00000000 to 0xF0000000
	pTD3->Cbp=NULL;
	pTD3->NextTD=NULL;
	pTD3->Bend=NULL;

	//Initialize the next TD for SetAddress status stage
	pTD2=Get_Free_GTD();
	PhyspTD2=((u32_t) pTD2);

	printf("\npTD2 Physical Address: %x",PhyspTD2);
	printf("\npTD2 Virtual Address: %x",pTD2);

	pTD2->TDdata=0xf3100000;
	pTD2->Cbp=NULL;
	pTD2->NextTD=PhyspTD3;
	pTD2->Bend=NULL;

	//Initialize the first TD for SetAddress setup stage
	pTD1=Get_Free_GTD();
	PhyspTD1=((u32_t) pTD1);

	printf("\npTD1 Physical Address: %x",PhyspTD1);
	printf("\npTD1 Virtual Address: %x",pTD1);

	pTD1->TDdata=0xf2000000;
	pTD1->Cbp=PhysCbp;
	pTD1->NextTD=PhyspTD2;
	pTD1->Bend=PhysBend;

	// Form the first control ED in 
	pED0=Get_Free_ED();
	PhyspED0=((u32_t) pED0);
	
	printf("\nED0 Physical Address: %x",PhyspED0);
	printf("\nED0 Virtual Address: %x",pED0);

	//Initialize this first ED
	pED0->EDdata=0x00000000| ohci_status.DevMPS << 16 |(ohci_status.DevAddress);
	pED0->TailP=PhyspTD3; 
	pED0->HeadP=PhyspTD1;
	pED0->NextED=NULL; //PhyspED1.LowPart;

	printf("before enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);

	// Write to Control Head ED register the Physical Address of the first ED
	writew(PhyspED0,HcCtrlHeadED(ohci_base));
//	printf("\nControl Head Initialized to		: %x\n",readw(0x180a4020));

	// Write to HcCommandStatus to tell control list filled
	HcCommandStatus=readw(HcCMDSTS(ohci_base));
	HcCommandStatus=HcCommandStatus|0x00000002;			//only control list filled bit is set in hccommandstatus register & bulk list filled
	printf("the value of HcCommandStatus is 		%x\n",HcCommandStatus);
	writew(HcCommandStatus,HcCMDSTS(ohci_base));
	printf("the value of hccommand status register%x\n",readw(HcCMDSTS(ohci_base)));

	// enabling control list enable
	OHCI_ControlList_Enable(1);
	printf("after enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
	
	HcCommandStatus=readw(HcCMDSTS(ohci_base)/*0x180a4008*/);
//	printf("HcHCCA                                                    :%x\n",readw(0x180a4018));
//	printf("the value of HcHcca.frame number is 	   :%x\n",HcHcca->frame_no);
	printf("HcPeriodCurrentEd                                     :%x\n",readw(HcPeriodCurrED(ohci_base)/*0x180a401C*/));
//	printf("HcControlHeadEd                                       :%x\n",readw(0x180a4020));
//	printf("HcControlCurrentEd                                    :%x\n",readw(0x180a4024));
	printf(("wait for transaction done interrupt\n"));
	while(!TransferComplete)
	{
//		printf("the value of HcHcca.frame number is 				 :%x\n",HcHcca->frame_no);
//		printf("HcControl									  :%x\n",readw(0x180a4004));
//		printf("HcCommandStatus 								 :%x\n",readw(0x180a4008));
//		printf("HcInterruptStatus								 :%x\n",readw(0x180a400C));
//		printf("HcHCCA										 :%x\n",readw(0x180a4018));
//		printf("HcControlHeadEd 								 :%x\n",readw(0x180a4020));
//		printf("HcControlCurrentEd								 :%x\n",readw(0x180a4024));
//		printf("HcFmRemaining									:%x\n",readw(0x180a4038));
//		printf("HcFmNumber									:%x\n",readw(0x180a403C));
//		printf("HcRhPortStatus									 :%x\n",readw(0x180a4054));
		printf("\n");
	}
	TransferComplete = 0;
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
//	HcInterruptStatusreg =readw(0x180a400C);
//	writew(HcInterruptStatusreg,0x180a400C);
//	HcDoneHeadreg = readw(0x180a4030);
//	printf("the value of HcDoneHead char is %x\n",HcDoneHeadreg);
//	printf("the value of HCCADoneHead is		:%x\n",HcHcca->done_head);
	if(HcHcca->done_head == PhyspTD2) 	// checking the status of the completion
	{
		if(!(pTD2->TDdata & 0xF0000000))
		{
			printf("the value of pTD2->TDdata is %x\n",pTD2->TDdata);
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
//			ohci_status.DevAddress=DevAddress;
			printf("Clear Feature Passed in Driver");
		}
		else 
		{
			printf("Clear Feature failed \n");
			printf("the value of pTD2->TDdata is %x\n",pTD2->TDdata);
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
		}
	}
	else
	{
		printf("status td not completed yet\n");
	}
	Add_Free_ED_Ring(pED0);
	Add_Free_GTD_Ring(pTD1);		
	Add_Free_GTD_Ring(pTD2);
	Add_Free_GTD_Ring(pTD3);		
	
	return true;
}

/* ******************************************************************************** *   
Function Name : int SetInterface_OHCI(BYTE DevAltSet,BYTE InterfaceNo)
Description   : selecting an alternate setting and interface number
Params        : Alternate Setting,Interface Number
Returns       : True:SetInterface_OHCI Success  / False: SetInterface_OHCI Fail    
Note          :  
******************************************************************************** */
int SetInterface_OHCI(BYTE DevAltSet,BYTE InterfaceNo)
{
	PED pED0;
	PGENTD pTD1,pTD2,pTD3;
	PUCHAR  Cbp;
	PUCHAR Bend;
	ULONG  PhyspTD1,PhyspTD2,PhyspTD3,PhyspED0,PhysCbp,PhysBend;
	ULONG  HcCommandStatus;//,HcInterruptStatusreg,HcDoneHeadreg;
//	ohci_status.DevAddress = 0;
	printf(("ENTERED IN TO THE SET INTERFACE\n"));

	OHCI_ControlList_Enable(0);		// commented as per pci driver flow
		
	// Allocate Buffer for PhysCbp
	Cbp=(PUCHAR)SETUP_BUFF_ADDR_OHCI;//dx->VirAddr+32;
	PhysCbp=((u32_t)(Cbp+0));
	
	//For the 8 bytes of setup stage
	*(Cbp+0)=0x01;
	*(Cbp+1)=USB_REQ_SET_INTERFACE;
	*(Cbp+2)=0x0 |DevAltSet;//Get Address from Application.
	*(Cbp+3)=0x00;
	*(Cbp+4)=0x00 | InterfaceNo;
	*(Cbp+5)=0x00;
	*(Cbp+6)=0x00;
	*(Cbp+7)=0x00;
	Bend=(PUCHAR)(Cbp+7);
	PhysBend=(ULONG)((PUCHAR)PhysCbp+7);
	printf("\nCbp Physical Address: %x",PhysCbp);
	printf("\nCbp Virtual Address: %x",Cbp);
	printf("\nPhysBend Physical Address: %x",PhysBend);
	printf("\nBend Virtual Address: %x",Bend);

	//Initialize the next Null TD 
	pTD3 = Get_Free_GTD();
	PhyspTD3=((u32_t) pTD3);

	printf("\npTD3 Physical Address: %x",PhyspTD3);
	printf("\npTD3 Virtual Address: %x",&pTD3);

	pTD3->TDdata=0xF0000000;	// changed from 0x00000000 to 0xF0000000
	pTD3->Cbp=NULL;
	pTD3->NextTD=NULL;
	pTD3->Bend=NULL;

	//Initialize the next TD for SetAddress status stage
	pTD2=Get_Free_GTD();
	PhyspTD2=((u32_t) pTD2);

	printf("\npTD2 Physical Address: %x",PhyspTD2);
	printf("\npTD2 Virtual Address: %x",pTD2);

	pTD2->TDdata=0xf3100000;
	pTD2->Cbp=NULL;
	pTD2->NextTD=PhyspTD3;
	pTD2->Bend=NULL;

	//Initialize the first TD for SetAddress setup stage
	pTD1=Get_Free_GTD();
	PhyspTD1=((u32_t) pTD1);

	printf("\npTD1 Physical Address: %x",PhyspTD1);
	printf("\npTD1 Virtual Address: %x",pTD1);

	pTD1->TDdata=0xf2000000;
	pTD1->Cbp=PhysCbp;
	pTD1->NextTD=PhyspTD2;
	pTD1->Bend=PhysBend;

	// Form the first control ED in 
	pED0=Get_Free_ED();
	PhyspED0=((u32_t) pED0);
	
	printf("\nED0 Physical Address: %x",PhyspED0);
	printf("\nED0 Virtual Address: %x",pED0);

	//Initialize this first ED
	pED0->EDdata=0x00000000| ohci_status.DevMPS << 16 |(ohci_status.DevAddress);
	pED0->TailP=PhyspTD3; 
	pED0->HeadP=PhyspTD1;
	pED0->NextED=NULL; //PhyspED1.LowPart;

	printf("before enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);

	// Write to Control Head ED register the Physical Address of the first ED
	writew(PhyspED0,HcCtrlHeadED(ohci_base));
//	printf("\nControl Head Initialized to		: %x\n",readw(0x180a4020));

	// Write to HcCommandStatus to tell control list filled
	HcCommandStatus=readw(HcCMDSTS(ohci_base));
	HcCommandStatus=HcCommandStatus|0x00000002;			//only control list filled bit is set in hccommandstatus register & bulk list filled
	printf("the value of HcCommandStatus is 		%x\n",HcCommandStatus);
	writew(HcCommandStatus,HcCMDSTS(ohci_base));
	printf("the value of hccommand status register%x\n",readw(HcCMDSTS(ohci_base)));

	// enabling control list enable
	OHCI_ControlList_Enable(1);
	printf("after enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
	
	HcCommandStatus=readw(HcCMDSTS(ohci_base)/*0x180a4008*/);
//	printf("HcHCCA                                                    :%x\n",readw(0x180a4018));
//	printf("the value of HcHcca.frame number is 	   :%x\n",HcHcca->frame_no);
	printf("HcPeriodCurrentEd                                     :%x\n",readw(HcPeriodCurrED(ohci_base)/*0x180a401C*/));
//	printf("HcControlHeadEd                                       :%x\n",readw(0x180a4020));
//	printf("HcControlCurrentEd                                    :%x\n",readw(0x180a4024));
	printf(("wait for transaction done interrupt\n"));
	while(!TransferComplete)
	{
//		printf("the value of HcHcca.frame number is 				 :%x\n",HcHcca->frame_no);
//		printf("HcControl									  :%x\n",readw(0x180a4004));
//		printf("HcCommandStatus 								 :%x\n",readw(0x180a4008));
//		printf("HcInterruptStatus								 :%x\n",readw(0x180a400C));
//		printf("HcHCCA										 :%x\n",readw(0x180a4018));
//		printf("HcControlHeadEd 								 :%x\n",readw(0x180a4020));
//		printf("HcControlCurrentEd								 :%x\n",readw(0x180a4024));
//		printf("HcFmRemaining									:%x\n",readw(0x180a4038));
//		printf("HcFmNumber									:%x\n",readw(0x180a403C));
//		printf("HcRhPortStatus									 :%x\n",readw(0x180a4054));
		printf("\n");
	}
	TransferComplete = 0;
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
//	HcInterruptStatusreg =readw(0x180a400C);
//	writew(HcInterruptStatusreg,0x180a400C);
//	HcDoneHeadreg = readw(0x180a4030);
//	printf("the value of HcDoneHead char is %x\n",HcDoneHeadreg);
//	printf("the value of HCCADoneHead is		:%x\n",HcHcca->done_head);
	if(HcHcca->done_head == PhyspTD2) 	// checking the status of the completion
	{
		if(!(pTD2->TDdata & 0xF0000000))
		{
			printf("the value of pTD2->TDdata is %x\n",pTD2->TDdata);
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
//			ohci_status.DevAddress=DevAddress;
			printf("Set Interface Passed in Driver");
		}
		else 
		{
			printf("set Interface failed \n");
			printf("the value of pTD2->TDdata is %x\n",pTD2->TDdata);
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
		}
	}
	else
	{
		printf("status td not completed yet\n");
	}
	Add_Free_ED_Ring(pED0);
	Add_Free_GTD_Ring(pTD1);		
	Add_Free_GTD_Ring(pTD2);
	Add_Free_GTD_Ring(pTD3);		
	
	return true;
}

/* *********************************************************************************   
Function Name : int GetInterface_OHCI()
Description   : Getting the current alternate setting of the interface 
Params        : 
Returns       : True:GetInterface_OHCI Success  / False: GetInterface_OHCI Fail    
Note          :  
*********************************************************************************/
int GetInterface_OHCI()
{
	unsigned char i;
	PED pED0;
	PGENTD pTD1,pTD2,pTD3,pTD4;
	PUCHAR	Buff;
	PUCHAR  Cbp;
	PUCHAR Bend,BuffEnd;
	ULONG  PhyspTD1,PhyspTD2,PhyspTD3,PhyspTD4,PhyspED0,PhysCbp,PhysBend;
	ULONG  PhyBuff,PhyBuffEnd,IntialInStartBuffAddr,TotalBytesTransfered;	
	ULONG  HcCommandStatus;//,HcInterruptStatusreg,HcDoneHeadreg;
	//ohci_status.DevAddress = 0;
	printf(("ENTERED IN TO THE GetInterface_OHCI\n"));

	OHCI_ControlList_Enable(0);
		
	// Allocate Buffer for PhysCbp
	Cbp=(PUCHAR)SETUP_BUFF_ADDR_OHCI;//dx->VirAddr+32;
	PhysCbp=((u32_t)(Cbp+0));
	Buff = (PUCHAR)SETUP_BUFF_ADDR_OHCI+0x110;
	PhyBuff = ((u32_t)(Buff+0));
		
	//For the 8 bytes of setup stage
	*(Cbp+0)=0x81;
	*(Cbp+1)=USB_REQ_GET_INTERFACE;
	*(Cbp+2)=0x00;
	*(Cbp+3)=0x00;
	*(Cbp+4)=0x00 | ohci_status.DevIface;
	*(Cbp+5)=0x00;
	*(Cbp+6)=0x01;
	*(Cbp+7)=0x00;

	Bend=(PUCHAR)(Cbp+7);
	PhysBend=(ULONG)((PUCHAR)PhysCbp+7);
	BuffEnd = (PUCHAR)(Buff + 0);
	PhyBuffEnd = ((u32_t)(BuffEnd+0));

	pTD4 = Get_Free_GTD();
	PhyspTD4=((u32_t) pTD3);
	printf("\npTD3 Physical Address: %x",PhyspTD4);
	printf("\npTD3 Virtual Address: %x",pTD4);
	pTD4->TDdata=NULL;
	pTD4->Cbp=NULL;
	pTD4->NextTD=NULL;
	pTD4->Bend=NULL;
	//Initialize the next Null TD 
	pTD3 = Get_Free_GTD();
	PhyspTD3=((u32_t) pTD3);
	printf("\npTD3 Physical Address: %x",PhyspTD3);
	printf("\npTD3 Virtual Address: %x",pTD3);
	pTD3->TDdata=0xF3080000;
	pTD3->Cbp=NULL;
	pTD3->NextTD=PhyspTD4;
	pTD3->Bend=NULL;
	//Initialize the next TD for SetAddress status stage
	pTD2=Get_Free_GTD();
	//Get the Physical address for this ED
	PhyspTD2=((u32_t) pTD2);
	printf("\npTD2 Physical Address: %x",PhyspTD2);
	printf("\npTD2 Virtual Address: %x",pTD2);
	pTD2->TDdata=0xf3140000;		// Buffer rounding bit is also set.
	pTD2->Cbp=PhyBuff;
	pTD2->NextTD=PhyspTD3;
	pTD2->Bend=PhyBuffEnd;
	IntialInStartBuffAddr = PhyBuff;
	//Initialize the first TD for SetAddress setup stage
	pTD1=Get_Free_GTD();
	//Get the Physical address for this TD
	PhyspTD1=((u32_t) pTD1);
	printf("\npTD1 Physical Address: %x",PhyspTD1);
	printf("\npTD1 Virtual Address: %x",pTD1);
	pTD1->TDdata=0xf2000000;
	pTD1->Cbp=PhysCbp;
	pTD1->NextTD=PhyspTD2;
	pTD1->Bend=PhysBend;

	pED0=Get_Free_ED();
	PhyspED0=((u32_t) pED0);
	printf("\nED0 Physical Address: %x",PhyspED0);
	printf("\nED0 Virtual Address: %x",pED0);
	//Initialize this first ED
	pED0->EDdata=0x00000000|ohci_status.DevMPS << 16 |(ohci_status.DevAddress);			//0x00080000 changed to 0x00400000
	pED0->TailP=PhyspTD4; 
	pED0->HeadP=PhyspTD1;
	pED0->NextED=NULL; //PhyspED1.LowPart;

	// Write to Control Head ED register the Physical Address of the first ED
	writew(PhyspED0,HcCtrlHeadED(ohci_base));
//	printf("\nControl Head Initialized to: %x",readw(0x180a4020));

	// Write to HcCommandStatus to tell control list filled
	HcCommandStatus=readw(HcCMDSTS(ohci_base));
	HcCommandStatus=HcCommandStatus|0x00000002; 		//only control list filled bit is set in hccommandstatus register & bulk list filled
	printf("the value of HcCommandStatus is 		%x\n",HcCommandStatus);
	writew(HcCommandStatus,HcCMDSTS(ohci_base));
	printf("the value of hccommand status register%x\n",readw(HcCMDSTS(ohci_base)));

	// enabling control list enable
	OHCI_ControlList_Enable(1);
	printf("after enable\n");
	printf("pED0->EDdata			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED			:%x\n",pED0->NextED);
	HcCommandStatus=readw(HcCMDSTS(ohci_base)/*0x180a4008*/);
//	printf("HcHCCA							:%x\n",readw(0x180a4018));
//	printf("the value of HcHcca.frame number is 	:%x\n",HcHcca->frame_no);
	printf("HcPeriodCurrentEd					:%x\n",readw(HcPeriodCurrED(ohci_base)/*0x180a401C*/));
//	printf("HcControlHeadEd 					:%x\n",readw(0x180a4020));
//	printf("HcControlCurrentEd					:%x\n",readw(0x180a4024));
	printf(("wait for transaction done interrupt\n"));
	while(!TransferComplete)
	{
//		printf("the value of HcHcca.frame number is 				 :%x\n",HcHcca->frame_no);
//		printf("HcControl									  :%x\n",readw(0x180a4004));
//		printf("HcCommandStatus 								 :%x\n",readw(0x180a4008));
//		printf("HcInterruptStatus								 :%x\n",readw(0x180a400C));
//		printf("HcHCCA										 :%x\n",readw(0x180a4018));
//		printf("HcControlHeadEd 								 :%x\n",readw(0x180a4020));
//		printf("HcControlCurrentEd								 :%x\n",readw(0x180a4024));
//		printf("HcFmRemaining									:%x\n",readw(0x180a4038));
//		printf("HcFmNumber									:%x\n",readw(0x180a403C));
//		printf("HcRhPortStatus									 :%x\n",readw(0x180a4054));
		printf("\n");
	}
	TransferComplete = 0;
	printf("pED0->EDdata			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED			:%x\n",pED0->NextED);
//	HcInterruptStatusreg =readw(0x180a400C);
//	writew(HcInterruptStatusreg,0x180a400C);
//	HcDoneHeadreg = readw(0x180a4030);
//	printf("the value of HcDoneHead char is 	:%x\n",HcDoneHeadreg);
//	printf("the value of HCCADoneHead is		:%x\n",HcHcca->done_head);
	if(HcHcca->done_head == PhyspTD3)	// checking the status of the completion
	{
		printf("the value of pTD3->TDdata is (Status)		:%x\n",pTD3->TDdata);
		printf("the value of pTD2->TDdata is (Data)	 	:%x\n",pTD2->TDdata);
		printf("the value of pTD1->TDdata is (Setup)		:%x\n",pTD1->TDdata);
		if(!(pTD3->TDdata & 0xF0000000))
		{
			printf("Get Interface Passed in Driver\n");
			TotalBytesTransfered = (pTD2->Cbp - IntialInStartBuffAddr);
			for(i=0;i<=TotalBytesTransfered;i++)
			{
				printf("the value of the dscr byte  is	:%x\n",(*(Buff+ i)));
			}
		}
		else 
		{
			printf("Get Interface  failed \n");
		}
	}
	else 
	{
		printf("status td not completed yet\n");
	}
	Add_Free_ED_Ring(pED0);
	Add_Free_GTD_Ring(pTD1);		
	Add_Free_GTD_Ring(pTD2);
	Add_Free_GTD_Ring(pTD3);		
	Add_Free_GTD_Ring(pTD4);		
	return true;
}

/* ******************************************************************************** *   
Function Name : int GetStatus_OHCI(BYTE DevRequest,BYTE DevDescIndex)
Description   : Reading the status of the device
Params        : bmrequest type
Returns       : True:GetStatus_OHCI Success  / False: GetStatus_OHCI Fail    
Note          :  
******************************************************************************** */
int GetStatus_OHCI(BYTE DevRequest,BYTE DevDescIndex)
{
	unsigned char i;
	PED pED0;
	PGENTD pTD1,pTD2,pTD3,pTD4;
	PUCHAR	Buff;
	PUCHAR  Cbp;
	PUCHAR Bend,BuffEnd;
	ULONG  PhyspTD1,PhyspTD2,PhyspTD3,PhyspTD4,PhyspED0,PhysCbp,PhysBend;
	ULONG  PhyBuff,PhyBuffEnd,IntialInStartBuffAddr,TotalBytesTransfered;	
	ULONG  HcCommandStatus;//,HcInterruptStatusreg,HcDoneHeadreg;
	//ohci_status.DevAddress = 0;
	printf(("ENTERED IN TO THE GetStatus_OHCI\n"));

	OHCI_ControlList_Enable(0);
		
	// Allocate Buffer for PhysCbp
	Cbp=(PUCHAR)SETUP_BUFF_ADDR_OHCI;//dx->VirAddr+32;
	PhysCbp=((u32_t)(Cbp+0));
	Buff = (PUCHAR)SETUP_BUFF_ADDR_OHCI+0x120;
	PhyBuff = ((u32_t)(Buff+0));
		
	//For the 8 bytes of setup stage
	*(Cbp+0)=0x00 |DevRequest ;
	*(Cbp+1)=USB_REQ_GET_STATUS;
	*(Cbp+2)=0x00;
	*(Cbp+3)=0x00;
	*(Cbp+4)=0x00;
	*(Cbp+5)=0x00;
	*(Cbp+6)=0x02;
	*(Cbp+7)=0x00;

	Bend=(PUCHAR)(Cbp+7);
	PhysBend=(ULONG)((PUCHAR)PhysCbp+7);
	BuffEnd = (PUCHAR)(Buff + 1);
	PhyBuffEnd = ((u32_t)(BuffEnd+1));

	pTD4 = Get_Free_GTD();
	PhyspTD4=((u32_t) pTD3);
	printf("\npTD3 Physical Address: %x",PhyspTD4);
	printf("\npTD3 Virtual Address: %x",pTD4);
	pTD4->TDdata=NULL;
	pTD4->Cbp=NULL;
	pTD4->NextTD=NULL;
	pTD4->Bend=NULL;
	//Initialize the next Null TD 
	pTD3 = Get_Free_GTD();
	PhyspTD3=((u32_t) pTD3);
	printf("\npTD3 Physical Address: %x",PhyspTD3);
	printf("\npTD3 Virtual Address: %x",pTD3);
	pTD3->TDdata=0xF3080000;
	pTD3->Cbp=NULL;
	pTD3->NextTD=PhyspTD4;
	pTD3->Bend=NULL;
	//Initialize the next TD for SetAddress status stage
	pTD2=Get_Free_GTD();
	//Get the Physical address for this ED
	PhyspTD2=((u32_t) pTD2);
	printf("\npTD2 Physical Address: %x",PhyspTD2);
	printf("\npTD2 Virtual Address: %x",pTD2);
	pTD2->TDdata=0xf3140000;		// Buffer rounding bit is also set.
	pTD2->Cbp=PhyBuff;
	pTD2->NextTD=PhyspTD3;
	pTD2->Bend=PhyBuffEnd;
	IntialInStartBuffAddr = PhyBuff;
	//Initialize the first TD for SetAddress setup stage
	pTD1=Get_Free_GTD();
	//Get the Physical address for this TD
	PhyspTD1=((u32_t) pTD1);
	printf("\npTD1 Physical Address: %x",PhyspTD1);
	printf("\npTD1 Virtual Address: %x",pTD1);
	pTD1->TDdata=0xf2000000;
	pTD1->Cbp=PhysCbp;
	pTD1->NextTD=PhyspTD2;
	pTD1->Bend=PhysBend;

	pED0=Get_Free_ED();
	PhyspED0=((u32_t) pED0);
	printf("\nED0 Physical Address: %x",PhyspED0);
	printf("\nED0 Virtual Address: %x",pED0);
	//Initialize this first ED
	pED0->EDdata=0x00000000|ohci_status.DevMPS << 16 |(ohci_status.DevAddress);			//0x00080000 changed to 0x00400000
	pED0->TailP=PhyspTD4; 
	pED0->HeadP=PhyspTD1;
	pED0->NextED=NULL; //PhyspED1.LowPart;

	// Write to Control Head ED register the Physical Address of the first ED
	writew(PhyspED0,HcCtrlHeadED(ohci_base));
//	printf("\nControl Head Initialized to: %x",readw(0x180a4020));

	// Write to HcCommandStatus to tell control list filled
	HcCommandStatus=readw(HcCMDSTS(ohci_base));
	HcCommandStatus=HcCommandStatus|0x00000002; 		//only control list filled bit is set in hccommandstatus register & bulk list filled
	printf("the value of HcCommandStatus is 		%x\n",HcCommandStatus);
	writew(HcCommandStatus,HcCMDSTS(ohci_base));
	printf("the value of hccommand status register%x\n",readw(HcCMDSTS(ohci_base)));

	// enabling control list enable
	OHCI_ControlList_Enable(1);
	printf("after enable\n");
	printf("pED0->EDdata			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED			:%x\n",pED0->NextED);
	HcCommandStatus=readw(HcCMDSTS(ohci_base)/*0x180a4008*/);
//	printf("HcHCCA							:%x\n",readw(0x180a4018));
//	printf("the value of HcHcca.frame number is 	:%x\n",HcHcca->frame_no);
	printf("HcPeriodCurrentEd					:%x\n",readw(HcPeriodCurrED(ohci_base)/*0x180a401C*/));
//	printf("HcControlHeadEd 					:%x\n",readw(0x180a4020));
//	printf("HcControlCurrentEd					:%x\n",readw(0x180a4024));
	printf(("wait for transaction done interrupt\n"));
	while(!TransferComplete)
	{
//		printf("the value of HcHcca.frame number is 				 :%x\n",HcHcca->frame_no);
//		printf("HcControl									  :%x\n",readw(0x180a4004));
//		printf("HcCommandStatus 								 :%x\n",readw(0x180a4008));
//		printf("HcInterruptStatus								 :%x\n",readw(0x180a400C));
//		printf("HcHCCA										 :%x\n",readw(0x180a4018));
//		printf("HcControlHeadEd 								 :%x\n",readw(0x180a4020));
//		printf("HcControlCurrentEd								 :%x\n",readw(0x180a4024));
//		printf("HcFmRemaining									:%x\n",readw(0x180a4038));
//		printf("HcFmNumber									:%x\n",readw(0x180a403C));
//		printf("HcRhPortStatus									 :%x\n",readw(0x180a4054));
		printf("\n");
	}
	TransferComplete = 0;
	printf("pED0->EDdata			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED			:%x\n",pED0->NextED);
//	HcInterruptStatusreg =readw(0x180a400C);
//	writew(HcInterruptStatusreg,0x180a400C);
//	HcDoneHeadreg = readw(0x180a4030);
//	printf("the value of HcDoneHead char is 	:%x\n",HcDoneHeadreg);
//	printf("the value of HCCADoneHead is		:%x\n",HcHcca->done_head);
	if(HcHcca->done_head == PhyspTD3)	// checking the status of the completion
	{
		printf("the value of pTD3->TDdata is (Status)		:%x\n",pTD3->TDdata);
		printf("the value of pTD2->TDdata is (Data)	 	:%x\n",pTD2->TDdata);
		printf("the value of pTD1->TDdata is (Setup)		:%x\n",pTD1->TDdata);
		if(!(pTD3->TDdata & 0xF0000000))
		{
			printf("Get status Passed in Driver\n");
			TotalBytesTransfered = (pTD2->Cbp - IntialInStartBuffAddr);
			for(i=0;i<=TotalBytesTransfered;i++)
			{
				printf("the value of the dscr byte  is	:%x\n",(*(Buff+ i)));
			}
		}
		else 
		{
			printf("Get status  failed \n");
		}
	}
	else 
	{
		printf("status td not completed yet\n");
	}
	Add_Free_ED_Ring(pED0);
	Add_Free_GTD_Ring(pTD1);		
	Add_Free_GTD_Ring(pTD2);
	Add_Free_GTD_Ring(pTD3);		
	Add_Free_GTD_Ring(pTD4);		
	return true;
}

/* ******************************************************************************** *   
Function Name : int ClearHubFeature_OHCI(BYTE DevFeature)
Description   : Clearing a certain feature of the connected hub
Params        : Feature selector
Returns       : True:ClearHubFeature_OHCI Success  / False: ClearHubFeature_OHCI Fail    
Note          :  
******************************************************************************** */
int ClearHubFeature_OHCI(BYTE DevFeature)
{
	PED pED0;
	PGENTD pTD1,pTD2,pTD3;
	PUCHAR  Cbp;
	PUCHAR Bend;
	ULONG  PhyspTD1,PhyspTD2,PhyspTD3,PhyspED0,PhysCbp,PhysBend;
	ULONG  HcCommandStatus;//,HcInterruptStatusreg,HcDoneHeadreg;
//	ohci_status.DevAddress = 0;
	printf(("ENTERED IN TO THE SET ADDRESS\n"));

	OHCI_ControlList_Enable(0);		// commented as per pci driver flow
		
	// Allocate Buffer for PhysCbp
	Cbp=(PUCHAR)SETUP_BUFF_ADDR_OHCI;//dx->VirAddr+32;
	PhysCbp=((u32_t)(Cbp+0));
	
	//For the 8 bytes of setup stage
	*(Cbp+0)=0x20 ;
	*(Cbp+1)=USB_REQ_CLEAR_FEATURE;
	*(Cbp+2)=0x0 |DevFeature;//Get Address from Application.
	*(Cbp+3)=0x00;
	*(Cbp+4)=0x00;
	*(Cbp+5)=0x00 ;
	*(Cbp+6)=0x00;
	*(Cbp+7)=0x00;
	Bend=(PUCHAR)(Cbp+7);
	PhysBend=(ULONG)((PUCHAR)PhysCbp+7);
	printf("\nCbp Physical Address: %x",PhysCbp);
	printf("\nCbp Virtual Address: %x",Cbp);
	printf("\nPhysBend Physical Address: %x",PhysBend);
	printf("\nBend Virtual Address: %x",Bend);

	//Initialize the next Null TD 
	pTD3 = Get_Free_GTD();
	PhyspTD3=((u32_t) pTD3);

	printf("\npTD3 Physical Address: %x",PhyspTD3);
	printf("\npTD3 Virtual Address: %x",&pTD3);

	pTD3->TDdata=0xF0000000;	// changed from 0x00000000 to 0xF0000000
	pTD3->Cbp=NULL;
	pTD3->NextTD=NULL;
	pTD3->Bend=NULL;

	//Initialize the next TD for SetAddress status stage
	pTD2=Get_Free_GTD();
	PhyspTD2=((u32_t) pTD2);

	printf("\npTD2 Physical Address: %x",PhyspTD2);
	printf("\npTD2 Virtual Address: %x",pTD2);

	pTD2->TDdata=0xf3100000;
	pTD2->Cbp=NULL;
	pTD2->NextTD=PhyspTD3;
	pTD2->Bend=NULL;

	//Initialize the first TD for SetAddress setup stage
	pTD1=Get_Free_GTD();
	PhyspTD1=((u32_t) pTD1);

	printf("\npTD1 Physical Address: %x",PhyspTD1);
	printf("\npTD1 Virtual Address: %x",pTD1);

	pTD1->TDdata=0xf2000000;
	pTD1->Cbp=PhysCbp;
	pTD1->NextTD=PhyspTD2;
	pTD1->Bend=PhysBend;

	// Form the first control ED in 
	pED0=Get_Free_ED();
	PhyspED0=((u32_t) pED0);
	
	printf("\nED0 Physical Address: %x",PhyspED0);
	printf("\nED0 Virtual Address: %x",pED0);

	//Initialize this first ED
	pED0->EDdata=0x00000000| ohci_status.DevMPS << 16 |(ohci_status.DevAddress);
	pED0->TailP=PhyspTD3; 
	pED0->HeadP=PhyspTD1;
	pED0->NextED=NULL; //PhyspED1.LowPart;

	printf("before enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);

	// Write to Control Head ED register the Physical Address of the first ED
	writew(PhyspED0,HcCtrlHeadED(ohci_base));
//	printf("\nControl Head Initialized to		: %x\n",readw(0x180a4020));

	// Write to HcCommandStatus to tell control list filled
	HcCommandStatus=readw(HcCMDSTS(ohci_base));
	HcCommandStatus=HcCommandStatus|0x00000002;			//only control list filled bit is set in hccommandstatus register & bulk list filled
	printf("the value of HcCommandStatus is 		%x\n",HcCommandStatus);
	writew(HcCommandStatus,HcCMDSTS(ohci_base));
	printf("the value of hccommand status register%x\n",readw(HcCMDSTS(ohci_base)));

	// enabling control list enable
	OHCI_ControlList_Enable(1);
	printf("after enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
	
//	HcCommandStatus=readw(0x180a4008);
//	printf("HcHCCA                                                    :%x\n",readw(0x180a4018));
//	printf("the value of HcHcca.frame number is 	   :%x\n",HcHcca->frame_no);
//	printf("HcPeriodCurrentEd                                     :%x\n",readw(0x180a401C));
//	printf("HcControlHeadEd                                       :%x\n",readw(0x180a4020));
//	printf("HcControlCurrentEd                                    :%x\n",readw(0x180a4024));
	printf(("wait for transaction done interrupt\n"));
	while(!TransferComplete)
	{
//		printf("the value of HcHcca.frame number is 				 :%x\n",HcHcca->frame_no);
//		printf("HcControl									  :%x\n",readw(0x180a4004));
//		printf("HcCommandStatus 								 :%x\n",readw(0x180a4008));
//		printf("HcInterruptStatus								 :%x\n",readw(0x180a400C));
//		printf("HcHCCA										 :%x\n",readw(0x180a4018));
//		printf("HcControlHeadEd 								 :%x\n",readw(0x180a4020));
//		printf("HcControlCurrentEd								 :%x\n",readw(0x180a4024));
//		printf("HcFmRemaining									:%x\n",readw(0x180a4038));
//		printf("HcFmNumber									:%x\n",readw(0x180a403C));
//		printf("HcRhPortStatus									 :%x\n",readw(0x180a4054));
//		printf("the value of HcHcca.frame number is 	:%x\n",HcHcca->frame_no);
		printf("\n");
	}
	TransferComplete = 0;
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
//	HcInterruptStatusreg =readw(0x180a400C);
//	writew(HcInterruptStatusreg,0x180a400C);
//	HcDoneHeadreg = readw(0x180a4030);
//	printf("the value of HcDoneHead char is %x\n",HcDoneHeadreg);
//	printf("the value of HCCADoneHead is		:%x\n",HcHcca->done_head);
	if(HcHcca->done_head == PhyspTD2) 	// checking the status of the completion
	{
		if(!(pTD2->TDdata & 0xF0000000))
		{
			printf("the value of pTD2->TDdata is %x\n",pTD2->TDdata);
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
//			ohci_status.DevAddress=DevAddress;
			printf("Hub Clear Feature Passed in Driver");
		}
		else 
		{
			printf("Hub Clear Feature failed \n");
			printf("the value of pTD2->TDdata is %x\n",pTD2->TDdata);
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
		}
	}
	else
	{
		printf("status td not completed yet\n");
	}
	Add_Free_ED_Ring(pED0);
	Add_Free_GTD_Ring(pTD1);		
	Add_Free_GTD_Ring(pTD2);
	Add_Free_GTD_Ring(pTD3);		
	
	return true;
}

/* ******************************************************************************** *   
Function Name : int ClearPortFeature_OHCI(BYTE DevFeature, BYTE Portnum )
Description   : Clearing a certain feature of a specific port in the connected hub
Params        : Feature selector , port number
Returns       : True:ClearPortFeature_OHCI Success  / False: ClearPortFeature_OHCI Fail    
Note          :  
******************************************************************************** */
int ClearPortFeature_OHCI(BYTE DevFeature, BYTE Portnum )
{
	PED pED0;
	PGENTD pTD1,pTD2,pTD3;
	PUCHAR  Cbp;
	PUCHAR Bend;
	ULONG  PhyspTD1,PhyspTD2,PhyspTD3,PhyspED0,PhysCbp,PhysBend;
	ULONG  HcCommandStatus;//,HcInterruptStatusreg,HcDoneHeadreg;
//	ohci_status.DevAddress = 0;
	printf(("ENTERED IN TO THE Clear Port feature\n"));

	OHCI_ControlList_Enable(0);		// commented as per pci driver flow
		
	// Allocate Buffer for PhysCbp
	Cbp=(PUCHAR)SETUP_BUFF_ADDR_OHCI;//dx->VirAddr+32;
	PhysCbp=((u32_t)(Cbp+0));
	
	//For the 8 bytes of setup stage
	*(Cbp+0)=0x23 ;
	*(Cbp+1)=USB_REQ_CLEAR_FEATURE;
	*(Cbp+2)=0x0 |DevFeature;//Get Address from Application.
	*(Cbp+3)=0x00;
	*(Cbp+4)=0x00|Portnum;
	*(Cbp+5)=0x00 ;
	*(Cbp+6)=0x00;
	*(Cbp+7)=0x00;
	Bend=(PUCHAR)(Cbp+7);
	PhysBend=(ULONG)((PUCHAR)PhysCbp+7);
	printf("\nCbp Physical Address: %x",PhysCbp);
	printf("\nCbp Virtual Address: %x",Cbp);
	printf("\nPhysBend Physical Address: %x",PhysBend);
	printf("\nBend Virtual Address: %x",Bend);

	//Initialize the next Null TD 
	pTD3 = Get_Free_GTD();
	PhyspTD3=((u32_t) pTD3);

	printf("\npTD3 Physical Address: %x",PhyspTD3);
	printf("\npTD3 Virtual Address: %x",&pTD3);

	pTD3->TDdata=0xF0000000;	// changed from 0x00000000 to 0xF0000000
	pTD3->Cbp=NULL;
	pTD3->NextTD=NULL;
	pTD3->Bend=NULL;

	//Initialize the next TD for SetAddress status stage
	pTD2=Get_Free_GTD();
	PhyspTD2=((u32_t) pTD2);

	printf("\npTD2 Physical Address: %x",PhyspTD2);
	printf("\npTD2 Virtual Address: %x",pTD2);

	pTD2->TDdata=0xf3100000;
	pTD2->Cbp=NULL;
	pTD2->NextTD=PhyspTD3;
	pTD2->Bend=NULL;

	//Initialize the first TD for SetAddress setup stage
	pTD1=Get_Free_GTD();
	PhyspTD1=((u32_t) pTD1);

	printf("\npTD1 Physical Address: %x",PhyspTD1);
	printf("\npTD1 Virtual Address: %x",pTD1);

	pTD1->TDdata=0xf2000000;
	pTD1->Cbp=PhysCbp;
	pTD1->NextTD=PhyspTD2;
	pTD1->Bend=PhysBend;

	// Form the first control ED in 
	pED0=Get_Free_ED();
	PhyspED0=((u32_t) pED0);
	
	printf("\nED0 Physical Address: %x",PhyspED0);
	printf("\nED0 Virtual Address: %x",pED0);

	//Initialize this first ED
	pED0->EDdata=0x00000000| ohci_status.DevMPS << 16 |(ohci_status.DevAddress);
	pED0->TailP=PhyspTD3; 
	pED0->HeadP=PhyspTD1;
	pED0->NextED=NULL; //PhyspED1.LowPart;

	printf("before enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);

	// Write to Control Head ED register the Physical Address of the first ED
	writew(PhyspED0,HcCtrlHeadED(ohci_base));
//	printf("\nControl Head Initialized to		: %x\n",readw(0x180a4020));

	// Write to HcCommandStatus to tell control list filled
	HcCommandStatus=readw(HcCMDSTS(ohci_base));
	HcCommandStatus=HcCommandStatus|0x00000002;			//only control list filled bit is set in hccommandstatus register & bulk list filled
	printf("the value of HcCommandStatus is 		%x\n",HcCommandStatus);
	writew(HcCommandStatus,HcCMDSTS(ohci_base));
//	printf("the value of hccommand status register%x\n",readw(0x180a4008));

	// enabling control list enable
	OHCI_ControlList_Enable(1);
	printf("after enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
	
	HcCommandStatus=readw(HcCMDSTS(ohci_base)/*0x180a4008*/);
//	printf("HcHCCA                                                    :%x\n",readw(0x180a4018));
//	printf("the value of HcHcca.frame number is 	   :%x\n",HcHcca->frame_no);
	printf("HcPeriodCurrentEd                                     :%x\n",readw(HcPeriodCurrED(ohci_base)/*0x180a401C*/));
//	printf("HcControlHeadEd                                       :%x\n",readw(0x180a4020));
//	printf("HcControlCurrentEd                                    :%x\n",readw(0x180a4024));
	printf(("wait for transaction done interrupt\n"));
	while(!TransferComplete)
	{
//		printf("the value of HcHcca.frame number is 				 :%x\n",HcHcca->frame_no);
//		printf("HcControl									  :%x\n",readw(0x180a4004));
//		printf("HcCommandStatus 								 :%x\n",readw(0x180a4008));
//		printf("HcInterruptStatus								 :%x\n",readw(0x180a400C));
//		printf("HcHCCA										 :%x\n",readw(0x180a4018));
//		printf("HcControlHeadEd 								 :%x\n",readw(0x180a4020));
//		printf("HcControlCurrentEd								 :%x\n",readw(0x180a4024));
//		printf("HcFmRemaining									:%x\n",readw(0x180a4038));
//		printf("HcFmNumber									:%x\n",readw(0x180a403C));
//		printf("HcRhPortStatus									 :%x\n",readw(0x180a4054));
//		printf("the value of HcHcca.frame number is 	:%x\n",HcHcca->frame_no);
		printf("\n");
	}
	TransferComplete = 0;
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
//	HcInterruptStatusreg =readw(0x180a400C);
//	writew(HcInterruptStatusreg,0x180a400C);
//	HcDoneHeadreg = readw(0x180a4030);
//	printf("the value of HcDoneHead char is %x\n",HcDoneHeadreg);
//	printf("the value of HCCADoneHead is		:%x\n",HcHcca->done_head);
	if(HcHcca->done_head == PhyspTD2) 	// checking the status of the completion
	{
		if(!(pTD2->TDdata & 0xF0000000))
		{
			printf("the value of pTD2->TDdata is %x\n",pTD2->TDdata);
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
//			ohci_status.DevAddress=DevAddress;
			printf("Hub Clear Port Feature Passed in Driver");
		}
		else 
		{
			printf("Hub Clear Port Feature failed \n");
			printf("the value of pTD2->TDdata is %x\n",pTD2->TDdata);
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
		}
	}
	else
	{
		printf("status td not completed yet\n");
	}
	Add_Free_ED_Ring(pED0);
	Add_Free_GTD_Ring(pTD1);		
	Add_Free_GTD_Ring(pTD2);
	Add_Free_GTD_Ring(pTD3);		
	
	return true;
}

/* ******************************************************************************** *   
Function Name : int SetHubFeature_OHCI(BYTE DevFeature)
Description   : This function set certain features of the connected hub.
Params        : Feature selector 
Returns       : True:SetHubFeature_OHCI Success  / False: SetHubFeature_OHCI Fail    
Note          :  
******************************************************************************** */
int SetHubFeature_OHCI(BYTE DevFeature)
{
	PED pED0;
	PGENTD pTD1,pTD2,pTD3;
	PUCHAR  Cbp;
	PUCHAR Bend;
	ULONG  PhyspTD1,PhyspTD2,PhyspTD3,PhyspED0,PhysCbp,PhysBend;
	ULONG  HcCommandStatus;//,HcInterruptStatusreg,HcDoneHeadreg;
//	ohci_status.DevAddress = 0;
	printf(("ENTERED IN TO THE SET Hub feature\n"));

	OHCI_ControlList_Enable(0);		// commented as per pci driver flow
		
	// Allocate Buffer for PhysCbp
	Cbp=(PUCHAR)SETUP_BUFF_ADDR_OHCI;//dx->VirAddr+32;
	PhysCbp=((u32_t)(Cbp+0));
	
	//For the 8 bytes of setup stage
	*(Cbp+0)=0x20 ;
	*(Cbp+1)=USB_REQ_SET_FEATURE;
	*(Cbp+2)=0x0 |DevFeature;//Get Address from Application.
	*(Cbp+3)=0x00;
	*(Cbp+4)=0x00;
	*(Cbp+5)=0x00 ;
	*(Cbp+6)=0x00;
	*(Cbp+7)=0x00;
	Bend=(PUCHAR)(Cbp+7);
	PhysBend=(ULONG)((PUCHAR)PhysCbp+7);
	printf("\nCbp Physical Address: %x",PhysCbp);
	printf("\nCbp Virtual Address: %x",Cbp);
	printf("\nPhysBend Physical Address: %x",PhysBend);
	printf("\nBend Virtual Address: %x",Bend);

	//Initialize the next Null TD 
	pTD3 = Get_Free_GTD();
	PhyspTD3=((u32_t) pTD3);

	printf("\npTD3 Physical Address: %x",PhyspTD3);
	printf("\npTD3 Virtual Address: %x",&pTD3);

	pTD3->TDdata=0xF0000000;	// changed from 0x00000000 to 0xF0000000
	pTD3->Cbp=NULL;
	pTD3->NextTD=NULL;
	pTD3->Bend=NULL;

	//Initialize the next TD for SetAddress status stage
	pTD2=Get_Free_GTD();
	PhyspTD2=((u32_t) pTD2);

	printf("\npTD2 Physical Address: %x",PhyspTD2);
	printf("\npTD2 Virtual Address: %x",pTD2);

	pTD2->TDdata=0xf3100000;
	pTD2->Cbp=NULL;
	pTD2->NextTD=PhyspTD3;
	pTD2->Bend=NULL;

	//Initialize the first TD for SetAddress setup stage
	pTD1=Get_Free_GTD();
	PhyspTD1=((u32_t) pTD1);

	printf("\npTD1 Physical Address: %x",PhyspTD1);
	printf("\npTD1 Virtual Address: %x",pTD1);

	pTD1->TDdata=0xf2000000;
	pTD1->Cbp=PhysCbp;
	pTD1->NextTD=PhyspTD2;
	pTD1->Bend=PhysBend;

	// Form the first control ED in 
	pED0=Get_Free_ED();
	PhyspED0=((u32_t) pED0);
	
	printf("\nED0 Physical Address: %x",PhyspED0);
	printf("\nED0 Virtual Address: %x",pED0);

	//Initialize this first ED
	pED0->EDdata=0x00000000| ohci_status.DevMPS << 16 |(ohci_status.DevAddress);
	pED0->TailP=PhyspTD3; 
	pED0->HeadP=PhyspTD1;
	pED0->NextED=NULL; //PhyspED1.LowPart;

	printf("before enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);

	// Write to Control Head ED register the Physical Address of the first ED
	writew(PhyspED0,HcCtrlHeadED(ohci_base));
//	printf("\nControl Head Initialized to		: %x\n",readw(0x180a4020));

	// Write to HcCommandStatus to tell control list filled
	HcCommandStatus=readw(HcCMDSTS(ohci_base));
	HcCommandStatus=HcCommandStatus|0x00000002;			//only control list filled bit is set in hccommandstatus register & bulk list filled
	printf("the value of HcCommandStatus is 		%x\n",HcCommandStatus);
	writew(HcCommandStatus,HcCMDSTS(ohci_base));
//	printf("the value of hccommand status register%x\n",readw(0x180a4008));

	// enabling control list enable
	OHCI_ControlList_Enable(1);
	printf("after enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
	
//	HcCommandStatus=readw(0x180a4008);
//	printf("HcHCCA                                                    :%x\n",readw(0x180a4018));
//	printf("the value of HcHcca.frame number is 	   :%x\n",HcHcca->frame_no);
//	printf("HcPeriodCurrentEd                                     :%x\n",readw(0x180a401C));
//	printf("HcControlHeadEd                                       :%x\n",readw(0x180a4020));
//	printf("HcControlCurrentEd                                    :%x\n",readw(0x180a4024));
	printf(("wait for transaction done interrupt\n"));
	while(!TransferComplete)
	{
//		printf("the value of HcHcca.frame number is 				 :%x\n",HcHcca->frame_no);
//		printf("HcControl									  :%x\n",readw(0x180a4004));
//		printf("HcCommandStatus 								 :%x\n",readw(0x180a4008));
//		printf("HcInterruptStatus								 :%x\n",readw(0x180a400C));
//		printf("HcHCCA										 :%x\n",readw(0x180a4018));
//		printf("HcControlHeadEd 								 :%x\n",readw(0x180a4020));
//		printf("HcControlCurrentEd								 :%x\n",readw(0x180a4024));
//		printf("HcFmRemaining									:%x\n",readw(0x180a4038));
//		printf("HcFmNumber									:%x\n",readw(0x180a403C));
//		printf("HcRhPortStatus									 :%x\n",readw(0x180a4054));
//		printf("the value of HcHcca.frame number is 	:%x\n",HcHcca->frame_no);
		printf("\n");
	}
	TransferComplete = 0;
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
//	HcInterruptStatusreg =readw(0x180a400C);
//	writew(HcInterruptStatusreg,0x180a400C);
//	HcDoneHeadreg = readw(0x180a4030);
//	printf("the value of HcDoneHead char is %x\n",HcDoneHeadreg);
//	printf("the value of HCCADoneHead is		:%x\n",HcHcca->done_head);
	if(HcHcca->done_head == PhyspTD2) 	// checking the status of the completion
	{
		if(!(pTD2->TDdata & 0xF0000000))
		{
			printf("the value of pTD2->TDdata is %x\n",pTD2->TDdata);
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
//			ohci_status.DevAddress=DevAddress;
			printf("Hub Set Feature Passed in Driver");
		}
		else 
		{
			printf("Hub Set Feature failed \n");
			printf("the value of pTD2->TDdata is %x\n",pTD2->TDdata);
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
		}
	}
	else
	{
		printf("status td not completed yet\n");
	}
	Add_Free_ED_Ring(pED0);
	Add_Free_GTD_Ring(pTD1);		
	Add_Free_GTD_Ring(pTD2);
	Add_Free_GTD_Ring(pTD3);		
	
	return true;
}

/* ******************************************************************************** *   
Function Name : int SetPortFeature_OHCI(BYTE DevFeature, BYTE Portnum )
Description   : This function set certain bits in the hub port register of the connected hub.
Params        : Feature selector 
Returns       : True:SetPortFeature_OHCI Success  / False: SetPortFeature_OHCI Fail    
Note          :  
******************************************************************************** */
int SetPortFeature_OHCI(BYTE DevFeature, BYTE Portnum )
{
	PED pED0;
	PGENTD pTD1,pTD2,pTD3;
	PUCHAR  Cbp;
	PUCHAR Bend;
	ULONG  PhyspTD1,PhyspTD2,PhyspTD3,PhyspED0,PhysCbp,PhysBend;
	ULONG  HcCommandStatus;//,HcInterruptStatusreg,HcDoneHeadreg;
//	ohci_status.DevAddress = 0;
	printf(("ENTERED IN TO THE Set Port feature\n"));

	OHCI_ControlList_Enable(0);		// commented as per pci driver flow
		
	// Allocate Buffer for PhysCbp
	Cbp=(PUCHAR)SETUP_BUFF_ADDR_OHCI;//dx->VirAddr+32;
	PhysCbp=((u32_t)(Cbp+0));
	
	//For the 8 bytes of setup stage
	*(Cbp+0)=0x23 ;
	*(Cbp+1)=USB_REQ_SET_FEATURE;
	*(Cbp+2)=0x0 |DevFeature;//Get Address from Application.
	*(Cbp+3)=0x00;
	*(Cbp+4)=0x00|Portnum;
	*(Cbp+5)=0x00  ;
	*(Cbp+6)=0x00;
	*(Cbp+7)=0x00;
	Bend=(PUCHAR)(Cbp+7);
	PhysBend=(ULONG)((PUCHAR)PhysCbp+7);
	printf("\nCbp Physical Address: %x",PhysCbp);
	printf("\nCbp Virtual Address: %x",Cbp);
	printf("\nPhysBend Physical Address: %x",PhysBend);
	printf("\nBend Virtual Address: %x",Bend);

	//Initialize the next Null TD 
	pTD3 = Get_Free_GTD();
	PhyspTD3=((u32_t) pTD3);

	printf("\npTD3 Physical Address: %x",PhyspTD3);
	printf("\npTD3 Virtual Address: %x",&pTD3);

	pTD3->TDdata=0xF0000000;	// changed from 0x00000000 to 0xF0000000
	pTD3->Cbp=NULL;
	pTD3->NextTD=NULL;
	pTD3->Bend=NULL;

	//Initialize the next TD for SetAddress status stage
	pTD2=Get_Free_GTD();
	PhyspTD2=((u32_t) pTD2);

	printf("\npTD2 Physical Address: %x",PhyspTD2);
	printf("\npTD2 Virtual Address: %x",pTD2);

	pTD2->TDdata=0xf3100000;
	pTD2->Cbp=NULL;
	pTD2->NextTD=PhyspTD3;
	pTD2->Bend=NULL;

	//Initialize the first TD for SetAddress setup stage
	pTD1=Get_Free_GTD();
	PhyspTD1=((u32_t) pTD1);

	printf("\npTD1 Physical Address: %x",PhyspTD1);
	printf("\npTD1 Virtual Address: %x",pTD1);

	pTD1->TDdata=0xf2000000;
	pTD1->Cbp=PhysCbp;
	pTD1->NextTD=PhyspTD2;
	pTD1->Bend=PhysBend;

	// Form the first control ED in 
	pED0=Get_Free_ED();
	PhyspED0=((u32_t) pED0);
	
	printf("\nED0 Physical Address: %x",PhyspED0);
	printf("\nED0 Virtual Address: %x",pED0);

	//Initialize this first ED
	pED0->EDdata=0x00000000| ohci_status.DevMPS << 16 |(ohci_status.DevAddress);
	pED0->TailP=PhyspTD3; 
	pED0->HeadP=PhyspTD1;
	pED0->NextED=NULL; //PhyspED1.LowPart;

	printf("before enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);

	// Write to Control Head ED register the Physical Address of the first ED
	writew(PhyspED0,HcCtrlHeadED(ohci_base));
//	printf("\nControl Head Initialized to		: %x\n",readw(0x180a4020));

	// Write to HcCommandStatus to tell control list filled
	HcCommandStatus=readw(HcCMDSTS(ohci_base));
	HcCommandStatus=HcCommandStatus|0x00000002;			//only control list filled bit is set in hccommandstatus register & bulk list filled
	printf("the value of HcCommandStatus is 		%x\n",HcCommandStatus);
	writew(HcCommandStatus,HcCMDSTS(ohci_base));
//	printf("the value of hccommand status register%x\n",readw(0x180a4008));

	// enabling control list enable
	OHCI_ControlList_Enable(1);
	printf("after enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
	
//	HcCommandStatus=readw(0x180a4008);
//	printf("HcHCCA                                                    :%x\n",readw(0x180a4018));
//	printf("the value of HcHcca.frame number is 	   :%x\n",HcHcca->frame_no);
//	printf("HcPeriodCurrentEd                                     :%x\n",readw(0x180a401C));
//	printf("HcControlHeadEd                                       :%x\n",readw(0x180a4020));
//	printf("HcControlCurrentEd                                    :%x\n",readw(0x180a4024));
	printf(("wait for transaction done interrupt\n"));
	while(!TransferComplete)
	{
//		printf("the value of HcHcca.frame number is 				 :%x\n",HcHcca->frame_no);
//		printf("HcControl									  :%x\n",readw(0x180a4004));
//		printf("HcCommandStatus 								 :%x\n",readw(0x180a4008));
//		printf("HcInterruptStatus								 :%x\n",readw(0x180a400C));
//		printf("HcHCCA										 :%x\n",readw(0x180a4018));
//		printf("HcControlHeadEd 								 :%x\n",readw(0x180a4020));
//		printf("HcControlCurrentEd								 :%x\n",readw(0x180a4024));
//		printf("HcFmRemaining									:%x\n",readw(0x180a4038));
//		printf("HcFmNumber									:%x\n",readw(0x180a403C));
//		printf("HcRhPortStatus									 :%x\n",readw(0x180a4054));
//		printf("the value of HcHcca.frame number is 	:%x\n",HcHcca->frame_no);
		printf("\n");
	}
	TransferComplete = 0;
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
//	HcInterruptStatusreg =readw(0x180a400C);
//	writew(HcInterruptStatusreg,0x180a400C);
//	HcDoneHeadreg = readw(0x180a4030);
//	printf("the value of HcDoneHead char is %x\n",HcDoneHeadreg);
//	printf("the value of HCCADoneHead is		:%x\n",HcHcca->done_head);
	if(HcHcca->done_head == PhyspTD2) 	// checking the status of the completion
	{
		if(!(pTD2->TDdata & 0xF0000000))
		{
			printf("the value of pTD2->TDdata is %x\n",pTD2->TDdata);
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
//			ohci_status.DevAddress=DevAddress;
			printf("Hub set Port Feature Passed in Driver");
		}
		else 
		{
			printf("Hub Set Port Feature failed \n");
			printf("the value of pTD2->TDdata is %x\n",pTD2->TDdata);
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
		}
	}
	else
	{
		printf("status td not completed yet\n");
	}
	Add_Free_ED_Ring(pED0);
	Add_Free_GTD_Ring(pTD1);		
	Add_Free_GTD_Ring(pTD2);
	Add_Free_GTD_Ring(pTD3);		
	
	return true;
}

/* ******************************************************************************** *   
Function Name : int GetHubStatus_OHCI()
Description   : This function reads the status of the connected hub.
Params        :  
Returns       : True:GetHubStatus_OHCI Success  / False: GetHubStatus_OHCI Fail    
Note          :  
******************************************************************************** */
int GetHubStatus_OHCI()
{
	unsigned char i;
	PED pED0;
	PGENTD pTD1,pTD2,pTD3,pTD4;
	PUCHAR	Buff;
	PUCHAR  Cbp;
	PUCHAR Bend,BuffEnd;
	ULONG  PhyspTD1,PhyspTD2,PhyspTD3,PhyspTD4,PhyspED0,PhysCbp,PhysBend;
	ULONG  PhyBuff,PhyBuffEnd,IntialInStartBuffAddr,TotalBytesTransfered;	
	ULONG  HcCommandStatus;//,HcInterruptStatusreg,HcDoneHeadreg;
	//ohci_status.DevAddress = 0;
	printf(("ENTERED IN TO THE GetStatus_OHCI\n"));

	OHCI_ControlList_Enable(0);
		
	// Allocate Buffer for PhysCbp
	Cbp=(PUCHAR)SETUP_BUFF_ADDR_OHCI;//dx->VirAddr+32;
	PhysCbp=((u32_t)(Cbp+0));
	Buff = (PUCHAR)SETUP_BUFF_ADDR_OHCI+0x120;
	PhyBuff = ((u32_t)(Buff+0));
		
	//For the 8 bytes of setup stage
	*(Cbp+0)=0xA0  ;
	*(Cbp+1)=USB_REQ_GET_STATUS;
	*(Cbp+2)=0x00;
	*(Cbp+3)=0x00;
	*(Cbp+4)=0x00;
	*(Cbp+5)=0x00;
	*(Cbp+6)=0x04;
	*(Cbp+7)=0x00;

	Bend=(PUCHAR)(Cbp+7);
	PhysBend=(ULONG)((PUCHAR)PhysCbp+7);
	BuffEnd = (PUCHAR)(Buff + 3);
	PhyBuffEnd = ((u32_t)(BuffEnd+3));

	pTD4 = Get_Free_GTD();
	PhyspTD4=((u32_t) pTD3);
	printf("\npTD3 Physical Address: %x",PhyspTD4);
	printf("\npTD3 Virtual Address: %x",pTD4);
	pTD4->TDdata=NULL;
	pTD4->Cbp=NULL;
	pTD4->NextTD=NULL;
	pTD4->Bend=NULL;
	//Initialize the next Null TD 
	pTD3 = Get_Free_GTD();
	PhyspTD3=((u32_t) pTD3);
	printf("\npTD3 Physical Address: %x",PhyspTD3);
	printf("\npTD3 Virtual Address: %x",pTD3);
	pTD3->TDdata=0xF3080000;
	pTD3->Cbp=NULL;
	pTD3->NextTD=PhyspTD4;
	pTD3->Bend=NULL;
	//Initialize the next TD for SetAddress status stage
	pTD2=Get_Free_GTD();
	//Get the Physical address for this ED
	PhyspTD2=((u32_t) pTD2);
	printf("\npTD2 Physical Address: %x",PhyspTD2);
	printf("\npTD2 Virtual Address: %x",pTD2);
	pTD2->TDdata=0xf3140000;		// Buffer rounding bit is also set.
	pTD2->Cbp=PhyBuff;
	pTD2->NextTD=PhyspTD3;
	pTD2->Bend=PhyBuffEnd;
	IntialInStartBuffAddr = PhyBuff;
	//Initialize the first TD for SetAddress setup stage
	pTD1=Get_Free_GTD();
	//Get the Physical address for this TD
	PhyspTD1=((u32_t) pTD1);
	printf("\npTD1 Physical Address: %x",PhyspTD1);
	printf("\npTD1 Virtual Address: %x",pTD1);
	pTD1->TDdata=0xf2000000;
	pTD1->Cbp=PhysCbp;
	pTD1->NextTD=PhyspTD2;
	pTD1->Bend=PhysBend;

	pED0=Get_Free_ED();
	PhyspED0=((u32_t) pED0);
	printf("\nED0 Physical Address: %x",PhyspED0);
	printf("\nED0 Virtual Address: %x",pED0);
	//Initialize this first ED
	pED0->EDdata=0x00000000|ohci_status.DevMPS << 16 |(ohci_status.DevAddress);			//0x00080000 changed to 0x00400000
	pED0->TailP=PhyspTD4; 
	pED0->HeadP=PhyspTD1;
	pED0->NextED=NULL; //PhyspED1.LowPart;

	// Write to Control Head ED register the Physical Address of the first ED
	writew(PhyspED0,HcCtrlHeadED(ohci_base));
//	printf("\nControl Head Initialized to: %x",readw(0x180a4020));

	// Write to HcCommandStatus to tell control list filled
	HcCommandStatus=readw(HcCMDSTS(ohci_base));
	HcCommandStatus=HcCommandStatus|0x00000002; 		//only control list filled bit is set in hccommandstatus register & bulk list filled
	printf("the value of HcCommandStatus is 		%x\n",HcCommandStatus);
	writew(HcCommandStatus,HcCMDSTS(ohci_base));
//	printf("the value of hccommand status register%x\n",readw(0x180a4008));

	// enabling control list enable
	OHCI_ControlList_Enable(1);
	printf("after enable\n");
	printf("pED0->EDdata			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED			:%x\n",pED0->NextED);
//	HcCommandStatus=readw(0x180a4008);
//	printf("HcHCCA							:%x\n",readw(0x180a4018));
//	printf("HcFmNumber						:%x\n",readw(0x180a403C));
	printf("the value of HcHcca.frame number is 	:%x\n",HcHcca->frame_no);
//	printf("HcPeriodCurrentEd					:%x\n",readw(0x180a401C));
//	printf("HcControlHeadEd 					:%x\n",readw(0x180a4020));
//	printf("HcControlCurrentEd					:%x\n",readw(0x180a4024));
	printf(("wait for transaction done interrupt\n"));
	while(!TransferComplete)
	{
//		printf("the value of HcHcca.frame number is 				 :%x\n",HcHcca->frame_no);
//		printf("HcControl									  :%x\n",readw(0x180a4004));
//		printf("HcCommandStatus 								 :%x\n",readw(0x180a4008));
//		printf("HcInterruptStatus								 :%x\n",readw(0x180a400C));
//		printf("HcHCCA										 :%x\n",readw(0x180a4018));
//		printf("HcControlHeadEd 								 :%x\n",readw(0x180a4020));
//		printf("HcControlCurrentEd								 :%x\n",readw(0x180a4024));
//		printf("HcFmRemaining									:%x\n",readw(0x180a4038));
//		printf("HcFmNumber									:%x\n",readw(0x180a403C));
//		printf("HcRhPortStatus									 :%x\n",readw(0x180a4054));
//		printf("the value of HcHcca.frame number is 	:%x\n",HcHcca->frame_no);
		printf("\n");
	}
	TransferComplete = 0;
	printf("pED0->EDdata			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED			:%x\n",pED0->NextED);
//	HcInterruptStatusreg =readw(0x180a400C);
//	writew(HcInterruptStatusreg,0x180a400C);
//	HcDoneHeadreg = readw(0x180a4030);
//	printf("the value of HcDoneHead char is 	:%x\n",HcDoneHeadreg);
//	printf("the value of HCCADoneHead is		:%x\n",HcHcca->done_head);
	if(HcHcca->done_head == PhyspTD3)	// checking the status of the completion
	{
		printf("the value of pTD3->TDdata is (Status)		:%x\n",pTD3->TDdata);
		printf("the value of pTD2->TDdata is (Data)	 	:%x\n",pTD2->TDdata);
		printf("the value of pTD1->TDdata is (Setup)		:%x\n",pTD1->TDdata);
		if(!(pTD3->TDdata & 0xF0000000))
		{
			printf("Get status Passed in Driver\n");
			TotalBytesTransfered = (pTD2->Cbp - IntialInStartBuffAddr);
			for(i=0;i<=TotalBytesTransfered;i++)
			{
				printf("the value of the dscr byte  is	:%x\n",(*(Buff+ i)));
			}
		}
		else 
		{
			printf("Get status  failed \n");
		}
	}
	else 
	{
		printf("status td not completed yet\n");
	}
	Add_Free_ED_Ring(pED0);
	Add_Free_GTD_Ring(pTD1);		
	Add_Free_GTD_Ring(pTD2);
	Add_Free_GTD_Ring(pTD3);		
	Add_Free_GTD_Ring(pTD4);		
	return true;
}

/* ******************************************************************************** *   
Function Name : int GetPortStatus_OHCI(BYTE Portnum)
Description   : This function reads the status of the specified port of the connected hub.
Params        : Portnumber 
Returns       : True:GetPortStatus_OHCI Success  / False: GetPortStatus_OHCI Fail    
Note          :  
******************************************************************************** */
int GetPortStatus_OHCI(BYTE Portnum)
{
	unsigned char i;
	PED pED0;
	PGENTD pTD1,pTD2,pTD3,pTD4;
	PUCHAR	Buff;
	PUCHAR  Cbp;
	PUCHAR Bend,BuffEnd;
	ULONG  PhyspTD1,PhyspTD2,PhyspTD3,PhyspTD4,PhyspED0,PhysCbp,PhysBend;
	ULONG  PhyBuff,PhyBuffEnd,IntialInStartBuffAddr,TotalBytesTransfered;	
	ULONG  HcCommandStatus;//,HcInterruptStatusreg,HcDoneHeadreg;
	//ohci_status.DevAddress = 0;
	printf(("ENTERED IN TO THE GetPortStatus_OHCI\n"));

	OHCI_ControlList_Enable(0);
		
	// Allocate Buffer for PhysCbp
	Cbp=(PUCHAR)SETUP_BUFF_ADDR_OHCI;//dx->VirAddr+32;
	PhysCbp=((u32_t)(Cbp+0));
	Buff = (PUCHAR)SETUP_BUFF_ADDR_OHCI+0x120;
	PhyBuff = ((u32_t)(Buff+0));
		
	//For the 8 bytes of setup stage
	*(Cbp+0)=0xA3  ;
	*(Cbp+1)=USB_REQ_GET_STATUS;
	*(Cbp+2)=0x00;
	*(Cbp+3)=0x00;
	*(Cbp+4)=0x00| Portnum;
	*(Cbp+5)=0x00 ;
	*(Cbp+6)=0x04;
	*(Cbp+7)=0x00;
	

	Bend=(PUCHAR)(Cbp+7);
	PhysBend=(ULONG)((PUCHAR)PhysCbp+7);
	BuffEnd = (PUCHAR)(Buff + 3);
	PhyBuffEnd = ((u32_t)(BuffEnd+3));

	pTD4 = Get_Free_GTD();
	PhyspTD4=((u32_t) pTD4);
	printf("\npTD4 Physical Address: %x",PhyspTD4);
	printf("\npTD4 Virtual Address: %x",pTD4);
	pTD4->TDdata=NULL;
	pTD4->Cbp=NULL;
	pTD4->NextTD=NULL;
	pTD4->Bend=NULL;
	//Initialize the next Null TD 
	pTD3 = Get_Free_GTD();
	PhyspTD3=((u32_t) pTD3);
	printf("\npTD3 Physical Address: %x",PhyspTD3);
	printf("\npTD3 Virtual Address: %x",pTD3);
	pTD3->TDdata=0xF3080000;
	pTD3->Cbp=NULL;
	pTD3->NextTD=PhyspTD4;
	pTD3->Bend=NULL;
	//Initialize the next TD for SetAddress status stage
	pTD2=Get_Free_GTD();
	//Get the Physical address for this ED
	PhyspTD2=((u32_t) pTD2);
	printf("\npTD2 Physical Address: %x",PhyspTD2);
	printf("\npTD2 Virtual Address: %x",pTD2);
	pTD2->TDdata=0xf3140000;		// Buffer rounding bit is also set.
	pTD2->Cbp=PhyBuff;
	pTD2->NextTD=PhyspTD3;
	pTD2->Bend=PhyBuffEnd;
	IntialInStartBuffAddr = PhyBuff;
	//Initialize the first TD for SetAddress setup stage
	pTD1=Get_Free_GTD();
	//Get the Physical address for this TD
	PhyspTD1=((u32_t) pTD1);
	printf("\npTD1 Physical Address: %x",PhyspTD1);
	printf("\npTD1 Virtual Address: %x",pTD1);
	pTD1->TDdata=0xf2000000;
	pTD1->Cbp=PhysCbp;
	pTD1->NextTD=PhyspTD2;
	pTD1->Bend=PhysBend;

	pED0=Get_Free_ED();
	PhyspED0=((u32_t) pED0);
	printf("\nED0 Physical Address: %x",PhyspED0);
	printf("\nED0 Virtual Address: %x",pED0);
	//Initialize this first ED
	pED0->EDdata=0x00000000|ohci_status.DevMPS << 16 |(ohci_status.DevAddress);			//0x00080000 changed to 0x00400000
	pED0->TailP=PhyspTD4; 
	pED0->HeadP=PhyspTD1;
	pED0->NextED=NULL; //PhyspED1.LowPart;

	// Write to Control Head ED register the Physical Address of the first ED
	writew(PhyspED0,HcCtrlHeadED(ohci_base));
//	printf("\nControl Head Initialized to: %x",readw(0x180a4020));

	printf("before enable\n");
	printf("pED0->EDdata			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED			:%x\n",pED0->NextED);
	// Write to HcCommandStatus to tell control list filled
	HcCommandStatus=readw(HcCMDSTS(ohci_base));
	HcCommandStatus=HcCommandStatus|0x00000002; 		//only control list filled bit is set in hccommandstatus register & bulk list filled
	printf("the value of HcCommandStatus is 		%x\n",HcCommandStatus);
	writew(HcCommandStatus,HcCMDSTS(ohci_base));
	printf("the value of hccommand status register%x\n",readw(HcCMDSTS(ohci_base)));

	// enabling control list enable
	OHCI_ControlList_Enable(1);
	printf("after enable\n");
	printf("pED0->EDdata			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED			:%x\n",pED0->NextED);
	HcCommandStatus=readw(HcCMDSTS(ohci_base)/*0x180a4008*/);
//	printf("HcHCCA							:%x\n",readw(0x180a4018));
//	printf("the value of HcHcca.frame number is 	:%x\n",HcHcca->frame_no);
	printf("HcPeriodCurrentEd					:%x\n",readw(HcPeriodCurrED(ohci_base)/*0x180a401C*/));
//	printf("HcControlHeadEd 					:%x\n",readw(0x180a4020));
//	printf("HcControlCurrentEd					:%x\n",readw(0x180a4024));
	printf(("wait for transaction done interrupt\n"));
	while(!TransferComplete)
	{
//		printf("the value of HcHcca.frame number is 				 :%x\n",HcHcca->frame_no);
//		printf("HcControl									  :%x\n",readw(0x180a4004));
//		printf("HcCommandStatus 								 :%x\n",readw(0x180a4008));
//		printf("HcInterruptStatus								 :%x\n",readw(0x180a400C));
//		printf("HcHCCA										 :%x\n",readw(0x180a4018));
//		printf("HcControlHeadEd 								 :%x\n",readw(0x180a4020));
//		printf("HcControlCurrentEd								 :%x\n",readw(0x180a4024));
//		printf("HcFmRemaining									:%x\n",readw(0x180a4038));
//		printf("HcFmNumber									:%x\n",readw(0x180a403C));
//		printf("HcRhPortStatus									 :%x\n",readw(0x180a4054));
		printf("\n");
	}
	TransferComplete = 0;
	printf("pED0->EDdata			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED			:%x\n",pED0->NextED);
//	HcInterruptStatusreg =readw(0x180a400C);
//	writew(HcInterruptStatusreg,0x180a400C);
//	HcDoneHeadreg = readw(0x180a4030);
//	printf("the value of HcDoneHead char is 	:%x\n",HcDoneHeadreg);
//	printf("the value of HCCADoneHead is		:%x\n",HcHcca->done_head);
	if(HcHcca->done_head == PhyspTD3)	// checking the status of the completion
	{
//		printf("HcHcca->done_head 	:%x\n",HcHcca->done_head);
		printf("the value of pTD3->TDdata is (Status)		:%x\n",pTD3->TDdata);
		printf("the value of pTD2->TDdata is (Data)	 	:%x\n",pTD2->TDdata);
		printf("the value of pTD1->TDdata is (Setup)		:%x\n",pTD1->TDdata);
		if(!(pTD3->TDdata & 0xF0000000))
		{
			printf("GetPortstatus Passed in Driver\n");
			TotalBytesTransfered = (pTD2->Cbp - IntialInStartBuffAddr);
			for(i=0;i<=TotalBytesTransfered;i++)
			{
				printf("the value of the dscr byte  is	:%x\n",(*(Buff+ i)));
			}
		}
		else 
		{
			printf("GetPortstatus  failed \n");
		}
	}
	else 
	{
//		printf("HcHcca->done_head 	:%x\n",HcHcca->done_head);
		printf("status td not completed yet\n");
	}
	Add_Free_ED_Ring(pED0);
	Add_Free_GTD_Ring(pTD1);		
	Add_Free_GTD_Ring(pTD2);
	Add_Free_GTD_Ring(pTD3);		
	Add_Free_GTD_Ring(pTD4);		
	return true;
}

/* ******************************************************************************** *   
Function Name : int GetHubDescriptor_OHCI(BYTE DevDescType,BYTE DevDescIndex,BYTE DevDescLength)
Description   : This function reads the hub descriptor form the connected hub.
Params        : Descriptor type,Descriptor index and Descriptor Length 
Returns       : True:GetHubDescriptor_OHCI Success  / False: GetHubDescriptor_OHCI Fail    
Note          :  
******************************************************************************** */
int GetHubDescriptor_OHCI(BYTE DevDescType,BYTE DevDescIndex,BYTE DevDescLength)
{
	unsigned char i;
	PED pED0;
	PGENTD pTD1,pTD2,pTD3,pTD4;
	PUCHAR	Buff;
	PUCHAR  Cbp;
	PUCHAR Bend,BuffEnd;
	ULONG  PhyspTD1,PhyspTD2,PhyspTD3,PhyspTD4,PhyspED0,PhysCbp,PhysBend;
	ULONG  PhyBuff,PhyBuffEnd,IntialInStartBuffAddr,TotalBytesTransfered;	
	ULONG  HcCommandStatus;//,HcInterruptStatusreg,HcDoneHeadreg,HCCADoneHead;
	//ohci_status.DevAddress = 0;
	printf(("ENTERED IN TO THE GetHubDescriptor_OHCI\n"));

	OHCI_ControlList_Enable(0);
		
	// Allocate Buffer for PhysCbp
	Cbp=(PUCHAR)SETUP_BUFF_ADDR_OHCI;//dx->VirAddr+32;
	PhysCbp=((u32_t)(Cbp+0));
	Buff = (PUCHAR)SETUP_BUFF_ADDR_OHCI+20;
	PhyBuff = ((u32_t)(Buff+0));
		
	//For the 8 bytes of setup stage
	*(Cbp+0)=0xA0;
	*(Cbp+1)=USB_REQ_GET_DESCRIPTOR;
	*(Cbp+2)=DevDescIndex;
	*(Cbp+3)=DevDescType;
	*(Cbp+4)=0x00;
	*(Cbp+5)=0x00;
	*(Cbp+6)=DevDescLength;
	*(Cbp+7)=0x00;

	Bend=(PUCHAR)(Cbp+7);
	PhysBend=(ULONG)((PUCHAR)PhysCbp+7);
	BuffEnd = (PUCHAR)(Buff + 63);
	PhyBuffEnd = ((u32_t)(BuffEnd+0));

	pTD4 = Get_Free_GTD();
	PhyspTD4=((u32_t) pTD4);
	printf("\npTD3 Physical Address: %x",PhyspTD4);
	printf("\npTD3 Virtual Address: %x",pTD4);
	pTD4->TDdata=NULL;
	pTD4->Cbp=NULL;
	pTD4->NextTD=NULL;
	pTD4->Bend=NULL;
	//Initialize the next Null TD 
	pTD3 = Get_Free_GTD();
	PhyspTD3=((u32_t) pTD3);
	printf("\npTD3 Physical Address: %x",PhyspTD3);
	printf("\npTD3 Virtual Address: %x",pTD3);
	pTD3->TDdata=0xF3080000;
	pTD3->Cbp=NULL;
	pTD3->NextTD=PhyspTD4;
	pTD3->Bend=NULL;
	//Initialize the next TD for SetAddress status stage
	pTD2=Get_Free_GTD();
	//Get the Physical address for this ED
	PhyspTD2=((u32_t) pTD2);
	printf("\npTD2 Physical Address: %x",PhyspTD2);
	printf("\npTD2 Virtual Address: %x",pTD2);
	pTD2->TDdata=0xf3140000;		// Buffer rounding bit is also set.
	pTD2->Cbp=PhyBuff;
	pTD2->NextTD=PhyspTD3;
	pTD2->Bend=PhyBuffEnd;
	IntialInStartBuffAddr = PhyBuff;
	//Initialize the first TD for SetAddress setup stage
	pTD1=Get_Free_GTD();
	//Get the Physical address for this TD
	PhyspTD1=((u32_t) pTD1);
	printf("\npTD1 Physical Address: %x",PhyspTD1);
	printf("\npTD1 Virtual Address: %x",pTD1);
	pTD1->TDdata=0xf2000000;
	pTD1->Cbp=PhysCbp;
	pTD1->NextTD=PhyspTD2;
	pTD1->Bend=PhysBend;

	pED0=Get_Free_ED();
	PhyspED0=((u32_t) pED0);
	printf("\nED0 Physical Address: %x",PhyspED0);
	printf("\nED0 Virtual Address: %x",pED0);
	//Initialize this first ED
	pED0->EDdata=0x00400000|(ohci_status.DevAddress);			//0x00080000 changed to 0x00400000
	pED0->TailP=PhyspTD4; 
	pED0->HeadP=PhyspTD1;
	pED0->NextED=NULL; //PhyspED1.LowPart;

	// Write to Control Head ED register the Physical Address of the first ED
	writew(PhyspED0,HcCtrlHeadED(ohci_base));
//	printf("\nControl Head Initialized to: %x",readw(0x180a4020));

	// Write to HcCommandStatus to tell control list filled
	HcCommandStatus=readw(HcCMDSTS(ohci_base));
	HcCommandStatus=HcCommandStatus|0x00000002; 		//only control list filled bit is set in hccommandstatus register & bulk list filled
	printf("the value of HcCommandStatus is 		%x\n",HcCommandStatus);
	writew(HcCommandStatus,HcCMDSTS(ohci_base));
	printf("the value of hccommand status register%x\n",readw(HcCMDSTS(ohci_base)));

	// enabling control list enable
	OHCI_ControlList_Enable(1);
	printf("after enable\n");
	printf("pED0->EDdata			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED			:%x\n",pED0->NextED);
	HcCommandStatus=readw(HcCMDSTS(ohci_base)/*0x180a4008*/);
//	printf("HcHCCA							:%x\n",readw(0x180a4018));
//	printf("the value of HcHcca.frame number is 	:%x\n",HcHcca->frame_no);
	printf("HcPeriodCurrentEd					:%x\n",readw(HcPeriodCurrED(ohci_base)/*0x180a401C*/));
//	printf("HcControlHeadEd 					:%x\n",readw(0x180a4020));
//	printf("HcControlCurrentEd					:%x\n",readw(0x180a4024));
	printf(("wait for transaction done interrupt\n"));
	while(!TransferComplete)
	{
//		printf("the value of HcHcca.frame number is 				 :%x\n",HcHcca->frame_no);
//		printf("HcControl									  :%x\n",readw(0x180a4004));
//		printf("HcCommandStatus 								 :%x\n",readw(0x180a4008));
//		printf("HcInterruptStatus								 :%x\n",readw(0x180a400C));
//		printf("HcHCCA										 :%x\n",readw(0x180a4018));
//		printf("HcControlHeadEd 								 :%x\n",readw(0x180a4020));
//		printf("HcControlCurrentEd								 :%x\n",readw(0x180a4024));
//		printf("HcFmRemaining									:%x\n",readw(0x180a4038));
//		printf("HcFmNumber									:%x\n",readw(0x180a403C));
//		printf("HcRhPortStatus									 :%x\n",readw(0x180a4054));
		printf("\n");
	}
	TransferComplete = 0;
	printf("pED0->EDdata			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED			:%x\n",pED0->NextED);
//	HcInterruptStatusreg =readw(0x180a400C);
//	writew(HcInterruptStatusreg,0x180a400C);
//	HcDoneHeadreg = readw(0x180a4030);
//	printf("the value of HcDoneHead char is 	:%x\n",HcDoneHeadreg);
//	printf("the value of HCCADoneHead is		:%x\n",HcHcca->done_head);
	if(HcHcca->done_head == PhyspTD3)	// checking the status of the completion
	{
		printf("the value of pTD3->TDdata is (Status)		:%x\n",pTD3->TDdata);
		printf("the value of pTD2->TDdata is (Data)	 	:%x\n",pTD2->TDdata);
		printf("the value of pTD1->TDdata is (Setup)		:%x\n",pTD1->TDdata);
		if(!(pTD3->TDdata & 0xF0000000))
		{
			printf("Get Hub Descriptor Passed in Driver\n");
			TotalBytesTransfered = (pTD2->Cbp - IntialInStartBuffAddr);
			for(i=0;i<=TotalBytesTransfered;i++)
			{
				printf("the value of the dscr byte  is	:%x\n",(*(Buff+ i)));
			}
			printf("the value of device control endpoint max pak size is %x\n",ohci_status.DevMPS);
			
		}
		else 
		{
			printf("Get Hub Descriptor failed \n");
		}
	}
	else 
	{
		printf("status td not completed yet\n");
	}
	Add_Free_ED_Ring(pED0);
	Add_Free_GTD_Ring(pTD1);		
	Add_Free_GTD_Ring(pTD2);
	Add_Free_GTD_Ring(pTD3);		
	Add_Free_GTD_Ring(pTD4);		
	return true;
}

/* ******************************************************************************** *   
Function Name : int OHCI_SendBulkData(int MPS,char EpNum,int Length)
Description   : This function initates a bulk transfer to the bulk out endpoint in the connected device.
Params        : MPS of the endpoint ,Enpdoint number ,Length to transfer 
Returns       : True:OHCI_SendBulkData Success  / False: OHCI_SendBulkData Fail    
Note          :  
******************************************************************************** */
int OHCI_SendBulkData(int MPS,char EpNum,int Length)
{
	PED pED0;
	PGENTD pTD1,pTD2;
	PUCHAR  Bp;
	PUCHAR Bend;
	ULONG  PhyspTD1,PhyspTD2,PhyspED0,PhysBp,PhysBend;
	ULONG  HcCommandStatus;//,HcDoneHeadreg;
//	ohci_status.DevAddress = 0;
	printf(("ENTERED IN TO THE OHCI_SendBulkData\n"));

	OHCI_BulkList_Enable(0);		// commented as per pci driver flow
		
	// Allocate Buffer for PhysCbp
	Bp=(PUCHAR)DATA_OUT_ADDR_OHCI;//dx->VirAddr+32;
	PhysBp=((u32_t)(Bp+0));
	
	Bend=(PUCHAR)(Bp+(Length-1));
	PhysBend=(ULONG)((PUCHAR)PhysBp+(Length-1));
	printf("\nCbp Physical Address: %x",PhysBp);
	printf("\nCbp Virtual Address: %x",Bp);
	printf("\nPhysBend Physical Address: %x",PhysBend);
	printf("\nBend Virtual Address: %x",Bend);
//	memset((UCHAR *)DATA_OUT_ADDR_OHCI,'A',Length);		// commented for checking mass storage
	//Initialize the next Null TD 
	pTD2 = Get_Free_GTD();
	PhyspTD2=((u32_t) pTD2);

	printf("\npTD3 Physical Address: %x",PhyspTD2);
	printf("\npTD3 Virtual Address: %x",&pTD2);

	pTD2->TDdata=0xF0000000;	// changed from 0x00000000 to 0xF0000000
	pTD2->Cbp=NULL;
	pTD2->NextTD=NULL;
	pTD2->Bend=NULL;

	//Initialize the first TD for Bulk out transfer
	pTD1=Get_Free_GTD();
	PhyspTD1=((u32_t) pTD1);

	printf("\npTD1 Physical Address: %x",PhyspTD1);
	printf("\npTD1 Virtual Address: %x",pTD1);

	pTD1->TDdata=0xf0080000;
	pTD1->Cbp=PhysBp;
	pTD1->NextTD=PhyspTD2;
	pTD1->Bend=PhysBend;

	// Form the first control ED in 
	pED0=Get_Free_ED();
	PhyspED0=((u32_t) pED0);
	
	printf("\nED0 Physical Address: %x",PhyspED0);
	printf("\nED0 Virtual Address: %x",pED0);

	//Initialize this first ED
	pED0->EDdata=0x00400000|(EpNum << 7) | (ohci_status.DevAddress);	//  (MPS << 16) |
	printf("the value of ped0->eddata is %x",pED0->EDdata);
	printf("the value of devaddress is %x\n",ohci_status.DevAddress);
	pED0->TailP=PhyspTD2; 
	pED0->HeadP=PhyspTD1 | (BulkOutTog << 1);
	pED0->NextED=NULL; //PhyspED1.LowPart;

	printf("before enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);

	// Write to Bulk Head ED register the Physical Address of the first ED
	writew(PhyspED0,HcBulkHeadED(ohci_base));
	printf("\nBulk Head Initialized to		: %x\n",readw(HcBulkHeadED(ohci_base)));

	// Write to HcCommandStatus to tell control list filled
	HcCommandStatus=readw(HcCMDSTS(ohci_base));
	HcCommandStatus=HcCommandStatus|0x00000004;			//only bulk list filled bit is set in hccommandstatus register & bulk list filled
	printf("the value of HcCommandStatus is 		%x\n",HcCommandStatus);
	writew(HcCommandStatus,HcCMDSTS(ohci_base));
	printf("the value of hccommand status register%x\n",readw(HcCMDSTS(ohci_base)));

	// enabling control list enable
	OHCI_BulkList_Enable(1);
	printf("after enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
	
//	HcCommandStatus=readw(0x180a4008);
//	printf("HcHCCA                                                    :%x\n",readw(0x180a4018));
//	printf("the value of HcHcca.frame number is 	   :%x\n",HcHcca->frame_no);
//	printf("HcPeriodCurrentEd                                     :%x\n",readw(0x180a401C));
//	printf("HcControlHeadEd                                       :%x\n",readw(0x180a4020));
//	printf("HcControlCurrentEd                                    :%x\n",readw(0x180a4024));
	printf(("wait for transaction done interrupt\n"));
	while(!TransferComplete)
	{
//		printf("the value of HcHcca.frame number is 				 :%x\n",HcHcca->frame_no);
//		printf("HcControl									  :%x\n",readw(0x180a4004));
//		printf("HcCommandStatus 								 :%x\n",readw(0x180a4008));
//		printf("HcInterruptStatus								 :%x\n",readw(0x180a400C));
//		printf("HcHCCA										 :%x\n",readw(0x180a4018));
//		printf("HcControlHeadEd 								 :%x\n",readw(0x180a4020));
//		printf("HcControlCurrentEd								 :%x\n",readw(0x180a4024));
//		printf("HcFmRemaining									:%x\n",readw(0x180a4038));
//		printf("HcFmNumber									:%x\n",readw(0x180a403C));
//		printf("HcRhPortStatus									 :%x\n",readw(0x180a4054));
		printf("the value of HcHcca.frame number is 	:%x\n",HcHcca->frame_no);
		printf("\n");
	}
	TransferComplete = 0;
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
//	HcInterruptStatusreg =readw(0x180a400C);
//	writew(HcInterruptStatusreg,0x180a400C);
//	HcDoneHeadreg = readw(0x180a4030);
//	printf("the value of HcDoneHead char is %x\n",HcDoneHeadreg);
//	printf("the value of HCCADoneHead is		:%x\n",HcHcca->done_head);
	if(HcHcca->done_head == PhyspTD1) 	// checking the status of the completion
	{
		if(!(pTD1->TDdata & 0xF0000000))
		{
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
//			ohci_status.DevAddress=DevAddress;
			printf("Bulk Out Passed in Driver");
		}
		else 
		{
			printf("Bulk Out failed \n");
			printf("the value of pTD2->TDdata is %x\n",pTD2->TDdata);
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
		}
	}
	else
	{
		printf("status td not completed yet\n");
	}
	if(pED0->HeadP & 0x00000002)
	{
		BulkOutTog = 1;
	}
	else
	{
		BulkOutTog = 0;
	}
	Add_Free_ED_Ring(pED0);
	Add_Free_GTD_Ring(pTD1);		
	Add_Free_GTD_Ring(pTD2);
	
	return true;
}


/* ******************************************************************************** *   
Function Name : int OHCI_ReceiveBulkData(int MPS,char EpNum,int Length)
Description   : This function initates a bulk transfer to the bulk in endpoint in the connected device.
Params        : MPS of the endpoint ,Endpoint number ,Length to transfer 
Returns       : True:OHCI_ReceiveBulkData Success  / False: OHCI_ReceiveBulkData Fail    
Note          :  
******************************************************************************** */
int OHCI_ReceiveBulkData(int MPS,char EpNum,int Length)
{
	PED pED0;
	PGENTD pTD1,pTD2;
	PUCHAR  Bp;
	PUCHAR Bend;
	ULONG  PhyspTD1,PhyspTD2,PhyspED0,PhysBp,PhysBend;
	ULONG  HcCommandStatus;//,HcInterruptStatusreg,HcDoneHeadreg;
//	ohci_status.DevAddress = 0;
	printf(("ENTERED IN TO THE OHCI_ReceiveBulkData\n"));

	OHCI_BulkList_Enable(0);		// commented as per pci driver flow
		
	// Allocate Buffer for PhysCbp
	Bp=(PUCHAR)DATA_IN_ADDR_OHCI;//dx->VirAddr+32;
	PhysBp=((u32_t)(Bp+0));
	
	Bend=(PUCHAR)(Bp+(Length-1));
	PhysBend=(ULONG)((PUCHAR)PhysBp+(Length-1));
	printf("\nCbp Physical Address: %x",PhysBp);
	printf("\nCbp Virtual Address: %x",Bp);
	printf("\nPhysBend Physical Address: %x",PhysBend);
	printf("\nBend Virtual Address: %x",Bend);

	//Initialize the next Null TD 
	pTD2 = Get_Free_GTD();
	PhyspTD2=((u32_t) pTD2);

	printf("\npTD3 Physical Address: %x",PhyspTD2);
	printf("\npTD3 Virtual Address: %x",&pTD2);

	pTD2->TDdata=0xF0000000;	// changed from 0x00000000 to 0xF0000000
	pTD2->Cbp=NULL;
	pTD2->NextTD=NULL;
	pTD2->Bend=NULL;

	//Initialize the first TD for Bulk out transfer
	pTD1=Get_Free_GTD();
	PhyspTD1=((u32_t) pTD1);

	printf("\npTD1 Physical Address: %x",PhyspTD1);
	printf("\npTD1 Virtual Address: %x",pTD1);

	pTD1->TDdata=0xf0100000;
	pTD1->Cbp=PhysBp;
	pTD1->NextTD=PhyspTD2;
	pTD1->Bend=PhysBend;

	// Form the first control ED in 
	pED0=Get_Free_ED();
	PhyspED0=((u32_t) pED0);
	
	printf("\nED0 Physical Address: %x",PhyspED0);
	printf("\nED0 Virtual Address: %x",pED0);

	//Initialize this first ED
	pED0->EDdata=0x00400000|(EpNum << 7) | (ohci_status.DevAddress);	//  (MPS << 16) |
	printf("the value of ped0->eddata is %x",pED0->EDdata);
	printf("the value of devaddress is %x\n",ohci_status.DevAddress);
	pED0->TailP=PhyspTD2; 
	pED0->HeadP=PhyspTD1 | (BulkInTog << 1);
	pED0->NextED=NULL; //PhyspED1.LowPart;

	printf("before enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);

	// Write to Bulk Head ED register the Physical Address of the first ED
	writew(PhyspED0,HcBulkHeadED(ohci_base));
//	printf("\nBulkl Head Initialized to		: %x\n",readw(0x180a4020));

	// Write to HcCommandStatus to tell control list filled
	HcCommandStatus=readw(HcCMDSTS(ohci_base));
	HcCommandStatus=HcCommandStatus|0x00000004;			//only bulk list filled bit is set in hccommandstatus register & bulk list filled
	printf("the value of HcCommandStatus is 		%x\n",HcCommandStatus);
	writew(HcCommandStatus,HcCMDSTS(ohci_base));
	printf("the value of hccommand status register%x\n",readw(HcCMDSTS(ohci_base)));

	// enabling control list enable
	OHCI_BulkList_Enable(1);
	printf("after enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
	
//	HcCommandStatus=readw(0x180a4008);
//	printf("HcHCCA                                                    :%x\n",readw(0x180a4018));
//	printf("the value of HcHcca.frame number is 	   :%x\n",HcHcca->frame_no);
//	printf("HcPeriodCurrentEd                                     :%x\n",readw(0x180a401C));
//	printf("HcControlHeadEd                                       :%x\n",readw(0x180a4020));
//	printf("HcControlCurrentEd                                    :%x\n",readw(0x180a4024));
	printf(("wait for transaction done interrupt\n"));
	while(!TransferComplete)
	{
//		printf("the value of HcHcca.frame number is 				 :%x\n",HcHcca->frame_no);
//		printf("HcControl									  :%x\n",readw(0x180a4004));
//		printf("HcCommandStatus 								 :%x\n",readw(0x180a4008));
//		printf("HcInterruptStatus								 :%x\n",readw(0x180a400C));
//		printf("HcHCCA										 :%x\n",readw(0x180a4018));
//		printf("HcControlHeadEd 								 :%x\n",readw(0x180a4020));
//		printf("HcControlCurrentEd								 :%x\n",readw(0x180a4024));
//		printf("HcFmRemaining									:%x\n",readw(0x180a4038));
//		printf("HcFmNumber									:%x\n",readw(0x180a403C));
//		printf("HcRhPortStatus									 :%x\n",readw(0x180a4054));
		printf("the value of HcHcca.frame number is 	:%x\n",HcHcca->frame_no);
		printf("\n");
	}
	TransferComplete = 0;
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
//	HcInterruptStatusreg =readw(0x180a400C);
//	writew(HcInterruptStatusreg,0x180a400C);
//	HcDoneHeadreg = readw(0x180a4030);
//	printf("the value of HcDoneHead char is %x\n",HcDoneHeadreg);
//	printf("the value of HCCADoneHead is		:%x\n",HcHcca->done_head);
	if(HcHcca->done_head == PhyspTD1) 	// checking the status of the completion
	{
		if(!(pTD1->TDdata & 0xF0000000))
		{
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
//			ohci_status.DevAddress=DevAddress;
			printf("Bulk In Passed in Driver");
		}
		else 
		{
			printf("Bulk In failed \n");
			printf("the value of pTD2->TDdata is %x\n",pTD2->TDdata);
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
		}
	}
	else
	{
		printf("status td not completed yet\n");
	}
	if(pED0->HeadP & 0x00000002)
	{
		BulkInTog = 1;
	}
	else
	{
		BulkInTog = 0;
	}
	Add_Free_ED_Ring(pED0);
	Add_Free_GTD_Ring(pTD1);		
	Add_Free_GTD_Ring(pTD2);
	
	return true;
}

/* ******************************************************************************** *   
Function Name : int OHCI_BulkLoopData(int MPS,char EpOutNum,char EpInNum,int Length,PUCHAR OutDataBuf,PUCHAR InDataBuf)
Description   : This function initates a bulk transfer to both the bulk out and bulk in  endpoint in the connected bulk loopback device and verify the data received with that of the transmitted.
Params        : MPS of the endpoint ,In Endpoint number,In Endpoint Number,Length to transfer,Out data buffer address,IN data buffer address 
Returns       : True:OHCI_BulkLoopData Success  / False: OHCI_BulkLoopData Fail    
Note          :  
******************************************************************************** */
int OHCI_BulkLoopData(int MPS,char EpOutNum,char EpInNum,int Length,PUCHAR OutDataBuf,PUCHAR InDataBuf)
{
	PED pED0,pED1;
	PGENTD pTD1,pTD2,pTD3,pTD4;
	PUCHAR	BpOut;
	PUCHAR	BpIn;
	PUCHAR BendOut;
	PUCHAR BendIn;
	ULONG  PhyspTD1,PhyspTD2,PhyspED0,PhysBpOut,PhysBendOut;
	ULONG  PhyspTD3,PhyspTD4,PhyspED1;
	ULONG  PhysBpIn,PhysBendIn;
	ULONG  HcCommandStatus;//,HcInterruptStatusreg,HcDoneHeadreg;
	int Error;
//	ohci_status.DevAddress = 0;
	//printf(("ENTERED IN TO THE OHCI_BulkLoopData\n"));

	OHCI_BulkList_Enable(0);		// commented as per pci driver flow
	Error = 0;
		
	// Allocate Buffer for PhysCbp
//	printf("the value of Bpin %x\n",DATA_IN_ADDR_OHCI);
	BpIn=InDataBuf;
	//printf("\nBpIn Virtual Address - first: %x",BpIn);
	PhysBpIn=((u32_t)(BpIn+0));
	//printf("\nBpIn Virtual Address - second: %x",BpIn);
	//printf("the value of Bpin %x\n",DATA_OUT_ADDR_OHCI);
	BpOut=OutDataBuf;//dx->VirAddr+32;
	//printf("\nBpOut Virtual Address - first: %x",BpOut);
	PhysBpOut=((u32_t)(BpOut+0));

	BendOut=(PUCHAR)(BpOut+(Length-1));
	PhysBendOut=(ULONG)((PUCHAR)PhysBpOut+(Length-1));
	BendIn=(PUCHAR)(BpIn+(Length-1));
	PhysBendIn=(ULONG)((PUCHAR)PhysBpIn+(Length-1));
#if 0
	printf("\nBpOut Physical Address: %x",PhysBpOut);
	printf("\nBpOut Virtual Address: %x",BpOut);
	printf("\nBendOut Physical Address: %x",PhysBendOut);
	printf("\nBendOut Virtual Address: %x",BendOut);
	printf("the value of DATA_IN_ADDR_OHCI %x\n",DATA_IN_ADDR_OHCI);
	printf("\nBpIn Physical Address: %x",PhysBpIn);
	printf("\nBpIn Virtual Address - third: %x",BpIn);
	printf("\nBendIn Physical Address: %x",PhysBendIn);
	printf("\nBendIn Virtual Address: %x",BendIn);
#endif
//	memset((UCHAR *)DATA_OUT_ADDR_OHCI,'A',Length);
//	memset((UCHAR *)DATA_IN_ADDR_OHCI,0,8192);
	// framing for bulk out endpoint
	pTD2 = Get_Free_GTD();
	PhyspTD2=((u32_t) pTD2);
#if 0	
	printf("\npTD2 Physical Address: %x",PhyspTD2);
	printf("\npTD2 Virtual Address: %x",pTD2);
#endif	
	pTD2->TDdata=0xF0000000;	// changed from 0x00000000 to 0xF0000000
	pTD2->Cbp=NULL;
	pTD2->NextTD=NULL;
	pTD2->Bend=NULL;
	
	//Initialize the first TD for Bulk out transfer
	pTD1=Get_Free_GTD();
	PhyspTD1=((u32_t) pTD1);
#if 0	
	printf("\npTD1 Physical Address: %x",PhyspTD1);
	printf("\npTD1 Virtual Address: %x",pTD1);
#endif	
	pTD1->TDdata=0xf0080000;
	pTD1->Cbp=PhysBpOut;
	pTD1->NextTD=PhyspTD2;
	pTD1->Bend=PhysBendOut;
	
	// Form the first control ED in 
	pED0=Get_Free_ED();
	PhyspED0=((u32_t) pED0);
#if 0	
	printf("\nED0 Physical Address: %x",PhyspED0);
	printf("\nED0 Virtual Address: %x",pED0);
#endif		
	//Initialize this first ED
	pED0->EDdata=0x00400000|(EpOutNum << 7) | (ohci_status.DevAddress);	//	(MPS << 16) |
#if 0	
	printf("the value of ped0->eddata is %x",pED0->EDdata);
	printf("the value of devaddress is %x\n",ohci_status.DevAddress);
#endif	
	pED0->TailP=PhyspTD2; 
	pED0->HeadP=PhyspTD1 | (BulkOutTog << 1);
	pED0->NextED=NULL; 
#if 0	
	printf("before enable\n");
	printf("pED0->EDdata			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED			:%x\n",pED0->NextED);
	printf("pTD1->TDdata 			:%x\n",pTD1->TDdata);
	printf("pTD1->Cbp 			:%x\n",pTD1->Cbp);
	printf("pTD1->NextTD 			:%x\n",pTD1->NextTD);
	printf("pTD1->Bend 			:%x\n",pTD1->Bend);
#endif
	// framing for bulk in endpoint
	//Initialize the next Null TD 
	pTD3 = Get_Free_GTD();
	PhyspTD3=((u32_t) pTD3);
#if 0	
	printf("\npTD3 Physical Address: %x",PhyspTD3);
	printf("\npTD3 Virtual Address: %x",pTD3);
#endif	
	pTD3->TDdata=0xF0000000;	// changed from 0x00000000 to 0xF0000000
	pTD3->Cbp=NULL;
	pTD3->NextTD=NULL;
	pTD3->Bend=NULL;
	
	//Initialize the first TD for Bulk out transfer
	pTD4=Get_Free_GTD();
	PhyspTD4=((u32_t) pTD4);
#if 0	
	printf("\npTD4 Physical Address: %x",PhyspTD4);
	printf("\npTD4 Virtual Address: %x",pTD4);
#endif	
	pTD4->TDdata=0xf0100000;
	pTD4->Cbp=PhysBpIn;
	pTD4->NextTD=PhyspTD3;
	pTD4->Bend=PhysBendIn;
#if 0	
	printf("pTD3->TDdata 			:%x\n",pTD3->TDdata);
	printf("pTD3->Cbp 			:%x\n",pTD3->Cbp);
	printf("pTD3->NextTD 			:%x\n",pTD3->NextTD);
	printf("pTD3->Bend 			:%x\n",pTD3->Bend);
	printf("pTD4->TDdata 			:%x\n",pTD4->TDdata);
	printf("pTD4->Cbp 			:%x\n",pTD4->Cbp);
	printf("pTD4->NextTD 			:%x\n",pTD4->NextTD);
	printf("pTD4->Bend 			:%x\n",pTD4->Bend);
#endif
	// Form the first control ED in 
	pED1=Get_Free_ED();
	PhyspED1=((u32_t) pED1);
#if 0	
	printf("\nED1 Physical Address: %x",PhyspED1);
	printf("\nED1 Virtual Address: %x",pED1);
#endif	
	//Initialize this first ED
	pED1->EDdata=0x00400000|(EpInNum << 7) | (ohci_status.DevAddress);	//	(MPS << 16) |
#if 0
	printf("the value of ped0->eddata is %x",pED0->EDdata);
	printf("the value of devaddress is %x\n",ohci_status.DevAddress);
#endif
	pED1->TailP=PhyspTD3; 
	pED1->HeadP=PhyspTD4 | (BulkInTog << 1);
	pED1->NextED=NULL; //PhyspED1.LowPart;
	// linking the two eds		
	pED0->NextED=PhyspED1; 
#if 0
	printf("before enable\n");
	printf("pED0->EDdata			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED			:%x\n",pED0->NextED);
	printf("before enable\n");
	printf("pED1->EDdata			:%x\n",pED1->EDdata);
	printf("pED1->TailP 			:%x\n",pED1->TailP);
	printf("pED1->HeadP 			:%x\n",pED1->HeadP);
	printf("pED1->NextED			:%x\n",pED1->NextED);
#endif
	writew(PhyspED0,HcBulkHeadED(ohci_base));
	//printf("\nBulkl Head Initialized to		: %x\n",readw(HcBulkHeadED));

	// Write to HcCommandStatus to tell Bulk list filled
	HcCommandStatus=readw(HcCMDSTS(ohci_base));
	HcCommandStatus=HcCommandStatus|0x00000004;			//only bulk list filled bit is set in hccommandstatus register & bulk list filled
	//printf("the value of HcCommandStatus is 		%x\n",HcCommandStatus);
	writew(HcCommandStatus,HcCMDSTS(ohci_base));
//	printf("the value of hccommand status register%x\n",readw(0x180a4008));

	// enabling control list enable
	OHCI_BulkList_Enable(1);
#if 0
	printf("after enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
	printf("pED1->EDdata			:%x\n",pED1->EDdata);
	printf("pED1->TailP 			:%x\n",pED1->TailP);
	printf("pED1->HeadP 			:%x\n",pED1->HeadP);
	printf("pED1->NextED			:%x\n",pED1->NextED);
#endif	
//	HcCommandStatus=readw(0x180a4008);
//	printf("HcHCCA                                                    :%x\n",readw(0x180a4018));
//	printf("the value of HcHcca.frame number is 	   :%x\n",HcHcca->frame_no);
//	printf("HcPeriodCurrentEd                                     :%x\n",readw(0x180a401C));
//	printf("HcControlHeadEd                                       :%x\n",readw(0x180a4020));
//	printf("HcControlCurrentEd                                    :%x\n",readw(0x180a4024));
//	printf(("wait for transaction done interrupt\n"));
	while(!TransferComplete)
	{
//		printf("the value of HcHcca.frame number is 				 :%x\n",HcHcca->frame_no);
//		printf("HcControl									  :%x\n",readw(0x180a4004));
//		printf("HcCommandStatus 								 :%x\n",readw(0x180a4008));
//		printf("HcInterruptStatus								 :%x\n",readw(0x180a400C));
//		printf("HcHCCA										 :%x\n",readw(0x180a4018));
//		printf("HcControlHeadEd 								 :%x\n",readw(0x180a4020));
//		printf("HcControlCurrentEd								 :%x\n",readw(0x180a4024));
//		printf("HcFmRemaining									:%x\n",readw(0x180a4038));
//		printf("HcFmNumber									:%x\n",readw(0x180a403C));
//		printf("HcRhPortStatus									 :%x\n",readw(0x180a4054));
		printf("HcBulkHeadEd										   :%x\n",readw(HcBulkHeadED(ohci_base)/*0x180a4028*/));
		printf("HcBulkCurrentEd 									  :%x\n",readw(HcBulkCurrED(ohci_base)/*0x180a402C*/));
		printf("the value of HcHcca.frame number is 	:%x\n",HcHcca->frame_no);
		printf("\n");
	}
	TransferComplete = 0;
#if 0
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
	printf("pTD1->TDdata 			:%x\n",pTD1->TDdata);
	printf("pTD1->Cbp 			:%x\n",pTD1->Cbp);
	printf("pTD1->NextTD 			:%x\n",pTD1->NextTD);
	printf("pTD1->Bend 			:%x\n",pTD1->Bend);
	printf("pED1->EDdata			:%x\n",pED1->EDdata);
	printf("pED1->TailP 			:%x\n",pED1->TailP);
	printf("pED1->HeadP 			:%x\n",pED1->HeadP);
	printf("pED1->NextED			:%x\n",pED1->NextED);
	printf("pTD4->TDdata 			:%x\n",pTD4->TDdata);
	printf("pTD4->Cbp 			:%x\n",pTD4->Cbp);
	printf("pTD4->NextTD 			:%x\n",pTD4->NextTD);
	printf("pTD4->Bend 			:%x\n",pTD4->Bend);
#endif
//	HcInterruptStatusreg =readw(0x180a400C);
//	writew(HcInterruptStatusreg,0x180a400C);
//	HcDoneHeadreg = readw(0x180a4030);
//	printf("the value of HcDoneHead char is %x\n",HcDoneHeadreg);
//	printf("the value of HCCADoneHead is		:%x\n",HcHcca->done_head);
	if(HcHcca->done_head == PhyspTD4) 	// checking the status of the completion
	{
		if(!(pTD1->TDdata & 0xF0000000))
		{
			//printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
			//printf("the value of pTD4->TDdata is %x\n",pTD4->TDdata);
//			ohci_status.DevAddress=DevAddress;
			//printf("Bulk Loop Passed in Driver");
			Error = 0;
		}
		else 
		{
			printf("Bulk Loop failed \n");
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
			printf("the value of pTD4->TDdata is %x\n",pTD4->TDdata);
			Error = 1;
		}
	}
	else
	{
		printf("td not completed yet\n");
	}		
	if(pED0->HeadP & 0x00000002)
	{
		BulkOutTog = 1;
	}
	else
	{
		BulkOutTog = 0;
	}
	if(pED1->HeadP & 0x00000002)
	{
		BulkInTog = 1;
	}
	else
	{
		BulkInTog = 0;
	}
	Add_Free_ED_Ring(pED0);
	Add_Free_ED_Ring(pED1);
	Add_Free_GTD_Ring(pTD1);		
	Add_Free_GTD_Ring(pTD2);
	Add_Free_GTD_Ring(pTD3);		
	Add_Free_GTD_Ring(pTD4);
	if(Error)
	{
		return 1;
	}
	else
	{
		return true;
	}
}

/* ******************************************************************************** *   
Function Name : int OHCI_SendIsoData(int MPS,char EpNum,int Framecount,PUCHAR OutDataBuf)
Description   : This function initates a iso transfer to the iso out endpoint in the connected device.
Params        : MPS of the endpoint ,Iso Out Endpoint number ,FrameCount value,Out data buffer address 
Returns       : True:OHCI_SendIsoData Success  / False: OHCI_SendIsoData Fail    
Note          :  
******************************************************************************** */
int OHCI_SendIsoData(int MPS,char EpNum,int Framecount,PUCHAR OutDataBuf)
{
	unsigned int i;
	unsigned short int CurrentFrame;
	unsigned short int StartFrame;
	PED pED0;
	PISOTD pTD1,pTD2;
	PUCHAR  Bp;
	PUCHAR Bend;
	ULONG  PhyspTD1,PhyspTD2,PhyspED0,PhysBp,PhysBend;
//	ULONG  HcCommandStatus,HcInterruptStatusreg,HcDoneHeadreg;
	printf(("ENTERED IN TO THE OHCI_SendIsoData\n"));

	OHCI_IsoList_Enable(0);

	// Allocate Buffer for PhysCbp
	Bp=(PUCHAR)OutDataBuf;
	PhysBp=((u32_t)(Bp+0));
	
//	Bend=(PUCHAR)(Bp+((Framecount*MPS)-1));

	Bend=(PUCHAR)(Bp+((1*MPS)-1));//api_new

//	PhysBend=(ULONG)((PUCHAR)PhysBp+((Framecount*MPS)-1));
		PhysBend=(ULONG)((PUCHAR)PhysBp+((1*MPS)-1));//api_new
	printf("\nCbp Physical Address: %x",PhysBp);
	printf("\nCbp Virtual Address: %x",Bp);
	printf("\nPhysBend Physical Address: %x",PhysBend);
	printf("\nBend Virtual Address: %x",Bend);
	memset((UCHAR *)DATA_OUT_ADDR_OHCI,'A',8192);
	//Initialize the next Null TD 
	pTD2 = Get_Free_ISOTD();
	PhyspTD2=((u32_t) pTD2);

	printf("\npTD2 Physical Address: %x",PhyspTD2);
	printf("\npTD2 Virtual Address: %x",&pTD2);
	// Framing NULL TD
	pTD2->TDdata=0xF0000000;	
	pTD2->Cbp=NULL;
	pTD2->NextTD=NULL;
	pTD2->Bend=NULL;
	pTD2->offset10=NULL;	
	pTD2->offset32=NULL;	
	pTD2->offset54=NULL;	
	pTD2->offset76=NULL;	

	//Initialize the first TD for Bulk out transfer
	pTD1=Get_Free_ISOTD();
	PhyspTD1=((u32_t) pTD1);

	printf("\npTD1 Physical Address: %x",PhyspTD1);
	printf("\npTD1 Virtual Address: %x",pTD1);

	pTD1->TDdata=0xf0000000 |(Framecount << 24);
	pTD1->Cbp=PhysBp;
	pTD1->NextTD=PhyspTD2;
	pTD1->Bend=PhysBend;
	pTD1->offset10=  0x0000F000 | ((MPS-1)<< 16);	// the condition code field fo first PSW field is set to NOT ACCESSED.
	pTD1->offset32=NULL ;	
	pTD1->offset54=NULL ;	
	pTD1->offset76=NULL ;	

	// Form the first ED in 
	pED0=Get_Free_ED();
	PhyspED0=((u32_t) pED0);
	
	printf("\nED0 Physical Address: %x",PhyspED0);
	printf("\nED0 Virtual Address: %x",pED0);

	//Initialize this first ED
	pED0->EDdata=0x00000000|(MPS << 16)|(ISOFMT)|EPOUTDIR |(EpNum << 7) | (ohci_status.DevAddress);	//  (MPS << 16) |
	printf("\nthe value of ped0->eddata is %x",pED0->EDdata);
	printf("\nthe value of devaddress is %x\n",ohci_status.DevAddress);
	pED0->TailP=PhyspTD2; 
	pED0->HeadP=PhyspTD1;
	pED0->NextED=NULL; //PhyspED1.LowPart;

	printf("before enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);

	// Write to ed address in the hcca area.
	for(i=0;i<32;i++)
	{
		HcHcca->int_table[i] = PhyspED0;	// first 8 entries are filled with the iso endpoint
	}
	printf("\nAfter hchcca int_table allocation");
	// updating the SF field of IsoTD
	CurrentFrame = HcHcca->frame_no;
	StartFrame = CurrentFrame + 0x100;
	pTD1->TDdata |= StartFrame;
	//enabling periodic schedule and iso list
	OHCI_IsoList_Enable(1);
	printf("after enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
	// wait for transaction done interrupt
	while(!TransferComplete)
	{
		//printf("the value of HcHcca.frame number is 	:%x\n",HcHcca->frame_no);
		//printf("\n");
	}
	TransferComplete = 0;
	printf("pED0->EDdata			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED			:%x\n",pED0->NextED);
	
	if(HcHcca->done_head == PhyspTD1) 	// checking the status of the completion
	{
		if(!(pTD1->TDdata & 0xF0000000))
		{
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
			printf("Iso Out Passed in Driver");
		}
		else 
		{
			printf("Iso Out failed \n");
			printf("the value of pTD2->TDdata is %x\n",pTD2->TDdata);
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
		}
	}
	else
	{
		printf("Itd not completed yet\n");
	}	OHCI_IsoList_Enable(0);//api_new _0531
	Add_Free_ED_Ring(pED0);
	Add_Free_ISOTD_Ring(pTD1);		
	Add_Free_ISOTD_Ring(pTD2);
	
	return TRUE;
}

/* ******************************************************************************** *   
Function Name : int OHCI_ReceiveIsoData(int MPS,char EpNum,int Framecount,PUCHAR InDataBuf)
Description   : This function initates a iso transfer to the Iso in endpoint in the connected device.
Params        : MPS of the endpoint ,Iso Out Endpoint number ,FrameCount value,Out data buffer address 
Returns       : True:OHCI_ReceiveIsoData Success  / False: OHCI_ReceiveIsoData Fail    
Note          :  
******************************************************************************** */
int OHCI_ReceiveIsoData(int MPS,char EpNum,int Framecount,PUCHAR InDataBuf)
{
	unsigned int i;
	unsigned short int CurrentFrame;
	unsigned short int StartFrame;
	PED pED0;
	PISOTD pTD1,pTD2;
	PUCHAR  Bp;
	PUCHAR Bend;
	ULONG  PhyspTD1,PhyspTD2,PhyspED0,PhysBp,PhysBend;
	printf(("ENTERED IN TO THE OHCI_ReceiveIsoData\n"));

	OHCI_IsoList_Enable(0);

	// Allocate Buffer for PhysCbp
	Bp=(PUCHAR)InDataBuf;
	PhysBp=((u32_t)(Bp+0));
	
//	Bend=(PUCHAR)(Bp+((Framecount*MPS)-1));
	Bend=(PUCHAR)(Bp+(1*MPS)-1);//api_new

//	PhysBend=(ULONG)((PUCHAR)PhysBp+((Framecount*MPS)-1));
		PhysBend=(ULONG)((PUCHAR)PhysBp+((1*MPS)-1));//api_new

	printf("\nCbp Physical Address: %x",PhysBp);
	printf("\nCbp Virtual Address: %x",Bp);
	printf("\nPhysBend Physical Address: %x",PhysBend);
	printf("\nBend Virtual Address: %x",Bend);
	memset((UCHAR *)DATA_OUT_ADDR_OHCI,'A',8192);//api_new
	//Initialize the next Null TD 
	pTD2 = Get_Free_ISOTD();
	PhyspTD2=((u32_t) pTD2);

	printf("\npTD3 Physical Address: %x",PhyspTD2);
	printf("\npTD3 Virtual Address: %x",pTD2);
	// Framing the NULL TD
	pTD2->TDdata=0xF0000000;	
	pTD2->Cbp=NULL;
	pTD2->NextTD=NULL;
	pTD2->Bend=NULL;
	pTD2->offset10=NULL;	
	pTD2->offset32=NULL;	
	pTD2->offset54=NULL;	
	pTD2->offset76=NULL;	

	//Initialize the first TD for Bulk out transfer
	pTD1=Get_Free_ISOTD();
	PhyspTD1=((u32_t) pTD1);

	printf("\npTD1 Physical Address: %x",PhyspTD1);
	printf("\npTD1 Virtual Address: %x",pTD1);

	pTD1->TDdata=0xf0000000 |(Framecount << 24);
	pTD1->Cbp=PhysBp;
	pTD1->NextTD=PhyspTD2;
	pTD1->Bend=PhysBend;
	pTD1->offset10=  0x0000F000 | ((MPS-1)<< 16);	// the condition code field fo first PSW is set to NOT ACCESSED.
	pTD1->offset32=NULL ;	
	pTD1->offset54=NULL ;	
	pTD1->offset76=NULL ;	

	// Form the first  ED in 
	pED0=Get_Free_ED();
	PhyspED0=((u32_t) pED0);
	
	printf("\nED0 Physical Address: %x",PhyspED0);
	printf("\nED0 Virtual Address: %x",pED0);

	//Initialize this first ED
	pED0->EDdata=0x00000000|(MPS << 16)|(ISOFMT)|EPINDIR |(EpNum << 7) | (ohci_status.DevAddress);	//  (MPS << 16) |
	printf("the value of ped0->eddata is %x",pED0->EDdata);
	printf("the value of devaddress is %x\n",ohci_status.DevAddress);
	pED0->TailP=PhyspTD2; 
	pED0->HeadP=PhyspTD1;
	pED0->NextED=NULL; //PhyspED1.LowPart;

	printf("before enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);

	// Write to ed address in the hcca area.
	for(i=0;i<32;i++)
	{
		HcHcca->int_table[i] = PhyspED0;	// first 8 entries are filled with the iso endpoint
	}
	// updating the SF field of IsoTD
	CurrentFrame = HcHcca->frame_no;
	StartFrame = CurrentFrame + 0x100;
	pTD1->TDdata |= StartFrame;
	//enabling periodic schedule and iso list
	OHCI_IsoList_Enable(1);
	printf("after enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
	// wait for transaction done interrupt
	while(!TransferComplete)
	{
		printf("the value of HcHcca.frame number is 	:%x\n",HcHcca->frame_no);
		printf("\n");
	}
	TransferComplete = 0;
	printf("pED0->EDdata			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED			:%x\n",pED0->NextED);
	
	if(HcHcca->done_head == PhyspTD1) 	// checking the status of the completion
	{
		if(!(pTD1->TDdata & 0xF0000000))
		{
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
			printf("Iso In Passed in Driver");
		}
		else 
		{
			printf("Iso In failed \n");
			printf("the value of pTD2->TDdata is %x\n",pTD2->TDdata);
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
			return FALSE;
		}
	}
	else
	{
		printf("Itd not completed yet\n");
	}
	Add_Free_ED_Ring(pED0);
	Add_Free_ISOTD_Ring(pTD1);		
	Add_Free_ISOTD_Ring(pTD2);
	
	return TRUE;
}


/* ******************************************************************************** *   
Function Name : int OHCI_ReceiveIntrData(int MPS,char EpNum)
Description   : This function initates a interrupt in transfer to a interrupt in endpoint to the connected device(HUB).
Params        : MPS of the endpoint ,Intr in endpoint number 
Returns       : True:OHCI_ReceiveIntrData Success  / False: OHCI_ReceiveIntrData Fail    
Note          :  
******************************************************************************** */
int OHCI_ReceiveIntrData(int MPS,char EpNum)
{
	PED pED0;
	PGENTD pTD1,pTD2;
	PUCHAR  Bp;
	PUCHAR Bend;
	ULONG  PhyspTD1,PhyspTD2,PhyspED0,PhysBp,PhysBend;
	ULONG  HcCommandStatus;//,HcDoneHeadreg,HcInterruptStatusreg,;
//	ohci_status.DevAddress = 0;
	printf(("ENTERED IN TO THE OHCI_ReceiveIntrData\n"));

	OHCI_PeriodicList_Enable(0);		// commented as per pci driver flow
		
	// Allocate Buffer for PhysCbp
	Bp=(PUCHAR)INTR_IN_ADDR_OHCI;//dx->VirAddr+32;
	PhysBp=((u32_t)(Bp+0));
	Bend=(PUCHAR)(Bp+((1*MPS)-1));//api_new
//	Bend=(PUCHAR)(Bp+(MPS-1));
	PhysBend=(ULONG)((PUCHAR)PhysBp+(MPS-1));
	printf("\nCbp Physical Address: %x",PhysBp);
	printf("\nCbp Virtual Address: %x",Bp);
	printf("\nPhysBend Physical Address: %x",PhysBend);
	printf("\nBend Virtual Address: %x",Bend);

	//Initialize the next Null TD 
	pTD2 = Get_Free_GTD();
	PhyspTD2=((u32_t) pTD2);

	printf("\npTD2 Physical Address: %x",PhyspTD2);
	printf("\npTD2 Virtual Address: %x",pTD2);

	pTD2->TDdata=0xF0000000;	// changed from 0x00000000 to 0xF0000000
	pTD2->Cbp=NULL;
	pTD2->NextTD=NULL;
	pTD2->Bend=NULL;

	//Initialize the first TD for Bulk out transfer
	pTD1=Get_Free_GTD();
	PhyspTD1=((u32_t) pTD1);

	printf("\npTD1 Physical Address: %x",PhyspTD1);
	printf("\npTD1 Virtual Address: %x",pTD1);

	pTD1->TDdata=0xf0100000;
	pTD1->Cbp=PhysBp;
	pTD1->NextTD=PhyspTD2;
	pTD1->Bend=PhysBend;

	// Form the first control ED in 
	pED0=Get_Free_ED();
	PhyspED0=((u32_t) pED0);
	
	printf("\nED0 Physical Address: %x",PhyspED0);
	printf("\nED0 Virtual Address: %x",pED0);

	//Initialize this first ED
	pED0->EDdata=0x00010000|(EpNum << 7) | (ohci_status.DevAddress);	//  (MPS << 16) |mps fixed as 1 byte for hub device.
	printf("the value of ped0->eddata is %x",pED0->EDdata);
	printf("the value of devaddress is %x\n",ohci_status.DevAddress);
	pED0->TailP=PhyspTD2; 
	pED0->HeadP=PhyspTD1 | (IntrInTog << 1);
	pED0->NextED=NULL; //PhyspED1.LowPart;

	printf("before enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);

	HcHcca->int_table[0] = PhyspED0;
	// enabling control list enable
	OHCI_PeriodicList_Enable(1);
	printf("after enable\n");
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
	
	HcCommandStatus=readw(HcCMDSTS(ohci_base));
//	printf("HcHCCA                                                    :%x\n",readw(0x180a4018));
//	printf("the value of HcHcca.frame number is 	   :%x\n",HcHcca->frame_no);
	printf("HcPeriodCurrentEd                                     :%x\n",readw(HcPeriodCurrED(ohci_base)/*0x180a401C*/));
//	printf("HcControlHeadEd                                       :%x\n",readw(0x180a4020));
//	printf("HcControlCurrentEd                                    :%x\n",readw(0x180a4024));
	printf(("wait for transaction done interrupt\n"));
	while(!TransferComplete)
	{
//		printf("the value of HcHcca.frame number is 				 :%x\n",HcHcca->frame_no);
//		printf("HcControl									  :%x\n",readw(0x180a4004));
//		printf("HcCommandStatus 								 :%x\n",readw(0x180a4008));
//		printf("HcInterruptStatus								 :%x\n",readw(0x180a400C));
//		printf("HcHCCA										 :%x\n",readw(0x180a4018));
//		printf("HcControlHeadEd 								 :%x\n",readw(0x180a4020));
//		printf("HcControlCurrentEd								 :%x\n",readw(0x180a4024));
//		printf("HcFmRemaining									:%x\n",readw(0x180a4038));
//		printf("HcFmNumber									:%x\n",readw(0x180a403C));
//		printf("HcRhPortStatus									 :%x\n",readw(0x180a4054));
		printf("\n");
	}
	TransferComplete = 0;
	printf("pED0->EDdata 			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED 			:%x\n",pED0->NextED);
//	HcInterruptStatusreg =readw(0x180a400C);
//	writew(HcInterruptStatusreg,0x180a400C);
//	HcDoneHeadreg = readw(0x180a4030);
//	printf("the value of HcDoneHead char is %x\n",HcDoneHeadreg);
//	printf("the value of HCCADoneHead is		:%x\n",HcHcca->done_head);
	if(HcHcca->done_head == PhyspTD1) 	// checking the status of the completion
	{
		if(!(pTD1->TDdata & 0xF0000000))
		{
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
//			ohci_status.DevAddress=DevAddress;
			printf("Intr In Passed in Driver");
		}
		else 
		{
			printf("Intr In failed \n");
			printf("the value of pTD2->TDdata is %x\n",pTD2->TDdata);
			printf("the value of pTD1->TDdata is %x\n",pTD1->TDdata);
		}
	}
	else
	{
		printf("status td not completed yet\n");
	}
	if(pED0->HeadP & 0x00000002)
	{
		IntrInTog = 1;
	}
	else
	{
		IntrInTog = 0;
	}
	Add_Free_ED_Ring(pED0);
	Add_Free_GTD_Ring(pTD1);		
	Add_Free_GTD_Ring(pTD2);
	
	return true;
}

void	Display_OHCIMenu(void)
{

	printf("LII OHCI Testing.\n");
	printf("==================================================\n");
	printf("1.Base Test\n");
	printf("2.Hub Test\n");
	printf("3.Ctrl Test\n");
	printf("4.Bulk Test\n");
	printf("5.INTR Test\n");
	printf("6.Isochronous Test\n");
	printf("\nX. Exit EHCI Part Back to Main Menu\n");
	printf("==================================================\n");

}

void Display_OHCIAutoTestMenu(void)
{
	printf("Auto Test\n");
	printf("==================================================\n");
	printf("1.Bulk In/Out Test\n");
	printf("2.Intr In Test\n");
	printf("\nX. Exit Auto Test Back to EHCI Main Menu\n");
	printf("==================================================\n");	
}

void Display_OHCIBASETestMenu(void)
{
	printf("BASE Test\n");
	printf("==================================================\n");
	printf("1.Default Values Check\n");	
//	printf("2.Test Invalid Port Enable\n");			
//	printf("3.Test Port Disable \n");	
//	printf("4.Frame List Increment Test\n");
//	printf("5.To verify that port is enabled after reset\n");
	printf("6.Ohci_reset\n");	
	printf("7.Ohci_start\n");
	printf("8.PrintOhciRegs\n");
	printf("\nX. Exit Base Test Back to EHCI Main Menu\n");
	printf("==================================================\n");	
}

void Display_OHCICtrlMenu(void)
{
	printf("Ctrl Test\n");
	printf("==================================================\n");
	printf("0.Reset Device\n");	
	printf("1.Set Address\n");
	printf("2.Set Config\n");
	printf("3.Get Config\n");	
	printf("4.Set Feature\n");
	printf("5.Clear Feature\n");	
	printf("6.Set Interface\n");
	printf("7.Get Interface\n");	
	printf("8.Get Desc\n");
	printf("9.Get Status\n");	
	printf("a.Printohciregs\n");
	printf("\nX. Exit Ctrl Test Back to EHCI Main Menu\n");
	printf("==================================================\n");	

}

void Display_OHCIIntrMenu(void)
{
	printf("Intr Test\n");
	printf("==================================================\n");
	printf("r.Intr Rx\n");
//	printf("t.Intr Tx\n");
//	printf("a.Intr Rx/Tx auto transfer \n");	
	printf("\nX. Exit Intr Test Back to EHCI Main Menu\n");
	printf("==================================================\n");	

}

void Display_OHCIBulkMenu(void)
{
	printf("Hub Request Test\n");
	printf("==================================================\n");
	printf("r.Bulk Rx\n");
	printf("t.Bulk Tx\n");
	printf("a.Bulk Rx/Tx auto transfer \n");	
	printf("\nX. Exit Bulk Test Back to EHCI Main Menu\n");
	printf("==================================================\n");	

}

void Display_OHCIHubMenu(void)
{
	printf("Hub Request Test\n");
	printf("==================================================\n");
	printf("1.GetHubDescriptor\n");
	printf("2.GetPortStatus\n");
	printf("3.GetHubStatus\n");	
	printf("4.ClearPortFeature\n");
	printf("5.ClearHubFeature\n");
//	printf("6.ResetTT\n");	
//	printf("7.StopTT\n");
	printf("8.SetHubFeature\n");	
	printf("9.SetPortFeature\n");	
//	printf("a.ClearTTBuffer\n");
//	printf("b.SetHubDescriptor\n");	
//	printf("c.GetTTState\n");		
	printf("\nX. Exit Hub Request Back to EHCI Main Menu\n");
	printf("==================================================\n");	
}

//viswa_iso
void Display_OHCIIsoMenu(void)
{
	printf("Iso Test\n");
	printf("==================================================\n");
	printf("r.Iso Rx\n");
	printf("t.Iso Tx\n");
	printf("a.Iso Rx/Tx auto transfer \n");	
	printf("\nX. Exit Intr Test Back to EHCI Main Menu\n");
	printf("==================================================\n");	

}


int GetDescriptor_OHCI_backup(BYTE DevDescType,BYTE DevDescIndex,int DevDescLength)
{
	unsigned char i;
	PED pED0;
	PGENTD pTD1,pTD2,pTD3,pTD4;
	PUCHAR	Buff;
	PUCHAR  Cbp;
	PUCHAR Bend,BuffEnd;
	ULONG  PhyspTD1,PhyspTD2,PhyspTD3,PhyspTD4,PhyspED0,PhysCbp,PhysBend;
	ULONG  PhyBuff,PhyBuffEnd,IntialInStartBuffAddr,TotalBytesTransfered;	
	ULONG  HcCommandStatus,HcInterruptStatusreg,HcDoneHeadreg;
	//ohci_status.DevAddress = 0;
	printf(("ENTERED IN TO THE GetDescriptor_OHCI\n"));

	OHCI_ControlList_Enable(0);
		
	// Allocate Buffer for PhysCbp
	Cbp=(PUCHAR)SETUP_BUFF_ADDR_OHCI;//dx->VirAddr+32;
	PhysCbp=((u32_t)(Cbp+0));
	Buff = (PUCHAR)SETUP_BUFF_ADDR_OHCI+20;
	PhyBuff = ((u32_t)(Buff+0));
		
	//For the 8 bytes of setup stage
	*(Cbp+0)=0x80;
	*(Cbp+1)=USB_REQ_GET_DESCRIPTOR;
	*(Cbp+2)=0x00;
	*(Cbp+3)=DevDescType;
	*(Cbp+4)=0x00;
	*(Cbp+5)=0x00;
	*(Cbp+6)=DevDescLength;
	*(Cbp+7)=DevDescLength >> 8;

	Bend=(PUCHAR)(Cbp+7);
	PhysBend=(ULONG)((PUCHAR)PhysCbp+7);
//	BuffEnd = (PUCHAR)(Buff + 63);
//	PhyBuffEnd = ((u32_t)(BuffEnd+0));
	BuffEnd = (PUCHAR)(Buff + (DevDescLength-1));
	PhyBuffEnd = ((u32_t)(BuffEnd+0));

	pTD4 = Get_Free_GTD();
	PhyspTD4=((u32_t) pTD4);
	printf("\npTD3 Physical Address: %x",PhyspTD4);
	printf("\npTD3 Virtual Address: %x",pTD4);
	pTD4->TDdata=NULL;
	pTD4->Cbp=NULL;
	pTD4->NextTD=NULL;
	pTD4->Bend=NULL;
	//Initialize the next Null TD 
	pTD3 = Get_Free_GTD();
	PhyspTD3=((u32_t) pTD3);
	printf("\npTD3 Physical Address: %x",PhyspTD3);
	printf("\npTD3 Virtual Address: %x",pTD3);
	pTD3->TDdata=0xF3080000;
	pTD3->Cbp=NULL;
	pTD3->NextTD=PhyspTD4;
	pTD3->Bend=NULL;
	//Initialize the next TD for SetAddress status stage
	pTD2=Get_Free_GTD();
	//Get the Physical address for this ED
	PhyspTD2=((u32_t) pTD2);
	printf("\npTD2 Physical Address: %x",PhyspTD2);
	printf("\npTD2 Virtual Address: %x",pTD2);
	pTD2->TDdata=0xf3140000;		// Buffer rounding bit is also set.
	pTD2->Cbp=PhyBuff;
	pTD2->NextTD=PhyspTD3;
	pTD2->Bend=PhyBuffEnd;
	IntialInStartBuffAddr = PhyBuff;
	//Initialize the first TD for SetAddress setup stage
	pTD1=Get_Free_GTD();
	//Get the Physical address for this TD
	PhyspTD1=((u32_t) pTD1);
	printf("\npTD1 Physical Address: %x",PhyspTD1);
	printf("\npTD1 Virtual Address: %x",pTD1);
	pTD1->TDdata=0xf2000000;
	pTD1->Cbp=PhysCbp;
	pTD1->NextTD=PhyspTD2;
	pTD1->Bend=PhysBend;

	pED0=Get_Free_ED();
	PhyspED0=((u32_t) pED0);
	printf("\nED0 Physical Address: %x",PhyspED0);
	printf("\nED0 Virtual Address: %x",pED0);
	//Initialize this first ED
	if(MpsKnown == 0)
	{
		pED0->EDdata=0x00400000||(ohci_status.DevAddress);			//0x00080000 changed to 0x00400000
	}
	else
	{
		pED0->EDdata=0x00000000|(ohci_status.DevMPS < 16) |(ohci_status.DevAddress);			//0x00080000 changed to 0x00400000
	}
	pED0->TailP=PhyspTD4; 
	pED0->HeadP=PhyspTD1;
	pED0->NextED=0x00000000; //PhyspED1.LowPart;

	// Write to Control Head ED register the Physical Address of the first ED
	writew(PhyspED0,HcCtrlHeadED(ohci_base)/*0x180a4020*/);
//	printf("\nControl Head Initialized to: %x",readw(0x180a4020));

	// Write to HcCommandStatus to tell control list filled
	HcCommandStatus=readw(HcCMDSTS(ohci_base)/*0x180a4008*/);
	HcCommandStatus=HcCommandStatus|0x00000002; 		//only control list filled bit is set in hccommandstatus register & bulk list filled
	printf("the value of HcCommandStatus is 		%x\n",HcCommandStatus);
	writew(HcCommandStatus,HcCMDSTS(ohci_base)/*0x180a4008*/);
	printf("the value of hccommand status register%x\n",readw(HcCMDSTS(ohci_base)/*0x180a4008*/));

	// enabling control list enable
	OHCI_ControlList_Enable(1);
	printf("after enable\n");
	printf("pED0->EDdata			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED			:%x\n",pED0->NextED);
	HcCommandStatus=readw(HcCMDSTS(ohci_base)/*0x180a4008*/);
	printf("HcHCCA							:%x\n",readw(HcHCCA(ohci_base)/*0x180a4018*/));
//	printf("the value of HcHcca.frame number is 	:%x\n",HcHcca->frame_no);
	printf("HcPeriodCurrentEd					:%x\n",readw(HcPeriodCurrED(ohci_base)/*0x180a401C*/));
//	printf("HcControlHeadEd 					:%x\n",readw(0x180a4020));
//	printf("HcControlCurrentEd					:%x\n",readw(0x180a4024));
	printf(("wait for transaction done interrupt\n"));
	while(!(readw(HcINTRSTS(ohci_base)/*0x180a400C*/) & 0x00000002))		// checking for the completion fo the qtd
	{
//		printf("the value of HcHcca.frame number is 		:%x\n",HcHcca->frame_no);
		printf("HcControl								:%x\n",readw(HcControl(ohci_base)/*0x180a4004*/));
		printf("HcCommandStatus 						:%x\n",readw(HcCMDSTS(ohci_base)/*0x180a4008*/));
		printf("HcInterruptStatus						:%x\n",readw(HcINTRSTS(ohci_base)/*0x180a400C*/));
		printf("HcHCCA								:%x\n",readw(HcHCCA(ohci_base)/*0x180a4018*/));
//		printf("HcControlHeadEd 						:%x\n",readw(0x180a4020));
//		printf("HcControlCurrentEd						:%x\n",readw(0x180a4024));
//		printf("HcFmRemaining							:%x\n",readw(0x180a4038));
//		printf("HcFmNumber							:%x\n",readw(0x180a403C));
//		printf("HcRhPortStatus							:%x\n",readw(0x180a4054));
		if(readw(HcINTRSTS(ohci_base)/*0x180a400C*/) & 0x00000024)
		{
			HcInterruptStatusreg=0x00000024;
			writew(HcInterruptStatusreg,HcINTRSTS(ohci_base)/*0x180a400C*/);
			printf("HcInterruptStatus					:%x\n",readw(HcINTRSTS(ohci_base)/*0x180a400C*/));
		}
		printf("\n");
	}
	printf("pED0->EDdata			:%x\n",pED0->EDdata);
	printf("pED0->TailP 			:%x\n",pED0->TailP);
	printf("pED0->HeadP 			:%x\n",pED0->HeadP);
	printf("pED0->NextED			:%x\n",pED0->NextED);
	HcInterruptStatusreg =readw(HcINTRSTS(ohci_base)/*0x180a400C*/);
	writew(HcInterruptStatusreg,HcINTRSTS(ohci_base)/*0x180a400C*/);
	HcDoneHeadreg = readw(HcDoneHead(ohci_base)/*0x180a4030*/);
	printf("the value of HcDoneHead char is 	:%x\n",HcDoneHeadreg);
//	printf("the value of HCCADoneHead is		:%x\n",HcHcca->done_head);
	if(HcHcca->done_head == PhyspTD3)	// checking the status of the completion
	{
		printf("the value of pTD3->TDdata is (Status)		:%x\n",pTD3->TDdata);
		printf("the value of pTD2->TDdata is (Data)	 	:%x\n",pTD2->TDdata);
		printf("the value of pTD1->TDdata is (Setup)		:%x\n",pTD1->TDdata);
		if(!(pTD3->TDdata & 0xF0000000))
		{
			printf("Get Descriptor Passed in Driver\n");
			TotalBytesTransfered = (pTD2->Cbp - IntialInStartBuffAddr);
			for(i=0;i<=TotalBytesTransfered;i++)
			{
				printf("the value of the dscr byte  is	:%x\n",(*(Buff+ i)));
			}
			if(DevDescType == 1)
			{
				ohci_status.DevMPS = *(Buff + 7);
				MpsKnown = 1;
				printf("the value of control endpoint mps is %x\n",ohci_status.DevMPS);
			}
			printf("the value of device control endpoint max pak size is %x\n",ohci_status.DevMPS);
			if(DevDescType == 2 && DevDescLength > 0x09)
			{
				printf("Full configuration descriptor\n");
				
			}
			
		}
		else 
		{
			printf("Get Descriptor failed \n");
		}
	}
	else 
	{
		printf("status td not completed yet\n");
	}
	Add_Free_ED_Ring(pED0);
	Add_Free_GTD_Ring(pTD1);		
	Add_Free_GTD_Ring(pTD2);
	Add_Free_GTD_Ring(pTD3);		
	Add_Free_GTD_Ring(pTD4);		
	return true;
}

int OHCI_Ctrl_TestItem(void)
{
	int		result;	
	char		testItem;
//	unsigned long HcInterruptStatus,HcRhPortStatus;

	printf("=============Ctrl TEST ================\n");
//	HcInterruptStatus=readw(0x180a400C);
//	HcRhPortStatus = readw(0x180a4054);
	printf("Wait for Device Connect\n");
	while(!portconnect)
	{
	}
	while(1)
	{
		if(portconnect)
		{
			printf("Device still connected\n");
		}
		else
		{
			printf("Device disconnected\n");
			break;
		}
//		printf("HcRhPortStatus                                        :%x\n",readw(0x180a4054));
		Display_OHCICtrlMenu();
		result = 0;
		printf("EHCI Ctrl Test>");
		testItem = getchar();
		switch(testItem)
		{
			case '0':
				OHCI_ResetDevice();
				break;
			case '1':	//SetAddress
				{
					int	Address;
					printf("Set Address >");
					scanf("%d",&Address);
					printf("Set Address %02x\n",Address);
					SetAddress_OHCI((BYTE)Address);
				}
				break;
			case '2': //SetConfig
				{
					int DevConfig;
					printf("Set Config (0~2)>");
					scanf("%d",&DevConfig);
					printf("Set Config %02x\n",DevConfig);
					SetConfiguration_OHCI(DevConfig);
				}
				break;
			case '3': //GetConfig
				{
					GetConfiguration_OHCI();
				}
				break;		
			case '4': //SetFeature
				{
					int 		DevRequest,DevFeature,DevDescIndex;
					int 		EpDir;
					BYTE	Item;
					EpDir = 0;
					printf("1:DEVICE_REMOTE_WAKEUP\n");
					printf("2:ENDPOINT HALT\n");
					Item = getchar();
					switch(Item)
					{
						case '1':
							DevRequest=0x00;
							DevFeature=0x01;
							DevDescIndex=0x00;
							break;
						case '2':
							DevRequest=0x02;
							DevFeature=0x00;
							printf("EndPoint No >");
							scanf("%d",&DevDescIndex);
							printf("EpDir >");
							scanf("%d",&EpDir);
							if(EpDir)
							{
								EpDir = 0x80;
							}
							else
							{
								EpDir = 0x00;
							}
							break;
						default:
							printf("2:ENDPOINT HALT");
							DevRequest=0x02;
							DevFeature=0x00;
							printf("EndPoint No >");
							scanf("%d",&DevDescIndex);							
							break;
					}
					SetFeature_OHCI(DevRequest,DevFeature,DevDescIndex,EpDir);
				}
				break;
			case '5': //ClearFeature
				{
					int 		DevRequest,DevFeature,DevDescIndex;
					int EpDir;
					BYTE Item;
					printf("1:DEVICE_REMOTE_WAKEUP\n");
					printf("2:ENDPOINT HALT\n");
					Item = getchar();
					printf("item value is %c\n",Item);
					switch(Item)
					{
						case '1':
							DevRequest=0x00;
							DevFeature=0x01;
							DevDescIndex=0x00;
							EpDir = 0;
							break;
						case '2':
							DevRequest=0x02;
							DevFeature=0x00;
							printf("EndPoint No >");
							scanf("%d",&DevDescIndex);
							printf("EpDir >");
							scanf("%d",&EpDir);
							if(EpDir)
							{
								EpDir = 0x80;
							}
							else
							{
								EpDir = 0x00;
							}
							break;
						default:
							printf("2:ENDPOINT HALT");
							DevRequest=0x02;
							DevFeature=0x00;
							printf("EndPoint No >");
							scanf("%d",&DevDescIndex);							
							break;
					}
					ClearFeature_OHCI(DevRequest,DevFeature,DevDescIndex,EpDir);
				}
				break;							
			case '6': //SetInterface
				{
					int 	DevIface,InterfaceNo;
					printf("Old Interface:%02x ,Set New Alternate setting>",ohci_status.DevIface);
					scanf("%d",&DevIface);					// alternate setting value
					printf("Set DevIface %02x\n",DevIface);
					printf("Enter the current interface number>");
					scanf("%d",&InterfaceNo);				// device interface value
					SetInterface_OHCI(DevIface,InterfaceNo);
				}
				break;			
			case '7':	//GetInterface
				{
					int	DevIface;
					printf("Interface Number >");
					scanf("%d",&DevIface);
					printf("Get Interface %02x\n",DevIface);
					GetInterface_OHCI();			//DevIface
				}
				break;
			case '8':	//GetDescriptor
				{
					int	Index,length;					
					char DevDescType;
//					char	Index;

					printf("0:CONFIGURATION\n");
					printf("1:DEVICE\n");
					printf("2:DEVICE_QUALIFIER\n");
					printf("3:ENDPOINT\n");
					printf("4:INTERFACE\n");
					printf("5:INTERFACE_POWER1\n");
					printf("6:OTHER_SPEED_CONFIGURATION\n");
					printf("7:STRING\n");					
					printf("Set Desc Type >");
					DevDescType = getchar();
					switch(DevDescType)
					{
						case '0':
							DevDescType=0x02;
							break;
						case '1':
							DevDescType=0x01;
							break;
						case '2':
							DevDescType=0x06;
							break;
						case '3':
							DevDescType=0x05;
							break;
						case '4':
							DevDescType=0x04;
							break;
						case '5':
							DevDescType=0x08;
							break;
						case '6':
							DevDescType=0x08;
							break;
						case '7':
							DevDescType=0x03;
							break;
						default:
							DevDescType=0x02;
							break;
					}
					printf("Set Index >");				
					scanf("%d",&Index);
					printf("Set Length >");								
					scanf("%d",&length);				
					printf("Set Type:%02x,Index:%02x,Length:%02x\n",DevDescType,Index,length);
					GetDescriptor_OHCI(DevDescType,Index,length);
				}
				break;
			case '9':	//getstatus
				{
					int Item,DevRequest,DevDescIndex;
					printf("1:DEVICE Status\n");
					printf("2:ENDPOINT Status\n");					
					printf("3:INTERFACE Status\n");
					printf("Type select: >");
					scanf("%d",&Item);
					if(Item == 1)
					{
						DevRequest=0x80;					
					}
					else if (Item == 2)
					{
						DevRequest=0x82;					
					}
					else if (Item == 3)
					{
						DevRequest=0x81;					
					}
					printf("Endpoint No Enter: >");
					scanf("%d",&DevDescIndex);
					GetStatus_OHCI(DevRequest,DevDescIndex);
				}
				break;
			case 'a':
				PrintOHCIRegs();
				break;
			case 'x' : //X. Exit 
			case 'X' :
				// Back to default
				printf("Bye Bye ..........\n");
				return result;							
			default:
				break;
		}
		printf("press Enter to continue ...");
		getchar();					
	}
	return 0;
}


int OHCI_Bulk_TestItem()
{
	int		result;	
	char		testItem;
	int 		Mps;
	int 		Length;
	int 		Loopcount;
	int 		EpNum;
	int 		EpOutNum;
	int 		EpInNum;
	int 		NoPac;
	int 		TdMaxbufLength;
	int 		TdBufLength;
	int 		i;
	int 		j;
	int 		k;
	int		l;
	PUCHAR  DataInBuf;
	PUCHAR  DataOutBuf;
	int Error;
	int MemNequal;

		
	printf("=============Bulk TEST ================\n");
	result = 0;
	while(1)
	{
		Display_OHCIBulkMenu();
		printf("OHCI Bulk Test>");
		testItem = getchar();
		switch(testItem)
		{
			case 'r':	//rx
				printf("Enter EpNum>");
				scanf("%d",&EpNum);
				printf("Enter Mps>");
				scanf("%d",&Mps);
				printf("Enter Length>");
				scanf("%d",&Length);
				OHCI_ReceiveBulkData(Mps,EpNum,Length);
				break;
			case 't':	//tx
				printf("Enter EpNum>");
				scanf("%d",&EpNum);
				printf("Enter Mps>");
				scanf("%d",&Mps);
				printf("Enter Length>");
				scanf("%d",&Length);
				OHCI_SendBulkData(Mps,EpNum,Length);
				break;
			case 'a':	//auto
				printf("Enter EpOutNum>");
				scanf("%d",&EpOutNum);
				printf("Enter EpInNum>");
				scanf("%d",&EpInNum);
				printf("Enter Mps>");
				scanf("%d",&Mps);
				printf("Enter Length>");
				scanf("%d",&Length);
				printf("Enter Loopcount>");
				scanf("%d",&Loopcount);
				for(l = 0;l<Loopcount;l++)
				{
					printf("the value of l is %d\n",l);
					DataInBuf = (PUCHAR)DATA_IN_ADDR_OHCI;
					DataOutBuf = (PUCHAR)DATA_OUT_ADDR_OHCI;
					for(k=0;k<Length;k++)
					{
						*(DataOutBuf+k) = k;
					}
	//				memset((UCHAR *)DataOutBuf,'A',8192);
	//				memset(((UCHAR *)DataOutBuf) + 8192,'B',8192);
	//				memset(((UCHAR *)DataOutBuf) + 16384,'C',8192);
	//				memset(((UCHAR *)DataOutBuf) + 24576,'D',8192);
					memset((UCHAR *)DataInBuf,0,Length);
					TdBufLength = Length;
					NoPac = Length/8192;
					TdMaxbufLength = 8192;
					if(!NoPac)
					{
						Error = OHCI_BulkLoopData(Mps,EpOutNum,EpInNum,Length,DataOutBuf,DataInBuf);
						if(!Error)
						{
							printf("Loopback failed\n");
							TdBufLength = 0;
						}
						else
						{
							printf("This loopback passed\n");
							for(j=0;j<Length;j++)
							{
								if(*(DataInBuf+j) == *(DataOutBuf+j))
								{
									MemNequal = 0;
								}
								else
								{
									MemNequal = 1;
									printf("the value of DatainBuf is %x\n",*(DataInBuf+j));
									printf("the value of DataoutBuf is %x\n",*(DataOutBuf+j));
									printf("the value of j is %x\n",j);
									break;
								}
							}
							if(MemNequal)
							{
								printf("Memory Mismatch\n");
								TdBufLength = 0;
								break;
							}
							else
							{
								printf("Memory matches correctly\n");
								TdBufLength = 0;
							}
						}
					}
					for(i=0;i<NoPac;i++)
					{
						//printf("the value of i is %d\n",i);
						Error = OHCI_BulkLoopData(Mps,EpOutNum,EpInNum,TdMaxbufLength,DataOutBuf,DataInBuf);
						if(!Error)
						{
							printf("Loopback failed\n");
							TdBufLength = 0;
							break;
						}
						else
						{
							printf("This loopback passed\n");
							for(j=0;j<8192;j++)
							{
								if(*(DataInBuf+j) == *(DataOutBuf+j))
								{
									MemNequal = 0;
								}
								else
								{
									printf("the value of DatainBuf is %x\n",*(DataInBuf+j));
									printf("the value of DataoutBuf is %x\n",*(DataOutBuf+j));
									printf("the value of j is %x\n",j);
									MemNequal = 1;
									break;
								}
							}
							if(MemNequal)
							{
								printf("Memory Mismatch\n");
								TdBufLength = 0;
	//							TdBufLength = TdBufLength - 8192;
								break;
							}
							else
							{
								printf("memory matches correctly\n");
								TdBufLength = TdBufLength - 8192;
								DataInBuf = (PUCHAR)((PUCHAR)DataInBuf + 8192);
								DataOutBuf = (PUCHAR)((PUCHAR)DataOutBuf + 8192);
							}
						}
					}
					if(TdBufLength != 0)
					{
						Error = OHCI_BulkLoopData(Mps,EpOutNum,EpInNum,TdBufLength,DataOutBuf,DataInBuf);
						if(!Error)
						{
							printf("Loopback failed\n");
							TdBufLength = 0;
						}
						else
						{
							printf("This loopback passed\n");
							for(j=0;j<TdBufLength;j++)
							{
								if(*(DataInBuf+j) == *(DataOutBuf+j))
								{
									MemNequal = 0;
								}
								else
								{
									MemNequal = 1;
									printf("the value of DatainBuf is %x\n",*(DataInBuf+j));
									printf("the value of DataoutBuf is %x\n",*(DataOutBuf+j));
									printf("the value of j is %x\n",j);
									break;
								}
							}
							if(MemNequal)
							{
								printf("Memory Mismatch\n");
								TdBufLength = 0;
							}
							else
							{
								printf("memory matches correctly\n");
								TdBufLength = 0;
							}
						}
					}
					if(!(Error) | MemNequal)
					{
						break;
					}
				}
				break;
			case 'x' : //X. Exit 
			case 'X' :
				// Back to default
				printf("Back to EHCI mainmenu ..........\n");
				return result;							
			default:
				break;
		}
		printf("press Enter to continue ...");
		getchar();					
	}
	
}

//api_new

int OHCI_Isochronous_TestItem()
{
	int		result;	
	char		testItem;
	int 		Mps;
	int  		count,i;
	int 		EpInNum;
	int 		EpOutNum;
	int 		Framecount;
	PUCHAR		OutDataBuf;
	PUCHAR		InDataBuf;
	int 		MemNequal;
	int j;
	Framecount = 0;
	int Error;
		
	printf("=============Iso TEST ================\n");
	while(1)
	{
		Display_OHCIIsoMenu();
		result = 0;
		printf("OHCI Iso Test>");
		testItem = getchar();
		switch(testItem)
		{
			case 'r':	//rx
				printf("Enter EpInNum>");
				scanf("%d",&EpInNum);
				printf("Enter Mps>");
				scanf("%d",&Mps);
				InDataBuf = (PUCHAR)DATA_IN_ADDR_OHCI;
				Error = OHCI_ReceiveIsoData(Mps,EpInNum,Framecount,InDataBuf);
				break;
			case 't':	//tx
				//SendIsoData();
				printf("Enter EpOutNum>");
				scanf("%d",&EpOutNum);
				printf("Enter Mps>");
				scanf("%d",&Mps);
				OutDataBuf = (PUCHAR)DATA_OUT_ADDR_OHCI;
				Error = OHCI_SendIsoData(Mps,EpOutNum,Framecount,OutDataBuf);
				break;
			case 'a':	//auto
				printf("Enter EpInNum>");
				scanf("%d",&EpInNum);
				printf("Enter Mps>");
				scanf("%d",&Mps);
				printf("Enter EpOutNum>");
				scanf("%d",&EpOutNum);
                            printf("Enter count>");
				scanf("%d",&count);
				// for higher iterations please include a for loop here
				InDataBuf = (PUCHAR)DATA_IN_ADDR_OHCI;
				OutDataBuf = (PUCHAR)DATA_OUT_ADDR_OHCI;
				for(i=0;i<count;i++)
				{
					Error = OHCI_SendIsoData(Mps,EpOutNum,Framecount,OutDataBuf);//api_new
					MSDELAY(2);
					Error = OHCI_ReceiveIsoData(Mps,EpInNum,Framecount,InDataBuf);//api_new
					// memory comparision
					for(j=0;j<Mps;j++)
					{
						if(*(InDataBuf+j) == *(OutDataBuf+j))
						{
							MemNequal = 0;
						}
						else
						{
							MemNequal = 1;
						       printf("the value of DatainBuf is %x\n",*(InDataBuf+j));
							printf("the value of DataoutBuf is %x\n",*(OutDataBuf+j));
							printf("the value of j is %x\n",j);
							break;
						}
					}
					if(MemNequal)
					{
						printf("Memory Mismatch\n");
					}
					else
					{
						printf("Memory Matches correctly\n");
					}
				}//end of FORcount
				break;
			case 'x' : //X. Exit 
			case 'X' :
				// Back to default
				printf("Back to EHCI mainmenu ..........\n");
				return result;							
			default:
				break;
		}
		printf("press Enter to continue ...");
		getchar();					
	}
	
}


int OHCI_Intr_TestItem()
{
	int		result;	
	char		testItem;
	int 		Mps;
	int 		EpInNum;

		
	printf("=============Intr TEST ================\n");
	while(1)
	{
		Display_OHCIIntrMenu();
		result = 0;
		printf("OHCI Intr Test>");
		testItem = getchar();
		switch(testItem)
		{
			case 'r':	//rx
				printf("Enter EpInNum>");
				scanf("%d",&EpInNum);
				printf("Enter Mps>");
				scanf("%d",&Mps);
				OHCI_ReceiveIntrData(Mps,EpInNum);
				break;
			case 't':	//tx
				//SendIntrData();
				break;
			case 'a':	//auto
				printf("wait device connect");
				break;
			case 'x' : //X. Exit 
			case 'X' :
				// Back to default
				printf("Back to EHCI mainmenu ..........\n");
				return result;							
			default:
				break;
		}
		printf("press Enter to continue ...");
		getchar();					
	}
	
}



int OHCI_HUB_TestItem()
{
	int		result;	
	char		testItem;

	printf("=============Hub TEST ================\n");
	while(1)
	{
		Display_OHCIHubMenu();
		result = 0;
		printf("Hub Request Test>");
		testItem = getchar();
		switch(testItem)
		{
		
			case '1':	//GetHubDescriptor
				{
					int DevDescIndex, DevDescLength,Dscrtype;
					printf("Set Dscrtype >");
					scanf("%d",&Dscrtype);
					printf("Set DevDescIndex >");
					scanf("%d",&DevDescIndex);
					printf("Set DevDescLength >");
					scanf("%d",&DevDescLength);
					printf("Set DevDescIndex:%02x, DevDescLength=%02x",DevDescIndex, DevDescLength);
					GetHubDescriptor_OHCI(Dscrtype,DevDescIndex,DevDescLength);
				}
				break;
			case '2':	//GetPortStatus
				{
					int Portnum;
					printf("Set Port No >");
					scanf("%d",&Portnum);	
					GetPortStatus_OHCI(Portnum);
				}
				break;
			case '3':	//GetHubStatus
					GetHubStatus_OHCI();
				break;	
			case '4':	//ClearPortFeature
				{
					int DevDescValue,DevDescIndex;
					BYTE GetCurSel;
					printf("0.PORT_CONNECTION\n");
					printf("1.PORT_ENABLE\n");
					printf("2.PORT_SUSPEND\n");
					printf("3.PORT_OVER_CURRENT\n");
					printf("4.PORT_RESET\n");
					printf("5.PORT_POWER\n");
					printf("6.PORT_LOW_SPEED\n");
					printf("7.C_PORT_CONNECTION\n");
					printf("8.C_PORT_ENABLE\n");
					printf("9.C_PORT_SUSPEND\n");
					printf("a.C_PORT_OVER_CURRENT\n");
					printf("b.C_PORT_RESET\n");
					printf("c.PORT_TEST\n");
					printf("d.PORT_INDICATOR\n");					
					printf("e.Set DevDescValue >\n");
					GetCurSel=getchar();	
					switch(GetCurSel)
					{
						case '0':
							DevDescValue = 0;
							break;
						case '1':						
							DevDescValue = 1;
							break;
						case '2':
							DevDescValue = 2;
							break;
						case '3':
							DevDescValue = 3;
							break;
						case '4':
							DevDescValue = 4;
							break;
						case '5':
							DevDescValue = 8;
							break;
						case '6':
							DevDescValue = 9;
							break;
						case '7':
							DevDescValue = 16;
							break;
						case '8':
							DevDescValue = 17;
							break;
						case '9':
							DevDescValue = 18;
							break;
						case 'a':
							DevDescValue = 19;
							break;
						case 'b':
							DevDescValue = 20;
							break;
						case 'c':
							DevDescValue = 21;
							break;
						case 'd':
							DevDescValue = 22;
							break;
					}
					//	State.DevDescType=0x29;  //HUB_DESCRIPTOR
					
					printf("Set Port No >");
					scanf("%d",&DevDescIndex);
					ClearPortFeature_OHCI(DevDescValue,DevDescIndex);
				}
				break;				
			case '5':	//ClearHubFeature
				{
					int DevDescIndex;
					printf("0.C_HUB_LOCAL_POWER");
					printf("1.C_HUB_OVER_CURRENT");
					scanf("%d",&DevDescIndex);
					//State.DevDescType=0x29;  //HUB_DESCRIPTOR
					//State.DevDescLength=0x0;
					ClearHubFeature_OHCI(DevDescIndex);
				}
				break;			
			case '6'://ResetTT
				{
					int DevDescIndex;
					printf("Enter Port No >");
					scanf("%d",&DevDescIndex);
				//	State.DevDescType=0x29;  //HUB_DESCRIPTOR
					//State.DevDescLength=0x0;
					//ClearPortFeature(RESET_TT,0,DevDescIndex);
				}				
				break;
			case '7'://StopTT
				{
					int DevDescIndex;
					printf("Enter Port No >");
					scanf("%d",&DevDescIndex);
				//	State.DevDescType=0x29;  //HUB_DESCRIPTOR
					//State.DevDescLength=0x0;
					//ClearPortFeature(STOP_TT,0,DevDescIndex);
				}
				break;
			case '8'://SetHubFeature
				{
					int DevDescValue;
					printf("0.C_HUB_LOCAL_POWER");
					printf("1.C_HUB_OVER_CURRENT");
					scanf("%d",&DevDescValue);
				//	State.DevDescType=0x29;  //HUB_DESCRIPTOR
					//State.DevDescLength=0x0;
					SetHubFeature_OHCI(DevDescValue);					
				}
				break;
			case '9'://SetPortFeature
				{
					int DevDescValue,DevDescIndex;
					BYTE GetCurSel;
					printf("0.PORT_CONNECTION");
					printf("1.PORT_ENABLE");
					printf("2.PORT_SUSPEND");
					printf("3.PORT_OVER_CURRENT");
					printf("4.PORT_RESET");
					printf("5.PORT_POWER");
					printf("6.PORT_LOW_SPEED");
					printf("7.C_PORT_CONNECTION");
					printf("8.C_PORT_ENABLE");
					printf("9.C_PORT_SUSPEND");
					printf("a.C_PORT_OVER_CURRENT");
					printf("b.C_PORT_RESET");
					printf("c.PORT_TEST");
					printf("d.PORT_INDICATOR");					
					printf("e.Set DevDescValue >");
					GetCurSel=getchar();	
					switch(GetCurSel)
					{
						case '0':
							DevDescValue = 0;
							break;
						case '1':						
							DevDescValue = 1;
							break;
						case '2':
							DevDescValue = 2;
							break;
						case '3':
							DevDescValue = 3;
							break;
						case '4':
							DevDescValue = 4;
							break;
						case '5':
							DevDescValue = 8;
							break;
						case '6':
							DevDescValue = 9;
							break;
						case '7':
							DevDescValue = 16;
							break;
						case '8':
							DevDescValue = 17;
							break;
						case '9':
							DevDescValue = 18;
							break;
						case 'a':
							DevDescValue = 19;
							break;
						case 'b':
							DevDescValue = 20;
							break;
						case 'c':
							DevDescValue = 21;
							break;
						case 'd':
							DevDescValue = 22;
							break;
					}
					//	State.DevDescType=0x29;  //HUB_DESCRIPTOR
					
					printf("Set Port No >");
					scanf("%d",&DevDescIndex);
					SetPortFeature_OHCI(DevDescValue,DevDescIndex);
				}
				break;
			case 'a'://ClearTTBuffer
				printf("no implement");
				break;
			case 'b'://SetHubDescriptor
				printf("no implement");
				break;
			case 'c'://GetTTState
				printf("no implement");
				break;
			case 'x' : //X. Exit 
			case 'X' :
				// Back to default
				printf("Bye Bye ..........\n");
				return result;							
			default:
				break;
		}
		printf("press Enter to continue ...");
		getchar();					
	}
	
}


extern int ohci_test(int autotest)
{
	int	result = 0;
	char	testItem;
		
	portconnect = 0;
	TransferComplete = 0;
	ResetChange = 0;
	MpsKnown = 0;
	BulkInTog = 0;
	BulkOutTog = 0;
	IntrInTog = 0;
	ohci_status.DevAddress = 0;
	printf("inside ohci testing\n");
//	PrintOHCIRegs();
	OHCI_Start();

//	PrintOHCIRegs();

	// initializing the endpoint descriptor.
	ED_Init();

	// initializing the transfer descriptors.
	GTD_Init();
	IsoTd_init();
	while (1) {

		result = 0;

			Display_OHCIMenu();
			printf("OHCI Test>");
			testItem = getchar();
		switch (testItem) 
		{
			case 'a':
				//result = OHCI_Auto_TestItem();
				break;		
			case '1':
				result = OHCI_Base_TestItem();
				break;
			case '2':
				result = OHCI_HUB_TestItem();
				break;
			case '3':
				result = OHCI_Ctrl_TestItem();				
				break;
			case '4':
				result = OHCI_Bulk_TestItem();
				break;
			case '5':
				result = OHCI_Intr_TestItem();
				break;
			case '6':
				result = OHCI_Isochronous_TestItem();
				break;
			case '7':
				PrintOHCIRegs();
				break;
				
			case 'x' : //X. Exit Memory Test Back to Main Menu
			case 'X' :
				// Back to default
				printf("Back to Mainmenu ..........\n");
				return result;
			default :
				Display_OHCIMenu();
				break;
		}

			printf(".......OHCI Item (%c)\n", testItem);
			printf("press Enter to continue ...");
			getchar();
	}
	
	return 0;

}

int OHCI_Auto_TestItem(void)
{
	int		result;	
	char		testItem;

	OHCI_Reset();
	
	printf("=============Auto Test ================\n");
	
	while(1)
	{
		Display_OHCIAutoTestMenu();
		result = 0;
		printf("EHCI Auto Test>");
		testItem = getchar();
		switch(testItem)
		{
			case '1':		//Bulk IN/Out
				break;
			case '2':		//Intr IN
				break;
			case '3':	
				break;
			case '4':
				break;
			case '5':

				break;
			case '6':
				
				break;
				
			case 'x' : //X. Exit 
			case 'X' :
				// Back to default
				printf("Bye Bye ..........\n");
				return result;							
			default:
				break;
		}
		printf("press Enter to continue ...");
		getchar();					
	}
	
}
	
int OHCI_Base_TestItem(void)
{
	int		result;	
	char		testItem;

		
	printf("=============BASE TEST ================\n");
	while(1)
	{
		Display_OHCIBASETestMenu();
		result = 0;
		printf("OHCI Base Test>");
		testItem = getchar();
		switch(testItem)
		{
			case '1':
				printf("\n=====Default Values Check=========================");	
				// TD1.4
				OHCI_Reset();
				Validate_OHCIRegs();
			break;
			case '2':
				printf("\n=====Test Invalid Port Enable =========================");					
				//TD2.4
//				OHCI_Reset();
//				writew(readw(PORTSC)| PORT_PE, PORTSC);
//				if(readw(PORTSC) & PORT_PE)
//					printf("Fail\n");
			break;
			case '3':
				printf("\n=====Test Port Disable =========================");					
				//TD2.5 device connect
//				writew(readw(PORTSC)& ~PORT_PE, PORTSC);
//				if(readw(PORTSC) & PORT_PE)
//					printf("Fail\n");
				break;
			case '4':	//Read Frameindex register at the end of every microframe
			{
				BYTE	i,index,FrameTimerInterval = 125;   //125microSec

				//FRINDEX  register  is incremented
				printf("\n==========================================================");	
				for (i=0;i<25;i++)
				{
					//index = readw(FRINDEX);
					printf("FRINDEX                 :%d\n",index);
					MSDELAY(FrameTimerInterval);
				}
				printf("==========================================================\n");
			}	
				break;
			case '5':
				break;
			case '6':	//OHCI_Reset
				OHCI_Reset();
				break;
			case '7':
				OHCI_Start();
				break;
			case '8':
				PrintOHCIRegs();
				break;
			case 'x' : //X. Exit 
			case 'X' :
				// Back to default
				printf("Bye Bye ..........\n");
				return result;							
			default:
				break;
		}
		printf("press Enter to continue ...");
		getchar();					
	}
	
}

void OHCIIntrHandler(void* pparam)
{
	ULONG HcInterruptDisable,HcRhPortStatus1;
	ULONG HcInterruptEnable;
	ULONG HcInterruptStatus;
//	printf("\nInside InterruptHandler\n");
	HcInterruptStatus=readw(HcINTRSTS(ohci_base));		// reading interrupt status register
	HcInterruptEnable=readw(HcINTREN(ohci_base));		//reading interrupt enable register
	// Did our device generated the interrupt
	if (!(HcInterruptStatus&HcInterruptEnable))
	{
		printf("\nERROR - Not a OHC interrupt");
	}
	else
	{
		//Our device needs attention
		// Now disable all the interrupts
		HcInterruptDisable=readw(HcINTRDISEN(ohci_base));
		HcInterruptDisable=HcInterruptDisable|0xc000007f;
		writew(HcInterruptDisable,HcINTRDISEN(ohci_base)); //disable all interrupts
//		printf("all interrupts are disabled\n");
		//Is this an SOF Interrupt
		if (HcInterruptStatus&0x00000004)
		{
//			printf(("\nStart of Frame Interrupt\n"));
			//clear start of frame in interrupt status register
			writew(0x00000004,HcINTRSTS(ohci_base));
//			printf("SOF interrupt cleared\n");
		}
		// Is this root hub statuchange interrupt
		if ((HcInterruptStatus&0x00000040))
		{
			printf(("\nRoot Hub Status change Interrupt\n"));
			HcRhPortStatus1=readw(HcRhPortStatus(ohci_base));
			// Is this connect status change on any port
			if (HcRhPortStatus1&0x00010000)  
			{
				printf(("\nConnect status change interrupt \n"));

				//Clear connect status change bit here in rhportstatus register
				writew(0x00010000,HcRhPortStatus(ohci_base));	
				printf("\nconnect status change interrupt cleared in RhPortStatus register\n");
				if (HcRhPortStatus1&0x00000001)
				{
					printf(("\nDevice now connected to port1\n"));
					portconnect = 1;
					// Apply Reset to this port
					//HcRhPortStatus1=0x00000010;
					//WriteRegister(dx,21,HcRhPortStatus1);
				}	
				else 
				{
					portconnect = 0;
					printf(("\nDevice disconnected from port1\n"));
				}
			}
			// Is this Reset Status Change on any port
			if (HcRhPortStatus1&0x00100000)  
			{
				printf(("\nReset status change interrupt on port 1\n"));
				//Clear the reset status change bit in Rhportstatus register
				writew(0x00100000,HcRhPortStatus(ohci_base));
				printf("reset status change interrupt cleared in RhPortStatus register\n");
				ResetChange = 1;
				if (HcRhPortStatus1&0x00000002)
				{
					printf(("\nPort 1 Reset Complete, Port Enabled\n"));
					// Give Control List Enable here
					//HcControl=ReadRegister(dx,1);
					//HcControl=HcControl|0x00000010;
					//WriteRegister(dx,1,HcControl);
					//dx->ResetComplete=1;
				}
				else 
				{
					printf(("\nPort 1 Reset Not Complete, Port Not Enabled"));
				}
			}
			// port enable status change interrupt.	
			if (HcRhPortStatus1&0x00020000)
			{
				printf(("\nPort Enable Status Change Interrupt on port 1\n"));
				//Clear Port Enable status change bit in Rhportstatusregister
				#if 0
				writew(0x00020000,HcRhPortStatus);
				printf("port enable status change interrupt cleared in RhPortStatus register\n");
				//Check if Port Disabled, if yes make control list enable 0
				if (!(HcRhPortStatus1&0x00000002))
				{
					printf("port is disabled and hence disabling control list\n");
					// Give Control List Disable here
					//HcControl=ReadRegister(0x180a4004);
					//HcControl=HcControl&0xffffffef;
					//writew(HcControl,0x180a4004);
				}
			#endif
			}
			
			//Clear Root Hub Status change in interrupt status register
			writew(0x00000040,HcINTRSTS(ohci_base));
			printf("RHSC interrupt is cleared\n");
		}
		// Write Back Done Head Interrupt
		if (HcInterruptStatus&0x00000002)
		{
//			printf(("\nWrite Back Done Head Interrupt Occurred\n"));
			TransferComplete = 1;
			writew(0x00000002,HcINTRSTS(ohci_base));
//			printf("WDH interrupt is cleared\n");
		}
	}
#if 0
	printf("\nHcControl %x",readw(HcControl));
	printf("\nHcCommandStatus %x",readw(HcCMDSTS));
	// Confirm Interrupt Clearance here again
	//HcInterruptStatus=ReadRegister(dx,3);
	//WriteRegister(dx,3,HcInterruptStatus);
	printf("\nHcInterruptStatus %x",readw(HcINTRSTS));
	printf("\nHcInterruptEnable %x",readw(HcINTREN));
	printf("\nHcHcca %x",readw(HcHCCA));
	printf("\nHcControlHeadED %x",readw(HcCtrlHeadED));
//	printf("\nHcControlCurrentED %x",readw(0x180a4024));
//	printf("\nHcFmNumber %x",readw(0x180a403C));
//	printf("\nHcRhPortStatus1 %x",readw(0x180a4054));
	printf(("\nInterrupt Handler Completed\n"));
#endif
	//Clear Interrupt Disabling bits here
	HcInterruptDisable=HcInterruptDisable&0x80000000;			// check this value since mie is disabled
	writew(HcInterruptDisable,HcINTRDISEN(ohci_base));
	//Reenable all the interrupts here
	HcInterruptEnable=HcInterruptEnable|0x80000042;
	writew(HcInterruptEnable,HcINTREN(ohci_base));

}

