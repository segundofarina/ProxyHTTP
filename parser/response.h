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



enum response_state {
    response_statusLine,
    response_headers,
    response_body,
    response_done,
    response_error
};

struct response_parser {
    enum response_state state;
    enum request_method method;
    int statusCode;
    struct statusLine_parser * statusLineParser;
    struct headerGroup_parser * headerParser;
    struct body_parser        * bodyParser;

    struct header_list        * headerList;


    bool shouldKeepLastChar;

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


enum body_type
getTransfEncodingResponse(char * value);

extern bool
isIgnored(uint32_t name);

#endif //PC_2018_04_RESPONSE_H
