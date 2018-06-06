#ifndef _REQUEST_STATE_H_
#define _REQUEST_STATE_H_

#include <sys/types.h>

#include "../../utils/selector/selector.h"

unsigned requestRead(struct selector_key * key);

unsigned requestWrite(struct selector_key * key);

unsigned requestBlockReady(struct selector_key * key);

void requestArrival(const unsigned state, struct selector_key * key);

void requestDeparture(const unsigned state, struct selector_key * key);

#endif