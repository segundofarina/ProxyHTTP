//
// Created by Segundo Fariña on 28/5/18.
//

#include "multi_parser.h"
#include "header.h"
#include "parser_errorCodes.h"


#define HEADER_VALUE_LEN 1024




static enum header_state
name(const uint8_t c, struct header_parser* p) {
    enum header_state next;
    switch (c) {
        case ':':
            next = header_value;
            if(p->name == p->nameParser->notMatch){
                p->value = NULL;
            } else{
                p->value= malloc(sizeof(char )* HEADER_VALUE_LEN);
            }
            break;
        default:
            p->name = multi_parser_feed(c,p->nameParser);
            next = header_name;
            break;
    }

    return next;
}

static enum header_state
value(const uint8_t c, struct header_parser* p) {
    enum header_state next;
    switch (c) {
        case '\r':
            if(p->value != NULL){
                p->value[p->len]=0;
            }
            next = header_crlf;
            break;
        default:
            if(p->name != p->nameParser->notMatch) {
                if (p->len < HEADER_VALUE_LEN) {
                    p->value[p->len++] = c;
                }
            }
            next = header_value;
            break;
    }

    return next;
}

static enum header_state
crlf(const uint8_t c, struct header_parser* p) {
    enum header_state next;
    switch (c) {
        case '\n':
            next = header_end;
            break;
        default:
            next = header_error;
            p->errorCode= ERROR_BAD_REQUEST;
            break;
    }

    return next;
}


static enum header_state
end(const uint8_t c, struct header_parser* p) {
    enum header_state next;
    switch (c) {
        default:
            next = header_error;
            p->errorCode= ERROR_BAD_REQUEST;
            break;
    }

    return next;
}

extern void
header_parser_init (struct header_parser* p,const int notMatch, char ** list, const int * matches,const int len) {
    p->nameParser = malloc(sizeof(struct multi_parser));

    p->name = notMatch;
    p->state = header_name;
    p->len =0;
    p->value=NULL;
    multi_parser_init(p->nameParser,notMatch,list,matches,len);


}

extern enum header_state
header_parser_feed(const uint8_t c,struct header_parser* p){
    enum header_state next;

    switch(p->state) {
        case header_name:
            next = name(c ,p);
            break;
        case header_value:
            next = value(c,p);
            break;
        case header_crlf:
            next = crlf(c,p);
            break;
        case header_end:
            next = end(c,p);
            break;
        default:
            next = header_error;
            break;
    }

    return p->state = next;
}

extern void
header_parser_close(struct header_parser* p){
    if(p!=NULL) {
        multi_parser_close(p->nameParser);
        if(p->name != p->nameParser->notMatch){
            free(p->value);
        }
        free(p->nameParser);
        p->nameParser = NULL;
    }
}

extern char *
header_state_string(enum header_state st){
    char * resp;
    switch(st){
        case header_name:
            resp = "Name";
            break;
        case header_value:
            resp = "Value";
            break;
        case header_crlf:
            resp = "CRLF";
            break;
        case header_end:
            resp = "End";
            break;
        case header_error:
            resp = "Error";
            break;
        default:
            resp = "UNOKNW";
            break;
    }
    return resp;

}

extern void
header_parser_reset   (struct header_parser *p){
    p->state=header_name;

    p->name = p->nameParser->notMatch;
    p->state = header_name;
    p->len =0;
    if(p->value!= NULL){
        free(p->value);
    }
    multi_parser_reset(p->nameParser);
}
