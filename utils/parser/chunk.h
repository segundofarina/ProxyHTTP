//
// Created by Florencia Cavallin on 28/5/18.
//

#ifndef PC_2018_04_CHUNK_H
#define PC_2018_04_CHUNK_H

#include <unitypes.h>
#include <stdbool.h>

enum chunk_state {
    chunk_bytes,
    chunk_extension,
    chunk_crlf,
    chunk_data,
    chunk_done,
    chunk_error
};

struct chunk_parser{
    int bytes_declared;
    int bytes_read;
    bool last;
    enum chunk_state state;
    enum chunk_state prev;
};

void chunk_parser_init (struct chunk_parser * p);

void chunk_parser_feed(const char c, struct chunk_parser* p);

void chunkExtension(const char c, struct chunk_parser *p);

void breakLine(const char c, struct chunk_parser *p);

void countData(const char c, struct chunk_parser *p);

void bytesDeclared(const char c, struct chunk_parser* p);

int sum(int declared, const char c);

enum chunk_state chunk_parser_consume(const char *b,struct chunk_parser *p);

void chunk_parser_close(struct chunk_parser* p);

void chunk_stateToString(enum chunk_state state);



#endif //PC_2018_04_CHUNK_H
