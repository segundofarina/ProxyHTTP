#include <stdio.h>
#include <arpa/inet.h>

#include "proxyPassiveHandlers.h"
#include "connection-structure.h"

#define MAX_POOL 50

/* Handlers of fd from an established connection */
static void connection_read   (struct selector_key *key);
static void connection_write  (struct selector_key *key);
static void connection_close  (struct selector_key *key);
static void connection_block  (struct selector_key *key);

static const struct fd_handler connectionHandler = {
	.handle_read   = connection_read,
    .handle_write  = connection_write,
    .handle_close  = connection_close,
    .handle_block  = connection_block
};

static int poolSize = 0;
static struct Connection * pool = NULL;


struct Connection * new_connection(const int clientFd) {
	struct Connection * connection;

	if(pool == NULL) {
		connection = malloc(sizeof(* connection));
	} else {
		connection = pool;
		pool = pool->next;
		poolSize--;
		connection->next = NULL;
	}

	if(connection == NULL) {
		return NULL;
	}

	/* Initialize connection to 0 */
	memset(connection, 0x00, sizeof(*connection));

	connection->clientFd = clientFd;
	connection->originFd = -1;

	//connection->stm.initial = ;
	//connection->stm.max_state = ;
	//connection->stm.states = ;
	stm_init(&connection->stm);

	buffer_init(&connection->readBuffer,  N(connection->rawBuff_a), connection->rawBuff_a);
    buffer_init(&connection->writeBuffer, N(connection->rawBuff_b), connection->rawBuff_b);

	connection->references = 1;

	return connection;
}

void freeConnection(struct Connection * connection) {
	// free origin_resolution
	free(connection);
}

/* removes a connection, if there are < MAX_POOL its added to the pool, otherwise it frees the memory */
void destroy_connection(struct Connection * connection) {
	if(connection == NULL) {
		return;
	}

	if(connection->references > 1) {
		connection->references -= 1;
		return;
	}

	if(poolSize < MAX_POOL) {
		connection->next = pool;
		pool = connection;
		poolSize++;
	} else {
		freeConnection(connection);
	}
}


void proxyPassiveAccept(struct selector_key *key) {
	struct sockaddr_storage clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
	struct Connection * connection = NULL;

	const int clientFd = accept(key->fd, (struct sockaddr*) &clientAddr, &clientAddrLen);
    if(clientFd == -1) {
        // handle error
    }
    if(selector_fd_set_nio(clientFd) == -1) {
        // handle error
    }

	connection = new_connection(clientFd);
	if(connection == NULL) {
		//handle error
	}
	memcpy(&connection->clientAddr, &clientAddr, clientAddrLen);
    connection->clientAddrLen = clientAddrLen;

	/* register client fd in selector */
	if(selector_register(key->s, clientFd, &connectionHandler, OP_READ, connection) != SELECTOR_SUCCESS) {
		//handle error
	}

	return;

	/* Used only as exception handler */
	handle_errors:

		if(clientFd != -1) {
			// send error to client
			//close(clientFd);
		}

		destroy_connection(connection);

}