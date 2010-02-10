/*
 * Copyright (c) 2001 by David Brownell
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* this file is part of ehci-hcd.c */

/*-------------------------------------------------------------------------*/

/*
 * There's basically three types of memory:
 *	- data used only by the HCD ... kmalloc is fine
 *	- async and periodic schedules, shared by HC and HCD ... these
 *	  need to use pci_pool or pci_alloc_consistent
 *	- driver buffers, read/written by HC ... single shot DMA mapped 
 *
 * There's also PCI "register" data, which is memory mapped.
 * No memory seen by this driver is pageable.
 */

/*-------------------------------------------------------------------------*/
/* 
 * Allocator / cleanup for the per device structure
 * Called by hcd init / removal code
 */
#include "ehcictrl.h"

//periodic_size

/*-------------------------------------------------------------------------*/


PQHEAD 		Root_QH,End_QH;
PQTD 		Root_QTD,End_QTD;
PQHEAD 		Root_Asyn_QH,End_Asyn_QH;
PiTD		Root_iTD,End_iTD;//api_new
BYTE		free_iTD_size;//api_new
BYTE 		free_qh_size,free_qtd_size;
BYTE 		ErrorComplete;
BYTE 		MultipleTT;
BYTE		C_Bit;
BYTE		Device_Hub_Speed;


void Add_Free_QH_Ring(PQHEAD freeQH)
{
	memset(freeQH,0,sizeof(QHEAD));
	if(free_qh_size == 0)
	{
		Root_QH = freeQH;
		freeQH ->QHEADLinkPtr = (ULONG)Root_QH;
		End_QH = freeQH;
	}
	else
	{
		End_QH->QHEADLinkPtr = (ULONG)freeQH;
		freeQH->QHEADLinkPtr = (ULONG)Root_QH;
		End_QH = freeQH;
	}
	free_qh_size++;
}

PQHEAD Get_Free_QH()
{
	PQHEAD pQH;
	if(free_qh_size>0)
	{
		
		pQH = Root_QH;
		Root_QH = (PQHEAD)(Root_QH->QHEADLinkPtr);
		End_QH->QHEADLinkPtr = Root_QH;
		free_qh_size--;
		return pQH;
		//return true;
	}
	else
	{
		printf("Get_Free_QH NULL \n");
		return NULL;
	}
}

void MountAsynchronousFun(PQHEAD pQH)
{
	if (Root_Asyn_QH == NULL) 
	{
	        //HsmMacRegWriteFun(MAC_DCFC,DCFC_DISABLE);
	        Root_Asyn_QH = pQH;
	        //HsmMacRegWriteFun(MAC_DCFDP,pTxDesc->PhysicalAddress);
	} 
	else 
	{
		 End_Asyn_QH->QHEADLinkPtr = pQH;
		 pQH ->QHEADLinkPtr = Root_Asyn_QH;
	        End_Asyn_QH = pQH;
	}
}

void Add_Free_QTD_Ring(PQTD freeQTD)
{
	memset(freeQTD,0,sizeof(QTD));
	if(free_qtd_size == 0)
	{
		Root_QTD = freeQTD;
		freeQTD ->NextQTD == (ULONG)Root_QTD;
		freeQTD ->AltQTD == (ULONG)Root_QTD;
		End_QTD = freeQTD;
	}
	else
	{
		End_QTD->NextQTD = (ULONG)freeQTD;
		End_QTD->AltQTD = (ULONG)freeQTD;
		freeQTD->NextQTD = (ULONG)Root_QTD;
		freeQTD->AltQTD = (ULONG)Root_QTD;
		End_QTD = freeQTD;
	}
	
	free_qtd_size++;
}

PQTD Get_Free_QTD()
{
	PQTD pQtd;
	if(free_qtd_size>0)
	{
		pQtd = Root_QTD;
		Root_QTD= Root_QTD->NextQTD;	
		End_QTD->NextQTD = Root_QTD;
		free_qtd_size--;
		return pQtd;
	}
	else
	{
		printf("Get_Free_QTD NULL\n");
		return NULL;
	}

}

BYTE GetFreeQHSize()
{
	return free_qh_size;
}




void Add_Free_iTD_Ring(PiTD freeiTD)
{
	memset(freeiTD,0,sizeof(iTD));
	if(free_iTD_size == 0)
	{
		Root_iTD = freeiTD;
		freeiTD->iTDNextLinkPtr = (ULONG)Root_iTD;
		End_iTD = freeiTD;
	}
	else
	{
		End_iTD->iTDNextLinkPtr = (ULONG)freeiTD;
		freeiTD->iTDNextLinkPtr = (ULONG)Root_iTD;
		End_iTD = freeiTD;
	}
	free_iTD_size++;
}

PiTD Get_Free_iTD()
{
	PiTD piTD;
	if(free_iTD_size>0)
	{
		
		piTD = Root_iTD;
		Root_iTD = (PiTD)(Root_iTD->iTDNextLinkPtr);
		End_iTD->iTDNextLinkPtr = Root_iTD;
		free_iTD_size--;
		return piTD;
		//return true;
	}
	else
	{
		printf("Get_Free_iTD NULL \n");
		return NULL;
	}
}

void QH_Init()
{	// 5 free QH
	PQHEAD freeQH;
	int i;
	//printf("sizeof QTD = %02x",sizeof(QHEAD));	
	//freeQH=(PQHEAD) malloc(5*sizeof(QHEAD));
	//memset(freeQH,1,5*sizeof(QHEAD));
	free_qh_size = 0;
	for(i=0;i<5;i++)
	{
//		freeQH = QHADDR + i*sizeof(QHEAD);
		freeQH = QHADDR + i*(64*sizeof(unsigned char));
		Add_Free_QH_Ring(freeQH);
	}
}

void QTD_Init()
{	//	10 free QTD
	PQTD	freeQTD;
	int 		i;

	//printf("sizeof QTD = %02x",sizeof(QTD));
	//freeQTD=(PQTD) malloc(5*sizeof(QTD));
	
	//memset(QTDADDR,0,10*sizeof(QTD));
	free_qtd_size = 0;
	//init free QTD list
	for(i=0;i<5;i++)
	{
		freeQTD = QTDADDR +i*sizeof(QTD);
		//memset(freeQTD,0,sizeof(QTD));
		Add_Free_QTD_Ring(freeQTD);
	}
}

void iTD_Init()
{	// 5 free QH
	PiTD freeiTD;
	int i;
	free_iTD_size = 0;
	for(i=0;i<5;i++)
	{
		freeiTD = iTDADDR + i*(64*sizeof(unsigned char));
		Add_Free_iTD_Ring(freeiTD);
	}
}

BYTE GetFreeiTDSize()
{
	return free_iTD_size;
}




