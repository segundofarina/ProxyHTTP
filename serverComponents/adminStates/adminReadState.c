#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "adminReadState.h"
#include "adminErrorState.h"

#include "../admin-structure.h"
#include "../adminStm.h"
#include "../adminActiveHandlers.h"
#include "../../utils/buffer/buffer.h"
#include "../../parser/request.h"
#include "../metrics.h"
#include "../transformationManager.h"
#include "adminParser/processAdminRequest.h"

unsigned adminRead(struct selector_key * key) {
    struct AdminConn * conn = DATA_TO_ADMIN(key);
    uint8_t *ptr;
    size_t count;
    ssize_t n;

    ptr = buffer_write_ptr(&conn->buff, &count);
    n = recv(conn->adminFd, ptr, count, 0);
    if(n <= 0) {
        /* Admin closed connection */
        return ADMIN_FATAL_ERROR;
    }
    buffer_write_adv(&conn->buff, n);

    enum admin_error_code error = processAdminRequest(&conn->buff);

    if( error == ADMIN_NO_ERROR) {
        if(selector_set_interest_key(key, OP_WRITE) != SELECTOR_SUCCESS) {
            return ADMIN_FATAL_ERROR;
        }
        return ADMIN_WRITE;
    }

    return adminSetError(key, error);
}
