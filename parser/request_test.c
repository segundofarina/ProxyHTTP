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

    printf("Printing list....\n");
    if(p->headerList == NULL){
        printf("LIST IS NULL\n");
    }
    header_list_print(p->headerList);

    printf("Method: %d\n",p->method);
    printf("request Uri :%s\n",p->requestURI);
    printf("FQDN:'%s'\n",p->fqdn);
    printf("port: %d\n",p->port);

    printf("Host type:%d\n",p->destintation.destAddrType);
    printf("Host port:%d\n",p->destintation.destPort);
    switch (p->destintation.destAddrType){
        case IPv4:
            //printf("Host address:%d\n",(int )p->destintation.destAddr.ipv4);
            break;
        case IPv6:
            //printf("Host address:%d\n",p->destintation.destAddr.ipv6);
            break;
        case DOMAIN:
            printf("Host address:%s\n",p->destintation.destAddr.fqdn);
            break;
        default:
            break;
    }

}



int main(){

    char * encoding = "         chUnKed";

    enum body_type type =getTransfEncoding(encoding);

    printf("Encoding %d\n",type);


    struct request_parser  p ;


    char * GET =
                        "GET /description.xml HTTP/1.1\r\n"
                        "Host: 10.0.0.2\r\n"
                        "Connection: keep-alive\r\n"
                        "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/65.0.3325.181 Safari/537.36\r\n"
                        "Accept-Encoding: gzip, deflate\r\n"
                        "\r\n";

    printf("====================GET REQUEST=====================\n");
    request_parser_init(&p);

    consume(GET,&p);

    request_parser_close(&p);

    char * POST = "POST /paw-2018a-4/search HTTP/1.1\r\n"
                  "Host: pawserver.it.itba.edu.ar\r\n"
                  "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:60.0) Gecko/20100101 Firefox/60.0\r\n"
                  "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
                  "Accept-Language: en-GB,en;q=0.5\r\n"
                  "Accept-Encoding: gzip, deflate\r\n"
                  "Referer: http://pawserver.it.itba.edu.ar/paw-2018a-4/\r\n"
                  "Content-Type: application/x-www-form-urlencoded\r\n"
                  "Content-Length: 20\r\n"
                  "Cookie: JSESSIONID=8EB339926ED5FC9E7B1A95EF0A425CFB; _ga=GA1.3.1246883313.1521487511; __utma=22901091.1246883313.1521487511.1521487512.1521487512.1; __utmz=22901091.1521487512.1.1.utmcsr=(direct)|utmccn=(direct)|utmcmd=(none); __zlcmid=lVhCfAkVJlo6Ot\r\n"
                  "Connection: keep-alive\r\n"
                  "Upgrade-Insecure-Requests: 1\r\n"
                  "\r\n"
                  "city=2&serviceType=1";


    printf("====================POST REQUEST=====================\n");

    request_parser_init(&p);

    consume(POST,&p);

    request_parser_close(&p);


}
