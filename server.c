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
#include "serverComponents/proxyPassiveHandlers.h"
#include "serverComponents/transErrorManager.h"

typedef enum {FALSE, TRUE} Bool;

#define MAX_CLIENTS 10
#define SELECTOR_SIZE 1024
#define DEFAULT_PROXY_HTTP_PORT 8080
#define DEFAULT_ADMIN_PORT 9090

#define LOGGER_LEVEL PRODUCTION

struct serverSettings {
	uint32_t httpAddr;
	uint16_t httpPort;
	uint32_t adminAddr;
	uint16_t adminPort;
};

int createPassiveSock(const uint32_t addr, const uint16_t port, const int protocol);
void getParams(const int argc, char * const * argv, struct serverSettings * settings);


int main(const int argc, char * const * argv) {
	struct serverSettings settings;
	int serverFd, adminFd;
	char * errMsg = NULL;
	selector_status selectorStatus = SELECTOR_SUCCESS;
    fd_selector selector = NULL;

	getParams(argc, argv, &settings);

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

	/* Start transformation manager */
	transformationManagerInit();

	/* Listen to clients */
	if( (serverFd = createPassiveSock(settings.httpAddr, settings.httpPort, IPPROTO_TCP)) < 0 ) {
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
	if( (adminFd = createPassiveSock(settings.adminAddr, settings.adminPort, IPPROTO_SCTP)) < 0 ) {
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

	/* Avoid process kill with sigpipe */
	signal(SIGPIPE, SIG_IGN);

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

		transformationManagerDestroy();

		if(selector != NULL) {
			selector_destroy(selector);
		}
		
		selector_close();

		//destroy pool

		printf("\x1b[31m[FATAL ERROR]\x1b[0m %s\n", errMsg);
		return 1;
}

/* Returns socket fd or -1 if error */
int createPassiveSock(const uint32_t address, const uint16_t port, const int protocol) {
	struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = address;
    addr.sin_port        = port;

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

void printHelp() {
	printf("help\n");
}

void printVersion() {
	printf("httpd server - version: 1.0.0\n");
}

void printUsage() {
	printf("usage \n");
}

void processMediaTypesList(const char * str) {
	char mediaType[25 +1] = {0};
	int i = 0, j = 0;
	while(str[i] != 0) {
		if(str[i] == ',') {
			if(j < 25 + 1) {
				mediaType[j] = 0;
				// mediaType to enum
				addMediaType(strToMediaType(mediaType));
			}
			j = 0;
			i++;
		} else {
			mediaType[j++] = str[i++];
		}
	}

	/* Add last one */
	if(j < 25 + 1) {
		mediaType[j] = 0;
		addMediaType(strToMediaType(mediaType));
	}
}

void getParams(const int argc, char * const * argv, struct serverSettings * settings) {
	/* Set default settings */
	settings->httpAddr = htonl(INADDR_ANY);
	settings->httpPort = htons(DEFAULT_PROXY_HTTP_PORT);
	settings->adminAddr = htonl(INADDR_LOOPBACK);
	settings->adminPort = htons(DEFAULT_ADMIN_PORT);
	
	int opt;
	while( (opt = getopt(argc, argv, "e:hl:L:M:o:p:t:v")) != -1 ) {
		switch (opt) {
			case 'e':
				if(!setTransformationErrorFile(optarg)) {
					printf("Error: the error file %s is too long\n", optarg);
					exit(EXIT_FAILURE);	
				}
				break;
			case 'h':
				printHelp();
				exit(EXIT_SUCCESS);
				break;
			case 'l':
				//printf("dir http is %s\n", optarg);
				//CAMBIAR Y MODIFICAR httpAddr
				break;
			case 'L':
				//printf("dir sctp is %s\n", optarg);
				// CAMBIAR Y MODIFICAR adminAddr
				break;
			case 'M':
				processMediaTypesList(optarg);
				break;
			case 'o':
				settings->adminPort = htons(atoi(optarg));
				break;
			case 'p':
				settings->httpPort = htons(atoi(optarg));
				break;
			case 't':
				if(!addTransformation(optarg)) {
					printf("Error: the command %s is invalid\n", optarg);
				}
				break;
			case 'v':
				printVersion();
				exit(EXIT_SUCCESS);
				break;

			default:
				printUsage();
				exit(EXIT_FAILURE);
		}
	}

}
