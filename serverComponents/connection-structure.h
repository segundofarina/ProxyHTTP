#ifndef _CONNECTION_STRUCTURE_
#define _CONNECTION_STRUCTURE_

#include <sys/socket.h>
#include "../utils/buffer/buffer.h"
#include "../utils/stm/stm.h"

struct Connection {
	/* client info */
	struct sockaddr_storage clientAddr;
    socklen_t clientAddrLen;
    int clientFd;

	/* server info */
	struct sockaddr_storage originAddr;
    socklen_t originAddrLen;
    //int                           origin_domain;
    int originFd;

	/* state machine */
	struct state_machine stm;

	/* general io buffers */
	uint8_t rawBuff_a[2048], rawBuff_b[2048];
    buffer readBuffer, writeBuffer;
    
    /* amount of references to this struct, if 1 it should be destroyed */
    unsigned references;

    /* next element in the pool, just using it when the struct is empty */
    struct Connection *next;

};

/* cast void * data to struct Connection */
#define DATATOCONN(key) ( (struct Connection *)(key)->data )

#endif