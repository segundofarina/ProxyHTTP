#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "request.h"


int
main(void)
{
	int i = 0;

	printf("Hello\n");

	char request1[] = "GET google.com/img/ HTTP/1.1\r\nHost: google.com\r\n\r\n";
	char request2[] = "GET google.com/img/ HTTP/1.1\r\nhost: google.com\r\n\r\n";
	char request3[] = "GET google.com/img/ HTTP/1.1\r\nHosts: google.com\r\n\r\n";
	char request4[] = "GET google.com/img/ HTTP/1.1\r\nHost:google.com\r\n\r\n";

	HeaderParser * p = headerParser_init();
	if(p == NULL) {
		printf("Error init\n");
		exit(1);
	}
	struct request_parser * pAux = malloc(sizeof(*pAux));
	if(pAux != NULL) {
		memset(pAux, 0, sizeof(*pAux));
	} else {
		printf("Error request_parser malloc\n");
		exit(1);
	}
	pAux->origin_dest = malloc(sizeof(struct socks_addr*));
	if(pAux->origin_dest != NULL) {
		memset(pAux->origin_dest, 0, sizeof(*pAux->origin_dest));
	} else {
		printf("Error origin_dest malloc\n");
		exit(1);
	}

	while(request1[i] != 0) {
		int st;
		printf("%c\n", request1[i]);
		st = requestHeaders(pAux, (uint8_t)request1[i], p);
		if(st == error) {
			printf("Error.\n");
		}
		i++;
	}

	printf("Request1 Host: %s\n", pAux->origin_dest->fqdn);

	// free(pAux->origin_dest);
	free(pAux);
	free(p);

	printf("Frees done.\n");

	return 0;
}