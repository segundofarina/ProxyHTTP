//
// Created by Segundo Fariña on 30/5/18.
//

#include <stdlib.h>
#include "string.h"
#include <stdio.h>
#include "body.h"


void consume(const char * string, struct body_parser * p){
    size_t i;
    enum body_state state;
    for(i =0 ; i< strlen(string) ;i++){
        state = body_parser_feed(string[i],p);
        char letter = string[i];
        if(letter == '\n'){
            letter = 'N';
        }else if (letter == '\r'){
            letter = 'R';
        }
        printf("feeded %c state now is %s\n",letter,body_state_string(state));
    }

    //printf("Header value is %s",p->value);
}
int main(){


    struct body_parser * p = malloc(sizeof(struct body_parser));


    const char * body =    "0...\n"
                        "......0....\t+.....0......0...0......Qh.....u<..edb...Yr;..20180531162401Z0s0q0I0\t..+.........&....~...B../j..._\n"
                        "..Qh.....u<..edb...Yr;..\n"
                        "...s.S...V..x.\\....20180531162401Z....20180607153901Z0\n"
                        ".\t*.H..\n"
                        "..........i8.$..aD..~.o..?CJ..o.........x..}.9.y.D....Ch.{I.. .q..A.$%..m..z..mm...H.g..x{a^....>..;.....0.{.o.\".O.[P(..%.!......u</..~-w...u..c.\n"
                        "8r..4.v.S.[.[..N.....^.....8..X..3.+:.6^..........C ..h.p.........P...EhOnLzO...P.X.............@....5........!.......^&";

    printf("===========================This should be valid==================\n");
    int contentLength = strlen(body);





    body_parser_init(p,body_type_identity,contentLength);

    consume(body,p);

    body_parser_close(p);

    printf("===========================This should be valid==================\n");

    const char * cg1 = "4\r\n"
                       "Wiki\r\n"
                       "5\r\n"
                       "pedia\r\n"
                       "E\r\n"
                       " in\r\n"
                       "\r\n"
                       "chunks.\r\n"
                       "0\r\n"
                       "\r\n";

    body_parser_init(p,body_type_chunked,-1);

    consume(cg1,p);

    body_parser_close(p);




    printf("===========================This should be valid==================\n");
    //this chunk group is valid
    const char * cg2 = "0\r\n"
                       "\r\n";
    body_parser_init(p,body_type_chunked,-1);

    consume(cg2,p);

    body_parser_close(p);

    //this chunk group is invalid

    printf("===========================This should be error==================");
    const char * cg3 = "\r\n"
                       "\r\n";
    body_parser_init(p,body_type_chunked,-1);

    consume(cg3,p);

    body_parser_close(p);


}
