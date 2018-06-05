#include <stdio.h>

#include "adminApp.h"

int
main(void)
{
	uint16_t * transf;
	struct protMsg * msgToSend;
	msgToSend = malloc(sizeof(struct protMsg*));
	msgToSend->instruction = ADMIN_REQUEST_HELP;

	while(msgToSend->instruction != ADMIN_REQUEST_ADMIN_LOG_OUT) {

		adminShell(msgToSend);

		printf("\n");
		printf("Command chosen: ");
		switch(msgToSend->instruction) {
			case ADMIN_REQUEST_HELP:
				printf("help\n");
				printf("Answer: (commands with subcommands)\n");
				printf("metrics, active configuration, set configuration, transformation list, set transformation + nro, admin log in + password, logout\n");
				break;
			case ADMIN_REQUEST_METRICS:
				printf("metrics\n");
				printf("Answer: (commands with subcommands)\n");
				printf("active clients, transferred bytes, GET count, HEAD count, POST count, history connections\n");
				break;
			case ADMIN_REQUEST_ACTIVE_CLIENTS:
				printf("Active clients: N\n");
				break;
			case ADMIN_REQUEST_TRANSFRRD_BYTES:
				printf("Answer: (commands with subcommands)\n");
				printf("client bytes read, server bytes written\n");
				break;
			case ADMIN_REQUEST_CLIENT_BYTES_READ:
				printf("Client bytes read: N\n");
				break;
			case ADMIN_REQUEST_SERVER_BYTES_WRITTEN:
				printf("Server bytes written: N\n");
				break;
			case ADMIN_REQUEST_GET_COUNT:
				printf("Amount of GETs received: N\n");
				break;
			case ADMIN_REQUEST_HEAD_COUNT:
				printf("Amount of HEADs received: N\n");
				break;
			case ADMIN_REQUEST_POST_COUNT:
				printf("Amount of POSTs received:\n");
				break;
			case ADMIN_REQUEST_HISTORY_CONNECTS:
				printf("Total clients (disconnected and active): N\n");
				break;
			case ADMIN_REQUEST_ACTIVE_CONFIG:
				printf("Active configuration is set to:\n\tTransformation: OFF\nActive clients: N\n");
				break;
			case ADMIN_REQUEST_SET_CONFIG:
				printf("Configuration set to:\n\tTransformation: InvertedPicture\nActive clients: M\n");
				break;
			case ADMIN_REQUEST_TRANSF_LIST:
				printf("List of transformations in memory:\n0) OFF\n1) InvertedPicture\n");
				break;
			case ADMIN_REQUEST_SET_TRANSF:
				transf = (uint16_t*)msgToSend->data;
				printf("Transformation: %d\n", *transf);
				break;
			case ADMIN_REQUEST_ADMIN_LOG_IN:
				printf("Welcome admin! || wrong password: %s\n", (char*) msgToSend->data);
				break;
			case ADMIN_REQUEST_ADMIN_LOG_OUT:
				printf("Logged out!\n");
		}
		printf("\n\n");
	}

	free(msgToSend->data);
	free(msgToSend);

	return 0;
}