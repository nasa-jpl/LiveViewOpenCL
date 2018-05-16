/**=================================================================================================
 * @file    edt_devid.h
 *
 * @brief   Defines constants for the device id defined in the PCI/PCIe firmware
 *          and for the stored mezzanine id values for mezzanine cards.
 *
 *          Extracted from edtreg.h 2/1/2012 for clarity
 *===============================================================================================**/

#ifndef _EDT_DEV_ID_H_
#define _EDT_DEV_ID_H_

/***********************************
 * Device ids
 * This is stored in the devid member of EdtDev
 ***********************************/

#define P11W_ID                 0x0000
#define P16D_ID                 0x0001
#define PDV_ID                  0x0010
#define PCD20_ID                0x0011
#define PCD40_ID                0x0012
#define PCD60_ID                0x0013
#define PDVK_ID                 0x0014
#define PDV44_ID                0x0015
#define PDVAERO_ID              0x0016
#define PDVCL_ID                0x0034

#define PGP20_ID                0x0021
#define PGP40_ID                0x0022
#define PGP60_ID                0x0023
#define PDVRGB_ID               0x0024
#define PGP_RGB_ID              0x0024
#define PGP_THARAS_ID           0x0025

#define PGP_ECL_ID              0x0026

#define PCD_16_ID               0x0027

#define P53B_ID                 0x0020

#define PDVFOI_ID               0x0030

#define PDVFCI_AIAG_ID          0x0038
#define PDVFCI_USPS_ID          0x0039
#define PCDFCI_SIM_ID           0x003a
#define PCDFCI_PCD_ID           0x003b

#define PSS4_ID                 0x0040
#define PSS16_ID                0x0041

#define PCDA_ID                 0x0044
#define PCDA16_ID               0x0045
#define PCDCL_ID                0x0046
#define PE4CDA_ID               0x0047

#define PDVA_ID                 0x0048
#define PDVA16_ID               0x0049
#define PDVCL2_ID               0x004a
#define PDVCLS_ID               0x004a /* alias */

#define PE4CDA16_ID             0x004b

#define PDVFOX_ID               0x004c
#define PCDFOX_ID               0x004e
#define PCDHSS_ID               PCDFOX_ID /* backwards compat: PCDHSS name is obsolete */
#define PDVHSS_ID               PDVFOX_ID /* backwards compat: PDVHSS name is obsolete */

#define PE4CDALL_ID             0x004f

#define PGS4_ID                 0x0050
#define PGS16_ID                0x0051

#define DMY_ID                  0x0080
#define DMYK_ID                 0x0081
#define DMYFOI_ID               0x0081
#define DMY44_ID                0x0081

#define PE8DVFOX_ID             0x0090
#define PE8DVAFOX_ID            0x0090 /* alias */
#define PE4DVCL_ID              0x0091
#define PE4DVFOX_ID             0x0092
#define PE4DVAFOX_ID            0x0092 /* alias */
#define PE8DVCL_ID              0x0093

#define PE4DVFCI_ID             0x0094
#define PE8DVFCI_ID             0x0095

#define PE8LX1_ID               0x0096
#define PE8LX16_ID              0x0097
#define PE8LX16_LS_ID           0x0098

#define PE1_53B_ID              0x0099

#define PE4AMC16_ID             0x009a

#define PE8DVCL2_ID             0x009b
#define PE8DVCLS_ID             0x009b   /* alias */

#define PE8LX32_ID              0x009c

#define PE1DVVL_ID               0x009d

#define PE4DVVLFOX_ID            0x009e

#define PC104ICB_ID             0x009f

#define PE8G3S5_ID              0x00a0
#define PE84P_ID                0x00a0 /* alias */
#define WSU1_ID                 0x00a1
#define SNAP1_ID                0x00a2

#define PE4DVVL_ID               0x00a3

/*
 * LCR group IDs
 */
#define PE4BL_RADIO_ID          0x00a4
#define PE1BL_TIMING_ID         0x00a5
#define PE8BL_NIC_ID            0x00a6 /* 2x10G, 2x12.5G */
#define PE8BL_2X10G_ID          0x00a6 /* aliases (DO NOT DELETE THESE) */ 
#define PE8BL_10GNI_ID          0x00a6
#define PE8BL_10GNIC_ID         0x00a6
#define LCRBOOT_ID              0x00a7
#define PE8G2V7_ID              0x00a8
#define PE8VLCLS_ID             0x00a9
#define PE8G3KU_ID              0x00aa
#define PE8G3A5_ID              0x00ab
#define RESERVED_AC_ID          0x00ac /* was 2x12.5G now reserved, could re-use */
#define PE4BL_RXLFRADIO_ID      0x00ad
#define PE4BL_LFRADIO_ID        0x00ad /* old (compat) */
#define RESERVED_AE_ID          0x00ae /* was E3NIC_ID now reserved, could re-use */
#define PE4BL_TXLFRADIO_ID      0x00af
#define PE4BL_TXRADIO_ID        0x00af /* old (compat) */
#define PE1BL_WBADC_ID          0x00b1
#define PE8BL_WBDSP_ID          0x00b2

#define PE8G2CML_ID             0x00b0

/*
 * reserved -- change here and everywhere else when assigned
 */

/*************************************
 * Mezzanine ID values 
 *
 *************************************/

#define MEZZ_RS422              0x0
#define MEZZ_LVDS               0x1
#define MEZZ_UNKN_EXTBDID       0x2
#define MEZZ_RS422_10_LVDS_10   0x3
#define MEZZ_SSE                0x4
#define MEZZ_HRC                0x5
#define MEZZ_OCM                0x6
#define MEZZ_COMBO2             0x7
#define MEZZ_ECL                0x8
#define MEZZ_TLK1501            0x9
#define MEZZ_SRXL               0xa
#define MEZZ_COMBO3_RS422       0xb
#define MEZZ_COMBO3_LVDS        0xc
#define MEZZ_COMBO3_ECL         0xd
#define MEZZ_COMBO2_RS422       0xe
#define MEZZ_COMBO              0xf
#define MEZZ_16TE3              0x10
#define MEZZ_OC192              0x11
#define MEZZ_3X3G               0x12
#define MEZZ_MSDV               0x13
#define MEZZ_SRXL2              0x14
#define MEZZ_NET10G             0x15
#define MEZZ_DRX                0x16
#define MEZZ_DDSP               0x17
#define MEZZ_SRXL2_IDM_LBM      0x18
#define MEZZ_SRXL2_IDM_IDM      0x19
#define MEZZ_SRXL2_IMM_IMM      0x1a
#define MEZZ_SRXL2_IMM_LBM      0x1b
#define MEZZ_SRXL2_IDM_IMM      0x1c
#define MEZZ_DRX16              0x1d
#define MEZZ_OCM2P7G            0x1e  /* TEMP: SB: not sure what it is *ACTUALLY* in the driver */
#define MEZZ_THREEP             0x1f
#define MEZZ_FAN                0x20
#define MEZZ_V4ACL              0x21

#define MEZZ_LAST MEZZ_V4ACL

#define MEZZ_ERR_NO_BITFILE 0xcc
#define MEZZ_ERR_NO_REGISTER 0xff
#define MEZZ_ERR_BAD_BITSTREAM 0xffffffff /* problem with extbdid lookup */

#define edt_is_combo3(id) (((id) == MEZZ_COMBO3_RS422) || \
        ((id) == MEZZ_COMBO3_LVDS) || \
        ((id) == MEZZ_COMBO3_ECL))

#define MEZZ_ID_UNKNOWN         0xffff

#endif
