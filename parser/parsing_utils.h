//
// Created by Segundo Fariña on 11/6/18.
//

#ifndef PC_2018_04_PARSING_UTILS_H
#define PC_2018_04_PARSING_UTILS_H

#include <stdint.h>

struct header_list{
    uint32_t name;
    char * value;

    struct header_list * next;
};

char *
getHeaderValue(struct header_list * list, uint32_t name);


#endif //PC_2018_04_PARSING_UTILS_H

