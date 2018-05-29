

#ifndef BAYER_FILTER_DLL_H
#define BAYER_FILTER_DLL_H


#ifdef __cplusplus

extern "C" {

#endif

#ifdef EDT_MMX_STATIC

#define EDT_MMX_DLLEXPORT 


#elif defined(EDT_MMX_EXPORTS)

#define EDT_MMX_DLLEXPORT __declspec(dllexport)


#else

#define EDT_MMX_DLLEXPORT __declspec(dllimport)

#endif


void EDT_MMX_DLLEXPORT
set_bayer_parameters_mmx(int input_bits,
					 double rgb_scale[3],
					 double gamma,
					 int blackoffset,
					 int red_row_first,
					 int green_pixel_first);


int EDT_MMX_DLLEXPORT
convert_bayer_image_BGR_mmx(u_char * src, 
						   int width, int rows, int pitch, 
						   u_char * dest, 
					   int shrink, int order, int depth);


extern int EDT_MMX_DLLEXPORT bayer_can_use_mmx();

//extern int EDT_MMX_DLLEXPORT bayer_quality;

//extern int EDT_MMX_DLLEXPORT bayer_gradient_color;

void EDT_MMX_DLLEXPORT set_bayer_quality(int quality);

void EDT_MMX_DLLEXPORT set_bayer_gradient_color(int color);

int EDT_MMX_DLLEXPORT get_bayer_quality(void);

int EDT_MMX_DLLEXPORT get_bayer_gradient_color(void);


void EDT_MMX_DLLEXPORT set_bayer_bias_value(int nvalue);

int EDT_MMX_DLLEXPORT 
deIntlv_ES10_8_mmx_inplace(u_char * src, int width, int rows, u_char * dest);

int EDT_MMX_DLLEXPORT 
deIntlv_ES10_8_mmx(u_char * src, int width, int rows, u_char * dest);

int EDT_MMX_DLLEXPORT 
deIntlv_ES10_8_mmx_inplace(u_char * src, int width, int rows, u_char * dest);

int EDT_MMX_DLLEXPORT 
dalsa_4ch_piranha_deinterleave_mmx(u_char *src, int width, int rows, 
							   u_char *dest);

int EDT_MMX_DLLEXPORT 
Inv_Rt_2ch_deInterleave_24_12_mmx(u_char *src, int width, int rows, 
							   u_short *dest);


#ifdef __cplusplus

}

#endif

#endif
