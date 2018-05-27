#ifndef _OBTAIN_ORIGIN_STATE_H_
#define _OBTAIN_ORIGIN_STATE_H_

#include "../../utils/selector/selector.h"

unsigned obtainOriginRead(struct selector_key * key);

unsigned writeEcho(struct selector_key * key);

#endif