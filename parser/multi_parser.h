//
// Created by Segundo Fariña on 27/5/18.
//

#ifndef PC_2018_04_MULTI_PARSER_H
#define PC_2018_04_MULTI_PARSER_H
#include <stdlib.h>
#include <stdint.h>
#include "parser_utils.h"


struct multi_parser{
    struct parser_data_list * list;
    uint32_t currentMatch;
    
    uint32_t notMatch;
};




extern uint32_t
multi_parser_feed(const uint8_t c, struct multi_parser* p);

extern uint32_t
multi_parser_consume(char * string, struct multi_parser* p);

extern void
multi_parser_init (struct multi_parser* p,  const int notMatch, char ** strings, const int * matches,const int len);

extern void
multi_parser_close(struct multi_parser* p);


void
parser_list_destroy( struct parser_data_list * list);

int
activeParsers(struct multi_parser * p);

/** permite resetear el parser al estado inicial */
void
multi_parser_reset    (struct multi_parser *p);

#endif //PC_2018_04_MULTI_PARSER_H
