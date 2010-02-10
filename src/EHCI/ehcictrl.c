#include "global.h"
#include "genlib.h"
#include "platform.h"
#include "irqs.h"
#include "ehcictrl.h"
#include "ehcimem.h"
#include "ehcireg.h"
#include "OHCI/ohcireg.h"
#include "usb.h"
#include "Interrupt/interrupt.h"
#ifdef CONFIG_PC9002
#include "MP_GPIO/regs-mp-gpio.h"
#include "MP_GPIO/mp-gpio.h"
#endif
#if defined (CONFIG_PC7210) || defined (CONFIG_PDK)
#include <scu.h>
#endif
#include <test_item.h>

//#define SOCLE_EHCI_DEBUG

#ifdef SOCLE_EHCI_DEBUG
#define socle_ehci_show(fmt, args ...)   printf(fmt, __FUNCTION__ , ## args)
#else
#define socle_ehci_show(fmt, args ...)	{}
#endif

u32 ehci_base;
int ehci_irq;


static autorun;
extern int ehci_test(int autotest);

extern struct test_item_container ehci_main_container;

extern int 
EHCITesting(int autotest)
{
	int ret=0;

        #ifdef CONFIG_PC9002
                socle_mp_gpio_set_port_num_value(PA,6,0);
        #endif

                //20080123 leonid add for USB Downstream
        #if defined(CONFIG_PC7210) || defined(CONFIG_PDK)
                socle_scu_usb_tranceiver_downstream();
        #endif

	ret = test_item_ctrl(&ehci_main_container,autotest);

        return ret;
}

extern int
ehci_0_test(int autotest)
{
        int ret=0;

	ehci_base = SOCLE_AHB0_UHC0;
        ehci_irq = SOCLE_INTC_UHC0;
        ret = ehci_test(autotest);

        return ret;
}

extern int
ehci_1_test(int autotest)
{
        int ret=0;

	ehci_base = SOCLE_AHB0_UHC1;
        ehci_irq = SOCLE_INTC_UHC1;
	ret = ehci_test(autotest);

        return ret;
}


EHCSTAT		ehci_status;	// structure used to store the information of device connected.

void Display_TestModeMenu(void)
{
	printf("USB Test Mode Testing.\n");
	printf("==================================================\n");
	printf("1.Host High-Speed Signal Quality Test\n");
	printf("2.Host Test J/K/SE0-STATE\n");
	printf("3.Host Controller Packet Parameters Test\n");
	printf("4.Host Chirp Timing Test\n");
	printf("5.Host Suspend Resume Timing Test\n");
	printf("\nX. Exit USB Test Mode Back to Main Menu\n");
	printf("==================================================\n");
}
void Display_TestJKModeMenu(void)
{
	printf("USB Test J/K/SE0 Mode Testing.\n");
	printf("==================================================\n");
	printf("E.Erumate Device\n");
	printf("J.Host Test J Test\n");
	printf("K.Host Test K Test\n");
	printf("S.Host Test SE0_NAK Test\n");
	printf("\nX. Exit USB Test Mode Back to Main Menu\n");
	printf("==================================================\n");
}

int EHCI_Halt(void)
{//check
	u32_t		UsbCmd;
	u32_t		UsbSts; 

	UsbSts = readw(USBSTS(ehci_base));
	if ((UsbSts & STS_HALT) != 0)
		return 0;

	UsbCmd=readw(USBCMD(ehci_base));
	UsbCmd=UsbCmd & ~CMD_RUN;
	writew(UsbCmd,USBCMD(ehci_base));
	socle_ehci_show(("EHC Halted Successfully"));	
	return 0;

}

int EHCI_Reset(void)
{//check
	u32_t	command;
	socle_ehci_show("Inside EHCI_Resetn");
	command = readw (USBCMD(ehci_base));
	command |= CMD_RESET;
	writew(command, USBCMD(ehci_base));
	command=readw(USBCMD(ehci_base));

	while(command &CMD_RESET)
		command=readw(USBCMD(ehci_base));
	return 0;
}
/*spec 4.14 */
int EHCI_Port_TestMode()
{
	u32_t		UsbCmd,Portsc;

	//Step1
	UsbCmd = readw(USBCMD(ehci_base));
	UsbCmd =UsbCmd & ~(CMD_ASE | CMD_PSE);
	writew(UsbCmd,USBCMD(ehci_base));

	//Step2
	Portsc = readw(PORTSC(ehci_base));
	writew((Portsc | PORT_SUSPEND),PORTSC(ehci_base));

	//Step3
	writew(readw(USBCMD(ehci_base)) & (~CMD_RUN),USBCMD(ehci_base));
	while(readw(USBSTS(ehci_base)) & STS_HALT)

	//Step4
	Portsc = readw(PORTSC(ehci_base));
	Portsc = Portsc | ( 0x05<<20);
	writew(Portsc,PORTSC(ehci_base));

	printf("enter any key to continous\n");
	getchar();
	//End test mode
	UsbCmd = readw(USBCMD(ehci_base));
	UsbCmd =UsbCmd | CMD_RESET;
	writew(UsbCmd,USBCMD(ehci_base));

	return 0;
	
}
/*
	0:Disable 1:Enable
*/
void EHCI_Asynchronous_Enable(BYTE Enable)
{//check
	u32_t	UsbCmd,UsbSts, cnt;

	//Disable/Enable Asynchronous list processing
	UsbCmd=readw(USBCMD(ehci_base));

	if(Enable == 1)
	{
		UsbCmd=UsbCmd |CMD_ASE;
		writew(UsbCmd,USBCMD(ehci_base));
	}
	else
	{
		UsbCmd=UsbCmd & (~CMD_ASE);
		writew(UsbCmd,USBCMD(ehci_base));
	}
	if(Enable == 0)
	{
		UsbSts=readw(USBSTS(ehci_base));
		cnt = 0;
		while(UsbSts & STS_ASS )
		{
			UsbSts=readw(USBSTS(ehci_base));
			socle_ehci_show("the value of usbsts reg is %x\n",UsbSts);
			if(cnt > 100)
				printf("the value of usbsts reg is %x & count 100 times\n",UsbSts);
			cnt ++;
		}
	}
	socle_ehci_show("the value in the usbcmd register is %x\n",readw(USBCMD(ehci_base)));
}

void EHCI_Periodic_Enable(BYTE Enable)
{//check
	u32_t	UsbCmd,UsbSts;
	if(Enable == 1)
	{
		UsbCmd = readw(USBCMD(ehci_base)) | CMD_PSE;
		writew (UsbCmd,USBCMD(ehci_base));
	}
	else
	{
		UsbCmd = readw(USBCMD(ehci_base)) & (~CMD_PSE);
		writew (UsbCmd,USBCMD(ehci_base));
	}
	if(Enable == 0)
	{
		UsbSts=readw(USBSTS(ehci_base));
		printf("The value of usbsts register is %x\n",UsbSts);
		while((UsbSts & STS_PSS ))
		{
			printf("FRINDEX(ehci_base) 				 :%x\n",readw(FRINDEX(ehci_base)));
			printf("The value of usbsts register is %x\n",UsbSts);
			UsbSts=readw(USBSTS(ehci_base));	
		}
		printf("the value in the usbcmd register is %x\n",readw(USBCMD(ehci_base)));
	}
	else
	{
		UsbSts=readw(USBSTS(ehci_base));
		printf("The value of usbsts register is %x\n",UsbSts);
		while(!(UsbSts & STS_PSS ))
		{
			printf("FRINDEX(ehci_base) 				 :%x\n",readw(FRINDEX(ehci_base)));
			printf("The value of usbsts register is %x\n",UsbSts);
			UsbSts=readw(USBSTS(ehci_base));	
		}
		printf("the value in the usbcmd register is %x\n",readw(USBCMD(ehci_base)));
	}
}

/////////////////////////////////////////////////////////////////////////////
void FrameIndexIncreTest(void)
{
	BYTE	i,index,FrameTimerInterval = 125;   //125microSec

	//FRINDEX(ehci_base)  register  is incremented
	printf("\n==========================================================");	
	for (i=0;i<25;i++)
	{
		index = readw(FRINDEX(ehci_base));
		printf("FRINDEX(ehci_base)                 :%d\n",index);
		MSDELAY(FrameTimerInterval);
	}
	printf("==========================================================\n");

}
void EHCI_Start()
{
	unsigned int i;
	PULONG 	FramelistPointer;
	socle_ehci_show("EHCI_Start\n");	
	FramelistPointer = (PULONG)PERIOD_FRAMELIST_ADDR;
	for(i=0;i<1024;i++)
	{
		*(FramelistPointer+i) = T_BIT;
	}
	
	EHCI_Intr_Disable();
	// Enable interrupts
	request_irq( ehci_irq, EHCIIntrHandler, NULL);
	
	//Check whether the HC is in halted state else halt it stop HC
	EHCI_Halt();

	//Apply HC Reset
	EHCI_Reset();			// commented by vinchip for testing

#if 0
	EHCI_Intr_Threshold_Ctrl(0x08);
#endif
	//Program the CTRLDSSEGMENT(ehci_base) Register
	writew(0x00000000,CTRLDSSEGMENT(ehci_base));
	//Program the PERIODIC LIST BASE Register
	writew(virt_to_phy(PERIOD_FRAMELIST_ADDR),PERIODICLISTBASE(ehci_base));
	
	//Program the ASYNCICLISTADDR(ehci_base) Register
	writew(0x00000000,ASYNCICLISTADDR(ehci_base));
	// Enable device
	EHCI_Intr_Enable();

	writew(FLAG_CF,CONFIGFLAG(ehci_base));
	writew(readw(USBCMD(ehci_base))|CMD_RUN,USBCMD(ehci_base));

}

void EHCI_Stop()
{
	socle_ehci_show("EHCI_Stop\n");	
	// Disable device
	EHCI_Intr_Disable();
	
	EHCI_Halt();
	
	// Disconnect from interrupt
	INT0_DISABLE(ehci_irq);

	INT0_CLEAR(ehci_irq);	
	INT0_CLR_MASK(ehci_irq);	

}

//=============================================================
//For Test Tools
//=============================================================
void EHCIIntrHandler(void* pparam)
{
	ULONG UsbSts;
	ULONG UsbIntr;
	ULONG PortSc1;
	
	socle_ehci_show("\nEHCIIintHandler\n");
	
	UsbSts=readw(USBSTS(ehci_base));
	UsbIntr=readw(USBINTR(ehci_base));
	PortSc1=readw(PORTSC(ehci_base));

	if(UsbSts & UsbIntr)
	{
		//Disable all interrupts
		writew(0x00, USBINTR(ehci_base));
		if(UsbSts & STS_INT)
		{
			socle_ehci_show(("USB Transaction Complete Interrupt\n"));
			//check asy_addr & periodic_addr
//			TransactionComplete();
			TransferComplete = 1;
			writew(STS_INT,USBSTS(ehci_base));
			socle_ehci_show(("cleared status register\n"));
			socle_ehci_show("the value of status register is %x\n",readw(USBSTS(ehci_base)));
		}
		if(UsbSts & STS_ERR)
		{
			socle_ehci_show(("USB Error Interrupt\n"));
			ErrorComplete = 1;			
			writew(STS_ERR,USBSTS(ehci_base));
		}
		if(UsbSts & STS_PCD)
		{
			socle_ehci_show(("USB Port  Change Detect Interrupt\n"));
			PortSc1=readw(PORTSC(ehci_base));

			if(PortSc1 & PORT_CSC)	//Connect status change
			{
				if(PortSc1 & PORT_CONNECT)
				{
					socle_ehci_show(("Device Connect on Port1\n"));
					PortSc1=PortSc1|PORT_CSC;
					writew(PortSc1,PORTSC(ehci_base));
					portconnect=1;
				}
				else
				{
					socle_ehci_show(("Device Disconnected From Port1\n"));
					PortSc1=PortSc1|PORT_CSC;
					writew(PortSc1,PORTSC(ehci_base));
					portconnect=0;
				}
			}
			else if(PortSc1 &  PORT_PEC) //port enable change
			{
				if(PortSc1 & 0x00000004)
				{
					socle_ehci_show(("Device Enabled on Port1\n"));
					PortSc1=PortSc1|PORT_PEC;
					writew(PortSc1,PORTSC(ehci_base));
				}
				else
				{
					socle_ehci_show(("Device Disabled From Port1\n"));
					PortSc1=PortSc1|PORT_PEC;
					writew(PortSc1,PORTSC(ehci_base));
				}
			}
			
			writew(STS_PCD,USBSTS(ehci_base));
		}
		if(UsbSts & STS_FLR)
		{
			socle_ehci_show(("Frame List Rollover Interrupt\n"));
			writew(STS_FLR,USBSTS(ehci_base));
		}
		if(UsbSts & STS_FATAL)
		{
			socle_ehci_show(("Host System Error Interrupt\n"));
			writew(STS_FATAL,USBSTS(ehci_base));
			///4.10.3.1
			
		}
		if(UsbSts & STS_IAA)
		{
			socle_ehci_show(("Interrupt on Async Advance\n"));
			writew(STS_IAA,USBSTS(ehci_base));
		}
		//Reenable PCD,USBERR,USBINT interrupts here
		writew(0x07,USBINTR(ehci_base));
		socle_ehci_show(("Renabled interrupts here\n"));
		//return TRUE;
	}
	else
	{
		socle_ehci_show("Other Device Interrupt\n");
	}
}

void TransactionComplete()
{
	PQHEAD 		QHead;
	PQTD		QueueTD;

	//check periodic
	
	//check sync
	QHead= readw(ASYNCICLISTADDR(ehci_base));

	while(QHead)
	{
		//release NextQTD
		QueueTD = QHead->NextQTD;
		while(QueueTD)
		{
			Add_Free_QTD_Ring(QueueTD);
			QueueTD = QHead->NextQTD;
		}
		//release AltQTD
		QueueTD = QHead->AltQTd;
		while(QueueTD)
		{
			Add_Free_QTD_Ring(QueueTD);
			QueueTD = QHead->AltQTd;
		}
		
		QHead = QHead->QHEADLinkPtr;
		Add_Free_QH_Ring(QHead);
	}
	
}

void PrintRegs(void)
{
	printf("####### EHCI Capability Registers ########\n");
	printf("==========================================\n");
	printf("HCIVERSION / CAPLENGTH(ehci_base)     		:%x\n",readw(CAPLENGTH(ehci_base)));
	printf("HCSPARAMS(ehci_base)					 	:%x\n",readw(HCSPARAMS(ehci_base)));
	printf("HCCPARAMS(ehci_base)					 	:%x\n",readw(HCCPARAMS(ehci_base)));
	printf("HCSP-PORTROUTE0			 		:%x\n",readw(HCSP_PORTROUTE(ehci_base)));
	//printf("HCIVERSION / CAPLENGTH(ehci_base)                   :%08x\n",readw(CAPLENGTH(ehci_base)));
	//printf("HCSPARAMS(ehci_base)                                         :%08x\n",readw(HCSPARAMS(ehci_base)));
	//printf("HCCPARAMS(ehci_base)                                        :%08x\n",readw(HCCPARAMS(ehci_base)));
	//printf("HCSP-PORTROUTE0                              :%08x\n",readw(HCSP_PORTROUTE(ehci_base)));
		
	printf("####### EHCI Operational Registers ########\n");
	printf("==========================================\n");
	
	printf("USBCMD(ehci_base)                  				:%08x\n",readw(USBCMD(ehci_base)));
	printf("USBSTS(ehci_base)                  					:%08x\n",readw(USBSTS(ehci_base)));
	printf("USBINTR(ehci_base)                 					:%08x\n",readw(USBINTR(ehci_base)));
	printf("FRINDEX(ehci_base)                 					:%08x\n",readw(FRINDEX(ehci_base)));
	printf("CTRLDSSEGMENT(ehci_base)			  		:%08x\n",readw(CTRLDSSEGMENT(ehci_base)));
	printf("PERIODICLISTBASE(ehci_base)		  			:%08x\n",readw(PERIODICLISTBASE(ehci_base)));
	printf("ASYNCICLISTADDR(ehci_base)			  		:%08x\n",readw(ASYNCICLISTADDR(ehci_base)));
	printf("CONFIGFLAG(ehci_base)			  			:%08x\n",readw(CONFIGFLAG(ehci_base)));
	printf("PORTSC(ehci_base)                 					:%08x\n",readw(PORTSC(ehci_base)));
	printf("-------------------------------------------------------------\n");

}

void ValidateRegs(void)
{
	int error=0;
	printf(("Validating Registers"));  

	if(readw(USBCMD(ehci_base)) != 0x80000)
	{
		printf(("ERROR : Check the Default value of USBCMD(ehci_base) register after HC Reset\n"));
		error = 1;
	}
	if(readw(USBSTS(ehci_base)) != 0x1000)
	{
		printf(("ERROR : Check the Default value of USBSTS(ehci_base) register after HC Reset\n"));
		error = 1;
	}
	if(readw(USBINTR(ehci_base)) != 0x0)
	{
		printf(("ERROR : Check the Default value of USBINTR(ehci_base) register after HC Reset\n"));
		error = 1;
	}
	if(readw(FRINDEX(ehci_base)) != 0x0)
	{
		printf(("ERROR : Check the Default value of FRINDEX(ehci_base) register after HC Reset\n"));
		error = 1;
	}
		
	if(readw(CTRLDSSEGMENT(ehci_base)) != 0x0)
	{
		printf(("ERROR : Check the Default value of CTRLDSSEGMENT(ehci_base) register after HC Reset\n"));
		error = 1;
	}
	if(readw(CONFIGFLAG(ehci_base)) != 0x0)
	{
		printf(("ERROR : Check the Default value of CONFIGFLAG(ehci_base) register after HC Reset\n"));
		error = 1;
	}
//	if(readw(PORTSC(ehci_base)) != 0x2000)
	if(readw(PORTSC(ehci_base)) != 0x3000) // changed by vinchip since port power is hardwired to 1.
	{
		printf(("ERROR : Check the Default value of PORTSC(ehci_base)1 register after HC Reset\n"));
		error = 1;
	}
	if(error == 0)
		printf(("SUCCESS : Validating Registers\n"));  

}

void PortResume()
{

	//PDEVICE_OBJECT DeviceObject=dx->fdo;
	ULONG PortSc1;
	ULONG Status=0;
	ULONG BytesTxd=0;
	
	//Set Force Port Resume bit to 0

	PortSc1=readw(PORTSC(ehci_base));
	PortSc1=PortSc1&0xffffffbf;
	writew(PortSc1,PORTSC(ehci_base));
	
	//Wait until Port Resume bit is set to 0
	PortSc1=readw(PORTSC(ehci_base));
	while(PortSc1 & 0x40)
		
		if(!(PortSc1 & 0x80))
		{
			printf(("Port 1 Resume after Suspend\n"));
			Status=1;
		}
		else
		{
			printf(("Port 1 Cannot be Resumed after Suspend\n"));
			Status=0;
		}
		
		BytesTxd=4;
		
}

void PortReset()
{

	ULONG UsbSts,PortSc1;

	UsbSts=readw(USBSTS(ehci_base));
	PortSc1=readw(PORTSC(ehci_base));
	
	//Clear Reset bit
	PortSc1=PortSc1&0xfffffeff;
	writew(PortSc1,PORTSC(ehci_base));
	
	//KeSetTimer(&dx->DummyTimer,dx->DummyInterval,&dx->DummyTimerDpc);
	
	//	while(PortSc1 & 0x100)
	//PortSc1=readw(PORTSC(ehci_base));
	/*
	//Commented after implementing 2ms Timer Dpc
	PortSc1=readw(PORTSC(ehci_base));
	printf(("USBSTS(ehci_base) After Clearing Reset bit  :%x",UsbSts);
	printf(("PORTSC(ehci_base)1 After Clearing Reset bit :%x",PortSc1);
	if(PortSc1 & 0x4)
	{
	printf(("Device on Port 1 Enabled in High Speed");

	}
	else
	{
	printf(("Device on Port 1 Enabled in Full Speed");

	}
	*/

}

/////////////////////////////////////////////////////////////////////////////
int EHCI_Intr_Threshold_Ctrl(BYTE Value)
{//check
	u32_t	UsbCmd;
	printf(("\n==================================================================="));
	UsbCmd=readw(USBCMD(ehci_base)); 
	UsbCmd=UsbCmd | (Value <<16);   

	//0x40:8ms 	64
	//0x20:4ms 	32	
	//0x10;2ms 	16
	//0x8: 8 micro frame  	//default
	//0x4: 4 micro frame  
	//0x2; 2 micro frame 
	//0x1; 1 micro frame
	writew(UsbCmd,USBCMD(ehci_base));

	printf(("===================================================================\n"));
	return 0;
}

int EHCI_Intr_Enable(void)
{
	ULONG UsbIntr;
	socle_ehci_show("inside EHCI_Intr_Enable\n");
	
	//Enable Portchangedetect,UsbErrint,UsbInt interrupts are enabled here
	UsbIntr=readw(USBINTR(ehci_base)) | 0x00000007;
	writew(UsbIntr, USBINTR(ehci_base));
	return 0;
}

int EHCI_Intr_Disable(void)
{
	ULONG UsbIntr;
	socle_ehci_show("inside EHCI_Intr_Disable\n");
	
	//Disable all interrupts here
	UsbIntr=readw(USBINTR(ehci_base)) & 0x00000000;
	writew(UsbIntr, USBINTR(ehci_base));
	return 0;
}

void InitMemory()
{
	int	i;
	//Initialize Memory for  Control transfers
	memset((UCHAR *)SETUP_BUFF_ADDR,0,4096);

	//Initialize Memory for OUT Data transfers
	memset((UCHAR *)DATA_OUT_ADDR,0,20480);
	
	//Initialize Memory for IN Data transfers
	memset((UCHAR *)DATA_IN_ADDR,0,20480);

	//Initialize Memory for Intr in 
	memset((UCHAR *)INTR_IN_ADDR,0,1048567);	

	//Initialize Memory for Intr Out
	memset((UCHAR *)INTR_OUT_ADDR,0,1048567);	
	
	//Initialize Memory for Periodic FrameList 0x80210000
	memset((UCHAR *)PERIOD_FRAMELIST_ADDR,0,20480);	

	//Initialize all the locations of the Periodic Frame List with the termination value here  ??
	for (i=0;i<1024;i++)
		*((UINT *)PERIOD_FRAMELIST_ADDR+i)=0x00000003;//0x01
	
	//Fill the PeriodicListBase Register with PeriodicFrameList base address
//	writew(PERIOD_FRAMELIST_ADDR,PERIODICLISTBASE(ehci_base));		// commented by vinchip since the address specified is not physical address
	
	writew(virt_to_phy(PERIOD_FRAMELIST_ADDR),PERIODICLISTBASE(ehci_base));

}

int HostCtrlReset()
{
	ULONG UsbCmd,UsbSts;
	
	UsbSts=readw(USBSTS(ehci_base));
	while(!(UsbSts & CMD_RESET))
	{
		//HC not halted ,Set the Run/Stop bit to 0
		UsbCmd=readw(USBCMD(ehci_base));
		UsbCmd=UsbCmd & (~CMD_RUN);         
		writew(UsbCmd,USBCMD(ehci_base));
		UsbSts=readw(USBSTS(ehci_base));
	}
	printf(("EHC Halted Successfully\n"));
	
	//Apply HC Reset
	EHCI_Reset();

	return 0;

}

int ResetDevice()
{
	ULONG UsbCmd,UsbSts,PortSc1;
	socle_ehci_show(("Inside ResetDevice\n"));

	//Check whether the HC is halted,if yes set the RUN bit 
	UsbSts=readw(USBSTS(ehci_base));

#if 1
	while(UsbSts & STS_HALT)		// should be checked
	{
		UsbCmd=readw(USBCMD(ehci_base));
//			command |= CMD_RESET;
		UsbCmd=UsbCmd|CMD_RUN;
		writew(UsbCmd,USBCMD(ehci_base));
		UsbSts=readw(USBSTS(ehci_base));
	}
	
#else
	//reg 1.5
	if(UsbSts & STS_HALT)
	{
		printf(("\n ## USBCMD(ehci_base)                  :%x",readw(USBCMD(ehci_base))));
		printf(("ERROR : Device Reset Failed since HC is in Halted state (RUN bit not set)"));
		return FALSE;
	}
#endif
	//Give Reset
	PortSc1=readw(PORTSC(ehci_base));
	socle_ehci_show(("setting port reset bit \n"));
	PortSc1=PortSc1|PORT_RESET;
	writew(PortSc1,PORTSC(ehci_base));		// setting the port reset bit
	MSDELAY(20);
	PortSc1=readw(PORTSC(ehci_base));
	socle_ehci_show(("clearing port reset bit"));
	PortSc1 = PortSc1 & (~PORT_RESET);
	writew(PortSc1,PORTSC(ehci_base));		// clearing the port reset bit
	MSDELAY(2);
	PortSc1=readw(PORTSC(ehci_base));
	socle_ehci_show("the value of portsc register after reset clearing is %x\n",PortSc1);
	if(PortSc1 & PORT_PE)			// checking the port enable bit
	{
		socle_ehci_show(("Port enabled in high speed\n"));
	}
	else
	{
		socle_ehci_show(("port settled in full speed after reset\n"));
	}
	MpsKnown = 0;
	ehci_status.DevAddress = 0;
	return TRUE;
}

/*USB spec 9.4 
Standard Device Request
*/
void SetCtrlSetupDataFun(pCtrlRequest pCtrlReq,BYTE bRequest)
{

	pCtrlReq->bRequest = bRequest;

	switch(bRequest)
	{
		case USB_REQ_CLEAR_FEATURE:
			//bRequest
			break;
		case USB_REQ_SET_ADDRESS:
			pCtrlReq->bRequestType = 0x00;
			pCtrlReq->wValue = ehci_status.DevAddress;
			pCtrlReq->wIndex = 0x00;
			pCtrlReq->wLength = 0x00;
			break;
		default:
			printf("Type Error");
			break;
	}

//	return pCtrlReq;
	
}

/* ******************************************************************************** *   
Function Name : int SetAddress(BYTE DevAddress)
Description   : Set Device  Address 
Params        :  Device Address
Returns       : True:Set Device Address Success  / False: Set Device Address Fail    
Note          :  
******************************************************************************** */

int SetAddress(BYTE DevAddress)
{
	PQHEAD 	pQHEAD1;
	PQTD 	pQTD1,pQTD2;
	ULONG 	PhyQHEAD1,PhyQTD1,PhyQTD2,PhyBuffer0;
	UCHAR 	*VirBuffAddr;
	unsigned int	UnderHub;	// used to specify whether the current transfert is for the device connected in the root hub or under a hub.
	UnderHub = 0;
	if(autorun == 1)
		UnderHub= 0;
	else{
		printf("Set Address for a device under hub ?\n");
		printf("0.No\n");
		printf("1.Yes\n");
		scanf("%d",&UnderHub);
	}

	//Disable Asynchronous list processing
	EHCI_Asynchronous_Enable(0);
	
	VirBuffAddr=(UCHAR *)SETUP_BUFF_ADDR;
	PhyBuffer0=virt_to_phy((u32_t)(VirBuffAddr+0));
	
	//For the 8 bytes of setup stage
	*(VirBuffAddr+0)=0x00;
	*(VirBuffAddr+1)=USB_REQ_SET_ADDRESS;
	*(VirBuffAddr+2)=0x0 |DevAddress;//Get Address from Application.
	*(VirBuffAddr+3)=0x00;
	*(VirBuffAddr+4)=0x00;
	*(VirBuffAddr+5)=0x00;
	*(VirBuffAddr+6)=0x00;
	*(VirBuffAddr+7)=0x00;
	
	
	//Form QTD2 for Status Stage of SetAddress=========================
	pQTD2 = Get_Free_QTD();
	PhyQTD2=virt_to_phy((u32_t) pQTD2);
	
	pQTD2->NextQTD=T_BIT;
	pQTD2->AltQTD=T_BIT;
	// data toggle field 1,IOC is enabled,CERR field 3,PID field 1,Active bit is set.
	//pQTD2->QTDChar=0x80008d80;
	pQTD2->QTDChar = DT |IOC |C_ERR|PID_IN|ACTIVE; //Ryan
	pQTD2->QTDBuffPrt0=0x00000000;
	pQTD2->QTDBuffPrt1=0x00000000;
	pQTD2->QTDBuffPrt2=0x00000000;
	pQTD2->QTDBuffPrt3=0x00000000;
	pQTD2->QTDBuffPrt4=0x00000000;
	socle_ehci_show("the value of pqtd2.NextQTD is %x\n",pQTD2->NextQTD);
	socle_ehci_show("the value of pQTD2->AltQTD is %x\n",pQTD2->AltQTD);
	socle_ehci_show("the value of pQTD2->QTDChar is %x\n",pQTD2->QTDChar);
	socle_ehci_show("the value of pQTD2->QTDBuffPrt0 is %x\n",pQTD2->QTDBuffPrt0);
	socle_ehci_show("the value of pQTD2->QTDBuffPrt1 is %x\n",pQTD2->QTDBuffPrt1);
	socle_ehci_show("the value of pQTD2->QTDBuffPrt2 is %x\n",pQTD2->QTDBuffPrt2);
	socle_ehci_show("the value of pQTD2->QTDBuffPrt3 is %x\n",pQTD2->QTDBuffPrt3);
	socle_ehci_show("the value of pQTD2->QTDBuffPrt4 is %x\n",pQTD2->QTDBuffPrt4);
	
	//Form QTD1 for Setup Stage of SetAddress==========================
	//Data0
	pQTD1 = Get_Free_QTD();
	PhyQTD1=virt_to_phy((u32_t)pQTD1);
	
	pQTD1->NextQTD=(ULONG)PhyQTD2;
	pQTD1->AltQTD=(ULONG)PhyQTD2;
	// Total bytes to transfer field 8,CERR is 3,PID is 2,active bit is set
	//pQTD1->QTDChar=0x00080e80;		// changed the value from 00088e80 to 00080e80 by vinchip
	pQTD1->QTDChar= (8*TOTAL_LENGTH) | C_ERR|PID_SETUP |ACTIVE;
	pQTD1->QTDBuffPrt0=(ULONG)PhyBuffer0;
	pQTD1->QTDBuffPrt1=0x00000000;
	pQTD1->QTDBuffPrt2=0x00000000;
	pQTD1->QTDBuffPrt3=0x00000000;
	pQTD1->QTDBuffPrt4=0x00000000;
	socle_ehci_show("the value of pqtd1.NextQTD is %x\n",pQTD1->NextQTD);
	socle_ehci_show("the value of pQTD1->AltQTD is %x\n",pQTD1->AltQTD);
	socle_ehci_show("the value of pQTD1->QTDChar is %x\n",pQTD1->QTDChar);
	socle_ehci_show("the value of pQTD1->QTDBuffPrt0 is %x\n",pQTD1->QTDBuffPrt0);
	socle_ehci_show("the value of pQTD1->QTDBuffPrt1 is %x\n",pQTD1->QTDBuffPrt1);
	socle_ehci_show("the value of pQTD1->QTDBuffPrt2 is %x\n",pQTD1->QTDBuffPrt2);
	socle_ehci_show("the value of pQTD1->QTDBuffPrt3 is %x\n",pQTD1->QTDBuffPrt3);
	socle_ehci_show("the value of pQTD1->QTDBuffPrt4 is %x\n",pQTD1->QTDBuffPrt4);
	
	//Form QHEAD==========================================
	pQHEAD1 = Get_Free_QH();
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);
	
	pQHEAD1->QHEADLinkPtr=PhyQHEAD1|QH_TYPE;	//??
	if(UnderHub)
	{
		pQHEAD1->EpChar=(3*RL) |(C_Bit << 27)|(0x40 <<16) |H_BIT |DTC |(Device_Hub_Speed << 12) ;
		pQHEAD1->EpCap= MULT | (ehci_status.Portnum << 23)|(ehci_status.DevAddress << 16);
	}
	else
	{
		pQHEAD1->EpChar=(3*RL) |(0x40 <<16) |H_BIT |DTC |(EPS) |ehci_status.DevAddress;
		pQHEAD1->EpCap=MULT;
	}
//	pQHEAD1->EpChar=(3*RL) |(0x40 <<16) |H_BIT |DTC |(EPS) |ehci_status.DevAddress;
	//pQHEAD1->EpChar=0x3040e000|ehci_status.DevAddress;//eumlate default DevAddress;
	
