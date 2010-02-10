#ifndef  _ehcimem_h_
#define  _ehcimem_h_


void QH_Init();
void Add_Free_QH_Ring(PQHEAD freeQH);
PQHEAD Get_Free_QH();
void QTD_Init();
void Add_Free_iTD_Ring(PiTD freeiTD);
PiTD Get_Free_iTD();
BYTE GetFreeiTDSize();

void iTD_Init();
void Add_Free_iTD_Ring(PiTD freeITD);
PQTD Get_Free_QTD();
BYTE GetFreeQHSize();


#endif // _ehcimem_h_
