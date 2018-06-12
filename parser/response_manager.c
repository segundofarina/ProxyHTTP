//
// Created by Segundo Fariña on 10/6/18.
//
#include <stdio.h>
#include "response.h"
#include "string.h"
#include "headerGroup.h"
#include "multi_parser.h"
#include "response_manager.h"


char * headersAdd = {"Transfer-Encoding: Chunked\r\nConnection: close\r\n\r\n"};


static enum manager_state
statusLine(struct response_manager *manager,const uint8_t c, bool * consumed, char *writebuff, int *written, int maxWrite) {
    enum manager_state next;
    enum response_state state =response_parser_feed(c,&manager->parser);
    switch(state){
        case response_statusLine:
            next = manager_statusLine;
            break;
        case response_headers:
            next = manager_headers;
            break;
        default:
            next = manager_error;
            break;
    }

    *consumed = true;
    writebuff[0]=c;
    *written=1;
    return next;
}



static enum manager_state
headers(struct response_manager *manager,const uint8_t c, bool * consumed, char *writebuff, int *written, int maxWrite){
    enum manager_state next;
    enum response_state state =response_parser_feed(c,&manager->parser);
    switch(state){
        case response_headers:
            next = manager_headers;
            break;
        case response_body:
        case response_done:
            next = manager_addingHeaders;
            break;
        default:
            next = manager_error;
            break;
    }

    struct response_parser *p = &manager->parser;

    *written = 0;
    if(p->state == response_headers &&
       (p->headerParser->state == headerGroup_header || p->headerParser->state == headerGroup_init)) {

        enum header_state state = p->headerParser->headerParser->state;
        switch (state) {
            case header_name:

                if (manager->headerBufferLen < maxNameBuffer) {
                    manager->headerNameBuffer[manager->headerBufferLen++] = c;
                    manager->hasBeenDumped = false;
                } else {
                    if (!manager->hasBeenDumped) {
                        memcpy(writebuff, manager->headerNameBuffer, manager->headerBufferLen);
                        manager->hasBeenDumped = true;
                        *written = manager->headerBufferLen;
                        manager->headerBufferLen = 0;
                    }

                    writebuff[(*written)++] = c;

                }
                break;
            case header_value:
                if (!isIgnored(p->headerParser->headerParser->nameParser->currentMatch)) {
                    if (!manager->hasBeenDumped) {
                        memcpy(writebuff, manager->headerNameBuffer, manager->headerBufferLen);
                        manager->hasBeenDumped = true;
                        *written = manager->headerBufferLen;
                    }
                    writebuff[(*written)++] = c;
                } else {
                    manager->hasBeenDumped = true;
                }
                break;

            default:
                if (!isIgnored(p->headerParser->headerParser->nameParser->currentMatch)) {
                    writebuff[(*written)++] = c;
                }
                manager->hasBeenDumped = false;
                manager->headerBufferLen = 0;
                break;
        }


    }else{
        //Header Parser state will be in final /r/n Won't write them as they will be written at the end of
        // addingHeaders state

        *written=0;
    }
    *consumed = true;



    return next;
}

static enum manager_state
addingHeaders(struct response_manager *manager,const uint8_t c, bool * consumed, char *writebuff, int *written, int maxWrite){
    enum manager_state next;
    int addlen= strlen(headersAdd);
    int toWrite;
    if(maxWrite >= addlen-manager->addHeadersIndex){
        toWrite = addlen-manager->addHeadersIndex;
    } else{
        toWrite = maxWrite;
    }

    memcpy(writebuff,headersAdd+manager->addHeadersIndex, toWrite);
    *written=toWrite;
    *consumed=false;
    manager->addHeadersIndex+=toWrite;
    if(manager->addHeadersIndex == addlen){
        next = manager_body;
    }else{
        next = manager_addingHeaders;
    }

    return next;
}



static enum manager_state
body(struct response_manager *manager,const uint8_t c, bool * consumed, char *writebuff, int *written, int maxWrite) {
    enum manager_state next;
    enum response_state state =response_parser_feed(c,&manager->parser);

    switch (state) {
        case response_body:
            next = manager_body;
            break;
        case response_done:
            next = manager_done;
            break;
        default:
            next =manager_error;
            break;
    }
    if (manager->parser.shouldKeepLastChar) {
        writebuff[0]=c;
        *written=1;
    }else{
        *written=0;
    }
    *consumed = true;
    return next;
}

static enum manager_state
done( struct response_manager *manager,const uint8_t c, bool * consumed, char *writebuff, int *written, int maxWrite) {
    enum manager_state next;
    next = manager_error;
    *consumed=false;
    *written=0;

    return next;
}



extern enum manager_state
manager_parser_feed(struct response_manager *manager,const uint8_t c, bool * consumed, char *writebuff, int *written, int maxWrite){
    enum manager_state next;

    switch (manager->state) {
        case manager_statusLine:
            next = statusLine(manager,c,consumed,writebuff,written,maxWrite);
            break;
        case manager_addingHeaders:
            next = addingHeaders(manager,c,consumed,writebuff,written,maxWrite);
            break;
        case manager_headers:
            next = headers(manager,c,consumed,writebuff,written,maxWrite);
            break;
        case manager_body:
            next = body(manager,c,consumed,writebuff,written,maxWrite);
            break;
        case manager_done:
            next = done(manager,c,consumed,writebuff,written,maxWrite);
            break;
        default:
            next = manager_error;
            break;
    }
    return manager->state = next;
}



extern enum manager_state
manager_parser_consume(struct response_manager *manager, char *readBuff, int *read, char *writeBuff, int *written) {

    int maxRead = *read;
    int maxWrite = *written;
    bool consumed = false;
     *read = 0;
     *written = 0;
    while((manager->state == manager_addingHeaders &&*written<maxWrite) ||(*read<maxRead && *written<maxWrite)){
        int wirtteninfeed =0;
        manager->prevState=manager->state;
        manager_parser_feed(manager,readBuff[*read],&consumed,writeBuff+*written,&wirtteninfeed,maxWrite-*written);
        if(consumed){
            (*read)++;
        }
        *written+=wirtteninfeed;
        if(manager->prevState == manager_headers && manager->state == manager_addingHeaders){
            //breaking so the proxy can change the read buffers
            break;
        }
        if(manager->prevState == manager_addingHeaders && manager->state == manager_body){
            //breaking so the proxy can change the read buffers
            break;
        }

    }


    return manager->state;
}
extern void
manager_parser_init(struct response_manager *manager, enum request_method method){
    manager->state= manager_statusLine;
    manager->hasBeenDumped = false;
    manager->addHeadersIndex=0;
    response_parser_init(&manager->parser,method);
}

extern void
manager_parser_close(struct response_manager *manager){
    response_parser_close(&manager->parser);
}
