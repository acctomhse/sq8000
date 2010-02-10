/* EEP25x.c - SOCLE EEPROM 25xxx series Access Libary */

/* Copyright SOCLE-Tech. */

/*
modification history
--------------------
01a,02Dec03,Sherlock  First Written.
*/
#include "global.h"
#include "genlib.h"
#include "EEP25x.h"			/* device struct */

#define	EEP25_DEVICE_NUM		1

EEP25Dev	eep25dev_list[EEP25_DEVICE_NUM];

int	eep25dev_num = 0;

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


int	initEEP25(void)
{
int	i;
	
	eep25dev_num=0;
	
	for(i=0;i<EEP25_DEVICE_NUM;i++)
	{
		eep25dev_list[i].ioStatus = EEP25ioSclose;	
	}
	
	return(TRUE);
}

void 	dummy5(void)
{

}

EEP25Dev	*	openEEP25
	(	
	int devaddr,
	EEP25type type25x,
	int bitmask_sdo,
	int bitmask_sdi,
	int	bitmask_sclk,
	int bitmask_sce
	)
{
EEP25Dev	*	devpt = NULL;	
int	i;

	if(eep25dev_num>=EEP25_DEVICE_NUM)
		return(NULL);
	
	eep25dev_num++;
	for(i=0;i<EEP25_DEVICE_NUM;i++)
	{
		if(eep25dev_list[i].ioStatus == EEP25ioSclose)
		{
			devpt = &eep25dev_list[i];
			break;
		}
	}
	
	devpt->devaddr     	=devaddr;
	devpt->type25x     	=type25x;
	devpt->bitmask_sdo 	=bitmask_sdo;
	devpt->bitmask_sdi 	=bitmask_sdi;
	devpt->bitmask_sclk	=bitmask_sclk;
	devpt->bitmask_sce 	=bitmask_sce;
	devpt->ioStatus		=EEP25ioSopenWRDI;	
	
	eep25_spi_cedi(devpt);
	
	return(devpt);
}

int	closeEEP25
	(
	EEP25Dev	* devpt
	)
{
	devpt->ioStatus = EEP25ioSclose;	
	eep25dev_num--;
	
	return(TRUE);
}

void 	dummy6(void)
{
	
}

int	ioEEP25
	(
	EEP25Dev	* devpt
	)
{
int 	result = TRUE;
	
	if(devpt->ioStatus == EEP25ioSclose)
		return(FALSE);
	
	//Wait Write finish
	if( (devpt->ioStatus == EEP25ioSopenWRENp) )
	{
		eep25_waitWfinsh(devpt);
	}
	
	switch(devpt->ioCmd)
	{
		case	EEP25ioWREN  :	
			eep25_wren(devpt);
		break;	
		case	EEP25ioWRDI 	:
			eep25_wrdi(devpt);
		break;
		case	EEP25ioRDSR 	:	
			result = eep25_rdsr(devpt);		
		break;
		case	EEP25ioWRSR 	:	
			eep25_wrsr(devpt, devpt->ioContent);	
		break;
		case	EEP25ioREAD 	:	
			result = eep25_read(devpt, devpt->ioEEPAddr);
		break;
		case	EEP25ioWRITE :	
			result = eep25_write(devpt, devpt->ioEEPAddr, devpt->ioContent);
		break;
		default	:
			return(FALSE);
		break;
	}
	return(result);
}

void	eep25_spi_ceen(EEP25Dev * devpt)
{
	//CE low, CLK low
	*((int *)devpt->devaddr) = ( *((int *)devpt->devaddr) & (~(devpt->bitmask_sce | devpt->bitmask_sclk)) );
	USDELAY(1);
}

void	eep25_spi_cedi(EEP25Dev * devpt)
{
	USDELAY(1);
	//CE High, CLK low
	*((int *)devpt->devaddr) = ( ( *((int *)devpt->devaddr) & (~(devpt->bitmask_sclk)) ) |  (devpt->bitmask_sce) );
}

void	eep25_spi_write(EEP25Dev * devpt, int data,int len)
{
int	i;
int	bit;

	for(i=1;i<=len;i++)
	{	
		bit = data & (1 << (len-i));
		//CLK Low change Data
		if(bit)
		{
			*((int *)devpt->devaddr)=( (*((int *)devpt->devaddr) | ((devpt->bitmask_sdo))) & (~(devpt->bitmask_sclk))  );
		}
		else
		{
			*((int *)devpt->devaddr)=( (*((int *)devpt->devaddr) & (~(devpt->bitmask_sdo))) & (~(devpt->bitmask_sclk)) );
		}
		USDELAY(1);
		
		//CLK high
		*((int *)devpt->devaddr) = ( *((int *)devpt->devaddr) | ((devpt->bitmask_sclk)) );
		USDELAY(1);
	}		
	
	//CLK Low 
	*((int *)devpt->devaddr) = ( *((int *)devpt->devaddr) & (~(devpt->bitmask_sclk)) );
}

