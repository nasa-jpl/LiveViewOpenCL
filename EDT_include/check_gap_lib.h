/* check_gap_lib.h: public interface file */


#ifndef CHECK_GAP_LIB_H_
#define CHECK_GAP_LIB_H_

#include "/opt/EDTpcd/edtinc.h"

#define MAXFREQ 100000 

#define UINT 0
#define USHORT 1

#define BOARD_TYPE_PCD 0
#define BOARD_TYPE_CL 1
#define BOARD_TYPE_SS 2
#define BOARD_TYPE_UNSUPPORTED 3

#define CHECK_GAP_USAGE_ERROR 1
#define CHECK_GAP_HELP_NEEDED 2

#define PCITESTSS 0x02010010 /* zeros shifted data if bit set */




typedef struct {

	/* For the next two arrays:
	 * let n = array[index], where index is the gapsize and n is the
	 * frequency at which gapsize is observed (or blocksize). */
	u_int gaps[MAXFREQ];
	u_int blocks[MAXFREQ];

	/* *_largest_index: the index of the last non-zero value 
	 * in the gaps/blocks array. It will be somewhere between 0 and MAXFREQ. */
	u_int gaps_largest_index;
	u_int blocks_largest_index;

	u_int last_index;
	double t_xferred;
	double total_clocks;

	int started;

} GapHistory;


/* The following structure is filled with data which is needed 
 * by one or more of the check gap library functions */
typedef struct {
	/* clock_speed is an assumption; user changes this with -F <freq>.
	 * This can also be calculated inside the app, but I'm not doing that */
	float clock_speed;
	int bufsize;
	int loops;
	/* bitload for ss only (for SS, this app runs bitload prior to test) */
	int bitload;
	char *unitstr;

	/* maxfreq: should be set to MAXFREQ. 
	 * this is the size of GapHistory.blocks array */
	int maxfreq; 

	int unit, sunswap;
	int max_channels;

	/* If the user is running a 1 channel bitfile for the SS, it 
	 * needs to be specified with --ss1.  flags mean: 0=no, else 1=yes */
	int ss1_flag, ss4_flag; 

	/* These arrays (edt_p, gaps, finished_count) get allocated after
	 * max_channels is figured out. */
	EdtDev **edt_p; /* Array of pointers for device handles */
	GapHistory *gaps; 
	int *finished_count; /*TODO: figure out what this is for */

	char dev[128]; /* device name (eg. "pcd") */
		
	/* nchannels: # of channels user wishes to use */
	int nchannels; /* set by command line or after board type decided. */
		
	/* default chan to talk to: 0 for SS/PCD, 2 for CL (set in initialize) */
	int default_channel;

	char *rawout; /* rawout, textout: filenames */
	char *textout;
		
	int board_type; /* specifies which card we're using. see BOARD_TYPE_* */
	/* actually, board_type maybe will be set to P..._ID */
	int exact_board_type; /* e.g. PCDA_ID */
	int buf_type; /* whether board deals w/ uint or ushort */
	int type_size; /* number of bytes in board's type */
} CheckGapData;


/** ResultHandler functions deal with the results of the check gap test:
 * usually by printing them (CLI) or displaying them (GUI). */
typedef int (*ResultHandler) (CheckGapData *d, int cur_channel);



/**
 * displays the description for each command line option.
 */
void chkgap_usage();


/** parse user specified options from command line.
 * returns: 0=success, 2=help needed */
int chkgap_parse_options(int argc, char **argv, CheckGapData *d); 

/** open and init board and program state. */
void chkgap_initialize (CheckGapData *d);

/** perform the actual test. */
void chkgap_run_test (CheckGapData *d, ResultHandler handler);

/** swaps bytes. */
void chkgap_do_swab(u_short *buf, u_int size);

/**
 * count_gap takes in an array of unsigned integer buffer and keep
 * track of the number of gaps in the array.
 * @param buf: array of either u_ints or u_shorts, as specified by buf_type.
 *   buf[n] should be one larger than buf[n-1]. If not, there is a gap.
 * @param pgaps: the data structure holding information on gaps and blocks.
 * @param size: the number of elements in the buffer/array
 * @param textout: name of file to send output to.
 * @param buf_type: the type of numbers stored in buf, should be UINT or SHORT
 * @return 0 for success, >0 for failure. (like, 5 means wrong buf_type).
 */
int
chkgap_count_gap (void * buf, u_int size, GapHistory * pgaps, 
				char *textout, int buf_type);

/*
 * output_gap takes in total length, total bytes transferred, total clock,
 * and frequency to display the result to standard output.
 */
int 
chkgap_output_gap(CheckGapData *d, int cur_channel);




void 
chkgap_InitGaps(GapHistory *pgaps);

#endif /* end ifndef CHECK_GAP_LIB_H_ */

