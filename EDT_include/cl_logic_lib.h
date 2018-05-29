
/* Parse c - link logic analyzer output */

#ifndef CL_LOGIC_LIB_H
#define CL_LOGIC_LIB_H

#define FV_MASK 0x4000
#define LV_MASK 0x2000
#define DV_MASK 0x1000
#define CLOCK_MASK 0xfff

#define CL_LOGIC_MAXLINES	0x8000

#define CL_LOGIC_DEFAULT_BUFSIZE 4096

#include "edtinc.h"

void pdv_cl_logic_set_bufsize(int new_nbuffers, int new_bufsize);

typedef struct line_delta
{
    int     newval;
    int     delta;
    int     n;
}       ClLineDelta;

typedef struct
{
    int     low, high, mean;
    uint64_t sum;
    unsigned int n;
}       ClLogicStat;

/* basic summary of a single frame */

typedef struct frame_summary {
    int frame_blank, line_blank, width, height;
} ClFrameSummary;

/* summary of sequence of frames, which may be of variable size */

typedef struct cl_logic_summary
{
    ClLogicStat 
	frameclocks, 
	width, 
	totallineclocks,
	height, 
	frame_gap, 
	totalframeclocks,
	hblank, 
	start_hblank, 
	end_hblank, 
	current_width,

	hblank_frame;
    int     nLines;
    ClLogicStat line_stats[CL_LOGIC_MAXLINES];

    double pixel_clock;

    int numbufs;
    int bufsize;
    int testmask;
    int timeout;

    /* added to store all frames */

    int nframesallocated;
    int nframes;
    int current_frame;
    ClFrameSummary *frames;

} ClLogicSummary;



EDTAPI void
cl_logic_stat_print(char *label, ClLogicStat * clp, double clockspeed, int verbose);

EDTAPI int
cl_logic_stats_neq(ClLogicStat * clp1, ClLogicStat * clp2);



EDTAPI int
pdv_cl_logic_sample(PdvDev *pdv_p, 
		    FILE *f, 
		    ClLogicSummary *clsp, 
		    int verbose, 
		    int quiet, 
		    int load, 
		    char *outfilename,
		    unsigned int loops,
		    int timeouts,
		    int max_timeouts);

EDTAPI void
cl_logic_summary_init(ClLogicSummary * cls_p, 
		      int testmask, 
		      int numbufs, 
		      int bufsize, 
		      int timeout,
		      double pixel_clock);


#endif
