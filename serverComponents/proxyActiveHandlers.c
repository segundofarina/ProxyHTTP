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

    if(state == ERROR || state == DONE) {
        connectionDone(key);
    }
}

void connection_write (struct selector_key *key) {
	struct state_machine *stm   = &DATA_TO_CONN(key)->stm;
    const enum proxyStates state = stm_handler_write(stm, key);

    if(state == ERROR || state == DONE) {
        connectionDone(key);
    }

}

void connection_close (struct selector_key *key) {
	destroy_connection(DATA_TO_CONN(key));
}

void connection_block (struct selector_key *key) {
	struct state_machine *stm   = &DATA_TO_CONN(key)->stm;
    const enum proxyStates state = stm_handler_block(stm, key);

    if(state == ERROR || state == DONE) {
        connectionDone(key);
    }

}

void connectionDone(struct selector_key * key) {
    unsigned int i;

	const int fds[] = {
        DATA_TO_CONN(key)->clientFd,
        DATA_TO_CONN(key)->originFd,
    };

    for(i = 0; i < N(fds); i++) {
        if(fds[i] != -1) {
            if(selector_unregister_fd(key->s, fds[i]) != SELECTOR_SUCCESS) {
                abort();
				//handle better this error
            }
            close(fds[i]);
        }
    }
}