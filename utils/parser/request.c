

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>


#include "request.h"
#include "requestLine.h"
#include "headerGroup.h"


enum header_name{
    HEADER_HOST,
    HEADER_CONT_LEN,
    HEADER_TRANSF_ENC,
    HEADER_NOT_INTERESTED

};

char * * headerNames  = ( char *[]){"Host"      ,"Content-Length","Transfer-Encoding"};
int      types[] = {            HEADER_HOST, HEADER_CONT_LEN,HEADER_TRANSF_ENC};
#define HEADERS_AMOUNT 3


enum request_state
requestLine(const uint8_t c,struct request_parser *p) {
    enum request_state next;
    enum requestLine_state state =requestLine_parser_feed(c,p->requestLineParser);
    switch(state){
        case rl_end:
            p->method = p->requestLineParser->method;
            strcpy(p->requestURI,p->requestLineParser->uri);
            requestLine_parser_close(p->requestLineParser);
            headerGroup_parser_init(p->headerParser,HEADER_NOT_INTERESTED,headerNames,types,HEADERS_AMOUNT);
            next = request_headers;
            break;
        default:
            next = request_requestLine;
            break;
    }
    return next;
}

enum request_state
headers(const uint8_t c,struct request_parser *p){
    enum request_state next;
    enum headerGroup_state state = headerGroup_parser_feed(c,p->headerParser);
    switch (state) {
        case headerGroup_end:
            p->headerList=p->headerParser->list;
            headerGroup_parser_close(p->headerParser);
            if(p->method == METHOD_POST){
                next = request_body;
            }else{
                next = request_done;
            }
            break;
        case headerGroup_error:

        default:
            next = request_headers;
            break;
    }

    return next;
}

enum request_state
body(const uint8_t c,struct request_parser *p) {
    enum request_state next;
    next=request_body;
    return next;
}

enum request_state
done( const uint8_t c,struct request_parser *p) {
    enum request_state next;
    next=request_error;
    return next;
}


void
request_parser_init(struct request_parser *p) {

    p->state = request_requestLine;
    p->requestLineParser = malloc(sizeof(struct requestLine_parser));
    p->headerParser = malloc(sizeof(struct headerGroup_parser));

    requestLine_parser_init(p->requestLineParser);
}



enum request_state
request_parser_feed (const uint8_t c, struct request_parser *p){
    enum request_state next;

    switch(p->state) {
        case request_requestLine:
            next = requestLine(c,p);
            break;
        case request_headers:
            next = headers(c,p);
            break;
        case request_body:
            next = body(c,p);
            break;
        case request_done:
            next = done(c,p);
            break;
        default:
            next = request_error;
            break;
    }
    return p->state = next;
}

void
request_parser_close(struct request_parser *p){
    if(p!= NULL){
        header_list_destroy(p->headerList);
    }

}


char *
request_state_string(enum request_state state){
    char * resp;
    switch(state) {
        case request_requestLine:
            resp ="Request Line";
            break;
        case request_headers:
            resp ="Headers";
            break;
        case request_body:
            resp ="Body";
            break;
        case request_done:
            resp ="Done";
            break;
        case request_error:
            resp = "Error";
            break;
        default:
            resp ="Unkown";
            break;
    }
    return resp;
}

enum request_state
request_parser_consume(struct request_parser *p, char * b,int len){
    enum request_state st = p->state;

    return st;
}
