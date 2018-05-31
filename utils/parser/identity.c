//
// Created by Segundo Fariña on 31/5/18.
//

#include "identity.h"
extern void
identity_parser_init(struct identity_parser* p, int endLen){
    p->state=identity_data;
    p->endLen=endLen;
    p->currentLen=0;
}

enum identity_state
identityData(const uint8_t c, struct identity_parser *p){
    enum identity_state next;

    if(p->currentLen == p->endLen){
        next = identity_end;
    }else{
        p->currentLen++;
        if(p->currentLen>p->endLen) {
            next = identity_error;
        }else {
            next = identity_data;
        }
    }

    return next;

}


enum identity_state
identityEnd(const uint8_t c, struct identity_parser *p){
    return identity_end;
}

extern enum identity_state
identity_parser_feed(const uint8_t c,struct identity_parser* p){
    enum identity_state next;
    switch(p->state) {
        case identity_data:
            next = identityData(c ,p);
            break;
        case identity_end:
            next = identityEnd(c,p);
            break;
        default:
            next = identity_error;
            break;
    }

    return p->state = next;
}

extern void
identity_parser_close(struct identity_parser* p){

}

char *
identity_state_string(enum identity_state st){
    char * resp;
    switch(st){
        case identity_data:
            resp= "Data";
            break;
        case identity_end:
            resp= "End";
            break;
        default:
            resp = "Unknown";
            break;
    }
    return resp;
}