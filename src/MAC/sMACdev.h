#ifndef _sMACdev_h_included_
#define _sMACdev_h_included_

#include "if_socle.h"

#ifdef MAC_INCLUDE_EEPROM
#include "EEP25x.h"
#endif

#ifdef 	MAC_DATA_BUFFER_SIZEVARIABLE
typedef struct rBuff_Vsize
{
	int		Buffer1_Size;
#ifdef	MAC_DATA_BUFFER2_USED
	int		Buffer2_Size;
#endif
} RBUFF_VSIZE;

typedef struct tBuff_Vsize
{
	int		Buffer1_Size;
#ifdef	MAC_DATA_BUFFER2_USED
	int		Buffer2_Size;
#endif
} TBUFF_VSIZE;

typedef	struct RcvDataPool
{
	int		Frame_Type;
	int		Frame_Length;
	char	DataPool[2048-8];
} RCVDATAPOOL;
#endif

#ifdef	SMAC_INTCOUNTER_DEBUG
typedef struct /* int_counter */
{
	int		int_eri;
	int		int_gte;
	int		int_eti;
	int		int_rps;
	int		int_ru;
	int		int_ri;
	int		int_unf;
	int		int_tu;
	int		int_tps;
	int		int_ti;
} INT_COUNTER;
#endif

#ifdef SMAC_ERRORDEBUG
typedef struct /* dc_stats */
    {
    /* Transmit Error Stats */
    int		txLostCarrier;
    int		txNoCarrier;
    int		txLateCollision;
    int		txExsCollision;
    int		txCollisonCount;
    int		txUnderflow;
    int		txDeferred;
    int		txErrorSum;
    int		txRetries;     
    int		txDropped;     
    int		txFlushCalled;
    int		txFlushNeeded;
    int		txFlushDone;   
    int		txQsizeMax;    
    int		txQsizeCur;    

    /* Receive Error Stats */
    int		rxFiltrErr;  
    int		rxDescErr;   
    int		rxRuntFrm;   
    int		rxTooLong;   
    int		rxCollision; 
    int		rxMiiErr;    
    int		rxDribbleBit;
    int		rxCrcErr;    
    
    int		rxOverflow;  
    int		rxMissed;

	int 	rxFifoErr;
	
    int     rxDiscard;
    
    int		rxLatency;
    int		rxStart;
    int		rxStop;
    } SMAC_STATS;
#endif /* SMAC_ERRORDEBUG */


typedef struct /* drv_ctrl */
{
//	IDR		idr;			/* Interface Data Record */	
#ifdef MAC_INCLUDE_EEPROM
	EEP25Dev	*eepdevpt;	/* EEPROM device stucture point */
#endif
	
	int			NumRds;			/* RMD ring size */
	int			rxIndex;    	/* index into RMD ring */
	SMAC_RDE	*rxRing;		/* RMD ring */
	int			NumTds;			/* TMD ring size */
	int			txIndex;		/* index into TMD ring */
	int			txDiIndex;		/* disposal index into TMD ring */
	SMAC_TDE	*txRing;		/* TMD ring */
	bool		attached;    	/* indicates unit is attached */
	bool		txScheduled;
	int			txCount;		/* Tx Counter */
//	SEM_ID		TxSem;   		/* transmitter semaphore */
	ULONG		OpMode;			/* mode of operation */

	int			ivec;       	/* interrupt vector (MIPS level)*/
	int			ilevel;			/* interrupt level (INTC level)*/
	ULONG		devAdrs;		/* device structure address */

//	ULONG		pciMemBase;		/* memory base as seen from PCI*/
//	int			memWidth;		/* width of data port */
//	CACHE_FUNCS 	cacheFuncs;			/* cache function pointers */
//	int			nLoanRx;				/* number of Rx buffers left to loan */
//	char		*lPool[DC_L_POOL];		/* receive loaner pool ptrs */
//	UINT8		*pRefCnt[DC_L_POOL];	/* stack of reference count pointers */
//	UINT8		refCnt[DC_L_POOL];		/* actual reference count values */
	ULONG		*pFltrFrm;				/* pointer to setup filter frame */
//	MediaBlocksType	dcMediaBlocks;		/* SROM Media Data */
	
	/* SW ISSUE IMPORTANT */
	int			rx_dead;	
	
	int			rx_ignore_error;
	int			tx_merge_counter;
	int			tx_merge_number;
		
#ifdef	SMAC_ERRORDEBUG
	SMAC_STATS	errStats;
#endif

	//////////////////////////////////////////////////////////////////////////
	//	We Add More Setting Here
	//////////////////////////////////////////////////////////////////////////
	ULONG		BusMode;			/* Bus mode */
#ifdef	MAC_DES_BOTHMODE
	int			DesChainMode;
	int			DesSingleBuff1;
	int			DesSingleBuff2;
#endif
#ifdef	MAC_DES_RINGSKIP
	int		 	DesSkipSize;		/* Descriptor Skip size for ring structure */
#endif
#ifdef 	MAC_DATA_BUFFER_SIZEVARIABLE
	RBUFF_VSIZE	*	rBuffSizePt;
	TBUFF_VSIZE	*	tBuffSizePt;
#endif

#ifdef 	MAC_DATA_BUFFER_SIZEVARIABLE
	int			rx_FristNeeded;
	int			rx_SegLength;
	RCVDATAPOOL	*rx_PoolPt;
	char		*rx_PoolCurrentPt;
	int			rx_PooliMax;
	int			rx_PooliWrite;
	int			rx_PooliRead;
	int			rxMultiFCount;	
#endif

	int			AutoPollTx;			/* Auto Polling Tx */
	ULONG		FilterTableType;	/* Filter Table Type Setting */

#ifdef	MAC_RX_POLLSCHEME
	bool		rxScheduled;
#endif
	int			txFrameCount;
	
#ifdef 	SMAC_TEST
	int			Tx_Underflow_Test;
#endif
	
	//////////////////////////////////////////////////////////////////////////
	//	We Add MII Setting Here
	//////////////////////////////////////////////////////////////////////////
	UINT		MII_PhyAD;
	// Request
	UINT		MII_ANA_Setting;
	UINT		MII_Loopback_Enable;
	UINT		MII_Force_Enable;
	UINT		MII_Force_Speed;
	UINT		MII_Force_Duplex;
	// Response
	UINT		MII_Link_OK;
	UINT		MII_AutoNeq_OK;
	UINT		MII_Auto_Speed;
	UINT		MII_Auto_Duplex;

#ifdef	SMAC_INTCOUNTER_DEBUG
	INT_COUNTER	int_counter;
#endif
	char		LocalMacAddr[6];
	char		DestinationMacAddr[6];

} DRV_CTRL;

#endif  // _sMACdev_h_included_
