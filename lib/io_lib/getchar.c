#include <global.h>

#ifdef	SEMI_HOST_FUNCTION
#include <epios.h>	//For EPI SemiHost HIF Function
#endif	/* SEMI_HOST_FUNCTION */

#ifdef __cplusplus
extern "C" {
#endif


/* If an application is using this source code, then the printf() calls
 * might want to be redirected to mon_printf...
 */

#ifdef CPU_ARCH_MIPS
int abort(void)
{
	__asm__ __volatile__ (
			      "break  0 \n"
			      "j      $31 \n"
			      "nop");
	return 0;

}


int mon_printf(char* str, ...)
{
	int i;
	for( i=0; str[i]!='\0'; i++);
	return i;
}
#endif


#if 0
#ifdef CPU_ARCH_ARM
int	mon_printf(char *s, ...)
{
	__asm__ __volatile__ (
		"mov r0,#4 \n"
		"mov r1,%0\n"
		"swi #0x123456"
		:
		:"r" (s));
}
#endif
#endif



/*************************************************************
 *  fgetc(fp) get char from stream
 */
//#ifdef	SEMI_HOST_FUNCTION
char fgetc (int fp)
//#else
//char fgetc (FILE *fp)
//#endif	/* SEMI_HOST_FUNCTION */
{
#ifdef	SEMI_HOST_FUNCTION
	char	cmd_buf[10];

	HIF_read(fp, &cmd_buf[0], 2);

	return cmd_buf[0];
#else
//	char            c;
//
//	if (fp->ungetcflag) {
//		fp->ungetcflag = 0;
//		return (fp->ungetchar);
//	}
//	if (read (fp->fd, &c, 1) == 0)
//		return EOF;
//
//	return c;
	return 0;
#endif	/* SEMI_HOST_FUNCTION */
}

/*************************************************************
 *  getc(fp) get char from stream
 */
//#ifdef	SEMI_HOST_FUNCTION
char getc (int fp)
//#else
//char getc (FILE *fp)
//#endif	/* SEMI_HOST_FUNCTION */
{
    return fgetc(fp);
}


/*************************************************************
 *  getchar(void) get char from stdin
 */
#if defined(SEMI_HOST_FUNCTION_ARM) && defined(UART_DEBUG)
char getchar(void)
{
	volatile unsigned long *p;
	//modified by cyli 060207
	
#if defined(CONFIG_PDK) || defined(CONFIG_PC7210)
	if(((ioread32(SOCLE_APB0_SCU+0xc) >> 13) & 0x7) !=6)		//20071231 leonid+ for check scu_ucfg
		p=(unsigned long*)SOCLE_APB0_UART1;
	else
		p=(unsigned long*)SOCLE_APB0_UART0;
#else
		p=(unsigned long*)SOCLE_APB0_UART0;
#endif
	while((*(p+5)&(1<<0))!=1);	//wait for data
	return(*p);
}
#endif

#if defined(SEMI_HOST_FUNCTION_ARM) && !defined(UART_DEBUG) 
#if 1
char getchar(void)
{
	char buff[2];
	int conf[4]={1,(int)buff,2};

	do{
	__asm__ __volatile__ (
		"mov r0,#6 \n"
		"mov r1,%[conf]\n"
		"swi #0x123456"
		:
		: [conf] "r" (conf) );
	}while(buff[0]==0);
	buff[1]=0;
	return buff[0];
}

#else
char getchar(void)
{
	char c;

	__asm__ __volatile__ (
		"mov r0,#7 \n"
		"mov r1,#0\n"
		"swi #0x123456\n"
		"mov %[output],r0"
		:[output] "=r" (c)
		:);

	return (c);
}
#endif
#endif // #ifdef SEMI_HOST_FUNCTION_ARM

#ifdef SEMI_HOST_FUNCTION
char getchar (void)
{
//#ifdef	SEMI_HOST_FUNCTION
    return getc(1);
//#else
//	return getc(stdin);
//#endif	/* SEMI_HOST_FUNCTION */
}
#endif


#ifdef __cplusplus
}
#endif
