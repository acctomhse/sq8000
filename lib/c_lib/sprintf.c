/* $Id: sprintf.c,v 1.2 1996/01/16 14:18:28 chris Exp $ */
#include <varargs.h>

/*************************************************************
 *  sprintf(buf,fmt,va_alist) send formatted string to buf
 */
int 
sprintf (char *buf, const char *fmt, ...)
{
    int             n;

    va_list ap;
    va_start (ap);
    n = vsprintf (buf, fmt, ap);
    va_end (ap);
    return (n);
}
#if  0
int 
sprintf (buf, fmt, va_alist)
     char *buf;
     const char *fmt;
     va_dcl
{
    va_list         ap;
    int             n;

    va_start (ap);
    n = vsprintf (buf, fmt, ap);
    va_end (ap);
    return (n);
}
#endif
