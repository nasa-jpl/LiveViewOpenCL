/**

*/

#ifndef _PDV_RECPLAY_H_
#define _PDV_RECPLAY_H_

#include "edtinc.h"

#include "edt_threads.h"

#define FILETYPE_RAW 1
#define FILETYPE_TIFF 2

#define DEFAULT_FILENAME "pdvrecord"


/* record/playback code */

/* This should be C++ but for now we use C equivalent */

/* We can make custom EdtRecPlayIO code for different file types */

typedef struct _EdtRecPlayIO EdtRecPlayIO;

struct _EdtRecPlayIO {
    /* "member" functions */

    int (*initialize)      (EdtRecPlayIO *io, char *basename, int direction, int imagesize);
    int (*read_next_image) (EdtRecPlayIO *io, int index, u_char *image_p,  
				int *width, int *height, int *depth,
				int *imagesize);
    int (*write_next_image)(EdtRecPlayIO *io, int index, u_char *data, int width, int height, int depth,
	    int imagesize);
    int (*close)           (EdtRecPlayIO * io);
    uint64_t (*get_size)	   (EdtRecPlayIO * io);

    HANDLE f;

    char basename[512];
    char bmpfname[512];

    u_char filetype;
    int direction;

    /* Note other versions can include this as first part of structure  to extend it */

};


typedef struct _RecPlay {
    PdvDev *pdv_p;

    EdtRecPlayIO *io;
    u_char inmemory;

    int loops;
    int width;
    int height;
    int depth;
    int nimages;
    
    int imagesize;    
    int dmasize; /* dmasize may be different if padding is used for playback */

    uint64_t totalbytes;
    char *cameratype;
    
    int started;
    int numbufs;
    
    double capturetime;
    double savetime;
    
    int direction;
    int constantsize;

    u_char **membuffers; /* internal storage if inmemory is set */

    int images_read;     
    int images_written;

    
    int dataheight; /* actual amount of data 
		    (may be less than image size on playback) */

    int datastart; /* index into playback buffer to put file data */


} EdtRecPlay; 

/* Message logger masks */

#define MINVRB	 1    /* print only the minimum */
#define CHECKDMA 2
#define CHECKIMG 4
#define CHECK_FV 8
#define CHECK_EVENT 16
#define CHECK_IMG_READ 32
#define MAXVRB	 0x80 /* print everything */


/* Symmetrical (almost) functions for recording and playback */
/* 
Open the record/playback structure. direction should be EDT_READ for record,
EDT_WRITE for playback
iofactory lets you replace default io type of raw
*/

EDTAPI EdtRecPlay * edt_record_playback_open(EdtRecPlay *rpb, char *devname, int unit, int channel, 
					     int direction, EdtRecPlayIO *(*iofactory)());


/* Shut things down */

EDTAPI int edt_record_playback_close(EdtRecPlay *rpb);



/* 
Initialize recording for nimages. If inmem is true, allocate enough memory 
   to hold the captured images for later writing to file. nimages must be defined. 
   */

EDTAPI int edt_record_init(EdtRecPlay *rpb, char *filename, int inmem, int nimages);
			  


/* Capture images */

EDTAPI int edt_record(EdtRecPlay *rpb);


/* 
Initialize playback for nimages. If inmem is true, allocate enough memory 
   to hold the file images for later playback. Loops will determine how many times to
   go through the file - 0 for forever.

   */

EDTAPI int edt_playback_init(EdtRecPlay *rpb, char *filename, int inmem, int loops);

/* Play back images */

EDTAPI int edt_playback(EdtRecPlay *rpb);


#endif
