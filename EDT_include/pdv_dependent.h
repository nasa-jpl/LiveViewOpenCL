/**
 * @file
 * PCI DV Dependent structure and defines
 */
#ifndef _PDV_DEPENDENT_H_
#define _PDV_DEPENDENT_H_

#define CAMNAMELEN	128
#define FNAMELEN	256
#define MAXINIT 1024
#define OLDMAXINIT 256
#define KBSFNAMELEN 64
#define CAMCLASSLEN 64

#define MAXSER		32
#define MAXXIL		32

/* just a simple overload of file name structure elements to save space.... */
#define DD_P_CAMERA_DOWNLOAD_FILE dd_p->camera_download_file
#define DD_P_CAMERA_COMMAND_FILE dd_p->camera_command_file


#define MAX_INTLV_TAPS 8

/* this is supposed to be 32 bytes to match old KBS stuff - in the long run it doesn't really matter */
#define FULL 1

/* CLSIM control interface */
typedef struct _sim_control {


    union
    {
        struct {
            /* cfga*/
            unsigned int linescan:1;
            unsigned int lvcont:1;
            unsigned int rven:1;
            unsigned int uartloop:1;
            unsigned int smallok:1;
            unsigned int intlven:1;
            unsigned int firstfc:1;
            unsigned int datacnt:1;
            /* cfgb */
            unsigned int dvskip:4;
            unsigned int dvmode:4;
            /* cfgc */
            unsigned int led:1;
            unsigned int trigsrc:1;
            unsigned int trigpol:1;
            unsigned int trigframe:1;
            unsigned int trigline:1;
        } Cfg;
        struct {
            u_char cfga;
            u_char cfgb;
            u_char cfgc;
        } b;
    } flags;


    u_char  Exsyncdly;
    u_char  FillA;
    u_char  FillB;
    u_char  taps;

    /* if hblank is specified, hcntmax varies with hactv in library routines */
    unsigned short hblank;
    unsigned short Hcntmax;
    unsigned short dummy;

    /* if vgap is specified, vcntmax varies with vactv in library routines */

    unsigned int   vblank; /* vactv + vgap = Vcntmax */
    unsigned int   Vcntmax;

    unsigned short Hfvstart;
    unsigned short Hfvend;
    unsigned short Hlvstart;
    unsigned short Hlvend;
    unsigned short Hrvstart;
    unsigned short Hrvend;

    float   pixel_clock;
    double  si570_nominal;

} ClSimControl;

/* General purpose reorder descriptor */
/* tap starts at startx, starty */
/* sample at dx,dy for length pixels */
/* go to startx, starty + stridex,stridey */

#define PDV_DIM_END -1
#define PDV_MAXTAPS 8

typedef struct _tap_descriptor {
    int startx; /* PDV_DIM_END means at end of row */
    int starty; /* PDV_DIM_END means at last row */
    int dx;
    int dy;
    int length;
    int stridex; /* PDV_DIM_END means at end of row */
    int stridey; /* PDV_DIM_END means at last row */
} PdvInterleaveTap;

/**
 * The PdvDependent structure holds PDV specific information inside the
 * #PdvDev structure.
 *
 * In the PDV software package, the file \e edtinc.h
 * defines the type \c Dependent to be \c PdvDependent.
 *
 * For portability, we strongly recommend using the \ref dv calls rather
 * than accessing the structure elements directly.
 */
