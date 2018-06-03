//
// Created by Segundo Fariña on 28/5/18.
//

#include <stdlib.h>
#include "headerGroup.h"
#include "string.h"
#include <stdio.h>


enum header_name{
    HEADER_HOST,
    HEADER_CONT_LEN,
    HEADER_TRANSF_ENC,
    HEADER_CONTENT_TYPE,
    HEADER_NOT_INTERESTED

};

void consume(char * string, struct headerGroup_parser * p){
    size_t i;
    enum headerGroup_state state;
    for(i =0 ; i< strlen(string) ;i++){
        state = headerGroup_parser_feed(string[i],p);
        char letter = string[i];
        if(letter == '\n'){
            letter = 'N';
        }else if (letter == '\r'){
            letter = 'R';
        }
        printf("feeded %c state now is %s\n",letter,headerGroup_state_string(state));
    }

    //printf("Header value is %s",p->value);
    printf("Printing list....\n");
    if(p->list == NULL){
        printf("LIST IS NULL");
    }
    header_list_print(p->list);
}
int main(){

    char * * list  = ( char *[]){"Host"      ,"Content-Length","Transfer-Encoding","Content-Type"};
    int      types[] = {            HEADER_HOST, HEADER_CONT_LEN,HEADER_TRANSF_ENC,HEADER_CONTENT_TYPE};
    int len  =4;

    char * headers =    "Date: Tue, 29 May 2018 01:48:49 GMT\r\n"
                        "Expires: -1\r\n"
                        "Cache-Control: private, max-age=0\r\n"
                        "Content-Type: text/html; charset=ISO-8859-1\r\n"
                        "P3P: CP=\"This is not a P3P policy! See g.co/p3phelp for more info.\"\r\n"
                        "Server: gws\r\n"
                        "X-XSS-Protection: 1; mode=block\r\n"
                        "X-Frame-Options: SAMEORIGIN\r\n"
                        "Set-Cookie: 1P_JAR=2018-05-29-01; expires=Thu, 28-Jun-2018 01:48:49 GMT; path=/; domain=.google.com\r\n"
                        "Set-Cookie: NID=131=Vvzuk4U7voUrvQt3n1bkKMrA9UNgbguKLrIdYAP61ZGN0X2I1FLgLBTMvkuFzRlxil1c8BJhF6pR57j2l_lAi1tVU9cx3TVzNsD8IoPXPu2hMr6YORlK-DeXOe77QGXE; expires=Wed, 28-Nov-2018 01:48:49 GMT; path=/; domain=.google.com; HttpOnly\r\n"
                        "Accept-Ranges: none\r\n"
                        "Vary: Accept-Encoding\r\n"
                        "Transfer-Encoding: chunked\r\n"
                        "\r\n";
    printf("%s",headers);


    struct headerGroup_parser * p = malloc(sizeof(struct headerGroup_parser));


    headerGroup_parser_init(p,HEADER_NOT_INTERESTED,list,types,len);

    consume(headers,p);

    headerGroup_parser_close(p);


}
