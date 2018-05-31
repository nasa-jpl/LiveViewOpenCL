
#ifndef EDT_NET_MSG_H_
#define EDT_NET_MSG_H_

#ifdef _NT_
#include <Winsock.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

/* port below picked arbitrarily. */
#define EDT_LOG_PORT 3490
#define EDT_LOG_BACKLOG 5


/* When using edt_msg functions, a NetMsgData can be set as the target, 
 * with edt_msg_net_send set as the function. 
 * Then messages are sent to the host specified in the NetMsgData,
 * and they can optionally (if use_stdout is true) be printed as well. */
typedef struct {
	/* host IP or name */
	char *host; 
	/* if true, also print msg on stdout */
	int use_stdout;

	/* internal use only: */
	int sockfd; /* socket we're connected to. */
	


} NetMsgData;

/* An EdtMsgFunction, that sends messages to a networked host and optionally stdout.
 * It takes a NetMsgData as the target.  
 * The string message parameter is sent to the host specified by the target,
 * and if the use_stdout value is true the message will be printed as well.
 * The level argument is currently ignored.
 */
int edt_msg_net_send (void *target, int level, char *message);

/* initialize networking */
void edt_net_init (NetMsgData *net_info);

/* close resources */
void edt_net_close (NetMsgData *net_info);


/* Sends the message string to the named host. */
void edt_net_send (char *messsage, char *hostname);


#endif