typedef struct _PdvDependent
{
    /* note: size of dependent must be DEPSIZE (currently 4096) */
    int     width;
    int     height;
    int     depth;
    int     imagesize;
    int     extdepth;
    int     slop;
    int     hwpad;
    int     byteswap;
    int     image_offset;
    int     interlace;
    int     shutter_speed;
    int     shutter_speed_frontp;
    int     shutter_speed_min;
    int     shutter_speed_max;
    int     default_shutter_speed;
    int     aperture;
    int     aperture_min;
    int     aperture_max;
    int     default_aperture;
    int     gain_frontp;
    int     gain_min;
    int     gain_max;
    int     default_gain;
    int     offset_frontp;
    int     offset_min;
    int     offset_max;
    int     continuous;
    int     pclock_speed;
    int     hwinterlace;
    int     inv_shutter;
    int     trig_pulse;
    int     dis_shutter;
    int     mode_cntl_norm;
    int     trigdiv; /* was dma_throttle */
    int     maxdmasize;
    int     direction;
    int     cameratest;
    int     genericsim;
    int     sim_width;
    int     sim_height;
    int     line_delay;
    int     frame_delay;
    int     frame_height;
    int     fv_once;
    int     enable_dalsa;
    int     lock_shutter;
    int     camera_shutter_timing;
    int     camera_continuous;
    int     camera_binning;
    int     camera_data_rate;
    int     camera_download;
    int     get_gain;
    int     get_offset;
    int     set_gain;
    int     set_offset;
    int     default_offset;
    int     first_open;
    int     last_close;
    int     pingpong_varsize;
    int     image_depth;
    int     header_type;
    int     interlace_offset;
    int     get_aperture;
    int     set_aperture;
    int     timeout_multiplier;
    int     rgb30; /* was pdv_type */
    int     sim_speed;
    int     swinterlace;
    int     started;
    int     foi_unit;
    int     disable_mdout;
    int     fixedlen;
    int     framesync_mode; /* was saved_speed */
    int     camera_shutter_speed;
    int     dual_channel;
    int     gain;
    int     level;
    int     gendata;
    int     skip;
    int     shortswap;
    int     markras;
    int     markbin;
    int     rascnt;
    int     markrasx;
    int     markrasy;
    int     xilinx_rev;
    int     xilinx_opts;
    int     util2;
    int     shift;
    int     mask;
    int     linerate;
    int     header_size;
    int     photo_trig;
    int     fieldid_trig;
    int     acquire_mult;
    int     timeout;
    int     sim_enable;
    int     startup_delay; /* was xilinx_clk */
    u_char *last_raw;
    u_char *last_image;
    u_char  datapath_reg;
    u_char  config_reg;
    u_char  xilinx_flag[MAXXIL];
    u_char  xilinx_value[MAXXIL];
    char    cameratype[CAMNAMELEN];
    char    rbtfile[FNAMELEN];
    char    cfgname[FNAMELEN];
    char    foi_remote_rbtfile[FNAMELEN];
    char    interlace_module[FNAMELEN];
    char    serial_init[OLDMAXINIT];  /* moved to Edtinfo; not used EXCEPT pdv_flshow; can overload when that goes away */
    char    foi_init[OLDMAXINIT];
    char    xilinx_init[OLDMAXINIT];
    char    serial_exposure[MAXSER];
    char    serial_gain[MAXSER];
    char    serial_offset[MAXSER];
    char    serial_prefix[MAXSER];
    char    serial_binning[MAXSER];
    char    RESERVED1[MAXSER];
    char    RESERVED2[MAXSER];
    char    camera_model[MAXSER];
    char    camera_info[MAXSER*2];
    char    serial_response[MAXSER];
    char    RESERVED4[MAXSER];
    char    serial_term[MAXSER];
    char    idstr[FNAMELEN];
    int     serial_timeout;
    int     serial_respcnt;
    int     serial_format;
    int     strobe_count;
    int     strobe_interval;
    int     strobe_enabled;
    int     sel_mc4;
    int     mc4;
    int     serial_baud;
    int     user_timeout;
    int     user_timeout_set;
    int     cam_width;		    /* camera's real width/height, */
    int     cam_height;		    /* for ref. in case of ROI */
    int     hskip;		        /* horiz pixels to skip */
    int     hactv;		        /* horiz. active pixels */
    int     vskip;		        /* vert. pixels to skip */
    int     vactv;		        /* vert. active pixels */
    u_int   RESERVEDUINT1;      /* was clr_cont_end */
    u_int   RESERVEDUINT2;      /* was clr_cont_start */
    u_int   RESERVEDUINT3;      /* was clr_cont_count */
    u_int   cnt_continuous;
    u_int   sim_ctl;	/* bits for simulator */
    int     started_continuous;
    int     kbs_red_row_first;
    int     kbs_green_pixel_first;
    ClSimControl cls;
    char    camera_download_file[KBSFNAMELEN];
    char    camera_command_file[KBSFNAMELEN];
    char    camera_class[CAMCLASSLEN];
    int     double_rate;
    int     force_single;
    char    serial_trigger[MAXSER];
    int     variable_size;
    int     header_offset;
    int     header_dma;
    int     header_position;
    int     pause_for_serial;
    int     roi_enabled;
    int     binx;
    int     biny;
    int     serial_mode;
    int     inv_ptrig;
    int     inv_fvalid;
    int     cl_cfg;
    int     cl_data_path;
    int     htaps;
    int     vtaps;
    int     pulnix;
    int     dbl_trig;
    int     cameralink;
    int     start_delay;
    u_int   serial_waitc ;
    int     frame_period ;
    int     frame_timing ;
    int     register_wrap ;
    char   *serial_binit ;   /* moved to Edtinfo; not used EXCEPT pdv_flshow; can overload when that goes away */
    int	   fval_done;
    int    cl_hmax;
    int    serial_init_delay ; /* moved to Edtinfo; not used EXCEPT pdv_flshow; can overload when that goes away */
    int    mode16;
    /* new for config editor */
    int	    n_intlv_taps; /* camera taps for interleave not necessarily same as CL channels */
    int	    cl_channels;
    PdvInterleaveTap intlv_taps[MAX_INTLV_TAPS];

    /* NOTE: rename/reuse RESERVEDn variables before creating new ones */
    /* Keep the same order as much as possible */

    int	flushdma;
    int startdma;
    int enddma;
    int cl_cfg2;
    u_char irig_raw;
    u_char irig_offset; /* seconds to add to irig */
    u_char irig_slave;

}       PdvDependent;