void 	dummy7(void)
{
	
}

int		eep25_spi_read(EEP25Dev * devpt, int len)
{
int	i;
int	bit;
int data;

	data=0;

	for(i=0;i<len;i++)
	{
		//CLK Low 
		*((int *)devpt->devaddr) = ( *((int *)devpt->devaddr) & (~(devpt->bitmask_sclk)) );		
		USDELAY(1);	

		
		data = data <<1;
		bit = *((int *)devpt->devaddr) & ((devpt->bitmask_sdi));
		
		if(bit)
		{
			data = data | 1;
		}
		
		//CLK high
		*((int *)devpt->devaddr) = ( *((int *)devpt->devaddr) | ((devpt->bitmask_sclk)) );
		USDELAY(1);
	}	
	
	//CLK Low 	
	*((int *)devpt->devaddr) = ( *((int *)devpt->devaddr) & (~(devpt->bitmask_sclk)) );
	
	return(data);
}

void	eep25_wren(EEP25Dev * devpt)
{
	if(devpt->ioStatus == EEP25ioSopenWRDI)
	{
		eep25_spi_ceen(devpt);
		eep25_spi_write(devpt, EEP25_CMD_WREN, 8);
		eep25_spi_cedi(devpt);
	}
	devpt->ioStatus		= EEP25ioSopenWRENf;
}	

void	eep25_wrdi(EEP25Dev * devpt)
{
	eep25_spi_ceen(devpt);
	eep25_spi_write(devpt, EEP25_CMD_WRDI, 8);
	eep25_spi_cedi(devpt);

	devpt->ioStatus		= EEP25ioSopenWRDI;
}	

int		eep25_rdsr(EEP25Dev * devpt)
{
int	srdata;	

	eep25_spi_ceen(devpt);
	eep25_spi_write(devpt, EEP25_CMD_RDSR, 8);
	srdata = eep25_spi_read(devpt, 8);
	eep25_spi_cedi(devpt);
	
	return(srdata);
}	

void	eep25_wrsr(EEP25Dev * devpt, int srdata)
{
	eep25_spi_ceen(devpt);
	eep25_spi_write(devpt, EEP25_CMD_WRSR, 8);
	eep25_spi_write(devpt, srdata, 8);
	eep25_spi_cedi(devpt);
	
	devpt->ioStatus		= EEP25ioSopenWRENp;
}	

int		eep25_read(EEP25Dev * devpt, int eepaddr)
{
int	eepaddr_a8;
int	data;

	eepaddr_a8 = eepaddr>>8;	
	eepaddr_a8 = (eepaddr_a8 << EEP25_A8_BITSHIFT);
	eepaddr = eepaddr & 0xFF;
	
	eep25_spi_ceen(devpt);
	eep25_spi_write(devpt, EEP25_CMD_READ | eepaddr_a8, 8);
	eep25_spi_write(devpt, eepaddr, 8);
	data = eep25_spi_read(devpt, 8);
	eep25_spi_cedi(devpt);
	
	return(data);
}

void 	dummy8(void)
{

}

int		eep25_write(EEP25Dev * devpt, int eepaddr, int data)
{
int	eepaddr_a8;

	if(devpt->ioStatus		!= EEP25ioSopenWRENf)
	{
		return(FALSE);
	}

	eepaddr_a8 = eepaddr>>8;	
	eepaddr_a8 = (eepaddr_a8 << EEP25_A8_BITSHIFT);
	eepaddr = eepaddr & 0xFF;
	
	eep25_spi_ceen(devpt);
	eep25_spi_write(devpt, EEP25_CMD_WRITE | eepaddr_a8, 8);
	eep25_spi_write(devpt, eepaddr, 8);
	eep25_spi_write(devpt, data, 8);
	eep25_spi_cedi(devpt);
	
	devpt->ioStatus		= EEP25ioSopenWRENp;
	
	return(TRUE);
}

void 	dummy9(void)
{

}
	
void	eep25_waitWfinsh(EEP25Dev * devpt)
{
int	srdata;
	
	srdata = eep25_rdsr(devpt);
	while(srdata & EEP25_STATUS_BUSY)
	{
		srdata = eep25_rdsr(devpt);
	} 
	devpt->ioStatus		= EEP25ioSopenWRENf;
}
