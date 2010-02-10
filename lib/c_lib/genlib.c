/* genlib.c:
 *  This file has some of the stuff that is typically pulled in by the c
 *  library.  It is provided as local source so that the monitor can be
 *  built without the need for a library.  Some of this code was taken from
 *  the Berkley source code (see notice below).
 *
 *  General notice:
 *  This code is part of a boot-monitor package developed as a generic base
 *  platform for embedded system designs.  As such, it is likely to be
 *  distributed to various projects beyond the control of the original
 *  author.  Please notify the author of any enhancements made or bugs found
 *  so that all may benefit from the changes.  In addition, notification back
 *  to the author will allow the new user to pick up changes that may have
 *  been made by other users after this version of the code was distributed.
 *
 *  Note1: the majority of this code was edited with 4-space tabs.
 *  Note2: as more and more contributions are accepted, the term "author"
 *         is becoming a mis-representation of credit.
 *
 *  Original author:    Ed Sutter
 *  Email:              esutter@lucent.com
 *  Phone:              908-582-2351
 */
/*
 * Copyright (c) 1988, 1993
 *  The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *  This product includes software developed by the University of
 *  California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <ctype.h>
#include <stddefs.h>
#include <global.h>

#ifdef __cplusplus
extern "C" {
#endif

char *BSDCRN = "Copyright (c) 1988, 1993 The Regents of the University of California.  All rights reserved.";

//static char* _mem_chunk = (char*)0x80070000;
#if defined(CONFIG_ARM7)
char* _mem_chunk = (char*)0x42000000;
#elif defined(CONFIG_ARM7_HI)
//static char* _mem_chunk = (char*)0x52000000;
char* _mem_chunk = (char*)0x52000000;
#else
char* _mem_chunk = (char*)0x42000000;
#endif

/* If an application is using this source code, then the printf() calls
 * might want to be redirected to mon_printf...
 */
char * malloc( int nbytes )
{
	char	*last_pos = _mem_chunk;

	// align to word boundary
	nbytes = ((nbytes >> 2) + 1) << 2;
	_mem_chunk += nbytes;

	return last_pos;
}

void	free( char* ap )
{
	return;
}


int
abs(int arg)
{
    return (arg >= 0 ? arg : -arg);
}

int
atoi(char *p)
{
    int n, c, neg;

    neg = 0;
    if (!isdigit((c = *p))) {
        while (isspace(c))
            c = *++p;
        switch (c) {
        case '-':
            neg++;
            break;
        case '+': /* fall-through */
            c = *++p;
            break;
        }
        if (!isdigit(c))
            return (0);
    }
    for (n = '0' - c; isdigit((c = *++p)); ) {
        n *= 10;        /* two steps to avoid unnecessary overflow */
        n += '0' - c;   /* accum neg to avoid surprises at MAX */
    }
    return (neg ? n : -n);
}

