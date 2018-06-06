//
// Created by Segundo Fariña on 28/5/18.
//

#ifndef PC_2018_04_HEADERGROUP_H
#define PC_2018_04_HEADERGROUP_H

#include "header.h"
#include <stdint.h>


enum headerGroup_state{
    headerGroup_init,
    headerGroup_header,
    headerGroup_crlf,
    headerGroup_end,
    headerGroup_error,
};


struct header_list{
    uint32_t name;
    char * value;

    struct header_list * next;
};

struct headerGroup_parser{

    enum headerGroup_state state;

    struct header_list * list;

    struct header_parser * headerParser;


};

extern void
headerGroup_parser_init (struct headerGroup_parser* p,const int notMatch, char ** strings, const int * matches,const int len);

extern void
headerGroup_parser_close(struct headerGroup_parser* p);

enum headerGroup_state
headerGroup_parser_feed(const uint8_t c,struct headerGroup_parser* p);

char *
headerGroup_state_string(enum headerGroup_state st);

void
header_list_print(struct header_list * list);

void
header_list_destroy(struct header_list *pList);

#endif //PC_2018_04_HEADERGROUP_H
