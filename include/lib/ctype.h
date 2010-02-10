/* ctype.h
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
#ifndef __INCctypeh
#define __INCctypeh

extern int isalnum(int);
extern int isalpha(int);
extern int iscntrl(int);
extern int isdigit(int);
extern int isgraph(int);
extern int islower(int);
extern int isprint(int);
extern int ispunct(int);
extern int isspace(int);
extern int isupper(int);
extern int isxdigit(int);
extern char tolower(char);
extern char toupper(char);

#define _UPPER         0x1      /* Upper case */
#define _LOWER         0x2      /* Lower case */
#define _NUMBER        0x4      /* Number */
#define _WHITE_SPACE   0x8      /* Whitespace */
#define _PUNCT         0x10     /* Punctuation */
#define _CONTROL       0x20     /* Control char */
#define _HEX_NUMBER    0x40     /* Hex digit */
#define _B             0x80     /* Blank */

#define _U  0x1     /* Upper case */
#define _L  0x2     /* Lower case */
#define _N  0x4     /* Numeral (digit) */
#define _S  0x8     /* Whitespace */
#define _P  0x10    /* Punctuation */
#define _C  0x20    /* Control character */
#define _X  0x40    /* Hex */
#define _B  0x80    /* blank */

extern  unsigned char ctypetbl[];

#define isalpha(c)    (ctypetbl[(int)c] & (_U | _L))
#define isupper(c)    (ctypetbl[(int)c] & (_U))
#define islower(c)    (ctypetbl[(int)c] & (_L))
#define isdigit(c)    (ctypetbl[(int)c] & (_N))
#define isxdigit(c)   (ctypetbl[(int)c] & (_X))
#define isspace(c)    (ctypetbl[(int)c] & (_S | _C))
#define ispunct(c)    (ctypetbl[(int)c] & (_P))
#define isalnum(c)    (ctypetbl[(int)c] & (_U | _L | _N))
#define isprint(c)    (ctypetbl[(int)c] & (_P | _U | _L | _S | _N))
#define isgraph(c)    (ctypetbl[(int)c] & (_P | _U | _L | _N))
#define iscntrl(c)    (ctypetbl[(int)c] & (_C | _C_B))
#define toupper(c)    ((('a' <= (c))&&((c) <= 'z')) ? ((c) - 'a' + 'A') : (c))
#define tolower(c)    ((('A' <= (c))&&((c) <= 'Z')) ? ((c) - 'A' + 'a') : (c))

#endif /* __INCctypeh */
