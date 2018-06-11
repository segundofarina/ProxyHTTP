#ifndef PC_2018_04_HTTPREQUEST_H
#define PC_2018_04_HTTPREQUEST_H

#include <stdint.h>
#include <stdbool.h>

#include <sys/socket.h>

#include "requestLine.h"
#include "body.h"
#include "parserHostReqLine.h"


enum request_state {
    request_requestLine,
    request_headers,
    request_body,
    request_done,
    request_error
};

struct request_parser {
    enum request_state state;

    struct requestLine_parser * requestLineParser;
    struct headerGroup_parser * headerParser;
    struct body_parser        * bodyParser;

    struct header_list        * headerList;

    enum request_method method;
    char  requestURI[0xFF];
    char fqdn[0xFF];
    uint16_t port;


    bool hasDestination;
    struct requestData destintation;

};




void
request_parser_init(struct request_parser *p);

enum request_state
request_parser_feed (const uint8_t c,struct request_parser *p);


enum request_state
request_parser_consume(struct request_parser *p, char * b,int len);

void
request_parser_close(struct request_parser *p);

char *
request_state_string(enum request_state state);


enum body_type
getTransfEncoding(char * value);

#endif //PC_2018_04_HTTPREQUEST_H


