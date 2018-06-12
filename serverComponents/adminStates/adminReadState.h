#ifndef _ADMIN_READ_STATE_H_
#define _ADMIN_READ_STATE_H_

#include <sys/types.h>

#include "../../utils/selector/selector.h"

unsigned adminRead(struct selector_key * key);

#endif
