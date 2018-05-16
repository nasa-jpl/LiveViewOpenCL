
#ifndef EDT_TYPES_H
#define EDT_TYPES_H

#ifndef NULL

#define NULL (0)

#endif

typedef enum {
        HeaderNone,
        /* header data should be allocated contiguously before DMA buffer */
        HeaderBefore,
        /* these next three are within DMA */
        HeaderBegin,
        HeaderMiddle,
        HeaderEnd,
         /* header data should be allocated contiguously after DMA buffer */
        HeaderAfter,
        /* header data is someweher else in memory */
        HeaderSeparate,
        HeaderAcrossStart, /* header starts before and goes into DMA */
        HeaderAcrossEnd    /* header starts at end of DMA and continues past it */
    } HdrPosition;


/****************************************/
/* Macros which define types and type combinations */
/****************************************/
 
#define EDT_TYPE_NULL 0

#define EDT_TYPE_CHAR 1
#define EDT_TYPE_BYTE 2
#define EDT_TYPE_SHORT 3
#define EDT_TYPE_USHORT 4
#define EDT_TYPE_INT 7
#define EDT_TYPE_UINT 8
#define EDT_TYPE_FLOAT 9
#define EDT_TYPE_DOUBLE 10

/* Shortcuts for type = byte, n = 3 or 4 */

#define EDT_TYPE_RGB	11
#define EDT_TYPE_BGR	12
#define EDT_TYPE_RGBA	13
#define	EDT_TYPE_BGRA	14
#define EDT_TYPE_RGB15  15
#define EDT_TYPE_RGB16  16

#define EDT_TYPE_RGB48	17
#define EDT_TYPE_BGR48	18

#define EDT_TYPE_MONO	19
#define EDT_TYPE_BIT	19 /* for bit fields */
#define EDT_TYPE_LONG	20
#define EDT_TYPE_ULONG	21
#define EDT_TYPE_INT64	22
#define EDT_TYPE_UINT64	23
#define EDT_TYPE_BITFIELD   24
#define EDT_TYPE_STR        25
#define EDT_TYPE_STLSTR     26 /* C++ STL string type*/


/* Defines as shorthand for combining type values into constants */

/* Monadic functions - one input, one output */
#define FUNC_MONAD(type1, type2) ((type2 << 8) | type1)

/* Dyadic functions - two inputs, one output */
#define FUNC_DYAD(type1, type2, type3) ((type3 << 16) | (type2 << 8) | type1)


#define FUNC_BYTE_BYTE FUNC_MONAD(EDT_TYPE_BYTE, EDT_TYPE_BYTE)
#define FUNC_BYTE_USHORT FUNC_MONAD(EDT_TYPE_BYTE, EDT_TYPE_USHORT)
#define FUNC_USHORT_BYTE FUNC_MONAD(EDT_TYPE_USHORT, EDT_TYPE_BYTE)
#define FUNC_USHORT_USHORT FUNC_MONAD(EDT_TYPE_USHORT, EDT_TYPE_USHORT)

#define FUNC_RGB_BGR FUNC_MONAD(EDT_TYPE_RGB, EDT_TYPE_BGR)
#define FUNC_BGR_BGR FUNC_MONAD(EDT_TYPE_BGR, EDT_TYPE_BGR)
#define FUNC_BYTE_BGR FUNC_MONAD(EDT_TYPE_BYTE, EDT_TYPE_BGR)
#define FUNC_USHORT_BGR FUNC_MONAD(EDT_TYPE_USHORT, EDT_TYPE_BGR)

#define FUNC_RGB_RGB FUNC_MONAD(EDT_TYPE_RGB, EDT_TYPE_RGB)
#define FUNC_BGR_RGB FUNC_MONAD(EDT_TYPE_BGR, EDT_TYPE_RGB)
#define FUNC_BYTE_RGB FUNC_MONAD(EDT_TYPE_BYTE, EDT_TYPE_RGB)
#define FUNC_USHORT_RGB FUNC_MONAD(EDT_TYPE_USHORT, EDT_TYPE_RGB)

#define FUNC_RGB_RGBA FUNC_MONAD(EDT_TYPE_RGB, EDT_TYPE_RGBA)
#define FUNC_BGR_RGBA FUNC_MONAD(EDT_TYPE_BGR, EDT_TYPE_RGBA)
#define FUNC_BYTE_RGBA FUNC_MONAD(EDT_TYPE_BYTE, EDT_TYPE_RGBA)
#define FUNC_USHORT_RGBA FUNC_MONAD(EDT_TYPE_USHORT, EDT_TYPE_RGBA)

