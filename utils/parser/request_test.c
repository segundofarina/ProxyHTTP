//
// Created by Segundo Fariña on 29/5/18.
//

#include <stdlib.h>
#include <stdio.h>
#include "request.h"
#include <string.h>
#include "headerGroup.h"
#include "body.h"

void consume(char * string, struct request_parser * p){
    size_t i;
    enum request_state state;
    for(i =0 ; i< strlen(string) ;i++){
        state = request_parser_feed(string[i],p);
        char letter = string[i];
        if(letter == '\n'){
            letter = 'N';
        }else if (letter == '\r'){
            letter = 'R';
        }
        printf("feeded %c state now is %s\n",letter,request_state_string(state));
    }

    //printf("Header value is %s",p->value);
    printf("Printing list....\n");
    if(p->headerList == NULL){
        printf("LIST IS NULL\n");
    }
    header_list_print(p->headerList);

    printf("Method: %d\n",p->method);
    printf("request Uri :%s",p->requestURI);
}



int main(){

    char * encoding = "         chUnKed";

    enum body_type type =getTransfEncoding(encoding);

    printf("Encoding %d\n",type);


    char * request =
                        "GET /description.xml HTTP/1.1\r\n"
                        "Host: 10.0.0.2\r\n"
                        "Connection: keep-alive\r\n"
                        "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/65.0.3325.181 Safari/537.36\r\n"
                        "Accept-Encoding: gzip, deflate\r\n"
                        "\r\n";


    struct request_parser * p = malloc(sizeof(struct request_parser));


    request_parser_init(p);

    consume(request,p);

    request_parser_close(p);


}