//	pQHEAD1->EpCap=MULT;
	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;
	socle_ehci_show("the value of pQHEAD1->QHEADLinkPtr is %x\n",pQHEAD1->QHEADLinkPtr);
	socle_ehci_show("the value of pQHEAD1->EpChar is %x\n",pQHEAD1->EpChar);
	socle_ehci_show("the value of pQHEAD1->EpCap is %x\n",pQHEAD1->EpCap);
	socle_ehci_show("the value of pQHEAD1->CurrQTD is %x\n",pQHEAD1->CurrQTD);
	socle_ehci_show("the value of pQHEAD1->NextQTD is %x\n",pQHEAD1->NextQTD);
	socle_ehci_show("the value of pQHEAD1->AltQTd is %x\n",pQHEAD1->AltQTd);

	socle_ehci_show("the value  of PhyQHEAD is %x\n",PhyQHEAD1);
	//Write the physical address into ASYNCICLISTADDR(ehci_base) Register
	
	writew((ULONG)PhyQHEAD1,ASYNCICLISTADDR(ehci_base));
	
	socle_ehci_show("the value in Aynclistaddr is %x\n",readw(ASYNCICLISTADDR(ehci_base)));
	
	//Enable Asynchronous list
	EHCI_Asynchronous_Enable(1);		
	while(!(TransferComplete | ErrorComplete))
	{
	}
	if(TransferComplete)
	{
		socle_ehci_show("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		socle_ehci_show("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		if(Success(pQTD2->QTDChar))		// checking the status of the completion
		{
			if(UnderHub)
			{
				ehci_status.DevAddress_Hub = DevAddress;
			}
			else
			{
				ehci_status.DevAddress=DevAddress;
			}
			socle_ehci_show("Set Address Passed in Driver\n");
		}
		else
		{
			socle_ehci_show("some other error\n");
		}
	}
	else if(ErrorComplete)
	{
		socle_ehci_show("Transfer failed\n");
		socle_ehci_show("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		socle_ehci_show("the value of qtd2 char is %x\n",pQTD2->QTDChar);
	}
	TransferComplete = 0;
	ErrorComplete = 0;
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QTD_Ring(pQTD1);		
	Add_Free_QTD_Ring(pQTD2);
	return TRUE;
}

/* ******************************************************************************** *   
Function Name : int GetDescriptor(BYTE DevDescType,BYTE DevDescIndex,int DevDescLength)
Description   : Get Device Descriptor 
Params        : Descriptor Type,Descriptor Index,Descriptor Length
Returns       : True:Get Device Descriptor Success  / False: Get Device Descriptor Fail    
Note          :  
******************************************************************************** */
int GetDescriptor(BYTE DevDescType,BYTE DevDescIndex,int DevDescLength)
{
	unsigned int i;
	unsigned int BytesTransferred;
	unsigned int InitialTransferLength;
	PQHEAD pQHEAD1;
	PQTD pQTD1,pQTD2,pQTD3;
	ULONG PhyQHEAD1,PhyQTD1,PhyQTD2,PhyQTD3,PhyBuffer0,PhyBuffer1;
	UCHAR *VirBuffAddr0,*VirBuffAddr1;
	unsigned int	UnderHub;
	UnderHub = 0;
	if (autorun == 1)
		UnderHub = 0;
	else{
		printf("for a device under hub ?\n");
		printf("0.No\n");
		printf("1.Yes\n");
		scanf("%d",&UnderHub);
	}
	//Disable Asynchronous list processing
	EHCI_Asynchronous_Enable(0);
	
	VirBuffAddr0=((UCHAR *)SETUP_BUFF_ADDR);						// for setup data
	VirBuffAddr1=(UCHAR *)((UCHAR *)SETUP_BUFF_ADDR + 0x20);		// for descriptor
	PhyBuffer0=virt_to_phy((u32_t)(VirBuffAddr0));
	PhyBuffer1=virt_to_phy((u32_t)(VirBuffAddr1));

	//For the 8 bytes of setup stage
	*(VirBuffAddr0+0)=0x80;
	*(VirBuffAddr0+1)=USB_REQ_GET_DESCRIPTOR;
	*(VirBuffAddr0+2)=0x00;// CHANGE Descriptor Index Here
	*(VirBuffAddr0+3)=0x00|DevDescType;// CHANGE Descriptor Type Here
	*(VirBuffAddr0+4)=0x00;
	*(VirBuffAddr0+5)=0x00;
	*(VirBuffAddr0+6)=0x00|DevDescLength;// CHANGE Descriptor Length Here
	*(VirBuffAddr0+7)=0x00|DevDescLength>>8;
		
	
	//Form QTD3 for Status Stage of GetDescriptor==============================

	pQTD3 = Get_Free_QTD();
	PhyQTD3=virt_to_phy((u32_t)pQTD3);
	
	//Change the data toggle to 1
	pQTD3->NextQTD=T_BIT;
	pQTD3->AltQTD=T_BIT;
	// data toggle field 1,IOC is enabled,CERR is 3,PID field is 0,Active bit is set
//	pQTD3->QTDChar=0x80008c80;
	pQTD3->QTDChar = DT |IOC |C_ERR|PID_OUT|ACTIVE; 
	pQTD3->QTDBuffPrt0=0x00000000;
	pQTD3->QTDBuffPrt1=0x00000000;
	pQTD3->QTDBuffPrt2=0x00000000;
	pQTD3->QTDBuffPrt3=0x00000000;
	pQTD3->QTDBuffPrt4=0x00000000;
	socle_ehci_show("the value of pqtd3.NextQTD is %x\n",pQTD3->NextQTD);
	socle_ehci_show("the value of pQTD3->AltQTD is %x\n",pQTD3->AltQTD);
	socle_ehci_show("the value of pQTD3->QTDChar is %x\n",pQTD3->QTDChar);
	socle_ehci_show("the value of pQTD3->QTDBuffPrt0 is %x\n",pQTD3->QTDBuffPrt0);
	socle_ehci_show("the value of pQTD3->QTDBuffPrt1 is %x\n",pQTD3->QTDBuffPrt1);
	socle_ehci_show("the value of pQTD3->QTDBuffPrt2 is %x\n",pQTD3->QTDBuffPrt2);
	socle_ehci_show("the value of pQTD3->QTDBuffPrt3 is %x\n",pQTD3->QTDBuffPrt3);
	socle_ehci_show("the value of pQTD3->QTDBuffPrt4 is %x\n",pQTD3->QTDBuffPrt4);

	//Form QTD2 for Data Stage of GetDescriptor===============================
	pQTD2 = Get_Free_QTD();
	//pQTD2=(PQTD)((UCHAR)SETUP_BUFF_ADDR+256);
	PhyQTD2=virt_to_phy((u32_t)pQTD2);
	
	pQTD2->NextQTD=(ULONG)PhyQTD3;
	pQTD2->AltQTD=(ULONG)PhyQTD3;
	// data toggle field 1,CERR field 3,PID field 1,Active bit is set 1.
//	pQTD2->QTDChar=0x80008d80|(DevDescLength<<16);	// for testing enabling the ioc
//	pQTD2->QTDChar=0x80000d80|(DevDescLength<<16);		// disabling the ioc since only in last qtd i,e status stage
	pQTD2->QTDChar= DT |C_ERR|PID_IN|ACTIVE|(DevDescLength*TOTAL_LENGTH);		// disabling the ioc since only in last qtd i,e status stage
	pQTD2->QTDBuffPrt0=(ULONG)PhyBuffer1;
	pQTD2->QTDBuffPrt1=0x00000000;
	pQTD2->QTDBuffPrt2=0x00000000;
	pQTD2->QTDBuffPrt3=0x00000000;
	pQTD2->QTDBuffPrt4=0x00000000;
	InitialTransferLength = (pQTD2->QTDChar & 0x7FFF0000)>> 16;
	socle_ehci_show("the value of pqtd2.NextQTD is %x\n",pQTD2->NextQTD);
	socle_ehci_show("the value of pQTD2->AltQTD is %x\n",pQTD2->AltQTD);
	socle_ehci_show("the value of pQTD2->QTDChar is %x\n",pQTD2->QTDChar);
	socle_ehci_show("the value of pQTD2->QTDBuffPrt0 is %x\n",pQTD2->QTDBuffPrt0);
	socle_ehci_show("the value of pQTD2->QTDBuffPrt1 is %x\n",pQTD2->QTDBuffPrt1);
	socle_ehci_show("the value of pQTD2->QTDBuffPrt2 is %x\n",pQTD2->QTDBuffPrt2);
	socle_ehci_show("the value of pQTD2->QTDBuffPrt3 is %x\n",pQTD2->QTDBuffPrt3);
	socle_ehci_show("the value of pQTD2->QTDBuffPrt4 is %x\n",pQTD2->QTDBuffPrt4);
	
	//Form QTD1 for Setup Stage of GetDescriptor==============================
	pQTD1 = Get_Free_QTD();
	PhyQTD1=virt_to_phy((u32_t)pQTD1);
	
	pQTD1->NextQTD=(ULONG)PhyQTD2;
	pQTD1->AltQTD=(ULONG)PhyQTD2;
	// data toggle field 0,total bytes to transfer is set to 8,CERR field 3,PID field 0,Active bit is enabled

//	pQTD1->QTDChar=0x00088e80;								// for testing enabling the ioc
//	pQTD1->QTDChar=0x00080e80;								// disabling the ioc only in last qtd
	pQTD1->QTDChar= (8*TOTAL_LENGTH) | C_ERR|PID_SETUP |ACTIVE;
	pQTD1->QTDBuffPrt0=(ULONG)PhyBuffer0;
	pQTD1->QTDBuffPrt1=0x00000000;
	pQTD1->QTDBuffPrt2=0x00000000;
	pQTD1->QTDBuffPrt3=0x00000000;
	pQTD1->QTDBuffPrt4=0x00000000;
	socle_ehci_show("the value of pqtd1.NextQTD is %x\n",pQTD1->NextQTD);
	socle_ehci_show("the value of pQTD1->AltQTD is %x\n",pQTD1->AltQTD);
	socle_ehci_show("the value of pQTD1->QTDChar is %x\n",pQTD1->QTDChar);
	socle_ehci_show("the value of pQTD1->QTDBuffPrt0 is %x\n",pQTD1->QTDBuffPrt0);
	socle_ehci_show("the value of pQTD1->QTDBuffPrt1 is %x\n",pQTD1->QTDBuffPrt1);
	socle_ehci_show("the value of pQTD1->QTDBuffPrt2 is %x\n",pQTD1->QTDBuffPrt2);
	socle_ehci_show("the value of pQTD1->QTDBuffPrt3 is %x\n",pQTD1->QTDBuffPrt3);
	socle_ehci_show("the value of pQTD1->QTDBuffPrt4 is %x\n",pQTD1->QTDBuffPrt4);
	
	//Form QHEAD====================================================
	pQHEAD1 = Get_Free_QH();
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);
	
	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
	if(MpsKnown == 0)
	{
		if(UnderHub)
		{
			pQHEAD1->EpChar=(3*RL) |(C_Bit << 27)|(0x40 <<16) |H_BIT |DTC |(Device_Hub_Speed << 12) |(ehci_status.DevAddress_Hub);
			pQHEAD1->EpCap= MULT | (ehci_status.Portnum << 23)|(ehci_status.DevAddress << 16);
		}
		else
		{
			pQHEAD1->EpChar=0x0040e000|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
			pQHEAD1->EpCap=0x40000000; 					// mult field is set to 1
		}
//		pQHEAD1->EpChar=0x0040e000|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
	}
	else
	{
		if(UnderHub)
		{
			pQHEAD1->EpChar=(3*RL) |(C_Bit << 27)|(ehci_status.CepMps_Hub <<16) |H_BIT |DTC |(Device_Hub_Speed << 12) |(ehci_status.DevAddress_Hub);
			pQHEAD1->EpCap= MULT | (ehci_status.Portnum << 23)|(ehci_status.DevAddress << 16);
		}
		else
		{
			pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
			pQHEAD1->EpCap=0x40000000; 					// mult field is set to 1
		}
//		pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
	}
//	pQHEAD1->EpCap=0x40000000;						// mult field is set to 1
	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;
	
	socle_ehci_show("the value of pQHEAD1->QHEADLinkPtr is %x\n",pQHEAD1->QHEADLinkPtr);
	socle_ehci_show("the value of pQHEAD1->EpChar is %x\n",pQHEAD1->EpChar);
	socle_ehci_show("the value of pQHEAD1->EpCap is %x\n",pQHEAD1->EpCap);
	socle_ehci_show("the value of pQHEAD1->CurrQTD is %x\n",pQHEAD1->CurrQTD);
	socle_ehci_show("the value of pQHEAD1->NextQTD is %x\n",pQHEAD1->NextQTD);
	socle_ehci_show("the value of pQHEAD1->AltQTd is %x\n",pQHEAD1->AltQTd);
	writew((ULONG)PhyQHEAD1,ASYNCICLISTADDR(ehci_base));
	
	EHCI_Asynchronous_Enable(1);
	//Enable Asynchronous list
	socle_ehci_show("the value of Asynclistbase addr register is %x\n",readw(ASYNCICLISTADDR(ehci_base)));
	
	while(!(TransferComplete | ErrorComplete))
	{
	}
	if(TransferComplete)
	{
		socle_ehci_show("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		socle_ehci_show("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		socle_ehci_show("the value of qtd3 char is %x\n",pQTD3->QTDChar);
		if(Success(pQTD3->QTDChar)) 	// checking the status of the completion
		{
			socle_ehci_show("Get Descriptor Passed in Driver");
			if(DevDescType == 1)
			{
				ehci_status.CepMps = *(VirBuffAddr1 + 7);
				MpsKnown = 1;
				if(UnderHub)
				{
					ehci_status.CepMps_Hub= *(VirBuffAddr1 + 7);
					socle_ehci_show("the value of control endpoint mps is %x\n",ehci_status.CepMps_Hub);
				}
				socle_ehci_show("the value of control endpoint mps is %x\n",ehci_status.CepMps);
			}
			BytesTransferred = InitialTransferLength - ((pQTD2->QTDChar & 0x7FFF0000)>> 16);
			socle_ehci_show("Total bytes transferred is %d\n",BytesTransferred);
			for(i=0;i<BytesTransferred;i++)
			{
				socle_ehci_show("the value of %d byte is %x\n",i,*(VirBuffAddr1+i));
				if(i == 4)
				{
					if(*(VirBuffAddr1+i) == 0x09)	// Hub class
					{
						socle_ehci_show("Connected device is a hub class device\n");
						if(*(VirBuffAddr1+6) == 1 )
						{
							socle_ehci_show("single TT\n");
							MultipleTT = 0;
						}
						else
						{
							socle_ehci_show("Mulitiple TT \n");
							MultipleTT = 1;
						}
					}
				}
			}
		}
		else
		{
			socle_ehci_show("some other error\n");
		}
	}
	else if(ErrorComplete)
	{
		socle_ehci_show("Transfer failed\n");
		socle_ehci_show("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		socle_ehci_show("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		socle_ehci_show("the value of qtd3 char is %x\n",pQTD3->QTDChar);
	}
	socle_ehci_show("the value of pQHEAD1->QHEADLinkPtr is %x\n",pQHEAD1->QHEADLinkPtr);
	socle_ehci_show("the value of pQHEAD1->EpChar is %x\n",pQHEAD1->EpChar);
	socle_ehci_show("the value of pQHEAD1->EpCap is %x\n",pQHEAD1->EpCap);
	socle_ehci_show("the value of pQHEAD1->CurrQTD is %x\n",pQHEAD1->CurrQTD);
	socle_ehci_show("the value of pQHEAD1->NextQTD is %x\n",pQHEAD1->NextQTD);
	socle_ehci_show("the value of pQHEAD1->AltQTd is %x\n",pQHEAD1->AltQTd);
	socle_ehci_show("the value of pQHEAD1->QtdChar is %x\n",pQHEAD1->QTDChar);
	socle_ehci_show("the value of pQHEAD1->Buffptr1 is %x\n",pQHEAD1->QHEADBuffPtr0);
	socle_ehci_show("the value of pQHEAD1->Buffptr2 is %x\n",pQHEAD1->QHEADBuffPtr1);
	socle_ehci_show("the value of pQHEAD1->Buffptr3 is %x\n",pQHEAD1->QHEADBuffPtr2);
	socle_ehci_show("the value of pQHEAD1->Buffptr4 is %x\n",pQHEAD1->QHEADBuffPtr3);
	socle_ehci_show("the value of pQHEAD1->Buffptr5 is %x\n",pQHEAD1->QHEADBuffPtr4);
	socle_ehci_show("the value of pqtd1.NextQTD is %x\n",pQTD1->NextQTD);
	socle_ehci_show("the value of pQTD1->AltQTD is %x\n",pQTD1->AltQTD);
	socle_ehci_show("the value of pQTD1->QTDChar is %x\n",pQTD1->QTDChar);
	socle_ehci_show("the value of pQTD1->QTDBuffPrt0 is %x\n",pQTD1->QTDBuffPrt0);
	socle_ehci_show("the value of pQTD1->QTDBuffPrt1 is %x\n",pQTD1->QTDBuffPrt1);
	socle_ehci_show("the value of pQTD1->QTDBuffPrt2 is %x\n",pQTD1->QTDBuffPrt2);
	socle_ehci_show("the value of pQTD1->QTDBuffPrt3 is %x\n",pQTD1->QTDBuffPrt3);
	socle_ehci_show("the value of pQTD1->QTDBuffPrt4 is %x\n",pQTD1->QTDBuffPrt4);
	socle_ehci_show("the value of pqtd2.NextQTD is %x\n",pQTD2->NextQTD);
	socle_ehci_show("the value of pQTD2->AltQTD is %x\n",pQTD2->AltQTD);
	socle_ehci_show("the value of pQTD2->QTDChar is %x\n",pQTD2->QTDChar);
	socle_ehci_show("the value of pQTD2->QTDBuffPrt0 is %x\n",pQTD2->QTDBuffPrt0);
	socle_ehci_show("the value of pQTD2->QTDBuffPrt1 is %x\n",pQTD2->QTDBuffPrt1);
	socle_ehci_show("the value of pQTD2->QTDBuffPrt2 is %x\n",pQTD2->QTDBuffPrt2);
	socle_ehci_show("the value of pQTD2->QTDBuffPrt3 is %x\n",pQTD2->QTDBuffPrt3);
	socle_ehci_show("the value of pQTD2->QTDBuffPrt4 is %x\n",pQTD2->QTDBuffPrt4);
	socle_ehci_show("the value of pqtd3.NextQTD is %x\n",pQTD3->NextQTD);
	socle_ehci_show("the value of pQTD3->AltQTD is %x\n",pQTD3->AltQTD);
	socle_ehci_show("the value of pQTD3->QTDChar is %x\n",pQTD3->QTDChar);
	socle_ehci_show("the value of pQTD3->QTDBuffPrt0 is %x\n",pQTD3->QTDBuffPrt0);
	socle_ehci_show("the value of pQTD3->QTDBuffPrt1 is %x\n",pQTD3->QTDBuffPrt1);
	socle_ehci_show("the value of pQTD3->QTDBuffPrt2 is %x\n",pQTD3->QTDBuffPrt2);
	socle_ehci_show("the value of pQTD3->QTDBuffPrt3 is %x\n",pQTD3->QTDBuffPrt3);
	socle_ehci_show("the value of pQTD3->QTDBuffPrt4 is %x\n",pQTD3->QTDBuffPrt4);
	socle_ehci_show("the value of Asynclistbase addr register is %x\n",readw(ASYNCICLISTADDR(ehci_base)));
	
	TransferComplete = 0;
	ErrorComplete = 0;
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QTD_Ring(pQTD1);		
	Add_Free_QTD_Ring(pQTD2);
	Add_Free_QTD_Ring(pQTD3);
	return TRUE;
}

/* ******************************************************************************** *   
Function Name : int SetConfiguration(BYTE DevConfig)
Description   : Setting the device to a configured state 
Params        : Configuration value
Returns       : True:Setconfiguration Success  / False: SetConfiguration Fail    
Note          :  
******************************************************************************** */
int SetConfiguration(BYTE DevConfig)
{
	PQHEAD pQHEAD1;
	PQTD pQTD1,pQTD2;
	ULONG PhyQHEAD1,PhyQTD1,PhyQTD2,PhyBuffer0;
	UCHAR *VirBuffAddr;
	unsigned int	UnderHub;
	UnderHub = 0;
	if(autorun == 1)
		UnderHub = 0;
	else{
		printf("for a device under hub ?\n");
		printf("0.No\n");
		printf("1.Yes\n");
		scanf("%d",&UnderHub);
	}
	
	//Disable Asynchronous list processing	
	EHCI_Asynchronous_Enable(0);
	
	//printf("Setting Configuration:%x",DevConfig);
	
	VirBuffAddr=(UCHAR *)SETUP_BUFF_ADDR;
	PhyBuffer0=virt_to_phy((u32_t)(VirBuffAddr+0));

	//For the 8 bytes of setup stage
	*(VirBuffAddr+0)=0x00;
	*(VirBuffAddr+1)=USB_REQ_SET_CONFIGURATION;
	*(VirBuffAddr+2)=0x00|(DevConfig);
	*(VirBuffAddr+3)=0x00;
	*(VirBuffAddr+4)=0x00;
	*(VirBuffAddr+5)=0x00;
	*(VirBuffAddr+6)=0x00;
	*(VirBuffAddr+7)=0x00;

	//Form QTD2 for Status Stage of SetConfiguration========================

	pQTD2 = Get_Free_QTD();
	PhyQTD2=virt_to_phy((u32_t)pQTD2);
	
	pQTD2->NextQTD=T_BIT;
	pQTD2->AltQTD=T_BIT;
//	pQTD2->QTDChar = DT|IOC|C_ERR |PID_OUT|ACTIVE;
//	pQTD2->QTDChar=0x80008D80;							// pid value is changed to in 
	pQTD2->QTDChar = DT |IOC |C_ERR|PID_IN|ACTIVE; 
	pQTD2->QTDBuffPrt0=0x00000000;
	pQTD2->QTDBuffPrt1=0x00000000;
	pQTD2->QTDBuffPrt2=0x00000000;
	pQTD2->QTDBuffPrt3=0x00000000;
	pQTD2->QTDBuffPrt4=0x00000000;

	//Form QTD1 for Setup Stage of SetConfiguration=========================
	pQTD1 = Get_Free_QTD();
	//pQTD1=(PQTD)(SETUP_BUFF_ADDR+416);
	PhyQTD1=virt_to_phy((u32_t)pQTD1);
	
	pQTD1->NextQTD=(ULONG)PhyQTD2;
	pQTD1->AltQTD=(ULONG)PhyQTD2;
//	pQTD1->QTDChar=(8*TOTAL_LENGTH) |IOC |C_ERR |PID_SETUP|ACTIVE;	
//	pQTD1->QTDChar=0x00080e80;							// ioc is disabled since only in status stage it is enabled
	pQTD1->QTDChar= (8*TOTAL_LENGTH) | C_ERR|PID_SETUP |ACTIVE;
	pQTD1->QTDBuffPrt0=(ULONG)PhyBuffer0;
	pQTD1->QTDBuffPrt1=0x00000000;
	pQTD1->QTDBuffPrt2=0x00000000;
	pQTD1->QTDBuffPrt3=0x00000000;
	pQTD1->QTDBuffPrt4=0x00000000;

	//Form QHEAD================================================
	pQHEAD1 = Get_Free_QH();
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);
	
	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
//	pQHEAD1->EpChar=(3*RL) |(0x40 <<16) |H_BIT |DTC |(EPS) |ehci_status.DevAddress;	
	if(UnderHub)
	{
		pQHEAD1->EpChar=(3*RL) |(C_Bit << 27)|(ehci_status.CepMps_Hub <<16) |H_BIT |DTC |(Device_Hub_Speed << 12) |(ehci_status.DevAddress_Hub);
		pQHEAD1->EpCap= MULT | (ehci_status.Portnum << 23)|(ehci_status.DevAddress << 16);
	}
	else
	{
		pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
		pQHEAD1->EpCap=0x40000000; 					// mult field is set to 1
	}
//	pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
//	pQHEAD1->EpCap=MULT;
	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;
	
	//Write the physical address into ASYNCICLISTADDR(ehci_base) Register
	writew(PhyQHEAD1,ASYNCICLISTADDR(ehci_base));
	
	//Enable Asynchronous list
	EHCI_Asynchronous_Enable(1);

	while(!(TransferComplete | ErrorComplete))
	{
	}
	if(TransferComplete)
	{
		socle_ehci_show("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		socle_ehci_show("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		if(Success(pQTD2->QTDChar)) 	// checking the status of the completion
		{
			socle_ehci_show("Set Configuration Passed in Driver");
		}
		else
		{
			socle_ehci_show("some other error\n");
		}
	}
	else if(ErrorComplete)
	{
		socle_ehci_show("Transfer failed\n");
		socle_ehci_show("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		socle_ehci_show("the value of qtd2 char is %x\n",pQTD2->QTDChar);
	}
	TransferComplete = 0;
	ErrorComplete = 0;
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QTD_Ring(pQTD1);		
	Add_Free_QTD_Ring(pQTD2);
	return TRUE;
}


/* ******************************************************************************** *   
Function Name : int GetConfiguration()
Description   : Get Current Device Configuration value 
Params        : NILL
Returns       : True:GetConfiguration Success  / False: GetConfiguration Fail    
Note          :  
******************************************************************************** */
int GetConfiguration()
{
	unsigned int i;
	unsigned int BytesTransferred;
	unsigned int InitialTransferLength;
	PQHEAD pQHEAD1;
	PQTD pQTD1,pQTD2,pQTD3;
	ULONG PhyQHEAD1,PhyQTD1,PhyQTD2,PhyQTD3,PhyBuffer0,PhyBuffer1;
	UCHAR *VirBuffAddr0,*VirBuffAddr1;
	unsigned int	UnderHub;
	UnderHub = 0;
	printf("for a device under hub ?\n");
	printf("0.No\n");
	printf("1.Yes\n");
	scanf("%d",&UnderHub);
	
	//Disable Asynchronous list processing
	EHCI_Asynchronous_Enable(0);
	
	VirBuffAddr0=((UCHAR *)SETUP_BUFF_ADDR);
	VirBuffAddr1=(UCHAR *)((UCHAR *)SETUP_BUFF_ADDR + 0x100);
	PhyBuffer0=virt_to_phy((u32_t)(VirBuffAddr0));
	PhyBuffer1=virt_to_phy((u32_t)(VirBuffAddr1));
	
	//For the 8 bytes of setup stage
	*(VirBuffAddr0+0)=0x80;
	*(VirBuffAddr0+1)=USB_REQ_GET_CONFIGURATION;
	*(VirBuffAddr0+2)=0x00;
	*(VirBuffAddr0+3)=0x00;
	*(VirBuffAddr0+4)=0x00;
	*(VirBuffAddr0+5)=0x00;
	*(VirBuffAddr0+6)=0x01;
	*(VirBuffAddr0+7)=0x00;
	
	//printf(("Initializing 1 QHEAD and 3 QTDs for GetConfiguration"));
	
	//Form QTD3 for Status Stage of GetConfiguration==============================
	pQTD3 = Get_Free_QTD();
	PhyQTD3=virt_to_phy((u32_t)pQTD3);
	
	pQTD3->NextQTD=T_BIT;
	pQTD3->AltQTD=T_BIT;
//	pQTD3->QTDChar=0x80008c80;
	pQTD3->QTDChar = DT |IOC |C_ERR|PID_OUT|ACTIVE; 
	pQTD3->QTDBuffPrt0=0x00000000;
	pQTD3->QTDBuffPrt1=0x00000000;
	pQTD3->QTDBuffPrt2=0x00000000;
	pQTD3->QTDBuffPrt3=0x00000000;
	pQTD3->QTDBuffPrt4=0x00000000;
	
	//Form QTD2 for Data Stage of GetConfiguration===============================
	pQTD2 = Get_Free_QTD();
	PhyQTD2=virt_to_phy((u32_t)pQTD2);
	
	pQTD2->NextQTD=(ULONG)PhyQTD3;
	pQTD2->AltQTD =(ULONG)PhyQTD3;
//	pQTD2->QTDChar=0x80010d80;				// disabling the ioc since only enabled in status stage
	pQTD2->QTDChar= DT |C_ERR|PID_IN|ACTIVE|(0x01*TOTAL_LENGTH);	// since transfer length is 1 byte ((0x01<<16))
	pQTD2->QTDBuffPrt0=(ULONG)PhyBuffer1;
	pQTD2->QTDBuffPrt1=0x00000000;
	pQTD2->QTDBuffPrt2=0x00000000;
	pQTD2->QTDBuffPrt3=0x00000000;
	pQTD2->QTDBuffPrt4=0x00000000;
	InitialTransferLength = (pQTD2->QTDChar & 0x7FFF0000)>> 16;

	//Form QTD1 for Setup Stage of GetConfiguration===============================
	pQTD1 = Get_Free_QTD();
	PhyQTD1=virt_to_phy((u32_t)pQTD1);
	
	pQTD1->NextQTD=(ULONG)PhyQTD2;
	pQTD1->AltQTD=(ULONG)PhyQTD2;
//	pQTD1->QTDChar=0x00080e80;				// disabling the ioc since only enabled in status stage
	pQTD1->QTDChar= (8*TOTAL_LENGTH) | C_ERR|PID_SETUP |ACTIVE;
	pQTD1->QTDBuffPrt0=(ULONG)PhyBuffer0;
	pQTD1->QTDBuffPrt1=0x00000000;
	pQTD1->QTDBuffPrt2=0x00000000;
	pQTD1->QTDBuffPrt3=0x00000000;
	pQTD1->QTDBuffPrt4=0x00000000;
	
	//Form QHEAD==================================================
	pQHEAD1 = Get_Free_QH();
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);
	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
	if(UnderHub)
	{
		pQHEAD1->EpChar=(3*RL) |(C_Bit << 27)|(ehci_status.CepMps_Hub <<16) |H_BIT |DTC |(Device_Hub_Speed << 12) |(ehci_status.DevAddress_Hub);
		pQHEAD1->EpCap= MULT | (ehci_status.Portnum << 23)|(ehci_status.DevAddress << 16);
	}
	else
	{
		pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
		pQHEAD1->EpCap=0x40000000; 					// mult field is set to 1
	}
//	pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
//	pQHEAD1->EpCap=MULT;
	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;
	
	printf(("Writing Physical Address of QHEAD into ASYNCICLISTADDR(ehci_base)"));
	//Write the physical address into ASYNCICLISTADDR(ehci_base) Register
	writew(PhyQHEAD1,ASYNCICLISTADDR(ehci_base));
	

	//Enable Asynchronous list
	EHCI_Asynchronous_Enable(1);
	while(!(TransferComplete | ErrorComplete))
	{
	}
	if(TransferComplete)
	{
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		printf("the value of qtd3 char is %x\n",pQTD3->QTDChar);
		if(Success(pQTD3->QTDChar)) 	// checking the status of the completion
		{
			printf("Get Configuration Passed in Driver\n");
			BytesTransferred = InitialTransferLength - ((pQTD2->QTDChar & 0x7FFF0000)>> 16);
			printf("Total bytes transferred is %d\n",BytesTransferred);
			for(i=0;i<BytesTransferred;i++)
			{
				printf("the value of %d byte is %x",i,*(VirBuffAddr1+i));
			}
		}
		else
		{
			printf("some other error\n");
		}
	}
	else if(ErrorComplete)
	{
		printf("Transfer failed\n");
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		printf("the value of qtd3 char is %x\n",pQTD3->QTDChar);
	}
	TransferComplete = 0;
	ErrorComplete = 0;
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QTD_Ring(pQTD1);		
	Add_Free_QTD_Ring(pQTD2);
	Add_Free_QTD_Ring(pQTD3);
	return TRUE;
}


/* ******************************************************************************** *   
Function Name : int SetFeature(BYTE DevRequest,BYTE DevFeature,BYTE EPNO,BYTE EpDir)
Description   : selecting or enabling a certain feature of the device
Params        : bmRequest value,Feature Selector,Endpoint number(DevDescIndex),Endpoint Direction
Returns       : True:SetFeature Success  / False: SetFeature Fail    
Note          :  
******************************************************************************** */
int SetFeature(BYTE DevRequest,BYTE DevFeature,BYTE EPNO,BYTE EpDir)
{
	PQHEAD pQHEAD1;
	PQTD pQTD1,pQTD2;
	ULONG PhyQHEAD1,PhyQTD1,PhyQTD2,PhyBuffer0;
	UCHAR *VirBuffAddr;
	unsigned int	UnderHub;
	UnderHub = 0;
	printf("for a device under hub ?\n");
	printf("0.No\n");
	printf("1.Yes\n");
	scanf("%d",&UnderHub);
	
	//Disable Asynchronous list processing
	EHCI_Asynchronous_Enable(0);
	
	VirBuffAddr=((UCHAR *)SETUP_BUFF_ADDR);
	PhyBuffer0=virt_to_phy((u32_t)(VirBuffAddr+0));

	//For the 8 bytes of setup stage
	*(VirBuffAddr+0)=0x00|(DevRequest);
	*(VirBuffAddr+1)=USB_REQ_SET_FEATURE;
	*(VirBuffAddr+2)=0x00|(DevFeature);
	*(VirBuffAddr+3)=0x00;
	*(VirBuffAddr+4)=0x00 |EpDir |EPNO;
	*(VirBuffAddr+5)=0x00;
	*(VirBuffAddr+6)=0x00;
	*(VirBuffAddr+7)=0x00;
	
	//printf(("Initializing 1 QHEAD and 2 QTDs for SetFeature"));
	//Form QTD2 for Status Stage of SetFeature===================================
	pQTD2 = Get_Free_QTD();
	PhyQTD2=virt_to_phy((u32_t)pQTD2);
	
	pQTD2->NextQTD=T_BIT;
	pQTD2->AltQTD=T_BIT;
//	pQTD2->QTDChar=0x80008d80;
	pQTD2->QTDChar = DT |IOC |C_ERR|PID_IN|ACTIVE; 
	pQTD2->QTDBuffPrt0=0x00000000;
	pQTD2->QTDBuffPrt1=0x00000000;
	pQTD2->QTDBuffPrt2=0x00000000;
	pQTD2->QTDBuffPrt3=0x00000000;
	pQTD2->QTDBuffPrt4=0x00000000;
	
	//Form QTD1 for Setup Stage of SetFeature===================================
	pQTD1 = Get_Free_QTD();
	PhyQTD1=virt_to_phy((u32_t)pQTD1);

	pQTD1->NextQTD=(ULONG)PhyQTD2;
	pQTD1->AltQTD=(ULONG)PhyQTD2;
//	pQTD1->QTDChar=0x00088e80;
//	pQTD1->QTDChar=0x00080e80;					// ioc is disabled since it is only enabled for status stage.
	pQTD1->QTDChar= (8*TOTAL_LENGTH) | C_ERR|PID_SETUP |ACTIVE;
	pQTD1->QTDBuffPrt0=(ULONG)PhyBuffer0;
	pQTD1->QTDBuffPrt1=0x00000000;
	pQTD1->QTDBuffPrt2=0x00000000;
	pQTD1->QTDBuffPrt3=0x00000000;
	pQTD1->QTDBuffPrt4=0x00000000;

	//Form QHEAD================================================
	pQHEAD1 = Get_Free_QH();
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);
	
	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
	if(UnderHub)
	{
		pQHEAD1->EpChar=(3*RL) |(C_Bit << 27)|(ehci_status.CepMps_Hub <<16) |H_BIT |DTC |(Device_Hub_Speed << 12) |(ehci_status.DevAddress_Hub);
		pQHEAD1->EpCap= MULT | (ehci_status.Portnum << 23)|(ehci_status.DevAddress << 16);
	}
	else
	{
		pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
		pQHEAD1->EpCap=0x40000000; 					// mult field is set to 1
	}
//	pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
//	pQHEAD1->EpCap=MULT;
	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;
	
	printf(("Writing Physical Address of QHEAD into ASYNCICLISTADDR(ehci_base)"));
	//Write the physical address into ASYNCICLISTADDR(ehci_base) Register
	writew(PhyQHEAD1,ASYNCICLISTADDR(ehci_base));
	
	//Enable Asynchronous list
	EHCI_Asynchronous_Enable(1);
	
	while(!(TransferComplete | ErrorComplete))
	{
	}
	if(TransferComplete)
	{
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		if(Success(pQTD2->QTDChar)) 	// checking the status of the completion
		{
			printf("Set Feature Passed in Driver\n");
		}
		else
		{
			printf("some other error\n");
		}
	}
	else if(ErrorComplete)
	{
		printf("Transfer failed\n");
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
	}
	TransferComplete = 0;
	ErrorComplete = 0;
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QTD_Ring(pQTD1);		
	Add_Free_QTD_Ring(pQTD2);
	return TRUE;
}

/* ******************************************************************************** *   
Function Name : int SetInterface(BYTE DevAltSet)
Description   : selecting an alternate setting and interface number
Params        : Alternate Setting,Interface Number
Returns       : True:SetInterface Success  / False: SetInterface Fail    
Note          :  
******************************************************************************** */
int SetInterface(BYTE DevAltSet)
{
	PQHEAD pQHEAD1;
	PQTD pQTD1,pQTD2;
	ULONG PhyQHEAD1,PhyQTD1,PhyQTD2,PhyBuffer0;
	UCHAR *VirBuffAddr;
	unsigned int	UnderHub;
	UnderHub = 0;
	printf("for a device under hub ?\n");
	printf("0.No\n");
	printf("1.Yes\n");
	scanf("%d",&UnderHub);

	//Disable Asynchronous list processing
	EHCI_Asynchronous_Enable(0);
	
//	printf(("Device Alternate Setting :%x",DevAltSet));
//	printf(("Device Interface         :%x",DevIface));
	VirBuffAddr=((UCHAR *)SETUP_BUFF_ADDR);
	PhyBuffer0=virt_to_phy((u32_t)(VirBuffAddr+0));

	//For the 8 bytes of setup stage
	*(VirBuffAddr+0)=0x01;
	*(VirBuffAddr+1)=USB_REQ_SET_INTERFACE;
	*(VirBuffAddr+2)=0x00|(DevAltSet);
	*(VirBuffAddr+3)=0x00;
	*(VirBuffAddr+4)=0x00|(ehci_status.DevIface);
	*(VirBuffAddr+5)=0x00;
	*(VirBuffAddr+6)=0x00;
	*(VirBuffAddr+7)=0x00;

	printf(("Initializing 1 QHEAD and 2 QTDs for SetInterface"));
	
	//Form QTD2 for Status Stage of SetInterface==========================
	pQTD2 = Get_Free_QTD();
	PhyQTD2=virt_to_phy((u32_t)pQTD2);
	
	pQTD2->NextQTD=T_BIT;
	pQTD2->AltQTD=T_BIT;
//	pQTD2->QTDChar=0x80008d80;
	pQTD2->QTDChar = DT |IOC |C_ERR|PID_IN|ACTIVE; 
	pQTD2->QTDBuffPrt0=0x00000000;
	pQTD2->QTDBuffPrt1=0x00000000;
	pQTD2->QTDBuffPrt2=0x00000000;
	pQTD2->QTDBuffPrt3=0x00000000;
	pQTD2->QTDBuffPrt4=0x00000000;
	
	//Form QTD1 for Setup Stage of SetInterface==========================
	pQTD1 = Get_Free_QTD();
	PhyQTD1=virt_to_phy((u32_t)pQTD1);
	
	pQTD1->NextQTD=(ULONG)PhyQTD2;
	pQTD1->AltQTD=(ULONG)PhyQTD2;
//	pQTD1->QTDChar=0x00088e80;
//	pQTD1->QTDChar=0x00080e80;					// ioc is disabled, it is only enabled in status qtd
	pQTD1->QTDChar= (8*TOTAL_LENGTH) | C_ERR|PID_SETUP |ACTIVE;
	pQTD1->QTDBuffPrt0=(ULONG)PhyBuffer0;
	pQTD1->QTDBuffPrt1=0x00000000;
	pQTD1->QTDBuffPrt2=0x00000000;
	pQTD1->QTDBuffPrt3=0x00000000;
	pQTD1->QTDBuffPrt4=0x00000000;
	
	//Form QHEAD==============================================
	pQHEAD1 = Get_Free_QH();
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);
	
	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
	if(UnderHub)
	{
		pQHEAD1->EpChar=(3*RL) |(C_Bit << 27)|(ehci_status.CepMps_Hub <<16) |H_BIT |DTC |(Device_Hub_Speed << 12) |(ehci_status.DevAddress_Hub);
		pQHEAD1->EpCap= MULT | (ehci_status.Portnum << 23)|(ehci_status.DevAddress << 16);
	}
	else
	{
		pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
		pQHEAD1->EpCap=0x40000000; 					// mult field is set to 1
	}
//	pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
//	pQHEAD1->EpCap=MULT;
	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;

	//Write the physical address into ASYNCICLISTADDR(ehci_base) Register
	writew(PhyQHEAD1,ASYNCICLISTADDR(ehci_base));
	
	//Enable Asynchronous list
	EHCI_Asynchronous_Enable(1);
	while(!(TransferComplete | ErrorComplete))
	{
	}
	if(TransferComplete)
	{
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		if(Success(pQTD2->QTDChar)) 	// checking the status of the completion
		{
			printf("Set Interface Passed in Driver\n");
		}
		else
		{
			printf("some other error\n");
		}
	}
	else if(ErrorComplete)
	{
		printf("Transfer failed\n");
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
	}
	TransferComplete = 0;
	ErrorComplete = 0;
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QTD_Ring(pQTD1);		
	Add_Free_QTD_Ring(pQTD2);
	return TRUE;
}

