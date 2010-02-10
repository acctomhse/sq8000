/* $Id: scanf.c,v 1.3 1999/03/08 07:34:17 gernot Exp $ */
#if defined(SEMI_HOST_FUNCTION)||defined(SEMI_HOST_FUNCTION_ARM)

#include <varargs.h>
#include <ctype.h>
#include <genlib.h>

#else

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#endif	/* SEMI_HOST_FUNCTION */

#ifdef SEMI_HOST_FUNCTION
#include <epios.h>	//For EPI SemiHost HIF Function
#endif
/*
 * ** fscanf --\    sscanf --\
 * **          |                  |
 * **  scanf --+-- vfscanf ----- vsscanf
 * **
 * ** This not been very well tested.. it probably has bugs
 */

#define MAXLN	512

#define ISSPACE " \t\n\r\f\v"

#ifdef PMCC
main (argc, argv)
     int             argc;
     char           *argv[];
{
    char            buf[40], s[30];
    int             a, b, n;
    char           *fmt = "%d %d";

    if (argc == 2) {
	fmt = argv[1];
    }
    printf ("fmt=[%s]\n", fmt);
    for (;;) {
	printf ("> ");
	gets (buf);
	/*
	 * n = sscanf(buf,fmt,&a,&b);
	 * printf("n=%d a=%d b=%d\n",n,a,b);
	 */
	n = sscanf (buf, fmt, s);
	printf ("n=%d s=[%s]\n", n, s);
    }
}
#endif


/*************************************************************
 *  vsscanf(buf,fmt,ap)
 */
int vsscanf (buf, s, ap)
     char           *buf, *s;
     va_list         ap;
{
    int             count, noassign, width, base, lflag;
    char           *t, tmp[MAXLN];

    count = noassign = width = lflag = 0;
    while (*s && *buf) {
	while (isspace (*s))
	    s++;
	if (*s == '%') {
	    s++;
	    for (; *s; s++) {
		if (strchr ("dibouxcsefg%", *s))
		    break;
		if (*s == '*')
		    noassign = 1;
		else if (*s == 'l' || *s == 'L')
		    lflag = 1;
		else if (*s >= '1' && *s <= '9') {
		    for (t = s; isdigit (*s); s++);
		    strncpy (tmp, t, s - t);
		    tmp[s - t] = '\0';
		    atob (&width, tmp, 10);
		    s--;
		}
	    }
	    if (*s == 's') {
		while (isspace (*buf))
		    buf++;
		if (!width)
		    width = strcspn (buf, ISSPACE);
		if (!noassign) {
		    strncpy (t = va_arg (ap, char *), buf, width);
		    t[width] = '\0';
		}
		buf += width;
	    } else if (*s == 'c') {
		if (!width)
		    width = 1;
		if (!noassign) {
		    strncpy (t = va_arg (ap, char *), buf, width);
		    t[width] = '\0';
		}
		buf += width;
	    } else if (strchr ("dobxu", *s)) {
		while (isspace (*buf))
		    buf++;
		if (*s == 'd' || *s == 'u')
		    base = 10;
		else if (*s == 'x')
		    base = 16;
		else if (*s == 'o')
		    base = 8;
		else if (*s == 'b')
		    base = 2;
		if (!width) {
		    if (isspace (*(s + 1)) || *(s + 1) == 0)
			width = strcspn (buf, ISSPACE);
		    else
			width = strchr (buf, *(s + 1)) - buf;
		}
		strncpy (tmp, buf, width);
		tmp[width] = '\0';
		buf += width;
		if (!noassign)
		    atob ((unsigned int *)va_arg (ap, int), tmp, base);
	    }
	    if (!noassign)
		count++;
	    width = noassign = lflag = 0;
	    s++;
	} else {
	    while (isspace (*buf))
		buf++;
	    if (*s != *buf)
		break;
	    else
		s++, buf++;
	}
    }
    return (count);
}


/*************************************************************
 *  vfscanf(fp,fmt,ap) 
 */
