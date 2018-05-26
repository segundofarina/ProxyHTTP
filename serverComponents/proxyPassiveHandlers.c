#include <stdio.h>
#include <arpa/inet.h>

#include "proxyPassiveHandlers.h"

void proxyPassiveAccept(struct selector_key *key) {
	struct sockaddr_storage client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

	const int clientFd = accept(key->fd, (struct sockaddr*) &client_addr, &client_addr_len);
    if(clientFd == -1) {
        // handle error
    }
    if(selector_fd_set_nio(clientFd) == -1) {
        // handle error
    }

	printf("new client accepted\n");

}