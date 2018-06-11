#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>

#include "adminActiveHandlers.h"
#include "adminPassiveHandlers.h"
#include "admin-structure.h"
#include "adminStm.h"
#include "../utils/selector/selector.h"

void adminConnDone(struct selector_key * key);


void adminConn_read (struct selector_key *key) {
	struct state_machine *stm   = &DATA_TO_ADMIN(key)->stm;
    const enum adminStates state = stm_handler_read(stm, key);

    if(state == ADMIN_FATAL_ERROR || state == ADMIN_DONE) {
        adminConnDone(key);
    }
}

void adminConn_write (struct selector_key *key) {
	struct state_machine *stm   = &DATA_TO_ADMIN(key)->stm;
    const enum adminStates state = stm_handler_write(stm, key);

    if(state == ADMIN_FATAL_ERROR || state == ADMIN_DONE) {
        adminConnDone(key);
    }

}

/* Called when unregistering a fd */
void adminConn_close (struct selector_key *key) {
    if(key->fd != -1) {
        close(key->fd);
    }
}

void adminConnDone(struct selector_key * key) {
    destroyAdminConnection(key);
}