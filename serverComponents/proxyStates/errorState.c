#include <stdio.h>
#include <string.h>

#include "errorState.h"
#include "../proxyStm.h"
#include "../connection-structure.h"

unsigned errorWrite(struct selector_key * key) {
    struct Connection * conn = DATA_TO_CONN(key);
	uint8_t *ptr;
	size_t count;
	ssize_t  n;

    ptr = buffer_read_ptr(&conn->writeBuffer, &count);
	n = send(key->fd, ptr, count, MSG_NOSIGNAL);
    
    if(n <= 0) { // client closed connection
        printf("client closed connection\n");
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
    memcpy(ptr, "ERROR", 5);
    ptr[6] = 0;
    return 6;
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

    if(selector_set_interest(key->s, conn->clientFd, OP_WRITE) != SELECTOR_SUCCESS) {
        return FATAL_ERROR;
    }

    return ERROR;
}