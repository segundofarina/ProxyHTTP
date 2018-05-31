

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>




#include "headerGroup.h"
#include "multi_parser.h"
#include "request.h"

enum header_name{
    HEADER_HOST,
    HEADER_CONT_LEN,
    HEADER_TRANSF_ENC,
    HEADER_NOT_INTERESTED

};

enum body_type
getBodyType(struct header_list * list, int * len);

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
    int len;
    enum body_type type = getBodyType(p->headerList,&len);

    p->bodyParser = malloc(sizeof (struct body_parser));
    body_parser_init(p->bodyParser,type);
    next = request_body;
    return next;
}

enum request_state
done( const uint8_t c,struct request_parser *p) {
    enum request_state next;
    next = request_error;
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
    if(p != NULL){
        header_list_destroy(p->headerList);
    }

}


char *
request_state_string(enum request_state state){
    char * resp;
    switch(state) {
        case request_requestLine:
            resp = "Request Line";
            break;
        case request_headers:
            resp = "Headers";
            break;
        case request_body:
            resp = "Body";
            break;
        case request_done:
            resp = "Done";
            break;
        case request_error:
            resp = "Error";
            break;
        default:
            resp = "Unkown";
            break;
    }
    return resp;
}

enum body_type
getTransfEncoding(char * value){

    struct multi_parser p;
    enum transf_types{
        transf_chunk,
        transf_notIntrested,

    };
    char * * transfNames  = ( char *[]){"Chunked"      };
    int      types[] = {            body_type_chunked};
    int len = 1;
    int i;

    for(i=0;value[i] !=0 && value[i]==' ';i++);

    multi_parser_init(&p,body_type_identity,transfNames,types,len);

    enum body_type result= multi_parser_consume(value+i,&p);

    return result;

}

enum body_type
getBodyType(struct header_list * list, int * len){

    if(list == NULL){
        return body_type_error;
    }
    if(list->name == HEADER_TRANSF_ENC ){
        return getTransfEncoding(list->value);
    }else if(list->name == HEADER_CONT_LEN){
        *len = atoi(list->value);
        return body_type_identity;
    }
    return getBodyType(list->next,len);
}

enum request_state
request_parser_consume(struct request_parser *p, char * b,int len){
    enum request_state st = p->state;
    int i;
    for(i =0 ; i<len ; i++){
        st = request_parser_feed(b[i],p);
    }

    return st;
}
