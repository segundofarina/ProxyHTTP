#ifndef _ERROR_STATE_H_
#define _ERROR_STATE_H_

#include <sys/types.h>

#include "../../utils/selector/selector.h"

enum error_code {
    NO_ERROR,
    BAD_REQUEST_400,
    LENGTH_REQUIRED_411,
    REQ_HEADER_TOO_LONG_431,
    INTERNAL_SERVER_ERR_500,
    NOT_IMPLEMENTED_501,
    BAD_GATEWAY_502,
    GATEWAY_TIMEOUT_504,
    HTTP_VER_ERR_505,
    LOOP_DETECTED_508
};

unsigned errorWrite(struct selector_key * key);

unsigned setError(struct selector_key * key, enum error_code error);

#endif

