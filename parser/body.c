//
// Created by Segundo Fariña on 30/5/18.
//

#include <stdlib.h>
#include "body.h"

extern void
body_parser_init (struct body_parser* p,enum body_type type, int len){
    if(type == body_type_chunked){
        p->state = body_chunked;
        p->chunkGroupParser = malloc(sizeof(struct chunkGroup_parser));
        chunkGroup_parser_init(p->chunkGroupParser);
    } else{
        p->state = body_identity;
        p->identityParser = malloc(sizeof(struct identity_parser));
        //init body parser
        identity_parser_init(p->identityParser,len);
    }
}

extern void
body_parser_close(struct body_parser* p){

}


enum body_state
chunked(const uint8_t c, struct body_parser *p) {

    enum body_state next;
     chunkGroup_parser_feed(c,p->chunkGroupParser);
    enum chunk_group_state state = p->chunkGroupParser->state;
    switch(state){
        case chunk_group_done:
            next = body_end;
            break;
        case chunk_group_error:
            next = body_error;
            break;
        default:
            next = body_chunked;
            break;
    }
    return next;
}

enum body_state
identity(const uint8_t c, struct body_parser *p) {
    enum body_state next;
    enum identity_state state = identity_parser_feed(c,p->identityParser);
    switch(state){
        case identity_end:
            next = body_end;
            break;
        case identity_error:
            next = body_error;
            break;
        default:
            next = body_identity;
            break;
    }
    return next;
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
