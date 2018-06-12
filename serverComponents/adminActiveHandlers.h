#ifndef _ADMIN_ACTIVE_HANDLERS_H_
#define _ADMIN_ACTIVE_HANDLERS_H_

#include "../utils/selector/selector.h"

void adminConn_read   (struct selector_key *key);
void adminConn_write  (struct selector_key *key);
void adminConn_close  (struct selector_key *key);
//void connection_block  (struct selector_key *key);


/* Handlers of fd from an established connection */
static const struct fd_handler adminActiveHandlers = {
	.handle_read   = adminConn_read,
    .handle_write  = adminConn_write,
    .handle_close  = adminConn_close,
    //.handle_block  = connection_block
};


#endif
