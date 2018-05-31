/* #pragma ident "@(#)libedt.h 1.302 04/03/08 EDT" */

/* Copyright (c) 1995 by Engineering Design Team, Inc. */
/* Copyright (c) 2011 by Engineering Design Team, Inc. */

#ifndef INCLUDE_edtlib_h
#define INCLUDE_edtlib_h

/**
 * @file
 * File containing most libedt declarations.
 */

/**
 * @defgroup dma EDT DMA Library
 * The DMA library provides a set of consistent routines across many of
 * the EDT products, with simple yet powerful ring-buffered DMA (Direct Memory
 * Access) capabilities.
 *
 * A DMA transfer can be continuous or noncontinuous:
 *
 * - For noncontinuous transfers, the driver uses DMA system calls
 * read() and write(). Each read() or write() system call
 * performs one DMA transfer.  These calls allocate kernel resources,
 * during which time DMA transfers are interrupted.
 *
 * - To perform continuous transfers, use the ring buffers -- a set of
 * buffers that applications can access continuously, reading and
 * writing as required.  When the last buffer in the set has been
 * accessed, the application then cycles back to the first buffer. See
 * #edt_configure_ring_buffers for a complete description of the
 * configurable ring buffer parameters. See the sample programs
 * simple_getdata.c and simple_putdata.c (in the installation
 * directory) for examples of using the ring buffers.
 *
 * @note When developing applications for EDT digital image capture boards such as
 * the PCIe8 DV C-Link, programmers should avoid direct access to the edt library (edt_ subroutines)
 * and instead use the higher level \ref dv. Some limited use of edtlib calls may be necessary in DV
 * applications, however we can not provide support for applications that directly call edtlib subroutines
 * for data acquisition (e.g. edt_configure_ring_buffers, edt_start_buffers); instead use
 * the pdvlib corollaries (e.g. pdv_multibuf, pdv_start_images).
 *
 * @note For portability, use the library calls #edt_reg_read,
 * #edt_reg_write, #edt_reg_or, or #edt_reg_and to read or write the
 * hardware registers, rather than using ioctls.
 *
 * <h3>Building and using the Library, Utilities and Example Applications</h3>
 * By default, EDT's pcd installation package is copied into  c:\EDT\pcd (Windows),
 * or /opt/EDTpdv (Linux / Macos). For pdv packages, see the \ref dv.
 *
 * @note Applications using EDT boards must be linked with the appropriate (32 or 64-bit) for the
 * platform in use. Applications linked with 32-bit EDT libraries will not run correctly on 64-bit
 * systems, or vice-versa.
 *
 * To rebuild a program or library, you'll need to use a compiler and
 * either the <code>nmake</code> application that comes with Visual Studio, or the
 * Unix <code>make</code> utility, as described below.
 *
 * 1. Do one of the following:
 *   - For Linux or MacOS, navigate to the installation directory in a terminal window.
 *   - For Windows, click on the \b PCD Utilities or \b PDV Utilities desktop icon to bring up a command window in the installation directory. If Visual Studio environment variables aren't set, you will need to do something like the following. This example assumes Visual Studio 8; consult Microsoft's documentation for other versions:
 @code
 c:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat amd64
 @endcode
 to build for 64-bit, or if you are building for 32-bit, 
 @code
 c:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat x86
 @endcode
 * (Hint: you may find it convenient to configure a Windows Command prompt to open and run the above automatically, e.g. by modifying the Properties >> Target: to be <em>%comspec% /k ""c:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat"" amd64</em>.)
 *
 * 2. Enter
 * @code
 * make file
 * @endcode
 * where <em>file</em> is the name of the example program you wish to build.
 * 3. To rebuild all the libraries, examplees, utilities and diagnostics, run
 * @code
 * make
 * @endcode
 * Alternately, on Windows you can use a Visual Studio. Releases are all built using makefiles;
 * reference <code>includes.mk</code> and <code>makefile.def</code> for lists of the library objects (which all have .c source files), applications, and header files.
 *
 * <H3> Elements of EDT Interface Applications </H3>
 * Applications that perform continuous transfers typically include the
 * following elements:
 * -# The preprocessor statement:
 * @code
 * #include "edtinc.h"
 * @endcode
 * -# A call to #edt_open to open the device. This returns a pointer to
 * a structure that represents the EDT board in software. All subsequent
 * calls will use this pointer to access the board.
 * -# Optionally, setup for writing a file or some other target for the
 * data to be acquired.
 * -# A call to #edt_configure_ring_buffers to configure the ring
 * buffers.
 * -# A call to start the DMA, such as #edt_start_buffers.
 * -# Data processing calls, as required.
 * -# A call to #edt_close to close the device.
 * -# Appropriate settings in your makefile or C workspace to compile
 * and link the library file libedt.c.
 *
 * \Example
 * @code
 * #include "edtinc.h"
 * main()
 * {
 *     EdtDev *edt_p = edt_open("pcd", 0) ;
 *     char *buf_ptr; int outfd = open("outfile", 1) ;
 *     // Configure a ring buffer with four 1MB buffers
 *     edt_configure_ring_buffers(edt_p, 1024*1024, 4, EDT_READ, NULL) ;
 *     edt_start_buffers(edt_p, 0) ; // 0 starts unlimited buffer DMA
 *
 *     // This loop will capture data indefinitely, but the write() (or
 *     // other data processing) must be able to keep up.
 *     while ((buf_ptr = edt_wait_for_buffers(edt_p, 1)) != NULL)
 *          write(outfd, buf_ptr, 1024*1024) ;
 *
 *     edt_close(edt_p) ;
 * }
 * @endcode
 *
 * Applications that perform noncontinuous transfers typically include
 * the following elements:
 * -# The preprocessor statement:
 * @code
 * #include "edtinc.h"
 * @endcode
 * -# A call to #edt_open to open the device. This returns a pointer to
 * a structure that represents the EDT board in software. All subsequent
 * calls will use this pointer to access the board.
 * -# Optionally, setup for writing a file or some other target for the
 * data to be acquired.
 * -# A system read() or write() call to cause one DMA transfer.
 * -# Data processing calls, as required.
 * -# A call to #edt_close to close the device.
 * -# Appropriate settings in your makefile or C workspace to compile
 * and link the library file libedt.c.
 *
 * Assuming that a multichannel FPGA configuration file has been loaded,
 * this example opens a specific DMA channel with #edt_open_channel:
 * @code
 * #include "edtinc.h"
 * main()
 * {
 *     EdtDev *edt_p = edt_open_channel("pcd", 1, 2) ;
 *     char buf[1024] ;
 *     int numbytes, outfd = open("outfile", 1) ;
 *
 *     // Because read()s are noncontinuous, without hardware
 *     // handshaking, the data will have gaps between each read().
 *     while ((numbytes = edt_read(edt_p, buf, 1024)) > 0)
 *         write(outfd, buf, numbytes) ;
 *
 *     edt_close(edt_p) ;
 * }
 * @endcode
 *
 * You can use ring buffer mode for real-time data capture using a small
 * number of buffers (typically 1 MB) configured in a round-robin data
 * FIFO. During capture, the application must be able to transfer or
 * process the data before data acquisition wraps around and overwrites
 * the buffer currently being processed.  The example below shows
 * real-time data capture using ring buffers, although it includes no
 * error-checking. In this example, <tt> process_data(bufptr) </tt> must
 * execute in the same amount of time it takes DMA to fill a single
 * buffer, or faster.
 * @code
 * #include "edtinc.h"
 * main()
 * {
 *   EdtDev *edt_p = edt_open("pcd", 0) ;
 *
 *   // Configure four 1 MB buffers:
 *   // one for DMA
 *   // one for the second DMA register on most EDT boards
 *   // one for "process_data(bufptr)" to work on
 *   // one to keep DMA away from "process_data()"
 *   //
 *   edt_configure_ring_buffers(edt_p, 0x100000, 4, EDT_READ, NULL) ;
 *   edt_start_buffers(edt_p, 0) ; // 0 starts unlimited buffer DMA
 *   for (;;)
 *   {
 *       char *bufptr ;
 *       // Wait for each buffer to complete, then process it.
 *       // The driver continues DMA concurrently with processing.
 *       //
 *       bufptr = edt_wait_for_buffers(edt_p, 1) ;
 *       process_data(bufptr) ;
 *   }
 * }
 * @endcode
 *
 * Check compiler options in the EDT-provided makefiles.
 *
 * <H3>Multithreaded Programming</H3>
 *
 * The EDT driver is thread-safe, with the following constraints:
 *
 * -# Because kernel DMA resources are allocated on a per-thread basis
 * and must be allocated and released in the same thread, perform all DMA
 * operations in the same thread as #edt_open and #edt_close with respect
 * to each channel. Other threads can open the same channel concurrently
 * with DMA, but must perform no DMA-related operations.
 * -# To avoid undefined application or system behavior, or even
 * system crashes, when exiting the program:
 *  - Join all threads spawned by a main program with the main program
 * after they exit and before the main program exits;
 * or:
 *  - If the main program does not wait for the child threads to exit,
 * then any program that is run following the main program must wait for
 * all the child threads to exit. This waiting period depends on system
 * load and availability of certain system resources, such as a hardware
 * memory management unit.
 */

/**
 * @defgroup dma_updown Startup / Shutdown
 * @ingroup dma
 * These functions are used to open and close the EDT device.
 */

/**
 * @defgroup dma_init Initialization
 * @ingroup dma
 */

/**
 * @defgroup dma_fifo FIFO Flushing
 * @ingroup dma
 * First-in, first-out (FIFO) memory buffers are used to smooth data
 * transmission between different types of data sinks internal to EDT
 * boards. For instance, the FIFO stores information processed by the
 * user interface Xilinx until the PCI Xilinx retrieves it across the
 * PCI bus. The PCI bus normally sends information in bursts, so the
 * FIFO allows this same information to be sent smoothly.
 * When acquiring or sending data, flush the FIFO immediately before
 * performing DMA. This also resets the FIFO to an empty state. The
 * following subroutines either flush the FIFO or set it to flush
 * automatically at the start of DMA.
 */

/**
 * @defgroup dma_inout Input/Output
 * These functions are used to perform and control DMA transfers.
 * @ingroup dma
 */

/**
 * @defgroup dma_registers Register Access
 * @ingroup dma
 * Register access functions
 */

/**
 * @defgroup dma_utility Utility
 * @ingroup dma
 * Utility functions
 */

#ifdef DOXYGEN_SHOW_UNDOC
/**
 * @internal
 * @defgroup edt_undoc Undocumented / Uncategorized
 */

/**
 * @defgroup edt_internal Internal
 */

/**
 * @defgroup dma_updown_internal Startup / Shutdown (Internal)
 * @ingroup dma_updown
 */

/**
 * @defgroup dma_fifo_internal FIFO Flushing (Internal)
 * @ingroup dma_fifo
 */

/**
 * @defgroup dma_inout_internal Input/Output (Internal)
 * @ingroup dma_inout
 */

/**
 * @defgroup dma_registers_internal Register Access (Internal)
 * @ingroup dma_registers
 */

/**
 * @defgroup dma_utility_internal Internal Utility Functions
 * @ingroup dma_utility
 */

/**
 * @weakgroup dma
 * @{
 */
#endif


#ifdef DOXYGEN_SHOW_UNDOC
/**
 * @addtogroup dma_updown_internal
 * @{
 */
#endif
#ifdef DOXYGEN_SHOW_UNDOC
/** @} */ /* end dma_updown_internal group */
#endif



#ifdef DOXYGEN_SHOW_UNDOC
/**
 * @addtogroup dma_fifo_internal
 * @{
 */
#endif
#ifdef DOXYGEN_SHOW_UNDOC
/** @} */ /* end dma_fifo_internal group */
#endif



#ifdef DOXYGEN_SHOW_UNDOC
/**
 * @addtogroup dma_inout_internal
 * @{
 */
#endif
#ifdef DOXYGEN_SHOW_UNDOC
/** @} */ /* end dma_inout_internal group */
#endif



#ifdef DOXYGEN_SHOW_UNDOC
/**
 * @addtogroup dma_registers_internal
 * @{
 */
#endif
#ifdef DOXYGEN_SHOW_UNDOC
/** @} */ /* end dma_registers_internal group */
#endif


#ifdef DOXYGEN_SHOW_UNDOC
/**
 * @addtogroup dma_utility_internal
 * @{
 */
#endif
#ifdef DOXYGEN_SHOW_UNDOC
/** @} */ /* end dma_utility_internal group */
#endif



#ifdef DOXYGEN_SHOW_UNDOC
/**
 * @addtogroup dma_internal
 * @{
 */
#endif
#ifdef DOXYGEN_SHOW_UNDOC
/** @} */ /* end dma_internal */
#endif

#define EDTAPI_VERSION 0x05050504

#define EDT_NORMAL_DMA 0
#define EDT_DIRECT_DMA 1

#define event_t HANDLE
/*****************************************************
 * Kernel events                                     *
 *****************************************************/

typedef char edt_version_string[128];

/* undefined this to turn off event handling  */

#define USE_EVENT_HANDLERS


/* Event flags */
/* These act as flag bits indicating our interest in these events */

#define EDT_MAX_KERNEL_EVENTS        20
#define EDT_BASE_EVENTS              1

#define EDT_EODMA_EVENT              (EDT_BASE_EVENTS + 0)
#define EDT_EODMA_EVENT_NAME         "edt_eodma"
#define EV_EODMA                     EDT_EODMA_EVENT /* compat */

#define EDT_EVENT_BUF                (EDT_BASE_EVENTS + 1)
#define EDT_BUF_EVENT_NAME           "edt_buf"

#define EDT_EVENT_STAT               (EDT_BASE_EVENTS + 2)
#define EDT_STAT_EVENT_NAME          "edt_stat"

#define EDT_EVENT_P16D_DINT          (EDT_BASE_EVENTS + 3)
#define EDT_P16D_DINT_EVENT_NAME     "edt_p16dint"

#define EDT_EVENT_P11W_ATTN          (EDT_BASE_EVENTS + 4)
#define EDT_P11W_ATTN_EVENT_NAME     "edt_p11wattn"

#define EDT_EVENT_P11W_CNT           (EDT_BASE_EVENTS + 5)
#define EDT_P11W_CNT_EVENT_NAME      "edt_cnt"

#define EDT_PDV_EVENT_ACQUIRE        (EDT_BASE_EVENTS + 6)
#define EDT_EVENT_ACQUIRE            EDT_PDV_EVENT_ACQUIRE /* compat */
#define EDT_PDV_ACQUIRE_EVENT_NAME   "edt_acquire"

#define EDT_EVENT_PCD_STAT1          (EDT_BASE_EVENTS + 7)
#define EDT_EVENT_PCD_STAT1_NAME     "edt_pcd_stat1"

#define EDT_EVENT_PCD_STAT2          (EDT_BASE_EVENTS + 8)
#define EDT_EVENT_PCD_STAT2_NAME     "edt_pcd_stat2"

#define EDT_EVENT_PCD_STAT3          (EDT_BASE_EVENTS + 9)
#define EDT_EVENT_PCD_STAT3_NAME     "edt_pcd_stat3"

#define EDT_EVENT_PCD_STAT4          (EDT_BASE_EVENTS + 10)
#define EDT_EVENT_PCD_STAT4_NAME     "edt_pcd_stat4"

#define EDT_PDV_STROBE_EVENT         (EDT_BASE_EVENTS + 11)
#define EDT_PDV_STROBE_EVENT_NAME    "edt_pdv_strobe"
 
#define EDT_EVENT_P53B_SRQ           (EDT_BASE_EVENTS + 12)
#define EDT_EVENT_P53B_SRQ_NAME      "edt_p53b_srq"

#define EDT_EVENT_P53B_INTERVAL      (EDT_BASE_EVENTS + 13)
#define EDT_EVENT_P53B_INTERVAL_NAME "edt_p53b_interval"

#define EDT_EVENT_P53B_MODECODE      (EDT_BASE_EVENTS + 14)
#define EDT_EVENT_P53B_MODECODE_NAME "edt_p53b_modecode"

#define EDT_EVENT_P53B_DONE          (EDT_BASE_EVENTS + 15)
#define EDT_EVENT_P53B_DONE_NAME     "edt_p53b_done"

#define EDT_PDV_EVENT_FVAL           (EDT_BASE_EVENTS + 16)
#define EDT_PDV_EVENT_FVAL_NAME      "edt_pdv_fval"

#define EDT_PDV_EVENT_TRIGINT        (EDT_BASE_EVENTS + 17)
#define EDT_PDV_EVENT_TRIGINT_NAME   "edt_pdv_trigint"

#define EDT_EVENT_TEMP               (EDT_BASE_EVENTS + 18)
#define EDT_EVENT_TEMP_NAME          "edt_temp_intr"

#define EDT_MAX_EVENT_TYPES          (EDT_EVENT_TEMP + 1)

/*
 * The EVENT_MODE controls the way the driver responds to events.
 * Mode 0 is similar to ACT_ONCE; mode 1 is similar to ACT_ALWAYS,
 * and mode 2 is similar to ACT_ALWAYS but disables the event
 * while user code is processing it to prevent OS lockups.
 */
#define EDT_EVENT_MODE_MASK     0xFF000000
#define EDT_EVENT_MODE_SHFT     24
#define EDT_EVENT_MODE_ONCE     0
#define EDT_EVENT_MODE_CONTINUOUS   1
#define EDT_EVENT_MODE_SERIALIZE    2

#if defined(WIN32) || defined(_NT_DRIVER_)
                typedef unsigned __int64 uint64_t ;
                typedef __int64 int64_t ;
                typedef unsigned int  uint32_t ;
                typedef int int32_t ;
#endif

/*
 * prom stuff, mostly for pciload
 */