unsigned char   adigit_table[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

char*
itoa (int nValue, char* sz, unsigned int nRadix)
{
	unsigned int temp_nValue;
	unsigned int quo;
	unsigned int rem;
	int	 sig=0;
	char *rpt;
	char result[33];

	if(nRadix>16)
	{
		return(NULL);
	}
	result[32]=0;
	rpt=&result[31];

	if((nRadix==10) && (nValue<0))
	{
		sig=1;
		temp_nValue=(unsigned int)(-nValue);
	}
	else
	{
		temp_nValue=(unsigned int)(nValue);
	}
	if(temp_nValue==0)
	{
		*rpt='0';
	}
	else
	{	
		while(1)
		{
			quo = temp_nValue/nRadix;
			rem = temp_nValue%nRadix;
			*rpt=adigit_table[rem];
			if(quo==0)
			{
				break;
			}
			rpt--;
			temp_nValue=quo;
		}
	}

	if(sig)			{rpt--;	*rpt='-';} 
	strcpy(sz,rpt);
	return(sz);
}


unsigned char   ctypetbl[] = {
     0,     _C,     _C,     _C,     _C,     _C,     _C,     _C,
    _C,     _S|_C,  _S|_C,  _S|_C,  _S|_C,  _S|_C,  _C,     _C,
    _C,     _C,     _C,     _C,     _C,     _C,     _C,     _C,
    _C,     _C,     _C,     _C,     _C,     _C,     _C,     _C,
    _S|_B,  _P,     _P,     _P,     _P,     _P,     _P,     _P,
    _P,     _P,     _P,     _P,     _P,     _P,     _P,     _P,
    _N|_X,  _N|_X,  _N|_X,  _N|_X,  _N|_X,  _N|_X,  _N|_X,  _N|_X,
    _N|_X,  _N|_X,  _P,     _P,     _P,     _P,     _P,     _P,
    _P,     _U|_X,  _U|_X,  _U|_X,  _U|_X,  _U|_X,  _U|_X,  _U,
    _U,     _U,     _U,     _U,     _U,     _U,     _U,     _U,
    _U,     _U,     _U,     _U,     _U,     _U,     _U,     _U,
    _U,     _U,     _U,     _P,     _P,     _P,     _P,     _P,
    _P,     _L|_X,  _L|_X,  _L|_X,  _L|_X,  _L|_X,  _L|_X,  _L,
    _L,     _L,     _L,     _L,     _L,     _L,     _L,     _L,
    _L,     _L,     _L,     _L,     _L,     _L,     _L,     _L,
    _L,     _L,     _L,     _P,     _P,     _P,     _P,     _C,
     0,      0,      0,      0,      0,      0,      0,      0,
     0,      0,      0,      0,      0,      0,      0,      0,
     0,      0,      0,      0,      0,      0,      0,      0,
     0,      0,      0,      0,      0,      0,      0,      0,
     0,      0,      0,      0,      0,      0,      0,      0,
     0,      0,      0,      0,      0,      0,      0,      0,
     0,      0,      0,      0,      0,      0,      0,      0,
     0,      0,      0,      0,      0,      0,      0,      0,
     0,      0,      0,      0,      0,      0,      0,      0,
     0,      0,      0,      0,      0,      0,      0,      0,
     0,      0,      0,      0,      0,      0,      0,      0,
     0,      0,      0,      0,      0,      0,      0,      0,
     0,      0,      0,      0,      0,      0,      0,      0,
     0,      0,      0,      0,      0,      0,      0,      0,
     0,      0,      0,      0,      0,      0,      0,      0,
     0,      0,      0,      0,      0,      0,      0,      0
};

/* memccpy():
 * Copy s2 to s1, stopping if character c is copied. Copy no more than n bytes.
 * Return a pointer to the byte after character c in the copy,
 * or NULL if c is not found in the first n bytes.
 */
char *
memccpy(register char *s1,register char *s2,register int c,register int n)
{
    while (--n >= 0)
        if ((*s1++ = *s2++) == c)
            return (s1);
    return (0);
}

/* memchr():
 * Return the ptr in sp at which the character c appears;
 *   NULL if not found in n chars; don't stop at \0.
 */
char *
memchr(register char *sp, register char c,register int n)
{
    while (--n >= 0)
        if (*sp++ == c)
            return (--sp);
    return (0);
}

/* memcmp():
 * Compare n bytes:  s1>s2: >0  s1==s2: 0  s1<s2: <0
 */
int
memcmp(register const char *s1,register const char *s2,register int n)
{
    int diff;

    if (s1 != s2)
        while (--n >= 0)
            if ((diff = (*s1++ - *s2++)))
                return (diff);
    return (0);
}

/* memcpy():
 *  Copy n bytes from 'from' to 'to'; return 'to'.
 *  This version of memcpy() tries to take advantage of address alignment.
 *  The goal is to do as many of the copies on 4-byte aligned addresses,
 *  falling back to 2-byte alignment, and finally, if there is no other
 *  way, simple byte-by-byte copy.
 *  Note that there is some point where the amount of overhead may exceed
 *  the byte count; hence, this will take longer for small byte counts.
 *  The assumption here is that small byte count memcpy() calls don't really
 *  care.
 */
char *
memcpy(char *to,const char *from,int count)
{
    char    *to_copy, *end;

    to_copy = to;

#if INCLUDE_QUICKMEMCPY
    /* If count is greater than 8, get fancy, else just do byte-copy... */
    if (count > 8) {
        /* Attempt to optimize the transfer here... */
        if (((int)to & 3) && ((int)from & 3)) {
            /* If from and to pointers are both unaligned to the
             * same degree then we can do a few char copies to get them
             * 4-byte aligned and then do a lot of 4-byte aligned copies.
             */
            if (((int)to & 3) == ((int)from & 3)) {
                while((int)to & 3) {
                    *to++ = *from++;
                    count--;
                }
            }
            /* If from and to pointers are both odd, but different, then
             * we can increment them both by 1 and do a bunch of 2-byte
             * aligned copies...
             */
            else if (((int)to & 1) && ((int)from & 1)) {
                *to++ = *from++;
                count--;
            }
        }

        /* If both pointers are now 4-byte aligned or 2-byte aligned,
         * take advantage of that here...
         */
        if (!((int)to & 3) && !((int)from & 3)) {
            end = to + (count & ~3);
            count = count & 3;
            while(to < end) {
                *(ulong *)to = *(ulong *)from;
                from += 4;
                to += 4;
            }
        }
        else if (!((int)to & 1) && !((int)from & 1)) {
            end = to + (count & ~1);
            count = count & 1;
            while(to < end) {
                *(ushort *)to = *(ushort *)from;
                from += 2;
                to += 2;
            }
        }
    }
#endif

    if (count) {
        end = to + count;
        while(to < end)
            *to++ = *from++;
    }
    return(to_copy);
}

void
bcopy(char *from, char *to, int size)
{
    memcpy(to,from,size);
}

/* memset():
 * Set an array of n chars starting at sp to the character c.
 * Return sp.
 */
char *
memset(register char *sp, register char c, register int n)
{
    register char *sp0 = sp;

    while (--n >= 0)
        *sp++ = c;
    return (sp0);
}

/* strcat():
 * Concatenate s2 on the end of s1.  S1's space must be large enough.
 * Return s1.
 */

char *
strcat(register char *s1, register char *s2)
{
    register char *os1;

    os1 = s1;
    while(*s1++)
        ;
    --s1;
    while((*s1++ = *s2++))
        ;
    return(os1);
}

/* strchr():
 * Return the ptr in sp at which the character c appears;
 * NULL if not found
 */
char *
strchr(register char *sp, register char c)
{
    do {
        if(*sp == c)
            return(sp);
    } while(*sp++);
    return((char *)0);
}

/* strstr():
 * Find the first occurrence of find in s.
 */
char *
strstr(register char *s, register char *find)
{
    register char c, sc;
    register int len;

    if ((c = *find++) != 0) {
        len = strlen(find);
        do {
            do {
                if ((sc = *s++) == 0)
                    return ((char *)0);
            } while (sc != c);
        } while (strncmp(s, find, len) != 0);
        s--;
    }
    return ((char *)s);
}

/* strcmp():
 * Compare strings:  s1>s2: >0  s1==s2: 0  s1<s2: <0
 */
int
strcmp(register char *s1,register char * s2)
{

    if(s1 == s2)
        return(0);
    while(*s1 == *s2++)
        if(*s1++ == '\0')
            return(0);
    return(*s1 - *--s2);
}

/* strcpy():
 * Copy string s2 to s1.  s1 must be large enough.
 * return s1
 */

char *
strcpy(register char *s1,register char *s2)
{
    register char *os1;

    os1 = s1;
    while((*s1++ = *s2++))
        ;
    return(os1);
}

/* strlen():
 * Returns the number of
 * non-NULL bytes in string argument.
 */
int
strlen(register char *s)
{
    register char *s0 = s + 1;

    while (*s++ != '\0')
        ;
    return (s - s0);
}

/* strncat():
 * Concatenate s2 on the end of s1.  S1's space must be large enough.
 * At most n characters are moved.
 * Return s1.
 */
char *
strncat(register char *s1,register char *s2,register int n)
{
    register char *os1;

    os1 = s1;
    while(*s1++)
        ;
    --s1;
    while((*s1++ = *s2++))
        if(--n < 0) {
            *--s1 = '\0';
            break;
        }
    return(os1);
}

/* strncmp():
 * Compare strings (at most n bytes)
 *  returns: s1>s2; >0  s1==s2; 0  s1<s2; <0
 */
int
strncmp(register char *s1,register char *s2,register int n)
{
    if(s1 == s2)
        return(0);
    while(--n >= 0 && *s1 == *s2++)
        if(*s1++ == '\0')
            return(0);
    return((n < 0)? 0: (*s1 - *--s2));
}

/* strncpy():
 * Copy s2 to s1, truncating or null-padding to always copy n bytes
 * return s1
 */
char *
strncpy(register char *s1,register char *s2,register int n)
{
    register char *os1 = s1;

    while (--n >= 0)
        if ((*s1++ = *s2++) == '\0')
            while (--n >= 0)
                *s1++ = '\0';
    return (os1);
}

/* strpbrk():
 * Return ptr to first occurance of any character from `brkset'
 * in the character string `string'; NULL if none exists.
 */

char *
strpbrk(register char *string,register char *brkset)
{
    register char *p;

    do {
        for(p=brkset; *p != '\0' && *p != *string; ++p)
            ;
        if(*p != '\0')
            return(string);
    }
    while(*string++);
    return((char *)0);
}
/* strrchr():
 * Return the ptr in sp at which the character c last
 * appears; NULL if not found
 */
char *
strrchr(register char *sp, register char c)
{
    register char *r;

    r = (char *)0;
    do {
        if(*sp == c)
            r = sp;
    } while(*sp++);
    return(r);
}

/* strspn():
 * Return the number of characters in the maximum leading segment
 * of string which consists solely of characters from charset.
 */
int
strspn(char *string,register char *charset)
{
    register char *p, *q;

    for(q=string; *q != '\0'; ++q) {
        for(p=charset; *p != '\0' && *p != *q; ++p)
            ;
        if(*p == '\0')
            break;
    }
    return(q-string);
}

/* strtok():
 * uses strpbrk and strspn to break string into tokens on
 * sequentially subsequent calls.  returns NULL when no
 * non-separator characters remain.
 * `subsequent' calls are calls with first argument NULL.
 */

char *
strtok(char *string,char *sepset)
{
    register char   *p, *q, *r;
    static char *savept;

    /*first or subsequent call*/
    p = (string == (char *)0)? savept: string;

    if(p == 0)      /* return if no tokens remaining */
        return((char *)0);

    q = p + strspn(p, sepset);  /* skip leading separators */

    if(*q == '\0')      /* return if no tokens remaining */
        return((char *)0);

    if((r = strpbrk(q, sepset)) == (char *)0)   /* move past token */
        savept = 0; /* indicate this is last token */
    else {
        *r = '\0';
        savept = ++r;
    }
    return(q);
}

#define DIGIT(x)    (isdigit(x) ? (x) - '0' : \
            islower(x) ? (x) + 10 - 'a' : (x) + 10 - 'A')
