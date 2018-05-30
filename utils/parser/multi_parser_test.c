//
// Created by Segundo Fariña on 27/5/18.
//

#include <stdlib.h>
#include "multi_parser.h"
#include <stdio.h>
#include <string.h>


void consume(char * string, struct multi_parser * p,char ** list);

enum types {
    foo=0,
    precious,
    when,
    you,
    smile,
    not_matched
};

int main(){
     char * *list = ( char *[]){"Foo1","Foo2","When","you", "smile","Not matching"};


    struct multi_parser * p = malloc(sizeof(struct multi_parser));




    int types [] ={foo,precious,when,you,smile,not_matched};

    multi_parser_init(p,not_matched,list,types,5);


    consume("Foo1",p,list);

    multi_parser_reset(p);

    consume("Foo",p,list);

    multi_parser_reset(p);

    consume("yOu",p,list);

    multi_parser_reset(p);

    consume("SMilEE",p,list);

    multi_parser_close(p);
}


void consume(char * string, struct multi_parser * p,char ** list){
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
}
