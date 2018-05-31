#include <stdlib.h>
#include <ctype.h>
#include <memory.h>
#include <printf.h>
#include "chunk.h"
#include "chunkGroup.h"

void chunkGroup_stateToString(enum chunk_group_state state){
    switch(state){
        case chunk_group_chunk: printf("  state = chunk  ");break;
        case chunk_group_error: printf("  state = error  ");break;
        case chunk_group_done: printf("  state = done  ");break;
    }
}

void chunkGroup_parser_init (struct chunkGroup_parser*  p){
    p->bytes_read = 0;
    p->chunk_quantity = 0;
    p->state  = chunk_group_chunk;
    p->cp = malloc(sizeof(struct chunk_parser));
    chunk_parser_init(p->cp);
}

void chunkGroup_parser_feed(const char c,struct chunkGroup_parser *p){

    chunk_parser_feed(c,p->cp);

    if(p->cp->state == chunk_error){
        p->state = chunk_group_error;
        chunk_parser_close(p->cp);
        return;
    }else if(p->cp->state == chunk_done) {

        p->bytes_read += p->cp->bytes_declared;

        if (p->cp->last) {
            p->state = chunk_group_done;
            chunk_parser_close(p->cp);
            return;
        }
        p->chunk_quantity++;
        chunk_parser_init(p->cp);
    }

}

void chunkGroup_parser_consume(const char *b,struct chunkGroup_parser *p){
    int i = 0;
    while(b[i]!= 0) {

        printf("   c = %c", b[i]);
        chunkGroup_stateToString(p->state);
        printf("%d",p->bytes_read);
        printf(" %d\n",p->chunk_quantity);

        chunkGroup_parser_feed(b[i],p);
        i++;
    }
}

void chunkGroup_parser_close(struct chunkGroup_parser* p){
    free(p);
}

int main(void){
    struct chunkGroup_parser *  p = malloc(sizeof(struct chunkGroup_parser));
    chunkGroup_parser_init(p);
//    const char * b = "0\r\n"
//                     "\r\n";
    const char * b = "4\r\n"
                     "Wiki\r\n"
                     "5\r\n"
                     "pedia\r\n"
                     "E\r\n"
                     " in\r\n"
                     "\r\n"
                     "chunks.\r\n"
                     "0\r\n"
                     "\r\n";
    chunkGroup_parser_consume(b,p);
    chunkGroup_parser_close(p);
}
