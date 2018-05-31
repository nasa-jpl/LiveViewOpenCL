
#ifndef _CHKBUF_H_
#define _CHKBUF_H_

#include "edtinc.h"

#ifdef DOXYGEN_SHOW_UNDOC
/**
  * @weakgroup edt_undoc
  * @{
  */
#endif

#ifdef _NT_
typedef unsigned __int64 u_longlong_t;
#endif

#if defined (sgi)
typedef __uint64_t u_longlong_t;
#endif

#if defined(__linux__)
typedef unsigned long long u_longlong_t ;
#endif

#if defined(__APPLE__)
typedef uint64_t u_longlong_t ;
#endif
/*
 * dump 64 longs
 */
EDTAPI void dumpblk(unsigned char *ptr, int index, int maxindex);

EDTAPI int chkbuf(unsigned char *align, int bufsize, int bufnum, int verbose);

EDTAPI int chkbuf_short(unsigned short *align, int bufsize, int bufnum, 
						int verbose);

/*
 * find the bit alignment in a byte for incrementing bytes
 * (look for the bit that flips each time, it is the lsb)
 */
EDTAPI int bitoffset(unsigned char * bptr);
EDTAPI int bitoffset32(unsigned int * bptr);
EDTAPI int bitoffset64(unsigned int * bptr);

/*
 * copy a buffer andshift a by n bits to the left
 * result buffer is one byte less
 * n is assumed to be less than 8
 */
EDTAPI void cp_shift(unsigned char *from,unsigned char  *to, int shift, int blen);
EDTAPI void cp_shift32(unsigned int *from,unsigned int  *to, int shift, int blen);
EDTAPI void cp_shift64(u_longlong_t *from, u_longlong_t *to, int shift, int blen);
EDTAPI void intswap(u_char *buf, int size);
EDTAPI u_longlong_t iswap(u_longlong_t val);

#ifdef DOXYGEN_SHOW_UNDOC
/** @} */ /* end weakgroup */
#endif

#endif
