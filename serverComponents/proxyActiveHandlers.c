#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>

#include "proxyActiveHandlers.h"
#include "proxyPassiveHandlers.h"
#include "connection-structure.h"
#include "proxyStm.h"
#include "../utils/selector/selector.h"

void connectionDone(struct selector_key * key);


void connection_read (struct selector_key *key) {
	struct state_machine *stm   = &DATA_TO_CONN(key)->stm;
    const enum proxyStates state = stm_handler_read(stm, key);

    if(state == FATAL_ERROR || state == DONE) {
        connectionDone(key);
    }
}

void connection_write (struct selector_key *key) {
	struct state_machine *stm   = &DATA_TO_CONN(key)->stm;
    const enum proxyStates state = stm_handler_write(stm, key);

    if(state == FATAL_ERROR || state == DONE) {
        connectionDone(key);
    }

}

/* Called when unregistering a fd */
void connection_close (struct selector_key *key) {
    if(key->fd != -1) {
        close(key->fd);
    }
}

void connection_block (struct selector_key *key) {
	struct state_machine *stm   = &DATA_TO_CONN(key)->stm;
    const enum proxyStates state = stm_handler_block(stm, key);

    if(state == FATAL_ERROR || state == DONE) {
        connectionDone(key);
    }

}

void connectionDone(struct selector_key * key) {
    destroy_connection(key);
}
