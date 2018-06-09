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
        return ADMIN_FATAL_ERROR;
	}

    buffer_read_adv(&conn->buff, n);

    return ADMIN_FATAL_ERROR;
}

int writeAdminErrorToBuff(uint8_t * ptr, const int count, enum admin_error_code error) {
    if(count == 0){
        return -1;
    }
    ptr[0] = error;
    return 1;
}

unsigned adminSetError(struct selector_key * key, enum admin_error_code error) {
    struct AdminConn * conn = DATA_TO_ADMIN(key);
    uint8_t *ptr;
	size_t count;
	int n;

    if(conn->adminFd == -1) {
        return ADMIN_FATAL_ERROR;
    }

    buffer_reset(&conn->buff);
    
    ptr = buffer_write_ptr(&conn->buff, &count);
    n = writeAdminErrorToBuff(ptr, count, error);
    buffer_write_adv(&conn->buff, n);

    selector_set_interest(key->s, conn->adminFd, OP_WRITE);
    
    return ADMIN_ERROR;
}