#ifndef _type_h_included_
#define _type_h_included_

// common data type
typedef volatile unsigned int  data_t;
typedef volatile unsigned int* addr_t;
typedef unsigned int           u32_t;
typedef unsigned char          u8_t;
typedef unsigned short         u16_t;
typedef unsigned long long     u64_t;

typedef unsigned int           u32;
typedef unsigned char          u8;
typedef unsigned short         u16;
typedef unsigned long long     u64;

typedef u16_t __u16;
typedef u32_t __u32;
typedef u64_t __u64;

typedef	unsigned int		boolean;
typedef	unsigned int		BOOLEAN;
typedef	unsigned int		bool;
typedef	unsigned int		BOOL;

typedef	unsigned int		UINT;
typedef	int					INT;
typedef	unsigned short		USHORT;
typedef	short				SHORT;
typedef	unsigned long		DWORD;
typedef	unsigned int		WORD;
typedef	unsigned char		BYTE;
typedef	char				CHAR;
typedef	unsigned char		UCHAR;
typedef unsigned long 		ULONG;

// general define
#ifndef NULL
#define	NULL		0
#endif
#define	null		0

#define	true		-1
#define	TRUE		-1
#define	false		0
#define	FALSE		0
#define	success		-1
#define	SUCCESS		-1
#define	failure		0
#define	FAILURE		0

#endif // _type_h_included_
