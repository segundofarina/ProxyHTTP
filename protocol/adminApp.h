//
// Created by Nicolas Paganini on 4/6/18.
//

#ifndef PC_2018_04_ADMINAPP_H
#define PC_2018_04_ADMINAPP_H

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "protocol.h"

typedef enum {ADMIN_APP_ERROR = 0, ADMIN_REQUEST_HELP,
	ADMIN_REQUEST_METRICS,
	ADMIN_REQUEST_ACTIVE_CONFIG, ADMIN_REQUEST_SET_CONFIG,
	ADMIN_REQUEST_TRANSF_LIST, ADMIN_REQUEST_SET_TRANSF,
	ADMIN_REQUEST_ADMIN_LOG_IN, ADMIN_REQUEST_ADMIN_LOG_OUT
} clientInstruction;

int welcome(int ip, int port);
int adminPasswordCheck(struct protMsg * passRequest);
int adminShell(struct protMsg * command);

#endif //PC_2018_04_ADMINAPP_H