#define FUNC_BGR_BGRA FUNC_MONAD(EDT_TYPE_BGR, EDT_TYPE_BGRA)
#define FUNC_BYTE_BGRA FUNC_MONAD(EDT_TYPE_BYTE, EDT_TYPE_BGRA)
#define FUNC_USHORT_BGRA FUNC_MONAD(EDT_TYPE_USHORT, EDT_TYPE_BGRA)

#define FUNC_BGR_RGB15 FUNC_MONAD(EDT_TYPE_BGR, EDT_TYPE_RGB15)
#define FUNC_BYTE_RGB15 FUNC_MONAD(EDT_TYPE_BYTE, EDT_TYPE_RGB15)
#define FUNC_USHORT_RGB15 FUNC_MONAD(EDT_TYPE_USHORT, EDT_TYPE_RGB15)

#define FUNC_BGR_RGB16 FUNC_MONAD(EDT_TYPE_BGR, EDT_TYPE_RGB16)
#define FUNC_BYTE_RGB16 FUNC_MONAD(EDT_TYPE_BYTE, EDT_TYPE_RGB16)
#define FUNC_USHORT_RGB16 FUNC_MONAD(EDT_TYPE_USHORT, EDT_TYPE_RGB16)


typedef unsigned char byte;

/* Macro to return whether type is a signed / unsigned type */

#define IsSigned(type) (((type) &1) || ((type) >= EDT_TYPE_FLOAT))

#define TypeSize(type) ((type < EDT_TYPE_FLOAT)?((type+1) >> 1) : \
(type == EDT_TYPE_DOUBLE || type == EDT_TYPE_INT64 || type == EDT_TYPE_UINT64)? 8 : \
(type == EDT_TYPE_BGR || type == EDT_TYPE_RGB) ? 3 : \
(type == EDT_TYPE_RGB15 || type == EDT_TYPE_RGB16)? 2 : \
(type == EDT_TYPE_MONO)? 0.125 : 4)

#define EDT_MAX_COLOR_PLANES 4

/* Color tag values - pixel color order  */

#define COLOR_MONO 0
#define COLOR_RGB	1
#define COLOR_RGBA	2
#define COLOR_BGR	3
#define COLOR_BGRA	4



enum EdtDataType {
    TypeNull = EDT_TYPE_NULL,
    TypeCHAR = EDT_TYPE_CHAR,
    TypeBYTE = EDT_TYPE_BYTE,
    TypeSHORT = EDT_TYPE_SHORT,
    TypeUSHORT = EDT_TYPE_USHORT,
    TypeINT = EDT_TYPE_INT,
    TypeUINT = EDT_TYPE_UINT,
    TypeFLOAT = EDT_TYPE_FLOAT,
    TypeDOUBLE = EDT_TYPE_DOUBLE,

/* Shortcuts for type = byte, n = 3 or 4 */

    TypeRGB = EDT_TYPE_RGB,
    TypeBGR = EDT_TYPE_BGR,
    TypeRGBA = EDT_TYPE_RGBA,
    TypeBGRA = EDT_TYPE_BGRA,
    TypeRGB15 = EDT_TYPE_RGB15,
    TypeRGB16 = EDT_TYPE_RGB16,

    TypeRGB48 = EDT_TYPE_RGB48,
    TypeBGR48 = EDT_TYPE_BGR48,

    TypeMONO = EDT_TYPE_MONO,
    TypeLONG = EDT_TYPE_LONG,
    TypeULONG = EDT_TYPE_ULONG,
    TypeINT64 = EDT_TYPE_INT64,
    TypeUINT64 = EDT_TYPE_UINT64,

    TypeBITFIELD = EDT_TYPE_BITFIELD,

    TypeSTR = EDT_TYPE_STR,
    TypeSTLSTR = EDT_TYPE_STLSTR
};




#define EDT_BYTE_SWAP   0x1
#define EDT_SHORT_SWAP  0x2
#define EDT_LSB_FIRST   0x4

/** Both swaps give network order on x86 architecture */

#define EDT_SWAP (EDT_BYTE_SWAP | EDT_SHORT_SWAP)

#define EDT_ORDER_MASK  0x7


#endif

