#ifndef _PROXY_STM_H_
#define _PROXY_STM_H_

#include "../utils/stm/stm.h"

enum proxyStates {
	/*
	 * Reads client http request and starts parsing it.
	 * 
	 * Interest:
	 * 		- OP_READ from clientFd
	 * 
	 * Transitions:
	 * 		- OBTAIN_ORIGIN if the parser don't have the domain yet
	 * 		- SOLVE_DOMAIN if the parser has the domain
	 * 		- CONNECT if the parser has a ip
	 * 		- ERROR if io buffer overflow
	 */
	OBTAIN_ORIGIN,

	/*
	 * Process the dns request
	 * 
	 * Interest:
	 * 		- OP_NOOP from clientFd
	 * 		It will be called from a blocking handler
	 * 
	 * Transitions:
	 * 		- CONNECT if it got the dns resolution
	 * 		- ERROR in any other case
	 */
	SOLVE_DOMAIN,

	/*
	 * Connects to the origin server
	 * 
	 * Interest:
	 * 		- OP_WRITE from originFd
	 * 
	 * Trensitions:
	 * 		- WRITE_REQ if it connected to the origin server
	 * 		- ERROR in any other case
	 */
	CONNECT,

	/*
	 * Send already bufferd request to the origin server
	 * 
	 * Interest:
	 * 		- OP_WRITE from originFd
	 * 
	 * Transitions:
	 * 		- 
	 */
	/*WRITE_REQ,
	READ_REQ,
	READ_RESP,
	WRITE_RESP,
*/
	/* Just for testing */ANSWER,

	DONE,
	ERROR
};



struct state_definition * getProxyStates();

#endif