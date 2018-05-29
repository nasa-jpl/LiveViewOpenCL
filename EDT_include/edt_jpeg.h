#ifndef _EDT_JPEG_H_
#define _EDT_JPEG_H_

#include "edtinc.h"

#ifdef __cplusplus

extern "C" {

#endif

#define XMD_H // avoid typedef FAR INT32 redefinition FATAL error in jmorecfg.h
#include <jpeglib.h>

EDTAPI int edt_save_jpeg(const char *szFilename, int width, int height,
                  int depth, int qvalue, unsigned char* buf);

#ifdef __cplusplus

}

#endif

#endif /* _EDT_JPEG_H_ */
