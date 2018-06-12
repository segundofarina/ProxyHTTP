#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "adminWriteState.h"

#include "../admin-structure.h"
#include "../adminStm.h"
#include "../adminActiveHandlers.h"
#include "../../utils/buffer/buffer.h"
#include "../../parser/request.h"

unsigned adminWrite(struct selector_key * key) {
    struct AdminConn * conn = DATA_TO_ADMIN(key);
    uint8_t *ptr;
    size_t count;
    ssize_t n;

    ptr = buffer_read_ptr(&conn->buff, &count);
    n = send(conn->adminFd, ptr, count, MSG_NOSIGNAL);
    if(n <= 0) {
        /* Admin closed connection */
        return ADMIN_FATAL_ERROR;
    }
    buffer_read_adv(&conn->buff, n);

    if(selector_set_interest_key(key, OP_READ) != SELECTOR_SUCCESS) {
        return ADMIN_ERROR;
    }

    return ADMIN_READ;
}