#define PCI_ID_SIZE      128
#define OSN_SIZE         32
#define ESN_SIZE         64
#define PCIE_INFO_SIZE   1024
#define PCIE_PID_SIZE    128
#define MACADDR_SIZE     12
#define MAX_MACADDRS     16
#define MACLIST_SIZE     216 /* should be ((MACADDR_SIZE+1) * MAX_MACADDRS) + 4 for tag + 4 for the count + comma, even number */
#define EXTRAADDR_SIZE   4
#define EXTRATAG_SIZE   4
#define OPTSN_SIZE       32
#define PROM_EXTRA_SIZE  512
#define E_SECTOR_SIZE    0x04000
#define AMD_SECTOR_SIZE  0x10000
#define AMD_SECTOR_SIZE2 0x20000
#define SPI_SECTOR_SIZE  0x100000
#define XLA_SECTOR_SIZE  AMD_SECTOR_SIZE   /* changed the name, backwards compat */
#define MIC_N25Q064A13ESE40G_PROMINFO_ADDR 0x7f0000 
#define EDTMACOUI        0x00251C

/*
 * try to standardize on size of various strings
 */
#define EDT_STRBUF_SIZE  128
#define EDT_PATHBUF_SIZE  256
#define EDT_BIGBUF_SIZE  512


/*
 * legal device proms for pciload and other prom detect code
 */
#define PROM_UNKN             0
#define AMD_4013E             1
#define AMD_4013XLA           2
#define AMD_4028XLA           3
#define AMD_XC2S150           4
#define AMD_XC2S200_4M        5
#define AMD_XC2S200_8M        6
#define AMD_XC2S100_8M        7
#define AMD_XC2S300E          8
#define SPI_XC3S1200E         9
#define AMD_XC5VLX30T        10 /* WAS SPI_XC5VLX30T */
#define AMD_XC5VLX50T        11 /* WAS SPI_XC5VLX50T */
#define AMD_EP2SGX30D        12
#define AMD_XC5VLX70T        13
#define AMD_XC5VLX30T_A      14
#define AMD_XC6SLX45         15
#define AMD_EP2SGX30D_A      16
#define AMD_EP2AGX45D        17
#define AMD_5SGXMA3K2F40C3   18
#define AMD_5SGXMA5K2F40C3   19
#define AMD_5SGXMA7K2F40C3   20
#define AMD_5SGXMA8K2H40C3N  21
#define AMD_5SGXMA7K1F40C2   22
#define AMD_5SGXEA2K1F40C2ES 23
#define AMD_5SGXMA9K2H40C2   24
#define MIC_N25Q064A13ESE40G 25
#define AMD_5AGZME1          26
#define MIC_N25Q256A13EF840  27

/* device (chip) programming types for pciload etc. */
#define FTYPE_X     0
#define FTYPE_BT    1
#define FTYPE_LTX   2
#define FTYPE_SPI   3
#define FTYPE_BT2   4
#define FTYPE_F16   5
#define FTYPE_MIC   6
#define FTYPE_F16A  7

/* magic numbers for devices (chips) */
typedef enum {
    UnknownMagic,
    XilinxMagic = 1,
    AlteraMagic = 2,
    Altera2Magic = 3
} FPGAMagic;

/* FPGA bitfile size constants */
#define MIN_BIT_SIZE_ANY    0x4000
#define MIN_BIT_SIZE_X      0x8000
#define MIN_BIT_SIZE_XLA    0x14000
#define MIN_BIT_SIZE_BT     0x20000
#define MIN_BIT_SIZE_SPI    0x60000
#define MIN_BIT_SIZE_AMD512 0x300000

#define EDT_ROM_JUMPER 0x01
#define EDT_5_VOLT     0x02

/*
 * status bits
 * bits 2-7 are Xilinx ID for 4028 and above.
 * shift down 2 and we get a number for the boot controller
 */
#define STAT_PROTECTED          0x01
#define STAT_5V                 0x02
#define STAT_IDMASK             0x7c
#define STAT_IDSHFT             2
#define IS_DEFAULT_SECTOR -6167

/*
 * command bits for the 4028xla
 * boot controller
 */
#define BT_READ     0x0100
#define BT_WRITE    0x0200
#define BT_INC_ADD  0x0400
#define BT_A0       0x0800
#define BT_A1       0x1000
#define BT_RSVD     0x2000
#define BT_REINIT   0x4000
#define BT_EN_READ  0x8000
#define BT_LD_LO    BT_WRITE
#define BT_LD_MID   BT_WRITE | BT_A0
#define BT_LD_HI    BT_WRITE | BT_A1
#define BT_LD_ROM   BT_WRITE | BT_A0 | BT_A1
#define BT_RD_ROM   BT_READ | BT_A0 | BT_A1 | BT_EN_READ
#define BT_RD_FSTAT BT_READ | BT_EN_READ
#define BT_RD_PALVER BT_READ | BT_A0 | BT_EN_READ
#define BT_MASK      0xff00


/*
 * structure to set up phase locked loop parameters.
 * some of these values go into the AV9110-02 PLL chip
 * and others set up prescalars in the xilinx.
 */
#ifdef USE_LONG_BUFCNT
#if defined(_NT_) || defined(_NT_DRIVER_)
                typedef unsigned long long bufcnt_t ;
#else
                typedef uint64_t bufcnt_t ;
#endif
#else
                typedef u_int bufcnt_t ;
#endif

                typedef struct
{
    int m; /* AV9110 refernce frequency divide range 3-127 */
    int n; /* AV9110 VCO feedback frequency divide range 3-127 */
    int v; /* AV9110 VCO feedback frequency prescalar range 1 or 8 */
    int r; /* AV9110 VCO output divider 1, 2, 4, 8 */
    int h; /* xilinx high speed divider (vco output) 1,3,5,7 */
    int l; /* xilinx divide by n 1-64 */
    int x; /* xilinx AV9110 prescale of 30MHz oscillator 1-256 */
} edt_pll ;

/* hard-coded database filenames for pciload and other load/id related files */
#define EDTMACS_FNAME "edtmactable.txt"
#define EDTPARTSFNAME "edt_parts.xpn"

/*
 * Typedef for edt_bitpath to send and retrieve bitfile pathnames
 * from the driver.
 */
typedef char edt_bitpath[128] ;

/*
 * SBus Device struct
 */
#define EDT_READ 0
#define EDT_WRITE 1

#ifndef TRUE

#define TRUE 1
#define FALSE 0

#endif

#define MAX_LOCK_SRC    60

/* for edt_get_dma_info */
typedef struct
{
    uint_t  used_dma ;  /* dma started within current open/close */
    uint_t  alloc_dma ; /* has allocated > 1 ring buffer */
    uint_t  active_dma ;    /* dma is currently active */
    uint_t  interrupts;
    uint_t  locks;
    uint64_t  wait_time;
    uint64_t  lock_time;
    uint_t  lock_array[MAX_LOCK_SRC + 1];
    uint_t direct_reads[256];
    uint_t direct_writes[256];
    uint_t indirect_reads[256];
    uint_t indirect_writes[256];
    uint_t dma_reads[8];
    uint_t dma_writes[8];
    uint_t active_list_size;
    uint_t free_list_size;
} edt_dma_info ;

/* defines for edt_set_kernel_buffers */
#define EDT_USER_BUFS 0
#define EDT_COPY_KBUFS 1
#define EDT_MMAP_KBUFS  2
#define EDT_PERSISTENT_KBUFS  4

#define MAX_EXTENDED_WORDS 32
typedef struct _EdtMezzDescriptor {
    int  id;
    int  n_extended_words;
    int  extended_rev;
    uint_t extended_data[MAX_EXTENDED_WORDS];
} EdtMezzDescriptor;

/*
 * embedded info
 */
typedef struct {
    int clock; /* 4 */
    char sn[11];
    char pn[11];
    char opt[15];
    int  rev; /* 4 */
    char ifx[11];
    char optsn[11];
    char maclist[MACLIST_SIZE];
} Edt_embinfo;

typedef struct {
    char  fpga[32];                 /* MUST match a pkg directory name */
    char  promdesc[32];             /* description may include size */
    u_short stat;                   /* status bits after shift */
    u_short statx;                  /* extended status bits for 16-bit devices */
    char  busdesc[8];               /* PCI, PCIe, ... */
    int   ftype;                    /* FTYPE_X, FTYPE_BT, FTYPE_LTX, FTYPE_SPI */
    int   magic;                    /* magic code (not the magic # itself but a code */
    u_int sectorsize;               /* sector size in bytes */
    u_int sectsperseg;              /* sectors per segment */
    u_int nsegments;                /* number of segments (banks) */
    int   defaultseg;               /* default unprotected segment */
    u_int (*id_addr)(void *, int);          /* id address function pointer */
    int   load_seg0;                /* 2d default unprotected segment (3v) */
    int   load_seg1;                /* 2d default unprotected segment (5v) */
#if 0 /* cool idea -- lets try to get this done, but filling in all the fields */
    /* will take some time and need lots of care, so commented out for now */
    struct sector_struct sector[16];        /* all sectors */
#endif
} Edt_prominfo ;

typedef struct {
    char type[4];
    u_int size;
} EdtPromParmBlock;

#define BAD_PARMS_BLOCK ((EdtPromParmBlock *) 0xffffffff)

#define EdtPromParmData(p) \
    ((u_char *) p + sizeof(EdtPromParmBlock))

typedef struct {
    char id[PCI_ID_SIZE];
    char esn[ESN_SIZE];
    char osn[OSN_SIZE];

    /* EXTRA area is variable size up to PROM_EXTRA_SIZE */
    /* size is int at beginning (top or bottom) of extra area */
    /* preceded by 4 char key "XTR:" */

    int  extra_size; /* total size of parm blocks */
    int  nblocks;    /* how many blocks */

    u_char extra_buf[PROM_EXTRA_SIZE];

    Edt_embinfo ei;
    char optsn[ESN_SIZE];
    char maclist[MACLIST_SIZE];
} EdtPromData;

typedef struct _prom_addr {
    u_int id_addr;
    u_int esn_addr;
    u_int osn_addr;
    u_int extra_data_addr;
    u_int extra_tag_addr;
    u_int extra_size_addr;
    u_int extra_size;
    u_int optsn_addr;
    u_int maclist_addr;
} EdtPromIdAddresses;

#ifndef _KERNEL
/* Event callback stuff */


typedef void (*EdtEventFunc)(void *);

typedef struct edt_event_handler {
    struct edt_event_handler *next; /* in case we want linked lists */
    EdtEventFunc    callback;       /* function to call when event occurs */
    struct edt_device *owner;       /* reflexive pointer */
    uint_t          event_type;
    void *          data;           /* closure pointer for callback */
    u_char          active;         /* flag for a graceful death */
    u_char          continuous;     /* flag for continuing events */
#ifdef _NT_
    thread_t        wait_thread;    /* thread handle */
    event_t         wait_event;     /* kernel event handle */
    event_t         closing_event;  /* abort event handle */
    uint_t          thrdid;         /* thread id # */
#elif defined(__sun)
    thread_t        thrdid ;        /* thread handle and ID in Solaris */
    sema_t          sema_thread ;   /* thread sync semaphore */
#elif defined(__linux__)


    thread_t thrdid;
#elif defined(__APPLE__)
    pthread_t thrdid ;

#endif /* _NT_ */

} EdtEventHandler;


/* An EdtBdFilterFunction ("Bd" = Board) function is used by
 * edt_detect_boards_filter() to determine if a board with the given
 * dev (e.g. "pcd"), unit number, and bd_id (eg.. PCD_ID) should be
 * included with the results of the board detection function.
 * This is used, for example, by hubload.c to do a detect of only "ltx"
 * boards which are those matching a small number of bd_ids.
 * The data parameter is passed to edt_detect_boards_filter, and
 * subsequently passed on to the filter function; it can be either NULL
 * or any data which the filter function wants to help it do its job.
 * Returns true for matches, false otherwise. */

typedef int (*EdtBdFilterFunction) (char *dev, int unit, int bd_id, void *data);

typedef struct {
    char type[8]; /* board family e.g., "pcd" */
    int id; /* board unit number */
    int bd_id; /* board id, e.g. PCD_ID */
    int promcode; /* e.g. AMD_XC5VLX30T */
} Edt_bdinfo ;


#define MAX_DMA_BUFFERS 2048

typedef struct {

    int size;

    int allocated_size;


#if defined(__sun)

    thread_t ring_tid;

#endif

    char write_flag;


    char owned;

} EdtRingBuffer;

typedef struct _dma_data_block {
    u_int buffernum;
    u_char *pointer;
    u_int  offset;
    u_int  length;
} EdtDMADataBlock;

#define EDT_SS_TYPE 1
#define EDT_GS_TYPE 2
#define EDT_CD_TYPE 3
#define EDT_LX_TYPE 4
#define EDT_LC_TYPE 5


#define DDMA_FIFOSIZE 2048
#define DDMA_BUFSIZE  4096

#define edt_swab32(x) \
    (\
     ((u_int)( \
         (((u_int)(x) & (u_int)0x000000ffUL) << 24) | \
         (((u_int)(x) & (u_int)0x0000ff00UL) <<  8) | \
         (((u_int)(x) & (u_int)0x00ff0000UL) >>  8) | \
         (((u_int)(x) & (u_int)0xff000000UL) >> 24) )) \
    )


typedef struct {
    int numbufs;
    int bufsize;
    int next_ringbuf;
    int initialized;
    uint64_t done_count;
    u_char **bufs;
    u_int *regmap;
    u_int sg_list[DDMA_FIFOSIZE*2];
} edt_directDMA_t;



typedef struct _optionstr_fields {
    int  board_type;
    int  DMA_channels;
    char mezzanine_type[68];
    char filename[68];
    int  version_number;
    int  rev_number;
    char date[12];
    int  custom_DMA_channels;
    int  available_DMA_channels;
} EdtOptionStringFields;

typedef struct _EdtBitfileDescriptor {
    edt_bitpath bitfile_name;
    edt_bitpath mezz_name0;
    edt_bitpath mezz_name1;
    char optionstr[68];
    char mezz_optionstr0[32];
    char mezz_optionstr1[32];

    EdtOptionStringFields ostr;

    int  revision_register;
    int  string_type;

} EdtBitfileDescriptor;

typedef struct edt_device {
#ifdef __APPLE__
    u_int       fd;
#else
    HANDLE      fd ;           /* file descriptor of opened device     */
#endif
    u_int       unit_no ;
    u_int       spi_reg_base ; /* Base address for SPI_ access registers (0 specifies default of 0x60) */
    uint_t      devid ;
    uint_t      devtype ;      /* PCD, PDV, ... */
    uint_t      todo;          /* n buffers started            */
    uint_t      b_count;       /* per open byte counter for edt_read/write */

    /* flags for each ring buffer */
    EdtRingBuffer   rb_control[MAX_DMA_BUFFERS];

    /* addresses kept in own array for backward compatibility */

    unsigned char * ring_buffers[MAX_DMA_BUFFERS];

    unsigned char * tmpbuf;          /* for interlace merging, etc. */

    uint_t      tmpbufsize;

    char        edt_devname[64] ;
    uint_t      cursample ;
    uint_t      minchunk ;
    bufcnt_t        donecount ;
    uint_t      nextwbuf ;     /* for edt_next_writebuf      */

    uint_t      ring_buffer_numbufs ;
    uint_t      ring_buffer_bufsize ;
    uint_t      ring_buffers_allocated ;
    uint_t      ring_buffers_configured ;
    uint_t      loops ;
    uint_t      ring_buffer_allocated_size ; /* if rounded up to page boundaries */

    uint_t      write_flag ;

    uint_t      port_no;

    uint_t      debug_level ;
    Dependent   *dd_p ;   /* device dependent struct        */
    void        *Pdma_p;  /* Pdma struct when Pdma used     */

    u_char *        data_end; /* end of base_buffer + 1 */

    /* For callback rountines */

    EdtEventHandler event_funcs[EDT_MAX_KERNEL_EVENTS];
    u_int       use_RT_for_event_func ;


    u_int channel_no;

    /* Use this for allocating all ring-buffers in a single chunk (w/possible headers before or
       after the ring-buffers). This is useful when trying to DMA into multiple ring-buffers which
       can be written/read as a block to a file*/

    unsigned char * base_buffer;

    u_int       header_size;
    int         header_offset;

    u_int       pending_samples; /* dma samples */

    int         hubidx;
    volatile caddr_t    mapaddr;

    void *      pInterleaver; /* for post-processing data, such as deinterleaving image */

    unsigned char *output_base;
    unsigned char **output_buffers; /* for results from post-processing */

    u_int is_serial_enabled;

    u_int buffer_granularity;
    u_int mmap_buffers;
    u_int totalsize;
    u_int       fullbufsize;

    /* low-latency dma routine fields
     * If we didn't need compatibility this could be a lot cleaner
     */

    EdtDMADataBlock *blocks;

    double      last_buffer_time;
    double      next_sample;
    u_int       period;

    u_char *    last_sample_end;

    u_char      wait_mode;
    u_char      freerun;

    u_char      DMA_channels;

    EdtBitfileDescriptor bfd;
    EdtMezzDescriptor mezz;


    char                last_direction;
    u_char      last_wait_ret;
    u_int               promcode;

    volatile u_int  *reg_fifo_io;
    volatile u_char *reg_fifo_cnt;
    volatile u_char *reg_fifo_ctl;
    volatile u_char *reg_intfc_off ;        /* mmap to intfc regs      */
    volatile u_char *reg_intfc_dat ;        /* mmap to intfc regs      */
    int              regBAR0_fd;
    int              regBAR1_fd;
    int              regUIFPGA_fd;

    edt_directDMA_t *directDMA_p;         /* Direct DMA management   */

    u_int       adt7461_reg ; /* register definition */

    u_int dmy_started;
    void    (*dmy_wait_for_buffers_callback) (struct edt_device *edt_p, u_char *buf);
    u_short (*dmy_reg_read_callback)         (struct edt_device *edt_p, u_int reg_desc);
    void    (*dmy_reg_write_callback)        (struct edt_device *edt_p, u_int reg_desc, u_int reg_value);

} EdtDev;



