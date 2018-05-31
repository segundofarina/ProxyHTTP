//
// Created by Segundo Fariña on 27/5/18.
//

#include "multi_parser.h"
#include <stdio.h>


struct parser_data_list {

    struct parser * p;

    struct parser_definition def;

    u_int32_t match;

    struct parser_data_list * next;
};


extern u_int32_t
multi_parser_feed(const uint8_t c, struct multi_parser* p){

    struct parser_data_list * current =p->list;

    p->currentMatch = p->notMatch;


    while (current != NULL) {
        const struct parser_event *e = parser_feed(current->p, c);

        struct parser_data_list * next= current->next;

        if (e->type == STRING_CMP_EQ) {
            p->currentMatch = current->match;

        }

        current = next;
    }
    return p->currentMatch;

}


extern void
multi_parser_init (struct multi_parser* p,  const int notMatch, char ** strings, const int * matches,const int len) {
    p->notMatch = notMatch;
    p->currentMatch = p->notMatch;
    p->list = NULL;
    struct parser_data_list * current;
    struct parser_data_list * prev = NULL;

    for(int i =0 ; i<len; i++){
        current =malloc(sizeof(struct parser_data_list));;
        if(p->list == NULL){
            p->list = current;
        }
        if(prev != NULL){
            prev->next= current;
        }
        current->next=NULL;

        current->def =parser_utils_strcmpi(strings[i]);
        current->p = parser_init(parser_no_classes(), &(current->def));
        current->match = matches[i];

        prev= current;
    }




}

extern void
multi_parser_close(struct multi_parser* p){

    if( p != NULL){

        parser_list_destroy(p->list);


        free(p);

    }



}

int activeParsers(struct multi_parser * p){
    struct parser_data_list * current =p->list;
    int i=0;
    while(current != NULL){
        i++;
        current = current->next;
    }
    return i;
}

void parser_list_destroy( struct parser_data_list * list){

    if(list == NULL){
        return;
    }

    parser_destroy(list->p);
    parser_utils_strcmpi_destroy(&list->def);
    parser_list_destroy(list->next);
    free(list);

}

static void
parser_list_reset( struct parser_data_list * list){

    if(list == NULL){
        return;
    }
    parser_reset(list->p);
    parser_list_reset(list->next);

}

void
multi_parser_reset (struct multi_parser *p){
    p->currentMatch = p->notMatch;
    parser_list_reset(p->list);
}