/* ******************************************************************************** *   
Function Name : int ClearFeature(BYTE DevRequest,BYTE DevFeature,BYTE EPNO,BYTE EpDir)
Description   : clearing a certain feature of the device
Params        : bmRequest value,Feature Selector,Endpoint number,Endpoint Direction
Returns       : True:ClearFeature Success  / False: ClearFeature Fail    
Note          :  
******************************************************************************** */
int ClearFeature(BYTE DevRequest,BYTE DevFeature,BYTE EPNO,BYTE EpDir)
{
	PQHEAD pQHEAD1;
	PQTD pQTD1,pQTD2;
	ULONG PhyQHEAD1,PhyQTD1,PhyQTD2,PhyBuffer0;
	UCHAR *VirBuffAddr;
	unsigned int	UnderHub;
	UnderHub = 0;
	printf("for a device under hub ?\n");
	printf("0.No\n");
	printf("1.Yes\n");
	scanf("%d",&UnderHub);
	
	//Disable Asynchronous list processing
	EHCI_Asynchronous_Enable(0);
	
	VirBuffAddr=((UCHAR *)SETUP_BUFF_ADDR);
	PhyBuffer0=virt_to_phy((u32_t)(VirBuffAddr+0));
	
	//For the 8 bytes of setup stage===========================================
	*(VirBuffAddr+0)=0x0|(DevRequest);
	*(VirBuffAddr+1)=USB_REQ_CLEAR_FEATURE;
	*(VirBuffAddr+2)=0x0|(DevFeature);
	*(VirBuffAddr+3)=0x00;
	*(VirBuffAddr+4)=0x00 |EpDir |EPNO ;			// check whether the devdescindex is required here.
	*(VirBuffAddr+5)=0x00;
	*(VirBuffAddr+6)=0x00;
	*(VirBuffAddr+7)=0x00;
	
	printf(("Initializing 1 QHEAD and 2 QTDs for ClearFeature"));
	
	//Form QTD2 for Status Stage of ClearFeature==================================
	pQTD2 = Get_Free_QTD();
	PhyQTD2=virt_to_phy((u32_t)pQTD2);
	
	pQTD2->NextQTD=T_BIT;
	pQTD2->AltQTD=T_BIT;
//	pQTD2->QTDChar=0x80008d80;
	pQTD2->QTDChar = DT |IOC |C_ERR|PID_IN|ACTIVE; 
	pQTD2->QTDBuffPrt0=0x00000000;
	pQTD2->QTDBuffPrt1=0x00000000;
	pQTD2->QTDBuffPrt2=0x00000000;
	pQTD2->QTDBuffPrt3=0x00000000;
	pQTD2->QTDBuffPrt4=0x00000000;

	//Form QTD1 for Setup Stage of ClearFeature===================================
	pQTD1 = Get_Free_QTD();
	PhyQTD1=virt_to_phy((u32_t)pQTD1);
	
	pQTD1->NextQTD=(ULONG)PhyQTD2;
	pQTD1->AltQTD=(ULONG)PhyQTD2;
//	pQTD1->QTDChar=0x00088e80;
//	pQTD1->QTDChar=0x00080e80;					// ioc is disabled and it is only enabled in status qtd
	pQTD1->QTDChar= (8*TOTAL_LENGTH) | C_ERR|PID_SETUP |ACTIVE;
	pQTD1->QTDBuffPrt0=(ULONG)PhyBuffer0;
	pQTD1->QTDBuffPrt1=0x00000000;
	pQTD1->QTDBuffPrt2=0x00000000;
	pQTD1->QTDBuffPrt3=0x00000000;
	pQTD1->QTDBuffPrt4=0x00000000;

	//Form QHEAD=====================================================
	pQHEAD1 = Get_Free_QH();
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);

	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
	if(UnderHub)
	{
		pQHEAD1->EpChar=(3*RL) |(C_Bit << 27)|(ehci_status.CepMps_Hub <<16) |H_BIT |DTC |(Device_Hub_Speed << 12) |(ehci_status.DevAddress_Hub);
		pQHEAD1->EpCap= MULT | (ehci_status.Portnum << 23)|(ehci_status.DevAddress << 16);
	}
	else
	{
		pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
		pQHEAD1->EpCap=0x40000000; 					// mult field is set to 1
	}
//	pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
//	pQHEAD1->EpCap=MULT;
	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;
	
	//Write the physical address into ASYNCICLISTADDR(ehci_base) Register
	writew(PhyQHEAD1,ASYNCICLISTADDR(ehci_base));
	
	//Enable Asynchronous list
	EHCI_Asynchronous_Enable(1);

	while(!(TransferComplete | ErrorComplete))
	{
	}
	if(TransferComplete)
	{
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		if(Success(pQTD2->QTDChar)) 	// checking the status of the completion
		{
			printf("Clear Feature Passed in Driver\n");
		}
		else
		{
			printf("some other error\n");
		}
	}
	else if(ErrorComplete)
	{
		printf("Transfer failed\n");
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
	}
	TransferComplete = 0;
	ErrorComplete = 0;
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QTD_Ring(pQTD1);		
	Add_Free_QTD_Ring(pQTD2);
	return TRUE;
}

/* *********************************************************************************   
Function Name : int GetInterface(BYTE DevIface)
Description   : Getting the current alternate setting of the interface 
Params        : 
Returns       : True:GetInterface Success  / False: GetInterface Fail    
Note          :  
*********************************************************************************/
int GetInterface(BYTE DevIface)
{
	unsigned int i;
	unsigned int BytesTransferred;
	unsigned int InitialTransferLength;
	PQHEAD pQHEAD1;
	PQTD pQTD1,pQTD2,pQTD3;
	ULONG PhyQHEAD1,PhyQTD1,PhyQTD2,PhyQTD3,PhyBuffer0,PhyBuffer1;
	UCHAR *VirBuffAddr0,*VirBuffAddr1;
	unsigned int	UnderHub;
	UnderHub = 0;
	printf("for a device under hub ?\n");
	printf("0.No\n");
	printf("1.Yes\n");
	scanf("%d",&UnderHub);
	
	EHCI_Asynchronous_Enable(0);
	
	VirBuffAddr0=((UCHAR *)SETUP_BUFF_ADDR);
	VirBuffAddr1=(UCHAR *)((UCHAR *)SETUP_BUFF_ADDR+0x110);
	PhyBuffer0=virt_to_phy((u32_t)(VirBuffAddr0));
	PhyBuffer1=virt_to_phy((u32_t)(VirBuffAddr1));
	
	//For the 8 bytes of setup stage
	*(VirBuffAddr0+0)=0x81;
	*(VirBuffAddr0+1)=USB_REQ_GET_INTERFACE;
	*(VirBuffAddr0+2)=0x00;
	*(VirBuffAddr0+3)=0x00;
	*(VirBuffAddr0+4)=0x0|(DevIface);			// check whether the deviface should be here
	*(VirBuffAddr0+5)=0x00;
	*(VirBuffAddr0+6)=0x01;
	*(VirBuffAddr0+7)=0x00;
	
	//printf(("Initializing 1 QHEAD and 3 QTDs for GetInterface"));
	//Form QTD3 for Status Stage of GetInterface=-===============================
	pQTD3 = Get_Free_QTD();
	PhyQTD3=virt_to_phy((u32_t)pQTD3);

	pQTD3->NextQTD=T_BIT;
	pQTD3->AltQTD=T_BIT;
//	pQTD3->QTDChar=0x80008c80;
	pQTD3->QTDChar = DT |IOC |C_ERR|PID_OUT|ACTIVE; 
	pQTD3->QTDBuffPrt0=0x00000000;
	pQTD3->QTDBuffPrt1=0x00000000;
	pQTD3->QTDBuffPrt2=0x00000000;
	pQTD3->QTDBuffPrt3=0x00000000;
	pQTD3->QTDBuffPrt4=0x00000000;
	
	//Form QTD2 for Data Stage of GetInterface===================================
	pQTD2 = Get_Free_QTD();
	PhyQTD2=virt_to_phy((u32_t)pQTD2);
	
	pQTD2->NextQTD=(ULONG)PhyQTD3;
	pQTD2->AltQTD=(ULONG)PhyQTD3;
//	pQTD2->QTDChar=0x80018d80;
//	pQTD2->QTDChar=0x80010d80;				// ioc is disabled and it is only enabled in status qtd
	pQTD2->QTDChar= DT |C_ERR|PID_IN|ACTIVE|(0x01*TOTAL_LENGTH);	// since transfer length is 1 byte ((0x01<<16))
	pQTD2->QTDBuffPrt0=(ULONG)PhyBuffer1;
	pQTD2->QTDBuffPrt1=0x00000000;
	pQTD2->QTDBuffPrt2=0x00000000;
	pQTD2->QTDBuffPrt3=0x00000000;
	pQTD2->QTDBuffPrt4=0x00000000;
	InitialTransferLength = (pQTD2->QTDChar & 0x7FFF0000)>> 16;

	//Form QTD1 for Setup Stage of GetInterface==================================
	pQTD1 = Get_Free_QTD();
	PhyQTD1=virt_to_phy((u32_t)pQTD1);
	
	pQTD1->NextQTD=(ULONG)PhyQTD2;
	pQTD1->AltQTD=(ULONG)PhyQTD2;
//	pQTD1->QTDChar=0x00080e80;				// ioc is disabled and it is only enbled in status qtd
	pQTD1->QTDChar= (8*TOTAL_LENGTH) | C_ERR|PID_SETUP |ACTIVE;
	pQTD1->QTDBuffPrt0=(ULONG)PhyBuffer0;
	pQTD1->QTDBuffPrt1=0x00000000;
	pQTD1->QTDBuffPrt2=0x00000000;
	pQTD1->QTDBuffPrt3=0x00000000;
	pQTD1->QTDBuffPrt4=0x00000000;

	//Form QHEAD=====================================================
	pQHEAD1 = Get_Free_QH();
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);

	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
	if(UnderHub)
	{
		pQHEAD1->EpChar=(3*RL) |(C_Bit << 27)|(ehci_status.CepMps_Hub<<16) |H_BIT |DTC |(Device_Hub_Speed << 12) |(ehci_status.DevAddress_Hub);
		pQHEAD1->EpCap= MULT | (ehci_status.Portnum << 23)|(ehci_status.DevAddress << 16);
	}
	else
	{
		pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
		pQHEAD1->EpCap=0x40000000; 					// mult field is set to 1
	}
//	pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
//	pQHEAD1->EpCap=MULT;
	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;
	
	//Write the physical address into ASYNCICLISTADDR(ehci_base) Register
	writew(PhyQHEAD1,ASYNCICLISTADDR(ehci_base));
	
	//Enable Asynchronous list
	EHCI_Asynchronous_Enable(1);

	while(!(TransferComplete | ErrorComplete))
	{
	}
	if(TransferComplete)
	{
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		printf("the value of qtd3 char is %x\n",pQTD3->QTDChar);
		if(Success(pQTD3->QTDChar)) 	// checking the status of the completion
		{
			printf("Get Descriptor Passed in Driver");
			BytesTransferred = InitialTransferLength - ((pQTD2->QTDChar & 0x7FFF0000)>> 16);
			printf("Total bytes transferred is %d\n",BytesTransferred);
			for(i=0;i<BytesTransferred;i++)
			{
				printf("the value of %d byte is %x",i,*(VirBuffAddr1+i));
			}
		}
		else
		{
			printf("some other error\n");
		}
	}
	else if(ErrorComplete)
	{
		printf("Transfer failed\n");
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		printf("the value of qtd3 char is %x\n",pQTD3->QTDChar);
	}
	TransferComplete = 0;
	ErrorComplete = 0;
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QTD_Ring(pQTD1);		
	Add_Free_QTD_Ring(pQTD2);
	Add_Free_QTD_Ring(pQTD3);
	return TRUE;
}

/* ******************************************************************************** *   
Function Name : int GetStatus(BYTE DevRequest,BYTE DevDescIndex)
Description   : Reading the status of the device
Params        : bmrequest type
Returns       : True:GetStatus Success  / False: GetStatus Fail    
Note          :  
******************************************************************************** */
int GetStatus(BYTE DevRequest,BYTE DevDescIndex)
{
	unsigned int i;
	unsigned int BytesTransferred;
	unsigned int InitialTransferLength;
	PQHEAD pQHEAD1;
	PQTD pQTD1,pQTD2,pQTD3;
	ULONG PhyQHEAD1,PhyQTD1,PhyQTD2,PhyQTD3,PhyBuffer0,PhyBuffer1;
	UCHAR *VirBuffAddr0,*VirBuffAddr1;
	unsigned int	UnderHub;
	UnderHub = 0;
	printf("for a device under hub ?\n");
	printf("0.No\n");
	printf("1.Yes\n");
	scanf("%d",&UnderHub);
	
	EHCI_Asynchronous_Enable(0);
	
	VirBuffAddr0=((UCHAR *)SETUP_BUFF_ADDR);
	VirBuffAddr1=(UCHAR *)((UCHAR *)SETUP_BUFF_ADDR+0x120);
	PhyBuffer0=virt_to_phy((u32_t)(VirBuffAddr0));
	PhyBuffer1=virt_to_phy((u32_t)(VirBuffAddr1));
	
	//For the 8 bytes of setup stage
	*(VirBuffAddr0+0)=0x00|(DevRequest);
	*(VirBuffAddr0+1)=USB_REQ_GET_STATUS;
	*(VirBuffAddr0+2)=0x00;
	*(VirBuffAddr0+3)=0x00;
	*(VirBuffAddr0+4)=0x00|(DevDescIndex);			// check whether this devdescindex is required
	*(VirBuffAddr0+5)=0x00;
	*(VirBuffAddr0+6)=0x02;
	*(VirBuffAddr0+7)=0x00;
	
	//Form QTD3 for Status Stage of GetStatus=================================
	pQTD3 = Get_Free_QTD();
	PhyQTD3=virt_to_phy((u32_t)pQTD3);
	
	pQTD3->NextQTD=T_BIT;
	pQTD3->AltQTD=T_BIT;
//	pQTD3->QTDChar=0x80008c80;
	pQTD3->QTDChar = DT |IOC |C_ERR|PID_OUT|ACTIVE; 
	pQTD3->QTDBuffPrt0=0x00000000;
	pQTD3->QTDBuffPrt1=0x00000000;
	pQTD3->QTDBuffPrt2=0x00000000;
	pQTD3->QTDBuffPrt3=0x00000000;
	pQTD3->QTDBuffPrt4=0x00000000;
	
	//Form QTD2 for Data Stage of GetStatus=====================================
	pQTD2 = Get_Free_QTD();
	PhyQTD2=virt_to_phy((u32_t)pQTD2);

	pQTD2->NextQTD=(ULONG)PhyQTD3;
	pQTD2->AltQTD=(ULONG)PhyQTD3;
//	pQTD2->QTDChar=0x80028d80;
//	pQTD2->QTDChar=0x80020d80;				// ioc is disabled
	pQTD2->QTDChar= DT |C_ERR|PID_IN|ACTIVE|(0x02*TOTAL_LENGTH);	// since transfer length is 2 byte ((0x02<<16))
	pQTD2->QTDBuffPrt0=(ULONG)PhyBuffer1;
	pQTD2->QTDBuffPrt1=0x00000000;
	pQTD2->QTDBuffPrt2=0x00000000;
	pQTD2->QTDBuffPrt3=0x00000000;
	pQTD2->QTDBuffPrt4=0x00000000;
	InitialTransferLength = (pQTD2->QTDChar & 0x7FFF0000)>> 16;
	
	//Form QTD1 for Setup Stage of GetStatus===================================
	pQTD1 = Get_Free_QTD();
	PhyQTD1=virt_to_phy((u32_t)pQTD1);
	
	pQTD1->NextQTD=(ULONG)PhyQTD2;
	pQTD1->AltQTD=(ULONG)PhyQTD2;
//	pQTD1->QTDChar=0x00088e80;
//	pQTD1->QTDChar=0x00080e80;				// ioc is disabled
	pQTD1->QTDChar= (8*TOTAL_LENGTH) | C_ERR|PID_SETUP |ACTIVE;
	pQTD1->QTDBuffPrt0=(ULONG)PhyBuffer0;
	pQTD1->QTDBuffPrt1=0x00000000;
	pQTD1->QTDBuffPrt2=0x00000000;               
	pQTD1->QTDBuffPrt3=0x00000000;
	pQTD1->QTDBuffPrt4=0x00000000;
	
	//Form QHEAD======================================================
	pQHEAD1 = Get_Free_QH();
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);
	
	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
	if(UnderHub)
	{
		pQHEAD1->EpChar=(3*RL) |(C_Bit << 27)|(ehci_status.CepMps_Hub <<16) |H_BIT |DTC |(Device_Hub_Speed << 12) |(ehci_status.DevAddress_Hub);
		pQHEAD1->EpCap= MULT | (ehci_status.Portnum << 23)|(ehci_status.DevAddress << 16);
	}
	else
	{
		pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
		pQHEAD1->EpCap=0x40000000; 					// mult field is set to 1
	}
//	pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
//	pQHEAD1->EpCap=MULT;
	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;
	

	//Write the physical address into ASYNCICLISTADDR(ehci_base) Register
	writew(PhyQHEAD1,ASYNCICLISTADDR(ehci_base));
	
	//Enable Asynchronous list
	EHCI_Asynchronous_Enable(1);
	printf("the value of Asynclistbase addr register is %x\n",readw(ASYNCICLISTADDR(ehci_base)));
	while(!(TransferComplete | ErrorComplete))
	{
	}
	if(TransferComplete)
	{
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		printf("the value of qtd3 char is %x\n",pQTD3->QTDChar);
		if(Success(pQTD3->QTDChar)) 	// checking the status of the completion
		{
			printf("Get status Passed in Driver");
			BytesTransferred = InitialTransferLength - ((pQTD2->QTDChar & 0x7FFF0000)>> 16);
			printf("Total bytes transferred is %d\n",BytesTransferred);
			for(i=0;i<BytesTransferred;i++)
			{
				printf("the value of %d byte is %x\n",i,*(VirBuffAddr1+i));
			}
		}
		else
		{
			printf("some other error\n");
		}
	}
	else if(ErrorComplete)
	{
		printf("Transfer failed\n");
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		printf("the value of qtd3 char is %x\n",pQTD3->QTDChar);
	}
	printf("the value of pQHEAD1->QHEADLinkPtr is %x\n",pQHEAD1->QHEADLinkPtr);
	printf("the value of pQHEAD1->EpChar is %x\n",pQHEAD1->EpChar);
	printf("the value of pQHEAD1->EpCap is %x\n",pQHEAD1->EpCap);
	printf("the value of pQHEAD1->CurrQTD is %x\n",pQHEAD1->CurrQTD);
	printf("the value of pQHEAD1->NextQTD is %x\n",pQHEAD1->NextQTD);
	printf("the value of pQHEAD1->AltQTd is %x\n",pQHEAD1->AltQTd);
	printf("the value of pQHEAD1->QtdChar is %x\n",pQHEAD1->QTDChar);
	printf("the value of pQHEAD1->Buffptr1 is %x\n",pQHEAD1->QHEADBuffPtr0);
	printf("the value of pQHEAD1->Buffptr2 is %x\n",pQHEAD1->QHEADBuffPtr1);
	printf("the value of pQHEAD1->Buffptr3 is %x\n",pQHEAD1->QHEADBuffPtr2);
	printf("the value of pQHEAD1->Buffptr4 is %x\n",pQHEAD1->QHEADBuffPtr3);
	printf("the value of pQHEAD1->Buffptr5 is %x\n",pQHEAD1->QHEADBuffPtr4);
	printf("the value of pqtd1.NextQTD is %x\n",pQTD1->NextQTD);
	printf("the value of pQTD1->AltQTD is %x\n",pQTD1->AltQTD);
	printf("the value of pQTD1->QTDChar is %x\n",pQTD1->QTDChar);
	printf("the value of pQTD1->QTDBuffPrt0 is %x\n",pQTD1->QTDBuffPrt0);
	printf("the value of pQTD1->QTDBuffPrt1 is %x\n",pQTD1->QTDBuffPrt1);
	printf("the value of pQTD1->QTDBuffPrt2 is %x\n",pQTD1->QTDBuffPrt2);
	printf("the value of pQTD1->QTDBuffPrt3 is %x\n",pQTD1->QTDBuffPrt3);
	printf("the value of pQTD1->QTDBuffPrt4 is %x\n",pQTD1->QTDBuffPrt4);
	printf("the value of pqtd2.NextQTD is %x\n",pQTD2->NextQTD);
	printf("the value of pQTD2->AltQTD is %x\n",pQTD2->AltQTD);
	printf("the value of pQTD2->QTDChar is %x\n",pQTD2->QTDChar);
	printf("the value of pQTD2->QTDBuffPrt0 is %x\n",pQTD2->QTDBuffPrt0);
	printf("the value of pQTD2->QTDBuffPrt1 is %x\n",pQTD2->QTDBuffPrt1);
	printf("the value of pQTD2->QTDBuffPrt2 is %x\n",pQTD2->QTDBuffPrt2);
	printf("the value of pQTD2->QTDBuffPrt3 is %x\n",pQTD2->QTDBuffPrt3);
	printf("the value of pQTD2->QTDBuffPrt4 is %x\n",pQTD2->QTDBuffPrt4);
	printf("the value of pqtd3.NextQTD is %x\n",pQTD3->NextQTD);
	printf("the value of pQTD3->AltQTD is %x\n",pQTD3->AltQTD);
	printf("the value of pQTD3->QTDChar is %x\n",pQTD3->QTDChar);
	printf("the value of pQTD3->QTDBuffPrt0 is %x\n",pQTD3->QTDBuffPrt0);
	printf("the value of pQTD3->QTDBuffPrt1 is %x\n",pQTD3->QTDBuffPrt1);
	printf("the value of pQTD3->QTDBuffPrt2 is %x\n",pQTD3->QTDBuffPrt2);
	printf("the value of pQTD3->QTDBuffPrt3 is %x\n",pQTD3->QTDBuffPrt3);
	printf("the value of pQTD3->QTDBuffPrt4 is %x\n",pQTD3->QTDBuffPrt4);
	printf("the value of Asynclistbase addr register is %x\n",readw(ASYNCICLISTADDR(ehci_base)));
	TransferComplete = 0;
	ErrorComplete = 0;
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QTD_Ring(pQTD1);		
	Add_Free_QTD_Ring(pQTD2);
	Add_Free_QTD_Ring(pQTD3);
	return TRUE;
}

/* ******************************************************************************** *   
Function Name : int SendBulkData(int MPS,char EpNum,int Length)
Description   : This function initates a bulk transfer to the bulk out endpoint in the connected device.
Params        : MPS of the endpoint ,Enpdoint number ,Length to transfer 
Returns       : True:SendBulkData Success  / False: SendBulkData Fail    
Note          :  
******************************************************************************** */
int SendBulkData(int Mps,BYTE EpNum, int  Length)
{
	unsigned char NoPages;
	unsigned int 	i;
	PQHEAD 		pQHEAD1;
	PQTD 		pQTD1;
	PUCHAR 		OutDataBuffer;
	ULONG		PhyDataBufPointer[5];
	ULONG 		PhyQHEAD1,PhyQTD1;
	unsigned int	UnderHub;
	UnderHub = 0;
	printf("for a device under hub ?\n");
	printf("0.No\n");
	printf("1.Yes\n");
	scanf("%d",&UnderHub);
	
	//Disable Asynchronous list processing
	EHCI_Asynchronous_Enable(0);
	
	
	//Form QTD1 for Data Stage of SendBulkData================================
	OutDataBuffer = (PUCHAR)DATA_OUT_ADDR;
	for(i=0;i<Length;i++)
	{
		*(OutDataBuffer+i) = i;
	}
	PhyDataBufPointer[0] = virt_to_phy((u32_t)OutDataBuffer);
	NoPages = Length / 4096;
	for(i=1;i<NoPages;i++)
	{
		PhyDataBufPointer[i] =(ULONG) ((PUCHAR)PhyDataBufPointer[i-1] + 4096);
	}
	pQTD1 = Get_Free_QTD();
	PhyQTD1=virt_to_phy((u32_t)pQTD1);
	
	pQTD1->NextQTD=T_BIT;
	pQTD1->AltQTD=T_BIT;
	pQTD1->QTDChar=0x00000000 | (BulkOutTog << 31) |(Length << 16)|IOC|C_ERR|PID_OUT|ACTIVE;		// C_ERR is negated for infinite retry
	pQTD1->QTDBuffPrt0=PhyDataBufPointer[0];
	pQTD1->QTDBuffPrt1=PhyDataBufPointer[1];
	pQTD1->QTDBuffPrt2=PhyDataBufPointer[2];
	pQTD1->QTDBuffPrt3=PhyDataBufPointer[3];
	pQTD1->QTDBuffPrt4=PhyDataBufPointer[4];
	printf("the value of pqtd1.NextQTD is %x\n",pQTD1->NextQTD);
	printf("the value of pQTD1->AltQTD is %x\n",pQTD1->AltQTD);
	printf("the value of pQTD1->QTDChar is %x\n",pQTD1->QTDChar);
	printf("the value of pQTD1->QTDBuffPrt0 is %x\n",pQTD1->QTDBuffPrt0);
	printf("the value of pQTD1->QTDBuffPrt1 is %x\n",pQTD1->QTDBuffPrt1);
	printf("the value of pQTD1->QTDBuffPrt2 is %x\n",pQTD1->QTDBuffPrt2);
	printf("the value of pQTD1->QTDBuffPrt3 is %x\n",pQTD1->QTDBuffPrt3);
	printf("the value of pQTD1->QTDBuffPrt4 is %x\n",pQTD1->QTDBuffPrt4);
	//Form QHEAD=====================================================
	pQHEAD1 = Get_Free_QH();
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);
	
	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
	if(UnderHub)
	{
		pQHEAD1->EpChar=(3*RL) |(Mps <<16) |H_BIT |DTC |(Device_Hub_Speed << 12)|(EpNum << 8)|(ehci_status.DevAddress_Hub);
		pQHEAD1->EpCap= MULT | (ehci_status.Portnum << 23)|(ehci_status.DevAddress << 16);
	}
	else
	{
		pQHEAD1->EpChar=(3*RL) |(Mps <<16) |H_BIT |DTC |(EPS) |(EpNum << 8)|ehci_status.DevAddress;
		pQHEAD1->EpCap=0x000000000; 					// mult field is set to 1
	}
