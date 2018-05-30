#ifndef _CONNECTION_STRUCTURE_
#define _CONNECTION_STRUCTURE_

#include <stdint.h>
#include <sys/socket.h>
#include <netdb.h>

#include <netinet/in.h>

#include "../utils/buffer/buffer.h"
#include "../utils/stm/stm.h"

enum TransformationType {
    NO_TRANSFORM,
    TRANSFORM_CAT
};

/* Parte del request parser */
enum addrType {
    IPv4, IPv6, DOMAIN 
};

union socks_addr {
    char fqdn[0xff];
    struct sockaddr_in  ipv4;
    struct sockaddr_in6 ipv6;
};

struct requestData {
    enum addrType destAddrType;
    union socks_addr destAddr;
    /** port in network byte order */
    in_port_t destPort;
};
/* end */

struct httpRequestParser {
    //struct parser
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

	/* state machine */
	struct state_machine stm;

    /* parsers */
    struct httpRequestParser requestParser;


	/* general io buffers for client */
	uint8_t rawBuff_a[2048], rawBuff_b[2048];
    buffer readBuffer, writeBuffer;

    /* response buffers */
    uint8_t rawBuff_c[2048], rawBuff_d[2048], rawBuff_e[2048];
    buffer respTempBuffer, inTransformBuffer, outTransformBuffer;

    /* transformation details */
    int writeTransformFd, readTransformFd;
    enum TransformationType trasformationType;
    
    /* amount of references to this struct, if 1 it should be destroyed */
    unsigned references;

    /* next element in the pool, just using it when the struct is empty */
    struct Connection *next;

};

/* cast void * data to struct Connection */
#define DATA_TO_CONN(key) ( (struct Connection *)(key)->data )

#endif