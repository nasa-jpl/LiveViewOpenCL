/*
 * Copyright (c) 2017 Engineering Design Team (EDT), Inc.
 * All rights reserved.
 *
 * This file is subject to the terms and conditions of the EULA defined at
 * www.edt.com/terms-of-use
 *
 * Technical Contact: tech@edt.com
 */

#ifndef EDT_SI5338_REGS_H
#define EDT_SI5338_REGS_H

#include "edtinc.h"
#include "lib_two_wire.h"

#define LOCK_MASK 0x15
#define LOS_MASK  0x04

#define BB0(x) ((u_char)x)             // Bust int32 into Bytes
#define BB1(x) ((u_char)(x>>8))
#define BB2(x) ((u_char)(x>>16))

#define SI5338_BX_XTAL ((u_int) 25000000)
#define SI5338_MSn ((u_int) 100)

#define ABOUT_TEXT "changes clock frequencies of a si5338"


#ifdef __cplusplus
extern "C" {
#endif

EDTAPI void edt_si5338_init(EdtDev *edt_p, int verbose);
EDTAPI void edt_si5338_setFreq(EdtDev *edt_p, u_char clk, u_int freq, int verbose);

#ifdef __cplusplus
}
#endif

#endif
