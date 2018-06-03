//
// Created by Segundo Fariña on 30/5/18.
//

#ifndef PC_2018_04_BODY_H
#define PC_2018_04_BODY_H

#include <stdint.h>
#include "identity.h"
#include "chunkGroup.h"

enum body_state{
    body_init,
    body_chunked,
    body_identity,
    body_notIntrested,
    body_end,
    body_error
};

enum body_type{
    body_type_identity,
    body_type_chunked,
    body_type_unknown,
    body_type_error
};
struct body_parser{
    enum body_state state;

    struct identity_parser *identityParser;
    struct chunkGroup_parser * chunkGroupParser;

};


extern void
body_parser_init (struct body_parser* p,enum body_type type,int len);

extern void
body_parser_close(struct body_parser* p);

enum body_state
body_parser_feed(const uint8_t c,struct body_parser* p);


char *
body_state_string(enum body_state st);

#endif //PC_2018_04_BODY_H
