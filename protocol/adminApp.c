//
// Created by Nicolas Paganini on 4/6/18.
//

#include <stdio.h>
#include "adminApp.h"

#define PASSWORD_SIZE 0xFF
#define PASSWORD_CORRECT 1
#define ADMIN_APP_EXIT 2

/*
 *	Password check.
 */
int adminPasswordCheck(struct protMsg * passRequest) {
	int c = 0, i = 0, success = 0;
	char password[PASSWORD_SIZE] = {0};

	printf("Please enter password:\n");
	while((c = getchar()) != '\n' && i < PASSWORD_SIZE) {
		password[i] = c;
	}
	// passRequest = malloc(sizeof(struct protMsg *));
	passRequest->instruction = ADMIN_REQUEST_ADMIN_LOG_IN;
	passRequest->length = i*sizeof(char);
	passRequest->data = password;
	printf("Attemping connect...\n");
	// TODO: send???????
	success = PASSWORD_CORRECT;

	return success;
}

/*
 *	Welcome message.
 */
int welcome(int ip, int port) {
	int loggedIn = 0, option = 0;
	struct protMsg * adminRequest = malloc(sizeof(struct protMsg*));

	printf("Welcome home!\n\n");
	do {
		loggedIn = adminPasswordCheck(adminRequest);
		printf("\n");
		if(!loggedIn) {
			printf("Wrong password. Would you like to:\n");
			printf("1) Retry\n2) Exit\n");
			option = getchar() - '0';
			while(getchar() != '\n');
			printf("\n\n");
		}
	} while(!loggedIn && option != ADMIN_APP_EXIT);
	while(loggedIn == PASSWORD_CORRECT) {
		loggedIn = adminShell(adminRequest);
	}

	free(adminRequest->data);
	free(adminRequest);

	printf("Goodbye!\n");
}

/*
 *	BLOQUEANTE POR printfs	--> NO importa, corre aparte del proxy
 */
int adminShell(struct protMsg * command) {
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

	// if(strcmp("help", buffer) == 0) {
	// 	command->instruction = ADMIN_REQUEST_HELP;
	// } else if(strcmp("metrics", buffer) == 0) {
	// 	command->instruction = ADMIN_REQUEST_METRICS;
	// } else if(strcmp("active clients", buffer) == 0) {
	// 	command->instruction = ADMIN_REQUEST_ACTIVE_CLIENTS;
	// } else if(strcmp("transferred bytes", buffer) == 0) {
	// 	command->instruction = ADMIN_REQUEST_TRANSFRRD_BYTES;
	// } else if(strcmp("client bytes read", buffer) == 0) {
	// 	command->instruction = ADMIN_REQUEST_CLIENT_BYTES_READ;
	// } else if(strcmp("server bytes written", buffer) == 0) {
	// 	command->instruction = ADMIN_REQUEST_SERVER_BYTES_WRITTEN;
	// } else if(strcmp("GET count", buffer) == 0) {
	// 	command->instruction = ADMIN_REQUEST_GET_COUNT;
	// } else if(strcmp("HEAD count", buffer) == 0) {
	// 	command->instruction = ADMIN_REQUEST_HEAD_COUNT;
	// } else if(strcmp("POST count", buffer) == 0) {
	// 	command->instruction = ADMIN_REQUEST_POST_COUNT;
	// } else if(strcmp("history connections", buffer) == 0) {
	// 	command->instruction = ADMIN_REQUEST_HISTORY_CONNECTS;
	// } else if(strcmp("active configuration", buffer) == 0) {
	// 	command->instruction = ADMIN_REQUEST_ACTIVE_CONFIG;
	// } else if(strcmp("set configuration", buffer) == 0) {
	// 	command->instruction = ADMIN_REQUEST_SET_CONFIG;
	// } else if(strcmp("transformation list", buffer) == 0) {
	// 	command->instruction = ADMIN_REQUEST_TRANSF_LIST;
	// } else if(strncmp("set transformation ", buffer, 19) == 0) {
	// 	command->instruction = ADMIN_REQUEST_SET_TRANSF;
	// 	free(command->data);
	// 	transformationChoice = malloc(sizeof(int));
	// 	aux = atoi(buffer+19);
	// 	// printf("%d\n", aux);	//	Esta bien.
	// 	if(aux) {
	// 		memcpy(transformationChoice, &aux, sizeof(int));
	// 		command->data = transformationChoice;
	// 	} else {
	// 		command->instruction = ADMIN_REQUEST_HELP;
	// 	}
	// } else if(strncmp("admin log in ", buffer, 13) == 0) {
	// 	command->instruction = ADMIN_REQUEST_ADMIN_LOG_IN;
	// 	// i = 13; j = 0;
	// 	// while(buffer[i] != 0) {
	// 	// 	password[j] = buffer[i];
	// 	// }
	// 	command->data = malloc(j*sizeof(char));
	// 	memcpy(command->data, buffer+13, (i - 13)*sizeof(char));
	// } else if(strcmp("logout", buffer) == 0) {
	// 	command->instruction = ADMIN_REQUEST_ADMIN_LOG_OUT;
	// } else {
	// 	printf("Command not recognized! These are the recognized commands.\n");
	// 	command->instruction = ADMIN_REQUEST_HELP;
	// }
	// if(command->instruction == ADMIN_APP_ERROR) {
	// 	printf("ERROR\n");
	// }
	command->length = 0;

	// TODO: resolve mallocs from this file.c
}

int
main(int argc, char ** argv)
{
	int error, ip, port;
	if(argc != 3) {
		printf("Admin requires a proxy to connect to.\n");
		printf("Run with [ip] [port].\n");
		error = 1;
	} else {
		ip = atoi(argv[1]);
		port = atoi(argv[2]);
		error = welcome(ip, port);
		// error = 0;
	}
	return error;
}
