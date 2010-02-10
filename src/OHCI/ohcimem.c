#include "ohcictrl.h"
#include "global.h"


#define EDADDR 				(TEST_PATTERN_START + 0x1000 + 0x10000 + 0x10000 + 0x5000 + 0x5000) //0x00400200		//0x30*5= 0xf0		// changed from 0x80400200 to 0xA0400200
#define GTDADDR 			(TEST_PATTERN_START + 0x1000 + 0x10000 + 0x10000 + 0x5000 + 0x5000 + 0x1000) //0x00400300		//0x20*5= 0xa0	// changed from 0x80400300 to 0xA0400300
#define ISOTDADDR 			(TEST_PATTERN_START + 0x1000 + 0x10000 + 0x10000 + 0x5000 + 0x5000 + 0x1000 + 0x5000) 	//0x00400800		


//periodic_size

//free ED,GTD
PED 		Root_ED,End_ED;
PGENTD 		Root_GTD,End_GTD;
//shoadow qh,qtd period
PED 		Root_Period_ED,End_Period_ED;
PGENTD 		Root_Period_GTD,End_Period_GTD;
//shoadow qh,qtd asynchronous
PED 		Root_Asyn_ED,End_Asyn_ED;
PGENTD 		Root_Asyn_GTD,End_Asyn_GTD;

PISOTD	Root_IsoTD,End_IsoTD;

BYTE 		free_ED_size,free_GTD_size,free_ISOTD_size;
BYTE		shadow_Period_no,shadow_Asychronous_no;
/*-------------------------------------------------------------------------*/

void Add_Free_ED_Ring(PED freeEd)
{
	memset((char *)freeEd,0,sizeof(PED));
	if(free_ED_size == 0)
	{
		Root_ED = freeEd;
		freeEd ->NextED = (ULONG)Root_ED;		// to be checked..whether circular list is required
		End_ED = freeEd;
	}
	else
	{
		End_ED->NextED = (ULONG)freeEd;
		freeEd->NextED = (ULONG)Root_ED;
		End_ED = freeEd;
	}
	free_ED_size++;
}

PED Get_Free_ED()
{
	PED pED;
	if(free_ED_size>0)
	{
		
		pED = Root_ED;
		Root_ED = (PED)(Root_ED->NextED);
		End_ED->NextED= (ULONG)Root_ED;
		free_ED_size--;
		return pED;
		//return true;
	}
	else
	{
		printf("Get_Free_ED NULL \n");
		return NULL;
	}
}

void ED_Init()
{	// 5 free ED
	PED freeED;
	int i;
	//printf("sizeof QTD = %02x",sizeof(QHEAD));	
	//freeQH=(PQHEAD) malloc(5*sizeof(QHEAD));
	//memset(freeQH,1,5*sizeof(QHEAD));
	free_ED_size = 0;
	for(i=0;i<5;i++)
	{
		freeED = (PED)(EDADDR + i*sizeof(ED));
		//memset(freeQH,0x5a,sizeof(QHEAD));
		Add_Free_ED_Ring(freeED);
	}
}
void Add_Free_GTD_Ring(PGENTD freeGTD)
{
	memset((char *)freeGTD,0,sizeof(PGENTD));
	if(free_GTD_size == 0)
	{
		Root_GTD = freeGTD;
		freeGTD ->NextTD == (ULONG)Root_GTD;
		End_GTD = freeGTD;
	}
	else
	{
		End_GTD->NextTD = (ULONG)freeGTD;
		freeGTD->NextTD = (ULONG)Root_GTD;
		End_GTD = freeGTD;
	}
	
	free_GTD_size++;
}

PGENTD Get_Free_GTD()
{
	PGENTD pGtd;
	if(free_GTD_size>0)
	{
		pGtd = Root_GTD;
		Root_GTD= Root_GTD->NextTD;	
		End_GTD->NextTD = Root_GTD;
		free_GTD_size--;
		return pGtd;
	}
	else
	{
		printf("Get_Free_GTD NULL\n");
		return NULL;
	}

}

void GTD_Init()
{	//	5 free GTD
	PGENTD	freeGTD;
	int 		i;

	//printf("sizeof QTD = %02x",sizeof(QTD));
	//freeQTD=(PQTD) malloc(5*sizeof(QTD));
	
	//memset(QTDADDR,0,10*sizeof(QTD));
	free_GTD_size = 0;
	//init free QTD list
	for(i=0;i<5;i++)
	{
		freeGTD = (PGENTD)(GTDADDR +i*sizeof(GENTD));
		//memset(freeQTD,0,sizeof(QTD));
		Add_Free_GTD_Ring(freeGTD);
	}
}
void Add_Free_ISOTD_Ring(PISOTD freeISOTD)
{
	memset(freeISOTD,0,sizeof(PISOTD));
	if(free_ISOTD_size == 0)
	{
		Root_IsoTD = freeISOTD;
		freeISOTD ->NextTD == (ULONG)Root_IsoTD;
		End_IsoTD = freeISOTD;
	}
	else
	{
		End_IsoTD->NextTD = (ULONG)freeISOTD;
		freeISOTD->NextTD = (ULONG)Root_IsoTD;
		End_IsoTD = freeISOTD;
	}
	
	free_ISOTD_size++;
}

PISOTD Get_Free_ISOTD()
{
	PISOTD pIsotd;
	if(free_ISOTD_size>0)
	{
		pIsotd = Root_IsoTD;
		Root_IsoTD= Root_IsoTD->NextTD;	
		End_IsoTD->NextTD = Root_IsoTD;
		free_ISOTD_size--;
		return pIsotd;
	}
	else
	{
		printf("Get_Free_IsoTD NULL\n");
		return NULL;
	}
}

// changes for iso td processing.
void IsoTd_init()
{
	PISOTD	freeIsoTD;
	int 		i;
	free_ISOTD_size = 0;
	//init free QTD list
	for(i=0;i<5;i++)
	{
		freeIsoTD = (PISOTD)(ISOTDADDR +i*sizeof(ISOTD));
		Add_Free_ISOTD_Ring(freeIsoTD);
	}
}

/*
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
*/


BYTE GetFreeEDSize()
{
	return free_ED_size;
}

