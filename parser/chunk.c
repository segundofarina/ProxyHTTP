//
// Created by Florencia Cavallin on 28/5/18.
//

#include <stdlib.h>
#include <ctype.h>
#include <memory.h>
#include <stdio.h>
#include "chunk.h"

void chunk_stateToString(enum chunk_state state){
    switch(state){
        case chunk_bytes: printf(" state = chunk_bytes "); break;
        case chunk_extension: printf(" state = chunk_extension "); break;
        case chunk_crlf: printf(" state = chunk_crlf "); break;
        case chunk_data: printf(" state = chunk_data "); break;
        case chunk_done: printf(" state = chunk_done "); break;
        case chunk_error: printf(" state = chunk_error "); break;
    }
}

void chunk_parser_init (struct chunk_parser * p){
    p->bytes_declared = -1;
    p->bytes_read = 0;
    p->state = chunk_bytes;
    p->last = false;
    p->shouldKeep = false;
}

void chunk_parser_feed(const char c, struct chunk_parser* p){

    switch(p->state){
        case chunk_bytes: bytesDeclared(c,p); break;
        case chunk_extension: chunkExtension(c,p); break;
        case chunk_data: countData(c,p); break;
        case chunk_crlf: breakLine(c,p); break;
        case chunk_done: break;
        case chunk_error: break;
    }
    return;
}

void chunkExtension(const char c, struct chunk_parser *p){
    if(c == '\r'){
        p->prev = chunk_extension;
        p->state = chunk_crlf;
        p->shouldKeep = false;
        return;
    }
}

void breakLine(const char c, struct chunk_parser *p){
    if(c == '\n'){
        p->shouldKeep = false;
        switch(p->prev){
            case chunk_bytes: p->state = chunk_data; break;
            case chunk_extension:  p->state = chunk_data; break;
            case chunk_data: p->state = chunk_done; break;
            default: p->state = chunk_error;
        }
        return;
    }
    p->state = chunk_error;
}

void countData(const char c, struct chunk_parser *p) {
    if(p->bytes_read < p->bytes_declared) {
        p->shouldKeep = true;
        p->bytes_read++;
        return;
    }
    if(c == '\r'){
        p->shouldKeep =false;
        p->prev = chunk_data;
        p->state = chunk_crlf;
        return;
    }
    p->state = chunk_error;
}

void bytesDeclared(const char c, struct chunk_parser* p){

    if(isalnum(c)){
        p->bytes_declared = sum(p->bytes_declared,c);
        if(p->bytes_declared == -1){
            p->state = chunk_error;
            return;
        }
        if(p->bytes_declared == 0){
            p->last = true;
        }
        return;
    }
    if(c == ';'){
        p->state = chunk_extension;
        return;
    }
    if(c == '\r' && p->bytes_declared != -1){
        p->prev = chunk_bytes;
        p->state = chunk_crlf;
        return;
    }
    p->state = chunk_error;
}
int sum(int declared, const char c) {

    int aux=-1;

    if(c >= 'A' && c <= 'F'){
        aux = c - 'A' + 10;
    }else if(c >= 'a' && c <= 'f'){
        aux = c - 'a' + 10;
    }else if(c >= '0' && c <= '9'){
        aux = c - '0';
    }

    if(aux == -1){
        return -1;
    }

    if(declared == -1){
        declared = 0;
    }

    return declared*16 + aux;
}

enum chunk_state chunk_parser_consume(const char *b, struct chunk_parser *p){
    int i = 0;
    while(b[i]!= 0) {
        if(b[i] == '\n'){
            printf("   c = \\n");
        }else if(b[i] == '\r'){
            printf("   c = \\r");
        }else {
            printf("   c = %c", b[i]);
        }
        chunk_parser_feed(b[i],p);
        chunk_stateToString(p->state);
        printf("%d\n",p->last);
        if(p->state == chunk_done || p->state == chunk_error){
            break;
        }
        i++;
    }
    return p->state;
}


void chunk_parser_close(struct chunk_parser* p){
    free(p);
}




