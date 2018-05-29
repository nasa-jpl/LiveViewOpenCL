#ifndef EDTREGBITS_H
#define EDTREGBITS_H


/***************************/
/* Bit field names extracted from edtreg.h */
/* Can be used for debugging and */
/* printed out by setdebug */
/***************************/


typedef struct _BitLabels {
	char *label;
	unsigned int bit;
	struct _BitLabels *sub_bits;
} BitLabels;


EDTAPI extern BitLabels BL_SG_NXT_CNT_WRITE[];
EDTAPI extern BitLabels BL_SG_NXT_CNT[];
EDTAPI extern BitLabels BL_SG_LIST_CNTL[];
EDTAPI extern BitLabels BL_DMA_CFG_WRITE[];
EDTAPI extern BitLabels BL_DMA_CFG[];
EDTAPI extern BitLabels BL_X_PROG[];
EDTAPI extern BitLabels BL_DMA_STATUS[]; 
EDTAPI extern BitLabels BL_REGISTERS[];
EDTAPI extern BitLabels BL_PDV_ROICTL[];
EDTAPI extern BitLabels BL_PDV_CMD[];
EDTAPI extern BitLabels BL_PDV_CFG[];
EDTAPI extern BitLabels BL_PDV_DATA_PATH[];
EDTAPI extern BitLabels BL_PDV_STAT[];
EDTAPI extern BitLabels BL_PDV_UTILITY[];
EDTAPI extern BitLabels BL_PDV_UTIL2[];
EDTAPI extern BitLabels BL_PDVA_UTIL2[];
EDTAPI extern BitLabels BL_PDV_BYTESWAP[];
EDTAPI extern BitLabels BL_PDV_SHIFT[];
EDTAPI extern BitLabels BL_PDV_MODE_CNTL[];
EDTAPI extern BitLabels BL_PDVCL_MODE_CNTL[];
EDTAPI extern BitLabels BL_PDV_MCTL_AIA[];
EDTAPI extern BitLabels BL_PDV_SERIAL_DATA_STAT[];
EDTAPI extern BitLabels BL_PDV_SERIAL_DATA_CNTL[];
EDTAPI extern BitLabels BL_PDV_UTIL3[];
EDTAPI extern BitLabels BL_PDV_LHS_CONTROL[];


#endif


