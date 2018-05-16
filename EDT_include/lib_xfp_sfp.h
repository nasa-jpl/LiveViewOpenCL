

#ifndef _EDT_LIB_XFP_SFP_H
#define _EDT_LIB_XFP_SFP_H

#include "lib_two_wire.h"

#ifdef __cplusplus
extern "C" {
#endif


/*
* read calibration constants for external calibration option of
* a SFF-8472 compliant transciever module.
*/
typedef struct ext_cal_constants {
    float	rx_pwr[5];
    float	tx_current_slope;
    int		tx_current_offset;
    float	tx_power_slope;
    int		tx_power_offset;
    float	temp_slope;
    int		temp_offset;
    float	volt_slope;
    int		volt_offset;
} EdtSffCalK;


/*
* SFF id structure to read the id rom into 
* overlay on the char array to get easy index for the
* values 
*/

typedef struct id_array {
    unsigned char   xcvr_type; 
    unsigned char   xcvr_ext_type; 
    unsigned char   connector; 
    unsigned char   xcvr_code[8]; 
    unsigned char   encoding; 
    unsigned char   nom_bit_rate; 
    unsigned char   rsvd_8079; 
    unsigned char   fiber_len[5]; 
    unsigned char   rsvd_19; 
    char	    vendor[16]; 
    unsigned char   rsvd_36; 
    unsigned char   vendor_oui[3]; 
    char	    part_number[16]; 
    char	    revision[4]; 
    unsigned short  wavelength; 
    unsigned char   rsvd_62; 
    unsigned char   cc_base; 
    unsigned char   options[2]; 
    unsigned char   bitrate_max; 
    unsigned char   bitrate_min; 
    char	    serial_num[16]; 
    char	    date_code[8]; 
    unsigned char   diag_mon_type; 
    unsigned char   enhanced_opt; 
    unsigned char   sff_compliance; 
    unsigned char   cc_ext;
} EdtSffID;


typedef struct sff_thresholds {
    int	temp[4];
    int	volts[4];
    int	bias[4];
    int	tx_power[4];
    int	rx_power[4];
    int	resved[8];
} EdtSffThresholds;


typedef struct sff_rawvalues {
    int	temp;
    int	volts;
    int	bias;
    int	tx_power;
    int	rx_power;
    int	resved[2];
} EdtSffRawValues;


typedef struct sff_description {
    EdtSffID		id;
    EdtSffThresholds	thresholds;
    EdtSffCalK		calibration;
    EdtSffRawValues	values;
} EdtSffDescription;

/*
* XFP id structure to read the id rom into 
* overlay on the char array to get easy index for the
* values 
*/
typedef struct xfp_id_array {
    unsigned char   xcvr_type; 
    unsigned char   xcvr_ext_type; 
    unsigned char   connector; 
    unsigned char   xcvr_code[8]; 
    unsigned char   encoding; 
    unsigned char   min_bit_rate; 
    unsigned char   max_bit_rate; 
    unsigned char   fiber_len[5]; 
    unsigned char   device_tech; 
    char	    vendor[16]; 
    unsigned char   cdr_support; 
    unsigned char   vendor_oui[3]; 
    char	    part_number[16]; 
    char	    revision[2]; 
    unsigned short  wavelength; 
    unsigned short  wavelength_tol; 
    unsigned char   max_case_temp; 
    unsigned char   cc_base; 
    unsigned char   power[4]; 
    char	    serial_num[16]; 
    char	    date_code[8]; 
    unsigned char   diag_mon_type; 
    unsigned char   enhanced_opt; 
    unsigned char   aux_mon; 
    unsigned char   cc_ext;
} EdtXfpMsaID;


typedef struct xfp_msa_ad_values {
    int	temp;
    int	rsvd;
    int	bias;
    int	tx_power;
    int	rx_power;
    int	aux[2];
} EdtXfpMsaAdValues;

/*
* xfp msa digital diagnostic page
* includes thresholds
*/

typedef struct xfp_dd_array {
    unsigned char dd_array[128]; 
} EdtXfpMsaDD;


typedef struct xfp_description {
    EdtXfpMsaID		id;
    EdtXfpMsaAdValues	values;
    EdtXfpMsaDD		dd;
} EdtXfpDescription;


typedef struct edt_xfp_sfp_device {
    EdtDev	*edt_p;
    int		type;
    int		sfp_ext_cal;
    EdtTwoWire	*tw_ptr; /* hardware register fixed array */

    union {
	EdtSffDescription sff;
	EdtXfpDescription xfp;
    } module;

    int has_module;
    int enabled;
    int laser_on;

} EdtXfpSfpDevice;


/*
* return real translated data for digital diagnostic values
*/
typedef struct real_value{
    char *name;
    char *units;
    double value;
} EdtXfpSfpRealValue;



/* index offsets for the above threshold tables */
#define	HIGH_ALARM	0
#define	LOW_ALARM	1
#define HIGH_WARNING	2
#define LOW_WARNING	3

/* the diagnostic values */
#define	EDT_TEMPERATURE		0
#define	EDT_VOLTAGE		1
#define	EDT_TX_BIAS		2
#define	EDT_TX_POWER_MW		3
#define	EDT_TX_POWER_DB		4
#define	EDT_RX_POWER_MW		5
#define	EDT_RX_POWER_DB		6


EDTAPI EdtXfpSfpDevice *
edt_open_xfp_sfp_device( EdtDev *edt_p );


/*
* close the xfp or sfp device -
* free memory and return
*/

EDTAPI void
edt_close_xfp_sfp_device(EdtXfpSfpDevice * d_ptr);


/*
* read the diagnostic values
* return 0 if fails
*/

EDTAPI int
edt_xfp_sfp_read_current_values(EdtDev * edt_p, EdtXfpSfpDevice * d_ptr);


EDTAPI void
edt_xfp_sfp_real_value(EdtDev * edt_p, 
		       EdtXfpSfpDevice * d_ptr, 
		       int measurement, 
		       EdtXfpSfpRealValue * real_ptr);


EDTAPI void
edt_xfp_sfp_get_status(EdtDev *edt_p, EdtXfpSfpDevice *d_ptr);


EDTAPI void
edt_xfp_sfp_get_status_values(EdtDev *edt_p, int *en_ptr, int *mod_ptr, int *las_ptr);


EDTAPI void
edt_xfp_sfp_enable(EdtDev *edt_p, 
		   u_char enable_xcvr, 
		   u_char enable_laser);

EDTAPI void
edt_xfp_sfp_print_id(EdtDev *edt_p, EdtXfpSfpDevice * d_ptr);



#ifdef __cplusplus
}
#endif

#endif