//	pQHEAD1->EpChar=(3*RL) |(Mps <<16) |H_BIT |DTC |(EPS) |(EpNum << 8)|ehci_status.DevAddress;
//	pQHEAD1->EpCap=0x000000000;
	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;
	printf("the value of pQHEAD1->QHEADLinkPtr is %x\n",pQHEAD1->QHEADLinkPtr);
	printf("the value of pQHEAD1->EpChar is %x\n",pQHEAD1->EpChar);
	printf("the value of pQHEAD1->EpCap is %x\n",pQHEAD1->EpCap);
	printf("the value of pQHEAD1->CurrQTD is %x\n",pQHEAD1->CurrQTD);
	printf("the value of pQHEAD1->NextQTD is %x\n",pQHEAD1->NextQTD);
	printf("the value of pQHEAD1->AltQTd is %x\n",pQHEAD1->AltQTd);

	printf("the value  of PhyQHEAD is %x\n",PhyQHEAD1);
	//Write the physical address into ASYNCICLISTADDR(ehci_base) Register
	
	writew((ULONG)PhyQHEAD1,ASYNCICLISTADDR(ehci_base)/*0x180a4138*/);
	
	printf("the value in Aynclistaddr is %x\n",readw(ASYNCICLISTADDR(ehci_base)));
	
	//Enable Asynchronous list
	EHCI_Asynchronous_Enable(1);		
	while(!(TransferComplete | ErrorComplete))
	{
	}
	if(TransferComplete)
	{
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		if(Success(pQTD1->QTDChar))		// checking the status of the completion
		{
			printf("Bulk Out in Successfull in Driver\n");
		}
		else
		{
			printf("some other error\n");
		}
	}
	else if(ErrorComplete)
	{
		printf("Transfer failed\n");
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
	}
	if(pQTD1->QTDChar & 0x80000000)
	{
		BulkOutTog = 0x1;
	}
	else
	{
		BulkOutTog = 0x0;
	}
	TransferComplete = 0;
	ErrorComplete = 0;
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QTD_Ring(pQTD1);		
	return TRUE;
}
/* ******************************************************************************** *   
Function Name : int ReceiveBulkData(int MPS,char EpNum,int Length)
Description   : This function initates a bulk transfer to the bulk in endpoint in the connected device.
Params        : MPS of the endpoint ,Endpoint number ,Length to transfer 
Returns       : True:ReceiveBulkData Success  / False: ReceiveBulkData Fail    
Note          :  
******************************************************************************** */
int ReceiveBulkData(int Mps,BYTE EpNum, int  Length)
{
	unsigned char NoPages;
	unsigned int 	i;
	PQHEAD 		pQHEAD1;
	PQTD 		pQTD1;
	PUCHAR 		InDataBuffer;
	ULONG		PhyDataBufPointer[5];
	ULONG 		PhyQHEAD1,PhyQTD1;
	unsigned int	UnderHub;
	UnderHub = 0;
	printf("for a device under hub ?\n");
	printf("0.No\n");
	printf("1.Yes\n");
	scanf("%d",&UnderHub);
	
	//Disable Asynchronous list processing
	EHCI_Asynchronous_Enable(0);
	
	
	//Form QTD1 for Data Stage of SendBulkData================================
	InDataBuffer = (PUCHAR)DATA_IN_ADDR;
	PhyDataBufPointer[0] = virt_to_phy((u32_t)InDataBuffer);
	NoPages = Length / 4096;
	for(i=1;i<NoPages;i++)
	{
		PhyDataBufPointer[i] =(ULONG) ((PUCHAR)PhyDataBufPointer[i-1] + 4096);
	}
	pQTD1 = Get_Free_QTD();
	PhyQTD1=virt_to_phy((u32_t)pQTD1);
	
	pQTD1->NextQTD=T_BIT;
	pQTD1->AltQTD=T_BIT;
	pQTD1->QTDChar=0x00000000 | (BulkInTog << 31) |(Length << 16)|IOC|C_ERR|PID_IN|ACTIVE;		// C_ERR is negated for infinite retry
	pQTD1->QTDBuffPrt0=PhyDataBufPointer[0];
	pQTD1->QTDBuffPrt1=PhyDataBufPointer[1];
	pQTD1->QTDBuffPrt2=PhyDataBufPointer[2];
	pQTD1->QTDBuffPrt3=PhyDataBufPointer[3];
	pQTD1->QTDBuffPrt4=PhyDataBufPointer[4];
	printf("the value of pqtd1.NextQTD is %x\n",pQTD1->NextQTD);
	printf("the value of pQTD1->AltQTD is %x\n",pQTD1->AltQTD);
	printf("the value of pQTD1->QTDChar is %x\n",pQTD1->QTDChar);
	printf("the value of pQTD1->QTDBuffPrt0 is %x\n",pQTD1->QTDBuffPrt0);
	printf("the value of pQTD1->QTDBuffPrt1 is %x\n",pQTD1->QTDBuffPrt1);
	printf("the value of pQTD1->QTDBuffPrt2 is %x\n",pQTD1->QTDBuffPrt2);
	printf("the value of pQTD1->QTDBuffPrt3 is %x\n",pQTD1->QTDBuffPrt3);
	printf("the value of pQTD1->QTDBuffPrt4 is %x\n",pQTD1->QTDBuffPrt4);
	//Form QHEAD=====================================================
	pQHEAD1 = Get_Free_QH();
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);
	
	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
	if(UnderHub)
	{
		pQHEAD1->EpChar=(3*RL) |(Mps <<16) |H_BIT |DTC |(Device_Hub_Speed << 12) |(EpNum << 8)|(ehci_status.DevAddress_Hub);
		pQHEAD1->EpCap= MULT | (ehci_status.Portnum << 23)|(ehci_status.DevAddress << 16);
	}
	else
	{
		pQHEAD1->EpChar=(3*RL) |(Mps <<16) |H_BIT |DTC |(EPS) |(EpNum << 8)|ehci_status.DevAddress;
		pQHEAD1->EpCap=0x000000000; 					// mult field is set to 1
	}
//	pQHEAD1->EpChar=(3*RL) |(Mps <<16) |H_BIT |DTC |(EPS) |(EpNum << 8)|ehci_status.DevAddress;
//	pQHEAD1->EpCap=0x000000000;
	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;
	printf("the value of pQHEAD1->QHEADLinkPtr is %x\n",pQHEAD1->QHEADLinkPtr);
	printf("the value of pQHEAD1->EpChar is %x\n",pQHEAD1->EpChar);
	printf("the value of pQHEAD1->EpCap is %x\n",pQHEAD1->EpCap);
	printf("the value of pQHEAD1->CurrQTD is %x\n",pQHEAD1->CurrQTD);
	printf("the value of pQHEAD1->NextQTD is %x\n",pQHEAD1->NextQTD);
	printf("the value of pQHEAD1->AltQTd is %x\n",pQHEAD1->AltQTd);

	printf("the value  of PhyQHEAD is %x\n",PhyQHEAD1);
	//Write the physical address into ASYNCICLISTADDR(ehci_base) Register
	
	writew((ULONG)PhyQHEAD1,ASYNCICLISTADDR(ehci_base)/*0x180a4138*/);
	
	printf("the value in Aynclistaddr is %x\n",readw(ASYNCICLISTADDR(ehci_base)));
	
	//Enable Asynchronous list
	EHCI_Asynchronous_Enable(1);		
	while(!(TransferComplete | ErrorComplete))
	{
	}
	if(TransferComplete)
	{
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		if(Success(pQTD1->QTDChar))		// checking the status of the completion
		{
			printf("Bulk In Successfull in Driver\n");
		}
		else
		{
			printf("some other error\n");
		}
	}
	else if(ErrorComplete)
	{
		printf("Transfer failed\n");
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
	}
	if(pQTD1->QTDChar & 0x80000000)
	{
		BulkInTog = 0x1;
	}
	else
	{
		BulkInTog = 0x0;
	}
	TransferComplete = 0;
	ErrorComplete = 0;
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QTD_Ring(pQTD1);		
	return TRUE;
}

/* ******************************************************************************** *   
Function Name : int BulkLoopData(int Mps,BYTE EpOutNum,BYTE EpInNum,int Length,PUCHAR DataOutBuf,PUCHAR DataInBuf,BYTE UnderHub)
Description   : This function initates a bulk transfer to both the bulk out and bulk in  endpoint in the connected bulk loopback device and verify the data received with that of the transmitted.
Params        : MPS of the endpoint ,In Endpoint number,In Endpoint Number,Length to transfer,Out data buffer address,IN data buffer address,whether performed under hub or direct device 
Returns       : True:BulkLoopData Success  / False: BulkLoopData Fail    
Note          :  
******************************************************************************** */
int BulkLoopData(int Mps,BYTE EpOutNum,BYTE EpInNum,int Length,PUCHAR DataOutBuf,PUCHAR DataInBuf,BYTE UnderHub)
{
	unsigned char NoPages;
	unsigned int 	i;
	PQHEAD 		pQHEAD1,pQHEAD2;
	PQTD 		pQTD1,pQTD2;
	PUCHAR 		OutDataBuffer;
	PUCHAR 		InDataBuffer;
	ULONG		PhyOutDataBufPointer[5];
	ULONG 		PhyQHEAD1,PhyQTD1;
	ULONG		PhyInDataBufPointer[5];
	ULONG 		PhyQHEAD2,PhyQTD2;
	int 	Error;
	Error = 0;
	//Disable Asynchronous list processing
	EHCI_Asynchronous_Enable(0);
	
	//Form QTD1 for Data Stage of SendBulkData================================
	OutDataBuffer = (PUCHAR)DataOutBuf;
	for(i=0;i<Length;i++)
	{
		*(OutDataBuffer+i) = i;
	}
	PhyOutDataBufPointer[0] = virt_to_phy((u32_t)OutDataBuffer);
	NoPages = Length / 4096;
	for(i=1;i<NoPages;i++)
	{
		PhyOutDataBufPointer[i] =(ULONG)((PUCHAR)PhyOutDataBufPointer[i-1] + 4096);
		socle_ehci_show("the value of PhyOutDataBufPointer[%d] is %x\n",i,PhyOutDataBufPointer[i]);
	}
	if(Length % 4096)
	{
		PhyOutDataBufPointer[i] =(ULONG)((PUCHAR)PhyOutDataBufPointer[i-1] + 4096);
	}
	pQTD1 = Get_Free_QTD();
	PhyQTD1=virt_to_phy((u32_t)pQTD1);
	
	pQTD1->NextQTD=T_BIT;
	pQTD1->AltQTD=T_BIT;
//	pQTD1->QTDChar=0x00000000 | (BulkOutTog << 31) |(Length << 16)|IOC|C_ERR|PID_OUT|ACTIVE;		// C_ERR is negated for infinite retry
	pQTD1->QTDChar=0x00000000 | (BulkOutTog << 31) |(Length << 16)|C_ERR|PID_OUT|ACTIVE;		// C_ERR is negated for infinite retry
	pQTD1->QTDBuffPrt0=PhyOutDataBufPointer[0];
	pQTD1->QTDBuffPrt1=PhyOutDataBufPointer[1];
	pQTD1->QTDBuffPrt2=PhyOutDataBufPointer[2];
	pQTD1->QTDBuffPrt3=PhyOutDataBufPointer[3];
	pQTD1->QTDBuffPrt4=PhyOutDataBufPointer[4];
	socle_ehci_show("the value of pqtd1.NextQTD is %x\n",pQTD1->NextQTD);
	socle_ehci_show("the value of pQTD1->AltQTD is %x\n",pQTD1->AltQTD);
	socle_ehci_show("the value of pQTD1->QTDChar is %x\n",pQTD1->QTDChar);
	socle_ehci_show("the value of pQTD1->QTDBuffPrt0 is %x\n",pQTD1->QTDBuffPrt0);
	socle_ehci_show("the value of pQTD1->QTDBuffPrt1 is %x\n",pQTD1->QTDBuffPrt1);
	socle_ehci_show("the value of pQTD1->QTDBuffPrt2 is %x\n",pQTD1->QTDBuffPrt2);
	socle_ehci_show("the value of pQTD1->QTDBuffPrt3 is %x\n",pQTD1->QTDBuffPrt3);
	socle_ehci_show("the value of pQTD1->QTDBuffPrt4 is %x\n",pQTD1->QTDBuffPrt4);
	//Form QHEAD=====================================================
	pQHEAD1 = Get_Free_QH();
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);
	
	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
	if(UnderHub)
	{
		pQHEAD1->EpChar=(Mps <<16) |H_BIT |DTC |(Device_Hub_Speed << 12) |(EpOutNum << 8)|(ehci_status.DevAddress_Hub);
		pQHEAD1->EpCap= MULT | (ehci_status.Portnum << 23)|(ehci_status.DevAddress << 16);
	}
	else
	{
		pQHEAD1->EpChar=(Mps <<16) |H_BIT |DTC |(EPS) |(EpOutNum << 8)|ehci_status.DevAddress;
		pQHEAD1->EpCap=0x000000000; 					// mult field is set to 1
	}
//	pQHEAD1->EpChar=(Mps <<16) |H_BIT |DTC |(EPS) |(EpOutNum << 8)|ehci_status.DevAddress;
//	pQHEAD1->EpCap=0x000000000;
	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;
	socle_ehci_show("the value of pQHEAD1->QHEADLinkPtr is %x\n",pQHEAD1->QHEADLinkPtr);
	socle_ehci_show("the value of pQHEAD1->EpChar is %x\n",pQHEAD1->EpChar);
	socle_ehci_show("the value of pQHEAD1->EpCap is %x\n",pQHEAD1->EpCap);
	socle_ehci_show("the value of pQHEAD1->CurrQTD is %x\n",pQHEAD1->CurrQTD);
	socle_ehci_show("the value of pQHEAD1->NextQTD is %x\n",pQHEAD1->NextQTD);
	socle_ehci_show("the value of pQHEAD1->AltQTd is %x\n",pQHEAD1->AltQTd);
	
	//Form QTD1 for Data Stage of SendBulkData================================
	InDataBuffer = (PUCHAR)DataInBuf;
	PhyInDataBufPointer[0] = virt_to_phy((u32_t)InDataBuffer);
	NoPages = Length / 4096;
	for(i=1;i<NoPages;i++)
	{
		PhyInDataBufPointer[i] =(ULONG)((PUCHAR)PhyInDataBufPointer[i-1] + 4096);
		socle_ehci_show("the value of PhyInDataBufPointer[%d] is %x\n",i,PhyInDataBufPointer[i]);
	}
	if(Length % 4096)
	{
		PhyInDataBufPointer[i] =(ULONG)((PUCHAR)PhyInDataBufPointer[i-1] + 4096);
	}
	pQTD2 = Get_Free_QTD();
	PhyQTD2=virt_to_phy((u32_t)pQTD2);
	
	pQTD2->NextQTD=T_BIT;
	pQTD2->AltQTD=T_BIT;
	pQTD2->QTDChar=0x00000000 | (BulkInTog << 31) |(Length << 16)|IOC|C_ERR|PID_IN|ACTIVE;		// C_ERR is negated for infinite retry
	pQTD2->QTDBuffPrt0=PhyInDataBufPointer[0];
	pQTD2->QTDBuffPrt1=PhyInDataBufPointer[1];
	pQTD2->QTDBuffPrt2=PhyInDataBufPointer[2];
	pQTD2->QTDBuffPrt3=PhyInDataBufPointer[3];
	pQTD2->QTDBuffPrt4=PhyInDataBufPointer[4];
	socle_ehci_show("the value of pqtd1.NextQTD is %x\n",pQTD2->NextQTD);
	socle_ehci_show("the value of pQTD1->AltQTD is %x\n",pQTD2->AltQTD);
	socle_ehci_show("the value of pQTD1->QTDChar is %x\n",pQTD2->QTDChar);
	socle_ehci_show("the value of pQTD1->QTDBuffPrt0 is %x\n",pQTD2->QTDBuffPrt0);
	socle_ehci_show("the value of pQTD1->QTDBuffPrt1 is %x\n",pQTD2->QTDBuffPrt1);
	socle_ehci_show("the value of pQTD1->QTDBuffPrt2 is %x\n",pQTD2->QTDBuffPrt2);
	socle_ehci_show("the value of pQTD1->QTDBuffPrt3 is %x\n",pQTD2->QTDBuffPrt3);
	socle_ehci_show("the value of pQTD1->QTDBuffPrt4 is %x\n",pQTD2->QTDBuffPrt4);
	//Form QHEAD=====================================================
	pQHEAD2 = Get_Free_QH();
	PhyQHEAD2=virt_to_phy((u32_t)pQHEAD2);
	
	pQHEAD2->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
	if(UnderHub)
	{
		pQHEAD2->EpChar=(Mps <<16) |DTC |(Device_Hub_Speed << 12) |(EpInNum << 8)|(ehci_status.DevAddress_Hub);
		pQHEAD2->EpCap= MULT | (ehci_status.Portnum << 23)|(ehci_status.DevAddress << 16);
	}
	else
	{
		pQHEAD2->EpChar=(Mps <<16) |DTC |(EPS) |(EpInNum << 8)|ehci_status.DevAddress;
		pQHEAD2->EpCap=0x000000000; 					// mult field is set to 1
	}
//	pQHEAD2->EpChar=(Mps <<16) |DTC |(EPS) |(EpInNum << 8)|ehci_status.DevAddress;
//	pQHEAD2->EpCap=0x000000000;
	pQHEAD2->CurrQTD=0x00000000;
	pQHEAD2->NextQTD=(ULONG)PhyQTD2;
	pQHEAD2->AltQTd=(ULONG)PhyQTD2;
	pQHEAD2->QTDChar=0x00000000;
	pQHEAD2->QHEADBuffPtr0=0x00000000;
	pQHEAD2->QHEADBuffPtr1=0x00000000;
	pQHEAD2->QHEADBuffPtr2=0x00000000;
	pQHEAD2->QHEADBuffPtr3=0x00000000;
	pQHEAD2->QHEADBuffPtr4=0x00000000;
	socle_ehci_show("the value of pQHEAD1->QHEADLinkPtr is %x\n",pQHEAD2->QHEADLinkPtr);
	socle_ehci_show("the value of pQHEAD1->EpChar is %x\n",pQHEAD2->EpChar);
	socle_ehci_show("the value of pQHEAD1->EpCap is %x\n",pQHEAD2->EpCap);
	socle_ehci_show("the value of pQHEAD1->CurrQTD is %x\n",pQHEAD2->CurrQTD);
	socle_ehci_show("the value of pQHEAD1->NextQTD is %x\n",pQHEAD2->NextQTD);
	socle_ehci_show("the value of pQHEAD1->AltQTd is %x\n",pQHEAD2->AltQTd);
	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD2)|0x2);
	socle_ehci_show("the value  of PhyQHEAD is %x\n",PhyQHEAD1);
	//Write the physical address into ASYNCICLISTADDR(ehci_base) Register
	
	writew((ULONG)PhyQHEAD1,ASYNCICLISTADDR(ehci_base));
	
	socle_ehci_show("the value in Aynclistaddr is %x\n",readw(ASYNCICLISTADDR(ehci_base)));
	
	//Enable Asynchronous list
	EHCI_Asynchronous_Enable(1);		
	while(!(TransferComplete | ErrorComplete))
	{
	}
	if(TransferComplete)
	{
		if(Success(pQTD1->QTDChar))		// checking the status of the completion
		{
			socle_ehci_show("the value of qtd1 char is %x\n",pQTD1->QTDChar);
			socle_ehci_show("Bulk Out  Successfull in Driver\n");
		}
		else
		{
			socle_ehci_show("some other error\n");
			Error = 1;
		}
		if(Success(pQTD2->QTDChar))		// checking the status of the completion
		{
			socle_ehci_show("the value of qtd2 char is %x\n",pQTD2->QTDChar);
			socle_ehci_show("Bulk In  Successfull in Driver\n");
		}
		else
		{
			socle_ehci_show("some other error\n");
			Error = 1;
		}
	}
	else if(ErrorComplete)
	{
		socle_ehci_show("Transfer failed\n");
		socle_ehci_show("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		socle_ehci_show("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		Error = 1;
	}
	if(pQTD1->QTDChar & 0x80000000)
	{
		BulkOutTog = 0x1;
	}
	else
	{
		BulkOutTog = 0x0;
	}
	if(pQTD2->QTDChar & 0x80000000)
	{
		BulkInTog = 0x1;
	}
	else
	{
		BulkInTog = 0x0;
	}
	TransferComplete = 0;
	ErrorComplete = 0;
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QH_Ring(pQHEAD2);
	Add_Free_QTD_Ring(pQTD1);		
	Add_Free_QTD_Ring(pQTD2);
	if(!Error)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/* ******************************************************************************** *   
Function Name : int IsoLoopData(int Mps,BYTE EpOutNum,BYTE EpInNum,int Length,PUCHAR DataOutBuf,PUCHAR DataInBuf)
Description   : This function initates a iso transfer to both the iso out and iso in  endpoint in the connected iso loopback device.
Params        : MPS of the endpoint ,In Endpoint number,In Endpoint Number,Length to transfer,Out data buffer address,IN data buffer address
Returns       : True:IsoLoopData Success  / False: IsoLoopData Fail    
Note          :  
******************************************************************************** */
//new Iso from Visa

int IsoLoopData(int Mps,BYTE EpOutNum,BYTE EpInNum,int Length,PUCHAR DataOutBuf,PUCHAR DataInBuf)
{
	unsigned char NoPages;
	unsigned int 	i;
	PiTD 		PiTD1,PiTD2;
       PULONG 		FramelistPointer; //api_new added
	PUCHAR 		OutDataBuffer;
	PUCHAR 		InDataBuffer;
	ULONG		PhyOutDataBufPointer[7];
	ULONG 		PhyiTD1;
	ULONG		PhyInDataBufPointer[7];
	ULONG 		PhyiTD2;
	int 	Error;
	Error = 0;
	//Disable Asynchronous list processing
	EHCI_Asynchronous_Enable(0);
	EHCI_Periodic_Enable(0); 

	
	OutDataBuffer = (PUCHAR)DataOutBuf;
	for(i=0;i<Length;i++)
	{
		*(OutDataBuffer+i) = i;
	}
	for(i=0;i<7;i++)
	{
		PhyOutDataBufPointer[i] = 0x00000000;
		PhyInDataBufPointer[i] = 0x00000000;
	}
	PhyOutDataBufPointer[0] = virt_to_phy((u32_t)OutDataBuffer);
	NoPages = Length / 4096;
	for(i=1;i<NoPages;i++)
	{
		PhyOutDataBufPointer[i] =(ULONG)((PUCHAR)PhyOutDataBufPointer[i-1] + 4096);
		printf("the value of PhyOutDataBufPointer[%d] is %x\n",i,PhyOutDataBufPointer[i]);
	}
	if(Length % 4096)
	{
		PhyOutDataBufPointer[i] =(ULONG)((PUCHAR)PhyOutDataBufPointer[i-1] + 4096);
	}
	PiTD1 = Get_Free_iTD();
	PhyiTD1=virt_to_phy((u32_t)PiTD1);
	
	PiTD1->iTDNextLinkPtr = T_BIT;
	PiTD1->iTDStatCon0 = 0x80000000 | (Length << 16);	// Active bit is set.
	PiTD1->iTDStatCon1 = 0x00000000;
	PiTD1->iTDStatCon2 = 0x00000000;
	PiTD1->iTDStatCon3 = 0x00000000;
	PiTD1->iTDStatCon4 = 0x00000000;
	PiTD1->iTDStatCon5 = 0x00000000;
	PiTD1->iTDStatCon6 = 0x00000000;
	PiTD1->iTDStatCon7 = 0x00000000;
	PiTD1->iTDBuffPtr0 = PhyOutDataBufPointer[0] |(EpOutNum<<8) |ehci_status.DevAddress;
	PiTD1->iTDBuffPtr1 = PhyOutDataBufPointer[1] | Mps;
	PiTD1->iTDBuffPtr2 = PhyOutDataBufPointer[2] | ISO_MULT;
	PiTD1->iTDBuffPtr3 = PhyOutDataBufPointer[3];
	PiTD1->iTDBuffPtr4 = PhyOutDataBufPointer[4];
	PiTD1->iTDBuffPtr5 = PhyOutDataBufPointer[5];
	PiTD1->iTDBuffPtr6 = PhyOutDataBufPointer[6];
	
	
	InDataBuffer = (PUCHAR)DataInBuf;
	PhyInDataBufPointer[0] = virt_to_phy((u32_t)InDataBuffer);
	NoPages = Length / 4096;
	for(i=1;i<NoPages;i++)
	{
		PhyInDataBufPointer[i] =(ULONG)((PUCHAR)PhyInDataBufPointer[i-1] + 4096);
		printf("the value of PhyInDataBufPointer[%d] is %x\n",i,PhyInDataBufPointer[i]);
	}
	if(Length % 4096)
	{
		PhyInDataBufPointer[i] =(ULONG)((PUCHAR)PhyInDataBufPointer[i-1] + 4096);
	}
	PiTD2 = Get_Free_iTD();
	PhyiTD2=virt_to_phy((u32_t)PiTD2);
	
	PiTD2->iTDNextLinkPtr = T_BIT;
	PiTD2->iTDStatCon0 = 0x00000000;
	PiTD2->iTDStatCon1 = 0x00000000;
	PiTD2->iTDStatCon2 = 0x00000000;
	PiTD2->iTDStatCon3 = 0x80000000 |(Length << 16) | IOC;
	PiTD2->iTDStatCon4 = 0x00000000;
	PiTD2->iTDStatCon5 = 0x00000000;
	PiTD2->iTDStatCon6 = 0x00000000;
	PiTD2->iTDStatCon7 = 0x00000000;
	PiTD2->iTDBuffPtr0 = PhyInDataBufPointer[0] |(EpInNum<<8) |ehci_status.DevAddress;
	PiTD2->iTDBuffPtr1 = PhyInDataBufPointer[1] |ISO_IN | Mps;
	PiTD2->iTDBuffPtr2 = PhyInDataBufPointer[2] | ISO_MULT;
	PiTD2->iTDBuffPtr3 = PhyInDataBufPointer[3];
	PiTD2->iTDBuffPtr4 = PhyInDataBufPointer[4];
	PiTD2->iTDBuffPtr5 = PhyInDataBufPointer[5];
	PiTD2->iTDBuffPtr6 = PhyInDataBufPointer[6];
	
	PiTD1->iTDNextLinkPtr =(ULONG) PhyiTD2 | ITD_TYPE;

	printf("the value of PiTD1->iTDNextLinkPtr is %x\n",PiTD1->iTDNextLinkPtr);
	printf("the value of PiTD1->iTDStatCon0 is %x\n",PiTD1->iTDStatCon0);
	printf("the value of PiTD1->iTDStatCon1 is %x\n",PiTD1->iTDStatCon1);
	printf("the value of PiTD1->iTDStatCon2 is %x\n",PiTD1->iTDStatCon2);
	printf("the value of PiTD1->iTDStatCon3 is %x\n",PiTD1->iTDStatCon3);
	printf("the value of PiTD1->iTDStatCon4 is %x\n",PiTD1->iTDStatCon4);
	printf("the value of PiTD1->iTDStatCon5 is %x\n",PiTD1->iTDStatCon5);
	printf("the value of PiTD1->iTDStatCon6 is %x\n",PiTD1->iTDStatCon6);
	printf("the value of PiTD1->iTDStatCon7 is %x\n",PiTD1->iTDStatCon7);
	printf("the value of PiTD1->iTDBuffPtr0 is %x\n",PiTD1->iTDBuffPtr0);
	printf("the value of PiTD1->iTDBuffPtr1 is %x\n",PiTD1->iTDBuffPtr1);
	printf("the value of PiTD1->iTDBuffPtr2 is %x\n",PiTD1->iTDBuffPtr2);
	printf("the value of PiTD1->iTDBuffPtr3 is %x\n",PiTD1->iTDBuffPtr3);
	printf("the value of PiTD1->iTDBuffPtr4 is %x\n",PiTD1->iTDBuffPtr4);
	printf("the value of PiTD1->iTDBuffPtr5 is %x\n",PiTD1->iTDBuffPtr5);
	printf("the value of PiTD1->iTDBuffPtr6 is %x\n",PiTD1->iTDBuffPtr6);
	printf("the value of PiTD2->iTDNextLinkPtr is %x\n",PiTD2->iTDNextLinkPtr);
	printf("the value of PiTD2->iTDStatCon0 is %x\n",PiTD2->iTDStatCon0);
	printf("the value of PiTD2->iTDStatCon1 is %x\n",PiTD2->iTDStatCon1);
	printf("the value of PiTD2->iTDStatCon2 is %x\n",PiTD2->iTDStatCon2);
	printf("the value of PiTD2->iTDStatCon3 is %x\n",PiTD2->iTDStatCon3);
	printf("the value of PiTD2->iTDStatCon4 is %x\n",PiTD2->iTDStatCon4);
	printf("the value of PiTD2->iTDStatCon5 is %x\n",PiTD2->iTDStatCon5);
	printf("the value of PiTD2->iTDStatCon6 is %x\n",PiTD2->iTDStatCon6);
	printf("the value of PiTD2->iTDStatCon7 is %x\n",PiTD2->iTDStatCon7);
	printf("the value of PiTD2->iTDBuffPtr0 is %x\n",PiTD2->iTDBuffPtr0);
	printf("the value of PiTD2->iTDBuffPtr1 is %x\n",PiTD2->iTDBuffPtr1);
	printf("the value of PiTD2->iTDBuffPtr2 is %x\n",PiTD2->iTDBuffPtr2);
	printf("the value of PiTD2->iTDBuffPtr3 is %x\n",PiTD2->iTDBuffPtr3);
	printf("the value of PiTD2->iTDBuffPtr4 is %x\n",PiTD2->iTDBuffPtr4);
	printf("the value of PiTD2->iTDBuffPtr5 is %x\n",PiTD2->iTDBuffPtr5);
	printf("the value of PiTD2->iTDBuffPtr6 is %x\n",PiTD2->iTDBuffPtr6);

	FramelistPointer = (PULONG)PERIOD_FRAMELIST_ADDR;
	for(i=0;i<1024;i++)
	{
		*(FramelistPointer+i) = PhyiTD1 | ITD_TYPE;

	}
	
	//Enable periodic list
	EHCI_Periodic_Enable(1);		
	while(!(TransferComplete | ErrorComplete))
	{
	}
	if(TransferComplete)
	{
		if((PiTD1->iTDStatCon0 & 0xF0000000) == 0)		// checking the status of the completion
		{
			printf("the value of itd1 is %x\n",PiTD1->iTDStatCon0);
			printf("Iso OUT  Successful in Driver\n");Error = 0;
		}
		else
		{
			printf("some other error in out transaction\n");
			Error = 1;
		}
		if((PiTD2->iTDStatCon3 & 0xF0000000) == 0)		// checking the status of the completion
		{
			printf("Iso IN  Successful in Driver\n");Error = 0;
		}
		else
		{
			printf("some other error in IN transaction\n");
			Error = 1;
		}
	}
	else if(ErrorComplete)
	{
		printf("Transfer failed");
		printf("\nthe value of qtd1 char is %x\n",PiTD1->iTDStatCon0);
		printf("the value of qtd2 char is %x\n",PiTD2->iTDStatCon3);
		Error = 1;
	}EHCI_Periodic_Enable(0);	//disable the periodic enable
	TransferComplete = 0;
	ErrorComplete = 0;
	Add_Free_iTD_Ring(PiTD1);		
	Add_Free_iTD_Ring(PiTD2);
	if(!Error)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	
}

//VendorRequest bctl1.2.0
int VendorRequest()
{
	PQHEAD pQHEAD1;
	PQTD pQTD1,pQTD2;
	ULONG PhyQHEAD1,PhyQTD1,PhyQTD2,PhyBuffer0;
	ULONG UsbIntr;
	UCHAR *VirBuffAddr;
	
	
	//Disable Asynchronous list processing
	EHCI_Asynchronous_Enable(0);
	
	VirBuffAddr=((UCHAR *)SETUP_BUFF_ADDR+32);
	PhyBuffer0=virt_to_phy((u32_t)(VirBuffAddr+0));
/*	
	//For the 8 bytes of setup stage
	*(VirBuffAddr+0)=0x0|(byte1);
	*(VirBuffAddr+1)=0x0|(byte2);
	*(VirBuffAddr+2)=0x0|(byte3);//Get Address from Application.
	*(VirBuffAddr+3)=0x0|(byte4);
	*(VirBuffAddr+4)=0x0|(byte5);
	*(VirBuffAddr+5)=0x0|((dx->EhcStat).byte6);
	*(VirBuffAddr+6)=0x0|((dx->EhcStat).byte7);
	*(VirBuffAddr+7)=0x0|((dx->EhcStat).byte8);
*/	

	*(VirBuffAddr+0)=0x40;
	*(VirBuffAddr+1)=0x25;                                     //dx->pFileLen
	*(VirBuffAddr+2)=0x0;
	*(VirBuffAddr+3)=0x0;
	*(VirBuffAddr+4)=0x0;
	*(VirBuffAddr+5)=0x0;
	*(VirBuffAddr+6)=0x0;
	*(VirBuffAddr+7)=0x0;

	//printf(("Initializing 1 QHEAD and 2 QTDs for VendorRequest"));
	
	//Form QHEAD=======================================================
	pQHEAD1 = Get_Free_QH();
	//pQHEAD1=(PQHEAD)(SETUP_BUFF_ADDR+0);
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);
	
	//Form QTD1 for Setup Stage of SetAddress
	pQTD1 = Get_Free_QTD();
	//pQTD1=(PQTD)(SETUP_BUFF_ADDR+16);			//+12
	PhyQTD1=virt_to_phy((u32_t)pQTD1);
	
	//Form QTD2 for Status Stage of SetAddress
	pQTD2 = Get_Free_QTD();
	//pQTD2=(PQTD)(SETUP_BUFF_ADDR+24);				//+20
	PhyQTD2=virt_to_phy((u32_t)pQTD2);
	
	//dx->DeviceAddress =0x0;
	//QHead1
	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
	pQHEAD1->EpChar=0x3040e000|(ehci_status.DevAddress);
	pQHEAD1->EpCap=0xc0000000;
	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;
	
	//Qtd1
	pQTD1->NextQTD=(ULONG)PhyQTD2;
	pQTD1->AltQTD=(ULONG)PhyQTD2;
	pQTD1->QTDChar=0x00088e80;
	pQTD1->QTDBuffPrt0=(ULONG)PhyBuffer0;
	pQTD1->QTDBuffPrt1=0x00000000;
	pQTD1->QTDBuffPrt2=0x00000000;
	pQTD1->QTDBuffPrt3=0x00000000;
	pQTD1->QTDBuffPrt4=0x00000000;
	
	//Qtd2
	pQTD2->NextQTD=0x00000001;
	pQTD2->AltQTD=0x00000001;
	pQTD2->QTDChar=0x80008d80;
	pQTD2->QTDBuffPrt0=0x00000000;
	pQTD2->QTDBuffPrt1=0x00000000;
	pQTD2->QTDBuffPrt2=0x00000000;
	pQTD2->QTDBuffPrt3=0x00000000;
	pQTD2->QTDBuffPrt4=0x00000000;
	
	printf(("Writing Physical Address of QHEAD into ASYNCLISTADDR"));
	//Write the physical address into ASYNCLISTADDR Register
	writew(PhyQHEAD1,ASYNCICLISTADDR(ehci_base));
	
	printf(("Disabling USB Transcation Complete Interrupt"));
	UsbIntr=readw(USBINTR(ehci_base));
	UsbIntr=UsbIntr&0xfffffffe;
	writew(UsbIntr,USBINTR(ehci_base));
	
	//Enable Asynchronous list
	EHCI_Asynchronous_Enable(1);
	
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QTD_Ring(pQTD1);			
	Add_Free_QTD_Ring(pQTD2);			


	printf(("After Calling Timer"));
	
	return TRUE;
}

