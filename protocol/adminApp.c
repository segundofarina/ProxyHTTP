//
// Created by Nicolas Paganini on 4/6/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "adminApp.h"

struct protMsg * adminShell(void) {
	char buffer[0xFF];
	int c = 0, i = 0;
	int j;
	struct protMsg * command = malloc(sizeof(struct protMsg*));

	printf("[Admin] $> ");
	while((c = tolower(getchar())) != '\n' && i < 0xFF) {
		buffer[i++] = c;
	}

	//
}