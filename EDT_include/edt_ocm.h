/*

 * register definition for OCM interface
 * Xilinx bitfile.
 */

#ifndef _EDT_OCM_H
#define _EDT_OCM_H



/*
 * OCM definitions.
 */
/*
  * OCM specific registers -
  * channel 0 registers are 0x20 - 0x24 have the same definitions as
  * channel 1 registers 0x30-0x34 except the channel does not have DDR memory fifo
  * so not bit to reset same.
  */

/* -- reg_group OCM */

#define	OCM_CH0_CONFIGL	0x01010020
#define	OCM_CH0_CONFIGH	0x01010021
#define	OCM_CH0_STATUS	0x01010022
#define	OCM_CH0_XCVR	0x01010023
#define	OCM_CH0_ENABLE	0x01010024

#define  OCM_CH0_CONFIG0            0x01010020
#define  OCM_CH0_CONFIG1            0x01010021
#define  OCM_CH0_STATUS             0x01010022
#define  OCM_CH0_TRANSCEIVER        0x01010023
#define  OCM_CH0_ENABLE             0x01010024

#define  OCM_CH1_CONFIG0            0x01010030
#define  OCM_CH1_CONFIG1            0x01010031
#define  OCM_CH1_STATUS             0x01010032
#define  OCM_CH1_TRANSCEIVER        0x01010033
#define  OCM_CH1_ENABLE             0x01010034

#define  OCM_FPGA0_LOAD             0x01010040
#define  OCM_FPGA1_LOAD             0x01010041
#define  OCM_FPGA2_LOAD             0x01010042
#define  OCM_FPGA3_LOAD             0x01010043

#define  OCM_CH0_RCV_FRAMING        0x01010080
#define  OCM_CH0_XMT_FRAMING        0x01010081
#define  OCM_CH0_XMT_NATIONAL       0x01010082
#define  OCM_CH0_RCV_FILTER         0x01010083
#define  OCM_CH0_XMT_TEST_DATA      0x04010084
#define  OCM_CH0_RCV_STATUS         0x01010094
#define  OCM_CH0_RCV_FRAME_STATUS   0x01010095
#define  OCM_CH0_DEMUX_BITMAP       0x01010097
#define  OCM_CH0_DEMUX_BITMAP_READ  0x01010098
#define  OCM_CH0_TX_STATUS          0x01010099
#define  OCM_CH0_BITFILE_VER        0x010100A0

#define  OCM_CH1_RCV_FRAMING        0x010100C0
#define  OCM_CH1_XMT_FRAMING        0x010100C1
#define  OCM_CH1_XMT_NATIONAL       0x010100C2
#define  OCM_CH1_RCV_FILTER         0x010100C3
#define  OCM_CH1_XMT_TEST_DATA      0x040100C4
#define  OCM_CH1_RCV_STATUS         0x010100D4
#define  OCM_CH1_RCV_FRAME_STATUS   0x010100D5
#define  OCM_CH1_DEMUX_BITMAP       0x010100D7
#define  OCM_CH1_DEMUX_BITMAP_READ  0x010100D8
#define  OCM_CH1_TX_STATUS          0x010100D9
#define  OCM_CH1_BITFILE_VER        0x010100E0


#define OCM_REG_CH0_MASK 0x01010097
#define OCM_REG_CH0_MASK_READBACK 0x01010098

/* Framing error registers */

#define OCM_CH0_B1_ERROR_CNT	0x03010088
#define OCM_CH0_B1_ERROR_MASK	0x0101008b
#define OCM_CH1_B1_ERROR_CNT	0x030100c8
#define OCM_CH1_B1_ERROR_MASK	0x010100cb

#define OCM_CH0_B2_ERROR_CNT	0x0401008c
#define OCM_CH1_B2_ERROR_CNT	0x040100cc

#define OCM_CH0_M1_ERROR_CNT	0x03010090
#define OCM_CH1_M2_ERROR_CNT	0x030100d0

#define OCM_CH0_CNT_CTRL	0x01010093
#define OCM_CH1_CNT_CTRL	0x010100D3

#define OCM_CH0_LOF_CNT		0x0201009C
#define OCM_CH1_LOF_CNT		0x020100DC

#define OCM_CH0_FRM_PAT_CNT	0x0201009E
#define OCM_CH1_FRM_PAT_CNT	0x020100DE

#define OCM_CH0_FALSE_FRM_CNT	0x020100A4
#define OCM_CH1_FALSE_FRM_CNT	0x020100E4

/* -- */

/* synonyms */

#define  OCM_RX_CTRL	0x01010080 
#define  OCM_TX_CONFIG	0x01010081 

