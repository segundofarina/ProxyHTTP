//
// Created by Segundo Fariña on 10/6/18.
//

#ifndef PC_2018_04_RESPONSE_MANAGER_H
#define PC_2018_04_RESPONSE_MANAGER_H

#include "response.h"
enum manager_state {
    manager_statusLine,
    manager_headers,
    manager_addingHeaders,
    manager_body,
    manager_done,
    manager_error
};

#define maxNameBuffer 20

struct response_manager{
    enum manager_state state;
    enum manager_state prevState;
    struct response_parser parser;
    bool transfActive;



    char headerNameBuffer[maxNameBuffer];
    bool hasBeenDumped;
    int headerBufferLen;

    int addHeadersIndex;
    char * headersAdd;
    int headersAddLen;
};


extern enum manager_state
manager_parser_consume(struct response_manager *manager, char *readBuff, int *read, char *writeBuff, int *written);

extern void
manager_parser_init(struct response_manager *manager, enum request_method method);

extern enum manager_state
manager_parser_feed(struct response_manager *manager,const uint8_t c, bool * consumed, char *writebuff, int *written, int maxWrite);

extern void
manager_parser_close(struct response_manager *manager);

extern void
manager_parser_setTransformation(struct response_manager *m,bool active);

extern void
manager_parser_getMediaType(struct response_manager * manager,char * buffer,int max);

#endif //PC_2018_04_RESPONSE_MANAGER_H
