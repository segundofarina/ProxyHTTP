//
// Created by Segundo Fariña on 27/5/18.
//

#include <stdlib.h>
#include <string.h>
#include "requestLine.h"
#include <stdio.h>

int main(){
    struct requestLine_parser * p = malloc(sizeof(struct requestLine_parser));

    requestLine_parser_init(p);

    char * string = "POhjsdhjdsST google.com HTTP/1.1\r\n";


    enum requestLine_state state =requestLine_parser_consume(string,strlen(string),p,NULL);

    printf("state: %s method: %s uri: %s",requestLine_state_toString(state),methodToString(p->method),p->uri);
}