/* Function declarations for EdtDev */


/* Header file for functions exported by libedt */
EDTAPI int initpcd_str(char *cfg_str, int unit, int verbose) ;
EDTAPI u_int edt_mzdemod_read(EdtDev * edt_p, u_int block, u_int offset);
EDTAPI void edt_mzdemod_write(EdtDev * edt_p, u_int block, u_int offset, u_int data);

#define LCR_DDC_REG_SPACE (0x08 << 19)

EDTAPI u_int edt_lcr_read(EdtDev *edt_p, unsigned int regBlock, unsigned int regOffset);
EDTAPI void edt_lcr_write(EdtDev *edt_p, unsigned int regBlock, unsigned int regOffset, unsigned int regVal);

EDTAPI int edt_init_direct_dma(EdtDev *edt_p);
EDTAPI int edt_direct_read (EdtDev *edt_p, u_char *buf, int bytes);
EDTAPI int edt_direct_write(EdtDev *edt_p, u_char *buf, int bytes);

/**
 * @addtogroup prominfo
 */
EDTAPI int     edt_flash_is_protected(EdtDev *edt_p);
EDTAPI void    edt_get_sns(EdtDev *edt_p, char *esn, char *osn);
EDTAPI void    edt_get_osn(EdtDev *edt_p, char *osn);
EDTAPI void    edt_get_esn(EdtDev *edt_p, char *esn);
EDTAPI void    edt_print_dev_flashstatus(EdtDev *edt_p, u_short stat, int sector);
EDTAPI void    edt_print_flashstatus(u_short stat, int sector, int frdata);
EDTAPI int     edt_print_pcie_negotiated_link(EdtDev *edt_p);
EDTAPI void    edt_init_promdata(EdtPromData *pdata);
EDTAPI void    edt_init_parmblock(EdtPromParmBlock *block, char *type, int datasize);
EDTAPI EdtPromParmBlock *edt_add_parmblock(EdtPromData *pdata, char *type, int datasize);
EDTAPI EdtPromParmBlock *edt_get_parms_block(EdtPromData *pdata, char *id);

EDTAPI u_int   edt_flash_get_promaddrs(EdtDev *edt_p, int promcode, int segment, EdtPromIdAddresses *paddr);
EDTAPI int     edt_flash_get_fname(EdtDev *edt_p, char *name);
EDTAPI int     edt_flash_get_fname_auto(EdtDev *edt_p, char *name);
EDTAPI void    edt_flash_program_prominfo(EdtDev *edt_p, int promcode, int sector, EdtPromData *pdata);
EDTAPI void    edt_flash_byte_program(EdtDev *edt_p, u_int addr, u_char data, int isbt);
EDTAPI void    edt_flash_verify(EdtDev *edt_p, u_int addr, u_char *data, int nbytes, int ftype);
EDTAPI void    edt_flash_reset(EdtDev * edt_p, int isbt);
EDTAPI void    edt_flash_print16(EdtDev * edt_p, u_int addr, int ftype);
EDTAPI int     edt_flash_prom_detect(EdtDev *edt_p, u_short *stat);
EDTAPI void    edt_dump_flash(EdtDev *edt_p, u_int addr, u_int size);

EDTAPI Edt_prominfo *edt_get_prominfo(int promcode);
EDTAPI const char *edt_get_fpga_mfg(EdtDev * edt_p);
EDTAPI u_char  edt_flash_read8(EdtDev * edt_p, u_int addr, int ftype);
EDTAPI u_short edt_flash_read16(EdtDev * edt_p, u_int addr, int ftype);
EDTAPI char   *edt_flash_type_string(int ftype);

EDTAPI void    edt_read_prom_data(EdtDev *edt_p, int promcode, int segment, EdtPromData *pdata);
EDTAPI u_int   edt_get_id_addr(int promcode, int segment);
EDTAPI u_int   edt_get_id_addrs(EdtDev *edt_p, int promcode, int segment, u_int *osn_addr, u_int *esn_addr);
EDTAPI int     edt_program_flash(EdtDev *edt_p, const u_char *buf, int size, int do_sleep);

EDTAPI int     edt_program_flash_start(EdtDev *edt_p);
EDTAPI void    edt_program_flash_chunk(EdtDev *edt_p, const u_char *buf, int xfer, int do_sleep);
EDTAPI int     edt_program_flash_end(EdtDev *edt_p);
EDTAPI int     edt_get_flash_file_header(const char *fname, char *header, int *size);
EDTAPI char   *edt_get_flash_prom_header(EdtDev *edt_p, char *name);

/** @} */ /* end prominfo group */

/**
 * @addtogroup dma_updown
 * @{
 */
EDTAPI EdtDev *edt_open(const char *device_name, int unit) ;
EDTAPI EdtDev *edt_open_quiet(const char *device_name, int unit) ;
EDTAPI EdtDev *edt_open_channel(const char *device_name, int unit, int channel) ;
EDTAPI EdtDev *edt_open_device(const char *device_name, int unit, int channel, int verbose) ;
EDTAPI int     edt_close(EdtDev *edt_p) ;

EDTAPI void    edt_set_port(EdtDev *edt_p, int port);
EDTAPI int     edt_get_port(EdtDev *edt_p);

/** @} */ /* end updown group */

/** @addtogroup dma_inout
 * @{
 */
EDTAPI int     edt_read(EdtDev *edt_p, void *buf, uint_t size) ;
EDTAPI int     edt_write(EdtDev *edt_p, void *buf, uint_t size) ;

EDTAPI int     edt_configure_ring_buffers(EdtDev *edt_p, int bufsize,
                                            int numbufs, int write_flag,
                                            unsigned char **bufarray) ;

EDTAPI void    edt_set_dmy_wait_for_buffers_callback(EdtDev *edt_p,
    		    void (*callBack)(struct edt_device *edt_p, u_char *buf));

EDTAPI void    edt_set_dmy_reg_read_callback(EdtDev *edt_p, u_int (*callBack)(struct edt_device *edt_p, u_int reg_desc));
EDTAPI void    edt_set_dmy_reg_write_callback(EdtDev *edt_p, void (*callBack)(struct edt_device *edt_p, u_int reg_desc, u_int reg_value));


EDTAPI int     edt_configure_block_buffers_mem(EdtDev *edt_p, int bufsize,
                                                int numbufs, int write_flag,
                                                int header_size, int header_before,
                                                u_char *user_mem);
EDTAPI int     edt_configure_block_buffers(EdtDev *edt_p, int bufsize,
                                        int numbufs, int write_flag,
                                        int header_size, int header_before);

EDTAPI caddr_t edt_map_dmamem(EdtDev *edt_p);

EDTAPI int     edt_disable_ring_buffers(EdtDev *edt_p) ;

EDTAPI int     edt_get_numbufs(EdtDev *edt_p);

EDTAPI int     edt_reset_ring_buffers(EdtDev *edt_p, uint_t bufnum);
EDTAPI int     edt_abort_dma(EdtDev *edt_p);
EDTAPI int     edt_abort_current_dma(EdtDev *edt_p);
EDTAPI int     edt_stop_buffers(EdtDev *edt_p);
EDTAPI int     edt_start_buffers(EdtDev *edt_p, uint_t count);

// Internal EDT lab use only - set all ring buffer SgList entries to specified 32-bit physaddr
EDTAPI int     edt_set_buffer_physaddr(EdtDev * edt_p, uint_t index, uint64_t physaddr);

EDTAPI int     edt_set_buffer_size(EdtDev *edt_p,
                                uint_t which_buf,
                                uint_t size,
                                uint_t write_flag);

EDTAPI unsigned int  edt_allocated_size(EdtDev *edt_p, int bufnum);
EDTAPI int           edt_get_total_bufsize(EdtDev *edt_p,
                                            int bufsize,
                                            int header_size);

EDTAPI unsigned char *edt_wait_for_buffers(EdtDev *edt_p, int count) ;


EDTAPI int             edt_ref_tmstamp(EdtDev *edt_p, u_int val) ;
EDTAPI int             edt_get_timestamp(EdtDev *edt_p, u_int *timep, u_int bufnum) ;
EDTAPI int             edt_get_reftime(EdtDev *edt_p, u_int *timep) ;
EDTAPI unsigned char  *edt_wait_for_next_buffer(EdtDev *edt_p);
EDTAPI unsigned char  *edt_last_buffer_timed(EdtDev *edt_p, u_int *timep) ;
EDTAPI unsigned char  *edt_last_buffer(EdtDev *edt_p) ;
EDTAPI unsigned char  *edt_wait_buffers_timed(EdtDev *edt_p, int count, u_int *timep);
EDTAPI int             edt_set_buffer(EdtDev *edt_p, uint_t bufnum) ;
EDTAPI unsigned char*  edt_next_writebuf(EdtDev    *edt_p) ;
EDTAPI uint_t          edt_next_writebuf_index(EdtDev *edt_p);
EDTAPI unsigned char** edt_buffer_addresses(EdtDev *edt_p) ;
EDTAPI unsigned char  *edt_get_current_dma_buf(EdtDev * edt_p);

EDTAPI bufcnt_t        edt_done_count(EdtDev   *edt_p) ;
EDTAPI unsigned char  *edt_check_for_buffers(EdtDev *edt_p, uint_t count);

EDTAPI uint_t          edt_get_bytecount(EdtDev *edt_p) ;
EDTAPI uint_t          edt_get_timecount(EdtDev *edt_p) ;
EDTAPI void            edt_set_direction(EdtDev *edt_p, int direction) ;
EDTAPI uint_t edt_get_timeout_count(EdtDev *edt_p);
EDTAPI unsigned short  edt_get_direction(EdtDev *edt_p) ;

EDTAPI void            edt_startdma_reg(EdtDev *edt_p, uint_t  desc, uint_t val) ;
EDTAPI void            edt_enddma_reg(EdtDev *edt_p, uint_t    desc, uint_t val) ;
EDTAPI void            edt_startdma_action(EdtDev *edt_p, uint_t   val) ;
EDTAPI void            edt_enddma_action(EdtDev *edt_p, uint_t val) ;

EDTAPI void edt_read_start_action(EdtDev * edt_p, u_int enable, u_int reg_desc,
                                    u_char set, u_char clear, u_char setclear,
                                    u_char clearset, int delay1, int delay2);
EDTAPI void edt_read_end_action(EdtDev * edt_p, u_int enable, u_int reg_desc,
                                    u_char set, u_char clear, u_char setclear,
                                    u_char clearset, int delay1, int delay2);
EDTAPI void edt_write_start_action(EdtDev * edt_p, u_int enable, u_int reg_desc,
                                    u_char set, u_char clear, u_char setclear,
                                    u_char clearset, int delay1, int delay2);
EDTAPI void edt_write_end_action(EdtDev * edt_p, u_int enable, u_int reg_desc,
                                    u_char set, u_char clear, u_char setclear,
                                    u_char clearset, int delay1, int delay2);

EDTAPI int             edt_set_timeout_action(EdtDev *edt_p, u_int action);
EDTAPI int             edt_get_timeout_goodbits(EdtDev *edt_p);
EDTAPI int             edt_get_goodbits(EdtDev *edt_p);

EDTAPI int             edt_set_event_func(EdtDev *edt_p, int event_type, EdtEventFunc f, void *data, int continuous) ;
EDTAPI int             edt_remove_event_func(EdtDev *edt_p, int event_type) ;

EDTAPI uint_t          edt_get_todo(EdtDev *edt_p) ;
EDTAPI int             edt_ring_buffer_overrun(EdtDev *edt_p) ;


/** @} */ /* end dma_inout */

/* Internal or obsolete, etc. - we don't want to document these. */
EDTAPI int             edt_configure_channel_ring_buffers(EdtDev *edt_p,
        int bufsize, int numbufs, int write_flag,
        unsigned char **bufarray) ;
EDTAPI int             edt_disable_ring_buffer(EdtDev *edt_p,
        int nIndex);
EDTAPI int             edt_cancel_current_dma(EdtDev *edt_p) ;
EDTAPI int             edt_user_dma_wakeup(EdtDev *edt_p);
EDTAPI int             edt_had_user_dma_wakeup(EdtDev *edt_p);

/** @addtogroup dma_registers
 * @{
 */
EDTAPI uint_t        edt_reg_read(EdtDev *edt_p, uint_t desc) ;
EDTAPI void          edt_reg_write(EdtDev *edt_p, uint_t desc, uint_t val) ;
EDTAPI uint_t        edt_reg_or(EdtDev *edt_p, uint_t desc, uint_t val) ;
EDTAPI uint_t        edt_reg_and(EdtDev *edt_p, uint_t desc, uint_t val) ;
EDTAPI void          edt_reg_clearset(EdtDev *edt_p, uint_t desc, uint_t val) ;
EDTAPI void          edt_reg_setclear(EdtDev *edt_p, uint_t desc, uint_t val) ;
EDTAPI void          edt_intfc_write(EdtDev *edt_p, uint_t offset, uchar_t val) ;
EDTAPI uchar_t       edt_intfc_read(EdtDev *edt_p, uint_t    offset) ;
EDTAPI void          edt_intfc_write_short(EdtDev *edt_p, uint_t offset, u_short val) ;
EDTAPI u_short       edt_intfc_read_short(EdtDev *edt_p, uint_t offset) ;
EDTAPI void          edt_intfc_write_32(EdtDev   *edt_p, uint_t offset,  uint_t val) ;
EDTAPI uint_t        edt_intfc_read_32(EdtDev *edt_p, uint_t offset) ;
EDTAPI void          edt_bar1_write(EdtDev *edt_p, u_int offset, u_int val) ;
EDTAPI u_int         edt_bar1_read(EdtDev  *edt_p, u_int offset) ;

EDTAPI u_int         edt_ind_2_read(EdtDev  *edt_p, u_int offset, u_int *width) ;
EDTAPI u_int         edt_ind_2_write(EdtDev  *edt_p, u_int offset, u_int value, u_int *width) ;


/** @} */ /* end dma_control */
/** @addtogroup dma_fifo
 * @{
 */
EDTAPI void         edt_flush_fifo(EdtDev *edt_p) ;
EDTAPI int          edt_set_firstflush(EdtDev *edt_p, int val)  ;
EDTAPI int          edt_get_firstflush(EdtDev *edt_p) ;
EDTAPI void         edt_flush_channel(EdtDev * edt_p, int channel) ;
EDTAPI int          edt_enable_channels(EdtDev * edt_p, u_int mask) ;
EDTAPI int          edt_enable_channel(EdtDev * edt_p, u_int channel) ;
EDTAPI int          edt_disable_channels(EdtDev * edt_p, u_int mask) ;
EDTAPI int          edt_disable_channel(EdtDev * edt_p, u_int channel) ;

/** @} */ /* end dma_fifo */


EDTAPI int          edt_get_wait_status(EdtDev *edt_p);
EDTAPI int          edt_set_timeout_ok(EdtDev *edt_p, int val);
EDTAPI int          edt_get_timeout_ok(EdtDev *edt_p);


/* for a while it was called "eodma_sig" */
EDTAPI void         pcd_set_funct(EdtDev *edt_p, uchar_t val)   ;
EDTAPI int          edt_set_eodma_int(EdtDev *edt_p, int sig) ;
EDTAPI int          edt_set_autodir(EdtDev  *edt_p, int val) ;


EDTAPI int         edt_send_msg(EdtDev *edt_p, int unit, const char *msg, int size) ;
EDTAPI int         edt_get_msg(EdtDev *edt_p, char *msgbuf, int maxsize) ;
EDTAPI int         edt_get_msg_unit(EdtDev *edt_p, char *msgbuf, int maxsize, int unit) ;
EDTAPI int         edt_serial_wait(EdtDev *edt_p, int size, int timeout) ;

EDTAPI void        edt_send_dma(EdtDev *edt_p, int unit, uint_t    start_val) ;
EDTAPI int         edt_wait_avail(EdtDev *edt_p) ;
EDTAPI void        edt_init_mac8100(EdtDev *edt_p) ;
EDTAPI u_short     edt_read_mac8100(EdtDev *edt_p, uint_t add) ;
EDTAPI void        edt_write_mac8100(EdtDev *edt_p, uint_t add, u_short data) ;

EDTAPI int         edt_get_dependent(EdtDev *edt_p, void *addr) ;
EDTAPI int         edt_set_dependent(EdtDev *edt_p, void *addr) ;

EDTAPI int         edt_flush_resp(EdtDev *edt_p) ;
EDTAPI int         edt_get_tracebuf(EdtDev *edt_p, uint_t *addr)   ;
EDTAPI int         edt_set_flush(EdtDev *edt_p, int val) ;
/** @addtogroup dma_inout
 * @{
 */
EDTAPI int         edt_timeouts(EdtDev *edt_p) ;
/** @} */ /* end dma_inout */


EDTAPI void        edt_flush_mode(EdtDev *edt_p, uint_t    val) ;

EDTAPI int         edt_set_rci_dma(EdtDev *edt_p, int unit, int channel) ;
EDTAPI int         edt_get_rci_dma(EdtDev *edt_p, int unit) ;
EDTAPI int         edt_set_rci_chan(EdtDev *edt_p, int unit, int channel) ;
EDTAPI int         edt_get_rci_chan(EdtDev *edt_p, int unit) ;


EDTAPI void        edt_reset_counts(EdtDev *edt_p) ;
EDTAPI void        edt_reset_serial(EdtDev *edt_p) ;


