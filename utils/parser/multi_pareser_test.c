//
// Created by Segundo Fariña on 27/5/18.
//

#include <stdlib.h>
#include "multi_parser.h"
#include <stdio.h>
#include <string.h>

int main(){
     char * *list = ( char *[]){"Foo","Fooster","Fooo","foooo", "fooj","Not matching"};


    struct multi_parser * p = malloc(sizeof(struct multi_parser));


    enum types {
        foo=0,
        precious,
        when,
        you,
        smile,
        not_matched
    };

    int types [] ={foo,precious,when,you,smile,not_matched};

    multi_parser_init(p,not_matched,list,types,5);

    char * string = "Fooster";

    size_t i;
    enum types state;
    for(i =0 ; i< strlen(string) ;i++){
        printf("Active parsers before feeding %d\n",activeParsers(p));
        state = multi_parser_feed(string[i],p);
        char letter = string[i];
        if(letter == '\n'){
            letter = 'N';
        }else if (letter == '\r'){
            letter = 'R';
        }
        printf("feeded %c state now is %s\n",letter,list[state]);
    }

    method_parser_close(p);
}
