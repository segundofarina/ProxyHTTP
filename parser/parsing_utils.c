//
// Created by Segundo Fariña on 11/6/18.
//
#include "parsing_utils.h"
#include <stddef.h>
#include <ctype.h>

char *
getHeaderValue(struct header_list * list, uint32_t name){
    if(list == NULL){
        return NULL;
    }
    if(list->name == name){
        return list->value;
    }
    return getHeaderValue(list->next,name);
}


int parseIntResponse(const char * b){
    int num = -1;
    int i = 0;

    if (b == NULL) {
        return -1;
    }

    while (b[i] != 0) {
        if (isdigit(b[i])) {
            if (num == -1) {
                num = 0;
            }
            num = num * 10 + b[i] - '0';
        } else if (b[i] != ' ') {
            return -1;
        }
        i++;
    }

    return num;

}


