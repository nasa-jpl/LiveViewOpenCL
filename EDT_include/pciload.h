/* #pragma ident "@(#)pciload.h	1.9 07/11/01 EDT" */
#ifndef INCLUDE_pciload_h
#define INCLUDE_pciload_h

#ifdef _NT_
 #define DIRECTORY_CHAR '\\'
#else
 #define DIRECTORY_CHAR '/'
#endif


extern int sect;

#define	MAX_STRING	128
#define MAX_BOARD_SEARCH 32
#define NUM_DEVICE_TYPES 5

#ifndef _KERNEL

#include "edt_bitload.h"

EDTAPI void   warnuser(EdtDev *edt_p, char *fname, int sector);
EDTAPI void   warnuser_ltx(EdtDev *edt_p, char *fname, int unit, int hub);

EDTAPI int    check_id_stuff(char *bid, char *pid, int devid, int verify_only, char *fname);
EDTAPI void   getinfo(EdtDev *edt_p, int promcode, int segment, char *pid, char *esn, char *osn, int verbose);
EDTAPI void   getinfonf(EdtDev *edt_p, int promcode, int segment, char *pid, char *esn, char *osn, int verbose);

EDTAPI void   program_sns(EdtDev *edt_p, int promtype, char *new_esn, char *new_osn, int sector, char *id, int verbose);
EDTAPI int    edt_flash_get_ftype(EdtDev *edt_p);
EDTAPI int    pciload_maccalc(char *serial, char *maclist);
EDTAPI void   edt_program_flashid(EdtDev *edt_p, u_int addr, u_char *idbuf, u_int idsize, int ftype, int verbose);
EDTAPI int    edt_flash_is_protected(EdtDev *edt_p);
EDTAPI void   edt_get_sns(EdtDev *edt_p, char *esn, char *osn);
EDTAPI void   edt_get_osn(EdtDev *edt_p, char *osn);
EDTAPI int    edt_flash_get_debug_fast();
EDTAPI u_int  edt_flash_writesize(int ftype);
EDTAPI int    edt_flash_set_debug_fast(int val);
EDTAPI int    edt_flash_get_do_fast();
EDTAPI void   edt_flash_set_do_fast(int val);
EDTAPI int    edt_flash_get_force_slow();
EDTAPI void   edt_flash_set_force_slow(int val);
EDTAPI void   edt_flash_block_program(EdtDev *edt_p, u_int addr, u_char *data, u_int nbytes, int ftype);
EDTAPI u_char *edt_flash_block_read(EdtDev *edt_p, u_int addr, u_char *buf, u_int size, int ftype);
EDTAPI char   *edt_flash_read_string(EdtDev *edt_p, u_int addr, char *buf, u_int size, int ftype);
EDTAPI void   edt_zero(void *s, size_t n);
EDTAPI void   edt_get_esn(EdtDev *edt_p, char *esn);
EDTAPI int    edt_get_max_promcode();
EDTAPI char * edt_fmt_promcode(int promcode, char *promcode_str);

EDTAPI int    edt_check_ask_info(EdtDev *edt_p, int promcode, int sect, EdtPromData *pdata, int verify_or_check_only, int proginfo, int nofs);
EDTAPI int    edt_ask_info(int devid, Edt_embinfo *si);
EDTAPI int    edt_ask_reboot(EdtDev *edt_p);
EDTAPI void   edt_ask_options(char *options);
EDTAPI void   edt_ask_rev(int *rev);
EDTAPI void   edt_ask_clock(int *clock, char *extra_txt);
EDTAPI void   edt_ask_pn(char *pn);
EDTAPI void   edt_ask_sn(int devid, char *tag, char *sn, int or_none);
EDTAPI int    edt_ask_nmacs(int devid, int *count, char *maclist);
EDTAPI void   edt_ask_maclist(int count, char *maclist);
EDTAPI int    edt_ask_addinfo();
EDTAPI void   edt_print_info(int devid, Edt_embinfo *si);
EDTAPI int    edt_ask_intfc_fpga(int devid, char *pn, int rev, char *ifx);
EDTAPI void   print_ifx_dirs();
EDTAPI int    edt_read_info_file(u_int devid, char *esn, int esn_size);
EDTAPI int    edt_save_info_file(u_int devid, char *info, int vb); 
EDTAPI char   *edt_make_esn_string(char *str, Edt_embinfo *ei);
EDTAPI char   *edt_merge_esns(char *new_esn, char *esn, char *new_sn, char *new_opts, char *new_pn, char *new_rev, char *new_clock);
EDTAPI void   edt_copy_maclist(char *dest, char *src);
EDTAPI void   edt_print_id_info(EdtPromData *pdata);

EDTAPI void   strip_newline(char *s);
EDTAPI int    pciload_isdigit_str(char *s);


typedef int (*EdtPciLoadVerify) (EdtDev *edt_p, 
                                 EdtBitfile *bitfile, 
                                 EdtPromData *pdata, 
                                 int promcode, 
                                 int segment, 
                                 int vfonly, 
                                 int warn, 
                                 int verbose);

int
program_verify_4013xla(EdtDev *edt_p, 
                        EdtBitfile *bitfile, 
                        EdtPromData *pdata, 
                        int promcode, 
                        int segment, 
                        int vfonly, 
                        int warn,
                        int verbose);
int
program_verify_XC2S300E(EdtDev *edt_p, 
                        EdtBitfile *bitfile, 
                        EdtPromData *pdata, 
                        int promcode, 
                        int segment, 
                        int vfonly, 
                        int warn,
                        int verbose);
int
program_verify_XC2S200(EdtDev *edt_p, 
                        EdtBitfile *bitfile, 
                        EdtPromData *pdata, 
                        int promcode, 
                        int segment, 
                        int vfonly, 
                        int warn,
                        int verbose);
int
program_verify_XC2S150(EdtDev *edt_p, 
                        EdtBitfile *bitfile, 
                        EdtPromData *pdata, 
                        int promcode, 
                        int segment, 
                        int vfonly, 
                        int warn,
                        int verbose);
int
program_verify_4028XLA(EdtDev *edt_p, 
                        EdtBitfile *bitfile, 
                        EdtPromData *pdata, 
                        int promcode, 
                        int segment, 
                        int vfonly, 
                        int warn,
                        int verbose);
int
program_verify_default(EdtDev *edt_p, 
                        EdtBitfile *bitfile, 
                        EdtPromData *pdata, 
                        int promcode, 
                        int segment, 
                        int vfonly, 
                        int warn,
                        int verbose);

int program_verify_SPI(EdtDev *edt_p, 
                       EdtBitfile *bitfile, 
                       EdtPromData *pdata, 
                       int promcode, 
                       int sector, 
                       int verify_only, 
                       int warn, 
                       int verbose);

int program_verify_4013E(EdtDev *edt_p, 
                       EdtBitfile *bitfile, 
                       EdtPromData *pdata, 
                       int promcode, 
                       int sector, 
                       int verify_only, 
                       int warn, 
                       int verbose);

int program_verify_4013XLA(EdtDev *edt_p, 
                       EdtBitfile *bitfile, 
                       EdtPromData *pdata, 
                       int promcode, 
                       int sector, 
                       int verify_only, 
                       int warn, 
                       int verbose);

#endif /* ! KERNEL */

#endif /* INCLUDE_pciload_h */
