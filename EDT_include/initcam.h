
#ifndef _INITCAM_H
#define _INITCAM_H

#include "edtinc.h"

enum serial_tag
{
    serial_tag_binary,
    serial_tag_ascii,
    serial_tag_baslerf,
    serial_tag_duncanf
};

typedef struct serial_init_node {
   enum serial_tag tag;
   char *data;
   struct serial_init_node *next;
} SerialInitNode;

/*
 * struct used by readcfg.c and initcam.c, for stuff
 * other than what's in depdendent
 */
typedef struct {
	u_int startdma ;
	u_int enddma ;
	u_int flushdma ;
	u_int timeout ;
    SerialInitNode *serial_init;
    int serial_init_delay;
    u_int cl_mgtspeed;
} Edtinfo;

EDTAPI int  pdv_readcfg(const char *cfgfile, Dependent *pm, Edtinfo *ei_p) ;
EDTAPI int  pdv_readcfg_emb(char *cfgfile, Dependent *pm, Edtinfo *ei_p) ;
EDTAPI void pdv_dep_set_default(PdvDependent * dd_p);
EDTAPI int  printcfg(Dependent *pm) ;


EDTAPI int   pdv_initcam(EdtDev *edt_p, Dependent *dd_p, int unit, Edtinfo *ei_p,
				const char *cfgfname, char *bitdir, int pdv_debug);
EDTAPI int   pdv_initcam_readcfg(char *cfgfile, Dependent * dd_p, Edtinfo * ei_p);
EDTAPI int   pdv_initcam_checkfoi(EdtDev *edt_p, Edtinfo *ei_p, int unit);
EDTAPI Dependent *pdv_alloc_dependent(void);
EDTAPI int   pdv_initcam_load_bitfile(EdtDev * edt_p, Dependent * dd_p,
				int unit, char *bitdir, const char *cfgfname);
EDTAPI int   pdv_initcam_load_bitfile(EdtDev * edt_p, Dependent * dd_p,
				int unit, char *bitdir, const char *cfgfname);
EDTAPI int   pdv_initcam_kbs_check_and_reset_camera(EdtDev * edt_p, Dependent * dd_p);
EDTAPI int   pdv_initcam_reset_camera(EdtDev *edt_p, Dependent *dd_p, Edtinfo *ei_p);
EDTAPI void  pdv_initcam_set_rci(EdtDev *edt_p, int rci_unit) ;

#endif /* _INITCAM_H */