EDTAPI int         edt_set_debug(EdtDev *edt_p, int count) ;
EDTAPI int         edt_get_debug(EdtDev *edt_p) ;


/**
 * @addtogroup dma_inout
 * @{
 */
EDTAPI int         edt_set_burst_enable(EdtDev *edt_p, int on) ;
EDTAPI int         edt_get_burst_enable(EdtDev *edt_p) ;

EDTAPI int         edt_set_rtimeout(EdtDev *edt_p, int value) ;
EDTAPI int         edt_set_wtimeout(EdtDev *edt_p, int value) ;

EDTAPI int         edt_get_rtimeout(EdtDev *edt_p) ;
EDTAPI int         edt_get_wtimeout(EdtDev *edt_p) ;

/** @} */ /* end dma_inout */

EDTAPI void        edt_set_out_clk(EdtDev *edt_p, edt_pll *clk_data) ;
EDTAPI u_char      edt_set_funct_bit(EdtDev    *edt_p, u_char mask) ;
EDTAPI u_char      edt_clr_funct_bit(EdtDev    *edt_p, u_char mask) ;
EDTAPI u_char      edt_set_pllct_bit(EdtDev * edt_p, u_char mask);
EDTAPI u_char      edt_clr_pllct_bit(EdtDev * edt_p, u_char mask);
EDTAPI int         edt_set_ignore_signals(EdtDev *edt_p, int ignore);

/** @addtogroup dma_utility
 * @{
 */
EDTAPI int         edt_device_id(EdtDev *edt_p);
EDTAPI char *      edt_idstr(int id) ;
EDTAPI u_char      edt_flipbits(u_char val);
EDTAPI char *      edt_idstring(int id, int promcode) ;
EDTAPI int         edt_access(char *fname, int perm) ;
EDTAPI int         edt_parse_unit(const char *str, char *dev, const char *default_dev) ;
EDTAPI int         edt_parse_unit_channel(const char *str, char *dev,
                                            const char *default_dev,
                                            int *channel) ;
EDTAPI int         edt_find_xpn(char *part_number, char *fpga);
EDTAPI int         edt_get_xref_info(const char *path, const char *pn, char *fpga, char *sn, char *mtype, char *moffs, char *mcount, char *desc, char *rsvd1, char *rsvd2);
EDTAPI uint_t      edt_overflow(EdtDev *edt_p) ;
EDTAPI void        edt_perror(char *str) ;
EDTAPI u_int       edt_errno(void) ;
EDTAPI const char *edt_home_dir(EdtDev *edt_p) ;
EDTAPI const char *edt_envvar_from_devtype(const int devtype) ;
EDTAPI const char *edt_envvar_from_devstr(const char *devstr) ;

EDTAPI char *      edt_timestring(u_int *timep) ;
EDTAPI int         edt_system(const char *cmdstr) ;
EDTAPI int         edt_fix_millennium(char *str, int rollover);

#if 1
EDTAPI char        *edt_fmt_pn(char *pn, char *pn_str);
EDTAPI int         edt_parse_esn(char *str, Edt_embinfo *ei);
EDTAPI int         edt_parse_devinfo(char *str, Edt_embinfo *ei);
EDTAPI void        edt_get_sns_sector(EdtDev *edt_p, char *esn, char *osn, int sector);
EDTAPI void        edt_get_osn(EdtDev *edt_p, char *osn);
EDTAPI void        edt_get_esn(EdtDev *edt_p, char *esn);
#endif



/* Subroutines to set driver last bitfile loaded; used in bitload */
EDTAPI int         edt_set_bitpath(EdtDev *edt_p, const char *bitpath) ;
EDTAPI int         edt_get_bitpath(EdtDev *edt_p, char *bitpath, int size) ;
EDTAPI int         edt_get_bitname(EdtDev *edt_p, char *bitpath, int size) ;
EDTAPI int         edt_set_mezz_chan_bitpath(EdtDev *edt_p, const char *bitpath, int channel) ;
EDTAPI int         edt_get_mezz_chan_bitpath(EdtDev *edt_p, char *bitpath, int size, int channel) ;
EDTAPI int         edt_set_mezz_bitpath(EdtDev *edt_p, const char *bitpath) ;
EDTAPI int         edt_get_mezz_bitpath(EdtDev *edt_p, char *bitpath, int size) ;

EDTAPI char *      edt_get_last_bitpath(EdtDev *edt_p);

EDTAPI u_int       edt_get_full_board_id(EdtDev *edt_p,
                                        int *extended_n,
                                        int *rev_id,
                                        u_int *extended_data);

EDTAPI u_int       edt_get_board_id(EdtDev *edt_p);

/* These two functions set/get mezz id info from driver, not board */

EDTAPI u_int       edt_set_mezz_id(EdtDev *edt_p);
EDTAPI u_int       edt_get_mezz_id(EdtDev *edt_p);


EDTAPI int         edt_get_driver_version(EdtDev *edt_p,
        char *versionstr,
        int size);

EDTAPI int         edt_get_driver_buildid(EdtDev *edt_p,
        char *build,
        int size);

EDTAPI int         edt_get_library_version(EdtDev *edt_p,
        char *versionstr,
        int size);

EDTAPI int         edt_get_library_buildid(EdtDev *edt_p,
        char *build,
        int size);

EDTAPI u_int       edt_get_version_number();

EDTAPI int         edt_check_version(EdtDev *edt_p);
EDTAPI int         edt_get_kernel_event(EdtDev *edt_p, int event_num);
EDTAPI u_int       edt_get_dma_info(EdtDev * edt_p, edt_dma_info *dmainfo);

/** @} */ /* end dma_utility */

EDTAPI int         edt_pci_reboot(EdtDev *edt_p);
EDTAPI int         edt_set_merge(EdtDev * edt_p, u_int size, int span, u_int offset, u_int count) ;

EDTAPI int         edt_set_sync_interval(EdtDev *edt_p, u_int interval);

EDTAPI void        edt_set_buffer_granularity(EdtDev *edt_p,
                                              u_int granularity);

EDTAPI void        edt_reset_fifo(EdtDev *) ;

EDTAPI u_int       edt_set_sgbuf(EdtDev *edt_p, u_int sgbuf, u_int bufsize,
                                 u_int bufdir, u_int verbose) ;
EDTAPI u_int       edt_set_sglist(EdtDev *edt_p, u_int bufnum,
                                   u_int *log_list, u_int log_entrys) ;

EDTAPI int         edt_lockoff(EdtDev *edt_p) ;

EDTAPI int         edt_enable_event(EdtDev *edt_p, int event_type) ;
EDTAPI int         edt_reset_event_counter(EdtDev * edt_p, int event_type) ;
EDTAPI int         edt_wait_event(EdtDev *edt_p, int event_type,
        int timeoutval) ;
EDTAPI void        edt_dmasync_fordev(EdtDev *edt, int bufnum, int offset,
                                        int bytecount) ;
EDTAPI void        edt_dmasync_forcpu(EdtDev *edt, int bufnum, int offset,
                                      int bytecount) ;
EDTAPI u_int       edt_get_bufbytecount(EdtDev * edt_p, u_int *cur_buffer) ;
EDTAPI int         edt_little_endian(void) ;
/* deal with a hardware timeout */
/**
 * @addtogroup dma_inout
 * @{
 */
EDTAPI int         edt_do_timeout(EdtDev *edt_p);
/** @} */ /* end dma_inout */
EDTAPI int         edt_set_continuous(EdtDev *edt_p, int on) ;
EDTAPI void        edt_resume(EdtDev *edt_p) ;
EDTAPI void        edt_set_timetype(EdtDev *edt_p, u_int type) ;
EDTAPI caddr_t     edt_mapmem(EdtDev *edt_p, u_int addr, int size) ;
EDTAPI u_int       edt_get_mappable_size(EdtDev *edt_p, int bar); /* get the size of the base address range */
EDTAPI u_int       edt_get_drivertype(EdtDev *edt_p) ;
EDTAPI int         edt_set_drivertype(EdtDev *edt_p, u_int type) ;
EDTAPI void        edt_set_abortintr(EdtDev *edt_p, u_int val) ;

EDTAPI int         edt_write_pio(EdtDev *edt_p, u_char *buf, int size);

EDTAPI int         edt_set_max_buffers(EdtDev *edt_p, int newmax);
EDTAPI int         edt_get_max_buffers(EdtDev *edt_p);

EDTAPI int         edt_set_kernel_buffers(EdtDev *edt_p, int onoff);
EDTAPI int         edt_get_kernel_buffers(EdtDev *edt_p);
EDTAPI int         edt_set_persistent_buffers(EdtDev *edt_p, int onoff);
EDTAPI int         edt_get_persistent_buffers(EdtDev *edt_p);
EDTAPI int         edt_set_mmap_buffers(EdtDev *edt_p, int onoff);
EDTAPI int         edt_get_mmap_buffers(EdtDev *edt_p);

EDTAPI int         edt_get_kernel_alloc(EdtDev *edt_p, int pool);

EDTAPI void        edt_set_dump_reg_access(int on);
EDTAPI int         edt_get_dump_reg_access();
EDTAPI void        edt_set_dump_reg_address(u_int reglow, u_int n, u_int on);
EDTAPI void        edt_set_dump_ir_access(u_int on);

#ifdef __sun
EDTAPI void        edt_set_RT(u_int pri) ;
EDTAPI int         edt_use_umem_lock(EdtDev *edt_p, u_int use_lock) ;
EDTAPI int         edt_get_umem_lock(EdtDev *edt_p) ;
#endif


#if 0
EDTAPI int edt_get_x_file_header_magic(char *fname, char *header, int *size, int *magic);
EDTAPI int edt_get_x_array_header_rstat(u_char *ba, char *header, int *size);
EDTAPI u_char *edt_get_x_array_header(u_char *ba, char *header, int *size);
EDTAPI u_char *edt_get_x_array_header_magic(u_char *ba, char *header, int *size, int *magic);
EDTAPI int edt_get_x_header_magic(FILE *fp, char *header, int *size, int *magic);
#endif

EDTAPI void edt_readinfo(EdtDev *edt_p, int promcode, int sect, char *idstr, char *devinfo, char *oemsn);

EDTAPI Edt_bdinfo *edt_detect_boards(char *dev, int unit, int *nunits, int verbose);
EDTAPI Edt_bdinfo *edt_detect_boards_id(char *dev, int unit, u_int id, int *nunits, int verbose);
EDTAPI Edt_bdinfo *edt_detect_boards_ids(char *dev, int unit, u_int *ids, int *nunits, int verbose);
EDTAPI Edt_bdinfo *edt_detect_boards_filter(EdtBdFilterFunction filter, void *data, int *nunits, int verbose);

EDTAPI int edt_sector_erase(EdtDev *edt_p, u_int sector, u_int sec_size, int type);

/* routines to enable/disable register accesses tracing in the driver */

EDTAPI void edt_set_trace_regs(EdtDev *edt_p, u_int reg_def, u_int state);

EDTAPI void edt_trace_regs_enable(EdtDev *edt_p, u_int state);

EDTAPI int edtdev_channels_from_type(EdtDev *edt_p);
EDTAPI int edt_devtype_from_id(int id);
EDTAPI int edt_check_1_vs_4(EdtDev *edt_p);

EDTAPI void edt_set_intr_mask(EdtDev *edt_p, u_int state);
EDTAPI u_int edt_get_intr_mask(EdtDev *edt_p);
EDTAPI void edt_set_remote_intr(EdtDev *edt_p, u_int onoff);
EDTAPI u_int edt_get_remote_intr(EdtDev *edt_p);

EDTAPI int edt_mic_set_protected(EdtDev *edt_p);
EDTAPI int edt_mic_unset_protected(EdtDev *edt_p);
EDTAPI int edt_mic_is_protected(EdtDev *edt_p);

/*
 * routines for pciload instead of globals
 */
EDTAPI void            edt_flash_set_do_fast(int val) ;
EDTAPI void            edt_flash_set_force_slow(int val) ;
EDTAPI int             edt_flash_get_do_fast(void) ;
EDTAPI int             edt_flash_get_force_slow(void) ;
EDTAPI int             edt_flash_get_force(void) ;
EDTAPI int             edt_flash_get_debug_fast(void);
EDTAPI int             edt_flash_set_debug_fast(int val);

/* EDT_IOCTL definitions */
EDTAPI int             edt_ioctl(EdtDev *, int code,   void *arg);
EDTAPI int             edt_ioctl_nt(EdtDev *edt_p, int controlCode,
                                    void *inBuffer, int inSize, void *outBuffer,
                                    int outSize, int *bytesReturned) ;

EDTAPI uchar_t         pcd_get_funct(EdtDev *edt_p) ;
EDTAPI void            pcd_set_byteswap(EdtDev *edt_p, int val) ;
EDTAPI int             pcd_set_statsig(EdtDev  *edt_p, int sig) ;
EDTAPI uchar_t         pcd_get_stat(EdtDev *edt_p) ;
EDTAPI uchar_t         pcd_get_stat_polarity(EdtDev *edt_p) ;
EDTAPI void            pcd_set_stat_polarity(EdtDev *edt_p, uchar_t val)   ;
EDTAPI unsigned char   pcd_get_cmd(EdtDev *edt_p) ;
EDTAPI void            pcd_set_cmd(EdtDev *edt_p, uchar_t val) ;
EDTAPI void            pcd_flush_channel(EdtDev * edt_p, int channel) ;

#ifdef PCD
EDTAPI u_char          pcd_get_option(EdtDev *edt_p) ;
EDTAPI void            sse_shift(EdtDev *edt_p, int shift) ;
EDTAPI double          sse_set_out_clk(EdtDev * edt_p, double fmhz) ;
EDTAPI void            pcd_pio_init(EdtDev *edt_p) ;
EDTAPI void            pcd_pio_flush_fifo(EdtDev * edt_p) ;
EDTAPI int             pcd_pio_read(EdtDev *edt_p, u_char *buf, int size) ;
EDTAPI int             pcd_pio_write(EdtDev *edt_p, u_char *buf, int size) ;
EDTAPI void            pcd_pio_set_direction(EdtDev *edt_p, int direction) ;
EDTAPI void            pcd_pio_intfc_write(EdtDev *, u_int, u_char)  ;
EDTAPI u_char          pcd_pio_intfc_read(EdtDev *, u_int) ;
EDTAPI void            pcd_set_abortdma_onintr(EdtDev *edt_p, int flag) ;

#endif /* PCD */

#ifdef P16D
EDTAPI void            p16d_set_command(EdtDev *edt_p, u_short val) ;
EDTAPI void            p16d_set_config(EdtDev *edt_p, u_short val) ;
EDTAPI u_short         p16d_get_stat(EdtDev *edt_p) ;
EDTAPI u_short         p16d_get_command(EdtDev *edt_p) ;
EDTAPI u_short         p16d_get_config(EdtDev *edt_p) ;
#endif /* P16D */

#ifdef P11W
EDTAPI void            p11w_set_command(EdtDev *edt_p, u_short val) ;
EDTAPI void            p11w_set_config(EdtDev *edt_p, u_short val) ;
EDTAPI void            p11w_set_data(EdtDev *edt_p, u_short val) ;
EDTAPI u_short         p11w_get_command(EdtDev *edt_p) ;
EDTAPI u_short         p11w_get_config(EdtDev *edt_p) ;
EDTAPI u_short         p11w_get_stat(EdtDev *edt_p) ;
EDTAPI u_short         p11w_get_data(EdtDev *edt_p) ;
EDTAPI u_int           p11w_get_count(EdtDev *edt_p) ;
EDTAPI void            p11w_abortdma_onattn(EdtDev *edt_p, int flag) ;
EDTAPI void            p11w_set_abortdma_onintr(EdtDev *edt_p, int flag) ;

#endif /* P11W */

#ifdef P53B
EDTAPI EdtDev         *p53b_open(int unit, int bus_element_descriptor) ;
EDTAPI EdtDev         *p53b_open_generic(int unit) ;
EDTAPI EdtDev         *p53b_rtopen_notactive(int unit, int bus_element_descriptor) ;
EDTAPI int             p53b_rtactive(EdtDev *p53b_p, int active) ;
EDTAPI int             p53b_ioctl(EdtDev *p53b_p, int action, void *arg) ;
EDTAPI int             p53b_write(EdtDev *p53b_p, void *buf, int size) ;
EDTAPI int             p53b_read(EdtDev *p53b_p, void *buf, int size) ;
EDTAPI int             p53b_bm_read(EdtDev *p53b_p, void *buf, int size) ;
EDTAPI int             p53b_load(EdtDev *p53b_p, void *addr, int size, int offset) ;
EDTAPI int             p53b_close(EdtDev *p53b_p) ;
EDTAPI void            p53b_perror(char *str) ;
EDTAPI void            p53b_msleep(int msecs) ;
EDTAPI int             p53b_rt_blockwrite(EdtDev *p53b_p, int sa, int count, u_short *buf);
EDTAPI int             p53b_rt_blockread(EdtDev *p53b_p, int sa, int count, u_short *buf);
EDTAPI int             p53b_rt_prep_blockwrite(EdtDev *p53b_p, int sa) ;
EDTAPI int             p53b_rt_prep_blockread(EdtDev *p53b_p, int sa) ;
EDTAPI int             p53b_rt_flush_block_sa(EdtDev *p53b_p, u_int sa);
EDTAPI int             p53b_rt_flush_block_rcv(EdtDev *p53b_p);

EDTAPI int edt_get_type_from_id(int id);

#endif /* P53B */

#if defined(VXWORKS) || defined(TEST_VXWORKS)
EDTAPI int         edt_vx_system_register_func(const char *funcstr, int (funcptr)(char *));
#endif


#endif  /* ndef _KERNEL */

