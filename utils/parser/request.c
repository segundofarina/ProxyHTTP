//
// Created by Florencia Cavallin on 26/5/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#define NULL 0

#include "request.h"

void request_init(struct request_parser *p) {
    memset(p->origin_dest, 0, sizeof(*(p->origin_dest)));
    p->state = request_line;
}

enum request_state request_feed (struct request_parser *p, const uint8_t c){
    enum request_state next;

    switch(p->state) {
        case request_line:
            next = requestLine(p,c);
            break;
        case headers:
            next = requestHeaders(p,c);
            break;
        case body:
            next = requestBody(p,c);
            break;
        case done:
            next = requestDone(p,c);
            break;
        default:
            next = error;
            break;
    }
    return p->state = next;
}

enum request_state request_consume(struct request_parser *p, buffer * b){
    enum request_state st = p->state;
    while(buffer_can_read(b)) {
        const uint8_t c = buffer_read(b);
        st = request_feed(p, c);
        if(st == done || st == error) {
            break;
        }
    }
    return st;
}