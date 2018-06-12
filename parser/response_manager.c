//
// Created by Segundo Fariña on 10/6/18.
//
#include <stdio.h>
#include "response.h"
#include "string.h"
#include "headerGroup.h"
#include "multi_parser.h"
#include "response_manager.h"


char * headersAdd = "Transfer-Encoding: Chunked\r\nConnection: close\r\n\r\n";


extern void
manager_parser_getMediaType(struct response_manager * manager,char * buffer,int max){

    char * value = getHeaderValue(manager->parser.headerList,HEADER_MEDIA_TYPE);
    if(value==NULL){
        buffer[0]=0;
        return;
    }
    int i;
    for(i=0;value[i] && value[i]==' ';i++);
    strncpy(buffer,value+i,max);
}

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
    int toWrite;
    if(maxWrite >= manager->headersAddLen  -  manager->addHeadersIndex){
        toWrite  = manager->headersAddLen  -  manager->addHeadersIndex;
    } else{
        toWrite = maxWrite;
    }

    memcpy(writebuff,manager->headersAdd+manager->addHeadersIndex, toWrite);
    *written=toWrite;
    *consumed=false;
    manager->addHeadersIndex+=toWrite;
    if(manager->addHeadersIndex == manager->headersAddLen){
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
    if (!manager->transfActive || manager->parser.shouldKeepLastChar) {
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

extern void
manager_parser_setTransformation(struct response_manager *m,bool active){
    m->transfActive=active;
    if(active){
        m->headersAdd=headersAdd;
    }else{
        int len=0;
        enum header_name * list = getIgnoredHeaders(&len);
        m->headersAddLen=0;
        m->headersAdd=NULL;
        for(int i=0;i<len;i++){
            char * value = getHeaderValue(m->parser.headerList,list[i]);

            if(value!=NULL){
                int valueLen=strlen(value);
                char *name = getHeaderName(list[i]);
                int nameLen = strlen(name);
                m->headersAdd=realloc(m->headersAdd,(m->headersAddLen+valueLen+nameLen+3)* sizeof(char));

                memcpy(m->headersAdd+m->headersAddLen,name,nameLen);
                m->headersAddLen+=nameLen;
                m->headersAdd[m->headersAddLen++]=':';
                memcpy(m->headersAdd+m->headersAddLen,value,valueLen);
                m->headersAddLen+=valueLen;
                m->headersAdd[m->headersAddLen++]='\r';
                m->headersAdd[m->headersAddLen++]='\n';
            }
        }
        m->headersAdd=realloc(m->headersAdd,(m->headersAddLen+2)* sizeof(char));
        m->headersAdd[m->headersAddLen++]='\r';
        m->headersAdd[m->headersAddLen++]='\n';
    }
    m->addHeadersIndex=0;
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
    manager->transfActive=true;
    manager->headersAdd=headersAdd;
    manager->headersAddLen=strlen(headersAdd);
    response_parser_init(&manager->parser,method);
}

extern void
manager_parser_close(struct response_manager *manager){
    response_parser_close(&manager->parser);
    if(manager->headersAdd!=headersAdd){
        free(manager->headersAdd);
    }

    manager->headersAdd=NULL;
}
