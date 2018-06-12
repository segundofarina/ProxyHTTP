//
// Created by Segundo Fariña on 31/5/18.
//

#include <stdio.h>
#include <string.h>
#include "statusLine.h"

int main(){
    struct statusLine_parser p;

    statusLine_parser_init(&p);

    char * string = "HTTP/1.1 204 No Content\r\n";
    enum statusLine_state state;
    for (int i=0; string[i];i++){
         state=statusLine_parser_feed(string[i],&p);
    }


    printf("state: %s code: %d",statusLine_state_string(state),p.code);

    statusLine_parser_close(&p);
}