typedef PdvDependent pdv_methods;

#define KODAK_AIA_MCL            1        /* mode control (typ. CC1) pulse-width */
#define AIA_MCL                  1        /* same as KODAK */
#define IRC_160                  2        /* IRC 160 */
#define AIA_SERIAL               3        /* trigger pulse (1 millisecond) */
#define KODAK_AIA_SER            3        /* dup */
#define AIA_SER                  3        /* dup */
#define KODAK_XHF_INTLC          4        /* horizontal interlace */
#define PDV_BYTE_INTLV           4        /* horizontal interlace */
#define KODAK_XHF_SKIP           5        /* subsample */
#define PDV_BYTE_INTLV_SKIP      5        /* subsample */
#define IMAGE_MERGE              6        /* merge */
#define PDV_FIELD_INTLC          6        /* merge */
#define HW_ONLY                  7
#define ES10_WORD_INTLC          8        /* need to deinterlace words */
#define PDV_WORD_INTLV           8        /* need to deinterlace words */
#define PDV_BGGR                 9        /* expermental */
#define PDV_BGGR_DUAL            10        /* expermental */
#define PDV_BGGR_WORD            11        /* expermental */
#define PDV_BYTE_TEST1           12        /* temporary */
#define PDV_BYTE_TEST2           13        /* temporary */
#define PDV_BYTE_TEST3           14        /* temporary */
#define KODAK_SER_14I            21        /* Kodak chgd their AIA syntax */
#define HAM_4880_SER             22        /* AIA serial controlled camera */
#define PULNIX_TM1000            23        /* AIA_MCL controlled camera */
#define PULNIX_TM9700            24        /* AIA_MCL controlled camera */
#define KODAK_DIG_GAIN           25        /* AIA_MCL w/digital gain */
#define FOI_REMOTE_AIA           26        /* AIA Remote Camera Interface */
#define KODAK_AIA_SER_CTRL       27        /* AIA serial but use shutter ctrl */
#define AIA_SER_CTRL             27
#define DALSA_CONTINUOUS         28        /* DALSA D71024T continuous mode */
#define HAM_4880_8X              29        /* AIA serial controlled camera */
#define HAM_4742_95              30        /* AIA serial controlled camera */
#define PROGRES                  31
#define AIA_MC4                  32        /* MCL gain only for photonics gain */
#define SMD_SERIAL               33        /* SMD binary serial method */
#define AIA_SERIAL_ES40          34        /* Kodak modified AIA */
#define PDV_DALSA_4CH_INTLV      35        /* Dalsa four channel interleave (frame) */
#define SPECINST_SERIAL          36        /* Spectral Instruments */
#define PDV_INVERT_RIGHT_INTLV   37        /* even pixels are right half, inverted */
#define PDV_DALSA_2CH_INTLV      PDV_INVERT_RIGHT_INTLV
#define PDV_WORD_INTLV_ODD       38        /* sameas WORD_INTLV but odd ln 1st */
#define PDV_DDCAM                39        /* EDT/Delta Design camera */
#define PDV_DALSA_LS             40
#define PDV_HEADER_BEFORE        41        /* header data comes before img data */
#define PDV_HEADER_AFTER         42        /* header data comes after img data */
#define PDV_HEADER_WITHIN        43        /* header data is within def'd img */
/* size - use hdr offs to dtmn where */
#define PDV_WORD_INTLV_HILO      44        /* deintrlace words, high/low */
#define PDV_SPECINST_4PORT_INTLV 45        /* Spectral Instruments 4 port intlv */
#define PDV_QUADRANT_INTLV       46        /* quadrant intlv */
#define PDV_ILLUNIS_INTLV        47        /* illunis quadrant intlv */
#define PDV_ILLUNIS_BGGR         48        /* illunis quadrant intlv + bggr */

