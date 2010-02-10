#ifndef _EEP25x_h_included_
#define _EEP25x_h_included_

typedef	enum
{
	EEP25010,
	EEP25020,
	EEP25040
}	EEP25type;

typedef	enum
{
	EEP25ioWREN,
	EEP25ioWRDI,
	EEP25ioRDSR,
	EEP25ioWRSR,
	EEP25ioREAD,
	EEP25ioWRITE
}	EEP25ioCmd;

typedef	enum
{
	EEP25ioSclose,
	EEP25ioSopenWRENf,		//	open write enable and write finish
	EEP25ioSopenWRENp,		//	open write enable and write process
	EEP25ioSopenWRDI		//	open but write disable
}	EEP25ioStatus;

typedef struct
{
	int 			devaddr;
	EEP25type		type25x;
	int 			bitmask_sdo;
	int 			bitmask_sdi;
	int 			bitmask_sclk;
	int 			bitmask_sce;
	EEP25ioStatus	ioStatus;
	EEP25ioCmd		ioCmd;
	int				ioEEPAddr;		//	IO control address
	int 			ioContent;		//	IO control content
} EEP25Dev;

#define	EEP25_CMD_WREN 		0x06	//	Set Write Enable Latch
#define	EEP25_CMD_WRDI 		0x04	//	Reset Write Enable Latch
#define	EEP25_CMD_RDSR 		0x05	//	Read Status Register
#define	EEP25_CMD_WRSR 		0x01	//	Write Status Register
#define	EEP25_CMD_READ 		0x03	//	Read Data from Memory Array
#define	EEP25_CMD_WRITE 	0x02	//	Write Data to Memory Array

#define	EEP25_A8_BITMASK	0x08	//	Extent Address 8 mask
#define	EEP25_A8_BITSHIFT	0x03	//	Extent Address 8 shift

#define EEP25_STATUS_BUSY	(1<<0)
#define	EEP25_STATUS_WEN	(1<<1)
#define	EEP25_STATUS_BP0	(1<<2)
#define	EEP25_STATUS_BP1	(1<<3)

#ifdef __cplusplus
extern "C" {
#endif
// Local referance function
void	eep25_spi_cedi(EEP25Dev * devpt);
void	eep25_spi_ceen(EEP25Dev * devpt);
void	eep25_spi_write(EEP25Dev * devpt, int data,int len);
int		eep25_spi_read(EEP25Dev * devpt, int len);
void	eep25_wren(EEP25Dev * devpt);
void 	eep25_wrdi(EEP25Dev * devpt);
int		eep25_rdsr(EEP25Dev * devpt);
void	eep25_wrsr(EEP25Dev * devpt, int srdata);
int		eep25_read(EEP25Dev * devpt, int eepaddr);
int		eep25_write(EEP25Dev * devpt, int eepaddr, int data);
void	eep25_waitWfinsh(EEP25Dev * devpt);

// Export Extern function
extern	int				initEEP25(void);
extern	EEP25Dev	*	openEEP25(int devaddr, EEP25type type25x, int bitmask_sdo, int bitmask_sdi,	int	bitmask_sclk, int bitmask_sce);
extern	int				closeEEP25(EEP25Dev	* devpt);
extern	int				ioEEP25(EEP25Dev * devpt);

#ifdef __cplusplus
}
#endif

#define	EEP25_I0_WREN(dev)				(EEP25Dev *)dev->ioCmd = EEP25ioWREN;	\
										ioEEP25(dev);

#define	EEP25_I0_WRDI(dev)				(EEP25Dev *)dev->ioCmd = EEP25ioWRDI;	\
										ioEEP25(dev);

#define	EEP25_I0_RDSR(dev,rtn)			(EEP25Dev *)dev->ioCmd = EEP25ioRDSR;	\
										rtn = ioEEP25(dev);

#define	EEP25_I0_WRSR(dev,wdata)		(EEP25Dev *)dev->ioContent = wdata;		\
										(EEP25Dev *)dev->ioCmd = EEP25ioWRSR;	\
										ioEEP25(dev);

#define	EEP25_I0_READ(dev,addr,rtn)		(EEP25Dev *)dev->ioEEPAddr = addr;		\
										(EEP25Dev *)dev->ioCmd = EEP25ioREAD;	\
										rtn = ioEEP25(dev);

#define	EEP25_I0_WRITE(dev,addr,wdata)	(EEP25Dev *)dev->ioEEPAddr = addr;		\
										(EEP25Dev *)dev->ioContent = wdata;		\
										(EEP25Dev *)dev->ioCmd = EEP25ioWRITE;	\
										ioEEP25(dev);

#endif  // _EEP25x_h_included_
