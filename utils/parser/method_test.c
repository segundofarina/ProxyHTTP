#include "method.h"
#include <stdio.h>
#include <string.h>

int method_test(){


    struct method_parser * parser= malloc(sizeof(struct method_parser));

    method_parser_init(parser);

    char * string = "HEADagsgagsh";

    method_parser_consume(string,strlen(string),parser);

    printf("string:%s , Parsed:%s\n",string,methodToString(parser->method));

    

    methodd_parser_reset(parser);

    string = "GET";

    method_parser_consume(string,strlen(string),parser);

    printf("string:%s, Parsed:%s\n",string,methodToString(parser->method));


    methodd_parser_reset(parser);

    string = "POST";

    method_parser_consume(string,strlen(string),parser);
    
    printf("string:%s , Parsed:%s\n",string,methodToString(parser->method));



    methodd_parser_reset(parser);

    string = "HEAD";

    method_parser_consume(string,strlen(string),parser);
   
    printf("string:%s , Parsed:%s\n",string,methodToString(parser->method));


    method_parser_close(parser);



   

}