#define edt_set_eodma_sig(p, s) edt_set_eodma_int(p, s)

#define EDTIO_V0 0
#define EIO_ACTION_MASK 0x000003ff      /* action is low 10 bits */
#define EIO_SIZE_MASK   0x00fffc00      /* mask off bits 10-23 */
#define EIO_SET         0x02000000  /* set is bit 25 */
#define EIO_GET     0x01000000  /* get is bit 24 */
#define EIO_SET_MASK    EIO_SET     /* set mask is same as set */
#define EIO_GET_MASK    EIO_GET     /* get mask is same as get */
#define EIO_SIZE_SHIFT  10      /* size -- shift down 10 bits */
#define EIO_TYPE_SHIFT  24      /* to get type, shift down 22 bits */
#define EIO_DECODE_ACTION(code) (code & EIO_ACTION_MASK)
#define EIODA(code) EIO_DECODE_ACTION(code) /* shorthand, looks better in case stmts */
#define EIO_DECODE_SIZE(code)   ((code & EIO_SIZE_MASK) >> EIO_SIZE_SHIFT)
#define EIO_DECODE_SET(code)    ((code & EIO_SET_MASK) != 0)
#define EIO_DECODE_GET(code)    ((code & EIO_GET_MASK) != 0)


#if defined(__sun) || defined (_NT_) || defined(VXWORKS) || defined(__APPLE__)

#define EDT_NT_IOCTL        0xf000f000

#endif

#ifdef __linux__

#define EDT_IOC_MAGIC  'k'

#define EDT_NT_IOCTL    _IOWR(EDT_IOC_MAGIC, 1, edt_ioctl_struct)
#define EDT_NT_IOCTL32  _IOWR(EDT_IOC_MAGIC, 1, edt_ioctl_struct32)

#define EDT_IOC_MAXNR   2

#endif /* __linux__ */

/*
 * This structure must correspond to the next edt_ioctl_struct
 * in order for 32-bit programs to run under 64-bit Linux.
 * This workaround was done for ioctl backwards compatibility purposes.
 * Mark Mason - Aug 2007
 */

typedef struct {
#ifdef __APPLE__
    uint_t device ;
#else
    HANDLE device ;
#endif
    uint_t controlCode  ;
    uint_t inSize   ;
    uint_t outSize ;
    uint_t bytesReturned ;
    u_int inBuffer ;      /* userspace address */
    uint32_t outBuffer ;     /* userspace address */
#if defined(__APPLE__)
    u_short unit ;
    u_short channel ;
#endif
} edt_ioctl_struct32 ;

/*
 * This structure must correspond to the above edt_ioctl_struct32
 * in order for 32-bit programs to run under 64-bit Linux.
 * Mark Mason - Aug 2007
 */
typedef struct {
#ifdef __APPLE__
    uint32_t device ;
#else
    HANDLE device ;
#endif
    uint32_t controlCode  ;
    uint32_t inSize   ;
    uint32_t outSize ;
    uint32_t bytesReturned ;
#if defined(__APPLE__)
    uint16_t unit ;
    uint16_t channel ;
#endif
    void *inBuffer ;
    void *outBuffer ;
} edt_ioctl_struct ;


/* for passing 2 args */
/* moved to long for 64-bit */
typedef struct
{
    /* must be multiple of 64 bits for amd64 */
    uint64_t    value;
    uint_t      desc;
    uint_t  flags ;
} edt_buf;


/* for serial read and write */
#define EDT_SERBUF_SIZE 2048
#define EDT_SERBUF_OVRHD 16  /* size of all but buf */
/* flags for serial */
#define EDT_SERIAL_WAITRESP 1
#define EDT_SERIAL_SAVERESP 2
typedef struct
{
    uint_t      unit;
    uint_t      size;
    uint_t      misc;
    uint_t      flags;
    char        buf[EDT_SERBUF_SIZE];
} ser_buf;

/* for describing ring buffer */
typedef struct
{
    uint64_t    addr ;
    uint_t      index ;
    uint_t         size ;
    uint_t         writeflag ;
} buf_args;

/* for specifying merging data between multiple boards or channels */
/* or for flipping or interleaving image */
typedef struct
{
    uint_t      line_size ;
    int         line_span ; /* can be negative */
    uint_t      line_offset ;
    uint_t      line_count ;
} edt_merge_args;



typedef struct {
    u_int addr ;
    u_int size ;
    u_int inc  ;
    u_int cnt  ;
    u_int mask ;
} p53b_test ;

#define SIZED_DATASIZE (EDT_DEPSIZE - sizeof(u_int))

typedef struct {
    u_int size;
    u_int data[SIZED_DATASIZE/4];
} edt_sized_buffer;

#define EDT_DEVICE_TYPE     0x8000
#ifdef METHOD_BUFFERED
#define EDT_MAKE_IOCTL(t,c)\
    (uint_t)CTL_CODE((t),   0x800+(c),  METHOD_BUFFERED, FILE_ANY_ACCESS)
#else
#define EDT_MAKE_IOCTL(t,c)\
    (uint_t)(c)
#endif

#define EIOC(action, type, size) (((uint_t)type)    \
        | (((uint_t)size) << EIO_SIZE_SHIFT) \
        | ((uint_t)action))

#define EDTS_DEBUG              EIOC(10, EIO_SET, sizeof(uint_t))
#define EDTG_DEBUG              EIOC(11, EIO_GET, sizeof(uint_t))
#define EDTS_INTFC              EIOC(12, EIO_SET, sizeof(edt_buf))
#define EDTG_INTFC              EIOC(13, EIO_GET|EIO_SET, sizeof(edt_buf))
#define EDTS_REG                EIOC(14, EIO_SET, sizeof(edt_buf))
#define EDTG_REG                EIOC(15, EIO_GET|EIO_SET, sizeof(edt_buf))
#define EDTS_FLASH              EIOC(16, EIO_SET, sizeof(edt_buf))
#define EDTG_FLASH              EIOC(17, EIO_GET|EIO_SET, sizeof(edt_buf))
#define EDTG_CHECKBF            EIOC(18, EIO_GET|EIO_SET, sizeof(buf_args))
#define EDTS_PROG               EIOC(19, EIO_SET, sizeof(uint_t))
#define EDTG_PROG               EIOC(20, EIO_GET, sizeof(uint_t))
#define EDTS_PROG_READBACK      EIOC(21, EIO_SET, sizeof(uint_t))
#define EDTG_PROG_READBACK      EIOC(22, EIO_GET, sizeof(uint_t))
#define EDTS_MERGE_SG           EIOC(23, EIO_SET, sizeof(uint_t))
#define EDTG_MERGE_SG           EIOC(24, EIO_GET, sizeof(uint_t))

#define EDTS_DEBUG_MASK         EIOC(25, EIO_SET, sizeof(u_int))
#define EDTG_DEBUG_MASK         EIOC(26, EIO_GET|EIO_SET, sizeof(u_int))
#define EDTS_ALLOC_KBUFFER      EIOC(27, EIO_GET|EIO_SET, sizeof(buf_args))

#define EDTS_SERIAL             EIOC(29, EIO_SET, sizeof(uint_t))
#define EDTG_SERIAL             EIOC(30, EIO_GET, sizeof(uint_t))
#define EDTS_DEPENDENT          EIOC(31, EIO_SET, EDT_DEPSIZE)
#define EDTG_DEPENDENT          EIOC(32, EIO_GET, EDT_DEPSIZE)
#define EDTG_DEVID              EIOC(33, EIO_GET, sizeof(uint_t))
#define EDTS_RTIMEOUT           EIOC(34, EIO_SET, sizeof(uint_t))
#define EDTS_WTIMEOUT           EIOC(35, EIO_SET, sizeof(uint_t))
#define EDTG_BUFDONE            EIOC(36, EIO_GET, sizeof(bufcnt_t))
#define EDTS_NUMBUFS            EIOC(37, EIO_SET, sizeof(int))
#define EDTS_BUF                EIOC(38, EIO_SET, sizeof(buf_args))
#define EDTS_STARTBUF           EIOC(39, EIO_SET, sizeof(uint_t))
#define EDTS_WAITBUF            EIOC(40, EIO_SET|EIO_GET, sizeof(uint_t))
#define EDTS_FREEBUF            EIOC(41, EIO_SET, sizeof(uint_t))
#define EDTS_STOPBUF            EIOC(42, EIO_SET, sizeof(uint_t))
#define EDTG_BYTECOUNT          EIOC(44, EIO_GET, sizeof(uint_t))
#define EDTS_SETBUF             EIOC(45, EIO_SET, sizeof(int))
#define EDTS_ABORT_DELAY        EIOC(46, EIO_SET, sizeof(int))
#define EDTG_TIMEOUTS           EIOC(47, EIO_GET, sizeof(int))
#define EDTG_TRACEBUF           EIOC(48, EIO_GET, (EDT_TRACESIZE  * sizeof(int)))
#define EDTS_STARTDMA           EIOC(49, EIO_SET, sizeof(edt_buf))
#define EDTS_ENDDMA             EIOC(50, EIO_SET, sizeof(edt_buf))
#define EDTS_SERIAL_FIFO        EIOC(51, EIO_SET, sizeof(int))
#define EDTG_SERIAL_FIFO        EIOC(52, EIO_GET, sizeof(int))
#define EDTS_UNUSED0            EIOC(53, EIO_SET, sizeof(int))
#define EDTG_UNUSED1            EIOC(54, EIO_GET, sizeof(int))
#define EDTG_RTIMEOUT           EIOC(55, EIO_GET, sizeof(uint_t))
#define EDTG_WTIMEOUT           EIOC(56, EIO_GET, sizeof(uint_t))
#define EDTS_EODMA_SIG          EIOC(57, EIO_SET, sizeof(uint_t))
#define EDTS_SERIALWAIT         EIOC(58, EIO_SET|EIO_GET, sizeof(edt_buf))
#define EDTS_EVENT_SIG          EIOC(59, EIO_SET, sizeof(edt_buf))
#define EDTG_OVERFLOW           EIOC(60, EIO_GET, sizeof(u_int))
#define EDTS_AUTODIR            EIOC(61, EIO_SET, sizeof(u_int))
#define EDTS_FIRSTFLUSH         EIOC(62, EIO_SET, sizeof(u_int))
#define EDTG_CONFIG_COPY        EIOC(63, EIO_GET|EIO_SET, sizeof(edt_buf))
#define EDTG_CONFIG             EIOC(64, EIO_GET|EIO_SET, sizeof(edt_buf))
#define EDTS_CONFIG             EIOC(65, EIO_SET, sizeof(edt_buf))
#define P53B_REGTEST            EIOC(66, EIO_SET, sizeof(p53b_test))
#define EDTG_LONG               EIOC(67, EIO_GET|EIO_SET, sizeof(edt_buf))
#define EDTS_LONG               EIOC(68, EIO_SET, sizeof(edt_buf))
#define EDTG_SGTODO             EIOC(69, EIO_GET, (EDT_TRACESIZE *  4))
#define EDTG_SGLIST             EIOC(70, EIO_SET|EIO_GET, sizeof(buf_args))
#define EDTS_SGLIST             EIOC(71, EIO_SET, sizeof(buf_args))
#define EDTG_SGINFO             EIOC(72, EIO_SET|EIO_GET, sizeof(edt_buf))
#define EDTG_TIMECOUNT          EIOC(73, EIO_GET, sizeof(uint_t))
#define EDTG_PADDR              EIOC(74, EIO_GET, sizeof(uint_t))
#define EDTS_SYNC               EIOC(75, EIO_SET, sizeof(uint_t))
#define EDTS_WAITN              EIOC(76, EIO_SET, sizeof(uint_t))
#define EDTS_STARTACT           EIOC(77, EIO_SET, sizeof(uint_t))
#define EDTS_ENDACT             EIOC(78, EIO_SET, sizeof(uint_t))
#define EDTS_RESETCOUNT         EIOC(79, EIO_SET, sizeof(uint_t))
#define EDTS_RESETSERIAL        EIOC(80, EIO_SET, sizeof(uint_t))
#define EDTS_CLR_EVENT          EIOC(81, EIO_SET, sizeof(uint_t))
#define EDTS_ADD_EVENT_FUNC     EIOC(82, EIO_SET, sizeof(uint_t))
#define EDTS_DEL_EVENT_FUNC     EIOC(83, EIO_SET, sizeof(uint_t))
#define EDTS_WAIT_EVENT_ONCE    EIOC(84, EIO_SET, sizeof(uint_t))
#define EDTS_WAIT_EVENT         EIOC(85, EIO_SET, sizeof(uint_t))
#define EDTS_CLEAR_WAIT_EVENT   EIOC(86, EIO_SET, sizeof(uint_t))
#define EDTG_TMSTAMP            EIOC(87, EIO_SET|EIO_GET, sizeof(uint_t) * 3)
#define EDTS_TIMEOUT_ACTION     EIOC(88, EIO_SET, sizeof(uint_t))
#define EDTG_TIMEOUT_GOODBITS   EIOC(89, EIO_GET, sizeof(uint_t))
#define EDTS_BAUDBITS           EIOC(90, EIO_SET, sizeof(uint_t))
#define EDTG_REFTIME            EIOC(91, EIO_GET, sizeof(uint_t) * 2)
#define EDTS_REFTIME            EIOC(92, EIO_SET, sizeof(uint_t) * 2)
#define EDTS_REG_OR             EIOC(93, EIO_SET|EIO_GET, sizeof(edt_buf))
#define EDTS_REG_AND            EIOC(94, EIO_SET|EIO_GET, sizeof(edt_buf))
#define EDTG_GOODBITS           EIOC(95, EIO_GET, sizeof(uint_t))
#define EDTS_BURST_EN           EIOC(96, EIO_SET, sizeof(uint_t))
#define EDTG_BURST_EN           EIOC(97, EIO_GET, sizeof(uint_t))
#define EDTG_FIRSTFLUSH         EIOC(98, EIO_GET, sizeof(u_int))
#define EDTS_ABORT_BP           EIOC(99, EIO_SET, sizeof(uint_t))
#define EDTS_DMASYNC_FORDEV     EIOC(100, EIO_SET, sizeof(uint_t) * 3)
#define EDTS_DMASYNC_FORCPU     EIOC(101, EIO_SET, sizeof(uint_t) * 3)
#define EDTG_BUFBYTECOUNT       EIOC(102, EIO_GET, sizeof(uint_t) * 2)
#define EDTS_DOTIMEOUT          EIOC(103, EIO_SET, sizeof(uint_t))
#define EDTS_REFTMSTAMP         EIOC(104, EIO_SET, sizeof(uint_t))
#define EDTS_PDVCONT            EIOC(105, EIO_SET, sizeof(uint_t))
#define EDTS_PDVDPATH           EIOC(106, EIO_SET, sizeof(uint_t))
#define EDTS_RESET_EVENT_COUNTER EIOC(107, EIO_SET, sizeof(uint_t))
#define EDTS_DUMP_SGLIST        EIOC(108, EIO_SET, sizeof(uint_t))
#define EDTG_TODO               EIOC(109, EIO_GET, sizeof(u_int))
#define EDTS_RESUME             EIOC(110, EIO_SET, sizeof(u_int))
#define EDTS_TIMETYPE           EIOC(111, EIO_SET, sizeof(u_int))
#define EDTS_EVENT_HNDL         EIOC(112, EIO_SET, sizeof(edt_buf))
#define EDTS_MAX_BUFFERS        EIOC(113, EIO_SET, sizeof(u_int))
#define EDTG_MAX_BUFFERS        EIOC(114, EIO_GET, sizeof(u_int))
#define EDTS_WRITE_PIO          EIOC(115, EIO_SET, sizeof(edt_sized_buffer))
#define EDTS_PROG_XILINX        EIOC(116, EIO_SET, sizeof(edt_sized_buffer))
#define EDTS_MAPMEM             EIOC(117, EIO_GET | EIO_SET, sizeof(edt_buf))
#define EDTS_ETEC_ERASEBUF_INIT EIOC(118, EIO_SET, sizeof(uint_t) * 2)
#define EDTS_ETEC_ERASEBUF      EIOC(119, EIO_SET, sizeof(u_int))
#define EDTG_DRIVER_TYPE        EIOC(120, EIO_GET, sizeof(u_int))
#define EDTS_DRIVER_TYPE        EIOC(121, EIO_SET, sizeof(u_int))
#define EDTS_DRV_BUFFER         EIOC(122, EIO_SET | EIO_GET, sizeof(u_int))
#define EDTS_ABORTINTR          EIOC(123, EIO_SET, sizeof(u_int))
#define EDTS_CUSTOMER           EIOC(124, EIO_SET, sizeof(u_int))
#define EDTS_ETEC_SET_IDLE      EIOC(125, EIO_SET, sizeof(u_int) * 3)
#define EDTS_SOLARIS_DMA_MODE   EIOC(126, EIO_SET, sizeof(u_int))
#define EDTS_UMEM_LOCK          EIOC(127, EIO_SET, sizeof(u_int))
#define EDTG_UMEM_LOCK          EIOC(128, EIO_GET, sizeof(u_int))
#define EDTS_RCI_CHAN           EIOC(129, EIO_SET, sizeof(edt_buf))
#define EDTG_RCI_CHAN           EIOC(130, EIO_SET|EIO_GET, sizeof(edt_buf))
#define EDTS_BITPATH            EIOC(140, EIO_SET, sizeof(edt_bitpath))
#define EDTG_BITPATH            EIOC(141, EIO_GET, sizeof(edt_bitpath))
#define EDTG_VERSION            EIOC(142, EIO_GET, sizeof(edt_version_string))
#define EDTG_BUILDID            EIOC(143, EIO_GET, sizeof(edt_version_string))
#define EDTS_WAITCHAR           EIOC(144, EIO_SET, sizeof(edt_buf))
#define EDTS_PDMA_MODE          EIOC(145, EIO_SET, sizeof(u_int))
#define EDTG_MEMSIZE            EIOC(146, EIO_GET, sizeof(u_int))
#define EDTS_DIRECTION          EIOC(147, EIO_SET, sizeof(u_int))
#define EDTG_CLRCIFLAGS         EIOC(148, EIO_GET, sizeof(u_int))
#define EDTS_CLRCIFLAGS         EIOC(149, EIO_SET, sizeof(u_int))
#define EDTS_MERGEPARMS         EIOC(150, EIO_SET, sizeof(edt_merge_args))
#define EDTS_ABORTDMA_ONINTR    EIOC(151, EIO_SET, sizeof(u_int))
#define EDTS_FVAL_DONE          EIOC(152, EIO_SET, sizeof(u_char))
#define EDTG_FVAL_DONE          EIOC(153, EIO_GET, sizeof(u_char))
#define EDTG_LINES_XFERRED      EIOC(154, EIO_SET|EIO_GET, sizeof(u_int))
#define EDTS_PROCESS_ISR        EIOC(155, EIO_SET|EIO_GET, sizeof(u_int))
#define EDTS_CLEAR_DMAID        EIOC(156, EIO_SET, sizeof(u_int))
#define EDTS_DRV_BUFFER_LEAD    EIOC(157, EIO_SET | EIO_GET, sizeof(u_int))
#define EDTG_SERIAL_WRITE_AVAIL EIOC(158, EIO_GET, sizeof(u_int))
#define EDTS_USER_DMA_WAKEUP    EIOC(159, EIO_SET, sizeof(u_int))
#define EDTG_USER_DMA_WAKEUP    EIOC(160, EIO_GET, sizeof(u_int))
#define EDTG_WAIT_STATUS        EIOC(161, EIO_GET, sizeof(u_int))
#define EDTS_WAIT_STATUS        EIOC(162, EIO_GET, sizeof(u_int))
#define EDTS_TIMEOUT_OK         EIOC(163, EIO_SET, sizeof(u_int))
#define EDTG_TIMEOUT_OK         EIOC(164, EIO_GET, sizeof(u_int))
#define EDTS_MULTI_DONE         EIOC(165, EIO_GET, sizeof(u_int))
#define EDTG_MULTI_DONE         EIOC(166, EIO_GET, sizeof(u_int))
#define EDTS_TEST_LOCK_ON       EIOC(167, EIO_SET, sizeof(u_int))
#define EDTG_FVAL_LOW           EIOC(168, EIO_SET|EIO_GET, sizeof(u_int))
#define EDTS_BUF_MMAP           EIOC(169, EIO_SET|EIO_GET, sizeof(buf_args))
#define EDTS_MEZZ_BITPATH       EIOC(170, EIO_SET, sizeof(edt_bitpath))
#define EDTG_MEZZ_BITPATH       EIOC(171, EIO_GET, sizeof(edt_bitpath))
#define EDTG_DMA_INFO           EIOC(172, EIO_GET, sizeof(edt_dma_info))
#define EDTS_USER_FUNC          EIOC(173, EIO_SET | EIO_GET, sizeof(edt_sized_buffer))
#define EDTS_TEST_STATUS        EIOC(174, EIO_SET | EIO_GET, sizeof(u_int))
#define EDTS_KERNEL_ALLOC       EIOC(175, EIO_SET | EIO_GET, sizeof(u_int))
#define EDTG_RESERVED_PAGES     EIOC(176, EIO_GET, sizeof(u_int))
#define EDTS_RAW_SGLIST         EIOC(177, EIO_SET, sizeof(buf_args))
#define EDTS_IGNORE_SIGNALS     EIOC(178, EIO_SET, sizeof(u_int))
#define EDTS_TRACE_REG          EIOC(179, EIO_SET, sizeof(u_int))
#define EDTS_TIMESTAMP_LEVEL    EIOC(180, EIO_SET, sizeof(u_int))
#define EDTS_REG_BIT_CLEARSET   EIOC(181, EIO_SET, sizeof(edt_buf))
#define EDTS_REG_BIT_SETCLEAR   EIOC(182, EIO_SET, sizeof(edt_buf))
#define EDTS_REG_READBACK       EIOC(183, EIO_SET, sizeof(u_int))
#define EDTS_MEZZ_ID            EIOC(184, EIO_SET, sizeof(edt_buf))
#define EDTG_MEZZ_ID            EIOC(185, EIO_SET|EIO_GET, sizeof(edt_buf))
#define EDTG_NUMBUFS            EIOC(186, EIO_GET, sizeof(int))
#define EDTS_READ_STARTACT      EIOC(187, EIO_SET, sizeof(edt_buf))
#define EDTS_READ_ENDACT        EIOC(188, EIO_SET, sizeof(edt_buf))
#define EDTS_WRITE_STARTACT     EIOC(189, EIO_SET, sizeof(edt_buf))
#define EDTS_WRITE_ENDACT       EIOC(190, EIO_SET, sizeof(edt_buf))
#define EDTS_READ_START_DELAYS  EIOC(191, EIO_SET, sizeof(u_int))
#define EDTS_READ_END_DELAYS    EIOC(192, EIO_SET, sizeof(u_int))
#define EDTS_WRITE_START_DELAYS EIOC(193, EIO_SET, sizeof(u_int))
#define EDTS_WRITE_END_DELAYS   EIOC(194, EIO_SET, sizeof(u_int))
#define EDTS_INDIRECT_REG_BASE  EIOC(195, EIO_SET, sizeof(u_int))
#define EDTG_INDIRECT_REG_BASE  EIOC(196, EIO_GET, sizeof(u_int))
#define EDTS_BITLOAD            EIOC(197, EIO_SET|EIO_GET, sizeof(buf_args))
#define EDTS_MEZZLOAD           EIOC(198, EIO_SET|EIO_GET, sizeof(buf_args))
#define EDTS_PCILOAD            EIOC(199, EIO_SET, sizeof(buf_args))
#define EDTS_SYNC_INTERVAL      EIOC(200, EIO_SET, sizeof(u_int))
#define EDTG_SYNC_INTERVAL      EIOC(201, EIO_GET, sizeof(u_int))

