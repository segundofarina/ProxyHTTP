//
// Created by Segundo Fariña on 31/5/18.
//


#include "statusLine.h"
#include <stdio.h>



static enum statusLine_state
extra(const uint8_t c, struct statusLine_parser* p) {
    enum statusLine_state next;
    switch (c) {
        case '\r':
            next = sl_clrf;
            break;
        default:
            next = sl_extra;
            break;
    }

    return next;
}

static enum statusLine_state
clrf(const uint8_t c, struct statusLine_parser* p) {
    enum statusLine_state next;
    switch (c) {
        case '\n':
            next = sl_end;
            break;
        default:
            next = sl_error;
            break;
    }

    return next;
}
static enum statusLine_state
end(const uint8_t c, struct statusLine_parser* p){
    enum statusLine_state next;
    switch (c) {
        default:
            next = sl_error;
            break;
    }

    return next;
}


extern enum statusLine_state
statusLine_parser_feed ( const uint8_t c,struct statusLine_parser* p) {
    enum statusLine_state next;

    switch(p->state) {
        case sl_extra:
            next = extra(c,p);
            break;
        case sl_clrf:
            next = clrf(c,p);
            break;
        case sl_end:
            next = end(c,p);
            break;
        default:
            next = sl_error;
            break;
    }

    return p->state = next;
}

void
statusLine_parser_init (struct statusLine_parser *p){
    p->state = sl_extra;

}


void
statusLine_parser_close(struct statusLine_parser *p){

}


char *
statusLine_state_string(const enum statusLine_state st){
    char * resp;
    switch(st) {
        case sl_extra:
            resp = "Extra";
            break;
        case sl_error:
            resp = "Error";
            break;
        case sl_end:
            resp = "End";
            break;
        case sl_clrf:
            resp = "CLRF";
            break;
        default:
            resp = "Unkown";
    }
    return resp;
}
