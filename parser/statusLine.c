//
// Created by Segundo Fariña on 31/5/18.
//


#include "statusLine.h"
#include <stdio.h>
#include <ctype.h>

static enum statusLine_state
version(const uint8_t c, struct statusLine_parser* p) {
    enum statusLine_state next;
    switch (c) {
        case ' ':
            next = sl_code;
            break;
        default:
            next = sl_version;
            break;
    }
    return next;
}

static enum statusLine_state
code(const uint8_t c, struct statusLine_parser* p) {
    enum statusLine_state next;
    switch (c) {
        case ' ':
            next = sl_extra;
            break;
        default:
            if(isdigit(c)){
                p->code = p->code*10+(c-'0');
                next = sl_code;
            }else{
                next = sl_error;
            }
    }
    return next;
}

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
        case sl_version:
            next = version(c,p);
            break;
        case sl_code:
            next = code(c,p);
            break;
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

extern void
statusLine_parser_init (struct statusLine_parser *p){
    p->state = sl_version;
    p->code =0;

}


extern void
statusLine_parser_close(struct statusLine_parser *p){

}


extern char *
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