#define EDTG_TRACE_SIZE         EIOC(202, EIO_GET, sizeof(u_int))
#define EDTS_TRACE_SIZE         EIOC(203, EIO_SET, sizeof(u_int))
#define EDTG_TRACE_ENTRIES      EIOC(204, EIO_GET, sizeof(u_int))
#define EDTG_TRACEBUF2          EIOC(205, EIO_GET, sizeof(buf_args))
#define EDTS_TRACE_CLEAR        EIOC(206, EIO_SET, sizeof(u_int))
#define EDTG_DEBUG_INFO         EIOC(207, EIO_SET|EIO_GET, sizeof(buf_args))
#define EDTG_MEM2SIZE           EIOC(208, EIO_GET, sizeof(u_int))
#define EDTS_INTR_MASK          EIOC(210, EIO_SET, sizeof(u_int))
#define EDTG_INTR_MASK          EIOC(211, EIO_GET, sizeof(u_int))
#define EDTS_IND_2_REG          EIOC(212, EIO_SET | EIO_GET, sizeof(edt_buf))
#define EDTG_IND_2_REG          EIOC(213, EIO_GET|EIO_SET, sizeof(edt_buf))
#define EDTS_DMA_MODE           EIOC(214, EIO_SET, sizeof(u_int))
#define EDTG_DMA_MODE           EIOC(215, EIO_GET, sizeof(u_int))
#define EDTG_BUFSIZE            EIOC(216, EIO_GET|EIO_SET, sizeof(u_int))
#define EDTS_DIRECT_DMA_DONE    EIOC(217, EIO_SET, sizeof(uint64_t))
#define EDTG_DIRECT_DMA_DONE    EIOC(218, EIO_GET, sizeof(uint64_t))
#define EDTS_WAIT_DIRECT_DMA_DONE EIOC(219, EIO_SET, sizeof(uint64_t))
#define EDTS_INCTIMEOUT         EIOC(220, EIO_SET, sizeof(uint_t))
#define EDTG_MAXCHAN            EIOC(221, EIO_GET, sizeof(int))

/* using unused codes */

/* defines for return from wait */
#define EDT_WAIT_OK 0
#define EDT_WAIT_TIMEOUT 1
#define EDT_WAIT_OK_TIMEOUT 2
#define EDT_WAIT_USER_WAKEUP 3

/* defines for driver type */
#define EDT_UNIX_DRIVER 0
#define EDT_NT_DRIVER   1
#define EDT_2K_DRIVER   2
#define EDT_WDM_DRIVER  3

/* defines for time type */
#define EDT_TM_SEC_NSEC  0
#define EDT_TM_CLICKS    1
#define EDT_TM_COUNTER   2
#define EDT_TM_FREQ  3
#define EDT_TM_INTR  4


/* defines for get DMA status */
#define EDT_DMA_IDLE 0
#define EDT_DMA_ACTIVE 1
#define EDT_DMA_TIMEOUT 2
#define EDT_DMA_ABORTED 3

/* defines for SG ioctls */
#define EDT_SGLIST_SIZE     1
#define EDT_SGLIST_VIRTUAL  2
#define EDT_SGLIST_PHYSICAL 3
#define EDT_SGTODO_SIZE     4
#define EDT_SGTODO_VIRTUAL  5
#define EDT_SGTODO_FIRST_SG 6
#define EDT_SG_LOADSIZE     7
#define EDT_SG_ALLOCSIZE    8
#define EDT_SG_TOTALUSED    9

/* defines for flush, start, end dma action */
#define EDT_ACT_NEVER       0
#define EDT_ACT_ONCE        1
#define EDT_ACT_ALWAYS      2
#define EDT_ACT_ONELEFT     3
#define EDT_ACT_CYCLE       4
#define EDT_ACT_KBS     5
#define EDT_ACT_ALWAYS_WRITEONLY        6

/* ddefines for timeout action */
#define EDT_TIMEOUT_NULL        0
#define EDT_TIMEOUT_BIT_STROBE  0x1


/* for kernel defines, use this shorthand */
#define EMAPI(x) EDT_MAKE_IOCTL(EDT_DEVICE_TYPE,EIODA(x))

