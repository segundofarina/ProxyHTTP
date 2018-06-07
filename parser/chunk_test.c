//
// Created by Florencia Cavallin on 31/5/18.
//
#include <stdlib.h>
#include <ctype.h>
#include <memory.h>
#include <printf.h>
#include "chunk.h"

//int main(void){
//    // this chunk is valid
//    struct chunk_parser *  chunk1 = malloc(sizeof(struct chunk_parser *));
//    chunk_parser_init(chunk1);
//    const char * c1 = "4\r\n"
//                      "Wiki\r\n";
//    chunk_parser_consume(c1,chunk1);
//
//    // this chunk is valid
//    struct chunk_parser *  chunk2 = malloc(sizeof(struct chunk_parser *));
//    chunk_parser_init(chunk2);
//    const char * c2 = "5\r\n"
//                      "pedia\r\n";
//    chunk_parser_consume(c2,chunk2);
//
//    // this chunk is valid
//    struct chunk_parser *  chunk3 = malloc(sizeof(struct chunk_parser *));
//    chunk_parser_init(chunk3);
//    const char * c3 = "E\r\n"
//                      " in\r\n"
//                      "\r\n";
//    chunk_parser_consume(c3,chunk3);
//
//    // this chunk is valid
//    struct chunk_parser *  chunk4 = malloc(sizeof(struct chunk_parser *));
//    chunk_parser_init(chunk4);
//    const char * c4 = "0\r\n"
//                      "\r\n";
//    chunk_parser_consume(c4,chunk4);
//    printf(chunk4->last?"True":"False");
//
//    // this chunk is invalid
//    struct chunk_parser *  chunk5 = malloc(sizeof(struct chunk_parser *));
//    chunk_parser_init(chunk5);
//    const char * c5 = "\r\n"
//                      "\r\n";
//    chunk_parser_consume(c5,chunk5);
//    chunk_stateToString(chunk5->state);
//}
