#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>

#include <arpa/inet.h>

#include "responseState.h"

#include "../connection-structure.h"
#include "../proxyStm.h"
#include "../proxyActiveHandlers.h"
#include "../../utils/buffer/buffer.h"


int pareserResponseIsDone() {
    return 0;
}

unsigned responseRead(struct selector_key * key) {
    struct Connection * conn = DATA_TO_CONN(key);
	uint8_t *ptr;
	size_t count;
	ssize_t  n;

    /* Read from origin and save in buffer */
	ptr = buffer_write_ptr(&conn->writeBuffer, &count);
	n = recv(key->fd, ptr, count, 0);
	if(n <= 0) { // client closed connection
printf("[ERROR] {response} recv got 0 bytes\n");
        return ERROR;
	}

    buffer_write_adv(&conn->writeBuffer, n);

    printf("[RESPONSE] got response from origin. Size: %d\n", (int) n);

    //parese response

    //key->fd should be originFd if there is no transformation

    /* Working with no transformation */

    /* Set write response to client */
    if(selector_set_interest(key->s, conn->clientFd, OP_WRITE) != SELECTOR_SUCCESS) { // write response to client
        return ERROR;
    }

    /* If there is space in buffer and response is not done, continue reading response */
    fd_interest originInterest = OP_NOOP;
    if(buffer_can_write(&conn->writeBuffer) && !pareserResponseIsDone()) {
        printf("continue reading from client\n");
        originInterest = OP_READ;
    }

    if(selector_set_interest_key(key, originInterest) != SELECTOR_SUCCESS) {
        return ERROR;
    }

    return RESPONSE;
}

unsigned responseWrite(struct selector_key * key) {
    struct Connection * conn = DATA_TO_CONN(key);
	uint8_t *ptr;
	size_t count;
	ssize_t  n;

    /* Send bufferd data to the client */
	ptr = buffer_read_ptr(&conn->writeBuffer, &count);
	n = send(key->fd, ptr, count, 0);
	if(n <= 0) { // client closed connection
printf("[ERROR] {response} send got 0 bytes\n");
        return ERROR;
	}

    buffer_read_adv(&conn->writeBuffer, n);

    printf("Writing to client. Size %d", (int) n);

    /* If buffer is empty and response is done go to done state */
    if(!buffer_can_read(&conn->writeBuffer) && pareserResponseIsDone()) {
        /* I can delete interests first */
        return DONE;
    }

    /* If the request is not done continue reading from origin */
    if(!pareserResponseIsDone()) {
       if(selector_set_interest(key->s, conn->originFd, OP_READ) != SELECTOR_SUCCESS) { // continue writing to origin server if buff is not empty
            return ERROR;
        } 
    }

    /* If buffer is not empty write again to the client */
    fd_interest clientInterest = OP_NOOP;
    if(buffer_can_read(&conn->writeBuffer)) {
        clientInterest = OP_WRITE;
    }

    if(selector_set_interest_key(key, clientInterest) != SELECTOR_SUCCESS) { // continue writing to origin server if buff is not empty
        return ERROR;
    }

    return RESPONSE;
}