#define PDV_ES10_BGGR            49        /* Roper Horizontal Intlc w/ bayer */
#define PDV_ES10_WORD_BGGR       50        /* Roper Horizontal Intlc w/ bayer */
#define PDV_ES10_WORD_ODD_BGGR   51        /* Roper Horizontal Intlc w/ bayer */
#define AIA_MCL_100US            52        /* AIA_MCL w/100 microsec decade ctr */
#define PDV_PIRANHA_4CH_INTLV    53        /* Piranha four channel interleave */
#define PDV_FMRATE_ENABLE        54        /* Frame Rate Enable flag */
#define PDV_FVAL_ADJUST          55        /* Frame Valid Adjust flag */
#define BASLER_202K              56        /* Basler A202K */
#define DUNCAN_2131              57        /* Basler A202K */
#define PDV_EVEN_RIGHT_INTLV     58        /* even pixels are right half */
#define PDV_DALSA_2M30_INTLV     PDV_EVEN_RIGHT_INTLV
#define PDV_INV_RT_INTLV_24_12   59        /* clink 24-12 DALSA_2CH_INLTV */
#define PDV_INTLV_24_12          60        /* clink 24-12  */
#define PDV_INTLV_1_8_MSB7       61        /* one-bit , pixel 0 at lsb */
#define PDV_INTLV_1_8_MSB0       62        /* one-bit , pixel 0 at msb */
#define PDV_INTLV_BGR_2_RGB      63        /* BGR -> RGB */
#define PDV_INTLV_USER           64        /* look up  */
#define SU320_SERIAL             65        /* Sensor Unlmited 320M serial expose method */
#define BASLER202K_SERIAL        66        /* Basler 202K serial expose method */
#define ADIMEC_SERIAL            67        /* Adimec serial expose method */
#define TIMC1001_SERIAL          68        /* TI MC1001 serial expose method */
#define PTM6710_SERIAL           69        /* Pulnix TM-6710 serial expose method */
#define PTM1020_SERIAL           70        /* Pulnix TM-1020 serial expose method */
#define AIA_TRIG                 71        /* new: explicit triggered mode, sets mode_cntl automatically */
#define PDV_PIRANHA_4CH_HWINTLV  72        /* piranha 4 channel deinterleave IN HARDWARE */
#define PDV_INVERT_RIGHT_BGGR_INTLV 73        /* even pixels are right half, inverted BAYER pattern*/

