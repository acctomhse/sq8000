/* if_socle.c - SOCLE AHB MAC IP - Ethernet LAN network interface driver */

/* Copyright SOCLE-Tech. */

/*
modification history
--------------------
01a,22sep03,Sherlock  First Written.
*/
//#include "global.h"
#include "genlib.h"
#include "../Interrupt/interrupt.h"
#include "if_socle.h"			/* device description header */
#include "sMACdev.h"			/* device struct */
#include "macif.h"

#ifdef SMAC_DEBUG
#endif


//#define PHY_TO_KNONCACHE(phyAddr)	(phyAddr)

#ifndef DELAY
#define DELAY(count)	{                                   \
			volatile int cx = 0;                            \
			for (cx = 0; cx < (count); cx++);               \
			}
#endif	/* DELAY */

#ifndef USDELAY
#define USDELAY(usec)	{                                   \
			volatile int ux = 0;                            \
			volatile int ux_loop = (int)(usec*LOOP_PER_US); \
			for (ux = 0; ux < ux_loop; ux++);               \
			}
#endif /* USDELAY */

#ifndef MSDELAY
#define MSDELAY(msec)	{                                   \
			volatile int mx = 0;                            \
			volatile int mx_loop = (int)(msec);    			\
			for (mx = 0; mx < mx_loop; mx++)                \
			{												\
				USDELAY(1000);								\
			}												\
			}
#endif /* MSDELAY */



#ifdef  SMAC_INTCOUNTER_DEBUG
//Enable all mask
#define	SMAC_INTMASK	(0	    				\
							|	CSR7_NIS		\
							|	CSR7_AIS		\
							|	CSR7_ERI		\
							|	CSR7_GPTE		\
							|	CSR7_ETI		\
							|	CSR7_RPS		\
							|	CSR7_RU			\
							|	CSR7_RI			\
							|	CSR7_UNF		\
							|	CSR7_TU			\
							|	CSR7_TPS		\
							|	CSR7_TI			\
						)
#else
//normal condition
// Error Sherlock TU change to TI
#define	SMAC_INTMASK	(0	    				\
							|	CSR7_NIS		\
							|	CSR7_AIS		\
							|	CSR7_RU			\
							|	CSR7_RI			\
							|	CSR7_UNF		\
							|	CSR7_TU			\
						)
#endif


/*
 * Device Structure Operation
 */
#define SMAC_TD_INDEX_NEXT(pDrvCtrl)									\
	(((pDrvCtrl)->txIndex + 1) % (pDrvCtrl)->NumTds)

#define SMAC_TD_DINDEX_NEXT(pDrvCtrl)									\
	(((pDrvCtrl)->txDiIndex + 1) % (pDrvCtrl)->NumTds)

#define SMAC_RD_INDEX_NEXT(pDrvCtrl)									\
	(((pDrvCtrl)->rxIndex + 1) % (pDrvCtrl)->NumRds)

#if (defined MAC_DES_BOTHMODE)
	#if	(defined MAC_DES_RINGSKIP)
		#define SMAC_GET_TDE_RING(xxx,pDrvCtrl)									\
			xxx = ( (SMAC_TDE *)( ((u8_t *)(pDrvCtrl->txRing))	\
			+ pDrvCtrl->txIndex*(sizeof(SMAC_TDE)+pDrvCtrl->DesSkipSize) ) )

		#define SMAC_GET_DTDE_RING(xxx,pDrvCtrl)								\
			xxx = ( (SMAC_TDE *)( ((u8_t *)(pDrvCtrl->txRing))	\
			+ pDrvCtrl->txDiIndex*(sizeof(SMAC_TDE)+pDrvCtrl->DesSkipSize) ) )

		#define SMAC_GET_RDE_RING(xxx,pDrvCtrl)									\
			xxx = ( (SMAC_RDE *)( ((u8_t *)(pDrvCtrl->rxRing))	\
			+ pDrvCtrl->rxIndex*(sizeof(SMAC_RDE)+pDrvCtrl->DesSkipSize) ) )
	#else
		#define SMAC_GET_TDE_RING(xxx,pDrvCtrl)									\
			xxx = ( (SMAC_TDE *)( ((u8_t *)(pDrvCtrl->txRing))	\
			+ pDrvCtrl->txIndex*(sizeof(SMAC_TDE)) ) )

		#define SMAC_GET_DTDE_RING(xxx,pDrvCtrl)								\
			xxx = ( (SMAC_TDE *)( ((u8_t *)(pDrvCtrl->txRing))	\
			+ pDrvCtrl->txDiIndex*(sizeof(SMAC_TDE)) ) )

		#define SMAC_GET_RDE_RING(xxx,pDrvCtrl)									\
			xxx = ( (SMAC_RDE *)( ((u8_t *)(pDrvCtrl->rxRing))	\
			+ pDrvCtrl->rxIndex*(sizeof(SMAC_RDE)) ) )
	#endif
	#define SMAC_GET_TDE_CHAIN(xxx,pDrvCtrl)								\
		{																	\
			int	ichain;														\
			xxx=(SMAC_TDE *)(pDrvCtrl->txRing);             \
			for(ichain=0;ichain<pDrvCtrl->txIndex;ichain++)                 \
			{                                                               \
				xxx=(SMAC_TDE *)(xxx->tDesc3);				\
			}                                                               \
		}

	#define SMAC_GET_DTDE_CHAIN(xxx,pDrvCtrl)								\
		{																	\
			int	ichain;														\
			xxx=(SMAC_TDE *)(pDrvCtrl->txRing);             \
			for(ichain=0;ichain<pDrvCtrl->txDiIndex;ichain++)               \
			{                                                               \
				xxx=(SMAC_TDE *)(xxx->tDesc3);				\
			}                                                               \
		}

	#define SMAC_GET_RDE_CHAIN(xxx,pDrvCtrl)								\
		{																	\
			int	ichain;														\
			xxx=(SMAC_RDE *)(pDrvCtrl->rxRing);             \
			for(ichain=0;ichain<pDrvCtrl->rxIndex;ichain++)                 \
			{                                                               \
				xxx=(SMAC_RDE *)(xxx->rDesc3);				\
			}                                                               \
		}

	#define	SMAC_GET_TDE(xxx,pDrvCtrl)										\
		if(pDrvCtrl->DesChainMode)											\
		{																	\
			SMAC_GET_TDE_CHAIN(xxx,pDrvCtrl);						        \
		}                                                                   \
		else                                                                \
		{                                                                   \
			SMAC_GET_TDE_RING(xxx,pDrvCtrl);                                \
		}

	#define	SMAC_GET_DTDE(xxx,pDrvCtrl)										\
		if(pDrvCtrl->DesChainMode)											\
		{																	\
			SMAC_GET_DTDE_CHAIN(xxx,pDrvCtrl);						        \
		}                                                                   \
		else                                                                \
		{                                                                   \
			SMAC_GET_DTDE_RING(xxx,pDrvCtrl);                               \
		}

	#define	SMAC_GET_RDE(xxx,pDrvCtrl)										\
		if(pDrvCtrl->DesChainMode)											\
		{																	\
			SMAC_GET_RDE_CHAIN(xxx,pDrvCtrl);						        \
		}                                                                   \
		else                                                                \
		{                                                                   \
			SMAC_GET_RDE_RING(xxx,pDrvCtrl);                                \
		}

#else
#if	(defined MAC_DES_CHAINMODE)
	#define SMAC_GET_TDE_CHAIN(xxx,pDrvCtrl)								\
		{																	\
			int	ichain;														\
			xxx=(SMAC_TDE *)(pDrvCtrl->txRing);                                           \
			for(ichain=0;ichain<pDrvCtrl->txIndex;ichain++)                 \
			{                                                               \
				xxx=(SMAC_TDE *)(xxx->tDesc3);				\
			}                                                               \
		}

	#define SMAC_GET_DTDE_CHAIN(xxx,pDrvCtrl)								\
		{																	\
			int	ichain;														\
			xxx=(SMAC_TDE *)(pDrvCtrl->txRing);                                           \
			for(ichain=0;ichain<pDrvCtrl->txDiIndex;ichain++)               \
			{                                                               \
				xxx=(SMAC_TDE *)(xxx->tDesc3);				\
			}                                                               \
		}

	#define SMAC_GET_RDE_CHAIN(xxx,pDrvCtrl)								\
		{																	\
			int	ichain;														\
			xxx=(SMAC_RDE *)(pDrvCtrl->rxRing);                                           \
			for(ichain=0;ichain<pDrvCtrl->rxIndex;ichain++)                 \
			{                                                               \
				xxx=(SMAC_RDE *)(xxx->rDesc3);				\
			}                                                               \
		}

	#define	SMAC_GET_TDE(xxx,pDrvCtrl)	SMAC_GET_TDE_CHAIN(xxx,pDrvCtrl)
	#define	SMAC_GET_DTDE(xxx,pDrvCtrl)	SMAC_GET_DTDE_CHAIN(xxx,pDrvCtrl)
	#define	SMAC_GET_RDE(xxx,pDrvCtrl)	SMAC_GET_RDE_CHAIN(xxx,pDrvCtrl)
#else
	//Default Ring Mode
	#if	(defined MAC_DES_RINGSKIP)
		#define SMAC_GET_TDE_RING(xxx,pDrvCtrl)									\
			xxx = ( (SMAC_TDE *)( ((u8_t *)(pDrvCtrl->txRing))	\
			+ pDrvCtrl->txIndex*(sizeof(SMAC_TDE)+pDrvCtrl->DesSkipSize) ) )

		#define SMAC_GET_DTDE_RING(xxx,pDrvCtrl)								\
			xxx = ( (SMAC_TDE *)( ((u8_t *)(pDrvCtrl->txRing))	\
			+ pDrvCtrl->txDiIndex*(sizeof(SMAC_TDE)+pDrvCtrl->DesSkipSize) ) )

		#define SMAC_GET_RDE_RING(xxx,pDrvCtrl)									\
			xxx = ( (SMAC_RDE *)( ((u8_t *)(pDrvCtrl->rxRing))	\
			+ pDrvCtrl->rxIndex*(sizeof(SMAC_RDE)+pDrvCtrl->DesSkipSize) ) )
	#else
		#define SMAC_GET_TDE_RING(xxx,pDrvCtrl)									\
			xxx = ( (SMAC_TDE *)( ((u8_t *)(pDrvCtrl->txRing))	\
			+ pDrvCtrl->txIndex*(sizeof(SMAC_TDE)) ) )

		#define SMAC_GET_DTDE_RING(xxx,pDrvCtrl)								\
			xxx = ( (SMAC_TDE *)( ((u8_t *)(pDrvCtrl->txRing))	\
			+ pDrvCtrl->txDiIndex*(sizeof(SMAC_TDE)) ) )

		#define SMAC_GET_RDE_RING(xxx,pDrvCtrl)									\
			xxx = ( (SMAC_RDE *)( ((u8_t *)(pDrvCtrl->rxRing))	\
			+ pDrvCtrl->rxIndex*(sizeof(SMAC_RDE)) ) )
	#endif

	#define	SMAC_GET_TDE(xxx,pDrvCtrl)	SMAC_GET_TDE_RING(xxx,pDrvCtrl)
	#define	SMAC_GET_DTDE(xxx,pDrvCtrl)	SMAC_GET_DTDE_RING(xxx,pDrvCtrl)
	#define	SMAC_GET_RDE(xxx,pDrvCtrl)	SMAC_GET_RDE_RING(xxx,pDrvCtrl)
