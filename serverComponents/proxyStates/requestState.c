#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>

#include <arpa/inet.h>

#include "requestState.h"
#include "errorState.h"

#include "../connection-structure.h"
#include "../proxyStm.h"
#include "../proxyActiveHandlers.h"
#include "../../utils/buffer/buffer.h"
#include "../../parser/request.h"
#include "../../logger/logger.h"
#include "../metrics.h"

int parserHasOrigin(struct request_parser parser) {
    return parser.hasDestination;
}

int parserIsRequestDone(struct request_parser parser) {
    return parser.state == request_done;
}

/* Returns 0 on error */
int connectToOrigin(struct selector_key * key) {
    struct Connection * conn = DATA_TO_CONN(key);

    loggerWrite(DEBUG, "Attempt connection to origin server\n");

    if( (conn->originFd = socket(conn->originDomain, SOCK_STREAM, 0)) == -1) {
        //handle error
        goto handle_errors;
    }

    //setsockopt(conn->originFd, SOL_SOCKET, SO_NOSIGPIPE, &(int){ 1 }, sizeof(int));

    if(selector_fd_set_nio(conn->originFd) == -1) {
        // handle error
        goto handle_errors;
    }

    loggerWrite(DEBUG, "Connecting to origin server\n");
    
    if (connect(conn->originFd, (const struct sockaddr *) &conn->originAddr, conn->originAddrLen) == -1) {
        if(errno == EINPROGRESS) { /* Wait until connection is established */
            
            if(selector_register(key->s, conn->originFd, &connectionHandler, OP_WRITE, key->data) != SELECTOR_SUCCESS) {
                // handle error
                goto handle_errors;
            }

            loggerWrite(DEBUG, "Connection will be done when I can write to it\n");
            
            conn->references += 1;

        } else {
            // handle errors
            goto handle_errors;
        }
    }

    return 1;


    handle_errors:

        loggerWrite(DEBUG, "Error while connecting to origin\n");
    
        /* It's never been registered in the selector */
        if(conn->originFd != -1) {
            close(conn->originFd);
        }
        return 0;
}

void * solveDNS(void * data) {
    struct selector_key * key = (struct selector_key *) data;
    struct Connection * conn = DATA_TO_CONN(key);

    pthread_detach(pthread_self());

    conn->origin_resolution = 0;

    /* Taken from socks5nio.c */
    struct addrinfo hints = {
        .ai_family    = AF_UNSPEC,    /* Allow IPv4 or IPv6 */
        .ai_socktype  = SOCK_STREAM,  /* Datagram socket */
        .ai_flags     = AI_PASSIVE,   /* For wildcard IP address */
        .ai_protocol  = 0,            /* Any protocol */
        .ai_canonname = NULL,
        .ai_addr      = NULL,
        .ai_next      = NULL,
    };

    char buff[7];
    snprintf(buff, sizeof(buff), "%d", ntohs(conn->requestParser.reqParser.destintation.destPort));

    loggerWrite(DEBUG, "New thread created to resolve DNS\n");

    getaddrinfo(conn->requestParser.reqParser.destintation.destAddr.fqdn, buff, &hints, &conn->origin_resolution);

    loggerWrite(DEBUG, "DNS resolved, notifying to parent thread\n");

    selector_notify_block(key->s, key->fd);

    free(data);

    return 0;
}

/* returns 0 on error */
int startOriginConnection(struct selector_key * key) {
    pthread_t tid;
    struct Connection * conn = DATA_TO_CONN(key);
    struct requestData * rData = &(conn->requestParser.reqParser.destintation);
    int ret = 0;
    struct selector_key * k;

    conn->isConnectingOrigin = 1;

    switch(rData->destAddrType) {
        case IPv4:
            conn->originDomain = AF_INET;
            rData->destAddr.ipv4.sin_port = rData->destPort;
            conn->originAddrLen = sizeof(rData->destAddr.ipv4);
            memcpy(&conn->originAddr, &rData->destAddr, conn->originAddrLen);
            ret = connectToOrigin(key);
            break;
        case IPv6:
            conn->originDomain = AF_INET6;
            rData->destAddr.ipv6.sin6_port = rData->destPort;
            conn->originAddrLen = sizeof(rData->destAddr.ipv6);
            memcpy(&conn->originAddr, &rData->destAddr, conn->originAddrLen);
            ret = connectToOrigin(key);
            break;
        case DOMAIN:
            k = malloc(sizeof(*k));
            if(k == NULL) {
                return 0;
            } else {
                memcpy(k, key, sizeof(*k));
                if(pthread_create(&tid, 0, solveDNS, k) == -1) {
                    ret = 0;
                } else {
                    ret = 1;
                }
            }
            break;
    }
    
    return ret;
}

