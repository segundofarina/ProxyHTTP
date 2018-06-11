#ifndef _AUTHENTICATE_READ_STATE_H_
#define _AUTHENTICATE_READ_STATE_H_

#include <sys/types.h>

#include "../../utils/selector/selector.h"

unsigned authenticateRead(struct selector_key * key);

#endif
