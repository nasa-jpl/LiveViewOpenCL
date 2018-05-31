/**
 * @file
 * Generalized error handling for edt and pdv libraries (header file)
 *
 * Copyright 2000 Engineering Design Team
 *
 */
#ifndef _edt_msg_H
#define _edt_msg_H

#include "edt_utils.h"
#include <stdarg.h>

#ifdef __cplusplus

extern "C" {

#endif


/** 
  * @defgroup msg EDT Message Handler Library
  *
  * Provides generalized error- and message-handling for the edt and pdv
  * libraries.
  *
  * These routines provide a way for application programs to intercept
  * and handle edtlib and pdvlib error, warning, and debug messages, but
  * you can also use them for application messages.
  *
  * By default, output goes to the console (stdout), but you can
  * substitute user-defined functions -- for example, a function that
  * pops up a window to display text. You can set different message
  * levels for different output, and multiple message handles can exist
  * within an application, with different message handlers associated
  * with them.
  *
  * Predefined message flags are described in the "Defines" section of
  * this document. Those starting with \c EDTAPP_MSG_ are for general
  * application use, those starting with \c EDTLIB_MSG_ are for libedt
  * messages, and those beginning with \c PDVLIB_MSG_ are for libpdv
  * messages.  Application programmers can define other flags in the
  * 0x1000 to 0x1000000 range.
  *
  * Message levels are defined by flag bits, and each bit can be set or
  * cleared individually. So, for example, to have a message-handler
  * called only for fatal and warning application messages, specify
  * <tt> EDTAPP_MSG_FATAL | EDTAPP_MSG_WARNING </tt>.
  *
  * As you can see, the edt and pci dv libraries have their own message
  * flags. These can be turned on and off from within an application,
  * and also by setting the environment variables \c EDTDEBUG and \c PDVDEBUG,
  * respectively, to values greater than zero.
  *
  * Application programs ordinarily specify combinations of either the
  * \c EDTAPP_MSG_ or \c EDT_MSG_ flags for their messages.
  *
  * \b Files
  *
  * \arg \e edt_error.h header file (automatically included if \e edtinc.h is
  *     included)
  * \arg \e edt_error.c: message subroutines
  *
  * The #EdtMsgHandler structure is defined in \e edt_error.h.  For
  * compatibility with possible future changes, do not access structure
  * elements directly; instead always use the error subroutines. 
  *
  * @{
  */



/**
  * An EdtMsgFunction is a function which outputs a message if that
  * message's level is high enough.  
  *
  * @param target this stores extra info useful to the specific function
  * defined.  In the default message handler setup by edt_msg_init, 
  * the function used expects target to be a FILE pointer. 
  * @param level The message level associated with with the message.
  * @param message The message which can be output by the function.
  */
typedef int (*EdtMsgFunction)(void *target, int level, const char *message);

/**
 * Structure used by the \ref msg "Message Handler Library" to control 
 * the output of messages. 
 */


typedef struct _edt_msg_handler {

  EdtMsgFunction func; 
  int level;
  /** The file the default handler function sends output to (\c stderr). */
  FILE * file; 
  /** Flag set by #edt_msg_set_name to indicate that we are responsible 
   * for closing the file */
  unsigned char own_file; 
  void * target;

} EdtMsgHandler;



/* default initialization - uses stdout and stderr */

EDTAPI void edt_msg_init(EdtMsgHandler *msg_p);

EDTAPI void edt_msg_init_names(EdtMsgHandler *msg_p, char *file, int level);

EDTAPI void edt_msg_init_files(EdtMsgHandler *msg_p, FILE *file, int level);

EDTAPI void edt_msg_close(EdtMsgHandler *msg_p);

EDTAPI void edt_msg_set_level(EdtMsgHandler *msg_p, int newlevel);

EDTAPI int edt_msg_get_level(EdtMsgHandler *msg_p);

EDTAPI void edt_msg_add_level(EdtMsgHandler *msg_p, int level);

EDTAPI void edt_msg_set_function(EdtMsgHandler *msg_p, EdtMsgFunction f);

EDTAPI void edt_msg_set_file(EdtMsgHandler *msg_p, FILE *f);

EDTAPI void edt_msg_set_target(EdtMsgHandler *msg_p, void *t);

/* Sets the file to that named in the argument f */
EDTAPI void edt_msg_set_name(EdtMsgHandler *msg_p, const char *f);

EDTAPI int edt_msg(int level, const char *format, ...);
EDTAPI int edt_msg_output(EdtMsgHandler *msg_p, int level, const char *format, ...);

EDTAPI int edt_msg_perror(int level, const char *msg);
EDTAPI int edt_msg_output_perror(EdtMsgHandler *msg_p, int level, const char *message);
EDTAPI int edt_msg_printf_perror(int level, const char *format, ...);
EDTAPI int edt_msg_output_printf_perror(EdtMsgHandler *msg_p,  int level, const char *format, ...);


EDTAPI EdtMsgHandler *edt_msg_default_handle(void);
EDTAPI int edt_msg_default_level(void);
EDTAPI char *edt_msg_last_error(void); /* returns pointer to most recent 
				      msg string */

EDTAPI void edt_set_verbosity(int verbose); /* simple verbose level setting */
EDTAPI int edt_get_verbosity(void); /* return simple verbose level setting */


EDTAPI int lvl_printf(int delta, char *format, ...);

/*
 * predefined message flags. EDTAPP_MSG are for general purpose application
 * use EDTLIB_MSG are for libedt messages. PDVLIB are for libpdv messages.
 * Application programmers can define other flags in the 0x1000 to
 * 0x1000000 range
 */

/** Fatal-error messages in applications. */
#define EDTAPP_MSG_FATAL   	0x1

/** Warning messages in applications */
#define EDTAPP_MSG_WARNING  	0x2

/** First level info messages in applications. */
#define EDTAPP_MSG_INFO_1   	0x4

/** Second level info messages in applications. */
#define EDTAPP_MSG_INFO_2   	0x8


/** Fatal-error messages in libedt. */
#define EDTLIB_MSG_FATAL	0x10
/** Warning messages in libedt. */
#define EDTLIB_MSG_WARNING	0x20

/** Informative messages in libedt. */
#define EDTLIB_MSG_INFO_1	0x40

/** Debugging messages in libedt. */
#define EDTLIB_MSG_INFO_2	0x80



/** Fatal-error messages in libpdv. */
#define PDVLIB_MSG_FATAL	0x100 

/** Warning messages in libpdv. */
#define PDVLIB_MSG_WARNING	0x200

/** Informative messages in libpdv. */
#define PDVLIB_MSG_INFO_1	0x400

/** Debugging messages in libpdv. */
#define PDVLIB_MSG_INFO_2	0x800

#define EDT_MSG_ALWAYS      0x80000000


#define EDT_MSG_FATAL \
	EDTAPP_MSG_FATAL | EDTLIB_MSG_FATAL | PDVLIB_MSG_FATAL

#define EDT_MSG_WARNING \
	EDTAPP_MSG_WARNING | EDTLIB_MSG_WARNING | PDVLIB_MSG_WARNING

#define EDT_MSG_INFO_1 \
	EDTAPP_MSG_INFO_1 | EDTLIB_MSG_INFO_1 | PDVLIB_MSG_INFO_1

#define EDT_MSG_INFO_2 \
	EDTAPP_MSG_INFO_2 | EDTLIB_MSG_INFO_2 | PDVLIB_MSG_INFO_2


#define edt_msg_add_default_level(addlevel) \
    edt_msg_set_level(edt_msg_default_handle(), edt_msg_default_level() | addlevel)


/** @} */ /* end group msg */

#ifdef __cplusplus
}
#endif

typedef enum EdtReturn {

RET_SUCCESS = 0,
ERROR_GENERIC = -1,
ERROR_BADCHANNEL = -2,
ERROR_CONNECTION = -3,
ERROR_BADPARAM = -4,
ERROR_BADPATH = -5,
ERROR_BADDEVICE = -6,
ERROR_PERMISSION = -7,
ERROR_NOCODE = -8,
ERROR_UNDEFINED = -9,
ERROR_NULL_POINTER = -10,
ERROR_NO_FILE = -11,
ERROR_RESPONSE = -12

} EdtReturn;

#endif