int SendIntrData()
{//INTR_OUT_ADDR

	PQHEAD 		PIntrOutQHEAD;
	PQTD 		G1_Td_Addr,pG1_Td[No_Tds_In_Grp];		
	ULONG		PhyIntrOutQHEAD,PhyG1_Td[No_Tds_In_Grp];
	ULONG 		UsbIntr,UsbSts,UsbFrm1,UsbFrm,IntrOutPollRate;   // no_tds,
	ULONG 		IntrOutBuff1[No_Tds_In_Grp];
	ULONG 		PhyIntrOutBuff1[No_Tds_In_Grp];	
	int k,i;
	
	//clear any intrrupt status
	UsbSts=readw(USBSTS(ehci_base));
	writew(UsbSts,USBSTS(ehci_base));

	// Initialize all the locations of the Periodic Frame List with the termination value here
	for (k=0;k<1024;k++)
		*((UINT *)PERIOD_FRAMELIST_ADDR+k)=0x00000001;

	//Disable Asynchronous list processing
	EHCI_Asynchronous_Enable(0);

	if(!(readw(USBCMD(ehci_base)) & CMD_PSE))
	{
		//Enable Periodic list
		EHCI_Periodic_Enable(1);
	}

	//QTD-====================================================
	G1_Td_Addr=(PQTD)(SETUP_BUFF_ADDR+1632);    //1568+64 = 1632

	for(i=0;i<No_Tds_In_Grp;i++)
	{
		pG1_Td[i]=(PQTD)(G1_Td_Addr+(i*32));
		PhyG1_Td[i]=virt_to_phy((u32_t)pG1_Td[i]);
		
		IntrOutBuff1[i]=(INTR_IN_ADDR+(0x5000*i));    //
		PhyIntrOutBuff1[i]=virt_to_phy((u32_t)IntrOutBuff1[i]);   // PP
		
		pG1_Td[i]->NextQTD=(ULONG)PhyG1_Td[i+1]; 
		pG1_Td[i]->AltQTD=0x1;	//dummy
		pG1_Td[i]->QTDChar=0x80008f00 |(IOC | C_PAGE | C_ERR | PID_OUT |(20480 << 16));  //<<size
		pG1_Td[i]->QTDBuffPrt0=(ULONG)PhyIntrOutBuff1[i];
		pG1_Td[i]->QTDBuffPrt1=(ULONG)(PhyIntrOutBuff1[i]+4096);
		pG1_Td[i]->QTDBuffPrt2=(ULONG)(PhyIntrOutBuff1[i]+8192);
		pG1_Td[i]->QTDBuffPrt3=(ULONG)(PhyIntrOutBuff1[i]+12288);
		pG1_Td[i]->QTDBuffPrt4=(ULONG)(PhyIntrOutBuff1[i]+16384);
	}	
	

	//Form QHEAD
	//QH======================================================
	PIntrOutQHEAD=(PQHEAD)(SETUP_BUFF_ADDR+1568); 
	PhyIntrOutQHEAD=virt_to_phy((u32_t)PIntrOutQHEAD);
	
	PIntrOutQHEAD->CurrQTD=0;
	PIntrOutQHEAD->NextQTD=(ULONG)(PhyG1_Td[0]);	
	PIntrOutQHEAD->AltQTd=0;
	PIntrOutQHEAD->QTDChar=0;
	PIntrOutQHEAD->QHEADBuffPtr0=0x00000000;
	PIntrOutQHEAD->QHEADBuffPtr1=0x00000000;
	PIntrOutQHEAD->QHEADBuffPtr2=0x00000000;
	PIntrOutQHEAD->QHEADBuffPtr3=0x00000000;
	PIntrOutQHEAD->QHEADBuffPtr4=0x00000000;

	
	//Enable required  EHCI Interrupts here
	UsbIntr=readw(USBINTR(ehci_base));
	//UsbIntr=UsbIntr|0x00000017;
	UsbIntr=UsbIntr | 0x00000007;
	writew(UsbIntr,USBINTR(ehci_base));
	

	UsbFrm=readw(FRINDEX(ehci_base));
	UsbFrm=(UsbFrm & 0x1ff8) >>1;
   
	UsbFrm1=(UsbFrm/4);


//	*(dx->VirFrameList+UsbFrm1)=((ULONG)dx->PhyIntrInQHEAD|0x2);

	IntrOutPollRate=1;
	// Initialize all the locations of the Periodic Frame List with the termination value here
	for (k=0;k<1024;k+=IntrOutPollRate)
	{
		if(k<=1023)
		{
			*((UCHAR *)PERIOD_FRAMELIST_ADDR+k)=((ULONG)PhyIntrOutQHEAD|0x2);
		}
	}

	return TRUE;
}

/* ******************************************************************************** *   
Function Name : int ReceiveIntrData(int MPS,char EpNum,int Length)
Description   : This function initates a interrupt in transfer to a interrupt in endpoint to the connected device(HUB).
Params        : MPS of the endpoint ,Intr in endpoint number,Length to transfer 
Returns       : True:ReceiveIntrData Success  / False: ReceiveIntrData Fail    
Note          :  
******************************************************************************** */
int ReceiveIntrData(int Mps,BYTE EpNum,int Length)
{
	PULONG		FramelistPointer;
	unsigned char NoPages;
	unsigned int 	i;
	PQHEAD 		pQHEAD1;
	PQTD 		pQTD1;
	PUCHAR 		InDataBuffer;
	ULONG		PhyDataBufPointer[5];
	ULONG 		PhyQHEAD1,PhyQTD1;
	unsigned int	UnderHub;
	UnderHub = 0;
	printf("for a device under hub ?\n");
	printf("0.No\n");
	printf("1.Yes\n");
	scanf("%d",&UnderHub);
	
	//Disable Asynchronous list processing
	EHCI_Asynchronous_Enable(0);
	EHCI_Periodic_Enable(0); 
	FramelistPointer = (PULONG)PERIOD_FRAMELIST_ADDR;
	InDataBuffer = (PUCHAR)INTR_IN_ADDR;
	PhyDataBufPointer[0] = virt_to_phy((u32_t)InDataBuffer);
	NoPages = Length / 4096;
	for(i=1;i<NoPages;i++)
	{
		PhyDataBufPointer[i] =(ULONG) ((PUCHAR)PhyDataBufPointer[i-1] + 4096);
	}
	if(!(NoPages) && (Length < 4096))
	{
		PhyDataBufPointer[1] = 0x00000000;
		PhyDataBufPointer[2] = 0x00000000;
		PhyDataBufPointer[3] = 0x00000000;
		PhyDataBufPointer[4] = 0x00000000;
	}
	pQTD1 = Get_Free_QTD();
	PhyQTD1=virt_to_phy((u32_t)pQTD1);
	// for testing PhyQTD1 is given in ptd1->nextqtd and ptd1->altqtd
	pQTD1->NextQTD=PhyQTD1 |T_BIT;
	pQTD1->AltQTD=PhyQTD1 |T_BIT;
	pQTD1->QTDChar=0x00000000 | (IntrInTog << 31) |(Length << 16)|IOC|C_ERR|PID_IN|ACTIVE;		// C_ERR is negated for infinite retry
	pQTD1->QTDBuffPrt0=PhyDataBufPointer[0];
	pQTD1->QTDBuffPrt1=PhyDataBufPointer[1];
	pQTD1->QTDBuffPrt2=PhyDataBufPointer[2];
	pQTD1->QTDBuffPrt3=PhyDataBufPointer[3];
	pQTD1->QTDBuffPrt4=PhyDataBufPointer[4];
	printf("the value of pqtd1.NextQTD is %x\n",pQTD1->NextQTD);
	printf("the value of pQTD1->AltQTD is %x\n",pQTD1->AltQTD);
	printf("the value of pQTD1->QTDChar is %x\n",pQTD1->QTDChar);
	printf("the value of pQTD1->QTDBuffPrt0 is %x\n",pQTD1->QTDBuffPrt0);
	printf("the value of pQTD1->QTDBuffPrt1 is %x\n",pQTD1->QTDBuffPrt1);
	printf("the value of pQTD1->QTDBuffPrt2 is %x\n",pQTD1->QTDBuffPrt2);
	printf("the value of pQTD1->QTDBuffPrt3 is %x\n",pQTD1->QTDBuffPrt3);
	printf("the value of pQTD1->QTDBuffPrt4 is %x\n",pQTD1->QTDBuffPrt4);
	//Form QHEAD=====================================================
	pQHEAD1 = Get_Free_QH();
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);
	
//	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
	pQHEAD1->QHEADLinkPtr=PhyQHEAD1 |QH_TYPE |T_BIT;
	if(UnderHub)
	{
		pQHEAD1->EpChar=(3*RL) |(Mps <<16) |DTC |(Device_Hub_Speed << 12) |(EpNum << 8)|(ehci_status.DevAddress_Hub);
		pQHEAD1->EpCap= 0x00007004|(ehci_status.Portnum << 23)|(ehci_status.DevAddress << 16);	//C-mask - 0x70 and S-mask - 0x04
	}
	else
	{
		pQHEAD1->EpChar=(3*RL) |(Mps <<16) |DTC |(EPS) |(EpNum << 8)|ehci_status.DevAddress;
		pQHEAD1->EpCap=0x40000004; 					// mult field is set to 1 /Multi|C_Mask
	}
//	pQHEAD1->EpChar=(Mps <<16) |DTC |(EPS) |(EpNum << 8)|ehci_status.DevAddress;
//	pQHEAD1->EpCap=0x40000004;	// smask field is set to 02 so that the polling will be done in the 2 Uframe.
	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;
	printf("the value of pQHEAD1->QHEADLinkPtr is %x\n",pQHEAD1->QHEADLinkPtr);
	printf("the value of pQHEAD1->EpChar is %x\n",pQHEAD1->EpChar);
	printf("the value of pQHEAD1->EpCap is %x\n",pQHEAD1->EpCap);
	printf("the value of pQHEAD1->CurrQTD is %x\n",pQHEAD1->CurrQTD);
	printf("the value of pQHEAD1->NextQTD is %x\n",pQHEAD1->NextQTD);
	printf("the value of pQHEAD1->AltQTd is %x\n",pQHEAD1->AltQTd);

	printf("the value  of PhyQHEAD is %x\n",PhyQHEAD1);
	// write the pointer to corresponding position in the frame list array
	// for hub we use the polling interval is C and hence it is enough if we poll every 256 Uframe.
	// for debugging interrupt.
	for(i=0;i<1024;i++)
	{
		*(FramelistPointer+i) = PhyQHEAD1 | QH_TYPE;
	}
//	*FramelistPointer = PhyQHEAD1 | QH_TYPE;
//	*(FramelistPointer+256) = PhyQHEAD1| QH_TYPE;
//	*(FramelistPointer+512) = PhyQHEAD1| QH_TYPE;
//	*(FramelistPointer+768) = PhyQHEAD1| QH_TYPE;
	EHCI_Periodic_Enable(1);		
	while(!(TransferComplete | ErrorComplete))
	{
//		printf("FRINDEX(ehci_base) 				 :%x\n",readw(FRINDEX(ehci_base)));
//		printf("PERIODICLISTBASE(ehci_base)		 :%x\n",readw(PERIODICLISTBASE(ehci_base)));
//		printf("The value of usbsts register is %x\n",readw(USBSTS(ehci_base)));
	}
	if(TransferComplete)
	{
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		if(Success(pQTD1->QTDChar))		// checking the status of the completion
		{
			printf("Intr In Successfull in Driver\n");
		}
		else
		{
			printf("some other error\n");
		}
	}
	else if(ErrorComplete)
	{
		printf("Transfer failed\n");
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
	}
	if(pQTD1->QTDChar & 0x80000000)
	{
		IntrInTog = 0x1;
	}
	else
	{
		IntrInTog= 0x0;
	}
	TransferComplete = 0;
	ErrorComplete = 0;
	EHCI_Periodic_Enable(0);
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QTD_Ring(pQTD1);		
	return TRUE;
}
/* ******************************************************************************** *   
Function Name : int GetHubDescriptor(BYTE DevDescType,BYTE DevDescIndex,BYTE DevDescLength)
Description   : This function reads the hub descriptor form the connected hub.
Params        : Descriptor type,Descriptor index and Descriptor Length 
Returns       : True:GetHubDescriptor Success  / False: GetHubDescriptor Fail    
Note          :  
******************************************************************************** */
int GetHubDescriptor(BYTE DevDescType,BYTE DevDescIndex,int DevDescLength)
{
	unsigned int i;
	unsigned int BytesTransferred;
	unsigned int InitialTransferLength;
	PQHEAD pQHEAD1;
	PQTD pQTD1,pQTD2,pQTD3;
	ULONG PhyQHEAD1,PhyQTD1,PhyQTD2,PhyQTD3,PhyBuffer0,PhyBuffer1;
	UCHAR *VirBuffAddr0,*VirBuffAddr1;
	unsigned int	UnderHub;
	UnderHub = 0;
	printf("for a device under hub ?\n");
	printf("0.No\n");
	printf("1.Yes\n");
	scanf("%d",&UnderHub);

	//Disable Asynchronous list processing
	EHCI_Asynchronous_Enable(0);
	
	VirBuffAddr0=((UCHAR *)SETUP_BUFF_ADDR);						// for setup data
	VirBuffAddr1=(UCHAR *)((UCHAR *)SETUP_BUFF_ADDR + 0x20);		// for descriptor
	PhyBuffer0=virt_to_phy((u32_t)(VirBuffAddr0));
	PhyBuffer1=virt_to_phy((u32_t)(VirBuffAddr1));

	//For the 8 bytes of setup stage
	*(VirBuffAddr0+0)=0xA0;
	*(VirBuffAddr0+1)=USB_REQ_GET_DESCRIPTOR;
	*(VirBuffAddr0+2)=0x00|DevDescIndex;// CHANGE Descriptor Index Here
	*(VirBuffAddr0+3)=0x00|DevDescType;// CHANGE Descriptor Type Here
	*(VirBuffAddr0+4)=0x00;
	*(VirBuffAddr0+5)=0x00;
	*(VirBuffAddr0+6)=0x00|DevDescLength;// CHANGE Descriptor Length Here
	*(VirBuffAddr0+7)=0x00|DevDescLength>>8;
		
	
	//Form QTD3 for Status Stage of GetDescriptor==============================

	pQTD3 = Get_Free_QTD();
	PhyQTD3=virt_to_phy((u32_t)pQTD3);
	
	//Change the data toggle to 1
	pQTD3->NextQTD=T_BIT;
	pQTD3->AltQTD=T_BIT;
//	pQTD3->QTDChar=0x80008c80;
	pQTD3->QTDChar = DT |IOC |C_ERR|PID_OUT|ACTIVE; 
	pQTD3->QTDBuffPrt0=0x00000000;
	pQTD3->QTDBuffPrt1=0x00000000;
	pQTD3->QTDBuffPrt2=0x00000000;
	pQTD3->QTDBuffPrt3=0x00000000;
	pQTD3->QTDBuffPrt4=0x00000000;

	//Form QTD2 for Data Stage of GetDescriptor===============================
	pQTD2 = Get_Free_QTD();
	PhyQTD2=virt_to_phy((u32_t)pQTD2);
	
	pQTD2->NextQTD=(ULONG)PhyQTD3;
	pQTD2->AltQTD=(ULONG)PhyQTD3;
//	pQTD2->QTDChar=0x80008d80|(DevDescLength<<16);
//	pQTD2->QTDChar=0x80000d80|(DevDescLength<<16);		// disabling the ioc since only in last qtd i,e status stage
	pQTD2->QTDChar= DT |C_ERR|PID_IN|ACTIVE|(DevDescLength*TOTAL_LENGTH);		// disabling the ioc since only in last qtd i,e status stage
	pQTD2->QTDBuffPrt0=(ULONG)PhyBuffer1;
	pQTD2->QTDBuffPrt1=0x00000000;
	pQTD2->QTDBuffPrt2=0x00000000;
	pQTD2->QTDBuffPrt3=0x00000000;
	pQTD2->QTDBuffPrt4=0x00000000;
	InitialTransferLength = (pQTD2->QTDChar & 0x7FFF0000)>> 16;
	
	//Form QTD1 for Setup Stage of GetDescriptor==============================
	pQTD1 = Get_Free_QTD();
	PhyQTD1=virt_to_phy((u32_t)pQTD1);
	
	pQTD1->NextQTD=(ULONG)PhyQTD2;
	pQTD1->AltQTD=(ULONG)PhyQTD2;
//	pQTD1->QTDChar=0x00088e80;
//	pQTD1->QTDChar=0x00080e80;								// disabling the ioc only in last qtd
	pQTD1->QTDChar= (8*TOTAL_LENGTH) | C_ERR|PID_SETUP |ACTIVE;
	pQTD1->QTDBuffPrt0=(ULONG)PhyBuffer0;
	pQTD1->QTDBuffPrt1=0x00000000;
	pQTD1->QTDBuffPrt2=0x00000000;
	pQTD1->QTDBuffPrt3=0x00000000;
	pQTD1->QTDBuffPrt4=0x00000000;
	
	//Form QHEAD====================================================
	pQHEAD1 = Get_Free_QH();
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);
	
	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
	if(MpsKnown == 0)
	{
		pQHEAD1->EpChar=0x0040e000|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
	}
	else
	{
		pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
	}
	pQHEAD1->EpCap=0x40000000;						// mult field is set to 1
	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;
	
	//Write the physical address into ASYNCICLISTADDR(ehci_base) Register
	writew((ULONG)PhyQHEAD1,ASYNCICLISTADDR(ehci_base));
	
	EHCI_Asynchronous_Enable(1);
	//Enable Asynchronous list
	
	while(!(TransferComplete | ErrorComplete))
	{
	}
	if(TransferComplete)
	{
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		printf("the value of qtd3 char is %x\n",pQTD3->QTDChar);
		if(Success(pQTD3->QTDChar)) 	// checking the status of the completion
		{
			printf("Get Hub Descriptor Passed in Driver");
			if(DevDescType == 1)
			{
//				ehci_status.CepMps = *(Buff + 7);
				MpsKnown = 1;
//				printf("the value of control endpoint mps is %x\n",ohci_status.DevMPS);
			}
			BytesTransferred = InitialTransferLength - ((pQTD2->QTDChar & 0x7FFF0000)>> 16);
			printf("Total bytes transferred is %d\n",BytesTransferred);
			for(i=0;i<BytesTransferred;i++)
			{
				printf("the value of %d byte is %x\n",i,*(VirBuffAddr1+i));
			}
		}
		else
		{
			printf("some other error\n");
		}
	}
	else if(ErrorComplete)
	{
		printf("Transfer failed\n");
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		printf("the value of qtd3 char is %x\n",pQTD3->QTDChar);
	}
	TransferComplete = 0;
	ErrorComplete = 0;
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QTD_Ring(pQTD1);		
	Add_Free_QTD_Ring(pQTD2);
	Add_Free_QTD_Ring(pQTD3);
	return TRUE;
}

/* ******************************************************************************** *   
Function Name : int GetPortStatus(BYTE Portnum)
Description   : This function reads the status of the specified port of the connected hub.
Params        : Portnumber 
Returns       : True:GetPortStatus Success  / False: GetPortStatus Fail    
Note          :  
******************************************************************************** */
int GetPortStatus(BYTE Portnum)
{
	unsigned int i;
	unsigned int BytesTransferred;
	unsigned int InitialTransferLength;
	PQHEAD pQHEAD1;
	PQTD pQTD1,pQTD2,pQTD3;
	ULONG PhyQHEAD1,PhyQTD1,PhyQTD2,PhyQTD3,PhyBuffer0,PhyBuffer1;
	UCHAR *VirBuffAddr0,*VirBuffAddr1;
	unsigned int	UnderHub;
	UnderHub = 0;
	printf("for a device under hub ?\n");
	printf("0.No\n");
	printf("1.Yes\n");
	scanf("%d",&UnderHub);
	
	EHCI_Asynchronous_Enable(0);
	
	VirBuffAddr0=((UCHAR *)SETUP_BUFF_ADDR);
	VirBuffAddr1=(UCHAR *)((UCHAR *)SETUP_BUFF_ADDR+0x120);
	PhyBuffer0=virt_to_phy((u32_t)(VirBuffAddr0));
	PhyBuffer1=virt_to_phy((u32_t)(VirBuffAddr1));
	
	//For the 8 bytes of setup stage
	*(VirBuffAddr0+0)=0xA3;
	*(VirBuffAddr0+1)=USB_REQ_GET_STATUS;
	*(VirBuffAddr0+2)=0x00;
	*(VirBuffAddr0+3)=0x00;
	*(VirBuffAddr0+4)=0x00|Portnum;			// check whether this devdescindex is required
	*(VirBuffAddr0+5)=0x00;
	*(VirBuffAddr0+6)=0x04;
	*(VirBuffAddr0+7)=0x00;
	
	//printf(("Initializing 1 QHEAD and 3 QTDs for GetStatus"));
	//Form QTD3 for Status Stage of GetStatus=================================
	pQTD3 = Get_Free_QTD();
	PhyQTD3=virt_to_phy((u32_t)pQTD3);
	
	pQTD3->NextQTD=T_BIT;
	pQTD3->AltQTD=T_BIT;
//	pQTD3->QTDChar=0x80008c80;
	pQTD3->QTDChar = DT |IOC |C_ERR|PID_OUT|ACTIVE; 
	pQTD3->QTDBuffPrt0=0x00000000;
	pQTD3->QTDBuffPrt1=0x00000000;
	pQTD3->QTDBuffPrt2=0x00000000;
	pQTD3->QTDBuffPrt3=0x00000000;
	pQTD3->QTDBuffPrt4=0x00000000;
	
	//Form QTD2 for Data Stage of GetStatus=====================================
	pQTD2 = Get_Free_QTD();
	PhyQTD2=virt_to_phy((u32_t)pQTD2);

	pQTD2->NextQTD=(ULONG)PhyQTD3;
	pQTD2->AltQTD=(ULONG)PhyQTD3;
//	pQTD2->QTDChar=0x80028d80;
//	pQTD2->QTDChar=0x80040d80;				// ioc is disabled
	pQTD2->QTDChar= DT |C_ERR|PID_IN|ACTIVE|(0x04*TOTAL_LENGTH);	// since transfer length is 4 bytes ((0x04<<16))
	pQTD2->QTDBuffPrt0=(ULONG)PhyBuffer1;
	pQTD2->QTDBuffPrt1=0x00000000;
	pQTD2->QTDBuffPrt2=0x00000000;
	pQTD2->QTDBuffPrt3=0x00000000;
	pQTD2->QTDBuffPrt4=0x00000000;
	InitialTransferLength = (pQTD2->QTDChar & 0x7FFF0000)>> 16;
	
	//Form QTD1 for Setup Stage of GetStatus===================================
	pQTD1 = Get_Free_QTD();
	//pQTD1=(PQTD)(SETUP_BUFF_ADDR+1440);
	PhyQTD1=virt_to_phy((u32_t)pQTD1);
	
	pQTD1->NextQTD=(ULONG)PhyQTD2;
	pQTD1->AltQTD=(ULONG)PhyQTD2;
//	pQTD1->QTDChar=0x00088e80;
//	pQTD1->QTDChar=0x00080e80;				// ioc is disabled
	pQTD1->QTDChar= (8*TOTAL_LENGTH) | C_ERR|PID_SETUP |ACTIVE;
	pQTD1->QTDBuffPrt0=(ULONG)PhyBuffer0;
	pQTD1->QTDBuffPrt1=0x00000000;
	pQTD1->QTDBuffPrt2=0x00000000;               
	pQTD1->QTDBuffPrt3=0x00000000;
	pQTD1->QTDBuffPrt4=0x00000000;
	
	//Form QHEAD======================================================
	pQHEAD1 = Get_Free_QH();
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);
	
	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
	if(UnderHub)
	{
		pQHEAD1->EpChar=(3*RL) |(C_Bit << 27)|(ehci_status.CepMps_Hub <<16) |H_BIT |DTC |(Device_Hub_Speed << 12) |(ehci_status.DevAddress_Hub);
		pQHEAD1->EpCap= MULT | (ehci_status.Portnum << 23)|(ehci_status.DevAddress << 16);
	}
	else
	{
		pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
		pQHEAD1->EpCap=0x40000000; 					// mult field is set to 1
	}
//	pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
//	pQHEAD1->EpCap=MULT;
//	pQHEAD1->EpChar=0x3040e000|(ehci_status.DevAddress);
//	pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
//	pQHEAD1->EpCap=0xc0000000;
//	pQHEAD1->EpCap=0x40000000;										// mult value is made 1
	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;
	

	//Write the physical address into ASYNCICLISTADDR(ehci_base) Register
	writew(PhyQHEAD1,ASYNCICLISTADDR(ehci_base));
	
	//Enable Asynchronous list
	EHCI_Asynchronous_Enable(1);
	while(!(TransferComplete | ErrorComplete))
	{
	}
	if(TransferComplete)
	{
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		printf("the value of qtd3 char is %x\n",pQTD3->QTDChar);
		if(Success(pQTD3->QTDChar)) 	// checking the status of the completion
		{
			printf("Get Port Status Passed in Driver");
			BytesTransferred = InitialTransferLength - ((pQTD2->QTDChar & 0x7FFF0000)>> 16);
			printf("Total bytes transferred is %d\n",BytesTransferred);
			for(i=0;i<BytesTransferred;i++)
			{
				printf("the value of %d byte is %x\n",i,*(VirBuffAddr1+i));
			}
			if(*(VirBuffAddr1+2)&0x10)
			{
				printf("Reset change bit is set\n");
				switch(*(VirBuffAddr1+1)&0x06)
				{
					case 0:
						printf("Connected device is full speed device\n");
						Device_Hub_Speed = 0;
						C_Bit = 1;
						break;
					case 2:
						printf("Connected device is low speed device\n");
						Device_Hub_Speed = 1;
						C_Bit = 0;
						break;
					case 4:
						printf("Connected device is high speed device\n");
						Device_Hub_Speed = 2;
						C_Bit = 1;
						break;
					default:
						break;
				}
				if(!UnderHub)
				{
					ehci_status.DevAddress_Hub = 0;
					ehci_status.CepMps_Hub = 0x40;
				}
			}
		}
		else
		{
			printf("some other error\n");
		}
	}
	else if(ErrorComplete)
	{
		printf("Transfer failed\n");
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		printf("the value of qtd3 char is %x\n",pQTD3->QTDChar);
	}
	TransferComplete = 0;
	ErrorComplete = 0;
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QTD_Ring(pQTD1);		
	Add_Free_QTD_Ring(pQTD2);
	Add_Free_QTD_Ring(pQTD3);
	return TRUE;
}

