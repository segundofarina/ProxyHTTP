#ifndef _AUTHENTICATE_WRITE_STATE_H_
#define _AUTHENTICATE_WRITE_STATE_H_

#include <sys/types.h>

#include "../../utils/selector/selector.h"

unsigned authenticateWrite(struct selector_key * key);

#endif
