
#ifndef _PDV_IRIG_H_
#define _PDV_IRIG_H_

/**
The IrigRecord holds the IRIG info at the 
rising edge of frame valid
*/

#define IRIG2_RAW_BCD 5
#define IRIG2_SECONDS 3

#define IRIG2_FIFO      0x010100B2
#define IRIG2_CTRL      0x010100B4
#define IRIG2_READWRITE      0x010100B5

#define IRIG2_SLAVE   0x80        
#define IRIG2_RESET_ERRS 2

#define IRIG2_MAGIC 0x01544445 /* 'E' 'D' 'T' 01 */

#pragma pack (push,1)

typedef struct Irig2Record {
    u_int magic; /* starts at 0 */
    u_int framecnt; /*  magic number */

    /*  There are two modes - seconds from 1970 (unix time)
    or bcd mode, in which the bcd values from IRIG
    are packed into 32 bits in the raw structure
    */

    union {
        u_int seconds;
        ts_raw_t raw;
    } t;

    u_int clocks; /*  how many 40 MHz ticks in last second*/
    u_int tickspps; /*  40 MHz ticks since last second */

    struct {
        u_char type:4;
        u_char irig_ok:1;
        u_char pps_ok:1;
        u_char had_irig_error:1;
        u_char had_pps_error:1;
    } status;

    u_char reserved[3];

    double timestamp; /* holds a 64-bit unix seconds time */
    /* This must be filled in  by software */

} Irig2Record ;

#pragma pack (pop)

EDTAPI int  pdv_reset_dma_framecount(PdvDev *pdv_p);

EDTAPI int  pdv_irig_set_bcd(PdvDev *pdv_p, int bcd);

EDTAPI Irig2Record * pdv_irig_get_footer(PdvDev *pdv_p, u_char * imagedata);

EDTAPI double pdv_irig_process_time(Irig2Record * footer);

EDTAPI void pdv_irig_reset_errors(PdvDev *pdv_p);

EDTAPI void pdv_irig_set_slave(PdvDev *pdv_p, int onoff);

EDTAPI int  pdv_irig_is_slave(PdvDev *pdv_p);

EDTAPI int pdv_irig_set_offset(PdvDev *pdv_p, int offset);

EDTAPI int pdv_irig_get_current(PdvDev *pdv_p, Irig2Record *footer);

#endif
