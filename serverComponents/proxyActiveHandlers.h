#ifndef _PROXY_ACTIVE_HANDLERS_
#define _PROXY_ACTIVE_HANDLERS_

#include "../utils/selector/selector.h"

void connection_read   (struct selector_key *key);
void connection_write  (struct selector_key *key);
void connection_close  (struct selector_key *key);
void connection_block  (struct selector_key *key);

#endif