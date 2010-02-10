#ifndef  _ohcimem_h_
#define  _ohcimem_h_


void ED_Init();
void Add_Free_ED_Ring(PED freeEd);
PED Get_Free_ED();
void GTD_Init();
void Add_Free_GTD_Ring(PGENTD freeGTD);
PGENTD Get_Free_GTD();
void IsoTd_init();
void Add_Free_ISOTD_Ring(PISOTD freeISOTD);
PISOTD Get_Free_ISOTD();

BYTE GetFreeEDSize();


#endif // _ehcimem_h_

