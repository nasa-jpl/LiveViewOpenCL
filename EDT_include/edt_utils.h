
#ifndef _EDT_UTILS_H
#define _EDT_UTILS_H

/**
 * @file
 * This file includes all necessary EDT headers, so user applications
 * only need to include this one file.
 */

#ifdef __cplusplus

extern "C" {

#endif


#include "edt_types.h"

/* #ifdef NO_FS*/
#ifdef VXWORKS
#define MAXPATH 32
#else
#define MAXPATH 256
#endif

/* Include OS-Specific stuff in these headers */

#ifdef _WIN32

#ifndef WIN32
#define WIN32
#endif

#endif

#ifdef WIN32

#include "edt_os_nt.h"

#else
#define EDTAPI

#endif

#ifdef __sun

#include "edt_os_sol.h"

#endif /* __sun */

#ifdef __linux__

#include "edt_os_lnx.h"


#endif

#ifdef __APPLE__
#include "edt_os_mac.h"
#endif

#ifdef VXWORKS

#include "edt_os_vx.h"
#ifdef NO_MAIN
#define exit return
#endif

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
EDTAPI int64_t edt_disk_size(const char *path);
EDTAPI int64_t edt_disk_used(const char *path);
EDTAPI int edt_file_exists(const char *name, int rdonly);
EDTAPI int edt_mkdir(const char *name, int access);

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
	
EDTAPI int edt_mkdir_p(const char *name, int access);

EDTAPI char * edt_strrchr(const char *cp, int ch);

/* To avoid the possibility of problems with
   the min and max macros
*/

#ifndef edt_min
#define edt_min(a,b) ((a) < (b) ? (a):(b))
#endif

#ifndef edt_max
#define edt_max(a,b) ((a) > (b) ? (a):(b))
#endif


/* debugging macros */

#ifdef DEBUG_CALLS

#define FENTER(f) lvl_printf(1,"-->> %s\n", f)

#define FENTER_S(f,s) lvl_printf(1,"-->> %s %s\n", f, s)

#define FENTER_I(f,i) lvl_printf(1,"-->> %s 0x%x\n", f, i)
#define FENTER_I_I(f,i,j) lvl_printf(1,"-->> %s 0x%x 0x%x\n", f, i, j)

#define FRETURN(f) lvl_printf(-1,"<<-- %s\n", f)

#define FRETURN_S(f,s) lvl_printf(-1,"<<-- %s %s\n", f, s)

#define FRETURN_I(f,i) lvl_printf(-1,"<<-- %s 0x%x\n", f, i)

#else

#define FENTER(f) 

#define FENTER_S(f,s)

#define FENTER_I(f,i) 

#define FENTER_I_I(f,i,j) 

#define FRETURN(f) 

#define FRETURN_S(f,s) 

#define FRETURN_I(f,i) 

#endif

#ifdef __cplusplus

}

#endif

#endif /* _EDTINC_H */