#endif
#endif


#ifdef	MAC_DATA_BUFFER_SIZEVARIABLE
#define	SMAC_GET_TXBUFFER_SIZE(bn,pDrvCtrl)										\
			( *( (int *)(&pDrvCtrl->tBuffSizePt[pDrvCtrl->txIndex])+(bn-1) ) )
#define	SMAC_GET_RXBUFFER_SIZE(bn,pDrvCtrl)										\
			( *( (int *)(&pDrvCtrl->rBuffSizePt[pDrvCtrl->rxIndex])+(bn-1) ) )
#else
#define	SMAC_GET_TXBUFFER_SIZE(bn,pDrvCtrl)		SMAC_BUFSIZ
#define	SMAC_GET_RXBUFFER_SIZE(bn,pDrvCtrl)		SMAC_BUFSIZ
#endif


/*
 * Default macro definitions for device register accesses:
 * These macros can be redefined in a wrapper file, to generate
 * a new module with an optimized interface.
 */
#define sMAC_CSR_READ(devAdrs, csrNum)                                    \
    	sMAC_READ_CSR((devAdrs), (csrNum))

#define sMAC_CSR_WRITE(devAdrs, csrNum, csrVal)                           \
    	sMAC_WRITE_CSR((devAdrs), (csrNum), (csrVal))

#define sMAC_CSR_UPDATE(devAdrs, csrNum, csrBits)                         \
    	sMAC_CSR_WRITE((devAdrs), (csrNum),                               \
                     sMAC_CSR_READ((devAdrs), (csrNum)) | (csrBits))

#define	sMAC_CSR_RESET(devAdrs, csrNum, csrBits)                          \
    	sMAC_CSR_WRITE((devAdrs), (csrNum),                               \
                     sMAC_CSR_READ((devAdrs), (csrNum)) & ~(csrBits))

#define sMAC_INT_ENABLE(devAdrs, X)                                       \
        sMAC_CSR_UPDATE ((devAdrs), CSR7, (X))

#define sMAC_INT_DISABLE(devAdrs, X)                                      \
    	sMAC_CSR_RESET ((devAdrs), CSR7, (X))


/*
 * MII read/write access macros
 */
#define	sMAC_MII_BIT_READ(devAdrs, pBData)                           		\
    	{                                                           		\
        sMAC_CSR_WRITE ((devAdrs), CSR9, CSR9_MII_RD );             		\
        USDELAY (1);                                            			\
        sMAC_CSR_WRITE ((devAdrs), CSR9, CSR9_MII_RD | CSR9_MDC);  			\
        USDELAY (1);                                                     	\
        *(pBData) |= CSR9_MII_DBIT_RD (sMAC_CSR_READ ((devAdrs), CSR9));    \
    	}

#define	sMAC_MII_BIT_WRITE(devAdrs, data)                                 	\
    	{                                                               	\
        sMAC_CSR_WRITE ((devAdrs), CSR9, CSR9_MII_DBIT_WR(data) |         	\
                      CSR9_MII_WR );                           				\
        USDELAY (1);                                                 		\
        sMAC_CSR_WRITE ((devAdrs), CSR9, CSR9_MII_DBIT_WR(data) |         	\
                      CSR9_MII_WR | CSR9_MDC);                				\
        USDELAY (1);                                                 		\
        }

#define	sMAC_MII_RTRISTATE(devAdrs)                                       	\
		{                                                               	\
        int retBData;                                                     	\
        sMAC_MII_BIT_READ ((devAdrs), &retBData);                          	\
        }

#define	sMAC_MII_WTRISTATE(devAdrs)                                       	\
		{                                                               	\
        sMAC_MII_BIT_WRITE((devAdrs), 0x1);                               	\
        sMAC_MII_BIT_WRITE((devAdrs), 0x0);                               	\
		}

#define sMAC_MII_WRITE(devAdrs, data, bitCount)                           	\
    	{                                                               	\
        int i=(bitCount);                                               	\
                                                                        	\
        while (i--)                                                    	 	\
            sMAC_MII_BIT_WRITE ((devAdrs), ((data) >> i) & 0x1);          	\
        }

#define	sMAC_MII_READ(devAdrs, pData, bitCount)                           	\
    	{                                                               	\
        int i=(bitCount);                                               	\
                                                                        	\
        while (i--)                                                     	\
            {                                                           	\
            *(pData) <<= 1;                                             	\
            sMAC_MII_BIT_READ ((devAdrs), (pData));                       	\
            }                                                           	\
        }

#define	sMAC_KICKSTART(pDrvCtrl)											\
		if(pDrvCtrl->AutoPollTx==0)											\
		{																	\
			/* xmit poll demand */											\
			sMAC_CSR_WRITE (pDrvCtrl->devAdrs, CSR1, CSR1_TPD);				\
		}

//////////////////////////////////////////////////////////////////////////////
//		Veriable Definition
//////////////////////////////////////////////////////////////////////////////

#ifdef	VERIABLE_BYTE_ORDER
int sMAC_Des_Byte_Order;
int	sMAC_DataB_Byte_Order;
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		Function Which Control the Byte Order Swap
		!!!	( This Function Only For Test Used,
			  On Normal Case, Should be Set on define)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
inline	int	DESSWAP(int x)
{
	if(sMAC_Des_Byte_Order==CPU_BYTE_ORDER) 	return(x);
	else									return(MACLONGSWAP(x));
}
inline	int	DATABSWAP(int x)
{
	if(sMAC_DataB_Byte_Order==CPU_BYTE_ORDER) 	return(x);
	else									return(MACLONGSWAP(x));
}
#endif	/* VERIABLE_BYTE_ORDER */

/*******************************************************************************
*
* sMacMiiPhyRead - read a PHY device register via MII
*
* RETURNS: the contents of a PHY device register.
*/
USHORT sMacMiiPhyRead
    (
    DRV_CTRL *	pDrvCtrl,					/* pointer to device control structure */
    UINT		phyAdrs, 					// PHY address to access
    UINT		phyReg   					// PHY register to read
    )
{
    USHORT retVal=0;
    
    /* Write 34-bit preamble */
    sMAC_MII_WRITE (pDrvCtrl->devAdrs, MII_PREAMBLE, 32);
    /* SW ISSUE Why 2 dummy ? */
    //sMAC_MII_WRITE (pDrvCtrl->devAdrs, MII_PREAMBLE, 2);

    /* start of frame + op-code nibble */
    sMAC_MII_WRITE (pDrvCtrl->devAdrs, MII_SOF | MII_RD, 4);

    /* device address */
    sMAC_MII_WRITE (pDrvCtrl->devAdrs, phyAdrs, 5);
    sMAC_MII_WRITE (pDrvCtrl->devAdrs, phyReg, 5);

    /* turn around */
    sMAC_MII_RTRISTATE (pDrvCtrl->devAdrs);

    /* read data */
    sMAC_MII_READ (pDrvCtrl->devAdrs, &retVal, 16);

	/* SW ISSUE */
	/* turn around */
    sMAC_MII_RTRISTATE (pDrvCtrl->devAdrs);

    return (retVal);
}

/*******************************************************************************
*
* sMacMiiPhyWrite - write to a PHY device register via MII
*
* RETURNS: none
*/

void sMacMiiPhyWrite
    (
    DRV_CTRL *	pDrvCtrl,					/* pointer to device control structure */
    UINT		phyAdrs,					// PHY address to access
    UINT		phyReg, 					// PHY register to write
    USHORT		data    					// Data to write
    )
{
    /* write 34-bit preamble */
    sMAC_MII_WRITE (pDrvCtrl->devAdrs, MII_PREAMBLE, 32);
    /* SW ISSUE Why 2 dummy ? */
    //sMAC_MII_WRITE (pDrvCtrl->devAdrs, MII_PREAMBLE, 2);

    /* start of frame + op-code nibble */
    sMAC_MII_WRITE (pDrvCtrl->devAdrs, MII_SOF | MII_WR, 4);

    /* device address */
    sMAC_MII_WRITE (pDrvCtrl->devAdrs, phyAdrs, 5);
    sMAC_MII_WRITE (pDrvCtrl->devAdrs, phyReg, 5);

    /* turn around */
    sMAC_MII_WTRISTATE (pDrvCtrl->devAdrs);

    /* write data */
    sMAC_MII_WRITE (pDrvCtrl->devAdrs, data, 16);
}

/*******************************************************************************
*
* sMacMiiScan - Scan MII chip get the PhyAD
* define PHY_xxx with different PHY  platform.h
*/
int sMacMiiScan
	(
    DRV_CTRL *	pDrvCtrl	/* pointer to device control structure */
    )
{
    u32_t	phyAddr;
    u32_t	phy_id0;
    u32_t	phy_id1;

	/* Find first PHY attached to Socle MAC */
	for (phyAddr = 0; phyAddr < MAC_MAX_PHY; phyAddr++){
		phy_id0 = sMacMiiPhyRead(pDrvCtrl, phyAddr, MII_PHY_ID0);
	    if (phy_id0 == PHY_ID0){ 
	    	phy_id1 = sMacMiiPhyRead(pDrvCtrl, phyAddr, MII_PHY_ID1);
	   		if (phy_id1 == PHY_ID1){ /* Found PHY */
	   			#ifdef SMAC_DEBUG
	    		printf("!!!! Scan get MII PHY at AD (0x%x)\n", phyAddr);
	    		//printf("!!!!                     ID0=0x%08x", phy_id0);
	    		//printf(" ID1=0x%08x\n", phy_id1);
				#endif
	    		break;
    		}
		}
	}
	pDrvCtrl->MII_PhyAD = phyAddr;

	if (phyAddr != MAC_MAX_PHY)
		return(TRUE);
	else
		return(FALSE);
}

