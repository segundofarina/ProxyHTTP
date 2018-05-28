#ifndef _PROXY_ACTIVE_HANDLERS_
#define _PROXY_ACTIVE_HANDLERS_

#include "../utils/selector/selector.h"

void connection_read   (struct selector_key *key);
void connection_write  (struct selector_key *key);
void connection_close  (struct selector_key *key);
void connection_block  (struct selector_key *key);

void connection_close  (struct selector_key *key);


/* Handlers of fd from an established connection */
static const struct fd_handler connectionHandler = {
	.handle_read   = connection_read,
    .handle_write  = connection_write,
    .handle_close  = connection_close,
    .handle_block  = connection_block
};

#endif