#ifndef _ADMIN_ERROR_STATE_H_
#define _ADMIN_ERROR_STATE_H_

#include <sys/types.h>

#include "../../utils/selector/selector.h"

enum admin_error_code {
    ADMIN_NO_ERROR = 0x00,
    ADMIN_REQ_ERR = 0x01,
    ADMIN_NOT_SUPPORTED_ERROR = 0x02,
    ADMIN_INTERNAL_SERVER_ERR = 0xFF
};

unsigned adminErrorWrite(struct selector_key * key);

unsigned adminSetError(struct selector_key * key, enum admin_error_code error);

#endif
