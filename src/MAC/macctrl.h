#ifndef _MACCTRL_H_
#define _MACCTRL_H_

//-+arthur 0407
//extern	int		MACTesting(void);
extern	int		MACTesting(int autotest);
extern	void 	Display_MacMenu(void);
extern	void	MacTestDevCrtlInit(void);
extern	void	MacTestDesInit(void);
extern	void	Init_Rx_Des(void);
extern	void	Init_Tx_Des(void);
extern	void	Init_Tx_DataBuff(void);
extern	void	Clear_Rx_DataBuff(void);
extern	void	MacTestFilterInit(void);
extern	void	MacTestPrepareTx(const char *DesMacAddr, const char *SourceMacAddr, int length, int ID);
extern	void	MacTestPolling(void);
extern	int		TestCheckRxNum(int rxnum);
extern	int		TestCheckRxPattern(const char *DesMacAddr, const char *SourceMacAddr, int length, int ID);

extern	int 	mac_normal_test(int autotest);
extern	int 	mac_phy_clock_test(int autotest);
extern	int 	mac_phy_clock_on_test(int autotest);
extern	int 	mac_phy_clock_off_test(int autotest);


#endif // _MEMCTRL_H_
