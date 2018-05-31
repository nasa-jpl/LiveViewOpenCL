#ifndef _INC_CPUID
#define _INC_CPUID


#include <windows.h>

#define _CPU_FEATURE_MMX    0x0001
#define _CPU_FEATURE_SSE    0x0002
#define _CPU_FEATURE_SSE2    0x0004
#define _CPU_FEATURE_3DNOW  0x0008


#define _MAX_VNAME_LEN	13
#define _MAX_MNAME_LEN	30

typedef struct _processor_info {
    char  v_name[_MAX_VNAME_LEN];       // vendor name
    char  model_name[_MAX_MNAME_LEN];   // Name of model eg. Intel_Pentium_Pro
    int family;                         // family of the processor, eg. Intel_Pentium_Pro is family 6 processor
    int model;                          // madel of processor, eg. Intel_Pentium_Pro is model 1 of family 6 processor
    int stepping;                       // Processor revision number
    int feature;                        // processor Feature ( same as return value).
    int os_support;                     // does OS Support the feature
    int checks;                         // what all bits are checked are set to one.
} _p_info;


int _cpuid (_p_info *);

#endif
