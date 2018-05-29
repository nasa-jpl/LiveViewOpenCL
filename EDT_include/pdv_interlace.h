/**
 * @file
 * PCI DV Library routines to reorder/interpolate image buffers from cameras (header file)
 */

#ifndef PDV_INTERLACE_H
#define PDV_INTERLACE_H

#ifdef __cplusplus
extern "C" {
#endif


	
#define PROCESS_ALL 0
#define PROCESS_SINGLE 1
#define PROCESS_NONE 2 /* disable */

typedef struct _EdtPostProc EdtPostProc;

typedef int (*post_process_f)(void *src,
			int width, 
			int height,
			void *dest, 
			EdtPostProc *pCtrl);

struct _EdtPostProc {
	/* swinterlace type */
	int func_type; 

	/* types are limited to
	   EDT_TYPE_BYTE
	   EDT_TYPE_USHORT
	   EDT_TYPE_BGR
	   EDT_TYPE_BGRA
	   */
	int src_type;
	int dest_type;

	/* pointer to actual processing function */
	post_process_f process;

	/* use the pdev (dd_p) to initialize */
	int (*defaultInit)(EdtDev *pDev, struct _EdtPostProc *pProc);

	int (*doinplace)();

	/* do for a single frame, or multiples */
	int process_mode; 

	/* extra stuff  usually defined in config file */
	int src_depth;
	int dest_depth;

	int order;
	int shrink;
	int offset;

	/* actual frame height in multi-frame buffers */
	int frame_height;

	/* offset for the odd field in interlaced frames */
	int interlace;

	/* file name of dynamically loaded routine */
	char dll_name[256];

	void *dll_handle;

	int  nTaps;
	PdvInterleaveTap taps[MAX_INTLV_TAPS];

} ;


EDTAPI void edt_set_post_process(EdtDev *edt_p, EdtPostProc *pProcessor);

EDTAPI void edt_set_process_mode(EdtDev *edt_p, int mode);

EDTAPI EdtPostProc * edt_get_default_processor(EdtDev *edt_p, 
	int src_type, 
	int dest_type, 
	int func_type);  
 
EDTAPI EdtPostProc * pdv_setup_postproc(PdvDev *pdv_p, 
					PdvDependent *dd_p, 
					EdtPostProc *pInCtrl);

EDTAPI int deIntlv_buffers(EdtPostProc *pCtrl, 
		       void *src_p, void *dest_p, 
		       int width, int height);


EDTAPI int 
pdv_set_postproc(EdtPostProc *pCtrl,
		 int depth,
		 int extdepth,
		 int frame_height,
		 int interlace,
		 int image_offset,
		 int order,
		 int n_intlv_taps,
		 PdvInterleaveTap *taps
		 );

EDTAPI EdtPostProc * 
pdv_lookup_postproc(int interlace,
		 int src_depth,
		 int depth
		 );


/* Loads external process library */

EDTAPI int
pdv_unload_postproc_module(EdtPostProc *pCtrl);

EDTAPI int
pdv_load_postproc_module(EdtPostProc *pCtrl,
			  char *name, int srcdepth, int destdepth);

EDTAPI void
pdv_set_full_bayer_parameters(int nSourceDepth,
			double scale[3],
			double gamma,
			int nBlackOffset,
			int bRedRowFirst,
			int bGreenPixelFirst,
			int quality,
			int bias,
			int gradientcolor);

#ifdef __cplusplus
}
#endif


#endif
