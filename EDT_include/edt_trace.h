

#ifndef EDT_TRACE_H
#define EDT_TRACE_H


/* trace support */


#define FILTER_INCLUDE 0
#define FILTER_EXCLUDE 1

EDTAPI void edt_trace(EdtDev *edt_p, FILE *output, int doraw, int loops, u_int *filter, u_int filter_mode);
EDTAPI void edt_trace_file(FILE *input, FILE *output, u_int *filter, u_int filter_mode);
EDTAPI void edt_dump_registers(EdtDev *edt_p, int debug);

#endif
