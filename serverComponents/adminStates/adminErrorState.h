#ifndef _ADMIN_ERROR_STATE_H_
#define _ADMIN_ERROR_STATE_H_

#include <sys/types.h>

#include "../../utils/selector/selector.h"

enum admin_error_code {
    ADMIN_NO_ERROR,
    ADMIN_INTERNAL_SERVER_ERR,
    ADMIN_REQ_ERR
};

unsigned adminErrorWrite(struct selector_key * key);

unsigned adminSetError(struct selector_key * key, enum admin_error_code error);

#endif