#define OCM_NAT_BYTE	0x01010082 
#define OCM_RX_FILTER	0x01010083
#define TEST_DATA_REG	0x03010084 
#define OCM_RX_STATUS	0x01010094 
#define FRAME_STATUS	0x01010095 

/*
 * registers 0 (PCD_CMD), 1 (PCD_DATA_PATH_STAT), 2 (OCD_FUNCT), 3 (PCD_STAT),  0xf (PCD_CONFIG),
 * 0x10 (SSD16_CHEN), 0x16 (SSD16_LSB), 0x18 (SSD16_UNDER) and 0x1b (SSD16_OVER) have
 * the regular address definition with the standard bit placement and definition as follows:
 * command - PCD_ENABLE bit 0x8 only, others are read/write but unused
 * data_path - bits are read/write but not used
 * funct - bits are read write but unused
 * status - bit 0 indicates the sysclk DCM on the Xilinx is locked to the clock from the OCM board
 */

/* --reg_bits PCD_STAT */

#define	LOCAL_SYS_LOCK		0x1

 /*
  * config - bit 0 is byteswap (PCD_BYTESWAP) and 3 is short swap (PCD_SHORTSWAP)
  * channel enable - bit 0 and 1 enable channel 0 and 1
  * lsb_first - bit 0 and 1 control channel 0 and 1
  * underflow - bit 0 and 1 report status, bit 2 to 15 are always 0.
  * overflow - bit 0 and 1 report status, bit 2 to 15 are always 0.
  */

 
/* channel enable bits in SSD16_CHEN */

/* --reg_bits SSD16_CHEN */

#define OCM_CH0_RX_ENABLE 0x1
#define OCM_CH1_RX_ENABLE 0x2
#define OCM_CH0_TX_ENABLE 0x4
#define OCM_CH1_TX_ENABLE 0x8

/* -- */

/* 
 * config register low bits
 * these bits directly control the SLK2511
 * the FRAME_EN bit also controls the framesync state machine for the channel
 */

#define OFFSET_CH1_BASE	0x10
#define OFFSET_CH1_MEZ	0x40

/* --reg_bits OCM_CH0_CONFIG0 */
/* --reg_bits OCM_CH1_CONFIG0 */
#define OCM_FRAME_EN	0x01
#define LOCK_REF	0x02
#define RX_SEL_MSK	0x0c
#define AUTO_DETECT	0x10
#define REMOTE_LOOP	0x20
#define LOCAL_LOOP	0x40
#define PRBS_EN		0x80

/* -- reg_enum OCM_RSEL_MSK */

#define OC48_STM16	0x00
#define OC24		0x04
#define OC12_STM4	0x08
#define OC3_STM1	0x0c

/*
 * config high bits
 */

/* --reg_bits OCM_CH0_CONFIG1 */
/* --reg_bits OCM_CH1_CONFIG1 */


#define	OCM_MODE_MSK	0x3
#define	OCM_PRE_MSK		0xc
#define	OCM_LOOPTIME	0x10

/* -- reg_enum OCM_MODE_MSK */

#define	OCM_PRE_DIS		0x0
#define	OCM_PRE_10		0x4
#define	OCM_PRE_20		0x8
#define	OCM_PRE_30		0xc

/* -- reg_enum OCM_PRE_MSK */

#define	OCM_MODE_FULL	0x0
#define	OCM_MODE_TX		0x1
#define	OCM_MODE_RX		0x2
#define	OCM_MODE_REP	0x3

/*
 * SLK2511 LIU  Status
 */
/* --reg_bits OCM_CH0_STATUS */
/* --reg_bits OCM_CH1_STATUS */

#define SPILL2511	0x01
#define PRBSPASS	0x02
#define RATE_DET_MSK	0x0c
/* bit defines for rates are the same as the config register above */
#define LOS		0x10
#define LOL		0x20
#define SIG_DET		0x80

/*
 * Transceiver Control and serial acess register
 */
/* --reg_bits OCM_CH0_XCVR */
/* --reg_bits OCM_CH1_XCVR */

#define DISABLE_TX	0x01
#define XCVR_SCL	0x02
#define XCVR_WDATA	0x04
#define XCVR_TS		0x08
#define XCVR_RDATA	0x10
#define XCVR_PRES	0x20
#define XCVR_FLT	0x40
/*
 * Enable Registers
 */
/* --reg_bits OCM_CH0_ENABLE */
/* --reg_bits OCM_CH1_ENABLE */

#define SLK_EN		0x01
#define PLL_EN		0x02
#define SYS_EN		0x04
#define RAM_EN		0x08
/* read status in top bits */
#define SYS_LOCK	0x20
#define RX_LOCK		0x40

/* -- */


/*
 * Receive Framer control register
 */


