//
// Created by Segundo Fariña on 31/5/18.
//

#ifndef PC_2018_04_IDENTITY_H
#define PC_2018_04_IDENTITY_H


#include <stdint.h>

enum identity_state{
    identity_data,
    identity_end,
    identity_error
};
struct identity_parser{
    int endLen;
    int currentLen;

    enum identity_state state;
};
extern void
identity_parser_init(struct identity_parser* p, int endLen);

extern void
identity_parser_close(struct identity_parser* p);

extern enum identity_state
identity_parser_feed(const uint8_t c,struct identity_parser* p);

extern char *
identity_state_string(enum identity_state st);

#endif //PC_2018_04_IDENTITY_H