#define PDV_LINE_INTLV           74
#define PDV_GENERIC_INTLV        75
#define PDV_LINE_INTLV_P3_8X4    76
#define TOSHIBA_SERIAL           77
#define COHU_SERIAL              78

#define PDV_WORD_INTLV_TOPBOTTOM 80
#define PDV_BYTE_INTLV_TOPBOTTOM 81
#define PDV_WORD_INTLV_INOUT     82
#define PDV_BYTE_INTLV_INOUT     83

#define PDV_DALSA_LS_4CH_INTLV       84        /* Dalsa four channel interleave (linescan) */
#define PDV_WORD_INTLV_HILO_LINE     85     /*PCO egde mode A*/
#define PDV_WORD_INTLV_TOPMID_LINE   86   /*PCO edge mode D */
#define PDV_BYTE_INTLV_MIDTOP_LINE   87   /*hamamatsu OrcaFlash4.0 */
#define PDV_WORD_INTLV_MIDTOP_LINE   88   /*hamanatsu OrcaFlash4.0 */
#define PDV_INTLV_10BIT_8TAP_PACKED  89
#define PDV_INTLV_10BIT_8TAP_TO_8BIT 90
#define PDV_INTLV_20BAND             91   /* custom 21 ... */ 
#define PDV_INTLV_21BAND             92   /* and 21-band   */

#define PDV_QUADRANT2_INTLV          93   /* 4 port deintlv, TopLeft, TopMiddle, MiddleLeft, MIddleMiddle */
#define PDV_QUADRANT3_INTLV          94   /* 4 port deintlv, like QUADRANT_INTLV but starting in the middle, iterating out */

#define KODAK_RDM_SINGLE          1
#define KODAK_RDM_DUAL            2

#define PDV_FRAMESYNC_OFF             0
#define PDV_FRAMESYNC_ON              1
#define PDV_FRAMESYNC_EMULATE_TIMEOUT 2

/* serial_format */
#define SERIAL_ASCII              0
#define SERIAL_BINARY             1
#define SERIAL_PULNIX_1010        2
#define SERIAL_ASCII_HEX          3
#define SERIAL_BASLER_FRAMING     4
#define SERIAL_DUNCAN_FRAMING     5
#define SERIAL_ASCII_NOSPACE      6
#define SERIAL_TOSHIBA            7

/* SMD registers common to all models (we hope!) */
#define SMD_READ_CAMTYPE          0xc3
#define SMD_READ_FWREV            0xc5
#define SMD_RESET                 0x80

/* camera type, returned from SMD_READ_CAMTYPE register */
#define SMD_TYPE_4M4              0x14
#define SMD_TYPE_BT25             0x38
#define SMD_TYPE_1M30P            0x41
#define SMD_TYPE_1M15P            0x11
#define SMD_TYPE_6M3P             0x33

/* from SMD 4M4 Camera Interface Specification */
#define SMD_4M4_WRITE_R1          0x81
#define SMD_4M4_WRITE_R2          0x82
#define SMD_4M4_WRITE_R3          0x84
#define SMD_4M4_WRITE_R4          0x88
#define SMD_4M4_READ_R1           0xc1
#define SMD_4M4_READ_R2           0xc2
#define SMD_4M4_READ_R3           0xc4
#define SMD_4M4_READ_R4           0xc8
#define SMD_4M4_READ_FRAMERATE    0xca

/* 4M4 (and 1M30P) reg bits (only partial, see manual for complete list) */
#define SMD_4M4_R1_INTEGRATE      0x00        /* integrate mode, 0=ext, 1=int */
#define SMD_4M4_R1_TRIG           0x02        /* trigger mode, 0=ext, 1=cont */
#define SMD_4M4_R1_BINMSK         0x0c        /* bin mode, 00=1x1, 01=2x2, 10=4x4 */
#define SMD_4M4_R1_GAIN           0x10        /* gain -- 0 = 1x, 1 = 4x */
#define SMD_4M4_BIN_2X2           0x04
#define SMD_4M4_BIN_4X4           0x08