/* --reg_bits OCM_CH0_RCV_FRAMING */
/* --reg_bits OCM_CH1_RCV_FRAMING */

#define RESET_FRM	0x01
#define FRAME_EN	0x02
#define RX_DATA_SRC_2G	0x04
#define DISABLE_SCRAM	0x10
#define SUSPEND_AQ	0x20
#define EN_PAR_CNT	0x40
#define EN_ERR_CNT	0x80
/*
 * Configure Transmit Framer
 */

/* --reg_bits OCM_CH0_XMT_FRAMING */
/* --reg_bits OCM_CH1_XMT_FRAMING */

#define RESET_PTR	0x01
#define TEST_DATA	0x02
#define EN_TX_FRAME	0x04
#define EN_IDLE		0x04
#define CLOCK_SEL_MSK	0xC0
#define	CLK_125MHZ	0x00
#define	CLOCK_SEL_MSK	0xC0
#define	CLK_SONET_SDH	0x40
#define	CLK_GE		0x80
#define	CLK_FEC		0xC0


/* --reg_bits OCM_CH0_RCV_FILTER */
/* --reg_bits OCM_CH1_RCV_FILTER */

/* for sdh bitfiles */

#define OVERHEAD_ONLY	0x01
/* enable counter data substitution */
/* for ethernet bitfiles only right now*/
#define	EN_DATA_COUNTER 0x02
#define	COUNT_FREERUN	0x04
#define EN_ALL_DECODED	0x08

/* added for OTU1 and OTU2 jsc 10/10/11 */

#define ODU_MASKED_PLUS_OVHD    0x02
#define ODU_NO_FEC      0x04

/* when this is set the mezz ignores other filter bits */
#define ODU_PASS_THRU  0x80

/* -- */



/* --reg_bits OCM_CH0_XMT_TEST_DATA */
/* --reg_bits OCM_CH1_XMT_TEST_DATA */

#define FRAMED		0x01
#define RX_LOCKED	0x01
#define RATE12		0x02
/*
 * Transmit test pattern 32 bit
 */

/* --reg_bits OCM_CH0_RCV_FRAME_STATUS */

#define BIT_SYNC	0x01
#define BYTE_SYNC	0x02
#define MATCH_CNT_MSK	0x0c
#define MATCH_CNT_SHFT	2
#define DROP_CNT_MSK	0x30
#define DROP_CNT_SHFT	4
#define FOUND		0x40
#define LOCKED		0x80

/* -- */


/* Bit masks for OCM_CHX_CNT_CTRL */

/* --reg_bits OCM_CH0_CNT_CTRL */
/* --reg_bits OCM_CH1_CNT_CTRL */


#define OCM_EN_ERROR_COUNT	0x80
#define OCM_ERROR_COUNT_HOLD	0x1

/* new defines for edt_ocm.h */
/* OCM_CH0_CONFIG1 */
#define EN_PRBSPASS_LATCH	0x80


/* prbs7 biterror counters in FPGA */
#define OCM_CH0_PRBS_ERR_CNTRL	0x010100ac
#define OCM_CH1_PRBS_ERR_CNTRL	0x010100ec

/* --reg_bits OCM_CH0_PRBS_ERR_CNTRL */
/* --reg_bits OCM_CH1_PRBS_ERR_CNTRL */

#define CLR_PRBS_CNT	0x01

#define OCM_CH0_PRBS_STATUS	0x010100ad
#define OCM_CH1_PRBS_STATUS	0x010100ed

/* --reg_bits OCM_CH0_PRBS_STATUS */
/* --reg_bits OCM_CH1_PRBS_STATUS */

#define PRBS_SYNCED	0x01
#define PRBS_ERROR	0x02
#define PRBS_ERR_LATCH	0x04

/* -- */

#define OCM_CH0_PRBS_ERR_CNT	0x010100ae
#define OCM_CH1_PRBS_ERR_CNT	0x010100ee
#define OCM_CH0_PRBS_LOS_CNT	0x010100af
#define OCM_CH1_PRBS_LOS_CNT	0x010100ef

/* frequency counter */
#define OCM_CH0_FREQ_CNT_CTRL	0x010100b0
#define OCM_CH1_FREQ_CNT_CTRL	0x010100f0

/* --reg_bits OCM_CH0_FREQ_CNT_CTRL */
/* --reg_bits OCM_CH1_FREQ_CNT_CTRL */

#define CLEAR_VALID	0x01

#define	OCM_CH0_FREQ_STATUS	0x010100b1
#define	OCM_CH1_FREQ_STATUS	0x010100f1

/* --reg_bits OCM_CH0_FREQ_STATUS */
/* --reg_bits OCM_CH1_FREQ_STATUS */

#define FREQ_VALID	0x01

