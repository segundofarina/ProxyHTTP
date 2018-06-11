#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

#include "adminPassiveHandlers.h"
#include "../utils/buffer/buffer.h"
#include "admin-structure.h"
#include "adminStm.h"
#include "adminActiveHandlers.h"
#include "maxFdHandler.h"

#include "../logger/logger.h"

#define ADMIN_MAX_POOL 50

static int adminPoolSize = 0;
static struct AdminConn * adminPool = NULL;
//static int spareFd = -1;

struct AdminConn * newAdminConnection(const int adminFd) {
    struct AdminConn * conn;

	if(adminPool == NULL) {
		conn = malloc(sizeof(*conn));
	} else {
		conn = adminPool;
		adminPool = adminPool->next;
		adminPoolSize--;
		conn->next = NULL;
	}
	if(conn == NULL) {
		return NULL;
	}

	/* Initialize connection to 0 */
	memset(conn, 0x00, sizeof(*conn));
	conn->adminFd = adminFd;
	
	conn->stm.initial = AUTHENTICATE_READ;
	conn->stm.max_state = ADMIN_FATAL_ERROR;
	conn->stm.states = getAdminStates();
	stm_init(&conn->stm);
	buffer_init(&conn->buff,  N(conn->rawBuff_a), conn->rawBuff_a);
	/* Still no error */
	//conn->errorCode = NO_ERROR;
	
	conn->references = 1;
	return conn;
}

void adminPassiveAccept(struct selector_key * key) {
    struct sockaddr_storage clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
	struct AdminConn * conn = NULL;
	struct selector_key errorKey;

	loggerWrite(DEBUG, "New admin connection\n");

	keepSpareFd();

	const int adminFd = accept(key->fd, (struct sockaddr*) &clientAddr, &clientAddrLen);
    if(adminFd == -1) {
		if(errno == EMFILE) {
			/* There are no available fd so close connection */
			handleAcceptEmfile(key->fd, (struct sockaddr*) &clientAddr, &clientAddrLen);
		}
        goto handle_errors;
    }
    if(selector_fd_set_nio(adminFd) == -1) {
        goto handle_errors;
    }

	conn = newAdminConnection(adminFd);
	if(conn == NULL) {
		goto handle_errors;
	}
	memcpy(&conn->clientAddr, &clientAddr, clientAddrLen);
    conn->clientAddrLen = clientAddrLen;

	/* register admin fd in selector */
	if(selector_register(key->s, adminFd, &adminActiveHandlers, OP_READ, conn) != SELECTOR_SUCCESS) {
		goto handle_errors;
	}

	return;

	/* Used only as exception handler */
	handle_errors:
		if(conn == NULL && adminFd != -1) {
			//it was never registerd to the selector
			close(adminFd);
			return;
		}

		errorKey.s = key->s;
		errorKey.fd = key->fd;
		errorKey.data = conn;

		destroyAdminConnection(&errorKey);
}

void destroyAdminConnection(struct selector_key * key) {
    struct AdminConn * conn = DATA_TO_ADMIN(key);
	
	loggerWrite(DEBUG, "Closing admin connection\n");
	
	if(conn == NULL) {
		return;
	}

	if(conn->adminFd != -1) {
		selector_unregister_fd(key->s, conn->adminFd);
		conn->adminFd = -1;
	}

	if(conn->references > 1) {
		conn->references -= 1;
		return;
	}

	if(adminPoolSize < ADMIN_MAX_POOL) {
		conn->next = adminPool;
		adminPool = conn;
		adminPoolSize++;
	} else {
		free(conn);
	}
}