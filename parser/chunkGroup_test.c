//
// Created by Florencia Cavallin on 31/5/18.
//
#include <stdlib.h>
#include <ctype.h>
#include <memory.h>
#include <printf.h>
#include "chunk.h"
#include "chunkGroup.h"

int main(void){

    //this chunk group is valid
    struct chunkGroup_parser *  chunkGroup1 = malloc(sizeof(struct chunkGroup_parser));
    chunkGroup_parser_init(chunkGroup1);
    const char * cg1 = "4\r\n"
                       "Wiki\r\n"
                       "5\r\n"
                       "pedia\r\n"
                       "E\r\n"
                       " in\r\n"
                       "\r\n"
                       "chunks.\r\n"
                       "0\r\n"
                       "\r\n";
    chunkGroup_parser_consume(cg1,chunkGroup1);

    //this chunk group is valid
    struct chunkGroup_parser *  chunkGroup2 = malloc(sizeof(struct chunkGroup_parser));
    chunkGroup_parser_init(chunkGroup2);
    const char * cg2 = "0\r\n"
                       "\r\n";
    chunkGroup_parser_consume(cg2,chunkGroup2);

    //this chunk group is invalid
    struct chunkGroup_parser *  chunkGroup3 = malloc(sizeof(struct chunkGroup_parser));
    chunkGroup_parser_init(chunkGroup3);
    const char * cg3 = "\r\n"
                       "\r\n";
    chunkGroup_parser_consume(cg3,chunkGroup3);
}
