//
// Created by Segundo Fariña on 30/5/18.
//
#include <MacTypes.h>
#include "body.h"
#include "identity.h"

extern void
body_parser_init (struct body_parser* p,enum body_type type){
    if(type == body_type_identity){
        p->state = body_identity;
        //init body parser

    }
}

extern void
body_parser_close(struct body_parser* p){

}

enum body_state
initBody(const uint8_t c, struct body_parser *p) {
    //enum body_state next;

    return body_init;
}

enum body_state
chunked(const uint8_t c, struct body_parser *p) {
    //enum body_state next;

    return body_chunked;
}

enum body_state
identity(const uint8_t c, struct body_parser *p) {
    //enum body_state next;

    return body_init;
}

enum body_state
endBody(const uint8_t c, struct body_parser *p) {
//    enum body_state next;
//    switch(c){
//
//
//    }
    return body_error;
}

enum body_state
body_parser_feed(const uint8_t c,struct body_parser* p){
    enum body_state next;
    switch(p->state) {
        case body_init:
            next = initBody(c ,p);
            break;
        case body_chunked:
            next = chunked(c,p);
            break;
        case body_identity:
            next = identity(c,p);
            break;
        case body_end:
            next = endBody(c,p);
            break;
        default:
            next = body_error;
            break;
    }

    return p->state = next;
}

char *
body_state_string(enum body_state st){
    char * resp;
    switch(st){
        case body_init:
            resp= "Init";
            break;
        case body_identity:
            resp= "Identity";
            break;
        case body_chunked:
            resp= "Chunked";
            break;
        case body_end:
            resp= "End";
            break;
        case body_error:
            resp= "Error";
            break;
        default:
            resp = "Unknown";
            break;
    }
    return resp;
}