/*******************************************************************************
*
* sMacMiiInit - Initial MII chip
*/
int sMacMiiInit
	(
    DRV_CTRL *	pDrvCtrl	/* pointer to device control structure */
    )
{

	//Try to find MII chip
	if(pDrvCtrl->MII_PhyAD >= MAC_MAX_PHY)
	{
		if(sMacMiiScan(pDrvCtrl)==FALSE)
		{
			printf("MII scan fail\n");
			return(FALSE);
		}
	}
	//sMacMiiChipReset
	sMacMiiPhyWrite(pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_CR, MII_PHY_CR_RESET);

	//Set the ANA
	sMacMiiPhyWrite(pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_ANA, pDrvCtrl->MII_ANA_Setting);

	if(pDrvCtrl->MII_Loopback_Enable)
	{
		/* SW ISSUE */
		if(pDrvCtrl->MII_Force_Speed)
		{
			sMacMiiPhyWrite(pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_LBR
							,MII_PHY_LBR_PHY|MII_PHY_LBR_F100M);
			//sMAC_CSR_WRITE (pDrvCtrl->devAdrs, CSR6, (sMAC_CSR_READ(pDrvCtrl->devAdrs, CSR6)|CSR6_SPEED));
			pDrvCtrl->OpMode |= CSR6_SPEED;
			
		}
		else
		{
			sMacMiiPhyWrite(pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_LBR
							,MII_PHY_LBR_PHY);
			//sMAC_CSR_WRITE (pDrvCtrl->devAdrs, CSR6, (sMAC_CSR_READ(pDrvCtrl->devAdrs, CSR6)&(~CSR6_SPEED)));
			pDrvCtrl->OpMode &= ~CSR6_SPEED;
		}
		sMacMiiPhyWrite(pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_CR
						, (MII_PHY_CR_LOOP|pDrvCtrl->MII_Force_Speed|pDrvCtrl->MII_Force_Duplex));

		//Loopback Don't need to check Link and AutoNeg
		pDrvCtrl->MII_Link_OK = TRUE;
		pDrvCtrl->MII_AutoNeq_OK=TRUE;
		pDrvCtrl->MII_Auto_Speed=pDrvCtrl->MII_Force_Speed;
		pDrvCtrl->MII_Auto_Duplex=pDrvCtrl->MII_Force_Duplex;

		/* SW ISSUE */
		sMacMiiPhyReadAll(pDrvCtrl);
		MSDELAY(100);

		return(TRUE);
	}
	else
	{
		//Setup Mode Control Register
		if(pDrvCtrl->MII_Force_Enable)
		{
			sMacMiiPhyWrite(pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_CR
							, (pDrvCtrl->MII_Force_Speed|pDrvCtrl->MII_Force_Duplex));
			pDrvCtrl->OpMode |= CSR6_SPEED;
		}
		else
		{
			sMacMiiPhyWrite(pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_CR
							, (MII_PHY_CR_AUTO));			
			pDrvCtrl->OpMode &= ~CSR6_SPEED;
		}

		//Reset MII_Link_OK
		pDrvCtrl->MII_Link_OK = FALSE;
		{
    		USHORT	miiData=0;
    		UINT	linkTry = 0;

			while ( !(miiData & MII_PHY_SR_LNK) &&
			    (linkTry < MAC_MAX_LINK_TOUT))
			{
				MSDELAY(1000);	/* 1 second delay */
				miiData = sMacMiiPhyRead(pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_SR);
				sMacMiiPhyShow(pDrvCtrl);
				linkTry++;
			}

			if(linkTry>=MAC_MAX_LINK_TOUT)
			{
				//Link Fail
				return(FALSE);
			}

			//Get Link
			pDrvCtrl->MII_Link_OK = TRUE;
			if(pDrvCtrl->MII_Force_Enable)
			{
				//Force so don't need AutoNeq
				pDrvCtrl->MII_AutoNeq_OK=TRUE;
				pDrvCtrl->MII_Auto_Speed=pDrvCtrl->MII_Force_Speed;
				pDrvCtrl->MII_Auto_Duplex=pDrvCtrl->MII_Force_Duplex;
				return(TRUE);
			}
			else
			{
				//Reset AutoNeq
				pDrvCtrl->MII_AutoNeq_OK = FALSE;
				sMacMiiAutoNegGet(pDrvCtrl);
				return(TRUE);
			}
		}
	}
}

/* SW ISSUE */
/*******************************************************************************
*
* sMacMiiCheckHighRate - Get Most High Rate Setting of PHY
*/
USHORT sMacMiiCheckHighRate
	(
    USHORT	ablitites
    )
{
	if(ablitites & MII_PHY_ANA_T4AM)		ablitites = ablitites & MII_PHY_ANA_T4AM;
	else if(ablitites & MII_PHY_ANA_100F)	ablitites = ablitites & MII_PHY_ANA_100F;
	else if(ablitites & MII_PHY_ANA_100H)	ablitites = ablitites & MII_PHY_ANA_100H;
	else if(ablitites & MII_PHY_ANA_10F)	ablitites = ablitites & MII_PHY_ANA_10F;
	else if(ablitites & MII_PHY_ANA_10H)	ablitites = ablitites & MII_PHY_ANA_10H;
	return(ablitites);
}

/*******************************************************************************
*
* sMacMiiAutoNegGet - Get Auto Negotitation Result
*/
int sMacMiiAutoNegGet
	(
    DRV_CTRL *	pDrvCtrl	/* pointer to device control structure */
    )
{
	USHORT	miiData=0;
	UINT	linkTry = 0;
	USHORT	ablitites;

	while ( !(miiData & MII_PHY_SR_ASS) &&
	    (linkTry < MAC_MAX_LINK_TOUT))
	{
		MSDELAY(1000);	/* 1 second delay */
		miiData = sMacMiiPhyRead(pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_SR);
		sMacMiiPhyShow(pDrvCtrl);

	    #ifdef SMAC_DEBUG
	    {
	    	printf(" AutoNegGet SRegister Read = 0x%8x\n", miiData);
		}
		#endif

		linkTry++;
	}

	/* SW ISSUE */
	ablitites = sMacMiiPhyRead(pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_ANA);
	ablitites &= MII_PHY_ANA_ALLAM;

	if(linkTry < MAC_MAX_LINK_TOUT)
	{
		pDrvCtrl->MII_AutoNeq_OK = TRUE;

		/* SW ISSUE */
		miiData = sMacMiiPhyRead(pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_ANLPA);
		ablitites &= miiData;
	}

	ablitites = sMacMiiCheckHighRate(ablitites);
	if(ablitites & MII_PHY_ANA_T4AM)
	{
											pDrvCtrl->MII_Auto_Speed=MII_PHY_CR_100M;
											pDrvCtrl->MII_Auto_Duplex=MII_PHY_CR_FDX;
	}
	else if(ablitites & MII_PHY_ANA_100M)	pDrvCtrl->MII_Auto_Speed=MII_PHY_CR_100M;
	else									pDrvCtrl->MII_Auto_Speed=MII_PHY_CR_10M;
	if(ablitites & MII_PHY_ANA_FDAM)		pDrvCtrl->MII_Auto_Duplex=MII_PHY_CR_FDX;
	else									pDrvCtrl->MII_Auto_Duplex=MII_PHY_CR_HDX;

	if(linkTry < MAC_MAX_LINK_TOUT)		return(TRUE);
	else								return(FALSE);
}


/*******************************************************************************
*
* sMacMiiReAutoNeg - Force Restart Auto Negotitation
*/
int sMacMiiReAutoNeg
	(
    DRV_CTRL *	pDrvCtrl	/* pointer to device control structure */
    )
{
	sMacMiiPhyWrite(pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_CR
					, (MII_PHY_CR_AUTO|MII_PHY_CR_RAN));
	//Reset AutoNeq
	pDrvCtrl->MII_AutoNeq_OK = FALSE;
	return(sMacMiiAutoNegGet(pDrvCtrl));
}

/*******************************************************************************
*
* sMacChipReset - hardware reset of chip (stop it)
*/
int sMacChipReset
    (
    DRV_CTRL *	pDrvCtrl	/* pointer to device control structure */
    )
{
    sMAC_CSR_WRITE (pDrvCtrl->devAdrs, CSR6, 0);		/* stop rcvr & xmitter */

    sMAC_CSR_WRITE (pDrvCtrl->devAdrs, CSR7, 0);		/* mask interrupts */

    sMAC_CSR_WRITE (pDrvCtrl->devAdrs, CSR0, CSR0_SWR);

    /* Wait Loop, Loop for at least 50 PCI cycles according to chip spec */
	DELAY(1000);

    return (TRUE);
}



/*******************************************************************************
*
* sMacCSRReSetup - device CSR Re-Setup
*
* RETURNS: N/A.
*/
void sMacCSRReSetup
    (
    DRV_CTRL *	pDrvCtrl	/* pointer to device control structure */
    )
{
	/* any additional bus mode */
	sMAC_CSR_WRITE (pDrvCtrl->devAdrs, CSR0, (pDrvCtrl->BusMode));

	sMAC_CSR_WRITE (pDrvCtrl->devAdrs, CSR3, ((ULONG)(KSEG_TO_PHY(pDrvCtrl->rxRing))));
	sMAC_CSR_WRITE (pDrvCtrl->devAdrs, CSR4, ((ULONG)(KSEG_TO_PHY(pDrvCtrl->txRing))));

	/* SW ISSUE CSR3 CSR4 must before CSR6 */
	sMAC_CSR_WRITE (pDrvCtrl->devAdrs, CSR6, (pDrvCtrl->OpMode));
	//Disable All Rx Tx Timer & Int Mitigation Function
	sMAC_CSR_WRITE (pDrvCtrl->devAdrs, CSR11, (0));
}

/*******************************************************************************
*
* sMacIntInit - Initial Interrupt
*
* RETURNS:
*/
void	sMacIntInit
	(
	DRV_CTRL *	pDrvCtrl
	)
{
#ifdef	SMAC_INTCOUNTER_DEBUG
	memset((char *)(&(pDrvCtrl->int_counter)), 0, sizeof(INT_COUNTER));
#endif
#ifdef	MAC_RX_POLLSCHEME
	pDrvCtrl->rxScheduled = FALSE;
#endif
    pDrvCtrl->txScheduled = FALSE; 
}

