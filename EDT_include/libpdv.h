
/* #pragma ident "@(#)libpdv.h	1.128 07/25/07 EDT" */


/** @file 
 * Header file containing PDV Library function declarations. 
 */

#ifndef _LIBPDV_H_
#define _LIBPDV_H_



#ifdef __cplusplus

extern "C" {

#endif

#include "edtinc.h"

/** 
 * For convenience, we use PdvDev in PDV applications, and EdtDev
 * in applications for all other EDT applications. 
 */
typedef  EdtDev PdvDev ;

#ifndef bits2bytes
#define bits2bytes(bits) (((int)bits + 7) / 8)
#endif

/* pdv_serial_command_flagged  flags */
#define SCFLAG_NORESP	1

EDTAPI EdtDev *pdv_open(char *edt_devname, int unit);

EDTAPI PdvDev * pdv_open_device(const char *edt_devname, int unit, int channel, int verbose);
EDTAPI PdvDev * pdv_open_channel(const char *edt_devname, int unit, int channel);
EDTAPI void pdv_serial_txrx(PdvDev * pdv_p, char *txbuf, int txcount, char *rxbuf, 
	                    int rxcount, int timeout, u_char *wchar);

EDTAPI int pdv_close(PdvDev *pdv_p);
EDTAPI int pdv_read(PdvDev *pdv_p, unsigned char *buf,	unsigned long size) ;
EDTAPI unsigned char  *pdv_image(PdvDev *pdv_p) ;
EDTAPI void pdv_start_image(PdvDev *pdv_p) ;
EDTAPI void pdv_start_images(PdvDev *pdv_p, int count) ;
EDTAPI void  pdv_perror(char *str) ;
EDTAPI void  pdv_setdebug(PdvDev *pdv_p, int debug) ;
EDTAPI void  pdv_new_debug(int debug) ;
EDTAPI void pdv_start_hardware_continuous(PdvDev *pdv_p) ;
EDTAPI void pdv_stop_hardware_continuous(PdvDev *pdv_p) ;
EDTAPI void pdv_flush_fifo(PdvDev *pdv_p) ;
EDTAPI void pdv_flush_channel_fifo(PdvDev *pdv_p) ;
EDTAPI void pdv_set_interlace(PdvDev *pdv_p, int interlace) ;
EDTAPI unsigned char  *pdv_wait_image(PdvDev *pdv_p) ;
EDTAPI unsigned char  *pdv_last_image_timed(PdvDev *pdv_p, u_int *timep) ;
EDTAPI unsigned char  *pdv_wait_last_image_timed(PdvDev *pdv_p, u_int *timep) ;
EDTAPI unsigned char  *pdv_wait_image_timed(PdvDev *pdv_p, u_int *timep) ;
EDTAPI unsigned char  *pdv_wait_images_timed(PdvDev *pdv_p, int count, u_int *timep) ;
EDTAPI unsigned char  *pdv_wait_images(PdvDev *pdv_p, int count) ;

EDTAPI unsigned char  *pdv_wait_image_raw(PdvDev *pdv_p) ;
EDTAPI unsigned char  *pdv_last_image_timed_raw(PdvDev *pdv_p, u_int *timep, int doRaw) ;
EDTAPI unsigned char  *pdv_wait_last_image_timed_raw(PdvDev *pdv_p, u_int *timep, int doRaw) ;
EDTAPI unsigned char  *pdv_wait_image_timed_raw(PdvDev *pdv_p, u_int *timep, int doRaw) ;
EDTAPI unsigned char  *pdv_wait_images_timed_raw(PdvDev *pdv_p, int count, u_int *timep, int doRaw) ;
EDTAPI unsigned char  *pdv_wait_images_raw(PdvDev *pdv_p, int count) ;

EDTAPI char *pdv_get_cameratype(PdvDev *pdv_p);

EDTAPI char *pdv_get_camera_class(PdvDev * pdv_p);
EDTAPI char *pdv_get_camera_model(PdvDev * pdv_p);
EDTAPI char *pdv_get_camera_info(PdvDev * pdv_p);

EDTAPI char *pdv_camera_type(PdvDev *pdv_p);
EDTAPI int  pdv_get_width(PdvDev *pdv_p);
EDTAPI int  pdv_get_pitch(PdvDev *pdv_p);
EDTAPI int  pdv_set_width(PdvDev *pdv_p, int value);
EDTAPI int  pdv_get_height(PdvDev *pdv_p);

EDTAPI int  pdv_get_frame_height(PdvDev *pdv_p);
EDTAPI int  pdv_set_frame_height(PdvDev *pdv_p, int value);
EDTAPI u_char *pdv_get_interleave_data(PdvDev *pdv_p, u_char *buf, int bufnum);
EDTAPI int  pdv_bytes_per_line(int width, int depth);

EDTAPI int  pdv_setsize(PdvDev* pdv_p, int width, int height);
EDTAPI int  pdv_set_height(PdvDev *pdv_p, int value);
EDTAPI int  pdv_get_depth(PdvDev *pdv_p);
EDTAPI int  pdv_get_extdepth(PdvDev *pdv_p);
EDTAPI int  pdv_set_depth(PdvDev *pdv_p, int value);
EDTAPI int  pdv_set_extdepth(PdvDev *pdv_p, int value);
EDTAPI int  pdv_set_depth_extdepth(PdvDev * pdv_p, int depth, int extdepth);
EDTAPI int  pdv_set_depth_extdepth_dpath(PdvDev * pdv_p, int depth, int extdepth, u_int dpath);
EDTAPI void pdv_cl_set_base_channels(PdvDev *pdv_p, int htaps, int vtaps);
EDTAPI int  pdv_get_imagesize(PdvDev *pdv_p);
EDTAPI int  pdv_image_size(PdvDev * pdv_p);
EDTAPI int  pdv_get_dmasize(PdvDev *pdv_p);
EDTAPI int  pdv_get_rawio_size(PdvDev *pdv_p);
EDTAPI int  pdv_get_allocated_size(PdvDev *pdv_p);
EDTAPI int  pdv_get_fulldma_size(PdvDev *pdv_p, int *extrasizep);
EDTAPI int  pdv_set_shutter_method(PdvDev *pdv_p, int method, unsigned int mcl);
EDTAPI int  pdv_set_exposure(PdvDev *pdv_p, int value);
EDTAPI int  pdv_set_exposure_mcl(PdvDev *pdv_p, int value);
EDTAPI int  pdv_set_gain(PdvDev *pdv_p, int value);
EDTAPI int  pdv_set_blacklevel(PdvDev *pdv_p, int value);
EDTAPI int  pdv_set_binning(PdvDev *pdv_p, int xval, int yval);
EDTAPI int  pdv_set_mode(PdvDev *pdv_p, char *mode, int mcl);
EDTAPI int  pdv_get_exposure(PdvDev *pdv_p) ;
EDTAPI int  pdv_get_gain(PdvDev *pdv_p) ;
EDTAPI int  pdv_get_blacklevel(PdvDev *pdv_p) ;
EDTAPI int  pdv_set_aperture(PdvDev *edt_p, int value);
EDTAPI int  pdv_get_aperture(PdvDev *edt_p);
EDTAPI int  pdv_set_timeout(PdvDev *pdv_p, int value) ;
EDTAPI int  pdv_auto_set_timeout(PdvDev *pdv_p) ;
EDTAPI int  pdv_get_timeout(PdvDev *pdv_p) ;
EDTAPI int  pdv_update_values_from_camera(PdvDev *pdv_p) ;
EDTAPI int  pdv_overrun(PdvDev *pdv_p) ;
EDTAPI int  pdv_timeouts(PdvDev *pdv_p) ;
EDTAPI int  pdv_timeout_cleanup(PdvDev *pdv_p) ;
EDTAPI int  pdv_timeout_restart(PdvDev *pdv_p, int restart) ;
EDTAPI int  pdv_in_continuous(PdvDev *pdv_p);
EDTAPI int  pdv_serial_write(PdvDev *ed, const char *buf, int size) ;
EDTAPI int  pdv_serial_read(PdvDev *fd, char *buf, int size) ;
EDTAPI int  pdv_serial_read_blocking(PdvDev *fd, char *buf, int size) ;
EDTAPI int  pdv_serial_read_nullterm(PdvDev *fd, char *buf, int size, int nullterm) ;
EDTAPI void pdv_do_xregwrites(EdtDev *edt_p, Dependent *dd_p);

EDTAPI int  pdv_serial_read_enable(PdvDev *pdv_p);
EDTAPI int  pdv_is_serial_enabled(PdvDev *pdv_p);
EDTAPI int  pdv_serial_read_disable(PdvDev *pdv_p);
EDTAPI int  pdv_serial_check_enabled(PdvDev *pdv_p);

EDTAPI char *pdv_serial_term(PdvDev *ed) ;
EDTAPI void pdv_set_serial_delimiters(PdvDev *ed, char *newprefix, char *newterm) ;
EDTAPI char *pdv_serial_prefix(PdvDev *ed) ;
EDTAPI void pdv_reset_serial(PdvDev *pdv_p) ;
EDTAPI int  pdv_serial_command(EdtDev *pd, const char *cmd) ;
EDTAPI int  pdv_serial_command_flagged(EdtDev *pd, const char *cmd, u_int flag) ;
EDTAPI int  pdv_serial_binary_command(EdtDev *pd, const char *cmd, int len) ;
EDTAPI int  pdv_serial_binary_command_flagged(EdtDev *pd, const char *cmd, int len, u_int flag) ;
EDTAPI int  pdv_send_basler_frame(EdtDev *pd, u_char *cmd, int len) ;
EDTAPI int  pdv_read_basler_frame(EdtDev *pd, u_char *cmd, int len) ;
EDTAPI int  pdv_read_duncan_frame(EdtDev * pdv_p, u_char *frame) ;
EDTAPI int  pdv_send_duncan_frame(PdvDev * pdv_p, u_char *cmdbuf, int size);
EDTAPI int  pdv_serial_command_hex(EdtDev *pd, const char *cmd, int len) ;
EDTAPI int  pdv_serial_wait(EdtDev *pd, int msecs, int count) ;
EDTAPI int  pdv_serial_get_numbytes(EdtDev *pd) ;
EDTAPI int  pdv_serial_wait_next(EdtDev * edt_p, int msecs, int count) ;

EDTAPI int  pdv_serial_write_available(PdvDev *pdv_p);

EDTAPI int  pdv_get_serial_block_size(void);
EDTAPI void pdv_set_serial_block_size(int newsize);


EDTAPI int  pdv_interlace_method(EdtDev *pd) ;
EDTAPI int  pdv_read_response(EdtDev *pd, char *buf) ;
EDTAPI int pdv_debug_level(void) ;
EDTAPI u_char ** pdv_buffer_addresses(EdtDev *edt_p) ;
EDTAPI uchar_t *pdv_alloc(int size) ;
EDTAPI void pdv_free(uchar_t *ptr) ;
EDTAPI int  ES10deInterleave(u_char *src, int width, int rows, u_char *dest) ;
EDTAPI int  ES10_byte64(u_char *src, int width, int rows, u_char *dest) ;
EDTAPI int  ES10_word_deInterleave(u_short *src, int width, int rows, u_short *dest) ;
EDTAPI int  ES10_word_deInterleave_odd(u_short *src, int width, int rows, u_short *dest) ;
EDTAPI int  ES10_word_deInterleave_hilo(u_short *src, int width, int rows, u_short *dest) ;
EDTAPI int  dalsa_4ch_deInterleave(u_char *src, int width, int rows, u_char *dest);
EDTAPI int  dalsa_2ch_deInterleave(u_char *src, int width, int rows, u_char *dest);
EDTAPI int  specinst_4port_deInterleave(u_short *src, int width, int rows, u_short *dest);
EDTAPI int  pdv_multibuf(EdtDev *edt_p, int numbufs) ;
EDTAPI int  merge_image(u_char *evenptr, u_char *oddptr,int width,
			int rows, int depth, u_char *dest, int offset) ;
EDTAPI void pdv_mark_ras(u_char *buf, int n, int width, int height, int x, int y) ;
EDTAPI void pdv_mark_bin(u_char *buf, int n, int width, int height, int x, int y) ;
EDTAPI void pdv_mark_ras_depth(void *buf, int n, int width, int height, int x, int y, int depth, int fg) ;
EDTAPI void pdv_mark_bin_16(u_short *buf, int n, int width, int height, int x, int y) ;
EDTAPI int little_endian(void);
EDTAPI int pdv_set_serial_parity(EdtDev  *edt_p, char polarity);
EDTAPI int pdv_set_baud(EdtDev  *edt_p, int  baud);
EDTAPI int pdv_get_baud(EdtDev  *edt_p);
EDTAPI void pdv_check_fpga_rev(PdvDev *pdv_p);
EDTAPI void pdv_check(EdtDev *edt_p) ;
EDTAPI void pdv_checkfrm(EdtDev *edt_p, u_short *image, u_int imagesize, int verbose) ;
EDTAPI int pdv_set_roi(PdvDev *pdv_p, int hskip, int hactv, int vskip, int vactv) ;
EDTAPI int pdv_get_roi_enabled(PdvDev *pdv_p);
EDTAPI int  pdv_auto_set_roi(PdvDev *pdv_p) ;
EDTAPI int pdv_enable_roi(PdvDev *pdv_p, int flag) ;
EDTAPI int pdv_set_cam_width(PdvDev *pdv_p, int value) ;
EDTAPI int pdv_set_cam_height(PdvDev *pdv_p, int value) ;
EDTAPI int pdv_access(char *fname, int perm) ;
EDTAPI int pdv_strobe(PdvDev *pdv_p, int count, int interval) ;
EDTAPI int pdv_set_strobe_dac(PdvDev * pdv_p, u_int value) ;
EDTAPI int pdv_set_strobe_counters(PdvDev * pdv_p, int count, int delay, int period);
EDTAPI int pdv_enable_strobe(PdvDev * pdv_p, int ena);
EDTAPI int pdv_strobe_method(PdvDev *pdv_p);

EDTAPI int pdv_read_response(PdvDev *pdv_p, char *buf);

EDTAPI int pdv_set_strobe_dac(PdvDev *pdv_p, u_int value);

EDTAPI void pdv_setup_continuous(PdvDev *pdv_p) ;
EDTAPI void pdv_setup_continuous_channel(PdvDev *pdv_p) ;
EDTAPI void pdv_stop_continuous(PdvDev *pdv_p) ;

EDTAPI int pdv_get_min_shutter(EdtDev *edt_p);
EDTAPI int pdv_get_max_shutter(EdtDev *edt_p);
EDTAPI int pdv_get_min_gain(EdtDev *edt_p);
EDTAPI int pdv_get_max_gain(EdtDev *edt_p);
EDTAPI int pdv_get_min_offset(EdtDev *edt_p);
EDTAPI int pdv_get_max_offset(EdtDev *edt_p);
EDTAPI void pdv_invert(PdvDev *pd, int val);
EDTAPI int pdv_get_invert(PdvDev * pdv_p);
EDTAPI void pdv_set_firstpixel_counter(PdvDev * pdv_p, int ena);
EDTAPI int pdv_get_firstpixel_counter(PdvDev * pdv_p);
EDTAPI void pdv_send_break(EdtDev *edt_p);

EDTAPI int pdv_set_header_type(PdvDev *pdv_p, int header_type, int irig_slave, int irig_offset, int irig_raw);
EDTAPI void pdv_set_header_size(EdtDev *edt_p, int nSize);
EDTAPI int pdv_get_header_size(EdtDev *edt_p);
EDTAPI void pdv_set_header_position(EdtDev *edt_p, HdrPosition pos);
EDTAPI HdrPosition pdv_get_header_position(EdtDev *edt_p);
EDTAPI void pdv_set_header_offset(EdtDev *edt_p, int noffset);
EDTAPI int pdv_get_header_offset(EdtDev *edt_p);
EDTAPI void pdv_set_header_dma(EdtDev *edt_p, int ndma);
EDTAPI int pdv_get_header_dma(EdtDev *edt_p);

EDTAPI int pdv_get_dmasize(EdtDev *pdv_p);

EDTAPI unsigned char * pdv_get_last_image(PdvDev * pdv_p);
EDTAPI unsigned char * pdv_wait_last_image(PdvDev * pdv_p, int *nSkipped);
EDTAPI unsigned char * pdv_wait_next_image(PdvDev * pdv_p, int *nSkipped);
EDTAPI unsigned char * pdv_wait_last_image_raw(PdvDev * pdv_p, int *nSkipped, int doRaw);
EDTAPI unsigned char * pdv_wait_next_image_raw(PdvDev * pdv_p, int *nSkipped, int doRaw);

EDTAPI int pdv_set_buffers(PdvDev *pdv_p, int nbufs, unsigned char **pBufs);
EDTAPI int pdv_set_buffers_x(PdvDev *pdv_p, int nbufs, int size, unsigned char **pBufs);

EDTAPI int pdv_get_cam_width(PdvDev *pdv_p);
EDTAPI int pdv_get_cam_height(PdvDev *pdv_p);
EDTAPI int pdv_force_single(PdvDev *pdv_p);
EDTAPI int pdv_variable_size(PdvDev *pdv_p);
EDTAPI int pdv_pause_for_serial(PdvDev *pdv_p);
EDTAPI int pdv_get_shutter_method(PdvDev *pdv_p, u_int *mcl);
EDTAPI int pdv_shutter_method(PdvDev *pdv_p);

EDTAPI int pdv_get_serial_timeout(PdvDev *pdv_p);

EDTAPI int pdv_dalsa_ls_set_expose(PdvDev *pdv_p, int hskip, int hactv) ;
EDTAPI int pdv_is_kodak_i(PdvDev *pdv_p);
EDTAPI void  pdv_set_defaults(PdvDev *pdv_p) ;

EDTAPI int pdv_is_atmel(PdvDev *pdv_p);
EDTAPI int pdv_hamamatsu(PdvDev *pdv_p);
EDTAPI void pdv_enable_external_trigger(PdvDev *pdv_p, int flag);

EDTAPI void pdv_set_fval_done(PdvDev *pdv_p, int enable);
EDTAPI int pdv_get_fval_done(PdvDev *pdv_p);
EDTAPI int pdv_get_lines_xferred(PdvDev *pdv_p);
EDTAPI int pdv_get_width_xferred(PdvDev *pdv_p);
EDTAPI int pdv_cl_get_fv_counter(PdvDev *pdv_p);
EDTAPI void pdv_cl_reset_fv_counter(PdvDev *pdv_p);
EDTAPI int pdv_cl_camera_connected(PdvDev *pdv_p);

EDTAPI int pdv_reset_dma_framecount(PdvDev *pdv_p);

EDTAPI int pdv_set_frame_period(PdvDev *pdv_p, int rate, int method);
EDTAPI int pdv_get_frame_period(PdvDev *pdv_p);
EDTAPI int pdv_cameralink_foiunit(PdvDev *pdv_p);
EDTAPI int pdv_is_cameralink(PdvDev *pdv_p);
EDTAPI int pdv_is_simulator(PdvDev *pdv_p);

EDTAPI void pdv_start_expose(PdvDev *pdv_p);

EDTAPI int pdv_set_exposure_basler202k(PdvDev * pdv_p, int value);
EDTAPI int pdv_set_gain_basler202k(PdvDev * pdv_p, int valuea, int valueb);
EDTAPI int pdv_set_offset_basler202k(PdvDev * pdv_p, int valuea, int valueb);
EDTAPI int pdv_set_exposure_duncan_ch(PdvDev * pdv_p, int value, int ch);
EDTAPI int pdv_set_gain_duncan_ch(PdvDev * pdv_p, int value, int ch);

EDTAPI int pdv_process_inplace(PdvDev *pdv_p);
EDTAPI int pdv_deinterlace(PdvDev *pdv_p, PdvDependent *dd_p, 
			u_char *dmabuf, u_char *output_buf);

EDTAPI int pdv_check_framesync(PdvDev *pdv_p, u_char *image_p, u_int *framecnt);
EDTAPI int pdv_enable_framesync(PdvDev *pdv_p, int mode);
EDTAPI int pdv_framesync_mode(PdvDev *pdv_p);

#ifndef PDV_TIMESTAMP_SIZE

#define PDV_TIMESTAMP_SIZE 8

#endif

/* Bayer filter interpolation routines */

extern double bayer_rgb_scale[3];

void EDTAPI get_bayer_luts(u_char **red, u_char **green, u_char **blue);

void EDTAPI set_bayer_parameters(int input_bits,
					 double rgb_scale[3],
					 double gamma,
					 int blackoffset,
					 int red_first,
					 int green_first);

/* Some cameras need different scale factors on even/odd rows */

void EDTAPI set_bayer_even_odd_row_scale(double evenscale, double oddscale);

extern double bayer_even_row_scale, bayer_odd_row_scale;


/********** DVC Routines ****************/

typedef struct _tagDVCState {
	char mode[4];

	int binx;
	int biny;
	int exposure;
	int blackoffset;
	int gain;

} DVCState;

EDTAPI int  pdv_set_binning_dvc(PdvDev *pdv_p, int xval, int yval);

EDTAPI int  pdv_set_mode_dvc(PdvDev *pdv_p, char *mode);

EDTAPI int pdv_is_dvc(PdvDev *pdv_p);

EDTAPI int pdv_update_from_dvc(PdvDev *pdv_p);

EDTAPI int pdv_get_dvc_state(PdvDev *pdv_p, DVCState *pState);

EDTAPI int pdv_set_dvc_state(PdvDev *pdv_p, DVCState *pState);

EDTAPI int pdv_set_waitchar(PdvDev *pdv_p, int enable, u_char wchar) ;
EDTAPI int pdv_get_waitchar(PdvDev * pdv_p, u_char *wchar) ;


#include "edt_types.h"

#include "pdv_interlace.h"



#ifdef __cplusplus

}

#endif

#endif /* _LIBPDV_H_ */

