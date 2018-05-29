
#ifndef _EDTINC_H_
#define _EDTINC_H_


/**
 * @file
 * This file includes all necessary EDT headers, so user applications
 * only need to include this one file.
 */

#ifdef __cplusplus

extern "C" {

#endif

#include "edtdef.h"

/* Include non-edt device stuff */

#include "edt_utils.h"


#ifdef P53B

#include "p53b_dependent.h"

typedef P53BDependent Dependent;

#elif defined(PDV)

#include "pdv_dependent.h"

typedef PdvDependent Dependent;

#else

typedef void * Dependent;

#endif

#include "edtreg.h"

#include "libedt.h"


#ifdef P53B
#include "p53b.h"
#include "p53bio.h"
#ifdef _KERNEL
#include "p53b_reg.h"
#endif
#endif


#ifdef P16D
#include "p16d.h"
#endif

#include "edt_ocm.h"

#ifndef _KERNEL

#include <time.h> 

#include "edt_error.h"

#ifdef PDV
#include "libpdv.h"
#include "libdvu.h"
#include "initcam.h"
#include "edt_bitload.h"
#include "libedt_timing.h"
#include "pdv_irig.h"
#endif /* PDV */

#ifdef PCD
#include "edt_vco.h"
#include "edt_ocx.h"
#include "edt_optstring.h"
#include "libedt_timing.h"
#endif


/* System time functions */
EDTAPI double          edt_dtime(void);
EDTAPI double          edt_timestamp(void);
EDTAPI double          edt_elapsed(u_char reset);

EDTAPI void            edt_msleep(int  msecs) ;
EDTAPI void            edt_usleep(int  usecs) ;
EDTAPI void		edt_usec_busywait(u_int usec);

EDTAPI uchar_t *       edt_alloc(int size) ;
EDTAPI void            edt_free(uchar_t *ptr) ;

EDTAPI DIRHANDLE edt_opendir(const char *dirname);
EDTAPI int edt_readdir(DIRHANDLE h, char *name);
EDTAPI void edt_closedir(DIRHANDLE h);

EDTAPI int64_t edt_disk_free(const char *path);

EDTAPI double edt_dtime();

EDTAPI double edt_timestamp();

EDTAPI HANDLE edt_open_datafile(const char *path,
                    const char *name,
                    u_char writing,
                    u_char direct,
                    u_char truncate);


EDTAPI void edt_close_datafile(HANDLE f);

EDTAPI int edt_write_datafile(HANDLE f, void *p, int bufsize);

EDTAPI int edt_read_datafile(HANDLE f, void *p, int bufsize);

EDTAPI void *edt_alloc_aligned(int size);

EDTAPI void edt_msleep(int msecs);

EDTAPI int             edt_access(char *fname, int perm);
EDTAPI void            edt_correct_slashes(char *str);
EDTAPI void            edt_fwd_to_back(char *str);
EDTAPI void            edt_back_to_fwd(char *str);

EDTAPI u_char edt_wait_for_console_input(char *line,
                                const int input_timeout,  
                                const int maxlen);


EDTAPI int edt_get_datestr(char *s, int maxlen);

EDTAPI uint64_t edt_get_file_position(HANDLE f);

EDTAPI uint64_t edt_get_file_size(HANDLE f);

EDTAPI uint64_t edt_file_seek(HANDLE f, uint64_t pos);

EDTAPI void edt_free_aligned(void *p);


#endif /* _KERNEL */



#ifdef __cplusplus

}

#endif

#endif /* _EDTINC_H_ */