/*******************************************************************************
*
* sMacInit - Socle MAC Initial procedure
*
* RETURNS: TRUE/FALSE
*/
int	sMacInit(	DRV_CTRL *	pDrvCtrl	)
{
	int 	result;

	// First we Reset
	sMacChipReset(pDrvCtrl);

	//Initail MII
	result=sMacMiiInit(pDrvCtrl);
	//sMacMiiPhyShow(pDrvCtrl);

	#ifdef MAC_INCLUDE_EEPROM    
		initEEP25();
	#endif  

  if(result!=TRUE){
   	return(result);
  }

	#ifdef MAC_INCLUDE_EEPROM    
	pDrvCtrl->eepdevpt = openEEP25( sMAC_CSR(pDrvCtrl->devAdrs,CSR9) 
								  , EEP25040
								  , CSRSWAP(CSR9_SDO)
								  , CSRSWAP(CSR9_SDI)
								  , CSRSWAP(CSR9_SCLK)
								  , CSRSWAP(CSR9_SCE));
	#endif    

    /* SW ISSUE */
    //After Phy get real speed and duplex we reset the opMode
  if(pDrvCtrl->MII_Auto_Speed & MII_PHY_CR_100M)
		pDrvCtrl->OpMode &= (~CSR6_TTM);
	else
		pDrvCtrl->OpMode |= (CSR6_TTM);
	if(pDrvCtrl->MII_Auto_Duplex & MII_PHY_CR_FDX)
		pDrvCtrl->OpMode |= CSR6_FD;
	else
		pDrvCtrl->OpMode &= (~CSR6_FD);

    //Initail CSR
	sMacCSRReSetup(pDrvCtrl);

 	/* start and tranmit (receiver will start after filter setup)*/
 	/* HW ISSU 4 */
  sMAC_CSR_UPDATE (pDrvCtrl->devAdrs, CSR6, (CSR6_ST));

  pDrvCtrl->rxIndex=0;
	pDrvCtrl->txIndex=0;
	pDrvCtrl->txDiIndex=0;
	pDrvCtrl->txCount=0;
	pDrvCtrl->txFrameCount=0;
	pDrvCtrl->rx_dead=0;

	#ifdef SMAC_ERRORDEBUG
		//Clear error debug
  memset ((char *)&pDrvCtrl->errStats, 0, sizeof (SMAC_STATS));
	#endif /* SMAC_ERRORDEBUG */

	#ifdef 	MAC_DATA_BUFFER_SIZEVARIABLE
  pDrvCtrl->rx_FristNeeded=TRUE;
  pDrvCtrl->rx_SegLength=0;
	pDrvCtrl->rx_PoolCurrentPt=pDrvCtrl->rx_PoolPt->DataPool;
  pDrvCtrl->rx_PooliRead=0;
  pDrvCtrl->rx_PooliWrite=0;
  pDrvCtrl->rxMultiFCount=0;
  #endif
 	//Initial Interrupt
 	sMacIntInit(pDrvCtrl);

    //Connect to Interrupt
  SYS_INT_CONNECT(pDrvCtrl, sMacInt, pDrvCtrl, &result);
	SYS_INT_SET_TYPE(pDrvCtrl, HI_LEVEL);
	//SYS_INT_SET_TYPE(pDrvCtrl, LO_LEVEL);

    /* clear the status register */
  sMAC_CSR_WRITE (pDrvCtrl->devAdrs, CSR5, 0xffffffff);
  #ifdef	SIMULATION_ONLY
  sMAC_CSR_WRITE (pDrvCtrl->devAdrs, CSR5, ~(0xffffffff));
  #endif

    /* set up the interrupts */
  sMAC_CSR_WRITE (pDrvCtrl->devAdrs, CSR7, SMAC_INTMASK);

  SYS_INT_ENABLE (pDrvCtrl);

    /* Set our flag */
  pDrvCtrl->attached = TRUE;

	return(result);
}

void 	dummy_if_1(void)
{

}

/*******************************************************************************
*
* sMacInt - handle controller interrupt
*
* This routine is called at interrupt level in response to an interrupt from
* the controller.
*/
void sMacInt
    (
    DRV_CTRL *	pDrvCtrl	/* pointer to device control struct */
    )
{
    ULONG	stat;		/* status register */
    ULONG	devAdrs;	/* device address */

    devAdrs = pDrvCtrl->devAdrs;

    /* Read the device status register */
    stat = sMAC_CSR_READ (devAdrs, CSR5);

    /* clear the interrupts */
    stat &= sMAC_CSR_READ (devAdrs, CSR7);
    sMAC_CSR_WRITE (devAdrs, CSR5, stat);
    #ifdef	SIMULATION_ONLY
    sMAC_CSR_WRITE (devAdrs, CSR5, ~(stat));
    #endif


    /* If false interrupt, return. */
    if ( ! (stat & (CSR5_NIS | CSR5_AIS)) )
        return;

	#ifdef	SMAC_INTCOUNTER_DEBUG
    	if(stat &  CSR5_ERI)	{pDrvCtrl->int_counter.int_eri++;	}
    	if(stat &  CSR5_GPTE)	{pDrvCtrl->int_counter.int_gte++;	}
    	if(stat &  CSR5_ETI)	{pDrvCtrl->int_counter.int_eti++;	}
    	if(stat &  CSR5_RPS)	{pDrvCtrl->int_counter.int_rps++;	}
    	if(stat &  CSR5_RU )	{pDrvCtrl->int_counter.int_ru++; 	}
    	if(stat &  CSR5_RI )	{pDrvCtrl->int_counter.int_ri++; 	}
    	if(stat &  CSR5_UNF)	{pDrvCtrl->int_counter.int_unf++;	}
    	if(stat &  CSR5_TU )	{pDrvCtrl->int_counter.int_tu++; 	}
    	if(stat &  CSR5_TPS)	{pDrvCtrl->int_counter.int_tps++;	}
    	if(stat &  CSR5_TI )	{pDrvCtrl->int_counter.int_ti++;	}
	#endif


    /* Have netTask handle any input packets */
    /* SW ISSUE important */
    if(stat & CSR5_RU)
    {
    	/* rx machine dead now we need 
    	   issue the poll demand when we release Owner to MAC */
    	pDrvCtrl->rx_dead = 1;   
    }    
    if (  (stat & (CSR5_RI | CSR5_RU) )
    	&&(! pDrvCtrl->rxScheduled) )
    {
    	#ifdef	MAC_RX_POLLSCHEME
    		#ifndef	SMAC_INTCOUNTER_DEBUG
        	sMAC_INT_DISABLE (devAdrs, (CSR7_RI | CSR7_RU));
        	#endif
        	pDrvCtrl->rxScheduled = TRUE;
        #endif
    }

    if (  (stat & (CSR5_TI | CSR5_TU))
        &&(! pDrvCtrl->txScheduled) )
    {
    		#ifndef	SMAC_INTCOUNTER_DEBUG
        	sMAC_INT_DISABLE (devAdrs, (CSR7_TI | CSR7_TU));
        	#endif
        	pDrvCtrl->txScheduled = TRUE;
    }
    return;
}

void 	dummy_if_1_1(void)
{

}

// SW sherlock
int if_sherlock_debug_count;
/*******************************************************************************
*
* sMacGetFullRMD - get next received message RMD
*
* Returns ptr to next Rx desc to process, or NULL if none ready.
*/
SMAC_RDE * sMacGetFullRMD
(
    DRV_CTRL *	pDrvCtrl	/* pointer to device control struct */
)
{
    SMAC_RDE *	rmd;		/* pointer to Rx ring descriptor */

    SMAC_GET_RDE(rmd, pDrvCtrl)	/* form ptr to Rx desc */

    if ((rmd->rDesc0 & DESSWAP(RDESC0_OWN)) == 0)
    {
    	//Owner By Host, Get Packet
        return (rmd);
    }
    else
    {
    	
    	if(if_sherlock_debug_count == 0)
        {
        	{
        		int	* TriggerPt;
        		TriggerPt = (int *)((0x100000));
        		*TriggerPt = 0xdead1234;
        	}
        //	printf("****************************God CPU have problem************");
        }	
        return ((SMAC_RDE *) NULL);
    }
}

void 	dummy_if_2(void)
{

}

#ifdef	MAC_RX_POLLSCHEME
/*******************************************************************************
*
* sMacRxSchedule - Polling scheme service for input packets
*
* This routine is called at task level indirectly by the interrupt
* service routine to do any message received processing.
*/
void sMacRxSchedule
(
    DRV_CTRL *	pDrvCtrl	/* pointer to device control struct */
)
{
    SMAC_RDE *	rmd;		/* pointer to Rx ring descriptor */
	int			getrmd;
	
	if_sherlock_debug_count=0;
	getrmd=1;
    while (getrmd)
    {
    	if((rmd = sMacGetFullRMD (pDrvCtrl)) != NULL)
    	{    	
        	if_sherlock_debug_count++;
        	sMacRecv (pDrvCtrl, rmd);
        }
        else
        {
        	getrmd=0;
        	if(if_sherlock_debug_count == 0)
        	{

        		if((rmd = sMacGetFullRMD (pDrvCtrl)) != NULL)
    			{    	
        			if_sherlock_debug_count++;
        			getrmd=1;
        			sMacRecv (pDrvCtrl, rmd);
        		}
				/*
        		{
        			int	* TriggerPt;
        			TriggerPt = (int *)((0x100000));
        			*TriggerPt = 0xdead1234;
        		}
        		printf("!!!!!!!!!!!!!!!!!!!!!!God CPU have problem!!!!!!!!!!!! ");
        		*/
        	}	
        }		
    }
	pDrvCtrl->rxScheduled = FALSE;
    if(pDrvCtrl->tx_merge_number)
    {
    	sMAC_INT_ENABLE (pDrvCtrl->devAdrs, (CSR7_RI ));
	}
	else
	{
		sMAC_INT_ENABLE (pDrvCtrl->devAdrs, (CSR7_RI | CSR7_RU));
	}
	#ifdef SMAC_ERRORDEBUG
	{
		ULONG	temp;
		//OCO FOC MFO MFC will clear when read
		temp = *((ULONG *)sMAC_CSR(pDrvCtrl->devAdrs, CSR8)); 
		temp = CSRSWAP(temp);
		//temp = sMAC_CSR_READ (pDrvCtrl->devAdrs, CSR8);
		pDrvCtrl->errStats.rxOverflow += CSR8_OCOFOC_GET(temp);
    	pDrvCtrl->errStats.rxMissed += (temp & (CSR8_MFO | CSR8_MFC_MSK));
    }
	#endif /* SMAC_ERRORDEBUG */
    return;
}
#endif /* MAC_RX_POLLSCHEME */

void 	dummy_if_3(void)
{

}

