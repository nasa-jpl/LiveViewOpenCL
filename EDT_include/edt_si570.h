
#ifndef EDT_SI570_REGS_H
#define EDT_SI570_REGS_H

#include "lib_two_wire.h"
#define EDT_SI570_NREGS 48


typedef struct _si570_regs {
    u_char div_n1;
    u_char n1_rfreq32;
    u_char rfreq24;
    u_char rfreq16;
    u_char rfreq8;
    u_char reset_ctrl;
    u_char freeze;
} EdtSI570regs;

/* timing specific values */

typedef struct si570_info {
    int n1;
    int hs_div;
    int rfreq_hi;
    int rfreq_fraction;
} EdtSI570;

/* description of all register values */

extern EdtRegisterDescriptor si570regs_map[];

#ifdef __cplusplus
extern "C" {
#endif

EDTAPI u_int get_si570_value(void *target, EdtRegisterDescriptor *map, 
		   int (*get_f)(void *target, int offset, int shift, int mask));


EDTAPI int set_si570_value(void *target, 
		 EdtRegisterDescriptor *map, 
		 u_int value, 
		 int (*set_f)(void *target, int offset, int value, int shift, int mask));

EDTAPI void edt_si570_read_values(EdtDev *edt_p, 
		      u_int base_desc, 
		      u_int device, 
		      EdtSI570 *sip);

EDTAPI void edt_si570_write_values(EdtDev *edt_p, 
		      u_int base_desc, 
		      u_int device, 
		      EdtSI570 *sip);

EDTAPI int edt_si570_dump(EdtDev *edt_p, u_int base_desc, u_int device);

EDTAPI void 
edt_si570_print(EdtSI570 *sip, double refclock);

EDTAPI double edt_vco_si570_compute(double xtal, double target, EdtSI570 *parms);

EDTAPI void edt_si570_reset(EdtDev *edt_p, u_int base_desc, u_int device);

EDTAPI int
edt_si570_set_clock(EdtDev *edt_p, u_int base_desc, 
		    u_int device,
		    double nominal, 
		    double target, 
		    EdtSI570 *parms);

EDTAPI double 
edt_si570_get_clock(EdtDev *edt_p, 
		    u_int base_desc, 
		    u_int device, 
		    double nominal, 
		    EdtSI570 *parms);

#ifdef __cplusplus

}
#endif
#endif
