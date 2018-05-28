#ifndef _REQUEST_STATE_H_
#define _REQUEST_STATE_H_

#include "../../utils/selector/selector.h"

unsigned requestRead(struct selector_key * key);

unsigned requestWrite(struct selector_key * key);

unsigned requestBlockReady(struct selector_key * key);

#endif