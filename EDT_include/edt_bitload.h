
#ifndef _EDT_BF_H_
#define _EDT_BF_H_

#ifdef __cplusplus
extern "C" {
#endif
/** @file
 * Header file for declarations & defines related to loading the Xilinx
 * bitfile.
 */

/* shorthand debug level */
#define DEBUG0 PDVLIB_MSG_FATAL
#define DEBUG1 EDTLIB_MSG_INFO_1
#define DEBUG2 EDTLIB_MSG_INFO_2


#ifdef DOXYGEN_SHOW_UNDOC
/**
  * @weakgroup edt_undoc
  * @{
  */
#endif

/* amount to read in for bitfile preamble */

#define BFH_EXTRASIZE 128

typedef struct {
    char filename[MAXPATH];
    u_char fi[8];
    u_char ncdname[MAXPATH];
    u_char id[32];
    u_char date[16];
    u_char time[16];
    u_int  dsize; /* get_long_size puts 4-byte dsize here */
    u_int  filesize;
    u_int  key;
    int magic;
    u_int  data_start;
    char promstr[256];
    u_char extra[BFH_EXTRASIZE];
} EdtBitfileHeader;

/** 
 * Retrieve the possibilities 
 * for a particular board for UI bitfile
 */

typedef struct EdtBitfile {
    int is_file;
    char *filename;
    
    u_int full_buffer_size;        /* current size of valid data */
    u_int buffer_allocated;   /* actual allocated size - can be bigger than buffer_size */
    u_char *full_buffer;         /* data */
    u_int cur_index;
    u_char *data;  /* start of actual data */
    u_int data_size; /* size - header */
    HANDLE f;

    EdtBitfileHeader hdr;

}EdtBitfile;

/* structure describing a list of bitfiles
   with assiocated file header data*/

typedef struct _bitfile_list {
    int nbfiles;
    EdtBitfileHeader *bitfiles;
} EdtBitfileList;

/* structure describing a bitfile and 
   carrying the data as file or array */

#define MAX_CHIPS_PER_ID 16

typedef struct {
    u_int id;
    char * fpga_0[MAX_CHIPS_PER_ID];
    char * fpga_1[MAX_CHIPS_PER_ID];
} EdtBoardFpgas;


extern EDTAPI EdtBoardFpgas board_chips[];
extern EDTAPI EdtBoardFpgas mezz_chips[];

#if 0

EDTAPI void sized_buf_init(EdtSizedBuffer *sb);

EDTAPI void sized_buf_destroy(EdtSizedBuffer *sb);

EDTAPI int sized_buf_allocate(EdtSizedBuffer *sb, int size);

#endif


EDTAPI void bf_init(EdtBitfileList *bf);
EDTAPI void bf_destroy(EdtBitfileList *bf);
EDTAPI void bf_add_entry(EdtBitfileList *bf, EdtBitfileHeader *bfh);
EDTAPI void bf_check_and_add(EdtBitfileList *bf, const char *fname);
EDTAPI void bf_sort_entries(EdtBitfileList *bf, int override_name);
EDTAPI int bf_allocate(EdtBitfileList *bf, int size);
EDTAPI int bf_allocate_max_buffer(EdtBitfileList *bf, EdtBitfile *data);
EDTAPI const char *
edt_bitload_basedir(EdtDev *edt_p, const char *in, char *out);

/* gets all possible bitfiles with name fname and stores
   them in list bf. If fpga_hint is present, it will be first 
   in the list. id is either a board id or a mezzanine id, 
   which is used to look up fpga type(s) in the fpga_list.
   */

EDTAPI int edt_get_bitfile_list(const char *basedir,
                         const char *devdir,
                         const char *fname,
                         EdtBoardFpgas *fpga_list,
                         int id,
                         int channel,
                         EdtBitfileList *bf,
                         char *fpga_hint);

EDTAPI int edt_load_mezzfile(EdtDev *edt_p, 
                     const char *basedir, 
                     const char *bitname, 
                     int bitload_flags, 
                     int skip_load, 
                     int channel);

EDTAPI int edt_bitfile_read_header(const char *bitpath, 
                                   EdtBitfileHeader *bfh, 
                                   char *header);

EDTAPI int edt_get_bitfile_header(EdtBitfile *bp,
				  EdtBitfileHeader *bfh);



EDTAPI int edt_access_bitfile(const char *tmppath, int perm);
EDTAPI int edt_oc192_rev2_fname_hack(EdtDev *edt_p, const char *bitname, char *hacked_bitname); 
EDTAPI int edt_program_mezzanine(EdtDev *edt_p, const u_char *buf, u_int size, int channel);
EDTAPI void edt_bitfile_destroy(EdtBitfile *bfd);

#ifdef DO_DIRECT_LOAD
EDTAPI int edt_program_flash_direct(EdtDev *edt_p, const u_char *buf, int size, int do_sleep);
EDTAPI int edt_program_mezzanine_direct(EdtDev *edt_p, const u_char *buf, int size, int channel);
#endif

/** 
 * @addtogroup dma_init
 * @{
 */ 
EDTAPI int edt_bitload(EdtDev *edt_p, const char *basedir, const char *fname, int flags, int skip);
EDTAPI int edt_bitload_from_prom(EdtDev *edt_p, u_int addr1, int size1, u_int addr2, int sized, int flags);
/** @} */ /* end dma_init */

EDTAPI int bitload_has_slashes(const char *name);

void EDTAPI edt_bitload_devid_to_bitdir(int id, char *devdir);
EDTAPI int edt_get_x_header(FILE * xfile, char *header, int *size);

/* flag bits to use in flags param to edt_bitload */
#define BITLOAD_FLAGS_NOFS 0x1 /**< embedded for vxworks */
#define BITLOAD_FLAGS_OVR  0x2 /**< override gs/ss bitfile size constraint */
#define BITLOAD_FLAGS_CH1  0x8 /**< programs ocm channel 1  if BITLOAD_FLAGS_OCM is set */

#define BITLOAD_FLAGS_READBACK 0x20

#define BITLOAD_FLAGS_SLEEP 0x40

#define BITLOAD_FLAGS_MEZZANINE 0x4 /**< changes programming algorithm to program mezzanine boards. */
#define BITLOAD_FLAGS_OCM  0x4 /**< DEPRECATED. Use BITLOAD_FLAGS_MEZZANINE. */
#define BITLOAD_FLAGS_SRXL 0x10 /**< DEPRECATED. Use BITLOAD_FLAGS_MEZZANINE. */


/* program method alternatives */
#define	ALT_INTERFACE	0 
#define ALT_MEZZANINE   1

EDTAPI int edt_bitfile_load_file(EdtBitfile *bfd, const char *name);
EDTAPI int edt_bitfile_load_array(EdtBitfile *bfd, u_char *data, u_int size);
EDTAPI int edt_bitfile_open_file(EdtBitfile *bfd, const char *name, u_char writing);
EDTAPI void edt_bitfile_init(EdtBitfile *bitfile);

EDTAPI void ensure_bitfile_name(const char *name, char *bitname);
EDTAPI int edt_get_x_array_header(u_char *ba, 
                       char *header, 
                       int *size);

/* This does bit load in user space */
/* undefine to do bit load in the driver */

#ifdef __linux__
#ifdef PCD
#ifndef _KERNEL
#define DO_DIRECT_LOAD
#endif
#endif
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif 

#endif


