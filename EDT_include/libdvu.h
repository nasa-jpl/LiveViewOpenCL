#ifndef INCLUDE_libdvu_h
#define INCLUDE_libdvu_h

/*
 * EDT digital imaging utilities library header file
 *
 * Copyright (c) 1997, Engineering Design Team, Inc.
 *
 */

typedef struct {
    u_char b ;
    u_char g ;
    u_char r ;
} bgrpoint ;

/*
 * struct for sub images 
 */
typedef struct {
    long left ;
    long top ;
    long right ;
    long bottom ;
    long xdim ;
    long ydim ;
    int  depth ;
    u_char *data ;
    u_char **mat ;
    u_short **mat16 ;
    bgrpoint **mat24 ;
} dvu_window ;


EDTAPI dvu_window *dvu_init_window(u_char *data, int sx, int sy, int dx, int dy,
				int stride, int rows, int depth) ;
EDTAPI dvu_window *dvu_read_window(char *filename) ;
EDTAPI dvu_window *dvu_reset_window(dvu_window *s, u_char *data, int sx, int sy, int dx, int dy) ;
EDTAPI int	   dvu_free_window(dvu_window *w) ;
EDTAPI int	   dvu_load_lookup(char *filename,int depth) ;
EDTAPI int	   dvu_save_lookup(char *filename,int depth) ;
EDTAPI int	   dvu_lookup(u_char *src, u_char *dst,int size,int depth) ;
EDTAPI int	   dvu_exp_histeq(u_char *src, u_char *dst,int size,int depth,int cutoff) ;
EDTAPI int	   dvu_write_rasfile(char *fname, u_char *addr, int x_size, int y_size);
EDTAPI int	   dvu_write_image(char *fname, u_char * addr, int x_size, int y_size, int istride );
EDTAPI int	   dvu_write_rasfile24(char *fname, u_char *addr, int x_size, int y_size);
EDTAPI int	   dvu_write_rasfile16(char *fname, u_char *addr, int x_size, int y_size,
								   int depth);
EDTAPI int	   dvu_write_image24(char *fname, u_char *addr, int x_size, int y_size, int istride );
EDTAPI int	   dvu_histeq(u_char *src, u_char *dst, int size, int depth);
EDTAPI int	   dvu_exp_histeq(u_char *src, u_char *dst, int size, int depth, int cutoff);
EDTAPI int    dvu_winscale(dvu_window *wi, dvu_window *bi, int minbyte, int maxbyte, int doinit)  ;

EDTAPI int	   mysleep(int val);
EDTAPI int	   ten2one(u_short *wbuf, u_char *bbuf, int count);
EDTAPI int	   dvu_word2byte(u_short *wbuf, u_char *bbuf, int count, int shift );
EDTAPI void	dvu_free_tables(void);
EDTAPI int dvu_write_raw(int imagesize, u_char *imagebuf, char *fname) ;
EDTAPI int dvu_write_bmp(char *fname, u_char *buffer,int w,int h) ;
EDTAPI int dvu_write_bmp_24(char *fname, u_char *buffer,int w,int h) ;
EDTAPI void dvu_long_to_charbuf(u_int val, u_char *buf);

#endif /* INCLUDE_dvulib_h */

