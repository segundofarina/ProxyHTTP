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

typedef enum {SERVER_RESPONSE_ERROR = 100, SERVER_RESPONSE_HELP,
	SERVER_RESPONSE_METRICS,
	SERVER_RESPONSE_ACTIVE_CONFIG, SERVER_RESPONSE_SET_CONFIG,
	SERVER_RESPONSE_TRANSF_LIST, SERVER_RESPONSE_TRANSF_SET,
	SERVER_RESPONSE_ADMIN_LOGGED_IN, SERVER_RESPONSE_ADMIN_LOGOUT
} serverResponse;

/*
 *	Struct For Message Passing.
 */
struct protMsg {
	uint8_t instruction;
	uint16_t length;
	void * data;
};

struct serverMetrics {
	uint16_t bytesRead;
	uint16_t bytesSent;
	uint16_t GETcount;
	uint16_t HEADcount;
	uint16_t POSTcount;
	uint16_t activeClients;
	uint16_t historyConnections;
};

struct proxyServerConfiguration {
	uint16_t bufferSize;
	uint16_t activeTransformation;
	uint16_t serverConnectionTimeout;
};

struct transformation {
	uint16_t transformationCommand;
	struct mediaType * mediaTypesList;
};

struct mediaType {
	char * type;
	struct mediaType * next;
};

// struct transformationList {
// 	uint16_t value;
// 	char * name;
// 	struct transformationList * next;
// };

#endif //PC_2018_04_PROTOCOL_H