#define MBASE   ('z' - 'a' + 1 + 10)

long
strtol(register char *str,char **ptr,register int base)
{
    register long val;
    register int c;
    int xx, neg = 0;

    if (ptr != (char **)0)
        *ptr = str; /* in case no number is formed */
    if (base < 0 || base > MBASE)
        return (0); /* base is invalid -- should be a fatal error */
    if (!isalnum((c = *str))) {
        while (isspace(c))
            c = *++str;
        switch (c) {
        case '-':
            neg++;
        case '+': /* fall-through */
            c = *++str;
        }
    }
    if (base == 0) {
        if (c != '0')
            base = 10;
        else if (str[1] == 'x' || str[1] == 'X')
            base = 16;
        else
            base = 8;
    }
    /*
     * for any base > 10, the digits incrementally following
     *  9 are assumed to be "abc...z" or "ABC...Z"
     */
    if (!isalnum(c) || (xx = DIGIT(c)) >= base)
        return (0); /* no number formed */
    if (base == 16 && c == '0' && isxdigit(str[2]) &&
        (str[1] == 'x' || str[1] == 'X'))
        c = *(str += 2); /* skip over leading "0x" or "0X" */
    for (val = -DIGIT(c); isalnum((c = *++str)) && (xx = DIGIT(c)) < base; )
        /* accumulate neg avoids surprises near MAXLONG */
        val = base * val - xx;
    if (ptr != (char **)0)
        *ptr = str;
    return (neg ? val : -val);
}

