
#include "requestLine.h"
#include <stdio.h>
#include "multi_parser.h"


char * * methodNames  = ( char *[]){"GET"      ,"HEAD"     ,"POST"};
int      methodTypes[] = {          METHOD_GET ,METHOD_HEAD,METHOD_POST};
#define METHODS_AMOUNT 3

static enum requestLine_state
method(const uint8_t c, struct requestLine_parser* p) {
    enum requestLine_state next;
    switch (c) {
        case ' ':
            // Guardar el method que quedo en p->method_parser.method
            p->method=p->methodParser->currentMatch;

            multi_parser_close(p->methodParser);
            free(p->methodParser);
            p->methodParser=NULL;

            next = rl_target;
            break;
        default:
            multi_parser_feed(c,p->methodParser);
            next = rl_method;
            break;
    }

    return next;
}

static enum requestLine_state
target(const uint8_t c, struct requestLine_parser* p) {
    enum requestLine_state next;
    switch (c) {
        case ' ':
            p->uri[p->len] = 0;
            next = rl_extra;
            break;
        default:
            //guardar el target en estructura
            if(p->len <0xFF){
                p->uri[p->len++] = c;
                next = rl_target;
            }else{
                next = rl_error;
            }

            break;
    }

    return next;
}

static enum requestLine_state
extra(const uint8_t c, struct requestLine_parser* p) {
    enum requestLine_state next;
    switch (c) {
        case '\r':
            next = rl_clrf;
            break;
        default:
            next = rl_extra;
            break;
    }

    return next;
}

static enum requestLine_state
clrf(const uint8_t c, struct requestLine_parser* p) {
    enum requestLine_state next;
    switch (c) {
        case '\n':
            next = rl_end;
            break;
        default:
            next = rl_error;
            break;
    }

    return next;
}
static enum requestLine_state
end(const uint8_t c, struct requestLine_parser* p){
    enum requestLine_state next;
    switch (c) {
        default:
            next = rl_error;
            break;
    }

    return next;
}


        extern enum requestLine_state
requestLine_parser_feed ( const uint8_t c,struct requestLine_parser* p) {
    enum requestLine_state next;

    switch(p->state) {
        case rl_method:
            next = method(c, p);
            break;
        case rl_target:
            next = target(c ,p);
            break;
        case rl_extra:
            next = extra(c,p);
            break;
        case rl_clrf:
            next = clrf(c,p);
            break;
        case rl_end:
            next = end(c,p);
            break;
        default:
            next = rl_error;
            break;
    }

    return p->state = next;
}

void
requestLine_parser_init (struct requestLine_parser *p){
    p->state = rl_method;
    p->len =0;
    p->methodParser = malloc(sizeof(struct multi_parser));

    multi_parser_init(p->methodParser,METHOD_NOTSUPPORTED,methodNames,methodTypes,METHODS_AMOUNT);

}


void
requestLine_parser_close(struct requestLine_parser *p){
    if(p != NULL){
        multi_parser_close(p->methodParser);
        free(p->methodParser);
        p->methodParser=NULL;
    }

}

bool
requestLine_is_done(const enum requestLine_state st, bool *errored){
    if(st== rl_error && errored!= NULL){
        *errored= true;
    }
    return st == rl_end || st == rl_error;
}

enum requestLine_state
requestLine_parser_consume(char *buffer,size_t len, struct requestLine_parser *p, bool *errored){
    size_t i;
    enum requestLine_state state;
    for(i =0 ; i< len ;i++){
       state = requestLine_parser_feed(buffer[i],p);
        char letter = buffer[i];
        if(letter == '\n'){
            letter = 'N';
        }else if (letter == '\r'){
            letter = 'R';
        }
    }
    if( errored != NULL) {
        if (state == rl_error) {
            *errored = true;
        } else {
            *errored = false;
        }
    }

    return state;
}

char * requestLine_state_toString(const enum requestLine_state st){
    char * resp;
    switch(st) {
        case rl_method:
            resp = "Method";
            break;
        case rl_target:
            resp = "Target";
            break;
        case rl_extra:
            resp = "Extra";
            break;
        case rl_error:
            resp = "Error";
            break;
        case rl_end:
            resp = "End";
            break;
        case rl_clrf:
            resp = "CLRF";
            break;
        default:
            resp = "Unkown";
    }
    return resp;
}
char * method_string(enum request_method type){
    char * resp;
    switch(type) {
        case METHOD_GET:
            resp = "GET";
            break;
        case METHOD_HEAD:
            resp = "HEAD";
            break;
        case METHOD_POST:
            resp = "POST";
            break;
        case METHOD_NOTSUPPORTED:
            resp = "Method not supported";
            break;
        default:
            resp = "ERROR_hostData";
            break;
    }
    return resp;
}
