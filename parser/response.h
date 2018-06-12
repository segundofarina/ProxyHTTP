//
// Created by Segundo Fariña on 31/5/18.
//

#ifndef PC_2018_04_RESPONSE_H
#define PC_2018_04_RESPONSE_H

#include <stdint.h>
#include <stdbool.h>

#include <sys/socket.h>

#include "body.h"
#include "requestLine.h"
#include "parser_errorCodes.h"



enum response_state {
    response_statusLine,
    response_headers,
    response_body,
    response_done,
    response_error
};

enum header_name {
    HEADER_CONT_LEN,
    HEADER_TRANSF_ENC,
    HEADER_NOT_INTERESTED,
    HEADER_CONT_ENCONDING,
    HEADER_CONNECTION,
    HEADER_MEDIA_TYPE

};

struct response_parser {
    enum response_state state;
    enum parser_errorCode errorCode;
    enum request_method method;
    int statusCode;
    struct statusLine_parser * statusLineParser;
    struct headerGroup_parser * headerParser;
    struct body_parser        * bodyParser;

    struct header_list        * headerList;


    bool shouldKeepLastChar;
    bool compresed;
    bool chunked;

};






extern void
response_parser_init(struct response_parser *p, enum request_method method);

enum response_state
response_parser_feed (const uint8_t c,struct response_parser *p);


//enum response_state
//response_parser_consume(struct response_parser *p, char * b,int * len, char * writebuff, int * written);

void
response_parser_close(struct response_parser *p);

char *
response_state_string(enum response_state state);

extern char *
getHeaderName(enum header_name name);

extern enum header_name *
getIgnoredHeaders(int * quantity);


extern bool
isIgnored(uint32_t name);

#endif //PC_2018_04_RESPONSE_H
