#ifndef _PROXY_PASSIVE_HANDLERS_H_
#define _PROXY_PASSIVE_HANDLERS_H_

#include "../utils/selector/selector.h"
#include "connection-structure.h"

void proxyPassiveAccept(struct selector_key *key);

void destroy_connection(struct Connection * connection);

#endif