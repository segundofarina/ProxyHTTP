#ifndef _CONNECTION_STRUCTURE_
#define _CONNECTION_STRUCTURE_

#include <stdint.h>
#include <sys/socket.h>
#include <netdb.h>

#include <netinet/in.h>

#include "../utils/buffer/buffer.h"
#include "../utils/stm/stm.h"
#include "../parser/request.h"
#include "../parser/response_manager.h"
#include "transformationManager.h"
#include "proxyStates/errorState.h"


enum TransformationType {
    NO_TRANSFORM,
    TRANSFORM,
    IS_TRANSFORMING
};

struct httpRequestParser {
    struct request_parser reqParser;
    struct requestData requestData;
};

struct Connection {
	/* client info */
	struct sockaddr_storage clientAddr;
    socklen_t clientAddrLen;
    int clientFd;

	/* server info */
	struct sockaddr_storage originAddr;
    socklen_t originAddrLen;
    int originDomain; // IPv4, IPv6
    int originFd;

    /* origin server resolution */
    struct addrinfo *origin_resolution;

    /* connection status info */
    int isConnectingOrigin;

    /* origin has answerd */
    int originHasAnswered;

	/* state machine */
	struct state_machine stm;

    /* parsers */
    struct httpRequestParser requestParser;
    struct response_manager responseParser;


	/* general io buffers for client */
	uint8_t rawBuff_a[204800], rawBuff_b[204800];
    buffer readBuffer, writeBuffer;

    /* response buffers */
    uint8_t rawBuff_c[204800], rawBuff_d[204800], rawBuff_e[204800];
    buffer respTempBuffer, inTransformBuffer, outTransformBuffer;

    /* transformation details */
    int writeTransformFd, readTransformFd;
    enum TransformationType transformationType;
    int transformationPid;
    struct mediaTypesNode * mediaTypesList;

    /* Keep an error to inform to the client */
    enum error_code errorCode;
    
    /* amount of references to this struct, if 1 it should be destroyed */
    unsigned references;

    /* next element in the pool, just using it when the struct is empty */
    struct Connection *next;

};

/* cast void * data to struct Connection */
#define DATA_TO_CONN(key) ( (struct Connection *)(key)->data )

#endif

