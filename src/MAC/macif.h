#ifndef _macif_h_included_
#define _macif_h_included_

#include "platform.h"
#include "gMAC.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef	VERIABLE_BYTE_ORDER
	extern	int sMAC_Des_Byte_Order;
	extern	int	sMAC_DataB_Byte_Order;
	inline	int	DESSWAP(int x);
	inline	int	DATABSWAP(int x);
	/* recv xmit descriptor read write */
	#define sMAC_READ_DESC(val_pt)			((ULONG)DESSWAP(*((ULONG *)(val_pt))))
	#define sMAC_WRITE_DESC(val_pt,val)		(*((ULONG *)(val_pt)) = DESSWAP(((ULONG)val)))
	#define sMAC_UPDATA_DESC(val_pt,val)	sMAC_WRITE_DESC(val_pt,sMAC_READ_DESC(val_pt)|val)
	#define sMAC_RESET_DESC(val_pt,val)		sMAC_WRITE_DESC(val_pt,sMAC_READ_DESC(val_pt)&(~val))

	/* Data Buffer Data read write */
	#define sMAC_READ_DATAB(val_pt)			((ULONG)DATABSWAP(*((ULONG *)(val_pt))))
	#define sMAC_WRITE_DATAB(val_pt,val)	(*((ULONG *)(val_pt)) = DATABSWAP(((ULONG)val)))
#endif	/* VERIABLE_BYTE_ORDER */

extern	USHORT 		sMacMiiPhyRead( DRV_CTRL *pDrvCtrl, UINT phyAdrs, UINT phyReg);
extern	void 		sMacMiiPhyWrite( DRV_CTRL*pDrvCtrl, UINT phyAdrs, UINT phyReg, USHORT data);
extern	int			sMacMiiScan( DRV_CTRL * pDrvCtrl);
extern	int			sMacMiiInit( DRV_CTRL * pDrvCtrl);
extern	USHORT 		sMacMiiCheckHighRate( USHORT ablitites);
extern	int			sMacMiiAutoNegGet( DRV_CTRL * pDrvCtrl);
extern	int			sMacMiiReAutoNeg( DRV_CTRL * pDrvCtrl);
extern	int			sMacChipReset( DRV_CTRL * pDrvCtrl);
extern	void		sMacCSRReSetup( DRV_CTRL *	pDrvCtrl);
extern	void		sMacIntInit( DRV_CTRL * pDrvCtrl);
extern	int			sMacInit( DRV_CTRL * pDrvCtrl);
extern	void 		sMacInt( DRV_CTRL * pDrvCtrl);
extern	SMAC_RDE * 	sMacGetFullRMD( DRV_CTRL * pDrvCtrl);
extern	void 		sMacRxSchedule( DRV_CTRL * pDrvCtrl);
extern	int 		sMacRecv( DRV_CTRL * pDrvCtrl, SMAC_RDE * rmd);
extern	int 		sMacFltrFrmXmit( DRV_CTRL * pDrvCtrl, char * pPhysAdrsTbl, int tblLen);
extern	int 		sMacFltrFrmSetup( DRV_CTRL * pDrvCtrl, char * pPhysAdrsTbl, int	tblLen);
extern	void 		sMacHashBuild( char * pPhysAdrsTbl, ULONG *	pFltrFrm, int tblLen);
extern	void 		sMacPacketSend( DRV_CTRL * pDrvCtrl, char * PacketPt, int PacketSize);
extern	void 		sMacTxRingClean( DRV_CTRL * pDrvCtrl);
extern	void 		sMacTxSchedule( DRV_CTRL *	pDrvCtrl);
extern	void 		sMacToDataBufCopy( char *from_pt, char *buf_pt, int length);
extern	void 		sMacFromDataBufCopy( char *buf_pt, char *to_pt, int length);
extern	void 		sMacMiiPhyReadAll( DRV_CTRL *	pDrvCtrl);
extern	void 		sMacMiiPhyShow( DRV_CTRL *	pDrvCtrl);
extern	void 		sMacCsrShow( DRV_CTRL *	pDrvCtrl);
extern	void 		sMacDevShow( DRV_CTRL *	pDrvCtrl);

extern	int			sMacEnableMiiPhyShow;
extern	int 		sMacEnableCsrShow;
extern	int 		sMacEnableDevShow;

#ifdef __cplusplus
}
#endif

#endif // _macif_h_included_
