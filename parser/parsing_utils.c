//
// Created by Segundo Fariña on 11/6/18.
//
#include "parsing_utils.h"
#include <stddef.h>

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
