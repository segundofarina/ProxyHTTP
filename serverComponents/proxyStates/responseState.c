#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>

#include <arpa/inet.h>

#include "responseState.h"
#include "errorState.h"

#include "../connection-structure.h"
#include "../proxyStm.h"
#include "../proxyActiveHandlers.h"
#include "../../utils/buffer/buffer.h"
#include "../../logger/logger.h"
#include "../metrics.h"
#include "../../utils/transformation/transformation.h"

#include "../../parser/response_manager.h"

#define MAX_MEDIA_TYPE_SIZE_BUFF 25

int pareserResponseIsDone(struct response_manager parser) {
    return parser.state == manager_done;
}

enum manager_state parser_consume(struct response_manager * parser, char * ptrToParse, int * bytesToParse, char * ptrFromParse, int * parsedBytes) {
    
    enum manager_state state = manager_parser_consume(parser, ptrToParse, bytesToParse, ptrFromParse, parsedBytes);
    return state;
}
/*
void chunkBytes(char * ptrToChunk, int * bytesToChunk, char * ptrFromChunk, int * chunkedBytes) {
    memcpy(ptrFromChunk, ptrToChunk, *bytesToChunk);
    *chunkedBytes = * bytesToChunk;
}
*
int min(int val1, int val2) {
    if(val1 < val2) {
        return val1;
    }
    return val2;
}
*/
int isValidTransformation(struct Connection * conn) {
    char c;
    int n = read(conn->readTransformFd, &c, 1);
    /* If its valid, it outputs with EAGAIN because its non-blockin, and nothing was sent */
    if(n == 0) {
        loggerWrite(PRODUCTION, "\x1b[31m[ERROR]\x1b[0m Invalid transformation command loaded. Applying no transformation.\n");
        return 0;
    }
    if(errno == EAGAIN) {
        return 1;
    }
    return 0;
}

int shouldTransform(struct Connection * conn) {
    if(conn->transformationType == TRANSFORM) {
        /* First time, check if media type is in the list and if content-type is valid and transfer-encoding is valid */
        /* If it is */
        //schar buff[MAX_MEDIA_TYPE_SIZE_BUFF + 1] = {0};
        if(hasMediaTypeInList(conn->mediaTypesList, strToMediaType("text/html")) && isValidTransformation(conn) ) { // CAMBIAR POR LO QUE ME DA EL PARSER
            conn->transformationType = IS_TRANSFORMING;
        } else {
            conn->transformationType = NO_TRANSFORM;
        }
    }

    if(conn->transformationType == IS_TRANSFORMING) {
        return 1;
    }
    return 0;
}

enum manager_state copyTempToWriteBuff(struct selector_key * key) {
    struct Connection * conn = DATA_TO_CONN(key);
	uint8_t * ptrToParse, * ptrFromParse;
	//size_t maxTempBuffSize, maxWriteBuffSize;
	size_t  parsedBytes = 0, bytesToParse = 0;
    enum manager_state state;

    /* parse string from buffer_read_ptr(&conn->respTempBuffer, &maxTempBuffSize) */
    ptrToParse = buffer_read_ptr(&conn->respTempBuffer, &bytesToParse);

    /* leave parsed data in buffer_write_ptr(&conn->writeBuffer, &maxWriteBuffSize) */
    ptrFromParse = buffer_write_ptr(&conn->writeBuffer, &parsedBytes);

    /* parse string size is min(maxWriteBuffSize, maxTempBuffSize) */
    /* leave bytes parsed in parsedBytes */
    //bytesToParse = min(maxWriteBuffSize, maxTempBuffSize);
    state = parser_consume(&conn->responseParser, (char *)ptrToParse, (int *)&bytesToParse, (char *)ptrFromParse, (int *)&parsedBytes);

    /* move temp buffer pointer accoring to parsedBytes */
    buffer_read_adv(&conn->respTempBuffer, bytesToParse);

    /* move write buffer pointer according to parsedBytes */
    buffer_write_adv(&conn->writeBuffer, parsedBytes);

    /* set interest */
    /* If I wrote to writeBuffer, clientFd OP_WRITE */
    fd_interest interest = OP_NOOP;
    if(buffer_can_read(&conn->writeBuffer)) {
        /* Write response to client */
        interest = OP_WRITE;
    }
    if(selector_set_interest(key->s, conn->clientFd, interest) != SELECTOR_SUCCESS) {
        return manager_error;
    }

        
    /* If respTempBuffer is not full and response is not done originFd OP_READ */
    interest = OP_NOOP;
    if(buffer_can_write(&conn->respTempBuffer) && !pareserResponseIsDone(conn->responseParser)) {
        interest = OP_READ;
    }
    if(selector_set_interest(key->s, conn->originFd, interest) != SELECTOR_SUCCESS) {
        return manager_error;
    }

