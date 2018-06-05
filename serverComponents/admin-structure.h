#ifndef _ADMIN_CONN_STRUCT_H_
#define _ADMIN_CONN_STRUCT_H_


#include <stdint.h>
#include <sys/socket.h>
#include <netdb.h>

#include <netinet/in.h>

#include "../utils/buffer/buffer.h"
#include "../utils/stm/stm.h"

struct AdminConn {
    /* client info */
	struct sockaddr_storage clientAddr;
    socklen_t clientAddrLen;
    int adminFd;

	/* state machine */
	struct state_machine stm;

    /* parsers */
    //struct httpRequestParser requestParser;
    //struct response_parser responseParser;


	/* general io buffer */
	uint8_t rawBuff_a[2048];
    buffer buff;

    /* Keep an error to inform to the client */
    //enum error_code errorCode;
    
    /* amount of references to this struct, if 1 it should be destroyed */
    unsigned references;

    /* next element in the pool, just using it when the struct is empty */
    struct AdminConn *next;
};


/* cast void * data to struct AdminConn */
#define DATA_TO_ADMIN(key) ( (struct AdminConn *)(key)->data )

#endif