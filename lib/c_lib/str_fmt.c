/* $Id: str_fmt.c,v 1.2 1996/01/16 14:18:30 chris Exp $ */
#include <genlib.h>

/* definitions for fmt parameter of str_fmt(p,width,fmt) */
#define FMT_RJUST 0
#define FMT_LJUST 1
#define FMT_RJUST0 2
#define FMT_CENTER 3

/** str_fmt(p,size,fmt) format p in a field size in width using fmt */
void str_fmt (p, size, fmt)
     char           *p;
     int             size, fmt;
{
    int             n, m, len;

    len = strlen (p);
    switch (fmt) {
    case FMT_RJUST:
	for (n = size - len; n > 0; n--)
	    strichr (p, ' ');
	break;
    case FMT_LJUST:
	for (m = size - len; m > 0; m--)
	    strcat (p, " ");
	break;
    case FMT_RJUST0:
	for (n = size - len; n > 0; n--)
	    strichr (p, '0');
	break;
    case FMT_CENTER:
	m = (size - len) / 2;
	n = size - (len + m);
	for (; m > 0; m--)
	    strcat (p, " ");
	for (; n > 0; n--)
	    strichr (p, ' ');
	break;
    }
}