/* SMD 1M30P is pretty much the same as 4M4 */
#define SMD_1M30P_REG_W_ADC_RESET 0x00
#define SMD_1M30P_REG_W_LS_OFFSET 0x02
#define SMD_1M30P_REG_W_MS_OFFSET 0x03
#define SMD_1M30P_REG_W_LS_GAIN   0x05
#define SMD_1M30P_REG_W_MS_GAIN   0x06

#define SMD_1M30P_REG_R_LS_OFFSET 0x42
#define SMD_1M30P_REG_R_MS_OFFSET 0x43
#define SMD_1M30P_REG_R_LS_GAIN   0x45
#define SMD_1M30P_REG_R_MS_GAIN   0x46

#define SMD_1M30P_REG_W_CAM_RESET 0x80
#define SMD_1M30P_REG_W_CONTROL   0x82
#define SMD_1M30P_REG_W_BINNING   0x85
#define SMD_1M30P_REG_W_INTEG0    0x8A
#define SMD_1M30P_REG_W_INTEG1    0x8B
#define SMD_1M30P_REG_W_INTEG2    0x8C
#define SMD_1M30P_REG_W_FRAME0    0x8D
#define SMD_1M30P_REG_W_FRAME1    0x8E
#define SMD_1M30P_REG_W_FRAME2    0x8F

#define SMD_1M30P_REG_R_CONTROL   0xc2


/* from SMD BT25 Camera Interface Specification */
#define SMD_BT25_WRITE_R1         0x81
#define SMD_BT25_WRITE_R2         0x82
#define SMD_BT25_WRITE_R3         0x84
#define SMD_BT25_WRITE_R4         0x88
#define SMD_BT25_READ_R1          0xc1
#define SMD_BT25_READ_R2          0xc2
#define SMD_BT25_READ_R3          0xc4
#define SMD_BT25_READ_R4          0xc8
#define SMD_BT25_READ_FRAMERATE   0xca

/* from DALSTAR 1M15P Camera Interface Specification */
#define SMD_1M15P_WRITE_R1        0x82
#define SMD_1M15P_WRITE_R2        0x84
#define SMD_1M15P_WRITE_R3        0x88
#define SMD_1M15P_READ_R1         0xc2
#define SMD_1M15P_READ_R2         0xc4
#define SMD_1M15P_READ_R3         0xc8

/* 1M15P regigster bits */
#define SMD_1M15P_R1_INTMODE      0x80        /* integrate mode, 0=ext, 1=int */
#define SMD_1M15P_R1_GAIN         0x40        /* gain, 0 = 1x, 1 = 4x */
#define SMD_1M15P_R1_ANTIBLOOM    0x20        /* anti-bloom, 0=enabled 1=disabled */
#define SMD_1M15P_R1_BINMODE      0x10        /* bin mode, 00=1x1, 01=2x2, 10=4x4 */
#define SMD_1M15P_R1_TRIGMODE     0x08        /* trigger mode, 0=ext, 1=cont */
#define SMD_1M15P_R1_INTMSK       0x03        /* integration time bits */

/* compat */
#define HAM_4880                   HAM_4880_SER
#define HAMAMATSU_4880             HAM_4880_SER
#define HAMAMATSU_4880_SER         HAM_4880_SER
#define HAMAMATSU_4880_8X          HAM_4880_8X

#define PDV_NOLOCKDEV              0
#define PDV_LOCKDEV                1

#define PDV_SERIAL_RS422           0
#define PDV_SERIAL_RS232           1

/* Front panel value */
#define FRONT_PANEL                0x7fffffff
#define FACTORY_SETTING            0x7ffffff0

/* LH Strobe Methods */
#define PDV_LHS_METHOD1            1
#define PDV_LHS_METHOD2            2

#define NOT_SET                    -9999999        /* doubtful it would ever be a valid value */


#endif				/* _DEPENDENT_H_ */
