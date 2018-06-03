#include "method.h"



extern void
method_parser_feed(const uint8_t c, struct method_parser* p){
    const struct parser_event * getEvent  = parser_feed(p->parserGet,c);
    const struct parser_event * headEvent = parser_feed(p->parserHead,c);
    const struct parser_event * postEvent = parser_feed(p->parserPost,c);


    if(       getEvent->type == STRING_CMP_EQ){
        p->method = METHOD_GET;
    }else if(headEvent->type == STRING_CMP_EQ){
        p->method = METHOD_HEAD;
    }else if(postEvent->type == STRING_CMP_EQ){
        p->method = METHOD_POST;
    }else{
        p->method = METHOD_NOTSUPPORTED;
    }
    
}


extern void
method_parser_init (struct method_parser* p) {
    p->method = METHOD_NOTSUPPORTED;

    p->dHead = parser_utils_strcmpi("HEAD");
    p->parserHead = parser_init(parser_no_classes(), &(p->dHead));

    p->dGet = parser_utils_strcmpi("GET");
    p->parserGet = parser_init(parser_no_classes(), &(p->dGet));

    p->dPost = parser_utils_strcmpi("POST");
    p->parserPost = parser_init(parser_no_classes(), &(p->dPost));
}

extern void
method_parser_close(struct method_parser* p){

    if( p != NULL){
        parser_destroy(p->parserGet);
        parser_utils_strcmpi_destroy((&p->dGet));

        parser_destroy(p->parserHead);
        parser_utils_strcmpi_destroy((&p->dHead));

        parser_destroy(p->parserPost);
        parser_utils_strcmpi_destroy((&p->dPost));

        free(p);

    }
   

    
}

void
methodd_parser_reset(struct method_parser *p) {
    parser_reset(p->parserGet);
    parser_reset(p->parserHead);
    parser_reset(p->parserPost);
}


char * methodToString(enum request_method type){
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
            resp = "ERROR";
            break;
    }
    return resp;
}

enum request_method
method_parser_consume(const char *b,size_t len ,struct method_parser *p){
    for (size_t i = 0; i<len; ++i)
   {
   	    method_parser_feed(b[i],p);
   }
   return p->method;
}