/* ******************************************************************************** *   
Function Name : int GetHubStatus()
Description   : This function reads the status of the connected hub.
Params        :  
Returns       : True:GetHubStatus Success  / False: GetHubStatus Fail    
Note          :  
******************************************************************************** */
int GetHubStatus()
{
	unsigned int i;
	unsigned int BytesTransferred;
	unsigned int InitialTransferLength;
	PQHEAD pQHEAD1;
	PQTD pQTD1,pQTD2,pQTD3;
	ULONG PhyQHEAD1,PhyQTD1,PhyQTD2,PhyQTD3,PhyBuffer0,PhyBuffer1;
	UCHAR *VirBuffAddr0,*VirBuffAddr1;
	unsigned int	UnderHub;
	UnderHub = 0;
	printf("for a device under hub ?\n");
	printf("0.No\n");
	printf("1.Yes\n");
	scanf("%d",&UnderHub);
	
	EHCI_Asynchronous_Enable(0);
	
	VirBuffAddr0=((UCHAR *)SETUP_BUFF_ADDR);
	VirBuffAddr1=(UCHAR *)((UCHAR *)SETUP_BUFF_ADDR+0x120);
	PhyBuffer0=virt_to_phy((u32_t)(VirBuffAddr0));
	PhyBuffer1=virt_to_phy((u32_t)(VirBuffAddr1));
	
	//For the 8 bytes of setup stage
	*(VirBuffAddr0+0)=0xA0;
	*(VirBuffAddr0+1)=USB_REQ_GET_STATUS;
	*(VirBuffAddr0+2)=0x00;
	*(VirBuffAddr0+3)=0x00;
	*(VirBuffAddr0+4)=0x00;			// check whether this devdescindex is required
	*(VirBuffAddr0+5)=0x00;
	*(VirBuffAddr0+6)=0x04;
	*(VirBuffAddr0+7)=0x00;
	
	//Form QTD3 for Status Stage of GetStatus=================================
	pQTD3 = Get_Free_QTD();
	PhyQTD3=virt_to_phy((u32_t)pQTD3);
	
	pQTD3->NextQTD=T_BIT;
	pQTD3->AltQTD=T_BIT;
//	pQTD3->QTDChar=0x80008c80;
	pQTD3->QTDChar = DT |IOC |C_ERR|PID_OUT|ACTIVE; 
	pQTD3->QTDBuffPrt0=0x00000000;
	pQTD3->QTDBuffPrt1=0x00000000;
	pQTD3->QTDBuffPrt2=0x00000000;
	pQTD3->QTDBuffPrt3=0x00000000;
	pQTD3->QTDBuffPrt4=0x00000000;
	
	//Form QTD2 for Data Stage of GetStatus=====================================
	pQTD2 = Get_Free_QTD();
	PhyQTD2=virt_to_phy((u32_t)pQTD2);

	pQTD2->NextQTD=(ULONG)PhyQTD3;
	pQTD2->AltQTD=(ULONG)PhyQTD3;
//	pQTD2->QTDChar=0x80028d80;
//	pQTD2->QTDChar=0x80040d80;				// ioc is disabled
	pQTD2->QTDChar= DT |C_ERR|PID_IN|ACTIVE|(0x04*TOTAL_LENGTH);	// since transfer length is 4 bytes ((0x04<<16))
	pQTD2->QTDBuffPrt0=(ULONG)PhyBuffer1;
	pQTD2->QTDBuffPrt1=0x00000000;
	pQTD2->QTDBuffPrt2=0x00000000;
	pQTD2->QTDBuffPrt3=0x00000000;
	pQTD2->QTDBuffPrt4=0x00000000;
	InitialTransferLength = (pQTD2->QTDChar & 0x7FFF0000)>> 16;
	
	//Form QTD1 for Setup Stage of GetStatus===================================
	pQTD1 = Get_Free_QTD();
	PhyQTD1=virt_to_phy((u32_t)pQTD1);
	
	pQTD1->NextQTD=(ULONG)PhyQTD2;
	pQTD1->AltQTD=(ULONG)PhyQTD2;
//	pQTD1->QTDChar=0x00088e80;
//	pQTD1->QTDChar=0x00080e80;				// ioc is disabled
	pQTD1->QTDChar= (8*TOTAL_LENGTH) | C_ERR|PID_SETUP |ACTIVE;
	pQTD1->QTDBuffPrt0=(ULONG)PhyBuffer0;
	pQTD1->QTDBuffPrt1=0x00000000;
	pQTD1->QTDBuffPrt2=0x00000000;               
	pQTD1->QTDBuffPrt3=0x00000000;
	pQTD1->QTDBuffPrt4=0x00000000;
	
	//Form QHEAD======================================================
	pQHEAD1 = Get_Free_QH();
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);
	
	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
	if(UnderHub)
	{
		pQHEAD1->EpChar=(3*RL) |(C_Bit << 27)|(ehci_status.CepMps_Hub <<16) |H_BIT |DTC |(Device_Hub_Speed << 12) |(ehci_status.DevAddress_Hub);
		pQHEAD1->EpCap= MULT | (ehci_status.Portnum << 23)|(ehci_status.DevAddress << 16);
	}
	else
	{
		pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
		pQHEAD1->EpCap=0x40000000; 					// mult field is set to 1
	}
//	pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
//	pQHEAD1->EpCap=MULT;
	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;
	

	//Write the physical address into ASYNCICLISTADDR(ehci_base) Register
	writew(PhyQHEAD1,ASYNCICLISTADDR(ehci_base));
	
	//Enable Asynchronous list
	EHCI_Asynchronous_Enable(1);
	while(!(TransferComplete | ErrorComplete))
	{
	}
	if(TransferComplete)
	{
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		printf("the value of qtd3 char is %x\n",pQTD3->QTDChar);
		if(Success(pQTD3->QTDChar)) 	// checking the status of the completion
		{
			printf("Get Hub Status Passed in Driver");
			BytesTransferred = InitialTransferLength - ((pQTD2->QTDChar & 0x7FFF0000)>> 16);
			printf("Total bytes transferred is %d\n",BytesTransferred);
			for(i=0;i<BytesTransferred;i++)
			{
				printf("the value of %d byte is %x\n",i,*(VirBuffAddr1+i));
			}
		}
		else
		{
			printf("some other error\n");
		}
	}
	else if(ErrorComplete)
	{
		printf("Transfer failed\n");
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		printf("the value of qtd3 char is %x\n",pQTD3->QTDChar);
	}
	TransferComplete = 0;
	ErrorComplete = 0;
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QTD_Ring(pQTD1);		
	Add_Free_QTD_Ring(pQTD2);
	Add_Free_QTD_Ring(pQTD3);
	return TRUE;
}

/* ******************************************************************************** *   
Function Name : int ClearPortFeature(BYTE DevFeature, BYTE Portnum )
Description   : Clearing a certain feature of a specific port in the connected hub
Params        : Feature selector , port number
Returns       : True:ClearPortFeature Success  / False: ClearPortFeature Fail    
Note          :  
******************************************************************************** */
int ClearPortFeature(BYTE DevFeature, BYTE Portnum)
{
	PQHEAD pQHEAD1;
	PQTD pQTD1,pQTD2;
	ULONG PhyQHEAD1,PhyQTD1,PhyQTD2,PhyBuffer0;
	UCHAR *VirBuffAddr;
	unsigned int	UnderHub;
	UnderHub = 0;
	printf("for a device under hub ?\n");
	printf("0.No\n");
	printf("1.Yes\n");
	scanf("%d",&UnderHub);
	
	//Disable Asynchronous list processing
	EHCI_Asynchronous_Enable(0);
	
	VirBuffAddr=((UCHAR *)SETUP_BUFF_ADDR);
	PhyBuffer0=virt_to_phy((u32_t)(VirBuffAddr+0));
	
	//For the 8 bytes of setup stage===========================================
	*(VirBuffAddr+0)=0x23;
	*(VirBuffAddr+1)=USB_REQ_CLEAR_FEATURE;
	*(VirBuffAddr+2)=0x0|(DevFeature);
	*(VirBuffAddr+3)=0x00;
	*(VirBuffAddr+4)=0x00 |Portnum;			// check whether the devdescindex is required here.
	*(VirBuffAddr+5)=0x00;
	*(VirBuffAddr+6)=0x00;
	*(VirBuffAddr+7)=0x00;
	
	printf(("Initializing 1 QHEAD and 2 QTDs for ClearFeature"));
	
	//Form QTD2 for Status Stage of ClearFeature==================================
	pQTD2 = Get_Free_QTD();
	PhyQTD2=virt_to_phy((u32_t)pQTD2);
	
	pQTD2->NextQTD=T_BIT;
	pQTD2->AltQTD=T_BIT;
//	pQTD2->QTDChar=0x80008d80;
	pQTD2->QTDChar = DT |IOC |C_ERR|PID_IN|ACTIVE; 
	pQTD2->QTDBuffPrt0=0x00000000;
	pQTD2->QTDBuffPrt1=0x00000000;
	pQTD2->QTDBuffPrt2=0x00000000;
	pQTD2->QTDBuffPrt3=0x00000000;
	pQTD2->QTDBuffPrt4=0x00000000;

	//Form QTD1 for Setup Stage of ClearFeature===================================
	pQTD1 = Get_Free_QTD();
	PhyQTD1=virt_to_phy((u32_t)pQTD1);
	
	pQTD1->NextQTD=(ULONG)PhyQTD2;
	pQTD1->AltQTD=(ULONG)PhyQTD2;
//	pQTD1->QTDChar=0x00088e80;
//	pQTD1->QTDChar=0x00080e80;					// ioc is disabled and it is only enabled in status qtd
	pQTD1->QTDChar= (8*TOTAL_LENGTH) | C_ERR|PID_SETUP |ACTIVE;
	pQTD1->QTDBuffPrt0=(ULONG)PhyBuffer0;
	pQTD1->QTDBuffPrt1=0x00000000;
	pQTD1->QTDBuffPrt2=0x00000000;
	pQTD1->QTDBuffPrt3=0x00000000;
	pQTD1->QTDBuffPrt4=0x00000000;

	//Form QHEAD=====================================================
	pQHEAD1 = Get_Free_QH();
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);

	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
	if(UnderHub)
	{
		pQHEAD1->EpChar=(3*RL) |(C_Bit << 27)|(ehci_status.CepMps_Hub <<16) |H_BIT |DTC |(Device_Hub_Speed << 12) |(ehci_status.DevAddress_Hub);
		pQHEAD1->EpCap= MULT | (ehci_status.Portnum << 23)|(ehci_status.DevAddress << 16);
	}
	else
	{
		pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
		pQHEAD1->EpCap=0x40000000; 					// mult field is set to 1
	}
//	pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
//	pQHEAD1->EpCap=MULT;
	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;
	
	//Write the physical address into ASYNCICLISTADDR(ehci_base) Register
	writew(PhyQHEAD1,ASYNCICLISTADDR(ehci_base));
	
	//Enable Asynchronous list
	EHCI_Asynchronous_Enable(1);

	while(!(TransferComplete | ErrorComplete))
	{
	}
	if(TransferComplete)
	{
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		if(Success(pQTD2->QTDChar)) 	// checking the status of the completion
		{
			printf("Clear Feature Passed in Driver\n");
		}
		else
		{
			printf("some other error\n");
		}
	}
	else if(ErrorComplete)
	{
		printf("Transfer failed\n");
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
	}
	TransferComplete = 0;
	ErrorComplete = 0;
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QTD_Ring(pQTD1);		
	Add_Free_QTD_Ring(pQTD2);
	return TRUE;
}

/* ******************************************************************************** *   
Function Name : int ClearHubFeature(BYTE DevFeature)
Description   : Clearing a certain feature of the connected hub
Params        : Feature selector
Returns       : True:ClearHubFeature Success  / False: ClearHubFeature Fail    
Note          :  
******************************************************************************** */
int ClearHubFeature(BYTE DevFeature)
{
	PQHEAD pQHEAD1;
	PQTD pQTD1,pQTD2;
	ULONG PhyQHEAD1,PhyQTD1,PhyQTD2,PhyBuffer0;
	UCHAR *VirBuffAddr;
	unsigned int	UnderHub;
	UnderHub = 0;
	printf("for a device under hub ?\n");
	printf("0.No\n");
	printf("1.Yes\n");
	scanf("%d",&UnderHub);
	
	//Disable Asynchronous list processing
	EHCI_Asynchronous_Enable(0);
	
	VirBuffAddr=((UCHAR *)SETUP_BUFF_ADDR);
	PhyBuffer0=virt_to_phy((u32_t)(VirBuffAddr+0));
	
	//For the 8 bytes of setup stage===========================================
	*(VirBuffAddr+0)=0x20;
	*(VirBuffAddr+1)=USB_REQ_CLEAR_FEATURE;
	*(VirBuffAddr+2)=0x0|(DevFeature);
	*(VirBuffAddr+3)=0x00;
	*(VirBuffAddr+4)=0x00;			// check whether the devdescindex is required here.
	*(VirBuffAddr+5)=0x00;
	*(VirBuffAddr+6)=0x00;
	*(VirBuffAddr+7)=0x00;
	
	printf(("Initializing 1 QHEAD and 2 QTDs for ClearFeature"));
	
	//Form QTD2 for Status Stage of ClearFeature==================================
	pQTD2 = Get_Free_QTD();
	PhyQTD2=virt_to_phy((u32_t)pQTD2);
	
	pQTD2->NextQTD=T_BIT;
	pQTD2->AltQTD=T_BIT;
//	pQTD2->QTDChar=0x80008d80;
	pQTD2->QTDChar = DT |IOC |C_ERR|PID_IN|ACTIVE; 
	pQTD2->QTDBuffPrt0=0x00000000;
	pQTD2->QTDBuffPrt1=0x00000000;
	pQTD2->QTDBuffPrt2=0x00000000;
	pQTD2->QTDBuffPrt3=0x00000000;
	pQTD2->QTDBuffPrt4=0x00000000;

	//Form QTD1 for Setup Stage of ClearFeature===================================
	pQTD1 = Get_Free_QTD();
	PhyQTD1=virt_to_phy((u32_t)pQTD1);
	
	pQTD1->NextQTD=(ULONG)PhyQTD2;
	pQTD1->AltQTD=(ULONG)PhyQTD2;
//	pQTD1->QTDChar=0x00088e80;
//	pQTD1->QTDChar=0x00080e80;					// ioc is disabled and it is only enabled in status qtd
	pQTD1->QTDChar= (8*TOTAL_LENGTH) | C_ERR|PID_SETUP |ACTIVE;
	pQTD1->QTDBuffPrt0=(ULONG)PhyBuffer0;
	pQTD1->QTDBuffPrt1=0x00000000;
	pQTD1->QTDBuffPrt2=0x00000000;
	pQTD1->QTDBuffPrt3=0x00000000;
	pQTD1->QTDBuffPrt4=0x00000000;

	//Form QHEAD=====================================================
	pQHEAD1 = Get_Free_QH();
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);

	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
	if(UnderHub)
	{
		pQHEAD1->EpChar=(3*RL) |(C_Bit << 27)|(ehci_status.CepMps_Hub <<16) |H_BIT |DTC |(Device_Hub_Speed << 12) |(ehci_status.DevAddress_Hub);
		pQHEAD1->EpCap= MULT | (ehci_status.Portnum << 23)|(ehci_status.DevAddress << 16);
	}
	else
	{
		pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
		pQHEAD1->EpCap=0x40000000; 					// mult field is set to 1
	}
//	pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
//	pQHEAD1->EpCap=MULT;
	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;
	
	//Write the physical address into ASYNCICLISTADDR(ehci_base) Register
	writew(PhyQHEAD1,ASYNCICLISTADDR(ehci_base));
	
	//Enable Asynchronous list
	EHCI_Asynchronous_Enable(1);

	while(!(TransferComplete | ErrorComplete))
	{
	}
	if(TransferComplete)
	{
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		if(Success(pQTD2->QTDChar)) 	// checking the status of the completion
		{
			printf("Clear Feature Passed in Driver\n");
		}
		else
		{
			printf("some other error\n");
		}
	}
	else if(ErrorComplete)
	{
		printf("Transfer failed\n");
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
	}
	TransferComplete = 0;
	ErrorComplete = 0;
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QTD_Ring(pQTD1);		
	Add_Free_QTD_Ring(pQTD2);
	return TRUE;
}

/* ******************************************************************************** *   
Function Name : int SetHubFeature(BYTE DevFeature)
Description   : This function set certain features of the connected hub.
Params        : Feature selector 
Returns       : True:SetHubFeature Success  / False: SetHubFeature Fail    
Note          :  
******************************************************************************** */
int SetHubFeature(BYTE DevFeature)
{
	PQHEAD pQHEAD1;
	PQTD pQTD1,pQTD2;
	ULONG PhyQHEAD1,PhyQTD1,PhyQTD2,PhyBuffer0;
	UCHAR *VirBuffAddr;
	unsigned int	UnderHub;
	UnderHub = 0;
	printf("for a device under hub ?\n");
	printf("0.No\n");
	printf("1.Yes\n");
	scanf("%d",&UnderHub);
	
	//Disable Asynchronous list processing
	EHCI_Asynchronous_Enable(0);
	
	VirBuffAddr=((UCHAR *)SETUP_BUFF_ADDR);
	PhyBuffer0=virt_to_phy((u32_t)(VirBuffAddr+0));

	//For the 8 bytes of setup stage
	*(VirBuffAddr+0)=0x20;
	*(VirBuffAddr+1)=USB_REQ_SET_FEATURE;
	*(VirBuffAddr+2)=0x00|(DevFeature);
	*(VirBuffAddr+3)=0x00;
	*(VirBuffAddr+4)=0x00;
	*(VirBuffAddr+5)=0x00;
	*(VirBuffAddr+6)=0x00;
	*(VirBuffAddr+7)=0x00;
	
	//Form QTD2 for Status Stage of SetFeature===================================
	pQTD2 = Get_Free_QTD();
	PhyQTD2=virt_to_phy((u32_t)pQTD2);
	
	pQTD2->NextQTD=T_BIT;
	pQTD2->AltQTD=T_BIT;
//	pQTD2->QTDChar=0x80008d80;
	pQTD2->QTDChar = DT |IOC |C_ERR|PID_IN|ACTIVE; 
	pQTD2->QTDBuffPrt0=0x00000000;
	pQTD2->QTDBuffPrt1=0x00000000;
	pQTD2->QTDBuffPrt2=0x00000000;
	pQTD2->QTDBuffPrt3=0x00000000;
	pQTD2->QTDBuffPrt4=0x00000000;
	
	//Form QTD1 for Setup Stage of SetFeature===================================
	pQTD1 = Get_Free_QTD();
	//pQTD1=(PQTD)(SETUP_BUFF_ADDR+576);
	PhyQTD1=virt_to_phy((u32_t)pQTD1);

	pQTD1->NextQTD=(ULONG)PhyQTD2;
	pQTD1->AltQTD=(ULONG)PhyQTD2;
//	pQTD1->QTDChar=0x00088e80;
//	pQTD1->QTDChar=0x00080e80;					// ioc is disabled since it is only enabled for status stage.
	pQTD1->QTDChar= (8*TOTAL_LENGTH) | C_ERR|PID_SETUP |ACTIVE;
	pQTD1->QTDBuffPrt0=(ULONG)PhyBuffer0;
	pQTD1->QTDBuffPrt1=0x00000000;
	pQTD1->QTDBuffPrt2=0x00000000;
	pQTD1->QTDBuffPrt3=0x00000000;
	pQTD1->QTDBuffPrt4=0x00000000;

	//Form QHEAD================================================
	pQHEAD1 = Get_Free_QH();
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);
	
	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
	if(UnderHub)
	{
		pQHEAD1->EpChar=(3*RL) |(C_Bit << 27)|(ehci_status.CepMps_Hub <<16) |H_BIT |DTC |(Device_Hub_Speed << 12) |(ehci_status.DevAddress_Hub);
		pQHEAD1->EpCap= MULT | (ehci_status.Portnum << 23)|(ehci_status.DevAddress << 16);
	}
	else
	{
		pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
		pQHEAD1->EpCap=0x40000000; 					// mult field is set to 1
	}
//	pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
//	pQHEAD1->EpCap=MULT;
	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;
	
	printf(("Writing Physical Address of QHEAD into ASYNCICLISTADDR(ehci_base)"));
	//Write the physical address into ASYNCICLISTADDR(ehci_base) Register
	writew(PhyQHEAD1,ASYNCICLISTADDR(ehci_base));
	
	//Enable Asynchronous list
	EHCI_Asynchronous_Enable(1);
	
	while(!(TransferComplete | ErrorComplete))
	{
	}
	if(TransferComplete)
	{
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		if(Success(pQTD2->QTDChar)) 	// checking the status of the completion
		{
			printf("Set Feature Passed in Driver\n");
		}
		else
		{
			printf("some other error\n");
		}
	}
	else if(ErrorComplete)
	{
		printf("Transfer failed\n");
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
	}
	TransferComplete = 0;
	ErrorComplete = 0;
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QTD_Ring(pQTD1);		
	Add_Free_QTD_Ring(pQTD2);
	return TRUE;
}

/* ******************************************************************************** *   
Function Name : int SetPortFeature(BYTE DevFeature, BYTE Portnum )
Description   : This function set certain bits in the hub port register of the connected hub.
Params        : Feature selector 
Returns       : True:SetPortFeature Success  / False: SetPortFeature Fail    
Note          :  
******************************************************************************** */
int SetPortFeature(BYTE DevFeature, BYTE Portnum )
{
	PQHEAD pQHEAD1;
	PQTD pQTD1,pQTD2;
	ULONG PhyQHEAD1,PhyQTD1,PhyQTD2,PhyBuffer0;
	UCHAR *VirBuffAddr;
	unsigned int	UnderHub;
	UnderHub = 0;
	printf("for a device under hub ?\n");
	printf("0.No\n");
	printf("1.Yes\n");
	scanf("%d",&UnderHub);
	
	//Disable Asynchronous list processing
	EHCI_Asynchronous_Enable(0);
	
	VirBuffAddr=((UCHAR *)SETUP_BUFF_ADDR);
	PhyBuffer0=virt_to_phy((u32_t)(VirBuffAddr+0));

	//For the 8 bytes of setup stage
	*(VirBuffAddr+0)=0x23;
	*(VirBuffAddr+1)=USB_REQ_SET_FEATURE;
	*(VirBuffAddr+2)=0x00|(DevFeature);
	*(VirBuffAddr+3)=0x00;
	*(VirBuffAddr+4)=0x00 |Portnum;
	*(VirBuffAddr+5)=0x00;
	*(VirBuffAddr+6)=0x00;
	*(VirBuffAddr+7)=0x00;
	
	//Form QTD2 for Status Stage of SetFeature===================================
	pQTD2 = Get_Free_QTD();
	PhyQTD2=virt_to_phy((u32_t)pQTD2);
	
	pQTD2->NextQTD=T_BIT;
	pQTD2->AltQTD=T_BIT;
//	pQTD2->QTDChar=0x80008d80;
	pQTD2->QTDChar = DT |IOC |C_ERR|PID_IN|ACTIVE; 
	pQTD2->QTDBuffPrt0=0x00000000;
	pQTD2->QTDBuffPrt1=0x00000000;
	pQTD2->QTDBuffPrt2=0x00000000;
	pQTD2->QTDBuffPrt3=0x00000000;
	pQTD2->QTDBuffPrt4=0x00000000;
	
	//Form QTD1 for Setup Stage of SetFeature===================================
	pQTD1 = Get_Free_QTD();
	PhyQTD1=virt_to_phy((u32_t)pQTD1);

	pQTD1->NextQTD=(ULONG)PhyQTD2;
	pQTD1->AltQTD=(ULONG)PhyQTD2;
//	pQTD1->QTDChar=0x00088e80;
//	pQTD1->QTDChar=0x00080e80;					// ioc is disabled since it is only enabled for status stage.
	pQTD1->QTDChar= (8*TOTAL_LENGTH) | C_ERR|PID_SETUP |ACTIVE;
	pQTD1->QTDBuffPrt0=(ULONG)PhyBuffer0;
	pQTD1->QTDBuffPrt1=0x00000000;
	pQTD1->QTDBuffPrt2=0x00000000;
	pQTD1->QTDBuffPrt3=0x00000000;
	pQTD1->QTDBuffPrt4=0x00000000;

	//Form QHEAD================================================
	pQHEAD1 = Get_Free_QH();
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);
	
	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
	if(UnderHub)
	{
		pQHEAD1->EpChar=(3*RL) |(C_Bit << 27)|(ehci_status.CepMps_Hub <<16) |H_BIT |DTC |(Device_Hub_Speed << 12) |(ehci_status.DevAddress_Hub);
		pQHEAD1->EpCap= MULT | (ehci_status.Portnum << 23)|(ehci_status.DevAddress << 16);
	}
	else
	{
		pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
		pQHEAD1->EpCap=0x40000000; 					// mult field is set to 1
	}
//	pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
//	pQHEAD1->EpCap=MULT;
	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;
	
	printf(("Writing Physical Address of QHEAD into ASYNCICLISTADDR(ehci_base)"));
	//Write the physical address into ASYNCICLISTADDR(ehci_base) Register
	writew(PhyQHEAD1,ASYNCICLISTADDR(ehci_base));
	
	//Enable Asynchronous list
	EHCI_Asynchronous_Enable(1);
	
	while(!(TransferComplete | ErrorComplete))
	{
	}
	if(TransferComplete)
	{
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		if(Success(pQTD2->QTDChar)) 	// checking the status of the completion
		{
			printf("Set Feature Passed in Driver\n");
		}
		else
		{
			printf("some other error\n");
		}
	}
	else if(ErrorComplete)
	{
		printf("Transfer failed\n");
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
	}
	TransferComplete = 0;
	ErrorComplete = 0;
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QTD_Ring(pQTD1);		
	Add_Free_QTD_Ring(pQTD2);
	return TRUE;
}


/* ******************************************************************************** *   
Function Name : int ClearTTBuffer(unsigned short EpChar, BYTE TTPort)
Description   : Clearing the TT buffer in the connected hub
Params        : Endpoint charachteristics,TTport number
Returns       : True:ClearTTBuffer Success  / False: ClearTTBuffer Fail    
Note          :  
******************************************************************************** */
int ClearTTBuffer(unsigned short EpChar, BYTE TTPort)
{
	PQHEAD pQHEAD1;
	PQTD pQTD1,pQTD2;
	ULONG PhyQHEAD1,PhyQTD1,PhyQTD2,PhyBuffer0;
	UCHAR *VirBuffAddr;
	unsigned int	UnderHub;
	UnderHub = 0;
	printf("for a device under hub ?\n");
	printf("0.No\n");
	printf("1.Yes\n");
	scanf("%d",&UnderHub);
	
	//Disable Asynchronous list processing
	EHCI_Asynchronous_Enable(0);
	
	VirBuffAddr=((UCHAR *)SETUP_BUFF_ADDR);
	PhyBuffer0=virt_to_phy((u32_t)(VirBuffAddr+0));
	
	//For the 8 bytes of setup stage===========================================
	*(VirBuffAddr+0)=0x23;
	*(VirBuffAddr+1)=HUB_CLASS_CLEAR_TT_BUFFER;
	*(VirBuffAddr+2)=EpChar;
	*(VirBuffAddr+3)=EpChar >> 8;
	*(VirBuffAddr+4)=0x00 |TTPort;			// check whether the devdescindex is required here.
	*(VirBuffAddr+5)=0x00;
	*(VirBuffAddr+6)=0x00;
	*(VirBuffAddr+7)=0x00;
	
	printf(("Initializing 1 QHEAD and 2 QTDs for ClearFeature"));
	
	//Form QTD2 for Status Stage of ClearFeature==================================
	pQTD2 = Get_Free_QTD();
	PhyQTD2=virt_to_phy((u32_t)pQTD2);
	
	pQTD2->NextQTD=T_BIT;
	pQTD2->AltQTD=T_BIT;
//	pQTD2->QTDChar=0x80008d80;
	pQTD2->QTDChar = DT |IOC |C_ERR|PID_IN|ACTIVE; 
	pQTD2->QTDBuffPrt0=0x00000000;
	pQTD2->QTDBuffPrt1=0x00000000;
	pQTD2->QTDBuffPrt2=0x00000000;
	pQTD2->QTDBuffPrt3=0x00000000;
	pQTD2->QTDBuffPrt4=0x00000000;

	//Form QTD1 for Setup Stage of ClearFeature===================================
	pQTD1 = Get_Free_QTD();
	PhyQTD1=virt_to_phy((u32_t)pQTD1);
	
	pQTD1->NextQTD=(ULONG)PhyQTD2;
	pQTD1->AltQTD=(ULONG)PhyQTD2;
//	pQTD1->QTDChar=0x00088e80;
//	pQTD1->QTDChar=0x00080e80;					// ioc is disabled and it is only enabled in status qtd
	pQTD1->QTDChar= (8*TOTAL_LENGTH) | C_ERR|PID_SETUP |ACTIVE;
	pQTD1->QTDBuffPrt0=(ULONG)PhyBuffer0;
	pQTD1->QTDBuffPrt1=0x00000000;
	pQTD1->QTDBuffPrt2=0x00000000;
	pQTD1->QTDBuffPrt3=0x00000000;
	pQTD1->QTDBuffPrt4=0x00000000;

	//Form QHEAD=====================================================
	pQHEAD1 = Get_Free_QH();
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);

	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
	if(UnderHub)
	{
		pQHEAD1->EpChar=(3*RL) |(C_Bit << 27)|(ehci_status.CepMps_Hub <<16) |H_BIT |DTC |(Device_Hub_Speed << 12) |(ehci_status.DevAddress_Hub);
		pQHEAD1->EpCap= MULT | (ehci_status.Portnum << 23)|(ehci_status.DevAddress << 16);
	}
	else
	{
		pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
		pQHEAD1->EpCap=0x40000000; 					// mult field is set to 1
	}
//	pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
//	pQHEAD1->EpCap=MULT;
	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;
	
	//Write the physical address into ASYNCICLISTADDR(ehci_base) Register
	writew(PhyQHEAD1,ASYNCICLISTADDR(ehci_base));
	
	//Enable Asynchronous list
	EHCI_Asynchronous_Enable(1);

	while(!(TransferComplete | ErrorComplete))
	{
	}
	if(TransferComplete)
	{
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		if(Success(pQTD2->QTDChar)) 	// checking the status of the completion
		{
			printf("Clear Feature Passed in Driver\n");
		}
		else
		{
			printf("some other error\n");
		}
	}
	else if(ErrorComplete)
	{
		printf("Transfer failed\n");
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
	}
	TransferComplete = 0;
	ErrorComplete = 0;
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QTD_Ring(pQTD1);		
	Add_Free_QTD_Ring(pQTD2);
	return TRUE;
}

/* ******************************************************************************** *   
Function Name : int ResetTT(BYTE TTPort)
Description   : Reseting the TT in the connected hub
Params        : TTport number
Returns       : True:ResetTT Success  / False: ResetTT Fail    
Note          :  
******************************************************************************** */
int ResetTT(BYTE TTPort)
{
	PQHEAD pQHEAD1;
	PQTD pQTD1,pQTD2;
	ULONG PhyQHEAD1,PhyQTD1,PhyQTD2,PhyBuffer0;
	UCHAR *VirBuffAddr;
	unsigned int	UnderHub;
	UnderHub = 0;
	printf("for a device under hub ?\n");
	printf("0.No\n");
	printf("1.Yes\n");
	scanf("%d",&UnderHub);
	
	//Disable Asynchronous list processing
	EHCI_Asynchronous_Enable(0);
	
	VirBuffAddr=((UCHAR *)SETUP_BUFF_ADDR);
	PhyBuffer0=virt_to_phy((u32_t)(VirBuffAddr+0));
	
	//For the 8 bytes of setup stage===========================================
	*(VirBuffAddr+0)=0x23;
	*(VirBuffAddr+1)=HUB_CLASS_RESET_TT;
	*(VirBuffAddr+2)=0x00;
	*(VirBuffAddr+3)=0x00;
	*(VirBuffAddr+4)=0x00 |TTPort;			// check whether the devdescindex is required here.
	*(VirBuffAddr+5)=0x00;
	*(VirBuffAddr+6)=0x00;
	*(VirBuffAddr+7)=0x00;
	
	printf(("Initializing 1 QHEAD and 2 QTDs for ClearFeature"));
	
	//Form QTD2 for Status Stage of ClearFeature==================================
	pQTD2 = Get_Free_QTD();
	PhyQTD2=virt_to_phy((u32_t)pQTD2);
	
	pQTD2->NextQTD=T_BIT;
	pQTD2->AltQTD=T_BIT;
//	pQTD2->QTDChar=0x80008d80;
	pQTD2->QTDChar = DT |IOC |C_ERR|PID_IN|ACTIVE; 
	pQTD2->QTDBuffPrt0=0x00000000;
	pQTD2->QTDBuffPrt1=0x00000000;
	pQTD2->QTDBuffPrt2=0x00000000;
	pQTD2->QTDBuffPrt3=0x00000000;
	pQTD2->QTDBuffPrt4=0x00000000;

	//Form QTD1 for Setup Stage of ClearFeature===================================
	pQTD1 = Get_Free_QTD();
	PhyQTD1=virt_to_phy((u32_t)pQTD1);
	
	pQTD1->NextQTD=(ULONG)PhyQTD2;
	pQTD1->AltQTD=(ULONG)PhyQTD2;
//	pQTD1->QTDChar=0x00088e80;
//	pQTD1->QTDChar=0x00080e80;					// ioc is disabled and it is only enabled in status qtd
	pQTD1->QTDChar= (8*TOTAL_LENGTH) | C_ERR|PID_SETUP |ACTIVE;
	pQTD1->QTDBuffPrt0=(ULONG)PhyBuffer0;
	pQTD1->QTDBuffPrt1=0x00000000;
	pQTD1->QTDBuffPrt2=0x00000000;
	pQTD1->QTDBuffPrt3=0x00000000;
	pQTD1->QTDBuffPrt4=0x00000000;

	//Form QHEAD=====================================================
	pQHEAD1 = Get_Free_QH();
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);

	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
	if(UnderHub)
	{
		pQHEAD1->EpChar=(3*RL) |(C_Bit << 27)|(ehci_status.CepMps_Hub <<16) |H_BIT |DTC |(Device_Hub_Speed << 12) |(ehci_status.DevAddress_Hub);
		pQHEAD1->EpCap= MULT | (ehci_status.Portnum << 23)|(ehci_status.DevAddress << 16);
	}
	else
	{
		pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
		pQHEAD1->EpCap=0x40000000; 					// mult field is set to 1
	}
