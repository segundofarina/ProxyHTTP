//
// Created by Florencia Cavallin on 27/5/18.
//

#ifndef PC_2018_04_chunk_group_H
#define PC_2018_04_chunk_group_H

#include <stdint.h>

enum chunk_group_state{
    chunk_group_chunk,
    chunk_group_done,
    chunk_group_error
};

struct chunkGroup_parser {

    int bytes_read;
    int chunk_quantity;
    enum chunk_group_state state;
    struct chunk_parser * cp;
};

void
chunkGroup_parser_init (struct chunkGroup_parser*  p);

void
chunkGroup_parser_consume(const char *b,struct chunkGroup_parser *p);

void
chunkGroup_parser_close(struct chunkGroup_parser* p);

void
chunkGroup_stateToString(enum chunk_group_state state);

void
chunkGroup_parser_feed(const char c,struct chunkGroup_parser *p);

#endif //PC_2018_04_chunk_group_H
