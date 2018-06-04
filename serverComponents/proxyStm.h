#ifndef _PROXY_STM_H_
#define _PROXY_STM_H_

#include "../utils/stm/stm.h"

enum proxyStates {
	REQUEST,
	RESPONSE,
	DONE,
	ERROR,
	FATAL_ERROR
};



struct state_definition * getProxyStates();

#endif