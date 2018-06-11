#ifndef _ADMIN_STM_H_
#define _ADMIN_STM_H_

#include "../utils/stm/stm.h"

enum adminStates {
	AUTHENTICATE_READ,
	AUTHENTICATE_WRITE,
    ADMIN_READ,
    ADMIN_WRITE,
	ADMIN_DONE,
	ADMIN_ERROR,
	ADMIN_FATAL_ERROR
};

struct state_definition * getAdminStates();


#endif