#define ES_DEBUG             EMAPI(EDTS_DEBUG)
#define EG_DEBUG             EMAPI(EDTG_DEBUG)
#define ES_INTFC             EMAPI(EDTS_INTFC)
#define EG_INTFC             EMAPI(EDTG_INTFC)
#define ES_REG               EMAPI(EDTS_REG)
#define EG_REG               EMAPI(EDTG_REG)
#define ES_FLASH             EMAPI(EDTS_FLASH)
#define EG_FLASH             EMAPI(EDTG_FLASH)
#define EG_CHECKBF           EMAPI(EDTG_CHECKBF)
#define ES_PROG              EMAPI(EDTS_PROG)
#define EG_PROG              EMAPI(EDTG_PROG)
#define ES_PROG_READBACK     EMAPI(EDTS_PROG_READBACK)
#define EG_PROG_READBACK     EMAPI(EDTG_PROG_READBACK)
#define ES_MERGE_SG          EMAPI(EDTS_MERGE_SG)
#define EG_MERGE_SG            EMAPI(EDTG_MERGE_SG)
#define EG_DEBUG_SIZE            EMAPI(EDTG_DEBUG_SIZE)
#define EG_DEBUG_INFO             EMAPI(EG_DEBUG_INFO)
#define ES_TYPE              EMAPI(EDTS_TYPE)
#define EG_TYPE              EMAPI(EDTG_TYPE)
#define ES_SERIAL            EMAPI(EDTS_SERIAL)
#define EG_SERIAL            EMAPI(EDTG_SERIAL)
#define ES_DEPENDENT         EMAPI(EDTS_DEPENDENT)
#define EG_DEPENDENT         EMAPI(EDTG_DEPENDENT)
#define EG_DEVID             EMAPI(EDTG_DEVID)
#define ES_RTIMEOUT          EMAPI(EDTS_RTIMEOUT)
#define ES_WTIMEOUT          EMAPI(EDTS_WTIMEOUT)
#define EG_BUFDONE           EMAPI(EDTG_BUFDONE)
#define ES_NUMBUFS           EMAPI(EDTS_NUMBUFS)
#define ES_BUF               EMAPI(EDTS_BUF)
#define ES_BUF_MMAP               EMAPI(EDTS_BUF_MMAP)
#define ES_STARTBUF          EMAPI(EDTS_STARTBUF)
#define ES_WAITBUF           EMAPI(EDTS_WAITBUF)
#define ES_FREEBUF           EMAPI(EDTS_FREEBUF)
#define ES_STOPBUF           EMAPI(EDTS_STOPBUF)
#define EG_BYTECOUNT         EMAPI(EDTG_BYTECOUNT)
#define ES_SETBUF            EMAPI(EDTS_SETBUF)
#define ES_ABORT_DELAY          EMAPI(EDTS_ABORT_DELAY)
#define EG_TIMEOUTS          EMAPI(EDTG_TIMEOUTS)
#define EG_TRACEBUF          EMAPI(EDTG_TRACEBUF)
#define ES_STARTDMA          EMAPI(EDTS_STARTDMA)
#define ES_ENDDMA            EMAPI(EDTS_ENDDMA)
#define ES_SERIAL_FIFO           EMAPI(EDTS_SERIAL_FIFO)
#define EG_SERIAL_FIFO           EMAPI(EDTG_SERIAL_FIFO)
#define ES_UNUSED0          EMAPI(EDTS_UNUSED0)
#define EG_UNUSED1          EMAPI(EDTG_UNUSED1)
#define EG_RTIMEOUT          EMAPI(EDTG_RTIMEOUT)
#define EG_WTIMEOUT          EMAPI(EDTG_WTIMEOUT)
#define ES_EODMA_SIG         EMAPI(EDTS_EODMA_SIG)
#define ES_SERIALWAIT        EMAPI(EDTS_SERIALWAIT)
#define ES_EVENT_SIG         EMAPI(EDTS_EVENT_SIG)
#define EG_OVERFLOW          EMAPI(EDTG_OVERFLOW)
#define ES_AUTODIR           EMAPI(EDTS_AUTODIR)
#define ES_FIRSTFLUSH        EMAPI(EDTS_FIRSTFLUSH)
#define EG_FIRSTFLUSH        EMAPI(EDTG_FIRSTFLUSH)
#define EG_CONFIG_COPY       EMAPI(EDTG_CONFIG_COPY)
#define ES_CONFIG            EMAPI(EDTS_CONFIG)
#define EG_CONFIG            EMAPI(EDTG_CONFIG)
#define P_REGTEST            EMAPI(P53B_REGTEST)
#define ES_LONG              EMAPI(EDTS_LONG)
#define EG_LONG              EMAPI(EDTG_LONG)
#define EG_SGTODO            EMAPI(EDTG_SGTODO)
#define EG_SGLIST            EMAPI(EDTG_SGLIST)
#define ES_SGLIST            EMAPI(EDTS_SGLIST)
#define EG_SGINFO            EMAPI(EDTG_SGINFO)
#define EG_TIMECOUNT         EMAPI(EDTG_TIMECOUNT)
#define EG_PADDR             EMAPI(EDTG_PADDR)
#define ES_SYNC              EMAPI(EDTS_SYNC)
#define ES_WAITN             EMAPI(EDTS_WAITN)
#define ES_STARTACT          EMAPI(EDTS_STARTACT)
#define ES_ENDACT            EMAPI(EDTS_ENDACT)
#define ES_RESETCOUNT        EMAPI(EDTS_RESETCOUNT)
#define ES_RESETSERIAL       EMAPI(EDTS_RESETSERIAL)
#define ES_BAUDBITS          EMAPI(EDTS_BAUDBITS)
#define ES_CLR_EVENT         EMAPI(EDTS_CLR_EVENT)
#define ES_ADD_EVENT_FUNC    EMAPI(EDTS_ADD_EVENT_FUNC)
#define ES_DEL_EVENT_FUNC    EMAPI(EDTS_DEL_EVENT_FUNC)
#define ES_WAIT_EVENT_ONCE   EMAPI(EDTS_WAIT_EVENT_ONCE)
#define ES_WAIT_EVENT        EMAPI(EDTS_WAIT_EVENT)
#define EG_TMSTAMP           EMAPI(EDTG_TMSTAMP)
#define ES_CLEAR_WAIT_EVENT  EMAPI(EDTS_CLEAR_WAIT_EVENT)
#define ES_TIMEOUT_ACTION    EMAPI(EDTS_TIMEOUT_ACTION)
#define EG_TIMEOUT_GOODBITS  EMAPI(EDTG_TIMEOUT_GOODBITS)
#define EG_REFTIME           EMAPI(EDTG_REFTIME)
#define ES_REFTIME           EMAPI(EDTS_REFTIME)
#define ES_REG_OR            EMAPI(EDTS_REG_OR)
#define ES_REG_AND           EMAPI(EDTS_REG_AND)
#define EG_GOODBITS          EMAPI(EDTG_GOODBITS)
#define ES_BURST_EN          EMAPI(EDTS_BURST_EN)
#define EG_BURST_EN          EMAPI(EDTG_BURST_EN)
#define ES_ABORT_BP          EMAPI(EDTS_ABORT_BP)
#define ES_DOTIMEOUT         EMAPI(EDTS_DOTIMEOUT)
#define ES_INCTIMEOUT         EMAPI(EDTS_INCTIMEOUT)
#define ES_REFTMSTAMP           EMAPI(EDTS_REFTMSTAMP)
#define ES_DMASYNC_FORDEV     EMAPI(EDTS_DMASYNC_FORDEV)
#define ES_DMASYNC_FORCPU     EMAPI(EDTS_DMASYNC_FORCPU)
#define EG_BUFBYTECOUNT       EMAPI(EDTG_BUFBYTECOUNT)
#define ES_PDVCONT            EMAPI(EDTS_PDVCONT)
#define ES_PDVDPATH           EMAPI(EDTS_PDVDPATH)
#define ES_RESET_EVENT_COUNTER        EMAPI(EDTS_RESET_EVENT_COUNTER)
#define ES_DUMP_SGLIST        EMAPI(EDTS_DUMP_SGLIST)
#define EG_TODO         EMAPI(EDTG_TODO)
#define ES_RESUME           EMAPI(EDTS_RESUME)
#define ES_TIMETYPE            EMAPI(EDTS_TIMETYPE)
#define ES_EVENT_HNDL          EMAPI(EDTS_EVENT_HNDL)
#define ES_MAX_BUFFERS             EMAPI(EDTS_MAX_BUFFERS)
#define EG_MAX_BUFFERS             EMAPI(EDTG_MAX_BUFFERS)
#define ES_WRITE_PIO           EMAPI(EDTS_WRITE_PIO)
#define ES_PROG_XILINX             EMAPI(EDTS_PROG_XILINX)
#define ES_MAPMEM          EMAPI(EDTS_MAPMEM)
#define ES_ETEC_ERASEBUF_INIT    EMAPI(EDTS_ETEC_ERASEBUF_INIT)
#define ES_ETEC_ERASEBUF         EMAPI(EDTS_ETEC_ERASEBUF)
#define EG_DRIVER_TYPE         EMAPI(EDTG_DRIVER_TYPE)
#define ES_DRIVER_TYPE         EMAPI(EDTS_DRIVER_TYPE)
#define ES_DRV_BUFFER          EMAPI(EDTS_DRV_BUFFER)
#define ES_ABORTINTR           EMAPI(EDTS_ABORTINTR)
#define ES_CUSTOMER            EMAPI(EDTS_CUSTOMER)
#define ES_ETEC_SET_IDLE       EMAPI(EDTS_ETEC_SET_IDLE)
#define ES_SOLARIS_DMA_MODE    EMAPI(EDTS_SOLARIS_DMA_MODE)
#define ES_UMEM_LOCK           EMAPI(EDTS_UMEM_LOCK)
#define EG_UMEM_LOCK           EMAPI(EDTG_UMEM_LOCK)
#define ES_RCI_CHAN        EMAPI(EDTS_RCI_CHAN)
#define EG_RCI_CHAN        EMAPI(EDTG_RCI_CHAN)
#define ES_BITPATH         EMAPI(EDTS_BITPATH)
#define EG_BITPATH         EMAPI(EDTG_BITPATH)
#define EG_VERSION         EMAPI(EDTG_VERSION)
#define EG_BUILDID         EMAPI(EDTG_BUILDID)
#define ES_WAITCHAR         EMAPI(EDTS_WAITCHAR)
#define ES_PDMA_MODE           EMAPI(EDTS_PDMA_MODE)
#define ES_DIRECTION           EMAPI(EDTS_DIRECTION)
#define EG_MEMSIZE          EMAPI(EDTG_MEMSIZE)
#define EG_MEM2SIZE          EMAPI(EDTG_MEM2SIZE)
#define ES_CLRCIFLAGS         EMAPI(EDTS_CLRCIFLAGS)
#define EG_CLRCIFLAGS         EMAPI(EDTG_CLRCIFLAGS)
#define ES_MERGEPARMS        EMAPI(EDTS_MERGEPARMS)
#define ES_ABORTDMA_ONINTR        EMAPI(EDTS_ABORTDMA_ONINTR)
#define ES_FVAL_DONE         EMAPI(EDTS_FVAL_DONE)
#define EG_FVAL_DONE         EMAPI(EDTG_FVAL_DONE)
#define EG_LINES_XFERRED         EMAPI(EDTG_LINES_XFERRED)
#define ES_PROCESS_ISR         EMAPI(EDTS_PROCESS_ISR)
#define ES_CLEAR_DMAID         EMAPI(EDTS_CLEAR_DMAID)
#define ES_DRV_BUFFER_LEAD        EMAPI(EDTS_DRV_BUFFER_LEAD)
#define EG_SERIAL_WRITE_AVAIL        EMAPI(EDTG_SERIAL_WRITE_AVAIL)
#define ES_USER_DMA_WAKEUP        EMAPI(EDTS_USER_DMA_WAKEUP)
#define EG_USER_DMA_WAKEUP        EMAPI(EDTG_USER_DMA_WAKEUP)
#define EG_WAIT_STATUS        EMAPI(EDTG_WAIT_STATUS)
#define ES_WAIT_STATUS        EMAPI(EDTS_WAIT_STATUS)
#define ES_TIMEOUT_OK        EMAPI(EDTS_TIMEOUT_OK)
#define EG_TIMEOUT_OK        EMAPI(EDTG_TIMEOUT_OK)
#define ES_MULTI_DONE        EMAPI(EDTS_MULTI_DONE)
#define EG_MULTI_DONE        EMAPI(EDTG_MULTI_DONE)
#define ES_TEST_LOCK_ON        EMAPI(EDTS_TEST_LOCK_ON)
#define EG_FVAL_LOW        EMAPI(EDTG_FVAL_LOW)
#define ES_MEZZ_BITPATH      EMAPI(EDTS_MEZZ_BITPATH)
#define EG_MEZZ_BITPATH      EMAPI(EDTG_MEZZ_BITPATH)
#define EG_DMA_INFO      EMAPI(EDTG_DMA_INFO)
#define ES_USER_FUNC         EMAPI(EDTS_USER_FUNC)
#define ES_TEST_STATUS         EMAPI(EDTS_TEST_STATUS)
#define ES_KERNEL_ALLOC         EMAPI(EDTS_KERNEL_ALLOC)
#define EG_RESERVED_PAGES   EMAPI(EDTG_RESERVED_PAGES)
#define ES_RAW_SGLIST            EMAPI(EDTS_RAW_SGLIST)
#define ES_IGNORE_SIGNALS            EMAPI(EDTS_IGNORE_SIGNALS)
#define ES_TRACE_REG            EMAPI(EDTS_TRACE_REG)
#define ES_TIMESTAMP_LEVEL            EMAPI(EDTS_TIMESTAMP_LEVEL)
#define ES_REG_BIT_CLEARSET            EMAPI(EDTS_REG_BIT_CLEARSET)
#define ES_REG_BIT_SETCLEAR            EMAPI(EDTS_REG_BIT_SETCLEAR)
#define ES_REG_READBACK                EMAPI(EDTS_REG_READBACK)
#define ES_MEZZ_ID                EMAPI(EDTS_MEZZ_ID)
#define EG_MEZZ_ID                EMAPI(EDTG_MEZZ_ID)
#define EG_NUMBUFS                EMAPI(EDTG_NUMBUFS)
#define ES_READ_STARTACT    EMAPI(EDTS_READ_STARTACT)
#define ES_READ_ENDACT      EMAPI(EDTS_READ_ENDACT)
#define ES_WRITE_STARTACT   EMAPI(EDTS_WRITE_STARTACT)
#define ES_WRITE_ENDACT     EMAPI(EDTS_WRITE_ENDACT)
#define ES_READ_START_DELAYS    EMAPI(EDTS_READ_START_DELAYS)
#define ES_READ_END_DELAYS  EMAPI(EDTS_READ_END_DELAYS)
#define ES_WRITE_START_DELAYS   EMAPI(EDTS_WRITE_START_DELAYS)
#define ES_WRITE_END_DELAYS EMAPI(EDTS_WRITE_END_DELAYS)
#define ES_INDIRECT_REG_BASE    EMAPI(EDTS_INDIRECT_REG_BASE)
#define EG_INDIRECT_REG_BASE    EMAPI(EDTG_INDIRECT_REG_BASE)
#define ES_BITLOAD              EMAPI(EDTS_BITLOAD)
#define ES_MEZZLOAD             EMAPI(EDTS_MEZZLOAD)
#define ES_PCILOAD              EMAPI(EDTS_PCILOAD)
#define ES_SYNC_INTERVAL             EMAPI(EDTS_SYNC_INTERVAL)
#define EG_SYNC_INTERVAL              EMAPI(EDTG_SYNC_INTERVAL)
#define ES_DEBUG_MASK                EMAPI(EDTS_DEBUG_MASK)
#define EG_DEBUG_MASK                EMAPI(EDTG_DEBUG_MASK)
#define ES_ALLOC_KBUFFER             EMAPI(EDTS_ALLOC_KBUFFER)
#define EG_TRACE_SIZE           EMAPI(EDTG_TRACE_SIZE)
#define ES_TRACE_SIZE           EMAPI(EDTS_TRACE_SIZE)
#define EG_TRACE_ENTRIES        EMAPI(EDTG_TRACE_ENTRIES)
#define EG_TRACEBUF2            EMAPI(EDTG_TRACEBUF2)
#define ES_TRACE_CLEAR          EMAPI(EDTS_TRACE_CLEAR)
#define EG_MAXCHAN              EMAPI(EDTG_MAXCHAN)

#define ES_INTR_MASK              EMAPI(EDTS_INTR_MASK)
#define EG_INTR_MASK              EMAPI(EDTG_INTR_MASK)
// Direct DMA support:  0 - Normal DMA; 1 - Direct DMA; mcm 10/2012
#define ES_DMA_MODE       EMAPI(EDTS_DMA_MODE)
#define EG_DMA_MODE       EMAPI(EDTG_DMA_MODE)

#define EG_BUFSIZE          EMAPI(EDTG_BUFSIZE)

#define ES_WAIT_DIRECT_DMA_DONE       EMAPI(EDTS_WAIT_DIRECT_DMA_DONE)
#define ES_DIRECT_DMA_DONE       EMAPI(EDTS_DIRECT_DMA_DONE)
#define EG_DIRECT_DMA_DONE       EMAPI(EDTG_DIRECT_DMA_DONE)

#define ES_IND_2_REG             EMAPI(EDTS_IND_2_REG)
#define EG_IND_2_REG              EMAPI(EDTG_IND_2_REG)


/* MUST BE EQUAL AT LEAST ONE GREATER THAN THE HIGHEST EDT IOCTL ABOVE */
#define MIN_PCI_IOCTL 300
#define PCIIOC(action, type, size) EIOC(action+MIN_PCI_IOCTL, type, size)

/* signals for pcd */
#define  PCD_STAT1_SIG      1
#define  PCD_STAT2_SIG      2
#define  PCD_STAT3_SIG      3
#define  PCD_STAT4_SIG      4
#define  PCD_STATX_SIG      5

/* signals for p16d */
#define P16_DINT_SIG        1

/* signals for p11w */
#define P11_ATT_SIG     1
#define P11_CNT_SIG     2

/* signals for p53b */
#define P53B_SRQ_SIG       1
#define P53B_INTERVAL_SIG  2
#define P53B_MODECODE_SIG  3

/* board (really driver) types - so far we have 5 */

#define EDT_BDTYPE_UNKN 0
#define EDT_BDTYPE_PCD  1
#define EDT_BDTYPE_PDV  2
#define EDT_BDTYPE_P11W 3
#define EDT_BDTYPE_P16D 4
#define EDT_BDTYPE_P53B 5

#define EDT_N_BDTYPES   6 /* well, 6 including UNKN */

#define ID_IS_PCD(id) ((id == PCD20_ID) \
        || (id == PCD40_ID) \
        || (id == PCD60_ID) \
        || (id == PGP20_ID) \
        || (id == PGP40_ID) \
        || (id == PGP60_ID) \
        || (id == PGP_ECL_ID) \
        || (id == PGP_THARAS_ID) \
        || (id == PCDFCI_SIM_ID) \
        || (id == PCDFCI_PCD_ID) \
        || (id == PSS4_ID) \
        || (id == PSS16_ID) \
        || (id == PCDA_ID) \
        || (id == PCDCL_ID) \
        || (id == PCDA16_ID) \
        || (id == PE4CDA_ID) \
        || (id == PE4CDA16_ID) \
        || (id == PCDHSS_ID) \
        || (id == PGS4_ID) \
        || (id == PGS16_ID) \
        || (id == PCD_16_ID) \
        || (id == PCDFOX_ID) \
        || (id == PE8LX1_ID) \
        || (id == PE8LX16_LS_ID) \
        || (id == PE8LX16_ID) \
        || (id == PE8LX32_ID) \
        || (id == PE8G2V7_ID) \
        || (id == PE8G3S5_ID) \
        || (id == PE8G3A5_ID) \
        || (id == PE8G3KU_ID) \
        || (id == PE8G2CML_ID) \
        || (id == WSU1_ID) \
        || (id == SNAP1_ID) \
        || (id == PE4BL_RADIO_ID) \
        || (id == PE4BL_RXLFRADIO_ID) \
        || (id == PE4BL_TXLFRADIO_ID) \
        || (id == PE8BL_NIC_ID) \
        || (id == PE1BL_TIMING_ID) \
        || (id == LCRBOOT_ID) \
        || (id == PE4AMC16_ID) \
        || (id == PE8BL_WBDSP_ID) \
        || (id == PE1BL_WBADC_ID) \
        )

 /* move / remove these from this list as they are assigned */
#define ID_IS_UNKNOWN(id) ( \
          id == 0xffff  \
        )

#define ID_IS_1553(id) ( \
           ( id == P53B_ID) \
        || ( id == PE1_53B_ID) \
        )

#define ID_IS_SS(id) ((id == PSS4_ID) \
        || (id == PSS16_ID))

#define ID_IS_GS(id) ((id == PGS4_ID) \
        || (id == PGS16_ID))

#define ID_IS_LX(id) ( \
           (id == PE8LX1_ID) \
        || (id == PE8LX16_LS_ID) \
        || (id == PE4AMC16_ID) \
        || (id == PE4CDA_ID) \
        || (id == PE4CDA16_ID) \
        || (id == PE8LX16_ID) \
        || (id == PE8BL_NIC_ID) \
        || (id == PE4BL_RADIO_ID) \
        || (id == PE4BL_RXLFRADIO_ID) \
        || (id == PE4BL_TXLFRADIO_ID) \
        || (id == PE8LX32_ID) \
        )

#define ID_HAS_CHANREG(id) (ID_HAS_MEZZ(id) || (id == PCDA_ID))

#define ID_IS_PDV(id) ( \
            (id == PDV_ID) \
         || (id == PDVK_ID) \
         || (id == PDV44_ID) \
         || (id == PDVAERO_ID) \
         || (id == PDVCL_ID) \
         || (id == PE1DVVL_ID) \
         || (id == PE4DVVL_ID) \
         || (id == PE4DVCL_ID) \
         || (id == PE8DVCL_ID) \
         || (id == PE8DVCLS_ID) \
         || (id == PE8VLCLS_ID) \
         || (id == PDVCL2_ID) \
         || (id == PDVFOI_ID) \
         || (id == PDVFCI_AIAG_ID) \
         || (id == PDVFCI_USPS_ID) \
         || (id == PDVA_ID) \
         || (id == PDVFOX_ID) \
         || (id == PE4DVAFOX_ID) \
         || (id == PE8DVFOX_ID) \
         || (id == PE4DVVLFOX_ID) \
         || (id == PDVA16_ID) \
         || (id == PGP_RGB_ID) \
         || (id == PE4DVFCI_ID) \
         || (id == PE8DVFCI_ID) \
         || (id == PC104ICB_ID) \
        )

#define ID_IS_DVFOX(id) \
    (    (id == PDVFOX_ID) \
      || (id == PE4DVVLFOX_ID) \
      || (id == PE4DVAFOX_ID) \
      || (id == PE8DVAFOX_ID))

#define ID_IS_PCIE_DVFOX(id) \
    (    (id == PE4DVVLFOX_ID) \
      || (id == PE4DVAFOX_ID) \
      || (id == PE8DVAFOX_ID))

#define ID_IS_DVCL(id) \
    (    (id == PDVCL_ID) \
      || (id == PE1DVVL_ID) \
      || (id == PE4DVVL_ID) \
      || (id == PE4DVVLFOX_ID) \
      || (id == PE4DVCL_ID) \
      || (id == PE8DVCL_ID) \
      || (id == PE4DVAFOX_ID))

/* pcie cl simulators only  */
#define ID_IS_DVCLS(id) \
      ( (id == PE8DVCLS_ID) \
      || (id == PE8VLCLS_ID))

/* all cl simulators */
#define ID_IS_CLSIM(id) \
    (    (id == PDVCL2_ID) \
      || (ID_IS_DVCLS(id)))  

#define ID_IS_FCIUSPS(id) \
    (    (id == PDVFCI_USPS_ID) \
      || (id == PC104ICB_ID))

#define ID_HAS_IRIGB(id) \
    (    (id == PE1DVVL_ID) \
        || (id == PE4DVVL_ID) \
        || (id == PE4DVCL_ID) \
        || (id == PE8DVCL_ID) \
        || (id == PE4DVAFOX_ID) \
    )

#define ID_STORES_MACADDRS(id) \
    (      (ID_HAS_MEZZ(id)) \
        || (id == PE8G3S5_ID) \
        || (id == PE8G3A5_ID) \
        || (id == PE8G3KU_ID) \
        || (id == PE8G2CML_ID) \
        || (id == WSU1_ID) \
        || (id == SNAP1_ID) \
        || (id == LCRBOOT_ID) \
        || (id == PE8BL_NIC_ID) \
        || (id == PE4AMC16_ID) \
        || (id == PE8BL_WBDSP_ID) \
    )

#define ID_IS_LCRBLADE(id) \
    (      (id == PE4BL_RADIO_ID) \
        || (id == PE4BL_RXLFRADIO_ID) \
        || (id == PE4BL_TXLFRADIO_ID) \
        || (id == PE1BL_TIMING_ID) \
        || (id == PE8BL_NIC_ID) \
        || (id == PE8BL_WBDSP_ID) \
        || (id == PE1BL_WBADC_ID) \
    )

