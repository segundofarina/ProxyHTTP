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

/* Esto va en response parser */
enum parserState {
    HEADERS,
    BODY,
    DONE,
    ERROR
};
/* End */

int pareserResponseIsDone() {
    return 0;
}

enum parserState copyTempToWriteBuff(struct selector_key * key) {
    struct Connection * conn = DATA_TO_CONN(key);
	uint8_t * ptrToParse, * ptrFromParse;
	size_t maxTempBuffSize, maxWriteBuffSize;
	int  parsedBytes;
    enum parserState state;

    /* parse string from buffer_read_ptr(&conn->respTempBuffer, &maxTempBuffSize) */
    ptrToParse = buffer_read_ptr(&conn->respTempBuffer, &maxTempBuffSize);

    /* leave parsed data in buffer_write_ptr(&conn->writeBuffer, &maxWriteBuffSize) */
    ptrFromParse = buffer_write_ptr(&conn->writeBuffer, &maxWriteBuffSize);

    /* parse string size is min(maxWriteBuffSize, maxTempBuffSize) */
    /* leave bytes parsed in parsedBytes */
    //state = parser_consume(parser, ptrToParse, min(), ptrFromParse, &parsedBytes);

    /* move temp buffer pointer accoring to parsedBytes */
    buffer_read_adv(&conn->respTempBuffer, parsedBytes);

    /* move write buffer pointer according to parsedBytes */
    buffer_write_adv(&conn->writeBuffer, parsedBytes);

    /* set interest */
    /* If I wrote to writeBuffer, clientFd OP_WRITE */
    if(parsedBytes > 1) {
        /* Write response to client */
        if(selector_set_interest(key->s, conn->clientFd, OP_WRITE) != SELECTOR_SUCCESS) {
            return ERROR;
        }
    }
        
    /* If respTempBuffer is not full and response is not done originFd OP_READ */
    if(buffer_can_read(&conn->respTempBuffer) && !pareserResponseIsDone()) {
        if(selector_set_interest(key->s, conn->originFd, OP_READ) != SELECTOR_SUCCESS) {
            return ERROR;
        }
    }

    /* save parser return status */
    return state;
}

enum parserState copyTempToTransformBuff(struct selector_key * key) {
    struct Connection * conn = DATA_TO_CONN(key);
	uint8_t * ptrToParse, * ptrFromParse;
	size_t maxTempBuffSize, maxWriteBuffSize;
	int  parsedBytes;
    enum parserState state;

    /* parse string from buffer_read_ptr(&conn->respTempBuffer, &maxTempBuffSize) */
    ptrToParse = buffer_read_ptr(&conn->respTempBuffer, &maxTempBuffSize);

    /* leave parsed data in buffer_write_ptr(&conn->inTransformBuffer, &maxWriteBuffSize) */
    ptrFromParse = buffer_write_ptr(&conn->inTransformBuffer, &maxWriteBuffSize);

    /* parse string size is min(maxWriteBuffSize, maxTempBuffSize) */
    /* leave bytes parsed in parsedBytes */
    //state = parser_consume(parser, ptrToParse, min(), ptrFromParse, &parsedBytes);

    /* move temp buffer pointer accoring to parsedBytes */
    buffer_read_adv(&conn->respTempBuffer, parsedBytes);

    /* move transform buffer pointer according to parsedBytes */
    buffer_write_adv(&conn->inTransformBuffer, parsedBytes);

    /* set interest */
    /* If I wrote to transformBuffer, writeTransformFd OP_WRITE */
    if(parsedBytes > 1) {
        /* Write response to transform */
        if(selector_set_interest(key->s, conn->writeTransformFd, OP_WRITE) != SELECTOR_SUCCESS) {
            return ERROR;
        }
    }
        
    /* If respTempBuffer is not full and response is not done originFd OP_READ */
    if(buffer_can_read(&conn->respTempBuffer) && !pareserResponseIsDone()) {
        if(selector_set_interest(key->s, conn->originFd, OP_READ) != SELECTOR_SUCCESS) {
            return ERROR;
        }
    }
    
    /* save parser return status */
    return state;
}