//	pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
//	pQHEAD1->EpCap=MULT;
	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;
	
	//Write the physical address into ASYNCICLISTADDR(ehci_base) Register
	writew(PhyQHEAD1,ASYNCICLISTADDR(ehci_base));
	
	//Enable Asynchronous list
	EHCI_Asynchronous_Enable(1);

	while(!(TransferComplete | ErrorComplete))
	{
	}
	if(TransferComplete)
	{
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		if(Success(pQTD2->QTDChar)) 	// checking the status of the completion
		{
			printf("Clear Feature Passed in Driver\n");
		}
		else
		{
			printf("some other error\n");
		}
	}
	else if(ErrorComplete)
	{
		printf("Transfer failed\n");
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
	}
	TransferComplete = 0;
	ErrorComplete = 0;
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QTD_Ring(pQTD1);		
	Add_Free_QTD_Ring(pQTD2);
	return TRUE;
}


/* ******************************************************************************** *   
Function Name : int StopTT(BYTE TTPort)
Description   : Stoping the TT in the connected hub
Params        : TTport number
Returns       : True:StopTT Success  / False: StopTT Fail    
Note          :  
******************************************************************************** */
int StopTT(BYTE TTPort)
{
	PQHEAD pQHEAD1;
	PQTD pQTD1,pQTD2;
	ULONG PhyQHEAD1,PhyQTD1,PhyQTD2,PhyBuffer0;
	UCHAR *VirBuffAddr;
	unsigned int	UnderHub;
	UnderHub = 0;
	printf("for a device under hub ?\n");
	printf("0.No\n");
	printf("1.Yes\n");
	scanf("%d",&UnderHub);
	
	//Disable Asynchronous list processing
	EHCI_Asynchronous_Enable(0);
	
	VirBuffAddr=((UCHAR *)SETUP_BUFF_ADDR);
	PhyBuffer0=virt_to_phy((u32_t)(VirBuffAddr+0));
	
	//For the 8 bytes of setup stage===========================================
	*(VirBuffAddr+0)=0x23;
	*(VirBuffAddr+1)=HUB_CLASS_STOP_TT;
	*(VirBuffAddr+2)=0x00;
	*(VirBuffAddr+3)=0x00;
	*(VirBuffAddr+4)=0x00 |TTPort;			// check whether the devdescindex is required here.
	*(VirBuffAddr+5)=0x00;
	*(VirBuffAddr+6)=0x00;
	*(VirBuffAddr+7)=0x00;
	
	printf(("Initializing 1 QHEAD and 2 QTDs for StopTT"));
	
	//Form QTD2 for Status Stage of ClearFeature==================================
	pQTD2 = Get_Free_QTD();
	PhyQTD2=virt_to_phy((u32_t)pQTD2);
	
	pQTD2->NextQTD=T_BIT;
	pQTD2->AltQTD=T_BIT;
//	pQTD2->QTDChar=0x80008d80;
	pQTD2->QTDChar = DT |IOC |C_ERR|PID_IN|ACTIVE; 
	pQTD2->QTDBuffPrt0=0x00000000;
	pQTD2->QTDBuffPrt1=0x00000000;
	pQTD2->QTDBuffPrt2=0x00000000;
	pQTD2->QTDBuffPrt3=0x00000000;
	pQTD2->QTDBuffPrt4=0x00000000;

	//Form QTD1 for Setup Stage of ClearFeature===================================
	pQTD1 = Get_Free_QTD();
	PhyQTD1=virt_to_phy((u32_t)pQTD1);
	
	pQTD1->NextQTD=(ULONG)PhyQTD2;
	pQTD1->AltQTD=(ULONG)PhyQTD2;
//	pQTD1->QTDChar=0x00088e80;
//	pQTD1->QTDChar=0x00080e80;					// ioc is disabled and it is only enabled in status qtd
	pQTD1->QTDChar= (8*TOTAL_LENGTH) | C_ERR|PID_SETUP |ACTIVE;
	pQTD1->QTDBuffPrt0=(ULONG)PhyBuffer0;
	pQTD1->QTDBuffPrt1=0x00000000;
	pQTD1->QTDBuffPrt2=0x00000000;
	pQTD1->QTDBuffPrt3=0x00000000;
	pQTD1->QTDBuffPrt4=0x00000000;

	//Form QHEAD=====================================================
	pQHEAD1 = Get_Free_QH();
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);

	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
	if(UnderHub)
	{
		pQHEAD1->EpChar=(3*RL) |(C_Bit << 27)|(ehci_status.CepMps_Hub <<16) |H_BIT |DTC |(Device_Hub_Speed << 12) |(ehci_status.DevAddress_Hub);
		pQHEAD1->EpCap= MULT | (ehci_status.Portnum << 23)|(ehci_status.DevAddress << 16);
	}
	else
	{
		pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
		pQHEAD1->EpCap=0x40000000; 					// mult field is set to 1
	}
//	pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
//	pQHEAD1->EpCap=MULT;
	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;
	
	//Write the physical address into ASYNCICLISTADDR(ehci_base) Register
	writew(PhyQHEAD1,ASYNCICLISTADDR(ehci_base));
	
	//Enable Asynchronous list
	EHCI_Asynchronous_Enable(1);

	while(!(TransferComplete | ErrorComplete))
	{
	}
	if(TransferComplete)
	{
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		if(Success(pQTD2->QTDChar)) 	// checking the status of the completion
		{
			printf("Clear Feature Passed in Driver\n");
		}
		else
		{
			printf("some other error\n");
		}
	}
	else if(ErrorComplete)
	{
		printf("Transfer failed\n");
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
	}
	TransferComplete = 0;
	ErrorComplete = 0;
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QTD_Ring(pQTD1);		
	Add_Free_QTD_Ring(pQTD2);
	return TRUE;
}

/* ******************************************************************************** *   
Function Name : int GetTTState(BYTE TTPort)
Description   : Reading the TT state of the connected hub
Params        : TTport number
Returns       : True:GetTTState Success  / False: GetTTState Fail    
Note          :  
******************************************************************************** */
//api_new
int GetTTState(BYTE TTPort)
{
//yet to be changed from getportstatus function

	unsigned int i;
	unsigned int BytesTransferred;
	unsigned int InitialTransferLength;
	PQHEAD pQHEAD1;
	PQTD pQTD1,pQTD2,pQTD3;
	ULONG PhyQHEAD1,PhyQTD1,PhyQTD2,PhyQTD3,PhyBuffer0,PhyBuffer1;
	UCHAR *VirBuffAddr0,*VirBuffAddr1;
	unsigned int	UnderHub;
	UnderHub = 0;
	printf("for a device under hub ?\n");
	printf("0.No\n");
	printf("1.Yes\n");
	scanf("%d",&UnderHub);
	
	EHCI_Asynchronous_Enable(0);
	
	VirBuffAddr0=((UCHAR *)SETUP_BUFF_ADDR);
	VirBuffAddr1=(UCHAR *)((UCHAR *)SETUP_BUFF_ADDR+0x120);
	PhyBuffer0=virt_to_phy((u32_t)(VirBuffAddr0));
	PhyBuffer1=virt_to_phy((u32_t)(VirBuffAddr1));
	
	//For the 8 bytes of setup stage
	*(VirBuffAddr0+0)=0xA3;
	*(VirBuffAddr0+1)=HUB_CLASS_GET_TT_STATE;
	*(VirBuffAddr0+2)=0x00;
	*(VirBuffAddr0+3)=0x00;
	*(VirBuffAddr0+4)=0x00|TTPort;			// tt port number is assigned here
	*(VirBuffAddr0+5)=0x00;
	*(VirBuffAddr0+6)=0x40; 		//length has been given as per as the need
	*(VirBuffAddr0+7)=0x00;
	
	//printf(("Initializing 1 QHEAD and 3 QTDs for GetStatus"));
	//Form QTD3 for Status Stage of GetTTState=================================
	pQTD3 = Get_Free_QTD();
	//pQTD3=(PQTD)(SETUP_BUFF_ADDR+1504);
	PhyQTD3=virt_to_phy((u32_t)pQTD3);
	
	pQTD3->NextQTD=T_BIT;
	pQTD3->AltQTD=T_BIT;
//	pQTD3->QTDChar=0x80008c80;
	pQTD3->QTDChar = DT |IOC |C_ERR|PID_OUT|ACTIVE; 
	pQTD3->QTDBuffPrt0=0x00000000;
	pQTD3->QTDBuffPrt1=0x00000000;
	pQTD3->QTDBuffPrt2=0x00000000;
	pQTD3->QTDBuffPrt3=0x00000000;
	pQTD3->QTDBuffPrt4=0x00000000;
	
	//Form QTD2 for Data Stage of GetStatus=====================================
	pQTD2 = Get_Free_QTD();
	//pQTD2=(PQTD)(SETUP_BUFF_ADDR+1472);
	PhyQTD2=virt_to_phy((u32_t)pQTD2);

	pQTD2->NextQTD=(ULONG)PhyQTD3;
	pQTD2->AltQTD=(ULONG)PhyQTD3;
//	pQTD2->QTDChar=0x80028d80;
//	pQTD2->QTDChar=0x80040d80;				// ioc is disabled
	pQTD2->QTDChar= DT |C_ERR|PID_IN|ACTIVE|(0x04*TOTAL_LENGTH);	// since transfer length is 4 bytes ((0x04<<16))
	pQTD2->QTDBuffPrt0=(ULONG)PhyBuffer1;
	pQTD2->QTDBuffPrt1=0x00000000;
	pQTD2->QTDBuffPrt2=0x00000000;
	pQTD2->QTDBuffPrt3=0x00000000;
	pQTD2->QTDBuffPrt4=0x00000000;
	InitialTransferLength = (pQTD2->QTDChar & 0x7FFF0000)>> 16;
	
	//Form QTD1 for Setup Stage of GetStatus===================================
	pQTD1 = Get_Free_QTD();
	//pQTD1=(PQTD)(SETUP_BUFF_ADDR+1440);
	PhyQTD1=virt_to_phy((u32_t)pQTD1);
	
	pQTD1->NextQTD=(ULONG)PhyQTD2;
	pQTD1->AltQTD=(ULONG)PhyQTD2;
//	pQTD1->QTDChar=0x00088e80;
//	pQTD1->QTDChar=0x00080e80;				// ioc is disabled
	pQTD1->QTDChar= (8*TOTAL_LENGTH) | C_ERR|PID_SETUP |ACTIVE;
	pQTD1->QTDBuffPrt0=(ULONG)PhyBuffer0;
	pQTD1->QTDBuffPrt1=0x00000000;
	pQTD1->QTDBuffPrt2=0x00000000;               
	pQTD1->QTDBuffPrt3=0x00000000;
	pQTD1->QTDBuffPrt4=0x00000000;
	
	//Form QHEAD======================================================
	pQHEAD1 = Get_Free_QH();
	//pQHEAD1=(PQHEAD)(SETUP_BUFF_ADDR+1376);
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);
	
	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
	if(UnderHub)
	{
		pQHEAD1->EpChar=(3*RL) |(C_Bit << 27)|(ehci_status.CepMps_Hub <<16) |H_BIT |DTC |(Device_Hub_Speed << 12) |(ehci_status.DevAddress_Hub);
		pQHEAD1->EpCap= MULT | (ehci_status.Portnum << 23)|(ehci_status.DevAddress << 16);
	}
	else
	{
		pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
		pQHEAD1->EpCap=0x40000000; 					// mult field is set to 1
	}
//	pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
//	pQHEAD1->EpCap=MULT;
//	pQHEAD1->EpChar=0x3040e000|(ehci_status.DevAddress);
//	pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
//	pQHEAD1->EpCap=0xc0000000;
//	pQHEAD1->EpCap=0x40000000;										// mult value is made 1
	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;
	

	//printf(("Writing Physical Address of QHEAD into ASYNCICLISTADDR(ehci_base)"));
	//Write the physical address into ASYNCICLISTADDR(ehci_base) Register
	writew(PhyQHEAD1,ASYNCICLISTADDR(ehci_base));
	
	//Enable Asynchronous list
	EHCI_Asynchronous_Enable(1);
	while(!(TransferComplete | ErrorComplete))
	{
	}
	if(TransferComplete)
	{
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		printf("the value of qtd3 char is %x\n",pQTD3->QTDChar);
		if(Success(pQTD3->QTDChar)) 	// checking the status of the completion
		{
			printf("Get Port Status Passed in Driver");
			BytesTransferred = InitialTransferLength - ((pQTD2->QTDChar & 0x7FFF0000)>> 16);
			printf("Total bytes transferred is %d\n",BytesTransferred);
			for(i=0;i<BytesTransferred;i++)
			{
				printf("the value of %d byte is %x\n",i,*(VirBuffAddr1+i));
			}
/*			if(*(VirBuffAddr1+2)&0x10)
			{
				printf("Reset change bit is set\n");
				switch(*(VirBuffAddr1+1)&0x06)
				{
					case 0:
						printf("Connected device is full speed device\n");
						Device_Hub_Speed = 0;
						C_Bit = 1;
						break;
					case 2:
						printf("Connected device is low speed device\n");
						Device_Hub_Speed = 1;
						C_Bit = 0;
						break;
					case 4:
						printf("Connected device is high speed device\n");
						Device_Hub_Speed = 2;
						C_Bit = 1;
						break;
					default:
						break;
				}
				if(!UnderHub)
				{
					ehci_status.DevAddress_Hub = 0;
					ehci_status.CepMps_Hub = 0x40;
//					C_Bit = 0;
				}
			}*/
		}
		else
		{
			printf("some other error\n");
		}
	}
	else if(ErrorComplete)
	{
		printf("Transfer failed\n");
		printf("the value of qtd1 char is %x\n",pQTD1->QTDChar);
		printf("the value of qtd2 char is %x\n",pQTD2->QTDChar);
		printf("the value of qtd3 char is %x\n",pQTD3->QTDChar);
	}
	TransferComplete = 0;
	ErrorComplete = 0;
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QTD_Ring(pQTD1);		
	Add_Free_QTD_Ring(pQTD2);
	Add_Free_QTD_Ring(pQTD3);
	return TRUE;
}
int Success(ULONG trans)	//Returns TRUE when QTD is a success, else returns FALSE
{
	ULONG val=0;
	ULONG status=0;
	
	val=trans&0x00000c00;
	if (val)
	{
		status= trans & 0x000000d0;
		if (!status)
			return TRUE;
		else
			return FALSE;
	}
	return FALSE;
}

// viswa added the following 4 functions to check ehci wiht ohci enabled
int EHCIOHCI_Reset(void)
{
	unsigned long HcCommandStatus;

	printf("Inside ohci reset\n");
	HcCommandStatus=(readw(HcCMDSTS(ehci_base)/*0x180a4008*/))|0x00000001;  //HostControllerReset
	writew(HcCommandStatus,HcCMDSTS(ehci_base)/*0x180a4008*/);
	HcCommandStatus = readw(HcCMDSTS(ehci_base)/*0x180a4008*/);
	printf("the value of hccommand status register is %x\n",HcCommandStatus);
	while(HcCommandStatus & 0x00000001)
	{
		HcCommandStatus=readw(HcCMDSTS(ehci_base)/*0x180a4008*/);
		printf("inside loop\n");
	}
	printf("Ohci reset completed\n");
	return 0;
}

int EHCIOHCI_Intr_Disable(void)
{
	u32_t Hcintrdis;

	printf("Disabling Device Interrupts\n");
	// read the Interrupt Disable register first
	Hcintrdis=readw(HcINTRDISEN(ehci_base)/*0x180a4014*/);
	
	//disable all the interrupts from ohc card, writing to Interrupt Disable register
	Hcintrdis=Hcintrdis|0xc000007f;
	writew(Hcintrdis,HcINTRDISEN(ehci_base)/*0x180a4014*/);
	printf("HcInterruptEnable                                      :%x\n",readw(HcINTREN(ehci_base)/*0x180a4010*/));
	printf("\nDisableDeviceInterrupts Completed\n");
	return 0;
}
int EHCIOHCI_Intr_Enable(void)
{
	u32_t HcInterruptEnable,Hcintrdis;
	printf("inside interrupt enable\n");
	HcInterruptEnable=readw(HcINTREN(ehci_base)/*0x180a4010*/);
//	Hcintrdis = 0x00000000;
//	writew(Hcintrdis,0x180a4014);
	printf("\nCurrent value of HcInterruptEnable: 		%x\n",HcInterruptEnable);
	// Enable interrupt due to Root hub status change and Writeback donehead
//	HcInterruptEnable=HcInterruptEnable|0x80000046;		// enabling WDH,SF and RHSC
	HcInterruptEnable=HcInterruptEnable|0x80000042;		// enabling WDH and RHSC
	writew(HcInterruptEnable,HcINTREN(ehci_base)/*0x180a4010*/);
	Hcintrdis=0x00000000;
	writew(Hcintrdis,HcINTRDISEN(ehci_base)/*0x180a4014*/);
	printf("ohci interrupt enabled\n");
	return 0;
}

void EHCIOHCI_Start()
{
	unsigned long temphcca,HcControlreg,HcRhPortstatusreg;
	EHCIOHCI_Intr_Disable();

	// moving the host controller to operational state
	HcControlreg = readw(HcControl(ehci_base)/*0x180a4004*/);
	HcControlreg= HcControlreg & 0xffffff3f;
	HcControlreg= HcControlreg|0x00000080;  //USB OPERATIONAL
	writew(HcControlreg,HcControl(ehci_base)/*0x180a4004*/);

	//Apply HC Reset
	EHCIOHCI_Reset();			//commented for testing

	printf("in ohci start after ohci reset\n");
//	PrintOHCIRegs();
	
	// moving the host controller to USBRESET state
	HcControlreg = readw(HcControl(ehci_base)/*0x180a4004*/);
	HcControlreg= HcControlreg & 0xffffff3f;
	HcControlreg= HcControlreg|0x00000000;  //USB RESET state
	writew(HcControlreg,HcControl(ehci_base)/*0x180a4004*/);
	printf("the value of hccontrol register is 		:%x\n",readw(HcControl(ehci_base)/*0x180a4004*/));

	// connecting to interrupt
//	INT_SET_TYPE(ehci_irq, LO_LEVEL);
//	INT_SET_PRIORITY(ehci_irq,7);		
//	connectInterrupt( ehci_irq, OHCIIntrHandler, NULL);
	//enabling
//	INT_ENABLE(ehci_irq);
//	INT_SET_MASK(ehci_irq);
	
	// Write to hcPeriodicStart register to specify when periodic transactions should start
	writew(0x00003e67,Hcperiodicstart(ehci_base)/*0x180a4040*/);
	printf("HcPeriodicStart                                        :%x\n",readw(Hcperiodicstart(ehci_base)/*0x180a4040*/));

	// Load the FSMPS 
//	writew(0x27782edf,0x180a4034);
	writew(0x26502edf,HcFmIntr(ehci_base)/*0x180a4034*/);
//	writew(0x27D72edf,0x180a4034);
	printf("HcFrameInterval                                       :%x\n",readw(HcFmIntr(ehci_base)/*0x180a4034*/));
	
	// Fill the base address of the HCCA into HcHCCA
	memset((PUCHAR)EHCIOHCI_Hc_HCCA,0,256);
	printf("the value of Hc_HCCA is 	%x\n",EHCIOHCI_Hc_HCCA);
	printf("the virtual address of Hc_HCCA is 	%x\n",virt_to_phy((u32_t)EHCIOHCI_Hc_HCCA));
	writew(virt_to_phy((u32_t)EHCIOHCI_Hc_HCCA),HcHCCA(ehci_base)/*0x180a4018*/);
	temphcca=readw(HcHCCA(ehci_base)/*0x180a4018*/);
	printf("\nHcHcca Physical Address				: %x\n",temphcca);
//	HcHcca = (POHCI_HCCA)Hc_HCCA;

	// moving the host controller to operational state
	HcControlreg = readw(HcControl(ehci_base)/*0x180a4004*/);
	HcControlreg= HcControlreg & 0xffffff3f;
	HcControlreg= HcControlreg|0x00000083;  //USB OPERATIONAL state and CBSR as 3:1
	writew(HcControlreg,HcControl(ehci_base)/*0x180a4004*/);
	printf("HcControl                                                 :%x\n",readw(HcControl(ehci_base)/*0x180a4004*/));
//	printf("the value of HcHcca.frame number is 	   :%x\n",HcHcca->frame_no);

	// apply port power status to RhPortStatus register
	HcRhPortstatusreg = 0x00000100;
	writew(HcRhPortstatusreg,HcRhPortStatus(ehci_base)/*0x180a4054*/);
	// Enable device interrupts
	EHCIOHCI_Intr_Enable();
//	printf("the value of HcHcca.frame number is 	   :%x\n",HcHcca->frame_no);
	
//	OHCI_InitMemory();
//	printf("the value of HcHcca.frame number is 	   :%x\n",HcHcca->frame_no);
}


void	Display_EHCIMenu(void)
{

	printf("LII EHCI Testing.\n");
	printf("==================================================\n");
	printf("1.Base Test\n");
	printf("2.Hub Test\n");
	printf("3.Ctrl Test\n");
	printf("4.Bulk Test\n");
	printf("5.INTR Test\n");
	printf("6.Isochronous Test\n");
	printf("7.Print registes\n");
	printf("8.Enable ohci also\n");
	printf("\nX. Exit EHCI Part Back to Main Menu\n");
	printf("\nz. auto test\n");
	printf("==================================================\n");

}

void Display_EHCIAutoTestMenu(void)
{
	printf("Auto Test\n");
	printf("==================================================\n");
	printf("1.Bulk In/Out Test\n");
	printf("2.Intr In Test\n");
	printf("\nX. Exit Auto Test Back to EHCI Main Menu\n");
	printf("==================================================\n");	
}

void Display_EHCIBASETestMenu(void)
{
	printf("BASE Test\n");
	printf("==================================================\n");
	printf("1.Default Values Check\n");	
	printf("2.Test Invalid Port Enable\n");			
	printf("3.Test Port Disable \n");	
	printf("4.Ehci_Suspend \n");
	printf("5.Ehci_Resume \n");
	printf("6.Frame List Increment Test\n");
	printf("7.EHCI START\n");
	printf("8.Ehci_reset\n");	
	printf("\nX. Exit Base Test Back to EHCI Main Menu\n");
	printf("==================================================\n");	
}

void Display_EHCICtrlMenu(void)
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
	printf("a.Print Registers\n");	
	printf("\nX. Exit Ctrl Test Back to EHCI Main Menu\n");
	printf("==================================================\n");	

}

void Display_EHCIIntrMenu(void)
{
	printf("Intr Test\n");
	printf("==================================================\n");
	printf("r.Intr Rx\n");
//	printf("t.Intr Tx\n");
//	printf("a.Intr Rx/Tx auto transfer \n");	
	printf("\nX. Exit Intr Test Back to EHCI Main Menu\n");
	printf("==================================================\n");	

}

void Display_EHCIBulkMenu(void)
{
	printf("Bulk Request Test\n");
	printf("==================================================\n");
	printf("r.Bulk Rx \n");
	printf("t.Bulk Tx \n");
	printf("a.Bulk Rx/Tx auto transfer \n");	
	printf("\nX. Exit Bulk Test Back to EHCI Main Menu\n");
	printf("==================================================\n");	

}

//api_new

void Display_EHCIIsoMenu(void)
{
	printf("Iso Request Test\n");
	printf("==================================================\n");
//	printf("r.Isochronous Rx \n");
//	printf("t.Isochronous Tx \n");
	printf("a.Isochrous Rx/Tx auto transfer \n");	
	printf("\nX. Exit Iso Test Back to EHCI Main Menu\n");
	printf("==================================================\n");	

}


void Display_EHCIHubMenu(void)
{
	printf("Hub Request Test\n");
	printf("==================================================\n");
	printf("1.GetHubDescriptor\n");
	printf("2.GetPortStatus\n");
	printf("3.GetHubStatus\n");	
	printf("4.ClearPortFeature\n");
	printf("5.ClearHubFeature\n");
	printf("6.ResetTT\n");	
	printf("7.StopTT\n");
	printf("8.SetHubFeature\n");	
	printf("9.SetPortFeature\n");	
	printf("a.ClearTTBuffer\n");
	printf("b.SetHubDescriptor\n");	
	printf("c.GetTTState\n");		
	printf("\nX. Exit Hub Request Back to EHCI Main Menu\n");
	printf("==================================================\n");	
}

void Erumate_Device()
{
	ResetDevice();
	SetAddress(1);
	GetDescriptor(1, 0, 18);

	SetConfiguration(1);
}

void Display_SuspendResumeMenu()
{
	printf("UHC Suspend and Resume Menu.\n");
	printf("==================================================\n");
	printf("1.Suspend Port\n");
	printf("2.Resume Port\n");
	printf("3.Reset Port\n");
	printf("4.Auto Suspend Resume\n");
	printf("\nX. Exit UHC Suspend and Resume Back to Main Menu\n");
	printf("==================================================\n");
}


/* ******************************************************************************** *   
Function Name : int GetDescriptor(BYTE DevDescType,BYTE DevDescIndex,int DevDescLength)
Description   : Get Device Descriptor 
Params        : Descriptor Type,Descriptor Index,Descriptor Length
Returns       : True:Get Device Descriptor Success  / False: Get Device Descriptor Fail    
Note          :  
******************************************************************************** */
int Single_GetDescriptor(BYTE DevDescType,BYTE DevDescIndex,int DevDescLength)
{
	unsigned int i;
	unsigned int BytesTransferred;
	unsigned int InitialTransferLength;
	PQHEAD pQHEAD1;
	PQTD pQTD1,pQTD2,pQTD3;
	ULONG PhyQHEAD1,PhyQTD1,PhyQTD2,PhyQTD3,PhyBuffer0,PhyBuffer1;
	UCHAR *VirBuffAddr0,*VirBuffAddr1;
	unsigned int	UnderHub;
	UnderHub = 0;

	//Disable Asynchronous list processing
	printf("\nSingle_get Desc \n");
//	getchar();
	getc();

	EHCI_Asynchronous_Enable(0);
	
	VirBuffAddr0=((UCHAR *)SETUP_BUFF_ADDR);						// for setup data
	VirBuffAddr1=(UCHAR *)((UCHAR *)SETUP_BUFF_ADDR + 0x20);		// for descriptor
	PhyBuffer0=virt_to_phy((u32_t)(VirBuffAddr0));
	PhyBuffer1=virt_to_phy((u32_t)(VirBuffAddr1));
	
	//For the 8 bytes of setup stage
	*(VirBuffAddr0+0)=0x80;
	*(VirBuffAddr0+1)=USB_REQ_GET_DESCRIPTOR;
	*(VirBuffAddr0+2)=0x00;// CHANGE Descriptor Index Here
	*(VirBuffAddr0+3)=0x00|DevDescType;// CHANGE Descriptor Type Here
	*(VirBuffAddr0+4)=0x00;
	*(VirBuffAddr0+5)=0x00;
	*(VirBuffAddr0+6)=0x00|DevDescLength;// CHANGE Descriptor Length Here
	*(VirBuffAddr0+7)=0x00|DevDescLength>>8;
		
	
	//Form QTD1 for Setup Stage of GetDescriptor==============================
	pQTD1 = Get_Free_QTD();
	PhyQTD1=virt_to_phy((u32_t)pQTD1);
	
	pQTD1->NextQTD=pQTD3;
	pQTD1->AltQTD=pQTD3;
	// data toggle field 0,total bytes to transfer is set to 8,CERR field 3,PID field 0,Active bit is enabled

	pQTD1->QTDChar= (8*TOTAL_LENGTH) | C_ERR|PID_SETUP |ACTIVE;
	pQTD1->QTDBuffPrt0=(ULONG)PhyBuffer0;
	pQTD1->QTDBuffPrt1=0x00000000;
	pQTD1->QTDBuffPrt2=0x00000000;
	pQTD1->QTDBuffPrt3=0x00000000;
	pQTD1->QTDBuffPrt4=0x00000000;

	//Form QHEAD====================================================
	pQHEAD1 = Get_Free_QH();
	PhyQHEAD1=virt_to_phy((u32_t)pQHEAD1);
	
	pQHEAD1->QHEADLinkPtr=(ULONG)(((ULONG)PhyQHEAD1)|0x2);
	
	if(MpsKnown == 0)
	{
			pQHEAD1->EpChar=0x0040e000|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
			pQHEAD1->EpCap=0x40000000; 					// mult field is set to 1
	}
	else
	{
			pQHEAD1->EpChar=0x0000e000|(ehci_status.CepMps<<16)|(ehci_status.DevAddress);//CHANGE Add Endpoint Address here
			pQHEAD1->EpCap=0x40000000; 					// mult field is set to 1
	}

	pQHEAD1->CurrQTD=0x00000000;
	pQHEAD1->NextQTD=(ULONG)PhyQTD1;
	pQHEAD1->AltQTd=(ULONG)PhyQTD1;
	pQHEAD1->QTDChar=0x00000000;
	pQHEAD1->QHEADBuffPtr0=0x00000000;
	pQHEAD1->QHEADBuffPtr1=0x00000000;
	pQHEAD1->QHEADBuffPtr2=0x00000000;
	pQHEAD1->QHEADBuffPtr3=0x00000000;
	pQHEAD1->QHEADBuffPtr4=0x00000000;
	
	writew((ULONG)PhyQHEAD1,ASYNCICLISTADDR(ehci_base));
	
	EHCI_Asynchronous_Enable(1);

	EHCI_Asynchronous_Enable(0);

	printf("next Step\n");
//	getchar();
	getc();

	//Form QTD2 for Data Stage of GetDescriptor===============================
	pQTD2 = Get_Free_QTD();
	PhyQTD2=virt_to_phy((u32_t)pQTD2);
	
	pQTD2->NextQTD=pQTD3;
	pQTD2->AltQTD=pQTD3;
	pQTD2->QTDChar= DT |C_ERR|PID_IN|ACTIVE|(DevDescLength*TOTAL_LENGTH);		// disabling the ioc since only in last qtd i,e status stage
	pQTD2->QTDBuffPrt0=(ULONG)PhyBuffer1;
	pQTD2->QTDBuffPrt1=0x00000000;
	pQTD2->QTDBuffPrt2=0x00000000;
	pQTD2->QTDBuffPrt3=0x00000000;
	pQTD2->QTDBuffPrt4=0x00000000;
	InitialTransferLength = (pQTD2->QTDChar & 0x7FFF0000)>> 16;

	pQHEAD1->NextQTD=(ULONG)PhyQTD2;
	pQHEAD1->AltQTd=(ULONG)PhyQTD2;
	writew((ULONG)PhyQHEAD1,ASYNCICLISTADDR(ehci_base));
	
	EHCI_Asynchronous_Enable(1);

	EHCI_Asynchronous_Enable(0);
	printf("next \n");
//	getchar();
	getc();

	//Form QTD3 for Status Stage of GetDescriptor==============================

	pQTD3 = Get_Free_QTD();
	PhyQTD3=virt_to_phy((u32_t)pQTD3);
	
	//Change the data toggle to 1
	pQTD3->NextQTD=T_BIT;
	pQTD3->AltQTD=T_BIT;
	// data toggle field 1,IOC is enabled,CERR is 3,PID field is 0,Active bit is set
//	pQTD3->QTDChar=0x80008c80;
	pQTD3->QTDChar = DT |IOC |C_ERR|PID_OUT|ACTIVE; 
	pQTD3->QTDBuffPrt0=0x00000000;
	pQTD3->QTDBuffPrt1=0x00000000;
	pQTD3->QTDBuffPrt2=0x00000000;
	pQTD3->QTDBuffPrt3=0x00000000;
	pQTD3->QTDBuffPrt4=0x00000000;
	
	pQHEAD1->NextQTD=(ULONG)PhyQTD3;
	pQHEAD1->AltQTd=(ULONG)PhyQTD3;
	writew((ULONG)PhyQHEAD1,ASYNCICLISTADDR(ehci_base));
	
	EHCI_Asynchronous_Enable(1);

	EHCI_Asynchronous_Enable(0);
	
//	printf("complete \n");
//	getchar();
	
	if(TransferComplete)
	{
		if(Success(pQTD3->QTDChar)) 	// checking the status of the completion
		{
			//printf("Get Descriptor Passed in Driver");
			if(DevDescType == 1)
			{
				ehci_status.CepMps = *(VirBuffAddr1 + 7);
				MpsKnown = 1;
				if(UnderHub)
				{
					ehci_status.CepMps_Hub= *(VirBuffAddr1 + 7);
					//printf("the value of control endpoint mps is %x\n",ehci_status.CepMps_Hub);
				}
				//printf("the value of control endpoint mps is %x\n",ehci_status.CepMps);
			}
			BytesTransferred = InitialTransferLength - ((pQTD2->QTDChar & 0x7FFF0000)>> 16);
			printf("Total bytes transferred is %d\n",BytesTransferred);
			for(i=0;i<BytesTransferred;i++)
			{
				printf("the value of %d byte is %x\n",i,*(VirBuffAddr1+i));
				if(i == 4)
				{
					if(*(VirBuffAddr1+i) == 0x09)	// Hub class
					{
						printf("Connected device is a hub class device\n");
						if(*(VirBuffAddr1+6) == 1 )
						{
							printf("single TT\n");
							MultipleTT = 0;
						}
						else
						{
							printf("Mulitiple TT \n");
							MultipleTT = 1;
						}
					}
				}
			}
		}
		else
		{
			printf("some other error\n");
		}
	}
	else if(ErrorComplete)
	{
		printf("Transfer failed\n");

	}
	
	TransferComplete = 0;
	ErrorComplete = 0;
	Add_Free_QH_Ring(pQHEAD1);
	Add_Free_QTD_Ring(pQTD1);		
	Add_Free_QTD_Ring(pQTD2);
	Add_Free_QTD_Ring(pQTD3);
	return 0;
}


