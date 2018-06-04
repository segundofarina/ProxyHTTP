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

typedef enum {HELP = 1,
	METRICS, ACTIVE_CLIENTS, TRANSFRRD_BYTES, CLIENT_BYTES_READ, SERVER_BYTES_WRITTEN,
	COUNT_GET, COUNT_HEAD, COUNT_POST, HISTORY_CONNECTS,
	ACTIVE_CONFIG, SET_CONFIG, TRANSF_LIST, SET_TRANSF, ADMIN_LOG_IN
} clientInstruction;

typedef enum {ERROR = 100, HELP,
	METRICS, ACTIVE_CLIENTS, TRANSFRRD_BYTES, CLIENT_BYTES_READ, SERVER_BYTES_WRITTEN,
	COUNT_GET, COUNT_HEAD, COUNT_POST, HISTORY_CONNECTS,
	ACTIVE_CONFIG, SET_CONFIG, TRANSF_LIST, TRANSF_SET, ADMIN_LOGGED_IN
} serverResponse;

/*
 *	Struct For Message Passing
 */
typedef struct protMsg {
	uint8_t instruction;
	uint16_t length;
	void * data;
} Msg;

#endif //PC_2018_04_PROTOCOL_H
