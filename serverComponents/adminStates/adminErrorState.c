#include <string.h>
#include <stdio.h>

#include "adminErrorState.h"
#include "../adminStm.h"
#include "../admin-structure.h"

unsigned adminErrorWrite(struct selector_key * key) {
    struct AdminConn * conn = DATA_TO_ADMIN(key);
	uint8_t *ptr;
	size_t count;
	ssize_t  n;

    ptr = buffer_read_ptr(&conn->buff, &count);
	n = send(key->fd, ptr, count, MSG_NOSIGNAL);
    
    if(n <= 0) { // client closed connection
        //printf("client closed connection\n");
        return ADMIN_FATAL_ERROR;
	}

    buffer_read_adv(&conn->buff, n);

    return ADMIN_FATAL_ERROR;
}

int writeAdminErrorToBuff(char * ptr, const int count, enum admin_error_code error) {
    memcpy(ptr, "ADMIN_ERROR", 11);
    ptr[11] = 0;
    return 11;
}

unsigned adminSetError(struct selector_key * key, enum admin_error_code error) {
    struct AdminConn * conn = DATA_TO_ADMIN(key);
    uint8_t *ptr;
	size_t count;
	int n;

    if(conn->adminFd == -1) {
        //printf("adminFd == -1\n");
        return ADMIN_FATAL_ERROR;
    }

    //conn->errorCode = error;
    buffer_reset(&conn->buff);
    
    ptr = buffer_write_ptr(&conn->buff, &count);
    n = writeAdminErrorToBuff((char *)ptr, count, error);
    buffer_write_adv(&conn->buff, n);

    selector_set_interest(key->s, conn->adminFd, OP_WRITE);
    
    return ADMIN_ERROR;
}