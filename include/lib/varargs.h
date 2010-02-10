/* $Id: varargs.h,v 1.3 1997/06/24 07:03:15 chris Exp $ */
#ifndef _VARARGS_
#define _VARARGS_

#define __VARARGS_H

#define __VALIST char*

typedef char *	va_list;

#if __GNUC__ > 1

	#define va_alist	__builtin_va_alist
	#define va_dcl		int __builtin_va_alist; ...
	#define va_start(list)	(list = __builtin_next_arg ())

#else

	#define va_dcl int	va_alist;
	#define va_start(list)	list = (char *) &va_alist

#endif

#define va_end(list)

//#if __mips >= 3

//	#define va_arg(list, mode) ((mode *)(list = (char *) (((int)list + 2*8 - 1) & -8)))[-1]

//#else

	#define va_arg(list, mode) ((mode *)(list = \
		(char *) (sizeof(mode) > 4 ? ((int)list + 2*8 - 1) & -8 \
					   : ((int)list + 2*4 - 1) & -4)))[-1]

//#endif


int vsprintf (char *d, const char *s, __VALIST ap);


#endif /* _VARARGS_ */
