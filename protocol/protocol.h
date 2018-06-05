//
// Created by Nicolas Paganini on 4/6/18.
//

#ifndef PC_2018_04_PROTOCOL_H
#define PC_2018_04_PROTOCOL_H

#include <stdint.h>

/*
 *	Byte oriented protocol.
 *
 *	|Instruction|Len|	Data	|
 *	|-----------|---|-----------|
 *	|	Byte	|len|	bytes 	|
 *	|			|	| with info	|
 *	|-----------|---|-----------|
 *
 *	Protocol Commands
 *
 *	Client-Side (requests)
 *	1) help
 *	2)	metrics
 *	3)		active clients
 *	4)		transferred bytes
 *	5)			client bytes read
 *	6)			server bytes written
 *	7)		count GET
 *	8)		count HEAD
 *	9)		count POST
 *	10)		history connections
 *	11)	active configuration
 *	12)	set configuration
 *	13)	transformation list
 *	14)		set transformation N
 *	15) admin log in + password
 *
 *	Server-Side (responses)
 *	100) error
 *	101) help
 *	102)	metrics
 *	103)		active clients
 *	104)		transferred bytes
 *	105)			client bytes read
 *	106)			server bytes written
 *	107)		count GET
 *	108)		count HEAD
 *	109)		count POST
 *	110)		history connections
 *	111)	active configuration
 *	112)	set configuration
 *	113)	transformation list
 *	114)	transformation set
 *	115) 	admin log in + accept
 */

typedef enum {ADMIN_REQUEST_HELP = 1,
	ADMIN_REQUEST_METRICS, ADMIN_REQUEST_ACTIVE_CLIENTS,
	ADMIN_REQUEST_TRANSFRRD_BYTES, ADMIN_REQUEST_CLIENT_BYTES_READ, ADMIN_REQUEST_SERVER_BYTES_WRITTEN,
	ADMIN_REQUEST_GET_COUNT, ADMIN_REQUEST_HEAD_COUNT, ADMIN_REQUEST_POST_COUNT, ADMIN_REQUEST_HISTORY_CONNECTS,
	ADMIN_REQUEST_ACTIVE_CONFIG, ADMIN_REQUEST_SET_CONFIG, ADMIN_REQUEST_TRANSF_LIST, ADMIN_REQUEST_SET_TRANSF,
	ADMIN_REQUEST_ADMIN_LOG_IN, ADMIN_REQUEST_ADMIN_LOG_OUT
} clientInstruction;

typedef enum {SERVER_RESPONSE_ERROR = 100, SERVER_RESPONSE_HELP,
	SERVER_RESPONSE_METRICS, SERVER_RESPONSE_ACTIVE_CLIENTS,
	SERVER_RESPONSE_TRANSFRRD_BYTES, SERVER_RESPONSE_CLIENT_BYTES_READ, SERVER_RESPONSE_SERVER_BYTES_WRITTEN,
	SERVER_RESPONSE_COUNT_GET, SERVER_RESPONSE_COUNT_HEAD, SERVER_RESPONSE_COUNT_POST, SERVER_RESPONSE_HISTORY_CONNECTS,
	SERVER_RESPONSE_ACTIVE_CONFIG, SERVER_RESPONSE_SET_CONFIG, SERVER_RESPONSE_TRANSF_LIST, SERVER_RESPONSE_TRANSF_SET,
	SERVER_RESPONSE_ADMIN_LOGGED_IN, SERVER_RESPONSE_ADMIN_LOGOUT
} serverResponse;

/*
 *	Struct For Message Passing.
 *	TODO: decide on "data" size?
 */
struct protMsg {
	uint8_t instruction;
	uint16_t length;
	void * data;
	// char data[0xFF];
};

#endif //PC_2018_04_PROTOCOL_H
