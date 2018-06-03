#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <signal.h>

#include "proxyPassiveHandlers.h"
#include "proxyActiveHandlers.h"
#include "proxyStm.h"

#include "../utils/buffer/buffer.h"
#include "transformationFork.h"

#define MAX_POOL 50

static int poolSize = 0;
static struct Connection * pool = NULL;

static enum TransformationType transformationType = NO_TRANSFORM;

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

	connection->stm.initial = REQUEST;
	connection->stm.max_state = ERROR;
	connection->stm.states = getProxyStates();
	stm_init(&connection->stm);

	buffer_init(&connection->readBuffer,  N(connection->rawBuff_a), connection->rawBuff_a);
    buffer_init(&connection->writeBuffer, N(connection->rawBuff_b), connection->rawBuff_b);

	buffer_init(&connection->respTempBuffer, N(connection->rawBuff_c), connection->rawBuff_c);
	
	/* transformation */
	connection->trasformationType = transformationType;
	connection->transformationPid = -1;
	connection->readTransformFd = -1;
	connection->writeTransformFd = -1;

	if(connection->trasformationType != NO_TRANSFORM) {
		buffer_init(&connection->inTransformBuffer, N(connection->rawBuff_d), connection->rawBuff_d);
		buffer_init(&connection->outTransformBuffer, N(connection->rawBuff_e), connection->rawBuff_e);

		/* Fork transformation process and create pipes */
		connection->transformationPid = forkTransformation(&connection->readTransformFd, &connection->writeTransformFd);
		if(connection->transformationPid == -1) {
			return NULL;
		}
	}
	
	connection->references = 1;

	return connection;
}

void freeConnection(struct Connection * connection) {
	// free origin_resolution
	free(connection);
}

void closeAndUnregister(struct selector_key * key, int fd) {
	selector_unregister_fd(key->s, fd);
	/* fd is closed by connection_close handler */
}

/* removes a connection, if there are < MAX_POOL its added to the pool, otherwise it frees the memory */
void destroy_connection(struct selector_key * key) {
	struct Connection * connection = DATA_TO_CONN(key);
	if(connection == NULL) {
		return;
	}

	// Close transformation process 
	if(connection->transformationPid != -1) {
		kill(connection->transformationPid, SIGKILL);
	}

	if(connection->clientFd != -1) {
		closeAndUnregister(key, connection->clientFd);
		connection->clientFd = -1;
	}

	if(connection->originFd != -1) {
		closeAndUnregister(key, connection->originFd);
		connection->originFd = -1;
	}

	if(connection->readTransformFd != -1) {
		closeAndUnregister(key, connection->readTransformFd);
		connection->readTransformFd = -1;
	}

	if(connection->writeTransformFd != -1) {
		closeAndUnregister(key, connection->writeTransformFd);
		connection->writeTransformFd = -1;
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
        goto handle_errors;
    }
    if(selector_fd_set_nio(clientFd) == -1) {
        goto handle_errors;
    }

	connection = new_connection(clientFd);
	if(connection == NULL) {
		goto handle_errors;
	}
	memcpy(&connection->clientAddr, &clientAddr, clientAddrLen);
    connection->clientAddrLen = clientAddrLen;

	/* register client fd in selector */
	if(selector_register(key->s, clientFd, &connectionHandler, OP_READ, connection) != SELECTOR_SUCCESS) {
		goto handle_errors;
	}
	/* register transformation fds */
	if(connection->trasformationType != NO_TRANSFORM) {
		if(selector_fd_set_nio(connection->readTransformFd) == -1 || selector_fd_set_nio(connection->writeTransformFd) == -1) {
			goto handle_errors;
		}
		if(selector_register(key->s, connection->readTransformFd, &connectionHandler, OP_NOOP, connection) != SELECTOR_SUCCESS) {
			goto handle_errors;
		}
		if(selector_register(key->s, connection->writeTransformFd, &connectionHandler, OP_NOOP, connection) != SELECTOR_SUCCESS) {
			goto handle_errors;
		}
	}

	return;

	/* Used only as exception handler */
	handle_errors:

		if(clientFd != -1) {
			// send error to client
			close(clientFd);
		}

		destroy_connection(key);

}