/*
 *  Packet_Parameter() - 
 *	
 *	
 *	
 *	Returns a negative error code on failure and 0 on success.
 */
int Packet_Parameter()
{
	int	result = 0;	
	char	testItem;

	printf("Please Connect Device\n");

	while(!portconnect)
	{
	}

	Erumate_Device();

	while (1) {
			result = 0;
			printf("\nPlease Enter Perform Single_Get_Descriptor (Device) \n");
			//testItem = getchar();
			testItem = getc();
			switch (testItem) 
			{		
				case 'x' : //X. Exit USB Test Mode Back to Main Menu
				case 'X' :
					// Back to default
					printf("Back to Mainmenu ..........\n");
					return result;
				default :
					Single_GetDescriptor(1, 0, 18);
					break;
			}
			if (result)
				printf("fail");
			printf("press Enter to continue ...");
			//getchar();
			getc();
		}
	
	return 0;

}

/*
 *  Display_Packet_ParameterMenu() - 
 *	
 *	
 *	
 *	Returns a negative error code on failure and 0 on success.
 */

void Display_Packet_ParameterMenu(void)
{
	printf("UHC Packet Parameter Menu.\n");
	printf("==================================================\n");
	printf("1.Response Time\n");
	printf("2.Device/Host Response Withe Worst Case Interpacket Delay\n");
	printf("3.Host SOF Sync and EOP Bits\n");
	printf("\nX. Exit UHC Packet Parameter Back to Main Menu\n");
	printf("==================================================\n");
}

void Auto_Erumation()
{
	int i;
	for(i=0;i<30;i++)
	{
		Erumate_Device();
		MSDELAY(5);
	}
}


int Chirp_Timing()
{
	int	result = 0;	
	char	testItem;
	
	printf("Please Connect Device \n");
	while(!portconnect)
	{
	}
	
	//erumate device functiuon
	
	while (1) {
		result = 0;

		printf("\n Please Enter to Erumate bus or X to Exit\n");
		//testItem = getchar();
		testItem = getc();
		switch (testItem) 
		{	
			case '1':
				Auto_Erumation();
				break;		
			case 'x' : //X. Exit USB Test Mode Back to Main Menu
			case 'X' :
				// Back to default
				printf("Back to Mainmenu ..........\n");
				return result;
			default :
				Erumate_Device();
				break;
		}
		printf(".......USB Test_Mode Item (%c)\n", testItem);
		printf("press Enter to continue ...");
		//getchar();
		getc();
	}	

	return 0;

}


/*
 *  TestJKSE0_State() - Run the J-/K-/SE0_NAK-state of test mode
 *	Descriptor:	These tests measure the DC drive capabilities of the high-speed drivers
 *				using a voltmeter between the signal lines and GND (i.e. D+/D-)
 *	Parameters:	Null
 *	Returns: 	Return success when finished the testing
 */
int TestJKSE0_State()
{
	u32_t portstate;
	char testItem, SubItem;
	int	result = 0;
	
	Display_TestJKModeMenu();
	
	while (1){
		result = 0;
		
		printf("Entry Test Item>");
		testItem = getc();
		portstate = readw(PORTSC(ehci_base));
		printf("portst=%x\n", portstate);
		switch (testItem){
			case 'e':
			case 'E':
				Erumate_Device();
				break;
			case 'j':
			case 'J':
				do {
					writew((portstate|PORT_J_TEST), PORTSC(ehci_base));
					printf("Continue Y/N>\n");
					SubItem = getc();
					//printf("%c\n", SubItem);
				}while(SubItem=='y');
				portstate = readw(PORTSC(ehci_base));
				printf("portst=%x\n", portstate);
				writew((portstate&(~PORT_TEST_CTRL)), PORTSC(ehci_base));	//Disable Test-Mode
				printf("portst=%x\n", readw(PORTSC(ehci_base)));
				printf("Disable J-state mode\n");
				break;
			case 'k':
			case 'K':
				do {
					writew((portstate|PORT_K_TEST), PORTSC(ehci_base));
					printf("Continue Y/N>\n");
					SubItem = getc();
					//printf("%c\n", SubItem);
				}while(SubItem=='y');
				portstate = readw(PORTSC(ehci_base));
				printf("portst=%x\n", portstate);
				writew((portstate&(~PORT_TEST_CTRL)), PORTSC(ehci_base));	//Disable Test-Mode
				printf("portst=%x\n", readw(PORTSC(ehci_base)));
				printf("Disable K-state mode\n");
				break;
			case 's':
			case 'S':
				writew((portstate|PORT_SE0_NAK_TEST), PORTSC(ehci_base));
				do {
					writew((portstate|PORT_SE0_NAK_TEST), PORTSC(ehci_base));
					printf("Continue Y/N>\n");
					SubItem = getc();
					//printf("%c\n", SubItem);
				}while(SubItem=='y');
				portstate = readw(PORTSC(ehci_base));
				printf("portst=%x\n", portstate);
				writew((portstate&(~PORT_TEST_CTRL)), PORTSC(ehci_base));	//Disable Test-Mode
				printf("portst=%x\n", readw(PORTSC(ehci_base)));
				printf("Disable SE0_NAK-state mode\n");
				break;
			case 'x' : //X. Exit 
			case 'X' :
				printf("Bye Bye ..........\n");
				return result;
			default:
				Display_TestJKModeMenu();
				break;
		}
		printf("press Enter to continue...\n");
		getc();
	}
	return result;
}


/*
 *  HS_Signal_Quality()
 *	Descriptor:	These tests measure the ability of transmitters to do 
 *				valid high speed signaling.
 *				It's measured on both upstream & downstream ports.
 *	Parameters:	Null
 *	Returns:	Return success when finished the testing
 */
int HS_Signal_Quality()
{
	u32_t portstate;
	int	result = 0;
	/*	
	*	Upstream port
	*	This test is run for all devices & hubs on their upstream port.
	*	Downstream port (host)
	*	This test checks a host's capability to transmit properly
	*/
	
	portstate = readw(PORTSC(ehci_base));
	printf("portstat=%x\n", portstate);

		writew((portstate|PORT_PACKET_TEST), PORTSC(ehci_base));	/* Place port in Test Mode TEST_PACKET */
	
	printf("press Enter to continue...\n");
	getc();
	
	portstate = readw(PORTSC(ehci_base));
	printf("Test_Mode portstat=%x\n", portstate);
	writew((portstate&(~PORT_TEST_CTRL)), PORTSC(ehci_base));	//Disable Test-Mode
	printf("Disable portstat=%x\n", readw(PORTSC(ehci_base)));
	return result;
	
}


/*
 * hc states include: unknown, halted, ready, running
 * transitional states are messy just now
 * trying to avoid "running" unless urbs are active
 * a "ready" hc can be finishing prefetched work
 */

int ehci_suspend()
{
	int					temp;

	/* suspend 1 port, then stop the hc */

	temp = readw (PORTSC(ehci_base));
	printf("portsc = %x \n",temp);

	if ((temp & PORT_PE) == 0	|| (temp & PORT_OWNER) != 0)
	{
		printf("========return \n");
		//return 0;
	}
		
	printf("suspend port ");
	
	temp |= PORT_SUSPEND;
	writew(temp, PORTSC(ehci_base));

//	while (readw (USBSTS(ehci_base)) & (STS_ASS | STS_PSS))
//		for(i=0;i<10;i++);
	
	//writew (readw (USBCMD(ehci_base))&~CMD_RUN, USBCMD(ehci_base));
	//printf("portsc %x sts:%x\n",readw(PORTSC(ehci_base)),readw(USBSTS(ehci_base)));

	temp = readw (PORTSC(ehci_base));
	printf("portsc = %x \n",temp);

	MSDELAY (10);
	printf("suspend ok \n");


	return 0;
}

int ehci_resume ()
{
	int					temp;

	printf("ehci resume\n");

	temp = readw (PORTSC(ehci_base));
	printf("portsc = %x \n",temp);
	
	temp = readw (PORTSC(ehci_base));

	temp |= PORT_RESUME;
	temp &= ~(PORT_SUSPEND);
	printf("w1 tmp: %x\n",temp);
	writew(temp,PORTSC(ehci_base));
	//readw (USBCMD(ehci_base));	/* unblock posted writes */


	MSDELAY (10);
	//writew (readw (USBCMD(ehci_base))&CMD_RUN, USBCMD(ehci_base));	

	temp = readw (PORTSC(ehci_base));
	temp &= ~(PORT_RESUME);
	temp &= ~(PORT_SUSPEND); 

	printf("w2 tmp: %x\n",temp);

	writew (temp,PORTSC(ehci_base));
	

	
	return 0;
}

void Suspend_Resume_auto()
{
	int i;
	for (i=0;i<30;i++)
	{
		ehci_suspend();
		ehci_resume();
	}
}


int Suspend_Resume()
{
	int	result = 0;	
	char	testItem;
	ULONG UsbIntr;
	
	printf("Please Connect Device \n");
	while(!portconnect)
	{
	}
	
	//erumate device functiuon
	Erumate_Device();

	while (1) {
		Display_SuspendResumeMenu();

		result = 0;

		//
		UsbIntr= 0x00000002;
		writew(UsbIntr, USBINTR(ehci_base));		
		//
		printf("Packet Parameter Menu>");
		//testItem = getchar();
		testItem = getc();
		switch (testItem) 
		{		
			case '1':
				printf("%x",readw(USBINTR(ehci_base)));
				result = ehci_suspend();
				break;
			case '2':
				printf("%x",readw(USBINTR(ehci_base)));
				result = ehci_resume();
				break;
			case '3':
				ResetDevice();
				//result = Reset_Port();
				break;
			case '4':
				Suspend_Resume_auto();
				break;

			case 'x' : //X. Exit USB Test Mode Back to Main Menu
			case 'X' :
				// Back to default
				printf("Back to Mainmenu ..........\n");
				return result;
			default :
				Display_SuspendResumeMenu();
				break;
		}
		printf(".......USB Test_Mode Item (%c)\n", testItem);
		printf("press Enter to continue ...");
		//getchar();
		getc();
	}

	return 0;

}


int USB_TestMode()
{
	int	result = 0;
	char	testItem;

	portconnect = 0;
	TransferComplete = 0;
	Device_Hub_Speed = 2;	// default high speed
	C_Bit =0;
	ehci_status.CepMps = 0x40;
	ehci_status.CepMps_Hub = 0x40;
	//PQHEAD pQH;
	//char *SetupData;
//	PrintRegs();
	
	EHCI_Start();

//	PrintRegs();
	
	InitMemory();
	//Set_1ms_timer(10);

	QH_Init();
	QTD_Init();
	iTD_Init();//api_new change
	
	while (1) {
		Display_TestModeMenu();

		result = 0;

		//printf("USB Test_Mode>");
		//testItem = getchar();
		printf("\nUSB Test_Mode>");
		testItem = getc();
		switch (testItem) 
		{		
			case '1':
				result = HS_Signal_Quality();
				break;
			case '2':
				result = TestJKSE0_State();
				break;
			case '3':
				result = Packet_Parameter();
				break;

			case '4':
				result =  Chirp_Timing();
				break;

			case '5':
				result =  Suspend_Resume();
				break;
			case 'x' : //X. Exit USB Test Mode Back to Main Menu
			case 'X' :
				// Back to default
				printf("Back to Mainmenu ..........\n");
				return result;
			default :
				Display_TestModeMenu();
				break;
		}
		printf(".......USB Test_Mode Item (%c)\n", testItem);
		if (result)
			printf("fail\n");
		printf("press Enter to continue ...\n");
		//getchar();
		getc();
	}
	return 0;
}

int EHCI_Base_TestItem(void)
{
	int		result;	
	char		testItem;

		
	printf("=============BASE TEST ================\n");
	while(1)
	{
		Display_EHCIBASETestMenu();
		result = 0;
		printf("EHCI Base Test>");
		testItem = getchar();
		switch(testItem)
		{
			case '1':
				printf("\n=====Default Values Check=========================");	
				// TD1.4
				EHCI_Reset();
				ValidateRegs();
			break;
			case '2':
				printf("\n=====Test Invalid Port Enable =========================");					
				//TD2.4
				EHCI_Reset();
				writew(readw(PORTSC(ehci_base))| PORT_PE, PORTSC(ehci_base));

				if(readw(PORTSC(ehci_base)) & PORT_PE)
					printf("Fail\n");
			break;
			case '3':
				printf("\n=====Test Port Disable =========================");					
				//TD2.5 device connect
				writew(readw(PORTSC(ehci_base))& ~PORT_PE, PORTSC(ehci_base));
				if(readw(PORTSC(ehci_base)) & PORT_PE)
					printf("Fail\n");
				break;
			case '4':
				ehci_suspend();
				break;
			case '5':
				ehci_resume();
				break;
				
			case '6':	//Read Frameindex register at the end of every microframe
			{
				BYTE	i,FrameTimerInterval = 125;   //125microSec

				//FRINDEX(ehci_base)  register  is incremented
				printf("\n==========================================================");	
				for (i=0;i<25;i++)
				{
					//index = readw(FRINDEX(ehci_base));
					//printf("FRINDEX(ehci_base)                 :%08x\n",readw(FRINDEX(ehci_base)));
					printf("FRINDEX(ehci_base)			  :%08x\n",readw(FRINDEX(ehci_base)/*0x180a412c*/)); // changed by vinchip.
					MSDELAY(FrameTimerInterval);
				}
				printf("==========================================================\n");
			}	
				break;
			case '7':
				EHCI_Start();
				break;
			case '8':	//EHCI_Reset
				EHCI_Reset();
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

//api_new
int EHCI_Isochronous_TestItem()
{
	
	int		result = 0;	
	char		testItem;

		
	printf("=============Iso TEST ================\n");
	while(1)
	{
		unsigned int EpInNum;
		unsigned int EpOutNum;
		unsigned int Mps;
		unsigned int Length;
		unsigned int MemNequal;
		unsigned int Loopcount;
		int x,j;
		int Error;
		PUCHAR DataInBuf;
		PUCHAR DataOutBuf;
		Display_EHCIIsoMenu();
		result = 0;
		printf("EHCI Iso Test>");
		testItem = getchar();
		MemNequal = 0;
		switch(testItem)
		{
			case 'a':	//rx
				{
				printf("Enter EpOutNum>");
				scanf("%d",&EpOutNum);
				printf("Enter EpInNum>");
				scanf("%d",&EpInNum);
				printf("Enter Mps>");
				scanf("%d",&Mps);
				//printf("Enter Length>");
				//scanf("%d",&Length);
				printf("Enter Loop count-->");
				scanf("%d",&Loopcount);
				Length = 1024;//api_new fixed length
				DataInBuf = (PUCHAR)DATA_IN_ADDR;
				DataOutBuf = (PUCHAR)DATA_OUT_ADDR;
				for(x=0;x<Loopcount;x++)
 				{
 				printf("Loop count value  :::: %d",x);
				Error = IsoLoopData(Mps,EpOutNum,EpInNum,Length,DataOutBuf,DataInBuf);
				if(!Error)
				{
					printf("Loopback failed\n");
					break;
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
						}
					}
					if(MemNequal)
						{
						printf("Memory mismatches and the location is here\n");
						printf("the value of DatainBuf is %x\n",*(DataInBuf+j));
						printf("the value of DataoutBuf is %x\n",*(DataOutBuf+j));
						printf("the value of j is %x\n",j);
						printf("loop : %d\n",x);
						}
					else
						{
						printf("\nMemory matches successfully");
						printf("loop : %d\n",x);
						}
				}
			}
				break;
				}
			case 't':	//tx
				//SendIntrData();
				break;
			case 'r':	//auto
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
		printf("\nPress Enter to continue ...");
		getchar();					
	}
	return result;
}

extern int ehci_test(int autotest)
{
	int	result = 0, choice=0;	
	char	testItem;

	autorun = autotest;
	portconnect = 0;
	TransferComplete = 0;
	Device_Hub_Speed = 2;	// default high speed
	C_Bit =0;
	ehci_status.CepMps = 0x40;
	ehci_status.CepMps_Hub = 0x40;
	//PQHEAD pQH;
	//char *SetupData;
	if(autorun != 1)
		PrintRegs();
	
	EHCI_Start();

	if(autorun != 1)
		PrintRegs();
	
	InitMemory();
	//Set_1ms_timer(10);

	QH_Init();

	QTD_Init();

	iTD_Init();//api_new change
	while (1) {

		result = 0;
		if(autorun == 1){
			switch(choice){
				case 0 :
					testItem = '3';
					break;
				case 1 :
					testItem = '4';
					break;
				default :
					return 0;
					break;
			}
			choice ++;
		}else{
			Display_EHCIMenu();

			printf("EHCI Test>");
			testItem = getchar();
		}	
		switch (testItem) 
		{
			case 'a':
				//result = EHCI_Auto_TestItem();
				break;		
			case '1':
				result = EHCI_Base_TestItem();
				break;
			case '2':
				result = EHCI_HUB_TestItem();
				break;
			case '3':
				result = EHCI_Ctrl_TestItem();				
				break;
			case '4':
				result = EHCI_Bulk_TestItem();
				break;
			case '5':
				result = EHCI_Intr_TestItem();
				break;
			case '6':
				result = EHCI_Isochronous_TestItem();
				break;
			case '7': // added by vinchip for reading registers
				PrintRegs();
				break;
			case '8':
				EHCIOHCI_Start()	;
				break;
				

				
			case 'x' : //X. Exit Memory Test Back to Main Menu
			case 'X' :
				// Back to default
				if(autorun != 1)
					printf("Back to Mainmenu ..........\n");
				return result;
			case 'z' : //z. auto test
			case 'Z' :
				autorun = 1;
				break;
			default :
				Display_EHCIMenu();
				break;
		}

		if(autorun != 1){
			printf(".......EHCI Item (%c)\n", testItem);
			printf("press Enter to continue ...");
			getchar();
		}

	}

	EHCI_Stop();
	return 0;

}

int EHCI_Auto_TestItem(void)
{
	int		result;	
	char		testItem;

	EHCI_Reset();
	
	printf("=============Auto Test ================\n");
	
	while(1)
	{
		Display_EHCIAutoTestMenu();
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

int EHCI_Ctrl_TestItem(void)
{
	int		result = 0, choice=0;	
	char		testItem;
	unsigned long PortSc1,Usbsts;

		if(autorun != 1)
			socle_ehci_show("=============Ctrl TEST ================\n");

	PortSc1=readw(PORTSC(ehci_base));
	Usbsts = readw(USBSTS(ehci_base)/*0x180a4124*/);
	printf("Wait for port Connect\n");
//	while(!portconnect);
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
		result = 0;
		if(autorun == 1){
			switch(choice){
				case 0 :
					testItem = '0';
					break;
				case 1 :
					testItem = '1';
					break;
				case 2 :
					testItem = '8';
					break;
				case 3 :
					testItem = '8';
					break;
				case 4 :
					testItem = '2';
					break;
				case 5 :
					testItem = 'x';
					break;
				default :
					return 0;
					break;
			}
			choice ++;
		}else{
			Display_EHCICtrlMenu();
			printf("EHCI Ctrl Test>");
			testItem = getchar();
		}
		switch(testItem)
		{
			case '0':
				ResetDevice();
				break;
			case '1':	//SetAddress
				{
					int	Address;
					if(autorun == 1)
						Address = 2;
					else{
						printf("Set Address >");
						scanf("%d",&Address);
						printf("Set Address %02x\n",Address);
					}
					SetAddress((BYTE)Address);
				}
				break;
			case '2': //SetConfig
				{
					int DevConfig;
					if(autorun == 1)
						DevConfig = 1;
					else{
						printf("Set Config (0~2)>");
						scanf("%d",&DevConfig);
						printf("Set Config %02x\n",DevConfig);
					}
					SetConfiguration(DevConfig);
				}
				break;
			case '3': //GetConfig
				{
					GetConfiguration();
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
					SetFeature(DevRequest,DevFeature,DevDescIndex,EpDir);
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
					ClearFeature(DevRequest,DevFeature,DevDescIndex,EpDir);
				}
				break;							
			case '6': //SetInterface
				{
					int 	DevIface;
					printf("Old Interface:%02x ,Set New DevIface >",ehci_status.DevIface);
					scanf("%d",&DevIface);
					printf("Set DevIface %02x\n",DevIface);
					SetInterface(DevIface);
				}
				break;			
			case '7':	//GetInterface
				{
					int	DevIface;
					printf("Interface Number >");
					scanf("%d",&DevIface);
					printf("Get Interface %02x\n",DevIface);
					GetInterface(DevIface);
				}
				break;
			case '8':	//GetDescriptor
				{
					int	Index,length;					
					int 	DevDescType;
					if(autorun == 1)
						DevDescType = 1;
					else{
						printf("1:DEVICE\n");
						printf("2:CONFIGURATION\n");
						printf("3:STRING\n");
						printf("4:INTERFACE\n");
						printf("5:ENDPOINT\n");
						printf("6:DEVICE_QUALIFIER\n");
						printf("7:OTHER_SPEED_CONFIGURATION\n");
						printf("8:INTERFACE_POWER\n");
						printf("Set Desc Type >");
						scanf("%d",&DevDescType);
					}
					if(DevDescType>8)
					{
						printf("Error ");
						DevDescType =  1;
					}
					if(autorun == 1){
						Index = 0;
						if(choice == 3)
							length = 8;
						else
							length = 18;
					}else{
						printf("Set Index >");				
						scanf("%d",&Index);
						printf("Set Length >");								
						scanf("%d",&length);				
						printf("Set Type:%02x,Index:%02x,Length:%02x\n",DevDescType,Index,length);
					}
					GetDescriptor(DevDescType,Index,length);
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
					scanf("%x",&DevDescIndex);
					GetStatus(DevRequest,DevDescIndex);
				}
				break;
			case 'a':
				PrintRegs();
				break;
						
			case 'x' : //X. Exit 
			case 'X' :
				// Back to default
				if(autorun != 1)
					printf("Bye Bye ..........\n");
				return result;							
			default:
				break;
		}
		if(autorun != 1){
			printf("press Enter to continue ...");
			getchar();					
		}
	}
	return result;
}

int EHCI_Bulk_TestItem()
{
	int		result, choice=0;	
	char		testItem;
	unsigned int Mps;
	unsigned int EpNum;
	unsigned int Length;
	unsigned int Loopcount;
	unsigned int EpOutNum;
	unsigned int EpInNum;
	unsigned int NoPac;
	unsigned int TdMaxbufLength;
	unsigned int TdBufLength;
	unsigned int i;
	unsigned int j;
	unsigned int k;
	unsigned int l;
	PUCHAR  DataInBuf;
	PUCHAR  DataOutBuf;
	int Error;
	int MemNequal;
	unsigned int	UnderHub;
	UnderHub = 0;

		
	if(autorun != 1)
		printf("=============Bulk TEST ================\n");
	result = 0;
	while(1)
	{
	if(autorun == 1){
		if(choice == 0)
			testItem = 'a';
		else
			return 0;
		choice ++;
	}else{
		Display_EHCIBulkMenu();
		printf("EHCI Bulk Test>");
		testItem = getchar();
	}
		switch(testItem)
		{
			case 'r':	//rx
				printf("Enter EpNum>");
				scanf("%d",&EpNum);
				printf("Enter Mps>");
				scanf("%d",&Mps);
				printf("Enter Length>");
				scanf("%d",&Length);
				ReceiveBulkData(Mps,EpNum,Length);
				break;
			case 't':	//tx
				printf("Enter EpNum>");
				scanf("%d",&EpNum);
				printf("Enter Mps>");
				scanf("%d",&Mps);
				printf("Enter Length>");
				scanf("%d",&Length);
				SendBulkData(Mps,EpNum,Length);
				break;
			case 'a':	//auto
			if(autorun == 1){
				EpOutNum = 1; 
				EpInNum = 2;
				Mps = 512;
				Length = 65536;
				Loopcount = 1000;
				UnderHub = 0;
			}else{
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
				printf("for a device under hub ?\n");
				printf("0.No\n");
				printf("1.Yes\n");
				scanf("%d",&UnderHub);
			}
				for(l = 0;l<Loopcount;l++)
				{					
					printf("loop %d\n",l);
					DataInBuf = (PUCHAR)DATA_IN_ADDR;
					DataOutBuf = (PUCHAR)DATA_OUT_ADDR;
					for(k=0;k<Length;k++)
					{
						*(DataOutBuf+k) = k;
					}
					memset((UCHAR *)DataInBuf,0,Length);
					TdBufLength = Length;
					NoPac = Length/20480;
					TdMaxbufLength = 20480;
					if(!NoPac)
					{
						Error = BulkLoopData(Mps,EpOutNum,EpInNum,Length,DataOutBuf,DataInBuf,UnderHub);
						if(!Error)
						{
							printf("Loopback failed\n");
							TdBufLength = 0;
						}
						else
						{
							socle_ehci_show("This loopback passed\n");
							for(j=0;j<Length;j++)
							{
								if(*(DataInBuf+j) == *(DataOutBuf+j))
								{
									MemNequal = 0;
								}
								else
								{
									MemNequal = 1;
									socle_ehci_show("the value of DatainBuf is %x\n",*(DataInBuf+j));
									socle_ehci_show("the value of DataoutBuf is %x\n",*(DataOutBuf+j));
									socle_ehci_show("the value of j is %x\n",j);
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
								socle_ehci_show("Memory matches correctly\n");
								TdBufLength = 0;
							}
						}
					}
					for(i=0;i<NoPac;i++)
					{
						socle_ehci_show("the value of i is %d\n",i);
						Error = BulkLoopData(Mps,EpOutNum,EpInNum,TdMaxbufLength,DataOutBuf,DataInBuf,UnderHub);
						if(!Error)
						{
							printf("Loopback failed\n");
							TdBufLength = 0;
							break;
						}
						else
						{
							socle_ehci_show("This loopback passed\n");
							for(j=0;j<20480;j++)
							{
								if(*(DataInBuf+j) == *(DataOutBuf+j))
								{
									MemNequal = 0;
								}
								else
								{
									socle_ehci_show("the value of DatainBuf is %x\n",*(DataInBuf+j));
									socle_ehci_show("the value of DataoutBuf is %x\n",*(DataOutBuf+j));
									socle_ehci_show("the value of j is %x\n",j);
									MemNequal = 1;
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
								socle_ehci_show("memory matches correctly\n");
								TdBufLength = TdBufLength - 20480;
								DataInBuf = (PUCHAR)((PUCHAR)DataInBuf + 20480);
								DataOutBuf = (PUCHAR)((PUCHAR)DataOutBuf + 20480);
							}
						}
					}
					if(TdBufLength != 0)
					{
						Error = BulkLoopData(Mps,EpOutNum,EpInNum,TdBufLength,DataOutBuf,DataInBuf,UnderHub);
						if(!Error)
						{
							printf("Loopback failed\n");
							TdBufLength = 0;
						}
						else
						{
							socle_ehci_show("This loopback passed\n");
							for(j=0;j<TdBufLength;j++)
							{
								if(*(DataInBuf+j) == *(DataOutBuf+j))
								{
									MemNequal = 0;
								}
								else
								{
									MemNequal = 1;
									socle_ehci_show("the value of DatainBuf is %x\n",*(DataInBuf+j));
									socle_ehci_show("the value of DataoutBuf is %x\n",*(DataOutBuf+j));
									socle_ehci_show("the value of j is %x\n",j);
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
								socle_ehci_show("memory matches correctly\n");
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
				if(autorun != 1)
					printf("Back to EHCI mainmenu ..........\n");
				return result;							
				break;
			default:
				break;
		}
		if(autorun != 1){
			printf("press Enter to continue ...");
			getchar();					
		}
	}
	
}

int EHCI_Intr_TestItem()
{
	int		result = 0;	
	char		testItem;

		
	printf("=============Interrupt  TEST ================\n");
	while(1)
	{
		unsigned int EpInNum;
		unsigned int Mps;
		unsigned int Length;
		Display_EHCIIntrMenu();
		result = 0;
		printf("EHCI Intr Test>");
		testItem = getchar();
		switch(testItem)
		{
			case 'r':	//rx
				printf("Enter EpInNum>\n");
				scanf("%d",&EpInNum);
				printf("Enter Mps>\n");
				scanf("%d",&Mps);
				printf("Enter Length>\n");
				scanf("%d",&Length);
				ReceiveIntrData(Mps,EpInNum,Length);
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
	return result;
}

int EHCI_HUB_TestItem()
{
	int		result=0;	
	char		testItem;

	printf("=============Hub TEST ================\n");
	while(1)
	{
		Display_EHCIHubMenu();
		result = 0;
		printf("Hub Request Test>");
		testItem = getchar();
		switch(testItem)
		{
		
			case '1':	//GetHubDescriptor
				{
					int DevDescIndex, DevDescLength, DescType;
					printf("Set Desc Type >");
					scanf("%d",&DescType);
					printf("Set DevDescIndex >");
					scanf("%d",&DevDescIndex);
					printf("Set DevDescLength >");
					scanf("%d",&DevDescLength);
					printf("Set DevDescIndex:%02x, DevDescLength=%02x",DevDescIndex, DevDescLength);
					GetHubDescriptor(DescType,DevDescIndex,DevDescLength);
				}
				break;
			case '2':	//GetPortStatus
				{
					int DevDescIndex;
					printf("Set Port No >");
					scanf("%d",&DevDescIndex);	
					GetPortStatus(DevDescIndex);
				}
				break;
			case '3':	//GetHubStatus
					GetHubStatus();
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
					ClearPortFeature(DevDescValue,DevDescIndex);
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
					ClearHubFeature(DevDescIndex);
				}
				break;			
			case '6'://ResetTT
				{
					unsigned int TTPort;
					if(MultipleTT)
					{
						printf("Enter TTPort>");
						scanf("%d",&TTPort);
					}
					else
					{
						TTPort = 1;
					}
					ResetTT(TTPort);					
				}				
				break;
			case '7'://StopTT
				{
					unsigned int TTPort;
					if(MultipleTT)
					{
						printf("Enter TTPort>");
						scanf("%d",&TTPort);
					}
					else
					{
						TTPort = 1;
					}
					StopTT(TTPort);
				}
				break;
			case '8'://SetHubFeature
				{
					int DevDescValue;
					printf("0.C_HUB_LOCAL_POWER");
					printf("1.C_HUB_OVER_CURRENT");
					scanf("%d",&DevDescValue);
					SetHubFeature(DevDescValue);					
				}
				break;
			case '9'://SetPortFeature
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
					ehci_status.Portnum = DevDescIndex;
					SetPortFeature(DevDescValue,DevDescIndex);
				}
				break;
			case 'a'://ClearTTBuffer
				{
					unsigned int DevAddr;
					unsigned int Epnum;
					unsigned int EpType;
					unsigned int Epdir;
					unsigned short Epchar;
					unsigned int TTPort;
					printf("Enter DevAddr(Beneath Hub)>");
					scanf("%d",&DevAddr);
					printf("Enter Epnum(Beneath Hub)>");
					scanf("%d",&Epnum);
					printf("Enter EpType(Beneath Hub)>");
					scanf("%d",&EpType);
					printf("Enter Epdir(Beneath Hub)>");
					scanf("%d",&Epdir);
					Epchar = (Epdir << 15) |(EpType << 11)|(DevAddr << 4)|Epnum;
					printf("Epchar is %x\n",Epchar);
					if(MultipleTT)
					{
						printf("Enter TTPort>");
						scanf("%d",&TTPort);
					}
					else
					{
						TTPort = 1;
					}
					ClearTTBuffer(Epchar, TTPort);					
				}						
				break;
			case 'b'://SetHubDescriptor
				printf("no implement");
				break;
			case 'c'://GetTTState
 		 	{
				unsigned int TTPort;

				if(MultipleTT)
				{ 
				 	printf("Enter TT Port>");
					scanf("%d",&TTPort);
			    	}
				else
					{
					TTPort = 1;
					}
				GetTTState(TTPort);
			//printf("no implement");
				}
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
	return result;
}
