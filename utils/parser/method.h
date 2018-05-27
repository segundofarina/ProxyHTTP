#ifndef PC_2018_04_METHOD_TYPE_H
#define PC_2018_04_METHOD_TYPE_H

#include "parser_utils.h"
#include <stdlib.h>


enum method_type{
    METHOD_GET,
    METHOD_HEAD,
    METHOD_POST,
    METHOD_NOTSUPPORTED
};

struct method_parser {
    struct parser *parserHead;
    struct parser *parserGet;
    struct parser *parserPost;


    /* Saving definitions so they can be destroyed later */
    struct parser_definition dHead;
    struct parser_definition dGet;
    struct parser_definition dPost;

    enum method_type method;


};

extern void
method_parser_init (struct method_parser* p);

extern void
method_parser_feed(const uint8_t c, struct method_parser* p);

void
methodd_parser_reset(struct method_parser *p);

char * methodToString(enum method_type type);

enum method_type
method_parser_consume(const char *b,size_t len ,struct method_parser *p);

extern void
method_parser_close(struct method_parser* p);

#endif