    /* save parser return status */
    return state;
}

enum manager_state copyTempToTransformBuff(struct selector_key * key) {
    struct Connection * conn = DATA_TO_CONN(key);
	uint8_t * ptrToParse, * ptrFromParse;
	//size_t maxTempBuffSize, maxWriteBuffSize;
	size_t  parsedBytes = 0, bytesToParse = 0;
    enum manager_state state;

    /* parse string from buffer_read_ptr(&conn->respTempBuffer, &maxTempBuffSize) */
    ptrToParse = buffer_read_ptr(&conn->respTempBuffer, &bytesToParse);

    /* leave parsed data in buffer_write_ptr(&conn->inTransformBuffer, &maxWriteBuffSize) */
    ptrFromParse = buffer_write_ptr(&conn->inTransformBuffer, &parsedBytes);

    /* parse string size is min(maxWriteBuffSize, maxTempBuffSize) */
    /* leave bytes parsed in parsedBytes */
    //bytesToParse = min(maxTempBuffSize, maxWriteBuffSize);
    state = parser_consume(&conn->responseParser, (char *)ptrToParse, (int *)&bytesToParse, (char *)ptrFromParse, (int *)&parsedBytes);

    /* move temp buffer pointer accoring to parsedBytes */
    buffer_read_adv(&conn->respTempBuffer, bytesToParse);

    /* move transform buffer pointer according to parsedBytes */
    buffer_write_adv(&conn->inTransformBuffer, parsedBytes);

    /* set interest */
    /* If I wrote to transformBuffer, writeTransformFd OP_WRITE */
    fd_interest interest = OP_NOOP;
    if(buffer_can_read(&conn->inTransformBuffer)) {
        /* Write response to transform */
        interest = OP_WRITE;
    }
    if(selector_set_interest(key->s, conn->writeTransformFd, interest) != SELECTOR_SUCCESS) {
        return manager_error;
    }
        
    /* If respTempBuffer is not full and response is not done originFd OP_READ */
    interest = OP_NOOP;
    if(buffer_can_write(&conn->respTempBuffer) && !pareserResponseIsDone(conn->responseParser)) {
        interest = OP_READ;
    }
    if(selector_set_interest(key->s, conn->originFd, interest) != SELECTOR_SUCCESS) {
        return manager_error;
    }
    
    /* save parser return status */
    return state;
}

