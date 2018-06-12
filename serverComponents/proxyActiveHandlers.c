#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>

#include "proxyActiveHandlers.h"
#include "proxyPassiveHandlers.h"
#include "connection-structure.h"
#include "proxyStm.h"
#include "../utils/selector/selector.h"
#include "../logger/logger.h"

#define LOG_BUFF_SIZE 256

void connectionDone(struct selector_key * key);

void logCloseConnection(const enum proxyStates state, struct Connection * conn) {
    char buff[LOG_BUFF_SIZE + 1] = {0};
    char addr[INET_ADDRSTRLEN + 1] = {0};

    /* Get addres */
    inet_ntop(conn->clientAddr.ss_family, &(((struct sockaddr_in *)&conn->clientAddr)->sin_addr), addr, INET_ADDRSTRLEN);

    if(state == FATAL_ERROR) {
        sprintf(buff, "\x1b[31m[ERROR]\x1b[0m There was an error, and client connection was closed - client: %s\n", addr);
    }
    if(state == DONE) {
        sprintf(buff, "\x1b[32m[INFO]\x1b[0m Client connection closed - client: %s\n", addr);
    }
    loggerWrite(PRODUCTION, buff);
}

void connection_read (struct selector_key *key) {
	struct state_machine *stm   = &DATA_TO_CONN(key)->stm;
    const enum proxyStates state = stm_handler_read(stm, key);

    if(state == FATAL_ERROR || state == DONE) {
        logCloseConnection(state, DATA_TO_CONN(key));
        connectionDone(key);
    }
}

void connection_write (struct selector_key *key) {
	struct state_machine *stm   = &DATA_TO_CONN(key)->stm;
    const enum proxyStates state = stm_handler_write(stm, key);

    if(state == FATAL_ERROR || state == DONE) {
        logCloseConnection(state, DATA_TO_CONN(key));
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
        logCloseConnection(state, DATA_TO_CONN(key));
        connectionDone(key);
    }

}

void connectionDone(struct selector_key * key) {
    destroy_connection(key);
}