/*******************************************************************************
*
* sMacRecv - process the next incoming packet
*
*/
int	sMacRecv
    (
    DRV_CTRL *	pDrvCtrl,	/* pointer to device control struct */
    SMAC_RDE *	rmd			/* pointer to Rx ring descriptor */
    )
{
    char *	pMbuf;			/* pointer to mbuf */
    char *	pData;			/* pointer to data */
    ULONG	rdesc0;			/* desc0 status word */
    int		framelength;	/* frame length */
    int		frametype;		/* frame type */
    int		dbufflen;		/* data buffer Length */
	int		copylen;
	int		status;

	#ifdef	MAC_DATA_BUFFER2_USED
	int		dbufflen1;
	#endif


    rdesc0 = sMAC_READ_DESC(&(rmd->rDesc0));
#ifdef SMAC_ERRORDEBUG
	if (rdesc0 & (RDESC0_RE))	{pDrvCtrl->errStats.rxMiiErr++;		}
	if (rdesc0 & (RDESC0_LS))
	{
    if (rdesc0 & (RDESC0_FF))	{pDrvCtrl->errStats.rxFiltrErr++;	}
    if (rdesc0 & (RDESC0_DE))	{pDrvCtrl->errStats.rxDescErr++;	}
    if (rdesc0 & (RDESC0_RF))	{pDrvCtrl->errStats.rxRuntFrm++;	}
    if (rdesc0 & (RDESC0_TL))	{pDrvCtrl->errStats.rxTooLong++;	}
    if (rdesc0 & (RDESC0_CS))	{pDrvCtrl->errStats.rxCollision++;	}
    if (rdesc0 & (RDESC0_DB))	{pDrvCtrl->errStats.rxDribbleBit++;	}
    if (rdesc0 & (RDESC0_CE))	{
    	pDrvCtrl->errStats.rxCrcErr++;
    }
    if (rdesc0 & (RDESC0_FIFOERR)) {
    	pDrvCtrl->errStats.rxFifoErr++;
    }
	}
#endif	/* SMAC_ERRORDEBUG */

    /* Discard if we have errors */
    if  ((rdesc0 & RDESC0_ES) && (rdesc0 & RDESC0_LS) && (pDrvCtrl->rx_ignore_error == 0))
    {
    	#ifdef 	MAC_DATA_BUFFER_SIZEVARIABLE
    	pDrvCtrl->rx_FristNeeded=TRUE;
    	#endif
    	pDrvCtrl->errStats.rxDiscard++;
    	status = FALSE;
        goto cleanRXD;
    }

	//Could not Find First Descriptor
	if((rdesc0 & RDESC0_FD) != RDESC0_FD)
	{
		#ifdef 	MAC_DATA_BUFFER_SIZEVARIABLE
		if(pDrvCtrl->rx_FristNeeded)
		#endif
		{
    		pDrvCtrl->errStats.rxDiscard++;
			status = FALSE;
        	goto cleanRXD;
        }
    }
	else
	{
		#ifdef 	MAC_DATA_BUFFER_SIZEVARIABLE
		int temp;
		temp=pDrvCtrl->rx_PooliWrite+1;
		if(temp==(pDrvCtrl->rx_PooliMax))	temp=0;

		if(temp==pDrvCtrl->rx_PooliRead)
		{
			// Don't have Buffer Stop Clear DES and keep sMacRecv Going
			printf("!!!!!!!!!!!!!!!!!!!!!!!Error No Buffer Avaiable !!!!!!!!!!!!!!");
			status = FALSE;
        	goto cleanRXD_noPool;
        }
		pDrvCtrl->rx_PoolCurrentPt=(pDrvCtrl->rx_PoolPt+(pDrvCtrl->rx_PooliWrite))->DataPool;
		pDrvCtrl->rx_FristNeeded=FALSE;
		pDrvCtrl->rx_SegLength=0;
		#endif	/* MAC_DATA_BUFFER_SIZEVARIABLE*/
	}

	if((rdesc0 & RDESC0_LS))
	{
    	/* Process clean packets */
    	framelength = RDESC0_FL_GET(rdesc0);	/* frame length */
		framelength -= 4; 	/* Frame length includes CRC in it so subtract it */
		frametype = RDESC0_FT_GET(rdesc0);
		dbufflen = framelength;
		#ifdef	MAC_DATA_BUFFER_SIZEVARIABLE
		{
			RCVDATAPOOL *	RcvPoolpt;
			RcvPoolpt=(pDrvCtrl->rx_PoolPt+(pDrvCtrl->rx_PooliWrite));
			RcvPoolpt->Frame_Type = frametype;
			RcvPoolpt->Frame_Length = framelength;
		}
		dbufflen -= pDrvCtrl->rx_SegLength;
		#ifdef	MAC_DATA_BUFFER2_USED
		if(pDrvCtrl->DesSingleBuff2==0)
		{
			dbufflen1=SMAC_GET_RXBUFFER_SIZE(1,pDrvCtrl);
		}
		else
		{
			dbufflen1=0;
		}
		#endif	/* MAC_DATA_BUFFER2_USED */
		pDrvCtrl->rx_FristNeeded=TRUE;
		pDrvCtrl->rx_PooliWrite++;
		if((pDrvCtrl->rx_PooliWrite)==(pDrvCtrl->rx_PooliMax))	pDrvCtrl->rx_PooliWrite=0;

		if(rdesc0 & (RDESC0_MF))
		{
			pDrvCtrl->rxMultiFCount++;
		}
		#endif	/* MAC_DATA_BUFFER_SIZEVARIABLE	*/
	}
	else
	{
		#ifdef 	MAC_DATA_BUFFER_SIZEVARIABLE
		if(pDrvCtrl->DesSingleBuff2==0)
		{
			dbufflen=SMAC_GET_RXBUFFER_SIZE(1,pDrvCtrl);
		}
		else
		{
			dbufflen=0;
		}
		#ifdef	MAC_DATA_BUFFER2_USED
		dbufflen1=dbufflen;
		#ifdef	MAC_DES_BOTHMODE
		if((pDrvCtrl->DesChainMode==0) && (pDrvCtrl->DesSingleBuff1==0))
		#endif
		{
			dbufflen += SMAC_GET_RXBUFFER_SIZE(2,pDrvCtrl);
		}
		#endif	/* MAC_DATA_BUFFER2_USED */
		pDrvCtrl->rx_SegLength += dbufflen;
		#endif	/* MAC_DATA_BUFFER_SIZEVARIABLE	*/
	}

	#ifdef 	MAC_DATA_BUFFER_SIZEVARIABLE
	/* Get pointer to memory buffer */
	pMbuf=pDrvCtrl->rx_PoolCurrentPt;

	/* Get pointer to packet */
	pData = (char *)(DESSWAP(rmd->rDesc2));

	#ifdef	MAC_DATA_BUFFER2_USED
	if(dbufflen>dbufflen1)
	{
		copylen=dbufflen1;
	}
	else
	{
		copylen=dbufflen;
	}
	#else
	copylen=dbufflen;
    #endif	/* MAC_DATA_BUFFER2_USED */

	if(copylen!=0)
	{
    	//Copy to Memory
		sMacFromDataBufCopy(pData, pMbuf, copylen);
		pDrvCtrl->rx_PoolCurrentPt += copylen;
	}

	#ifdef	MAC_DATA_BUFFER2_USED
	if(copylen!=dbufflen)
	{
		copylen=dbufflen-dbufflen1;

		/* Get pointer to memory buffer */
		pMbuf=pDrvCtrl->rx_PoolCurrentPt;

		/* Get pointer to packet */
		pData = (char *)(DESSWAP(rmd->rDesc3));

		//Copy to Memory
		sMacFromDataBufCopy(pData, pMbuf, copylen);
		pDrvCtrl->rx_PoolCurrentPt += copylen;
	}
	#endif	/* MAC_DATA_BUFFER2_USED */
	#endif	/* MAC_DATA_BUFFER_SIZEVARIABLE	*/

	status=TRUE;

    /* Done with descriptor, clean up and give it to the device. */
cleanRXD:

    /* Advance our management index */
    pDrvCtrl->rxIndex = SMAC_RD_INDEX_NEXT(pDrvCtrl);

    /* clear status bits and give ownership to device */
    rmd->rDesc0 = DESSWAP(RDESC0_OWN);
    
    /* SW ISSUE IMPORTANT */
    /* rx machine dead now we need 
       issue the poll demand when we release Owner to MAC */
    if(pDrvCtrl->rx_dead)
    {
    	pDrvCtrl->rx_dead=0;
    	sMAC_CSR_WRITE (pDrvCtrl->devAdrs, CSR2, CSR2_RPD);
    }   

cleanRXD_noPool:
    return (status);
}


/*******************************************************************************
*
* sMacFltrFrmXmit - Transmit the setup filter frame.
*
* This routine transmits the setup filter frame.
* The setup frame is not transmitted actually over the wire. The setup frame
* which is transmitted is 192 bytes. The tranmitter should be in an on state
* before this function is called. This call has been coded so that the
* setup frame can be transmitted after the chip is done with the
* intialization taking into consideration for adding multicast support.
*
* RETURNS: TRUE/FALSE
*/

