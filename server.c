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

typedef enum {FALSE, TRUE} Bool;

#define MAX_CLIENTS 10
#define SELECTOR_SIZE 1024

int createProxyPassiveSock(const int port, char ** errMsg);


//int main(const int argc, const char * argv[]) {
int main() {

	int port = 1080, serverFd;
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
        .handle_read       = proxyPassiveAccept,
        .handle_write      = NULL,
        .handle_close      = NULL
    };

	// podemos cerrar la entrada estandard

	/* Selector init */
	if(selector_init(&conf) != 0) {
		//handle error
	}

	if( (selector = selector_new(SELECTOR_SIZE)) == NULL ) {
		//handle error
	}

	if( (serverFd = createProxyPassiveSock(port, &errMsg)) < 0 ) {
		//handle error
	}

	selectorStatus = selector_register(selector, serverFd, &proxyPassiveHandlers, OP_READ, NULL);
    if(selectorStatus != SELECTOR_SUCCESS) {
        errMsg = "Unable to register passive socket fd";
		//handler error
    }

    while(TRUE) {
		printf("Waiting in select\n");
        selectorStatus = selector_select(selector);
        if(selectorStatus != SELECTOR_SUCCESS) {
            //handle error
        }
    }

	return 0;
}

/* Returns socket fd or -1 if error */
int createProxyPassiveSock(const int port, char ** errMsg) {
	struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port        = htons(port);

	const int serverFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(serverFd < 0) {
		*errMsg = "Unable to create socket";
		return -1;
	}

	setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));

	if(bind(serverFd, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
		*errMsg = "Unable to bind socket";
		return -1;
	}

	if(listen(serverFd, MAX_CLIENTS) < 0) {
		*errMsg = "Unable to listen socket";
		return -1;
	}

	/* Set fd as non-blocking */
	if(selector_fd_set_nio(serverFd) < 0) {
		*errMsg = "Unable to ser socket as non-blocking";
		return -1;
	}

	return serverFd;
}