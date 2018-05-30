//
// Created by Segundo Fariña on 28/5/18.
//

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "headerGroup.h"
#include "multi_parser.h"


void header_list_destroy(struct header_list *pList);


enum headerGroup_state
init(const uint8_t c, struct headerGroup_parser *p);

enum headerGroup_state
header(const uint8_t c, struct headerGroup_parser *p);

enum headerGroup_state
crlf(const uint8_t c, struct headerGroup_parser *p);

enum headerGroup_state
end(const uint8_t c, struct headerGroup_parser *p);

struct header_list *
header_list_add(struct header_list * list , u_int32_t name, char * value, int len);


enum headerGroup_state init(const uint8_t c, struct headerGroup_parser *p) {
    enum headerGroup_state next;
    switch(c){
        case '\r':
            next = headerGroup_crlf;
            break;
        default:
            header_parser_reset(p->headerParser);

            next = header(c,p);


    }
    return next;
}

enum headerGroup_state header(const uint8_t c, struct headerGroup_parser *p) {
    enum headerGroup_state next;
    enum header_state state= header_parser_feed(c,p->headerParser);
    switch(state){
        case header_end:
            if(p->headerParser->name != p->headerParser->nameParser->notMatch){
               p->list= header_list_add( p->list,p->headerParser->name,p->headerParser->value,p->headerParser->len);
            }

            next = headerGroup_init;
            break;
        case header_error:
            //???que hago??
        default:
            next = headerGroup_header;

    }
    return next;
}

enum headerGroup_state crlf(const uint8_t c, struct headerGroup_parser *p) {
    enum headerGroup_state next;
    switch (c) {
        case '\n':
            next = headerGroup_end;
            break;
        default:
            next = headerGroup_error;
            break;
    }
    return next;
}

enum headerGroup_state end(const uint8_t c, struct headerGroup_parser *p) {
    enum headerGroup_state next;
    switch (c) {
        default:
            next = headerGroup_error;
            break;
    }
    return next;
}








extern void
headerGroup_parser_init (struct headerGroup_parser* p,const int notMatch, char ** strings, const int * matches,const int len){
    p->state = headerGroup_init;
    p->list = NULL;
    p->headerParser = malloc(sizeof(struct header_parser));
    header_parser_init(p->headerParser,notMatch,strings,matches,len);
}

enum headerGroup_state
headerGroup_parser_feed(const uint8_t c,struct headerGroup_parser* p){
    enum headerGroup_state next;

    switch(p->state) {
        case headerGroup_init:
            next = init(c ,p);
            break;
        case headerGroup_header:
            next = header(c,p);
            break;
        case headerGroup_crlf:
            next = crlf(c,p);
            break;
        case headerGroup_end:
            next = end(c,p);
            break;
        default:
            next = headerGroup_error;
            break;
    }

    return p->state = next;
}

extern void
headerGroup_parser_close(struct headerGroup_parser* p){
    // header_list_destroy(p->list);
}

char *
headerGroup_state_string(enum headerGroup_state st){
    char * resp;
    switch(st){
        case headerGroup_init:
            resp= "Init";
            break;
        case headerGroup_header:
            resp= "Header";
            break;
        case headerGroup_crlf:
            resp= "CRLF";
            break;
        case headerGroup_end:
            resp= "End";
            break;
        case headerGroup_error:
            resp= "Error";
            break;
        default:
            resp = "Unknown";
            break;
    }
    return resp;
}

void
header_list_destroy(struct header_list *pList) {
    if(pList == NULL){
        return;
    }else{
        header_list_destroy(pList->next);
        free(pList->value);
        free(pList);
    }
}
struct header_list *
header_list_add(struct header_list * list , u_int32_t name, char * value, int len){
    if(list == NULL){
        list = malloc(sizeof(struct header_list));
        list->name = name;
        list->value = malloc(sizeof(char)*(len+1));
        memcpy(list->value,value,len);
        list->value[len]=0;
        list->next=NULL;
        return list;
    }
    list->next =header_list_add(list->next,name,value,len);
    return list;
}

void
header_list_print(struct header_list * list){
    if(list == NULL){
        return;
    }
    printf("Header name: '%d' , Header value: '%s'\n",list->name,list->value);
    header_list_print(list->next);
}
