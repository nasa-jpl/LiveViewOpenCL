/* #pragma ident "@(#)libedt_timing.h	1.20 10/26/10 EDT" */

/* Copyright (c) 2007 by Engineering Design Team, Inc. */

#ifndef INCLUDE_libedt_timing_h
#define INCLUDE_libedt_timing_h


/*
* Timing register offsets:
* 	Time Dist. Board:	0x60 (default)
* 	MSDV:			0xa4
* 	Net10G:			0xac
* 	SRXL-2:			0x5c
* 	CLINK:			0x31
*/
#define TIMECODE_BASE_REGISTER		0x60
#define TIMECODE_MSDV_BASE_REGISTER	0xa4
#define TIMECODE_NET10G_BASE_REGISTER	0xac
#define TIMECODE_SRXL2_BASE_REGISTER	0x5c
#define TIMECODE_CLINK_BASE_REGISTER	0xb1
#define PDV_IRIG_SPI_BASE 0xb1

typedef	struct _ts_raw_t {	/* Raw timecode format */
    u_int seconds:6;
    u_int minutes:6;
    u_int hours:5;
    u_int days:9;
    u_int years:6;
} ts_raw_t;


/* SPI register definitions */
#define SPI_DATA	0x01010060
#define SPI_STAT_CTRL	0x01010061
#define SPI_STROBE	0x01010062


/* SPI STATUS register bit definitions */
#define SPI_MASTER	0x80
#define FIFO_OUT_EMPTY	0x08
#define FIFO_OUT_FULL	0x04
#define FIFO_IN_EMPTY	0x02
#define FIFO_IN_OV	0x01
#define FIFO_RESET	0x01

/* SPI input datapath. */
#define ACK			0x90
#define NAK			0xA0
/* Packet based commands: */
#define SPI_PKT_START		0x82	/* Start of packet byte. */
#define SPI_PACKET_LOOP		0	/* Send packet to msp430 which loops it back to FPGA host.  No ACK/NAK response. */
#define SPI_TIMECODE_EN		1	/* Enable (1) or disable (0) 1 Hz timestamp to FPGA host.  ACK/NAK response. */
#define SPI_CLK_SELECT		2	/* Select internal (0) or external (1) msp430 clock source and uint32 clock rate LS byte first.  ACK/NAK response. */
#define SPI_TIMECODE_PKT	3	/* 4-8 bytes of data containing timecode LSByte first. */
#define SPI_TIMECODE_RAW 	4	/* 4 bytes of data containing raw irig timecode */
#define SPI_TIMECODE_PKT_RAW 	5	/* 4 bytes of data containing raw irig timecode */
#define SPI_TIMECODE_ADD_SECONDS    6   /* Number of seconds to add to timestamp to offset datapath delays */
#define SPI_SET_DIAGNOSTIC_MODE	7	/* Enable, disable, and configure diagnostic mode. */
#define SPI_READ_DIAGNOSTIC_PKT	8	/* Assemble and return diagnostic packet based on diagnostic mode. */
#define SPI_ENABLE_PROGRAMMED_YEAR	9	/* Use argument for the year instead of the CONTROL FIELD in the IRIG-B signal */
#define SPI_DISABLE_PROGRAMMED_YEAR	10	/* Use the CONTROL FIELD in the IRIG-B signal for the year */



/**
 * @addtogroup timecode_configuration
 * @{
 */
EDTAPI EdtDev * edt_spi_open(char *edt_interface, int unit, u_int spi_reg_base);  /* edt_interface is "pcd" or "pdv" or "pe53b", etc. */
EDTAPI int    edt_spi_close(EdtDev *edt_p);
EDTAPI int    edt_set_timecode_enable(EdtDev *edt_p, int enable);
EDTAPI void   edt_set_msp430_clock(EdtDev *edt_p, int clock_sel, int clock_hz);
EDTAPI void   edt_set_timecode_raw(EdtDev *edt_p, int enable);
EDTAPI void   edt_set_timecode_seconds_offset(EdtDev *edt_p, u_int seconds);
EDTAPI void   edt_enable_timecode_programmable_year(EdtDev *edt_p, u_short year);
EDTAPI void   edt_disable_timecode_programmable_year(EdtDev *edt_p);

/** @} */ /* end timecode_configuration group */

/**
 * @addtogroup timecode_display
 * @{
 */
EDTAPI u_char edt_spi_get_stat(EdtDev *edt_p);
EDTAPI void   edt_spi_display_time(EdtDev *edt_p, int loops);

/** @} */ /* end timecode_display group */

/**
 * @addtogroup timecode_update
 * @{
 */
EDTAPI int    edt_get_timecode_version(EdtDev *edt_p);
EDTAPI int    edt_spi_invoke_flash_loader(EdtDev *edt_p);
/** @} */ /* end timecode_update group */

/**
 * @addtogroup timecode_utility
 * @{
 */
EDTAPI u_char edt_spi_get_byte(EdtDev *edt_p);
EDTAPI u_char edt_spi_get_byte_nointr(EdtDev *edt_p);
EDTAPI u_char edt_spi_put_byte(EdtDev *edt_p, u_char ch);
EDTAPI void   edt_spi_flush_fifo(EdtDev *edt_p);
EDTAPI char  *edt_spi_putstr(EdtDev *edt_p, char *str);
EDTAPI u_int  edt_spi_reg_read(EdtDev *edt_p, u_int desc);
EDTAPI void   edt_spi_reg_write(EdtDev *edt_p, u_int desc, u_int val);
EDTAPI u_char edt_crc16_lowbyte(u_char *buf, int len);
EDTAPI u_char edt_crc16_highbyte(u_char *buf, int len);
EDTAPI void   edt_spi_send_packet(EdtDev *edt_p, u_char *cmdbuf);
/** @} */ /* end timecode_utility group */


#endif /* INCLUDE_libedt_h */