/*
#ifdef SEMI_HOST_FUNCTION_ARM
static int semi_getc(void)
{
	int c;
	int conf[4]={1,&c,1};

	__asm__ __volatile__ (
		"mov r0,#6 \n"
		"mov r1,%[conf]\n"
		"swi #0x123456"
		:
		: [conf] "r" (conf) );

	return c;
#if 0
       	function(0x7) is not work
	__asm__ __volatile__ (
		"mov r0,#7 \n"
		"mov r1,#0\n"
		"swi #0x123456\n"
		"mov %[output],r0"
		:[output] "=r" (c)
		:);
#endif
}
#endif
*/


#if defined(SEMI_HOST_FUNCTION)||defined(SEMI_HOST_FUNCTION_ARM)
int vfscanf (fp, fmt, ap)
	int            fp;
#else
int vfscanf (fp, fmt, ap)
	FILE           *fp;
#endif	/* SEMI_HOST_FUNCTION */
	char           *fmt;
	va_list         ap;
{
	int             count;
	char            buf[MAXLN + 1];
#if defined(SEMI_HOST_FUNCTION_ARM) && !defined(UART_DEBUG)
	unsigned long conf[3]={1,(unsigned long)buf,MAXLN};
#endif

#if defined(SEMI_HOST_FUNCTION)
	if (HIF_read(fp, buf, MAXLN) == 0)
#elif defined(SEMI_HOST_FUNCTION_ARM) && !defined(UART_DEBUG)
	__asm__ __volatile__ (
		"mov r0,#6 \n"
		"mov r1,%[input]\n"
		"swi #0x123456"
		:
		:[input] "r" (conf));
#elif defined(SEMI_HOST_FUNCTION_ARM) && defined(UART_DEBUG)
	{	
		//modified by cyli 060126
		int i;
		for(i=0;i<MAXLN;i++){
		ReInput:	
			buf[i]=getchar();
			
			if((buf[i]>=32)&&(i<(MAXLN-1))){	//if not press control key
				printf("%c",buf[i]);	//print the press key
				continue;
			}
			
			if(buf[i]==8){			//if press backspace
				if(i==0)
					goto ReInput;

				i--;
				printf("%c",8);		//cursor back
				printf("%c",32);	//print space
				printf("%c",8);		//cursor back
				goto ReInput;
			}
			
			if((buf[i]==13)||(i==(MAXLN-1))){//if press enter or equal buf[] size
				buf[i]='\0';
				printf("\n");
				break;
			}
			
			//if press other control key
			goto ReInput;
		}
	}
#else
	if (fgets (buf, MAXLN, fp) == 0)
		return (-1);
#endif	/* SEMI_HOST_FUNCTION */
	count = vsscanf (buf, fmt, ap);

	return (count);
}



/*************************************************************
 *  scanf(fmt,va_alist) 
 */

int 
scanf (const char *fmt, ...)
{
    va_list         ap;
    int             count;

    va_start (ap);
#if defined(SEMI_HOST_FUNCTION)||defined(SEMI_HOST_FUNCTION_ARM)
    count = vfscanf (1, fmt, ap);
#else
    count = vfscanf (stdin, fmt, ap);
#endif	/* SEMI_HOST_FUNCTION */
    va_end (ap);

    return (count);
}

/*************************************************************
 *  fscanf(fp,fmt,va_alist)
 */
int 
#if defined(SEMI_HOST_FUNCTION)||defined(SEMI_HOST_FUNCTION_ARM)
fscanf (int fp, const char *fmt, ...)
#else
fscanf (FILE *fp, const char *fmt, ...)
#endif	/* SEMI_HOST_FUNCTION */
{
	va_list		ap;
	int			count;

	va_start (ap);
	count = vfscanf (fp, fmt, ap);
	va_end (ap);

	return (count);
}

/*************************************************************
 *  sscanf(buf,fmt,va_alist)
 */
int 
sscanf (const char *buf, const char *fmt, ...)
{
	va_list		ap;
	int			count;

	va_start (ap);
	count = vsscanf (buf, fmt, ap);
	va_end (ap);

	return (count);
}

