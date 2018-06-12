#include <stdio.h>
#include <string.h>

#include "errorState.h"
#include "../proxyStm.h"
#include "../connection-structure.h"
#include "../metrics.h"

unsigned errorWrite(struct selector_key * key) {
    struct Connection * conn = DATA_TO_CONN(key);
	uint8_t *ptr;
	size_t count;
	ssize_t  n;

    ptr = buffer_read_ptr(&conn->writeBuffer, &count);
	n = send(key->fd, ptr, count, MSG_NOSIGNAL);
    
    if(n <= 0) { // client closed connection
        return FATAL_ERROR;
	}

    buffer_read_adv(&conn->writeBuffer, n);

    fd_interest interest = OP_NOOP;
    if(buffer_can_read(&conn->writeBuffer)) {
        interest = OP_WRITE;
    } else {
        /* Its not fatal, but I should close the connection */
        return FATAL_ERROR;
    }

    if(selector_set_interest_key(key, interest) != SELECTOR_SUCCESS) {
        return FATAL_ERROR;
    }

    return ERROR;
}

int writeErrorToBuff(char * ptr, const int count, enum error_code error) {
    char *ans = NULL;
    switch(error){
        case NO_ERROR:
            return 0;
        case BAD_REQUEST_400:
            ans = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\nContent-Length: 19\r\n\r\n400 Bad Request\r\n\r\n";
            break;
        case LENGTH_REQUIRED_411:
            ans = "HTTP/1.1 411 Length Required\r\nContent-Type: text/plain\r\nContent-Length: 23\r\n\r\n411 Length Required\r\n\r\n";
            break;
        case REQ_HEADER_TOO_LONG_431:
            ans = "HTTP/1.1 431 Request Header Fields Too Large\r\nContent-Type: text/plain\r\nContent-Length: 39\r\n\r\n431 Request Header Fields Too Large\r\n\r\n";
            break;
        case INTERNAL_SERVER_ERR_500:
            ans = "HTTP/1.1 500 Internal Server\r\nContent-Type: text/plain\r\nContent-Length: 23\r\n\r\n500 Internal Server\r\n\r\n";
            break;
        case NOT_IMPLEMENTED_501:
            ans = "HTTP/1.1 501 Internal Server\r\nContent-Type: text/plain\r\nContent-Length: 29\r\n\r\n501 Internal Server Error\r\n\r\n";
            break;
        case BAD_GATEWAY_502:
            ans = "HTTP/1.1 502 Bad Gateway\r\nContent-Type: text/plain\r\nContent-Length: 19\r\n\r\n502 Bad Gateway\r\n\r\n";
            break;
        case HTTP_VER_ERR_505:
            ans = "HTTP/1.1 505 HTTP Version Not Supported\r\nContent-Type: text/plain\r\nContent-Length: 34\r\n\r\n505 HTTP Version Not Supported\r\n\r\n";
            break;
        case LOOP_DETECTED_508:
            ans = "HTTP/1.1 508 Loop Detected\r\nContent-Type: text/plain\r\nContent-Length: 21\r\n\r\n508 Loop Detected\r\n\r\n";
            break;
        default:
            return -1;
    }
    int i = strlen(ans);
    if(i > count){
        return -1;
    }
    memcpy(ptr,ans, i);
    return i;
}

unsigned setError(struct selector_key * key, enum error_code error) {
    struct Connection * conn = DATA_TO_CONN(key);
    uint8_t *ptr;
	size_t count;
	int n;

    if(conn->clientFd == -1) {
        return FATAL_ERROR;
    }

    conn->errorCode = error;
    buffer_reset(&conn->writeBuffer);
    
    ptr = buffer_write_ptr(&conn->writeBuffer, &count);
    n = writeErrorToBuff((char *)ptr, count, error);
    buffer_write_adv(&conn->writeBuffer, n);

    addBytesSent(n);

    if(selector_set_interest(key->s, conn->clientFd, OP_WRITE) != SELECTOR_SUCCESS) {
        return FATAL_ERROR;
    }

    return ERROR;
}