int sMacFltrFrmXmit
    (
    DRV_CTRL *	pDrvCtrl,		/* pointer to device control structure */
    char * 	pPhysAdrsTbl,		/* pointer to physical address table */
    int 	tblLen				/* size of the physical address table */
    )
{
	SMAC_TDE *	tmd;			/* pointer to Tx ring descriptor */
	ULONG * 	pBuff;			/* pointer to the Xmit buffer */
	ULONG * 	pFltrBuff;		/* pointer to the Fltr buffer */
	ULONG		csr7Val;		/* value in CSR7 */
	int 		result=TRUE;
	int 		i;
	
	if (sMacFltrFrmSetup (pDrvCtrl, pPhysAdrsTbl, tblLen) != TRUE)
	{
		/* not able to set up filter frame */
		return(FALSE);
	}

	/* See if next TXD is available */
	SMAC_GET_TDE( tmd, pDrvCtrl );

	while  (  ( (tmd->tDesc0 & DESSWAP(TDESC0_OWN)) != 0 )
	     ||( ((pDrvCtrl->txIndex + 1) % pDrvCtrl->NumTds) == pDrvCtrl->txDiIndex ) )
	{
		// DeadLock until Tx Buffer available
		/* HW ISSUE 2 */
		// sMAC_KICKSTART(pDrvCtrl);
	}

	/* Get pointer to transmit buffer */
	pBuff = (ULONG *)(sMAC_READ_DESC(&(tmd->tDesc2)));
	pFltrBuff = pDrvCtrl->pFltrFrm;
	
	/* copy into Xmit buffer */
	for(i=0;i<FLTR_FRM_SIZE;i=i+4)
	{
		sMAC_WRITE_DATAB(pBuff,*pFltrBuff);
		pFltrBuff++;
		pBuff++;
	}
	
	tmd->tDesc0 = 0;							/* clear buffer error status */
	//clear every time will be changed setting
    tmd->tDesc1 &= DESSWAP (~(TDESC1_LS|TDESC1_FS|TDESC1_SET|TDESC1_TBS1_MSK|TDESC1_TBS2_MSK));
	// frame type as set up and set Frame Size
	tmd->tDesc1 |= DESSWAP((pDrvCtrl->FilterTableType|TDESC1_SET|TDESC1_TBS1_VAL(FLTR_FRM_SIZE)));

	csr7Val = sMAC_CSR_READ (pDrvCtrl->devAdrs, CSR7);
	sMAC_CSR_WRITE (pDrvCtrl->devAdrs, CSR7, 0);	/* mask interrupts */

	// Enable
	tmd->tDesc0 = DESSWAP(TDESC0_OWN); 		/* give ownership to device */

	/* Advance our management index */
	pDrvCtrl->txIndex = SMAC_TD_INDEX_NEXT(pDrvCtrl);
	pDrvCtrl->txDiIndex = SMAC_TD_DINDEX_NEXT(pDrvCtrl);

/////////////////////////////////////////////////////////////////////////////////////////////////
	/* Follow a zero length buffer for general purpose (Non 1st one Des)*/
	/* See if next TXD is available */
	SMAC_GET_TDE( tmd, pDrvCtrl );
	while  (  ( (tmd->tDesc0 & DESSWAP(TDESC0_OWN)) != 0 )
	     ||( ((pDrvCtrl->txIndex + 1) % pDrvCtrl->NumTds) == pDrvCtrl->txDiIndex ) )
	{
		// DeadLock until Tx Buffer available
		/* HW ISSUE 2 */
		// sMAC_KICKSTART(pDrvCtrl);
	}
	tmd->tDesc0 = 0;							/* clear buffer error status */
	//clear every time will be changed setting
    tmd->tDesc1 &= DESSWAP (~(TDESC1_LS|TDESC1_FS|TDESC1_SET|TDESC1_TBS1_MSK|TDESC1_TBS2_MSK));
	// frame type as set up and set no Frame Size
	tmd->tDesc1 |= DESSWAP((pDrvCtrl->FilterTableType|TDESC1_SET));
	// Enable
	tmd->tDesc0 = DESSWAP(TDESC0_OWN); 		/* give ownership to device */
	/* Advance our management index */
	pDrvCtrl->txIndex = SMAC_TD_INDEX_NEXT(pDrvCtrl);
	pDrvCtrl->txDiIndex = SMAC_TD_DINDEX_NEXT(pDrvCtrl);
/////////////////////////////////////////////////////////////////////////////////////////////////
	/* HW ISSUE 4 */
	// sMAC_CSR_UPDATE (pDrvCtrl->devAdrs, CSR6, (CSR6_ST));

	sMAC_KICKSTART(pDrvCtrl)

	#ifdef SIMULATION_ONLY
		//For simulation only we clear the check of HW
		tmd->tDesc0 &= DESSWAP(~TDESC0_OWN);
	#endif

	/* wait for the own bit to change */
	while (tmd->tDesc0 & DESSWAP(TDESC0_OWN))
	{
		/* dead lock until setupt finish */
		/* HW ISSUE 2 */
		//sMAC_KICKSTART(pDrvCtrl);
	}

    tmd->tDesc0 = 0;				/* clear status bits */

	//Double check Setting OK or not
	if(pDrvCtrl->FilterTableType==TDESC1_HASH)
	{
		if( (sMAC_CSR_READ (pDrvCtrl->devAdrs, CSR6) & (CSR6_IF|CSR6_HO|CSR6_HP))
		  !=(CSR6_HP) )
		 {
		 	result=FALSE;
		 }
	}
	else if(pDrvCtrl->FilterTableType==TDESC1_HASH_ONLY)
	{
		if( (sMAC_CSR_READ (pDrvCtrl->devAdrs, CSR6) & (CSR6_IF|CSR6_HO|CSR6_HP))
		  !=(CSR6_HO|CSR6_HP) )
		 {
		 	result=FALSE;
		 }
	}
	else if(pDrvCtrl->FilterTableType==TDESC1_INVERSE)
	{
		if( (sMAC_CSR_READ (pDrvCtrl->devAdrs, CSR6) & (CSR6_IF|CSR6_HO|CSR6_HP))
		  !=(CSR6_IF) )
		 {
		 	result=FALSE;
		 }
	}
	else
	//FilterTableType==TDESC1_PERFECT
	{
		if( (sMAC_CSR_READ (pDrvCtrl->devAdrs, CSR6) & (CSR6_IF|CSR6_HO|CSR6_HP))
		  !=(0) )
		 {
		 	result=FALSE;
		 }
	}

	sMAC_CSR_WRITE (pDrvCtrl->devAdrs, CSR7, csr7Val); /* restore value */

	// Finsh Filter setting we auto restart receiver
    sMAC_CSR_UPDATE (pDrvCtrl->devAdrs, CSR6, (CSR6_SR));

	return (result);
}

/*******************************************************************************
*
* sMacFltrFrmSetup - set up the filter frame.
*
* This routine sets up the filter frame to filter the physical addresses
* on the incoming frames. The setup filter frame buffer is 192 bytes. This
* setup frame needs to be transmitted before transmitting and receiving
* any frames.
*
* RETURNS: TURE/FALSE
*/
int sMacFltrFrmSetup
    (
    DRV_CTRL *	pDrvCtrl,	/* pointer to device control structure */
    char * 	pPhysAdrsTbl,	/* pointer to physical address table */
    int 	tblLen			/* size of the physical address table */
    )
{
    int			ix;			/* index variable */
    int			jx;			/* inner index variable */
    ULONG *		pFltrFrm;	/* pointer to the filter frame */
    char *		pPhysAddrs;	/* pointer to the physical addresses */

    if (tblLen > FLTR_FRM_ADRS_NUM)
	return (FALSE);

    pFltrFrm = pDrvCtrl->pFltrFrm;

    for (ix = 0; ix < FLTR_FRM_SIZE_ULONGS; ix++)
		*pFltrFrm++ = FLTR_FRM_DEF_ADRS;

   	pFltrFrm = pDrvCtrl->pFltrFrm;
   	pPhysAddrs = pPhysAdrsTbl;

	if(pDrvCtrl->FilterTableType==TDESC1_HASH)
	{
		// 1 Perfect + HASH
		// 1st input Address is Perfect
		// From 2nd Address belong to Hash

		//Set Pefect at 39 ULONG (156)
		pFltrFrm=pFltrFrm+39;
		for (jx = 0; jx < FLTR_FRM_ADRS_SIZE; jx = jx+2)
		{
		 	*pFltrFrm = (*pPhysAddrs)|( (*(pPhysAddrs +1)) << 8 );
		   	pPhysAddrs = pPhysAddrs + 2;
		   	pFltrFrm++;
		}

		//Remove Back pFltrFrm and try to calculator Hash
		pFltrFrm = pDrvCtrl->pFltrFrm;
		sMacHashBuild((char *)pPhysAddrs,pFltrFrm,tblLen-1);
	}
	else if(pDrvCtrl->FilterTableType==TDESC1_HASH_ONLY)
	{
		sMacHashBuild(pPhysAdrsTbl,pFltrFrm,tblLen);
	}
	else
	//FilterTableType==TDESC1_PERFECT
	//FilterTableType==TDESC1_INVERSE
	{
		//16 Perfect address
    	for (ix = 0; ix < tblLen; ix++)
		{
			for (jx = 0; jx < FLTR_FRM_ADRS_SIZE; jx = jx+2)
			{
		 		*pFltrFrm = (*pPhysAddrs)|( (*(pPhysAddrs +1)) << 8 );
		   		pPhysAddrs = pPhysAddrs + 2;
		   		pFltrFrm++;
			}
		}
	}
    return (TRUE);
}

/*******************************************************************************
*
* sMacHashBuild - Build up Hash Table
*/
/* The little-endian AUTODIN32 ethernet CRC calculation.
   N.B. Do not use for bulk data, use a table-based routine instead.
   This is common code and should be moved to net/core/crc.c */
static unsigned const ethernet_polynomial_le = 0xedb88320U;
static inline u32_t ether_crc_le(int length, unsigned char *data)
{
	u32_t crc = 0xffffffff;	/* Initial value. */
	while(--length >= 0) {
		unsigned char current_octet = *data++;
		int bit;
		for (bit = 8; --bit >= 0; current_octet >>= 1) {
			if ((crc ^ current_octet) & 1) {
				crc >>= 1;
				crc ^= ethernet_polynomial_le;
			} else
				crc >>= 1;
		}
	}
	return crc;
}

void sMacHashBuild
    (
    char * 	pPhysAdrsTbl,	/* pointer to physical address table */
    ULONG *	pFltrFrm,		/* pointer to the filter frame */
    int 	tblLen			/* size of the physical address table */
    )
{
	int		i;
	u32_t	index;
	ULONG *	temp_pFltrFrm;		/* pointer to the filter frame */
	USHORT 	DataTemp;

	//Clear Hash Bit Index Table
	temp_pFltrFrm=pFltrFrm;
    for (i = 0; i < FLTR_HASH_FRM_SIZE_ULONGS; i++)
		*temp_pFltrFrm++ = 0;

	/* This should work on big-endian machines as well. */
	for (i = 0; i < tblLen; i++)
	{
		index = ether_crc_le(FLTR_FRM_ADRS_SIZE, pPhysAdrsTbl) & 0x1ff;
		DataTemp = (0x01) << (index%16);
		pFltrFrm[index/16] |= DataTemp;
		pPhysAdrsTbl=pPhysAdrsTbl+FLTR_FRM_ADRS_SIZE;
	}
	/* Broadcast entry */
	pFltrFrm[255/16] |= ((0x01)<<15);
}

void 	dummy_if_4(void)
{

}

