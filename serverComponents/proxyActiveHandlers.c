#include <stdio.h>

#include <sys/socket.h>

#include "proxyActiveHandlers.h"


void connection_read (struct selector_key *key) {
	char buff[1000] = {0};
	int n = recv(key->fd, buff, 999, 0);
	printf("read something: %s\n", buff);
}

void connection_write (struct selector_key *key) {

}

void connection_close (struct selector_key *key) {

}

void connection_block (struct selector_key *key) {

}