/* return 0 on error */
int copyTransformToWriteBuffer(struct selector_key * key) {
    struct Connection * conn = DATA_TO_CONN(key);
	uint8_t * ptrToChunk, * ptrFromChunk;
	size_t maxTransformBuffSize, maxWriteBuffSize;
	int  chunkedBytes;

    /* chunk string from buffer_read_ptr(&conn->outTransformBuff, &maxTransformBuffSize) */
    ptrToChunk = buffer_read_ptr(&conn->outTransformBuffer, &maxTransformBuffSize);

    /* leave chunked data in buffer_write_ptr(&conn->writeBuffer, &maxWriteBuffSize) */
    ptrFromChunk = buffer_write_ptr(&conn->writeBuffer, &maxWriteBuffSize);

    /* chunk string size is min(maxTransformBuffSize, maxTempBuffSize) */
    /* leave amount of bytes chunked in chunkedBytes */
    //chunkBytes(ptrToChunk, min(), ptrFromChunk, &chunkedBytes);

    /* move temp buffer pointer accoring to chunkedBytes */
    buffer_read_adv(&conn->outTransformBuffer, chunkedBytes);

    /* move transform buffer pointer according to chunkedBytes */
    buffer_write_adv(&conn->writeBuffer, chunkedBytes);

    /* set interest */
    /* If I wrote to writeBuffer, clientFd OP_WRITE */
    if(chunkedBytes > 1) {
        /* Write response to transform */
        if(selector_set_interest(key->s, conn->clientFd, OP_WRITE) != SELECTOR_SUCCESS) {
            return 0;
        }
    }
        
    /* If outTransformBuffer is not full and missing bytes readTransformFd OP_READ */
    if(buffer_can_read(&conn->outTransformBuffer) && !pareserResponseIsDone()) {//no va a ser parserResponseDone
        if(selector_set_interest(key->s, conn->readTransformFd, OP_READ) != SELECTOR_SUCCESS) {
            return 0;
        }
    }

    return 1;
}

unsigned readFromOrigin(struct selector_key * key) {
    struct Connection * conn = DATA_TO_CONN(key);
	uint8_t *ptr;
	size_t count;
	ssize_t  n;
    enum parserState state = HEADERS;// buscarlo con funcion del parser
    
    /* Read from origin and save in temp buffer */
	ptr = buffer_write_ptr(&conn->respTempBuffer, &count);
	n = recv(key->fd, ptr, count, 0);
	if(n <= 0) {
        /* origin close connection */
        printf("[ERROR] {response} recv got 0 bytes\n");
        return ERROR;
	}
    buffer_write_adv(&conn->respTempBuffer, n);

    printf("[RESPONSE] got response from origin. Size: %d\n", (int) n);

    /* If im not in body write to writeBuffer */
    /* Always write to writeBuffer if there is no transformation */
    if(conn->trasformationType != NO_TRANSFORM || state == HEADERS) {
        state = copyTempToWriteBuff(key);
    }

    /* If im in body write to inTransformBuffer */
    if(state == BODY) {
        state = copyTempToTransformBuff(key);
    }

    return RESPONSE;
}

unsigned readFromTranformation(struct selector_key * key) {
    struct Connection * conn = DATA_TO_CONN(key);
	uint8_t *ptr;
	size_t count;
	ssize_t  n;
    //enum parserState state = HEADERS;// buscarlo con funcion del parser
    
    /* Read from transformation and save in out transform buffer */
	ptr = buffer_write_ptr(&conn->outTransformBuffer, &count);
	n = recv(key->fd, ptr, count, 0);
	if(n <= 0) {
        /* transformation closed connection */
        printf("[ERROR] {response} recv got 0 bytes\n");
        return ERROR;
	}
    buffer_write_adv(&conn->outTransformBuffer, n);

    printf("[RESPONSE] got response from origin. Size: %d\n", (int) n);

    copyTransformToWriteBuffer(key);

    return RESPONSE;
}

unsigned responseRead(struct selector_key * key) {
    struct Connection * conn = DATA_TO_CONN(key);
	uint8_t *ptr;
	size_t count;
	ssize_t  n;

    /* Check which fd is ready to read (originFd, readTransformFd) */
    if(key->fd == conn->originFd) {
        /* There is data to read form the origin server */
        return readFromOrigin(key);
    } else if(key->fd == conn->readTransformFd) {
        /* Thers is data to read form the transformation */
        return readFromTranformation(key);
    } else {
        /* Error */
        return ERROR;
    }
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