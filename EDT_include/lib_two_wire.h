

#ifndef _LIB_TWO_WIRE_H
#define _LIB_TWO_WIRE_H


#define EDTFATAL EDTLIB_MSG_FATAL
#define EDTWARN EDTLIB_MSG_WARNING

#ifdef __cplusplus

extern "C" {

#endif


/*
* Library to read a 2 wire protocol device
* usch as a serial eeprom or a diagnostic port on
* a serial trasciever.
*
* the port must be implemented as a register address
* like the PCISS/GS OCM
* bit one is the two wire clock, it is always an output
* bit 2 is the two wire serial data out, it is enable onto the serial data when
* bit 3 enables bit 2 onto the serial data bus when low
* bit 4 reads back the stat of the serial data wire, it should follow bit 2 when bit 3 is on
*
* The ocm has three 2 wire device addresses:
* 1) channel 0 SFP tranciever at 0x23
* 2) channel 1 SFP trancsiver at 0x33
* 3) serial eeprom calibration  - currently unimplemented
*
* some two wire devices have 1 byte address others 2 etc the 
* address_length field designates number of bytes
*
* the page write size varies from device to device setting to 1
* will work on anything but writes will be slow 
*/

typedef struct edt_two_wire {
    u_int	read_address;
    u_int	write_address;
    u_int	sclk_mask;
    u_int 	tri_state_mask;
    u_int	sda_out_mask;
    u_int	sda_in_mask;
    u_int	scl_tri_state_mask;
    u_int	scl_in_mask;
    u_int	options;
    u_int	address_length;
    u_int	page_write_size;
    u_int	expected_xcvr_type;
} EdtTwoWire;

/* options */
#define TWO_WIRE_INVERT_TS	0x1
#define TWO_WIRE_SCL_TS		0x2
#define TWO_WIRE_INVERT_SCL_TS	0x4

/*
* OCM Transceiver Control and serial acess register
*/
#define DISABLE_TX	0x01
#define XCVR_SCL	0x02
#define XCVR_WDATA	0x04
#define XCVR_TS		0x08
#define XCVR_RDATA	0x10
#define XCVR_PRES	0x20
#define XCVR_OPT	0x0

/* two wire interfaces interface id indexs for open */
#define	TWO_WIRE_OCM_SFP_CH0	0
#define TWO_WIRE_OCM_SFP_CH1	1
#define TWO_WIRE_OC192_SFP_CH0	2
#define TWO_WIRE_OC192_XFP_CH1	3

/* two_wire interface options */
/* some i2c can driver the clock and will hold the scl line low until data is ready */
/* the XFP modules do the register for them has a clk tristate as well as clock input */
#define	TWO_WIRE_CLK_STRETCH	0x1

/* oc-192 */
#define	OC192_CH0_TWO_WIRE_COM	OCM_CH0_TRANSCEIVER
#define	OC192_CH1_TWO_WIRE_COM	0x01010031
/* bit definitions */
#define	OC192_WDATA	0x01
#define OC192_TS	0x02
#define	OC192_SCL	0x04
#define	OC192_SCL_TS	0x08
#define OC192_RCLK	0x40
#define OC192_RDATA	0x80

/* 
* edt_open_two_wire is a routine to make the two wire 
* generic. Pass in the EdtDev pointer for the board and
* and define constant for the EDT board, mezzanine and bitfile.
* returns a ponter to a edt_two_wire structure which contains the relavant data
* for further calls.
*/

EDTAPI EdtTwoWire *
edt_open_two_wire(EdtDev *edt_p, int interface_id);

/*
* change the bits in a register 
* the bits set to 1 in the set mask are set after the bits in the clr mask are reset,
* then the new byte is written. There is only on write.
* the byte is read to insure the write has happened
*/
EDTAPI void
edt_two_wire_chg_bit(EdtDev * edt_p, u_int reg_add, u_int clr_mask, u_int set_mask);

/*
* stop and then start the two wire interface
*/

EDTAPI void
edt_two_wire_start(EdtDev * edt_p, EdtTwoWire *tw_ptr);


/*
* the clock routine will cause clock hi. If 
* clock stretching is true the the clock line is tristate and
* waits for the clock to go high
*/

EDTAPI void
edt_two_wire_clock_hi(EdtDev * edt_p, EdtTwoWire *tw_ptr);

/*
* this clock routine will cause clock low.
*/

EDTAPI void
edt_two_wire_clock_low(EdtDev * edt_p, EdtTwoWire *tw_ptr);


/*
* clock in a serial data bit to the eeprom
* clock is low  sda tristate on entry
*/

EDTAPI void
edt_two_wire_wr_databit(EdtDev * edt_p, EdtTwoWire *tw_ptr, int bit);


/*
* stop
* assume clock is low, data can be either
*/
EDTAPI void
edt_two_wire_stop(EdtDev * edt_p, EdtTwoWire *tw_ptr);

/*
* check ack returns TRUE if ack is asserted, FALSE otherwise
* assumes clock is low and data is as last set
*/
EDTAPI int
edt_two_wire_check_ack(EdtDev * edt_p, EdtTwoWire * tw_ptr);

/*
* read byte returns  next 8 bits read from the EEPROM
* assumes clock is low and data is as last set
*/
EDTAPI unsigned char
edt_two_wire_read_8bits(EdtDev * edt_p, EdtTwoWire * tw_ptr);


/*
* write a byte to the two wire interface
*/

EDTAPI void
edt_two_wire_write_8bits(EdtDev * edt_p, EdtTwoWire * tw_ptr, unsigned char byte);


/*
* now the higher level commands
*/

/*
* reset interface
* send up to 9 sclks while reading sda until sda is high
* then enable sda and bring is low to make a start
* return a zero if successful.
*/ 
EDTAPI int
edt_two_wire_reset(EdtDev * edt_p, EdtTwoWire * tw_ptr);


/*
*  read
* requires device address (7 bits)
* address to read from (up to 32 bits)
* length of the address in bytes (1 to 4)
* number of bytes to read (u_int) 
* an char pointer to storage for the bytes read
* returns the length read (0 if fails)
*/ 
EDTAPI u_int
edt_two_wire_read(EdtDev * edt_p, EdtTwoWire * tw_ptr, char device, u_int address, u_int len_data, unsigned char *data);


/*
*  write a single byte
* requires device address (7 bits)
* address to write to (up to 32 bits)
*  a byte to write
* returns  a 0 if successfull
*/ 
EDTAPI u_int
edt_two_wire_write_byte(EdtDev * edt_p, EdtTwoWire * tw_ptr, char device, u_int address, char data);


/* define the two wire transceiever types we can handle */
#define TWO_WIRE_SFP	0x03
#define TWO_WIRE_XFP	0x06
#define TWO_WIRE_SERIAL_ID_DEVICE	0x50

/*
* read the first byte of the two wire device
* this will indicate the transceiver type for subsequent reads
*/

EDTAPI unsigned char
edt_two_wire_read_type(EdtDev * edt_p, EdtTwoWire * tw_ptr);

/*
* read or write a register in a i2c or spi serial device
* the base_desc is the address decriptor for the base of the 4 bytes used to read and write the serial device
* the address is a 15bit address in the serial device or can be decoded into multiple devices by the FPGA
* the value is the 8 bit data
*/
/*
* the hardware implements 4 bytes with these offsets from the base_desc
* offset 0 includes 7 bits of extended address for larger serial devices or to decode multiple devices
* bit 7 of offset 0 reads back the transfer busy bit indicating an operation is already in progress
* writing the lower 8 bits of address to offset 1 initiates a read, the data is read from offset 1
* as well after the xfer busy is clear
* offset 2 initiates a write of the data in offset 3 to the address written in od\ffset 2
*/

EDTAPI void
edt_serial_dev_reg_write(EdtDev * edt_p, 
			 uint_t base_desc,
			 uint_t device_id,
			 uint_t address, 
			 uint_t value);

EDTAPI uint_t
edt_serial_dev_reg_read(EdtDev * edt_p, 
			uint_t base_desc, 
			uint_t device_id,
			uint_t address);

EDTAPI uint_t
edt_serial_dev_reg_read_block(EdtDev *edt_p,
                        uint_t base_desc, 
                        uint_t device_id,
                        uint_t address,
                        uint_t length,
                        u_char *block) ;
EDTAPI uint_t
edt_serial_dev_reg_read_block(EdtDev *edt_p,
                        uint_t base_desc, 
                        uint_t device_id,
                        uint_t address,
                        uint_t length,
                        u_char *block) ;
EDTAPI u_int
edt_serial_dev_set_bits(EdtDev *edt_p, u_int base_desc, u_int device, int address, int value, int shift, int mask);

/* Register decriptor code  - to automate setting up a two-wire device */

/* Used to store values for setting on 2-wire interfaces */

typedef struct {
    u_short offset;
    u_char value;
    u_char read_only;
    u_char modified_mask;
    u_char valid;
} Edt2WireRegArray;

/* Basic descriptor for registers */
/* This code will also work for standard edt registers as well */

typedef struct _regmap {
    u_int reg;
    u_int bitstart;
    u_int nbits;
    char *name;
    int offset;
} EdtRegisterDescriptor;

/* Based on a descriptor pointer, get a register value from an 8-bit interface */

EDTAPI u_int edt_get_two_wire_value(EdtDev *edt_p, 
				    EdtRegisterDescriptor *map, 
				    u_int base_desc, 
				    u_int device);

/* Based on a desedt_serial_criptor pointer, set a register value from an 8-bit interface */

EDTAPI int edt_set_two_wire_value(EdtDev *edt_p, EdtRegisterDescriptor *map, u_int base_desc, u_int device, u_int value);

EDTAPI int edt_two_wire_reg_dump_raw(EdtDev *edt_p, 
		      EdtRegisterDescriptor *map, 
		      u_int base_desc, 
		      u_int device);

EDTAPI int edt_two_wire_reg_dump(EdtDev *edt_p, 
		      EdtRegisterDescriptor *map, 
		      u_int base_desc, 
		      u_int device);

#ifdef __cplusplus

}

#endif

#endif