/* -- */

#define OCM_CH0_FREQUENCY	0x020100b2
#define OCM_CH1_FREQUENCY	0x020100f2

/* receive clock phase control */
#define	OCM_CH0_RX_CLK_CTRL	0x0101009e
#define	OCM_CH1_RX_CLK_CTRL	0x010100de

/* --reg_bits OCM_CH0_RX_CLK_CTRL */
/* --reg_bits OCM_CH1_RX_CLK_CTRL */

/* the first 4 bits select what is read back in the OCM_CHX_RX_CLK_STAT reg */
#define	STAT_SEL_MSK		0x0f
#define SIG_STAT		0x0
/* resd the phase positions at edges and current */
#define CLK_POS_START		0x4
#define	CLK_NEG_EDGE		0x5
#define CLK_NXT_POS		0x6
#define CLK_FINAL		0x7
#define SHADOW_CNT		0x8
/* phase select control bits */
#define PRG_PS_EN		0x10
#define PRG_PS_INC		0x20

#define OCM_CH0_RX_CLK_STAT	0x0101009f
#define OCM_CH1_RX_CLK_STAT	0x010100df

/* --reg_bits OCM_CH0_RX_CLK_STAT */
/* --reg_bits OCM_CH1_RX_CLK_STAT */

/* when the OCM_CHX_RX_CTRL STAT_SEL is set to SIG_STAT */
#define	CLK_F			0x01
#define	CLK_R			0x02

/* next two bits store the state of the auto phase adjust */
#define DONE_STATE_MSK		0x0c

/* last 5 bits (one unused) */

#define	STATE_DONE_EN_1		0x08
#define	DCM_LOCK_SET		0x10
#define NO_RXCLK		0x40
#define PHASE_OVF		0x80

/* -- reg_enum DONE_STATE_MSK */

#define	STATE_AUTO		0x00
#define STATE_DONE		0x04
#define	STATE_PRG_WAIT		0x0c

/* -- */

/*
 * additional bits for OCM_CHX_RCV_STATUS for ethernet
 * 0 - RX PLL LOCKED
 * 1 - SLK2511 set to OC12/STM4 Rate or ethernet (should be on for ethernet)
 */
#define	CATCH_FIFO_OVF	0x4
#define	CATCH_ALIGN_CHG	0x8
/* following errors when either bit or both are set */
#define	CATCH_CODE_VIOL	0x30
#define	CATCH_CODE_ERR	0xc0
/*
 * ethernet bits for OCM_CHX_RCV_FRAME_STATUS
 * reflect the state of the 10 bit alignment to 8B/10B commas
 */
#define	COMMA_ALIGNED	0x01
/* set when the catch registers are active for errors above */
#define	CATCH_ENABLED	0x02
/* two bits are unused */
#define	ALIGNMENT_MSK	0xf0

/*
 * program the Xilinx for channel 0  and 1 registers
 * the data register is loaded with the bit file data. It is a fifo which holds 15 bytes
 * the number of bytes in the fifo is read in the top 4 bist of the status register
 * The CONT register controls the PROGL and INIT pins of each Xilinx, bit 4 controls which
 * xilinx gets the data being loaded in the data register
 * The STAT register reflects the status of the DONE and INIT pins.
 * The constant register is written and read to determine if the ocm.bit file is loaded before
 * attempting to load the individual channel xilinx.
 */

#define OCM_X_DATA        0x01010040
#define OCM_X_CONT        0x01010041
#define OCM_X_STAT        0x01010042
#define OCM_X_CONST       0x01010043

/* control register bits */
#define	OCM_CONT_CH0_INIT    0x1  /* one tristates INIT pin */
#define	OCM_CONT_CH0_PROG    0x2  /* one drives the PROG_L pin low (resets xilinx program) */
#define	OCM_CONT_CH1_INIT    0x4
#define	OCM_CONT_CH1_PROG    0x8
#define	OCM_CONT_PRG_CH1     0x10  /* directs data to channel 1 */
#define	OCM_CONT_ENABLE      0x20  /* enables the fifo, cclk and bit serializer */
#define	OCM_CONT_EN_FIFO     0x40  /* enables the fifo, cclk and bit serializer */

/* status bits  */
#define	OCM_STAT_CH0_INIT	0x1	/* reads state of INIT pin */
#define	OCM_STAT_CH0_DONE	0x2	/* reads state of DONE pin */
#define	OCM_STAT_CH1_INIT	0x4	
#define	OCM_STAT_CH1_DONE	0x8	
#define	OCM_FCNT_MSK		0xf0
#define	OCM_FCNT_SHFT		4

#define	OCM_CONSTANT		0x0c	/* fixed pattern in OCM_X_CONST register */


#endif
