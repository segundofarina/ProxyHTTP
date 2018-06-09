#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>

#include "utils/selector/selector.h"
#include "serverComponents/proxyPassiveHandlers.h"
#include "serverComponents/adminPassiveHandlers.h"
#include "logger/logger.h"

typedef enum {FALSE, TRUE} Bool;

#define MAX_CLIENTS 10
#define SELECTOR_SIZE 1024

#define LOGGER_LEVEL DEBUG

int createPassiveSock(const int port, const int protocol);


//int main(const int argc, const char * argv[]) {
int main() {

	int port = 1080, serverFd, adminPort = 1081, adminFd;
	char * errMsg = NULL;
	selector_status selectorStatus = SELECTOR_SUCCESS;
    fd_selector selector;

	/* Selector conf structure */
	const struct selector_init conf = {
        .signal = SIGALRM,
        .select_timeout = {
            .tv_sec  = 10,
            .tv_nsec = 0,
        }
    };

	/* Proxy socket handler */
	const struct fd_handler proxyPassiveHandlers = {
        .handle_read = proxyPassiveAccept,
        .handle_write = NULL,
        .handle_close = NULL
    };

	/* Admin socket handler */
	const struct fd_handler adminPassiveHandlers = {
		.handle_read = adminPassiveAccept,
		.handle_write = NULL,
		.handle_close = NULL
	};

	/* Podemos cerrar la entrada estandard */
	close(0);

	/* Selector init */
	if(selector_init(&conf) != 0) {
		/* Handle error */
        errMsg = "Unable to start selector";
		goto error_handler;
	}

	if( (selector = selector_new(SELECTOR_SIZE)) == NULL ) {
		/* Handle error */
		errMsg = "Unable to create new selector";
		goto error_handler;
	}

	/* Start logger */
	loggerInit(LOGGER_LEVEL, selector);

	/* Listen to clients */
	if( (serverFd = createPassiveSock(port, IPPROTO_TCP)) < 0 ) {
		/* Handle error */
		errMsg = "Error creating the passive socket for http requests, check if the port is not already in use";
		goto error_handler;
	}

	selectorStatus = selector_register(selector, serverFd, &proxyPassiveHandlers, OP_READ, NULL);
    if(selectorStatus != SELECTOR_SUCCESS) {
        /* Handle error */
		errMsg = "Unable to register proxy passive socket fd in selector";
		goto error_handler;
		
    }

	/* Listen to admin */
	if( (adminFd = createPassiveSock(adminPort, IPPROTO_SCTP)) < 0 ) {
		/* Handle error */
		errMsg = "Error creating the passive socket for admin requests, check if the port is not already in use";
		goto error_handler;
	}

	selectorStatus = selector_register(selector, adminFd, &adminPassiveHandlers, OP_READ, NULL);
    if(selectorStatus != SELECTOR_SUCCESS) {
        /* Handle error */
		errMsg = "Unable to register admin passive socket fd";
		goto error_handler;
		
    }

	loggerWrite(PRODUCTION, "\x1b[32m[INFO]\x1b[0m Proxy started and waiting for new connections\n");

    while(TRUE) {
        selectorStatus = selector_select(selector);
        if(selectorStatus != SELECTOR_SUCCESS) {
			/* Handle error */
		    errMsg = "Error in selector";
			goto error_handler;
        }
    }

	return 0;

	error_handler:
		printf("\x1b[31m[FATAL ERROR]\x1b[0m %s\n", errMsg);
		return 1;
}

/* Returns socket fd or -1 if error */
int createPassiveSock(const int port, const int protocol) {
	struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port        = htons(port);

	const int serverFd = socket(AF_INET, SOCK_STREAM, protocol);
	if(serverFd < 0) {
		return -1;
	}

	setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));

	if(bind(serverFd, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
		return -1;
	}

	if(listen(serverFd, MAX_CLIENTS) < 0) {
		return -1;
	}

	/* Set fd as non-blocking */
	if(selector_fd_set_nio(serverFd) < 0) {
		return -1;
	}

	return serverFd;
}