/*******************************************************************************
*
* sMacPacketSend - MAC Send out packet
*/
void sMacPacketSend
    (
    DRV_CTRL *	pDrvCtrl,	/* pointer to device control structure */
    char	 *  PacketPt,	/* point to send out packet */
    int			PacketSize  /* packet size */
    )
{
	SMAC_TDE *	pTxD;
	char	 *	pTxdBuf;
	ULONG		FirstDes;
	ULONG		LastDes;

	int			dBufSiz1;
	#ifdef	MAC_DATA_BUFFER2_USED
	int			dBufSiz2;
	#endif

	/* SW ISSUE */
	/* God !!!! So stupe !!! But so easy */
	int			OwerDES_Index;
	int			i;
	ULONG	*	OwerDESArray[9];
	OwerDES_Index=0;

	FirstDes=TDESC1_FS;
	LastDes=0;
	while(PacketSize>0)
	{
		/* See if next TXD is available */
		SMAC_GET_TDE( pTxD, pDrvCtrl );
		if  (  ((pTxD->tDesc0 & DESSWAP(TDESC0_OWN)) != 0)
		     ||(SMAC_TD_INDEX_NEXT (pDrvCtrl) == pDrvCtrl->txDiIndex) )
		{
			//Clean TxRing for txDiIndex
			sMacTxRingClean (pDrvCtrl);
			#ifdef	SMAC_ERRORDEBUG
			pDrvCtrl->errStats.txRetries++;
			#endif /* SMAC_ERRORDEBUG */
			continue;
			// if TDESC0_OWN
			// DeadLock until Tx Buffer available
			// The better way is use OS delay
		}

		pTxdBuf = (char *)(DESSWAP(pTxD->tDesc2));
		if(pDrvCtrl->DesSingleBuff2)
		{
			dBufSiz1 = 0;
		}
		else
		{
			dBufSiz1 = SMAC_GET_TXBUFFER_SIZE(1,pDrvCtrl);
		}
		if(dBufSiz1>=PacketSize)
		{
			dBufSiz1=PacketSize;
			LastDes=TDESC1_LS;
		}
		if(dBufSiz1!=0)
		{
        	sMacToDataBufCopy(PacketPt, pTxdBuf, dBufSiz1);
        	PacketPt=PacketPt+dBufSiz1;
    	}
        PacketSize=PacketSize-dBufSiz1;
        #ifdef		MAC_DATA_BUFFER2_USED
        if(  (PacketSize==0)
        #ifdef	MAC_DES_BOTHMODE
           ||(pDrvCtrl->DesChainMode)
           ||(pDrvCtrl->DesSingleBuff1)
        #endif
          )
        {
			dBufSiz2=0;
		}
		else
		{
        	pTxdBuf = (char *)(DESSWAP(pTxD->tDesc3));
			dBufSiz2 = SMAC_GET_TXBUFFER_SIZE(2,pDrvCtrl);
			if(dBufSiz2>=PacketSize)
			{
				dBufSiz2=PacketSize;
				LastDes=TDESC1_LS;
			}
        	sMacToDataBufCopy(PacketPt, pTxdBuf, dBufSiz2);
        	PacketPt=PacketPt+dBufSiz2;
        	PacketSize=PacketSize-dBufSiz2;
        }
        #endif

        pTxD->tDesc0  = 0;		/* clear buffer error status */
        //clear every time will be changed setting
        pTxD->tDesc1 &= DESSWAP (~(TDESC1_LS|TDESC1_FS|TDESC1_SET|TDESC1_TBS1_MSK|TDESC1_TBS2_MSK));
        #ifdef		MAC_DATA_BUFFER2_USED
        pTxD->tDesc1 |= DESSWAP ( TDESC1_TBS1_VAL(dBufSiz1)
        						 |TDESC1_TBS2_VAL(dBufSiz2)
        						 |FirstDes
        						 |LastDes);
        #else
        pTxD->tDesc1 |= DESSWAP ( TDESC1_TBS1_VAL(dBufSiz1)
        						 |FirstDes
        						 |LastDes);
        #endif

		/* SW ISSUE */
		/* Don't Kick start and give owner befor you
			finish complete the packet seting.
			Otherwise software are race with HW
			then you will find out the underflow problem.
		*/
		//#ifdef	SMAC_TEST
		//if((pDrvCtrl->Tx_Underflow_Test) && (LastDes))
		//{
		//	// For underflow Test we don't enable the last des owner
		//	// Make Tx FIFO underflow
		//}
		//else
		//#endif
        //{
        //	pTxD->tDesc0  = DESSWAP (TDESC0_OWN);  /* give ownership to device */
		//}
		/* SW ISSUE */
		/* God !!!! So stupe !!! But so easy */
		if((pDrvCtrl->Tx_Underflow_Test) && (LastDes))
		{
			// For underflow Test we don't enable the last des owner
			// Make Tx FIFO underflow
		}
		else
		{
			OwerDESArray[OwerDES_Index]=&(pTxD->tDesc0);
			OwerDES_Index++;
		}

        /* Advance our management index */
        pDrvCtrl->txIndex = SMAC_TD_INDEX_NEXT (pDrvCtrl);

		/* Kickstart transmit */
		/* SW ISSUE */
		/* Don't Kick start and give owner befor you
			finish complete the packet seting.
			Otherwise software are race with HW
			then you will find out the underflow problem.
		*/
		//sMAC_KICKSTART(pDrvCtrl);

		FirstDes=0;
		pDrvCtrl->txCount++;
	}
	pDrvCtrl->txFrameCount++;

	/* SW ISSUE */
	/* God !!!! So stupe !!! But so easy */
	for(i=0;i<OwerDES_Index;i++)
	{
		*(OwerDESArray[i]) = DESSWAP (TDESC0_OWN);  /* give ownership to device */
	}
	/* Kickstart transmit */
	if(pDrvCtrl->tx_merge_counter==0)
	{
		sMAC_KICKSTART(pDrvCtrl);
		pDrvCtrl->tx_merge_counter=pDrvCtrl->tx_merge_number;
	}
	else
	{
		pDrvCtrl->tx_merge_counter--;
	}

}

/*******************************************************************************
*
* sMacTxRingClean - cleans up the transmit ring
*
* This routine is called at task level in the context to collect statistics
* , and mark "free" all descriptors consumed by the device.
*/
void sMacTxRingClean
    (
    DRV_CTRL *pDrvCtrl
    )
{
    SMAC_TDE *	tmd;            /* pointer to Xmit ring descriptor */
	ULONG		tdesc0;

    while (pDrvCtrl->txDiIndex != pDrvCtrl->txIndex)
    {
    	/* disposal has not caught up */
        SMAC_GET_DTDE(tmd, pDrvCtrl);

		tdesc0 = sMAC_READ_DESC(&(tmd->tDesc0));

        /* if the buffer is still owned by device, don't touch it */
        if (tdesc0 & (TDESC0_OWN))
            break;

        /* now bump the tmd disposal index pointer around the ring */
        pDrvCtrl->txDiIndex = SMAC_TD_DINDEX_NEXT(pDrvCtrl);
        pDrvCtrl->txCount--;
		
		//Only for last Tx packet need check error
		if( tmd->tDesc1 & DESSWAP(TDESC1_LS))
		{
			#ifdef	SMAC_ERRORDEBUG
        	if (tdesc0 & TDESC0_LO)	
        		{pDrvCtrl->errStats.txLostCarrier++;	}
        	if (tdesc0 & TDESC0_NC)	
        		{pDrvCtrl->errStats.txNoCarrier++;		}
        	if (tdesc0 & TDESC0_UF)	
        		{pDrvCtrl->errStats.txUnderflow++;		}
        	else if (tdesc0 & TDESC0_LC)
        							{pDrvCtrl->errStats.txLateCollision++;	}
        	if (tdesc0 & TDESC0_EC)	
        		{pDrvCtrl->errStats.txExsCollision++;	pDrvCtrl->errStats.txCollisonCount += 16;	}
        	else if (tdesc0 & TDESC0_CC_MSK)
        							{pDrvCtrl->errStats.txCollisonCount += TDESC0_CC_GET(tdesc0);	}
        	if (tdesc0 & TDESC0_DE)	
        		{pDrvCtrl->errStats.txDeferred++;		}
			#endif	/* SMAC_ERRORDEBUG */

        	/*
        	 * TDESC0_ES is an "OR" of LC, NC, UF, EC LO
        	 * here for error conditions.
        	 */
        	if (tdesc0 & (TDESC0_ES))
	    	{
	    		pDrvCtrl->errStats.txErrorSum++;
        	    /*
	    	 	 * If error was due to excess collisions, bump the collision
        	     * counter.
        	     */

        	    /*
	    	 	 * Restart chip on fatal errors.
        	     * The following code handles the situation where the transmitter
        	     * shuts down due to an underflow error.  This is a situation that
        	     * will occur if the DMA cannot keep up with the transmitter.
        	     * It will occur if the device is being held off from DMA access
        	     * for too long or due to significant memory latency.  DRAM
        	     * refresh or slow memory could influence this.  Many
        	     * implementation use a dedicated device buffer.  This can be
        	     * static RAM to eliminate refresh conflicts; or dual-port RAM
        	     * so that the device can have free run of this memory during its
        	     * DMA transfers.
        	     */
        	}
        }
        tmd->tDesc0 = 0;	/* clear all error & stat stuff */
    }
}


/*******************************************************************************
*
* sMacTxSchedule - Check enqueued transmit buffers
*
*/
void sMacTxSchedule
	(
    DRV_CTRL *	pDrvCtrl
	)
{
	#ifdef	SMAC_ERRORDEBUG
    pDrvCtrl->errStats.txFlushCalled++;
	#endif	/* SMAC_ERRORDEBUG */

    sMacTxRingClean (pDrvCtrl);

    if(SMAC_TD_INDEX_NEXT(pDrvCtrl) != pDrvCtrl->txDiIndex)
    {
    	//refill buffer
    	//Call StartOutput
    }

    pDrvCtrl->txScheduled = FALSE;
    if(pDrvCtrl->tx_merge_number)
    {
    	sMAC_INT_ENABLE (pDrvCtrl->devAdrs, (CSR7_TI ));
    }
    else
    {
    	sMAC_INT_ENABLE (pDrvCtrl->devAdrs, (CSR7_TI | CSR7_TU));
    }	
}


void sMacToDataBufCopy
	(
	char 	*from_pt,
	char	*buf_pt,
	int		length
	)
{
	ULONG	*Lbuf_pt;
	ULONG	Temp;
	int		i;

	i=0;
	Temp=0;
	Lbuf_pt=(ULONG	*)buf_pt;
	for(i=0;i<length;i++)
	{
		Temp=(Temp>>8)|((*from_pt)<<24);
		from_pt++;

		if((i%4)==3)
		{
			*Lbuf_pt=DATABSWAP(Temp);
			Lbuf_pt++;
		}
	}
	while((i%4)!=0)
	{
		Temp=(Temp>>8);
		if((i%4)==3)
		{
			*Lbuf_pt=DATABSWAP(Temp);
		}
		i++;
	}
}

void sMacFromDataBufCopy
	(
	char 	*buf_pt,
	char	*to_pt,
	int		length
	)
{
	ULONG	*Lbuf_pt;
	ULONG	Temp;
	int		i;

	i=0;
	Temp=0;
	Lbuf_pt=(ULONG	*)buf_pt;
	for(i=0;i<length;i++)
	{
		if((i%4)==0)
		{
			Temp=DATABSWAP(*Lbuf_pt);
			Lbuf_pt++;
		}
		*to_pt=(char) Temp;
		Temp=Temp>>8;
		to_pt++;
	}
}

