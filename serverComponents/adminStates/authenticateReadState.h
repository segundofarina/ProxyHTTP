#ifndef _AUTHENTICATE_READ_STATE_H_
#define _AUTHENTICATE_READ_STATE_H_

#include <sys/types.h>

#include "../../utils/selector/selector.h"
#define PASSWORD "dulcedeleche"


enum auth_response{
    LOGGED_IN=0x00,
    LOGIN_FAILED=0x01
};

unsigned authenticateRead(struct selector_key * key);

#endif
