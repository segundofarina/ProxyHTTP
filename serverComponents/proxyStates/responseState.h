#ifndef _RESPONSE_STATE_H_
#define _RESPONSE_STATE_H_

#include <sys/types.h>

#include "../../utils/selector/selector.h"

unsigned responseRead(struct selector_key * key);

unsigned responseWrite(struct selector_key * key);

void responseArrival(unsigned state, struct selector_key * key);

void responseDeparture(unsigned state, struct selector_key * key);

#endif
