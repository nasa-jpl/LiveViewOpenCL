
#ifndef EDT_VCO_H
#define EDT_VCO_H

#define SSD_REF_XTAL    10000000.0
#define	XTAL20		10000000.0
#define	XTAL40		20000000.0
#define	XTAL60		30000000.0
#define	LOW_REF		225000.0
#define	HI_REF		5000000.0
#define LOW_VCO 	50000000.0
#define HI_VCO		250000000.0
#define	F_XILINX	100000000.0

/* maximum speed out of the PLL */
/* PCD / PDV */
#define F_LOW		 30000000.0

/* PCI SS */
#define F_SS_LOW	 120000000.0


#define F_XILINX_307 200000000.0
#ifdef __cplusplus
extern "C" {
#endif

EDTAPI
double
edt_find_vco_frequency(EdtDev *edt_p, double target, double xtal,
					edt_pll *pll, int verbose);
EDTAPI
void
edt_set_pll_clock(EdtDev *edt_p, int ref_xtal, edt_pll *clkset, int verbose);


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
edt_set_frequency_ics307(EdtDev *edt_p, 
						 double ref_xtal, 
						 double target,
						 int clock_channel,
						 int finaldivide);

EDTAPI
void
edt_set_out_clk_ics307(EdtDev * edt_p, edt_pll * clk_data, int clock_channel);


#ifdef   __cplusplus
}
#endif

#endif


