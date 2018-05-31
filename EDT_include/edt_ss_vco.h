
#ifndef EDT_SS_VCO_H
#define EDT_SS_VCO_H

#ifdef DOXYGEN_SHOW_UNDOC
/**
  * @weakgroup edt_undoc
  * @{
  */
#endif

#define	XTAL20		10000000.0
#define	XTAL40		20000000.0
#define	XTAL60		30000000.0

#define XTAL_SS		10368100.0


/* PCI SS */
#define F_SS_LOW	 120000000.0


#define F_XILINX_307 200000000.0
#ifdef __cplusplus
extern "C" {
#endif


#define RDW m
#define VDW n
#define OD v


EDTAPI
double
edt_find_vco_frequency_ics307(EdtDev *edt_p, double target,
				double xtal, edt_pll *pll, int verbose);

EDTAPI
double
edt_find_vco_frequency_ics307_nodivide(EdtDev *edt_p, double target,
				double xtal, edt_pll *pll, int verbose);

EDTAPI
double
edt_find_vco_frequency_ics307_raw(EdtDev *edt_p, double target,
				double xtal, edt_pll *pll, int verbose);

EDTAPI
double 
edt_set_frequency_ics307(EdtDev *edt_p, 
						 double ref_xtal, 
						 double target,
						 int clock_channel,
						 int finaldivide);

EDTAPI
double 
edt_set_frequency_fcipcd(EdtDev *edt_p, 
						 double target);

EDTAPI
void
edt_set_out_clk_ics307(EdtDev * edt_p, edt_pll * clk_data, int clock_channel);



#ifdef   __cplusplus
}
#endif

#ifdef DOXYGEN_SHOW_UNDOC
/** @} */ /* end weakgroup */
#endif

#endif