unsigned requestRead(struct selector_key * key) {
	struct Connection * conn = DATA_TO_CONN(key);
	uint8_t *ptr;
	size_t count;
	ssize_t  n;

    /* Read request and keep it in readBuffer */
	ptr = buffer_write_ptr(&conn->readBuffer, &count);
	n = recv(key->fd, ptr, count, 0);
	if(n <= 0) { // client closed connection
        return FATAL_ERROR;
	}
    /* Save value for metrics */
    addBytesRead(n);

    buffer_write_adv(&conn->readBuffer, n);
    
    request_parser_consume(&conn->requestParser.reqParser, (char *)ptr, n);

    if(!parserHasOrigin(conn->requestParser.reqParser)) { // si no tengo el origin y no lo puedo tener de la request
        
        loggerWrite(DEBUG, "Parser needs origin\n");
        
        if(buffer_can_write(&conn->readBuffer)) {
            return REQUEST;
        } else {
            //return ERROR;
            return setError(key, REQ_HEADER_TOO_LONG_431);
        }
    }

    if(conn->originFd == -1 && !conn->isConnectingOrigin) { // y no estoy resolviendo todavia
    
        loggerWrite(DEBUG, "Attempt connection to origin server\n");
    
        /* New thread to solve DNS */
        if(startOriginConnection(key) == 0) {
            
            loggerWrite(DEBUG, "[ERROR] Failed to connect to origin server\n");
            
            return setError(key, BAD_GATEWAY_502);
        }
    }

    /* Check if I should still listen to the client. Only if buffer is not full and the request is not done */
    fd_interest cliInterest = OP_NOOP;
    if(buffer_can_write(&conn->readBuffer) && !parserIsRequestDone(conn->requestParser.reqParser)) {
        cliInterest = OP_READ;
    }

    /* Set client interest */
    if(selector_set_interest_key(key, cliInterest) != SELECTOR_SUCCESS) { 
        return setError(key, INTERNAL_SERVER_ERR_500);
    }

    /* Set origin interest */
    if(conn->originFd != -1) {
        if(selector_set_interest(key->s, conn->originFd, OP_WRITE) != SELECTOR_SUCCESS) { 
            return setError(key, INTERNAL_SERVER_ERR_500);
        }
    }
	return REQUEST;
}



unsigned requestWrite(struct selector_key * key) {
    struct Connection * conn = DATA_TO_CONN(key);
	uint8_t *ptr;
	size_t count;
	ssize_t  n;
    unsigned ret = REQUEST;

    /* Send bufferd data to the origin server */
    ptr = buffer_read_ptr(&conn->readBuffer, &count);
	n = send(conn->originFd, ptr, count, MSG_NOSIGNAL);

	if(n <= 0) { // origin closed connection

        loggerWrite(DEBUG, "Origin closed connection\n");
    
        return setError(key, BAD_GATEWAY_502);
	}

    buffer_read_adv(&conn->readBuffer, n);

    /* If the request is not done enable client to read */
    if(!parserIsRequestDone(conn->requestParser.reqParser)) {
        if(selector_set_interest(key->s, conn->clientFd, OP_READ) != SELECTOR_SUCCESS) { 
            return setError(key, INTERNAL_SERVER_ERR_500);
        }
    }

    /* If the buffer is not empty continue writing */
    fd_interest originInterst = OP_NOOP;
    if(buffer_can_read(&conn->readBuffer)) {
        originInterst = OP_WRITE;
    }

    /* If the buffer is empty and the request is done move to response state */
    if(!buffer_can_read(&conn->readBuffer) && parserIsRequestDone(conn->requestParser.reqParser)) {
        originInterst = OP_READ;
        ret = RESPONSE;
    }

    /* Set origin fd interests */
    if(selector_set_interest_key(key, originInterst) != SELECTOR_SUCCESS) { 
        return setError(key, INTERNAL_SERVER_ERR_500);
    }

    return ret;
}

unsigned requestBlockReady(struct selector_key * key) {
    struct Connection * conn = DATA_TO_CONN(key);

    if(conn->origin_resolution == 0) {
        return setError(key, BAD_REQUEST_400); //maybe should be 500
    }

    /* Save resolution */
    conn->originDomain = conn->origin_resolution->ai_family;
    conn->originAddrLen = conn->origin_resolution->ai_addrlen;
    memcpy(&conn->originAddr, conn->origin_resolution->ai_addr, conn->originAddrLen);
    freeaddrinfo(conn->origin_resolution);
    conn->origin_resolution = 0;

    if( !connectToOrigin(key) ) {

        loggerWrite(DEBUG, "[ERROR] Connection to origin failed\n");

        return setError(key, BAD_GATEWAY_502);
    }
    
    /* What to do next? */

    /* Write buffered request to the origin server */
    if(selector_set_interest(key->s, conn->originFd, OP_WRITE) != SELECTOR_SUCCESS) {
        return setError(key, INTERNAL_SERVER_ERR_500);
    }

    return REQUEST;
}

void requestArrival(const unsigned state, struct selector_key * key) {
    struct Connection * conn = DATA_TO_CONN(key);
    request_parser_init(&conn->requestParser.reqParser);
}

void requestDeparture(const unsigned state, struct selector_key * key) {
    struct Connection * conn = DATA_TO_CONN(key);
    request_parser_close(&conn->requestParser.reqParser);
}
