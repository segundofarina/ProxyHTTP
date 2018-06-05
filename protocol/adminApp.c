//
// Created by Nicolas Paganini on 4/6/18.
//

#include <stdio.h>
#include "adminApp.h"

/*
typedef enum {HELP = 1,
	METRICS, ACTIVE_CLIENTS, TRANSFRRD_BYTES, CLIENT_BYTES_READ, SERVER_BYTES_WRITTEN,
	COUNT_GET, COUNT_HEAD, COUNT_POST, HISTORY_CONNECTS,
	ACTIVE_CONFIG, SET_CONFIG, TRANSF_LIST, SET_TRANSF, ADMIN_LOG_IN
} clientInstruction;
*/

/*
 *	BLOQUEANTE POR printfs	--> NO importa, corre aparte del proxy
 */
void adminShell(struct protMsg * command) {
	// clientInstruction ans = HELP;
	char buffer[0xFF] = {0};
	// char password[0xFF] = {0};
	int c = 0, i = 0;
	int j = 0;
	// int transformation = 0;
	int aux = 0;
	void * transformationChoice;

	command->instruction = ADMIN_APP_ERROR;
	command->length = 0;
	command->data = NULL;

	printf("[Admin] $> ");
	while((c = tolower(getchar())) != '\n' && i < 0xFF) {
		buffer[i++] = c;
	}

	if(strcmp("help", buffer) == 0) {
		command->instruction = ADMIN_REQUEST_HELP;
	} else if(strcmp("metrics", buffer) == 0) {
		command->instruction = ADMIN_REQUEST_METRICS;
	} else if(strcmp("active clients", buffer) == 0) {
		command->instruction = ADMIN_REQUEST_ACTIVE_CLIENTS;
	} else if(strcmp("transferred bytes", buffer) == 0) {
		command->instruction = ADMIN_REQUEST_TRANSFRRD_BYTES;
	} else if(strcmp("client bytes read", buffer) == 0) {
		command->instruction = ADMIN_REQUEST_CLIENT_BYTES_READ;
	} else if(strcmp("server bytes written", buffer) == 0) {
		command->instruction = ADMIN_REQUEST_SERVER_BYTES_WRITTEN;
	} else if(strcmp("GET count", buffer) == 0) {
		command->instruction = ADMIN_REQUEST_GET_COUNT;
	} else if(strcmp("HEAD count", buffer) == 0) {
		command->instruction = ADMIN_REQUEST_HEAD_COUNT;
	} else if(strcmp("POST count", buffer) == 0) {
		command->instruction = ADMIN_REQUEST_POST_COUNT;
	} else if(strcmp("history connections", buffer) == 0) {
		command->instruction = ADMIN_REQUEST_HISTORY_CONNECTS;
	} else if(strcmp("active configuration", buffer) == 0) {
		command->instruction = ADMIN_REQUEST_ACTIVE_CONFIG;
	} else if(strcmp("set configuration", buffer) == 0) {
		command->instruction = ADMIN_REQUEST_SET_CONFIG;
	} else if(strcmp("transformation list", buffer) == 0) {
		command->instruction = ADMIN_REQUEST_TRANSF_LIST;
	} else if(strncmp("set transformation ", buffer, 19) == 0) {
		command->instruction = ADMIN_REQUEST_SET_TRANSF;
		free(command->data);
		transformationChoice = malloc(sizeof(int));
		aux = atoi(buffer+19);
		// printf("%d\n", aux);	//	Esta bien.
		if(aux) {
			memcpy(transformationChoice, &aux, sizeof(int));
			command->data = transformationChoice;
		} else {
			command->instruction = ADMIN_REQUEST_HELP;
		}
	} else if(strncmp("admin log in ", buffer, 13) == 0) {
		command->instruction = ADMIN_REQUEST_ADMIN_LOG_IN;
		// i = 13; j = 0;
		// while(buffer[i] != 0) {
		// 	password[j] = buffer[i];
		// }
		command->data = malloc(j*sizeof(char));
		memcpy(command->data, buffer+13, (i - 13)*sizeof(char));
	} else if(strcmp("logout", buffer) == 0) {
		command->instruction = ADMIN_REQUEST_ADMIN_LOG_OUT;
	} else {
		printf("Command not recognized! These are the recognized commands.\n");
		command->instruction = ADMIN_REQUEST_HELP;
	}
	if(command->instruction == ADMIN_APP_ERROR) {
		printf("ERROR\n");
	}
	command->length = 0;

	// TODO: resolve mallocs from this file.c
}
