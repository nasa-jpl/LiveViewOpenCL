
#ifndef _PDV_INTERLACE_METHODS_H
#define _PDV_INTERLACE_METHODS_H

#ifdef __cplusplus
extern "C" {
#endif

EDTAPI int
convert_bayer_image_16_BGR(u_short * src, int width, int rows, int pitch, 
			   u_char * dest, int order, int depth);

EDTAPI int
convert_bayer_image_8_BGR(u_char * src, int width, int rows, int pitch, 
			   u_char * dest, int order);

int
quadrant_deInterleave_16(u_short *src, int width, int rows, u_short *dest);

int
illunis_deInterleave(u_short *src, int width, int rows, u_short *dest);

int
illunis_deInterleave8(u_char *src, int width, int rows, u_char *dest);

int
dalsa_4ch_piranha_deinterleave(u_char *src, int width, int rows, u_char *dest);

int
dalsa_2ch_deInterleave_16(u_short * src, int width, int rows, u_short * dest);

int
dalsa_2ch_deInterleave_8(u_char * src, int width, int rows, u_char * dest);

int
even_right_deInterleave_16(u_short * src, int width, int rows, u_short * dest);

int
even_right_deInterleave(u_char * src, int width, int rows, u_char * dest);

int 
Inv_Rt_2ch_deInterleave_24_12(u_char * src, int width, int rows, u_short * dest);

int 
deInterleave_24_12(u_char * src, int width, int rows, u_short * dest);

int
deInterleave_1_8_msb7(u_char * src, int width, int rows, u_char * dest);

int
deInterleave_1_8_msb0(u_char * src, int width, int rows, u_char * dest);

#ifdef __cplusplus
}
#endif

#endif
