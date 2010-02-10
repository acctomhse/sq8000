/* genlib.h:
 *  Header file for functions in genlib.c (and some others).
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
#ifndef __INCgenlibh
#define __INCgenlibh

#ifdef __cplusplus
extern "C" {
#endif

/* Some compilers consider sizeof() to be unsigned... */
#define sizeof (int)sizeof

extern int optind;
extern char *optarg;

char *	malloc(int nbytes);
void	free(char* ap);
int abort(void);

int abs(int);
int atoi(char *);
int atob(unsigned int *, char *, int);
long long llatob(unsigned long long *, char *, int);
char *itoa (int nValue, char* sz, unsigned int nRadix);
int memcmp(const char *, const char *, int);
int strcmp(char *, char *);
int strncmp(char *, char *, int);
int strlen(char *);
int strspn(char *, char *);
char *memccpy(char *, char *, int, int);
char *memchr(char *, char, int);
char *memcpy(char *, const char *, int);
char *memset(char *, char, int);
int strcspn(char *, char *);
char *strcat(char *, char *);
char *strchr(char *, char);
char *strichr(char *, int);
char *strstr(char *, char *);
char *strcpy(char *, char *);
char *strncat(char *, char *, int);
char *strncpy(char *, char *, int);
char *strpbrk(char *, char *);
char *strrchr(char *, char);
char *strtok(char *, char *);
char *strtolower(char *string);
char *strtoupper(char *string);
long strtol(char *, char **, int);
void qsort(void *, unsigned long, unsigned long, const void *);
unsigned short swap2(unsigned short);
unsigned long swap4(unsigned long);
unsigned long strtoul(char *, char **, int);

#ifdef MONPRINTF
	int mon_printf(char* str, ...);
        #define printf mon_printf
#else
//	int printf(), sprintf(), cprintf();
#endif


int scanf(const char *fmt, ...);
int printf (char *fmt, ...);
int sprintf(char *, const char*, ...);
void str_fmt(char *, int, int);

/* Included here, but not in genlib.c: */
int rputchar(char);
char getchar(void);
int putchar(unsigned char);
int AddrToSym(int,unsigned long,char *,unsigned long *);

int getbytes(char *,int,int);
int putbytes(char *,int);
int gotachar(void);
int getUsrLvl(void);
int setenv(char *,char *);
int shell_sprintf(void);
int getline(char *,int,int);
int getline_t(char *,int,int);
int getline_p(char *,int,int,char *);
void getoptinit(void);
int getopt(int,char **,char *);
int stkchk(char *);
int inRange(char *,int);
int More(void);
int validPassword(char *,int);
int askuser(char *);
int hitakey(void);
int getreg(char *,unsigned long *);
int putargv(int,char *);
int addrtosector(unsigned char *,int *,int *,unsigned char **);
int AppFlashWrite(unsigned long *,unsigned long *,long);
int AppFlashErase(int);
int flushDcache(char *,int);
int invalidateIcache(char *,int);
int pollConsole(char *);
int sectortoaddr(int,int *,unsigned char **);
int sectorProtect(char *,int);
int FlashInit(void);
int cacheInit(void);
int pioget(char,int);
int extendHeap(char *,int);
int decompress(char *,int,char *);
int RedirectionCheck(char *);
int docommand(char *, int);
int SymFileFd(int);
unsigned short xcrc16(unsigned char *buffer,unsigned long nbytes);
unsigned long crc32(unsigned char *,unsigned long);
unsigned long intsoff(void);
unsigned long getAppRamStart(void);
char *line_edit(char *);
char *malloc(int);
char *realloc(char *,int);
//char *getenv(char *);
char *getpass(char *,char *,int);
char *getsym(char *,char *,int);
char *monVersion(void);
char *ExceptionType2String(int);
void Mtrace(void);
void MtraceInit(char *,int);
void monrestart(int);
void historylog(char *);
void free(char *);
void puts(char *);
void MonitorBuiltEnvSet(void);
void writeprompt(void);
void intsrestore(unsigned long);
void prascii(unsigned char *,int);
void cacheInitForTarget(void);
void exceptionAutoRestart(int);
void clrTmpMaxUsrLvl(int (*)(void));
void rawon(void), rawoff(void);
void monHeader(int);
void mstatshowcom(void);
void CommandLoop(void);
void showregs(void), reginit(void);
void initUsrLvl(int);
void warmstart(int);
void coldstart(void);
void InitRemoteIO(void);
void appexit(int);
void pioset(char,int);
void pioclr(char,int);
void getargv(int *argc, char ***argv);
void init1(int), init2(void), init3(void);
void EnableBreakInterrupt(void);
void DisableBreakInterrupt(void);
void ctxMON(void), ctxAPP(void);
#if INCLUDE_REDIRECT
void RedirectCharacter(char);
void RedirectionCmdDone(void);
#else
#define RedirectCharacter(c)
#define RedirectionCmdDone()
#endif

extern unsigned short xcrc16tab[];
extern unsigned long crc32tab[];
extern char *Mtracebuf;
extern char ApplicationInfo[];
extern unsigned long ExceptionAddr;
extern unsigned long APPLICATION_RAMSTART, BOOTROM_BASE;
extern int ConsoleDevice;
extern int ConsoleBaudRate, LoopsPerSecond;
extern int StateOfMonitor, AppExitStatus, ExceptionType;
extern int  moncomptr;
extern int  bss_start, bss_end, boot_base;
extern int  __bss_start[], _end[], _ftext[];
extern int  (*remoterawon)(void), (*remoterawoff)(void);
extern int  (*remoteputchar)(void), (*remotegetchar)(void), (*remotegotachar)(void);
extern int  (*dcacheFlush)(void), (*icacheInvalidate)(void);
extern int  (*extgetUsrLvl)(void);



#ifdef __cplusplus
}
#endif


#endif
