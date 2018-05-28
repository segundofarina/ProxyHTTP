
#include "requestLine.h"
#include <stdio.h>

static enum requestLine_state
method(const uint8_t c, struct requestLine_parser* p) {
    enum requestLine_state next;
    switch (c) {
        case ' ':
            // Guardar el method que quedo en p->method_parser.method
            p->method=p->methodparser->method;

            //No se si hacerlo aca  o hacer una buena maquina de estdos con un on deprature
            method_parser_close(p->methodparser);
            next = rl_target;
            break;
        default:
            method_parser_feed(c,p->methodparser);
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
            p->uri[p->index] = 0;
            next = rl_extra;
            break;
        default:
            //guardar el target en estructura
            if(p->index <0xFF){
                p->uri[p->index++] = c;
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
requestLine_parser_feed (struct requestLine_parser* p, const uint8_t c) {
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
    p->index =0;
    p->methodparser = malloc(sizeof(struct method_parser));

    method_parser_init(p->methodparser);

}


void
requestLine_close(struct requestLine_parser *p){
    if(p != NULL){
        method_parser_close(p->methodparser);
        free(p);
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
       state = requestLine_parser_feed(p,buffer[i]);
        char letter = buffer[i];
        if(letter == '\n'){
            letter = 'N';
        }else if (letter == '\r'){
            letter = 'R';
        }
       printf("feeded %c state now is %s\n",letter,requestLine_state_toString(state));
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
