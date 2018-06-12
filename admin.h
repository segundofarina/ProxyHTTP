//
// Created by Florencia Cavallin on 11/6/18.
//

#ifndef PC_2018_04_ADMIN_H
#define PC_2018_04_ADMIN_H


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_SERVER_RESPONSE 255
#define MAX_ADMIN_REQUEST 255
#define SIZE_CHAR sizeof(uint8_t)
#define SIZE_INTEGER 4*SIZE_CHAR

enum auth_response{
    LOGGED_IN=0x00,
    LOGIN_FAILED=0x01
};
enum server_response{
    OK=0x00,
    ERR=0x01,
    NOT_SUPPORTED=0x02,
    INTERNAL_SERVER_ERR = 0xFF
};
enum client_request{
    SET_TRANSFORMATION=0x00,
    ADD_MEDIA_TYPE=0x01,
    REMOVE_MEDIA_TYPE=0x02,
    SET_BUFFER_SIZE=0x03,
    SET_TIMEOUT=0x04,
    GET_METRICS=0x05,
    GET_TRANSFORMATION=0x06,
    GET_MEDIA_TYPES=0x07,
    GET_BUFFER_SIZE=0x08,
    GET_TIMEOUT=0x09
};


#endif //PC_2018_04_ADMIN_H