void sMacMiiPhyReadAll
    (
    DRV_CTRL	*pDrvCtrl
    )
{
    USHORT reg;
	reg=sMacMiiPhyRead (pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_CR);
	//printf("MII_PHY_CR = 0x%08x\n", reg);	//leonid+
	reg=sMacMiiPhyRead (pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_SR);
	//printf("MII_PHY_SR = 0x%08x\n", reg);	//leonid+
	reg=sMacMiiPhyRead (pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_ID0);
	//printf("MII_PHY_ID0 = 0x%08x\n", reg);	//leonid+
	reg=sMacMiiPhyRead (pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_ID1);
	//printf("MII_PHY_ID1 = 0x%08x\n", reg);	//leonid+
	reg=sMacMiiPhyRead (pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_ANA);
	//printf("MII_PHY_ANA = 0x%08x\n", reg);	//leonid+
	reg=sMacMiiPhyRead (pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_ANLPA);
	//printf("MII_PHY_ANLPA = 0x%08x\n", reg);	//leonid+
	reg=sMacMiiPhyRead (pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_ANE);
	//printf("MII_PHY_ANE = 0x%08x\n", reg);	//leonid+
	reg=sMacMiiPhyRead (pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_ANP);
	//printf("MII_PHY_ANP = 0x%08x\n", reg);	//leonid+
	reg=sMacMiiPhyRead (pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_LBR);
	//printf("MII_PHY_LBR = 0x%08x\n", reg);	//leonid+
	reg=sMacMiiPhyRead (pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_TEST);
	//printf("MII_PHY_TEST = 0x%08x\n", reg);	//leonid+
}


#ifdef	SMAC_DEBUG
int	sMacEnableMiiPhyShow;
void sMacMiiPhyShow
    (
    DRV_CTRL	*pDrvCtrl
    )
{
    USHORT reg;

if(sMacEnableMiiPhyShow)
{
	printf("   CR=");
	reg=sMacMiiPhyRead (pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_CR);
	printf("%08x",reg);

	printf("   SR=");
	reg=sMacMiiPhyRead (pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_SR);
	printf("%08x",reg);

	printf("  ID0=");
	reg=sMacMiiPhyRead (pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_ID0);
	printf("%08x",reg);

	printf("  ID1=");
	reg=sMacMiiPhyRead (pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_ID1);
	printf("%08x",reg);

	printf("\n");

	printf("  ANA=");
	reg=sMacMiiPhyRead (pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_ANA);
	printf("%08x",reg);

	printf("ANLPA=");
	reg=sMacMiiPhyRead (pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_ANLPA);
	printf("%08x",reg);

	printf("  ANE=");
	reg=sMacMiiPhyRead (pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_ANE);
	printf("%08x",reg);

	printf("  ANP=");
	reg=sMacMiiPhyRead (pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_ANP);
 	printf("%08x",reg);

	printf("\n");

	printf("  LBR=");
	reg=sMacMiiPhyRead (pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_LBR);
 	printf("%08x",reg);

	printf(" TEST=");
	reg=sMacMiiPhyRead (pDrvCtrl, pDrvCtrl->MII_PhyAD, MII_PHY_TEST);
 	printf("%08x",reg);

	printf("\n");
}
}


int sMacEnableCsrShow;
void sMacCsrShow
    (
    DRV_CTRL	*pDrvCtrl
    )
{
    ULONG reg;

if(sMacEnableCsrShow)
{
    printf("Socle Mac Configuration Registers: CSR\n");

    printf(" R0=");
    reg=sMAC_CSR_READ (pDrvCtrl->devAdrs, CSR0);
 	printf("0x%08x",reg);

    printf(" R1=");
    reg=sMAC_CSR_READ (pDrvCtrl->devAdrs, CSR1);
 	printf("0x%08x",reg);

    printf(" R2=");
    reg=sMAC_CSR_READ (pDrvCtrl->devAdrs, CSR2);
 	printf("0x%08x",reg);

    printf(" R3=");
    reg=sMAC_CSR_READ (pDrvCtrl->devAdrs, CSR3);
 	printf("0x%08x",reg);

	printf("\n");

    printf(" R4=");
    reg=sMAC_CSR_READ (pDrvCtrl->devAdrs, CSR4);
 	printf("0x%08x",reg);

    printf(" R5=");
    reg=sMAC_CSR_READ (pDrvCtrl->devAdrs, CSR5);
 	printf("0x%08x",reg);

    printf(" R6=");
    reg=sMAC_CSR_READ (pDrvCtrl->devAdrs, CSR6);
 	printf("0x%08x",reg);

    printf(" R7=");
    reg=sMAC_CSR_READ (pDrvCtrl->devAdrs, CSR7);
 	printf("0x%08x",reg);

	printf("\n");

    printf(" R8=");
    reg=sMAC_CSR_READ (pDrvCtrl->devAdrs, CSR8);
 	printf("0x%08x",reg);

    printf(" R9=");
    reg=sMAC_CSR_READ (pDrvCtrl->devAdrs, CSR9);
 	printf("0x%08x",reg);

    printf("R10=");
    reg=sMAC_CSR_READ (pDrvCtrl->devAdrs, CSR10);
 	printf("0x%08x",reg);

    printf("R11=");
    reg=sMAC_CSR_READ (pDrvCtrl->devAdrs, CSR11);
 	printf("0x%08x",reg);

	printf("\n");
}
}


int sMacEnableDevShow;
void sMacDevShow
    (
    DRV_CTRL	*pDrvCtrl
    )
{

if(sMacEnableDevShow)
{
	printf("Socle Mac Device Content & Error : pDrvCtrl\n");

	printf("                     rxIndex=");
 	printf("0x%08x",pDrvCtrl->rxIndex);

	printf("        txIndex=");
 	printf("0x%08x",pDrvCtrl->txIndex);

 	printf("      txDiIndex=");
 	printf("0x%08x",pDrvCtrl->txDiIndex);

	printf("\n");

	printf("                     txCount=");
 	printf("0x%08x",pDrvCtrl->txCount);

	printf("   txFrameCount=");
 	printf("0x%08x",pDrvCtrl->txFrameCount);

	printf("  rx_PooliWrite=");
 	printf("0x%08x",pDrvCtrl->rx_PooliWrite);


	printf("\n");
	printf("                rx_PooliRead=");
 	printf("0x%08x",pDrvCtrl->rx_PooliRead);

	printf("  rxMultiFCount=");
 	printf("0x%08x",pDrvCtrl->rxMultiFCount);

	printf("\n\n");

#ifdef SMAC_ERRORDEBUG
	printf("-- txDesErr :");
	printf("  txLostCarrier=");
 	printf("0x%08x",pDrvCtrl->errStats.txLostCarrier);

	printf("    txNoCarrier=");
 	printf("0x%08x",pDrvCtrl->errStats.txNoCarrier);

	printf("txLateCollision=");
 	printf("0x%08x",pDrvCtrl->errStats.txLateCollision);

	printf("\n");

	printf("             ");
	printf(" txExsCollision=");
 	printf("0x%08x",pDrvCtrl->errStats.txExsCollision);

	printf("txCollisonCount=");
 	printf("0x%08x",pDrvCtrl->errStats.txCollisonCount);

	printf("    txUnderflow=");
 	printf("0x%08x",pDrvCtrl->errStats.txUnderflow);

	printf("\n");

	printf("             ");
	printf("     txDeferred=");
 	printf("0x%08x",pDrvCtrl->errStats.txDeferred);

	printf("     txErrorSum=");
 	printf("0x%08x",pDrvCtrl->errStats.txErrorSum);

	printf("\n\n");

	printf("-- rxDesErr :");
	printf("     rxFiltrErr=");
 	printf("0x%08x",pDrvCtrl->errStats.rxFiltrErr);

	printf("      rxDescErr=");
 	printf("0x%08x",pDrvCtrl->errStats.rxDescErr);

	printf("      rxRuntFrm=");
 	printf("0x%08x",pDrvCtrl->errStats.rxRuntFrm);

	printf("\n");

	printf("             ");
	printf("      rxTooLong=");
 	printf("0x%08x",pDrvCtrl->errStats.rxTooLong);

	printf("    rxCollision=");
 	printf("0x%08x",pDrvCtrl->errStats.rxCollision);

	printf("       rxMiiErr=");
 	printf("0x%08x",pDrvCtrl->errStats.rxMiiErr);

	printf("\n");

	printf("             ");
	printf("   rxDribbleBit=");
 	printf("0x%08x",pDrvCtrl->errStats.rxDribbleBit);

	printf("       rxCrcErr=");
 	printf("0x%08x",pDrvCtrl->errStats.rxCrcErr);

	printf("      rxDiscard=");
 	printf("0x%08x",pDrvCtrl->errStats.rxDiscard);

	printf("\n\n");

	printf("-- rxCount  :");
	printf("     rxOverflow=");
 	printf("0x%08x",pDrvCtrl->errStats.rxOverflow);

	printf("       rxMissed=");
 	printf("0x%08x",pDrvCtrl->errStats.rxMissed);

	printf("      rxFifoErr=");
 	printf("0x%08x",pDrvCtrl->errStats.rxFifoErr);

	printf("\n\n");
#endif	/* SMAC_ERRORDEBUG */

#ifdef	SMAC_INTCOUNTER_DEBUG
	printf("-- IntCount :");
	printf("            eri=");
 	printf("0x%08x",pDrvCtrl->int_counter.int_eri);

	printf("            gte=");
 	printf("0x%08x",pDrvCtrl->int_counter.int_gte);

	printf("            eti=");
 	printf("0x%08x",pDrvCtrl->int_counter.int_eti);

	printf("\n");

	printf("             ");
	printf("            rps=");
 	printf("0x%08x",pDrvCtrl->int_counter.int_rps);

	printf("             ru=");
 	printf("0x%08x",pDrvCtrl->int_counter.int_ru);

	printf("             ri=");
 	printf("0x%08x",pDrvCtrl->int_counter.int_ri);

	printf("\n");

	printf("             ");
	printf("            unf=");
 	printf("0x%08x",pDrvCtrl->int_counter.int_unf);

	printf("             tu=");
 	printf("0x%08x",pDrvCtrl->int_counter.int_tu);

	printf("            tps=");
 	printf("0x%08x",pDrvCtrl->int_counter.int_tps);

	printf("\n");

	printf("             ");
	printf("             ti=");
 	printf("0x%08x",pDrvCtrl->int_counter.int_ti);

	printf("\n\n");
#endif	/* SMAC_INTCOUNTER_DEBUG */
}
}
#endif	/* SMAC_DEBUG */