unsigned long
strtoul(char *str, char **ptr,int base)
{
    long    val;

    val = strtol(str, ptr, base);
    return((unsigned long)val);
}

#if 0
/* tolower():
 * If arg is upper-case, return the lower-case, else return the arg.
 */
int
tolower(register int c)
{
    if(c >= 'A' && c <= 'Z')
        c -= 'A' - 'a';
    return(c);
}

/* toupper():
 * If arg is lower-case, return upper-case, otherwise return arg.
 */
int
toupper(register int c)
{
    if(c >= 'a' && c <= 'z')
        c += 'A' - 'a';
    return(c);
}
#endif

/* strtolower():
 *  In-place modification of a string to be all lower case.
 */
char *
strtolower(char *string)
{
    char *cp;

    cp = string;
    while(*cp) {
        *cp = tolower(*cp);
        cp++;
    }
    return(string);
}

/* strtoupper():
 *  In-place modification of a string to be all upper case.
 */
char *
strtoupper(char *string)
{
    char *cp;

    cp = string;
    while(*cp) {
        *cp = toupper(*cp);
        cp++;
    }
    return(string);
}

ushort
swap2(ushort sval_in)
{
    uchar *cp_in, *cp_out;
    ushort sval_out;

    cp_in = (uchar *)&sval_in;
    cp_out = (uchar *)&sval_out;
    cp_out[0] = cp_in[1];
    cp_out[1] = cp_in[0];
    return(sval_out);
}

ulong
swap4(ulong sval_in)
{
    uchar *cp_in, *cp_out;
    ulong sval_out;

    cp_in = (uchar *)&sval_in;
    cp_out = (uchar *)&sval_out;
    cp_out[0] = cp_in[3];
    cp_out[1] = cp_in[2];
    cp_out[2] = cp_in[1];
    cp_out[3] = cp_in[0];
    return(sval_out);
}


#ifdef __cplusplus
}
#endif