#define ID_IS_RADIOBLADE(id) \
    || (id == PE4BL_RADIO_ID) \
    || (id == PE4BL_RXLFRADIO_ID) \
    || (id == PE4BL_TXLFRADIO_ID) \
    )

/* ADD any devices that don't have a separate interface FPGA here */
#define ID_HAS_COMBINED_FPGA(id) \
    (  (id == P11W_ID) \
    || (id == P16D_ID) \
    || (id == PDVCL_ID) \
    || (id == PE4DVCL_ID) \
    || (id == PE8DVCL_ID) \
    || (id == PC104ICB_ID) \
    || (id == PE4CDA_ID) \
    || (id == PE4CDA16_ID) \
    || (id == PE8DVAFOX_ID) \
    || (id == PE4DVAFOX_ID) \
    || (id == PE4DVFCI_ID) \
    || (id == PE8DVFCI_ID) \
    || (id == PC104ICB_ID) \
    || (id == PE8DVCL2_ID) \
    || (id == PE8VLCLS_ID) \
    || (id == PDVFCI_AIAG_ID) \
    || (id == PDVFCI_USPS_ID) \
    || (id == PCDFCI_SIM_ID) \
    || (id == PE8G3S5_ID) \
    || (id == PE8G3A5_ID) \
    || (id == WSU1_ID) \
    || (id == SNAP1_ID) \
    || (id == PE4BL_RADIO_ID) \
    || (id == PE4BL_RXLFRADIO_ID) \
    || (id == PE4BL_TXLFRADIO_ID) \
    || (id == PE1BL_TIMING_ID) \
    || (id == PE8BL_NIC_ID) \
    || (id == PE8G2V7_ID) \
    || (id == PE1DVVL_ID) \
    || (id == PE4DVVL_ID) \
    || (id == PE4DVVLFOX_ID) \
    || (id == LCRBOOT_ID) \
    || (id == PE8BL_WBDSP_ID) \
    || (id == PE1BL_WBADC_ID) \
    || (id == PCDFCI_PCD_ID) \
    || (id == PE1BL_WBADC_ID) \
    )

#define ID_HAS_16BIT_PROM(id) \
    (  (id == PE8G3S5_ID)     \
    || (id == PE8G3A5_ID)     \
    || (id == WSU1_ID)        \
    || (id == SNAP1_ID)       \
    || (id == PE4CDA_ID)      \
    || (id == PE4CDA16_ID)    \
    || (id == PE8G3KU_ID)     \
    || (id == PE8G2CML_ID)     \
    )

#define ID_IS_MICRON_PROM(id) ( \
       (id == PE1DVVL_ID) \
    || (id == PE4DVVL_ID) \
    || (id == PE8VLCLS_ID) \
    || (id == PE4DVVLFOX_ID) \
    || (id == PE4BL_RADIO_ID) \
    || (id == PE4BL_RXLFRADIO_ID) \
    || (id == PE4BL_TXLFRADIO_ID) \
    || (id == PE1BL_TIMING_ID) \
    || (id == PE8BL_NIC_ID) \
    || (id == PE8G3A5_ID) \
    || (id == LCRBOOT_ID) \
    || (id == PE8BL_WBDSP_ID) \
    || (id == PE1BL_WBADC_ID) \
    )

#define ID_IS_MULTICHAN(id) \
    (  (id == PSS16_ID) \
    || (id == PSS4_ID) \
    || (id == PCDA16_ID) \
    || (id == PE4CDA_ID) \
    || (id == PE4CDA16_ID) \
    || (id == PGS4_ID) \
    || (id == PGS16_ID) \
    || (id == PE8LX16_ID) \
    || (id == PE8LX32_ID) \
    || (id == PE8G2V7_ID) \
    || (id == PE8BL_NIC_ID) \
    || (id == PE8LX32_ID) \
    || (id == PE4AMC16_ID) \
    || (id == PE8G3S5_ID) \
    || (id == PE8G3A5_ID) \
    || (id == PE8G3KU_ID) \
    || (id == PE8G2CML_ID) \
    || (id == WSU1_ID) \
    || (id == SNAP1_ID) \
    || (id == PE4BL_RADIO_ID) \
    || (id == PE4BL_RXLFRADIO_ID) \
    || (id == PE4BL_TXLFRADIO_ID) \
    || (id == PE8LX16_LS_ID) \
    || (id == PE8BL_WBDSP_ID) \
    )

#define ID_IS_2CHANNEL(id) \
    (  (id == PDVFCI_USPS_ID) \
    || (id == PE4DVFCI_ID) \
    || (id == PE8DVFCI_ID) \
    || (id == PE8VLCLS_ID) \
    || (id == PE8DVCLS_ID))

#define ID_IS_3CHANNEL(id) \
    (  (id == PE1DVVL_ID) \
    || (id == PE4DVVL_ID) \
    || (id == PE4DVCL_ID) \
    || (id == PE8DVCL_ID) \
    || (id == PDVCL_ID) \
    || (id == PC104ICB_ID))

#define ID_IS_4CHANNEL(id) \
    (  (id == PCD20_ID) \
    || (id == PCD40_ID) \
    || (id == PCD60_ID) \
    || (id == PGP20_ID) \
    || (id == PGP40_ID) \
    || (id == PGP60_ID) \
    || (id == PGP_ECL_ID) \
    || (id == PDVFOI_ID) \
    || (id == PDVAERO_ID) \
    || (id == PE4DVFOX_ID) \
    || (id == PE8DVFOX_ID) \
    || (id == PDVCL2_ID) )


#define ID_HAS_PCD_DIR_BIT(id) \
    (  (id == PCD20_ID) \
    || (id == PCD40_ID) \
    || (id == PCD60_ID) \
    || (id == PGP20_ID) \
    || (id == PGP40_ID) \
    || (id == PGP60_ID) \
    || (id == PCDA_ID) \
    || (id == PE4CDA_ID) \
    || (id == PDVAERO_ID) )


#define ID_IS_1OR4CHANNEL(id) \
    (  (id == PSS4_ID) \
    || (id == PGS4_ID) \
    || (id == PCDFOX_ID) \
    || (id == PE4CDA_ID) \
    || (id == PCDA_ID) \
    || (id == PCDCL_ID) \
    || (id == PDVFOX_ID) )

#define ID_IS_16CHANNEL(id) \
    (  (id == PCD_16_ID) \
    || (id == PSS16_ID) \
    || (id == PGS16_ID) \
    || (id == PCDA16_ID) \
    || (id == PE4CDA16_ID) \
    || (id == PE8LX16_ID) \
    || (id == WSU1_ID) \
    || (id == SNAP1_ID) \
    || (id == PE8BL_NIC_ID) \
    || (id == PE4BL_RADIO_ID) \
    || (id == PE4BL_RXLFRADIO_ID) \
    || (id == PE4BL_TXLFRADIO_ID) \
    || (id == PE8G3S5_ID) \
    || (id == PE8G3A5_ID) \
    || (id == PE8G3KU_ID) \
    || (id == PE8G2CML_ID) \
    || (id == PE8LX16_LS_ID) \
    || (id == PE4AMC16_ID) \
    || (id == PE8BL_WBDSP_ID) \
    )

#define ID_IS_32CHANNEL(id) \
    (  (id == PE8LX32_ID))

#define ID_IS_DUMMY(id) \
    (  (id == DMY_ID) \
    || (id == DMYK_ID))

#define ID_IS_1LANE(id) \
    (  (id == PE1_53B_ID) \
    || (id == PE1BL_TIMING_ID) \
    || (id == PE1_53B_ID) \
    || (id == PE1DVVL_ID) \
    || ( id == PE1BL_WBADC_ID) \
    )

#define ID_IS_4LANE(id) \
    (  (id == PE4CDA_ID) \
    || (id == PE4CDA16_ID) \
    || (id == PE4DVCL_ID) \
    || (id == PE4DVVL_ID) \
    || (id == PE4DVVLFOX_ID) \
    || (id == PE4DVAFOX_ID) \
    || (id == PE4DVFCI_ID) \
    || (id == PE4BL_RADIO_ID) \
    || (id == PE4BL_RXLFRADIO_ID) \
    || (id == PE4BL_TXLFRADIO_ID) \
    || (id == PE4AMC16_ID) \
    )

#define ID_IS_8LANE(id) \
    (  ( id == PE8DVAFOX_ID) \
    || ( id == PE8DVCL_ID) \
    || ( id == PE8DVFCI_ID) \
    || ( id == PE8LX1_ID) \
    || ( id == PE8LX16_ID) \
    || ( id == PE8LX16_LS_ID) \
    || ( id == PE8DVCL2_ID) \
    || ( id == PE8BL_NIC_ID) \
    || ( id == PE8LX32_ID) \
    || ( id == PE8G2V7_ID) \
    || ( id == PE8G3S5_ID) \
    || ( id == PE8G3A5_ID) \
    || ( id == PE8G3KU_ID) \
    || ( id == PE8G2CML_ID) \
    || ( id == PE8VLCLS_ID) \
    || ( id == PE8BL_WBDSP_ID) \
    )

#define ID_PCILOAD_INFO_NA(id) \
    (  ( id == PE8VLCLS_ID) \
    )

#define ID_HAS_MEZZ(id) (ID_IS_SS(id) || ID_IS_GS(id) || ID_IS_LX(id)) || id == PE8G2V7_ID

/**
* @addtogroup dma_utility
* @{
*/
#define has_pcda_direction_bit(edt_p) (ID_HAS_PCD_DIR_BIT(edt_p->devid))
#define edt_is_pdv(edt_p) (ID_IS_PDV(edt_p->devid))
#define edt_is_pcd(edt_p) (ID_IS_PCD(edt_p->devid))
#define edt_is_1553(edt_p) (ID_IS_1553(edt_p->devid))
#define edt_is_dvfox(edt_p) (ID_IS_DVFOX(edt_p->devid))
#define edt_is_pcie_dvfox(edt_p) (ID_IS_PCIE_DVFOX(edt_p->devid))
#define edt_is_dvcl(edt_p) (ID_IS_DVCL(edt_p->devid))
#define edt_is_simulator(edt_p) (ID_IS_CLSIM(edt_p->devid)) /* all simulators */
#define edt_is_dvcl2(edt_p) (ID_IS_CLSIM(edt_p->devid))     /* alias (old) */
#define edt_is_dvcls(edt_p) (ID_IS_DVCLS(edt_p->devid))     /* only PCIe (2-ch) simulators */
#define edt_is_fciusps(edt_p) (ID_IS_FCIUSPS(edt_p->devid))
#define edt_has_irigb(edt_p) (ID_HAS_IRIGB(edt_p->devid))
#define edt_has_combined_fpga(edt_p) (ID_HAS_COMBINED_FPGA(edt_p->devid))
#define edt_is_lcr_blade(edt_p) (ID_IS_LCRBLADE(edt_p->devid))
#define edt_is_radio_blade(edt_p) (ID_IS_RADIOBLADE(edt_p->devid))
#define edt_has_chanreg(edt_p) (ID_HAS_CHANREG(edt_p->devid))
#define edt_stores_macaddrs(edt_p) (ID_STORES_MACADDRS(edt_p->devid))
#define edt_is_16bit_prom(edt_p) (ID_HAS_16BIT_PROM(edt_p->devid))
#define edt_is_micron_prom(edt_p) (ID_IS_MICRON_PROM(edt_p->devid))
#define edt_is_multichan(edt_p) (ID_IS_MULTICHAN(edt_p->devid))
#define edt_is_2channel(edt_p) (ID_IS_2CHANNEL(edt_p->devid))
#define edt_is_3channel(edt_p) (ID_IS_3CHANNEL(edt_p->devid))
#define edt_is_4channel(edt_p) (ID_IS_4CHANNEL(edt_p->devid))
#define edt_is_1or4channel(edt_p) (ID_IS_1OR4CHANNEL(edt_p->devid))
#define edt_is_16channel(edt_p) (ID_IS_16CHANNEL(edt_p->devid))
#define edt_is_32channel(edt_p) (ID_IS_32CHANNEL(edt_p->devid))
#define edt_is_dummy(edt_p) (ID_IS_DUMMY(edt_p->devid))
#define edt_is_1lane(edt_p) (ID_IS_1LANE(edt_p->devid))
#define edt_is_4lane(edt_p) (ID_IS_4LANE(edt_p->devid))
#define edt_is_8lane(edt_p) (ID_IS_8LANE(edt_p->devid))
#define edt_is_unknown(edt_p) (ID_IS_UNKNOWN(edt_p->devid))
#define edt_pciload_info_na(edt_p) (ID_PCILOAD_INFO_NA(edt_p->devid))

#define edt_is_dv_multichannel(edt_p) (edt_is_dvcl(edt_p) || edt_is_dvfox(edt_p) || edt_p->devid == PDVAERO_ID)

/** @} */ /* end dma_utility */

/** @} */ /* end weakgroup dma */

/* definition for header data for buffers */
/* for disk I/O speed, header mem may be allocated with ring-buffer memory
even if header not in the DMA stream. This covers all possible relationships
of header info to acquisition info */

#ifndef HDR_TYPE_IRIG1

/* defined types of within-dma header data */
#define HDR_TYPE_NONE            0
#define HDR_TYPE_IRIG1           1
#define HDR_TYPE_FRAMECNT        2
#define HDR_TYPE_IRIG2           3
#define HDR_TYPE_BUFHEADER       4 /* fro general data (wrap) */

#endif

/* Driver Debugging */

#define TRL_NONE        0

#define TRL_CRITICAL    1
#define TRL_FATAL       1
#define TRL_ERROR       2
#define TRL_WARN        3
#define TRL_INFO        4
#define TRL_VERBOSE     5
#define TRL_REALVERBOSE 6
#define TRL_RESERVED7   7
#define TRL_RESERVED8   8
#define TRL_RESERVED9   9

#define EDT_DEBUG_LEVELS 10

/* Driver trace mask bits */

#define DBG_INIT          0x00000100
#define DBG_PNP           0x00000200
#define DBG_POWER         0x00000400
#define DBG_CREATE_CLOSE  0x00001000
#define DBG_IOCTLS        0x00002000
#define DBG_WRITE         0x00004000
#define DBG_READ          0x00008000
#define DBG_DPC           0x00010000
#define DBG_ISR           0x00020000
#define DBG_LOCKS         0x00040000
#define DBG_EVENTS        0x00080000
#define DBG_HW            0x00100000
#define DBG_REG           0x00200000
#define DBG_DMA           0x00400000
#define DBG_DMA_SETUP     0x00800000
#define DBG_SERIAL        0x01000000
#define DBG_BOARD         0x02000000
#define DBG_ALLOC         0x04000000
#define DBG_MEMMAP        0x08000000
#define DBG_TIME          0x10000000
#define DBG_TIMEOUT       0x20000000
#define DBG_INTR_EN       0x40000000
#define DBG_ALL           0xffffff00

#define N_DBG_STATES 24

/* The debug level is in the lower 8 bits of a debug mask */

#define DBG_MASK_VAL(x)     ((x) & 0xffffff00)
#define DBG_MASK_LEVEL(x)   ((x) & 0xff)
#define DBG_MASK(level,mask)  ((mask) | (level & 0xff))

#define DBG_READWRITE  (DBG_READ | DBG_WRITE)

/**
* @enum    EdtIOPort
*
* @brief   Values that represent a port on a card. This is to enforce typed access in C++
***/

enum EdtIOPort
{
    EDT_NO_PORT = -1,
    EDT_IO_PORT_0 = 0,
    EDT_IO_PORT_1 = 1,
    EDT_IO_PORT_2 = 2,
    EDT_IO_PORT_3 = 3,
    EDT_IO_PORT_4 = 4,
    EDT_IO_PORT_5 = 5,
    EDT_IO_PORT_6 = 6,
    EDT_IO_PORT_7 = 7,
    EDT_IO_PORT_8 = 8,
    EDT_IO_PORT_9 = 9,
    EDT_IO_PORT_10 = 10,
    EDT_IO_PORT_11 = 11,
    EDT_IO_PORT_12 = 12,
    EDT_IO_PORT_13 = 13,
    EDT_IO_PORT_14 = 14,
    EDT_IO_PORT_15 = 15,
    EDT_IO_PORT_16 = 16,
    EDT_IO_PORT_17 = 17,
    EDT_IO_PORT_18 = 18,
    EDT_IO_PORT_19 = 19,
    EDT_IO_PORT_20 = 20,
    EDT_IO_PORT_21 = 21,
    EDT_IO_PORT_22 = 22,
    EDT_IO_PORT_23 = 23,
    EDT_IO_PORT_24 = 24,
    EDT_IO_PORT_25 = 25,
    EDT_IO_PORT_26 = 26,
    EDT_IO_PORT_27 = 27,
    EDT_IO_PORT_28 = 28,
    EDT_IO_PORT_29 = 29,
    EDT_IO_PORT_30 = 30,
    EDT_IO_PORT_31 = 31,
    EDT_IO_PORT_32 = 32
};

/* Device load tasks - 
   1. enumerate boards.
   2. load bitfile.
   a. from specified name
   b. auto update
   3. verify bitfile.
   4. set serial number values
   5. display one board's values
*/
typedef enum {
    EDT_Enumerate,
    EDT_LoadProm,
    EDT_AutoUpdate,
    EDT_VerifyOnly,
    EDT_SetSerial,
    EDT_Display,
    EDT_Erase,
    EDT_CheckIdOnly,
    EDT_CheckUpdate,
    EDT_DumpFlash
} EdtLoadState;


#endif /* INCLUDE_edtlib_h */
