
#ifndef _EDT_LIBSG_H
#define _EDT_LIBSG_H


#ifdef __cplusplus
extern "C" {
#endif


/***********************************
 * Scatter-gather list routines    
 **********************************/

typedef struct EdtSgListEntry
{
    u_int addr;
    u_int size;
} EdtSgListEntry;


typedef struct EdtSgList {
    u_int entries;
    EdtSgListEntry *list;
    u_char own_array;
} EdtSgList;

EDTAPI void
edt_sglist_init(EdtSgList *p);

EDTAPI void
edt_sglist_print(EdtSgList *sg);

EDTAPI int
edt_sglist_read(EdtDev *edt_p, uint_t sgbuf, EdtSgList * sg);

EDTAPI int
edt_sglist_write(EdtDev *edt_p, uint_t sgbuf, EdtSgList *sg);

EDTAPI int
edt_sglist_set_page(EdtDev *edt_p, int sgbuf, u_int set_to, int verbose);



#ifdef __cplusplus
}

#endif

#endif