/* return 0 on error */
int copyTransformToWriteBuffer(struct selector_key * key) {
    struct Connection * conn = DATA_TO_CONN(key);
	uint8_t * ptrToChunk, * ptrFromChunk;
	//size_t maxTransformBuffSize, maxWriteBuffSize;
	size_t  chunkedBytes = 0, bytesToChunk = 0;

    /* chunk string from buffer_read_ptr(&conn->outTransformBuff, &maxTransformBuffSize) */
    ptrToChunk = buffer_read_ptr(&conn->outTransformBuffer, &bytesToChunk);

    /* leave chunked data in buffer_write_ptr(&conn->writeBuffer, &maxWriteBuffSize) */
    ptrFromChunk = buffer_write_ptr(&conn->writeBuffer, &chunkedBytes);

    /* chunk string size is min(maxTransformBuffSize, maxWriteBuffSize) */
    /* leave amount of bytes chunked in chunkedBytes */
    //bytesToChunk = min(maxTransformBuffSize, maxWriteBuffSize);
    chunkBody((char *)ptrToChunk, (int *)&bytesToChunk, (char *)ptrFromChunk, (int *)&chunkedBytes);

    /* move temp buffer pointer accoring to chunkedBytes */
    buffer_read_adv(&conn->outTransformBuffer, bytesToChunk);

    /* move transform buffer pointer according to chunkedBytes */
    buffer_write_adv(&conn->writeBuffer, chunkedBytes);

    /* set interest */
    /* If I wrote to writeBuffer, clientFd OP_WRITE */
    fd_interest interest = OP_NOOP;
    if(buffer_can_read(&conn->writeBuffer)) {
        /* Write response to transform */
        interest = OP_WRITE;
    }
    if(selector_set_interest(key->s, conn->clientFd, interest) != SELECTOR_SUCCESS) {
        return 0;
    }
        
    /* If outTransformBuffer is not full and missing bytes readTransformFd OP_READ */
    if(conn->readTransformFd != -1) {
        interest = OP_NOOP;
        if(buffer_can_write(&conn->outTransformBuffer) && conn->readTransformFd != -1) {
            interest = OP_READ;
        }
        if(selector_set_interest(key->s, conn->readTransformFd, interest) != SELECTOR_SUCCESS) {
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
    enum manager_state state = conn->responseParser.state;
    
    /* Read from origin and save in temp buffer */
	ptr = buffer_write_ptr(&conn->respTempBuffer, &count);
	n = recv(key->fd, ptr, count, 0);
	if(n <= 0) {
        /* origin close connection */
        loggerWrite(DEBUG, "[ERROR] receved 0 bytes from origin\n");
        /* Show error to client if server never answerd */
        if(!conn->originHasAnswered) {
            return setError(key, BAD_GATEWAY_502);
        }
        return FATAL_ERROR;
	}
    buffer_write_adv(&conn->respTempBuffer, n);

    /* origin has sent a response */
    conn->originHasAnswered = 1;

    /* If im not in body write to writeBuffer */
    /* Always write to writeBuffer if there is no transformation */
    if(!shouldTransform(conn) || state == manager_statusLine || state == manager_headers || state == manager_addingHeaders) {
        state = copyTempToWriteBuff(key);
        if(state == manager_error) {
            return setError(key, INTERNAL_SERVER_ERR_500);
        }
    }

    /* If im in body write to inTransformBuffer */
    if(state == manager_body) {
        if(!shouldTransform(conn)) {
            state = copyTempToWriteBuff(key);
        } else {
            state = copyTempToTransformBuff(key);
        }
        if(state == manager_error) {
            return setError(key, INTERNAL_SERVER_ERR_500);
        }
    }

    return RESPONSE;
}

unsigned readFromTranformation(struct selector_key * key) {
    struct Connection * conn = DATA_TO_CONN(key);
	uint8_t *ptr;
	size_t count;
	ssize_t  n;

    /* Read from transformation and save in out transform buffer */
	ptr = buffer_write_ptr(&conn->outTransformBuffer, &count);
	n = read(key->fd, ptr, count);
    if(n == 0) {
        /* transformation closed connection. unregister it from the selector and close it */
        if(selector_unregister_fd(key->s, key->fd) != SELECTOR_SUCCESS) {
            return setError(key, INTERNAL_SERVER_ERR_500);
        }
        
        conn->readTransformFd = -1;
    }
	if(n < 0) {
        loggerWrite(DEBUG, "[ERROR] Receive got < 0 bytes from transformation\n");
        return setError(key, INTERNAL_SERVER_ERR_500);
	}
    buffer_write_adv(&conn->outTransformBuffer, n);

    if(!copyTransformToWriteBuffer(key)) {
        return setError(key, INTERNAL_SERVER_ERR_500);
    }

    if(pareserResponseIsDone(conn->responseParser) && !buffer_can_read(&conn->writeBuffer) && (conn->transformationType == NO_TRANSFORM || conn->readTransformFd == -1) ) {
        loggerWrite(DEBUG, "Response is done\n");
        return DONE;
    }

    return RESPONSE;
}

unsigned responseRead(struct selector_key * key) {
    struct Connection * conn = DATA_TO_CONN(key);

    /* Check which fd is ready to read (originFd, readTransformFd) */
    if(key->fd == conn->originFd) {
        /* There is data to read form the origin server */
        return readFromOrigin(key);
    } else if(key->fd == conn->readTransformFd) {
        /* Thers is data to read form the transformation */
        return readFromTranformation(key);
    } else {
        /* Error */
        return setError(key, INTERNAL_SERVER_ERR_500);
    }
}

unsigned writeToClient(struct selector_key * key) {
    struct Connection * conn = DATA_TO_CONN(key);
	uint8_t *ptr;
	size_t count;
	ssize_t  n;
    enum manager_state state = conn->responseParser.state;
    enum manager_state originalState = state;

    /* Send bufferd data to the client */
	ptr = buffer_read_ptr(&conn->writeBuffer, &count);
	n = send(key->fd, ptr, count, MSG_NOSIGNAL);
	if(n <= 0) { // transformation closed connection

        loggerWrite(DEBUG, "[ERROR] Send returned 0 when sending to client\n");
    
        return setError(key, INTERNAL_SERVER_ERR_500);
	}
    buffer_read_adv(&conn->writeBuffer, n);

    /* Save value for metrics */
    addBytesSent(n);

    /* Copy from temp if it's on headers or no transformation */
    if(!shouldTransform(conn) || state == manager_headers || state == manager_statusLine || state == manager_addingHeaders) {
        state = copyTempToWriteBuff(key);
        if(state == manager_error) {
            return setError(key, INTERNAL_SERVER_ERR_500);
        }
    }

    /* I have not enterd in the above if and I'm in the body */
    if(shouldTransform(conn) && originalState == manager_body) {
        if(!copyTransformToWriteBuffer(key)) {
            return setError(key, INTERNAL_SERVER_ERR_500);
        }
    }

    /* If the parser changed state to body and there is no transformation */
    if(shouldTransform(conn) && originalState != manager_body && state == manager_body) {
        if(copyTempToTransformBuff(key) == manager_error) {
            return setError(key, INTERNAL_SERVER_ERR_500);
        }
    }

    /* Fix parser stop reading when changing to body */
    if(!shouldTransform(conn) && state == manager_body) {
        if(copyTempToWriteBuff(key) == manager_error) {
            return setError(key, INTERNAL_SERVER_ERR_500);
        }
    }

    /* Set interests */
    /* If writeBuffer is not empty set me on WRITE */
    fd_interest interest = OP_NOOP;
    if(buffer_can_read(&conn->writeBuffer)) {
        interest = OP_WRITE;
    }
    if(selector_set_interest_key(key, interest) != SELECTOR_SUCCESS) {
        return setError(key, INTERNAL_SERVER_ERR_500);
    }

    /* Check if it's done */
    if(pareserResponseIsDone(conn->responseParser) && !buffer_can_read(&conn->writeBuffer) && (conn->transformationType == NO_TRANSFORM || conn->readTransformFd == -1) ) {
        loggerWrite(DEBUG, "Response is done\n");
        return DONE;
    }
    
    return RESPONSE;
}

unsigned writeToTransformation(struct selector_key * key) {
    struct Connection * conn = DATA_TO_CONN(key);
	uint8_t *ptr;
	size_t count;
	ssize_t  n;

    /* Send bufferd data to the client */
	ptr = buffer_read_ptr(&conn->inTransformBuffer, &count);
	n = send(key->fd, ptr, count, MSG_NOSIGNAL);
    printf("count is: %d\n", (int) count);
    n = write(key->fd, ptr, count);
	if(n <= 0) { // transformation closed connection

        loggerWrite(DEBUG, "[ERROR] write to transformation returned 0\n");
    
        return FATAL_ERROR; // since I have already sent the headers
	}
    buffer_read_adv(&conn->inTransformBuffer, n);

    /* I have free space in buffer, copy tempBuffer if it's not empty */
    if(copyTempToTransformBuff(key) == manager_error) {
        return setError(key, INTERNAL_SERVER_ERR_500);
    }

    /* If response is done and buffer is empty close writeTransformFd */
    if(pareserResponseIsDone(conn->responseParser) && !buffer_can_read(&conn->inTransformBuffer)) {
        if(selector_unregister_fd(key->s, conn->writeTransformFd) != SELECTOR_SUCCESS) {
            return setError(key, INTERNAL_SERVER_ERR_500);
        }
        conn->writeTransformFd = -1;
    }

    /* Set interests */
    /* readTransformFd can read */
    if(selector_set_interest(key->s, conn->readTransformFd, OP_READ) != SELECTOR_SUCCESS) {
        return setError(key, INTERNAL_SERVER_ERR_500);
    }

    /* Avoid this if I have closed the fd */
    if(conn->writeTransformFd != -1) {
        /* if buffer is not empty I can write to transform */
        fd_interest interest = OP_NOOP;
        if(buffer_can_read(&conn->inTransformBuffer)) {
            interest = OP_WRITE;
        }
        if(selector_set_interest_key(key, interest) != SELECTOR_SUCCESS) {
            return setError(key, INTERNAL_SERVER_ERR_500);
        }
    }

    return RESPONSE;
}

unsigned responseWrite(struct selector_key * key) {
    struct Connection * conn = DATA_TO_CONN(key);

    /* Check wich f I'm writing to */
    if(key->fd == conn->clientFd) {
        /* Write to client */
        return writeToClient(key);
    } else if(key->fd == conn->writeTransformFd) {
        /* Write to the transformation */
        return writeToTransformation(key);
    } else {
        /* Error */
        return setError(key, INTERNAL_SERVER_ERR_500);
    }
}

void responseArrival(unsigned state, struct selector_key * key) {
    struct Connection * conn = DATA_TO_CONN(key);
    manager_parser_init(&conn->responseParser, conn->requestParser.reqParser.method);
}

void responseDeparture(unsigned state, struct selector_key * key) {
    struct Connection * conn = DATA_TO_CONN(key);
    manager_parser_close(&conn->responseParser);
}
