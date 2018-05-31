
/* Routines and structures to allow low-latency DMA in chunks < buffersize */

#ifndef EDT_LLDMA_H
#define EDT_LLDMA_H

#ifdef __cplusplus
extern "C" {
#endif


typedef double DmaTimeStamp;


/* Flags for edt_dma_configure */

/* Use existing configure ring buffers whatever their size; otherwise setup automatically*/
/* bit 0 is used for EDT_WRITE */

/* Make buffersize= bytespersec/sample_us,
    wait_for_buffers always called */
#define EDT_FORCE_BUFSIZE 0x2 

/* Force use of usleep - calculate available interval */
#define EDT_USE_USLEEP 0x4

/* force use of sampling not wait - for variable size data */
#define EDT_FORCE_SAMPLE 0x8 



/* Modes for edt_p->wait_mode */

#define EDT_DMA_WAIT	1
#define EDT_DMA_SLEEP	2
#define EDT_DMA_SPIN	3

/******************************/
/* DMA routines */
/******************************/

EDTAPI int  edt_dma_configure(EdtDev *edt_p, 
				u_int bytespersec,  /* expected data rate */
				u_int sample_us,    /* desired interval samples */
				u_int buffertime,   /* total amount of time to buffer in ms*/
				u_int fixed_buffer_size,  /* data buffer size desired,
							or zero if = sample_us*/
				u_int flags);	    /* see above for flags */

EDTAPI void edt_dma_start(EdtDev *edt_p, int seconds);

/* Equivalent to wait function, except blocks gets filled in every time
   and return starts at end of last chunk always */

EDTAPI u_char * edt_dma_get_sample(EdtDev *edt_p, u_int *datasize, DmaTimeStamp * stamp);


/* returns actual min usleep time as seconds */
EDTAPI double edt_min_usleep_time();


EDTAPI int   edt_setup_channel_dma(EdtDev *edt_p, 
				   u_int bytespersec, 
				   u_int sample_usecs, 
				   u_int total_usecs, 
				   u_int fixed_buffer_size,
   				   u_int flags);

EDTAPI int edt_close_channel_dma(EdtDev *edt_p);

EDTAPI int  edt_start_dma(EdtDev *edt_p, int seconds);

EDTAPI void * edt_get_next_sample(EdtDev *edt_p, DmaTimeStamp *stamp);

EDTAPI void edt_stop_dma(EdtDev *edt_p);

/* wrap separate capture thread */

typedef int (*EdtDmaCallback) (EdtDev *edt_p, void *data, DmaTimeStamp stamp) ;
#ifdef __APPLE__
EDTAPI mac_thread_t edt_launch_dma_thread(EdtDev *edt_p, EdtDmaCallback cb, int *stop_flag);

EDTAPI void stop_dma_thread(mac_thread_t thread, int *stop_flag);
#else
EDTAPI thread_t edt_launch_dma_thread(EdtDev *edt_p, EdtDmaCallback cb, int *stop_flag);

EDTAPI void stop_dma_thread(thread_t thread, int *stop_flag);
#endif

EDTAPI int edt_get_reserved_pages(EdtDev *edt_p);

EDTAPI int edt_get_reserved_mem(EdtDev *edt_p);

EDTAPI unsigned int edt_ring_buffer_pages_used(int n, int size);

EDTAPI unsigned int
edt_check_ring_buffers_reserved(EdtDev *edt_p, int n, int size) ;



#ifdef __cplusplus
}

#endif

#endif
