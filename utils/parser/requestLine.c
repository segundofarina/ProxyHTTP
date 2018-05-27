
#include "requestLine.h"


static enum requestLine_state
method(const uint8_t c, struct requestLine_parser* p) {
    enum requestLine_state next;
    switch (c) {
        case ' ':
            // Guardar el method que quedo en p->method_parser.method
            next = rl_target;
            break;
        default:
            method_parser_feed(c,p->method_parser);
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
            p->fqdn[p->index] = 0;
            next = rl_extra;
            break;
        default:
            //guardar el target en estructura
            if(index <0xFF){
                p->fqdn[p->index++] = c;
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
requestLine_close(struct request_parser *p){
    if(p != NULL){
        method_parser_close(p->methodparser);
        free(p);
    }

}

bool
requestLine_is_done(const enum requestLine_parser st, bool *errored){
    if(st.state == rl_error && errored!= NULL){
        *errored= true;
    }
    return st.state == rl_end || st.state == rl_error;
}
