//
// Created by Florencia Cavallin on 26/5/18.
//

#ifndef PC_2018_04_METHOD_H
#define PC_2018_04_METHOD_H

#include <stdint.h>

enum requestLine_state {
    method,
    target
};

enum request_state requestLine_feed(struct request_parser *p, const uint8_t c);

#endif //PC_2018_04_METHOD_H
