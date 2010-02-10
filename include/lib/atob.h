#ifndef _atob_h_included_
#define _atob_h_included_

/*************************************************************
 *  atob(vp,p,base) 
 *      converts p to binary result in vp, rtn 1 on success
 */
int atob (unsigned int *vp, char *p, int base);
     
/*************************************************************
 *  llatob(vp,p,base) 
 *      converts p to binary result in vp, rtn 1 on success
 */
long long llatob (unsigned long long *vp, char *p, int base);
     
/*************************************************************
 *  char *btoa(dst,value,base) 
 *      converts value to ascii, result in dst
 */
char * btoa (char *dst, unsigned int value, int base);
     
/*************************************************************
 *  char *btoa(dst,value,base) 
 *      converts value to ascii, result in dst
 */
char * llbtoa (char *dst, unsigned long long value, int base);
     
/*************************************************************
 *  gethex(vp,p,n) 
 *      convert n hex digits from p to binary, result in vp, 
 *      rtn 1 on success
 */
int gethex (unsigned long *vp, char *p, int n);

#endif // _atob_h_included_
