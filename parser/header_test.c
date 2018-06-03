//
// Created by Segundo Fariña on 28/5/18.
//

#include <stdlib.h>
#include "header.h"
#include "stdio.h"
#include "string.h"

enum header_name{
    HEADER_HOST,
    HEADER_CONT_LEN,
    HEADER_NOT_INTERESTED
};

char * header_name_string(int name){
    char * resp;
    switch(name){
        case  HEADER_HOST:
            resp= "Host";
            break;
        case  HEADER_CONT_LEN:
            resp = "Content-Length";
            break;
        case   HEADER_NOT_INTERESTED:
            resp = "Not Intrested";
            break;
        default:
            resp ="Unkown";
            break;
    }
    return resp;
}


void consume(char * string, struct header_parser * p){
    size_t i;
    enum header_state state;
    for(i =0 ; i< strlen(string) ;i++){
        state = header_parser_feed(string[i],p);
        char letter = string[i];
        if(letter == '\n'){
            letter = 'N';
        }else if (letter == '\r'){
            letter = 'R';
        }
        printf("feeded %c state now is %s\n",letter,header_state_string(state));
    }
    printf("------------- Header name is: %s ---------------\n",header_name_string(p->name));
    printf("------------- Header value is: %s ---------------\n",p->value);
}




int main(){


    struct header_parser * p = malloc(sizeof(struct header_parser));

    char * * list  = ( char *[]){"Host"      ,"Content-Length"};
    int      types[] = {            HEADER_HOST, HEADER_CONT_LEN};
    //int valueMaxLen[] ={1000,1000};
    int len  =2;
    header_parser_init(p,HEADER_NOT_INTERESTED,list,types,len);


    consume("Date: Tue, 29 May 2018 01:48:49 GMT\r\n",p);
    header_parser_reset(p);

    consume("Expires: -1\r\n",p);
    header_parser_reset(p);

    consume("Cache-Control: private, max-age=0\r\n",p);
    header_parser_reset(p);

    consume("Content-Type: text/html; charset=ISO-8859-1\r\n",p);
    header_parser_reset(p);

    consume("P3P: CP=\"This is not a P3P policy! See g.co/p3phelp for more info.\"\r\n",p);
    header_parser_reset(p);

    consume("Server: gws\r\n",p);
    header_parser_reset(p);

    consume("X-XSS-Protection: 1; mode=block\r\n",p);
    header_parser_reset(p);

    consume("X-Frame-Options: SAMEORIGIN\r\n",p);
    header_parser_reset(p);

    consume("Set-Cookie: 1P_JAR=2018-05-29-01; expires=Thu, 28-Jun-2018 01:48:49 GMT; path=/; domain=.google.com\r\n",p);
    header_parser_reset(p);

    consume("Set-Cookie: NID=131=Vvzuk4U7voUrvQt3n1bkKMrA9UNgbguKLrIdYAP61ZGN0X2I1FLgLBTMvkuFzRlxil1c8BJhF6pR57j2l_lAi1tVU9cx3TVzNsD8IoPXPu2hMr6YORlK-DeXOe77QGXE; expires=Wed, 28-Nov-2018 01:48:49 GMT; path=/; domain=.google.com; HttpOnly\r\n",p);
    header_parser_reset(p);

    consume("Accept-Ranges: none\r\n",p);
    header_parser_reset(p);

    consume("Vary: Accept-Encoding\r\n",p);
    header_parser_reset(p);

    consume("Transfer-Encoding: chunked\r\n",p);
    header_parser_reset(p);


    header_parser_close(p);
}
