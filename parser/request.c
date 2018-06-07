

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>




#include "headerGroup.h"
#include "multi_parser.h"
#include "request.h"

enum header_name{
    HEADER_HOST,
    HEADER_CONT_LEN,
    HEADER_TRANSF_ENC,
    HEADER_NOT_INTERESTED

};


char * * headerNames  = ( char *[]){"Host"      ,"Content-Length","Transfer-Encoding"};
int      types[] = {            HEADER_HOST, HEADER_CONT_LEN,HEADER_TRANSF_ENC};
#define HEADERS_AMOUNT 3

enum body_type
getTransfEncoding(char * value){

    struct multi_parser p;
    enum transf_types{
        transf_chunk,
        transf_notIntrested,

    };
    char * * transfNames  = ( char *[]){"Chunked"      };
    int      types[] = {            body_type_chunked};
    int len = 1;
    int i;

    for(i=0;value[i] !=0 && value[i]==' ';i++);

    multi_parser_init(&p,body_type_identity,transfNames,types,len);

    enum body_type result= multi_parser_consume(value+i,&p);

    multi_parser_close(&p);

    return result;

}


int parseInt(const char * b){
    int num = -1;
    int i = 0;

    if(b == NULL){
        return -1;
    }

    while(b[i] != 0){
        if(isdigit(b[i])){
            if(num == -1){
                num = 0;
            }
            num = num*10 + b[i]-'0';
        }else if(b[i] != ' '){
            return -1;
        }
        i++;
    }

    return num;

}

enum body_type
getBodyType(struct header_list * list){

    if(list == NULL){
        return body_type_error;
    }
    if(list->name == HEADER_TRANSF_ENC ){
        return getTransfEncoding(list->value);
    }
    return getBodyType(list->next);
}

/* Searches in header_list for header Content-Length
 * returns -1 if it's not present or it's value is in an incorrect format
 */
int
getContentLength(struct header_list *list){
    if(list == NULL){
        return -1;
    } else if(list->name == HEADER_CONT_LEN){
        return parseInt(list->value);
    }
    return getContentLength(list->next);
}

char *
getHeaderValue(struct header_list * list, enum header_name name){
    if(list == NULL){
        return NULL;
    }
    if(list->name == name){
        return list->value;
    }
    return getHeaderValue(list->next,name);
}

enum request_state
requestLine(const uint8_t c,struct request_parser *p) {
    enum request_state next;
    enum requestLine_state state =requestLine_parser_feed(c,p->requestLineParser);
    switch(state){
        case rl_end:
            p->method = p->requestLineParser->method;
            strcpy(p->requestURI,p->requestLineParser->uri);

            hostData result = requestTarget_marshall(p->requestURI,p->fqdn,0xFF,&(p->port));

            if(result == ERROR_hostData || result == EMPTY){
                p->hasDestination = false;
            }else{
                p->hasDestination = true;
                fillRequestData_marshall(result,p->fqdn,p->port,&p->destintation);
            };

            requestLine_parser_close(p->requestLineParser);
            free(p->requestLineParser);
            p->requestLineParser=NULL;

            headerGroup_parser_init(p->headerParser,HEADER_NOT_INTERESTED,headerNames,types,HEADERS_AMOUNT);
            next = request_headers;
            break;
        default:
            next = request_requestLine;
            break;
    }
    return next;
}

enum request_state
headers(const uint8_t c,struct request_parser *p){
    enum request_state next;
    enum headerGroup_state state = headerGroup_parser_feed(c,p->headerParser);
    switch (state) {
        case headerGroup_end:
            p->headerList=p->headerParser->list;
            headerGroup_parser_close(p->headerParser);
            free(p->headerParser);
            p->headerParser=NULL;
            if(p->method == METHOD_POST){

                int type = getBodyType(p->headerList);
                int len = getContentLength(p->headerList);

                if(!p->hasDestination){
                    char * value = getHeaderValue(p->headerList,HEADER_HOST);
                    hostData result = requestTarget_marshall(value,p->fqdn,0xFF,&(p->port));

                    if(result == ERROR_hostData){
                        p->hasDestination = false;
                    }else{
                        p->hasDestination = true;
                    };
                }
                p->bodyParser = malloc(sizeof (struct body_parser));
                body_parser_init(p->bodyParser,type,len);

                next = request_body;
            }else{
                next = request_done;
            }
            break;
        case headerGroup_error:
            next = request_error;
            break;
        default:
            if(!p->hasDestination){
                p->headerList=p->headerParser->list;
                char * value = getHeaderValue(p->headerList,HEADER_HOST);
                if(value!=NULL) {
                    hostData result = requestTarget_marshall(value, p->fqdn, 0xFF, &(p->port));

                    if (result == ERROR_hostData) {
                        p->hasDestination = false;
                    } else {
                        p->hasDestination = true;
                        fillRequestData_marshall(result,p->fqdn,p->port,&p->destintation);
                    };
                }
            }
            next = request_headers;
            break;
    }

    return next;
}


enum request_state
body(const uint8_t c,struct request_parser *p) {
    enum request_state next;
    enum body_state state= body_parser_feed(c,p->bodyParser);

    switch(state){
        case body_end:
            body_parser_close(p->bodyParser);
            free(p->bodyParser);
            p->bodyParser=NULL;
            next = request_done;
            break;
        case body_error:
            next = request_error;
            break;
        default:
            next = request_body;
            break;
    }

    return next;
}

enum request_state
done( const uint8_t c,struct request_parser *p) {
    enum request_state next;
    next = request_error;
    return next;
}


void
request_parser_init(struct request_parser *p) {

    p->state = request_requestLine;
    p->requestLineParser = malloc(sizeof(struct requestLine_parser));
    p->headerParser = malloc(sizeof(struct headerGroup_parser));
    p->hasDestination =false;

    requestLine_parser_init(p->requestLineParser);
}



enum request_state
request_parser_feed (const uint8_t c, struct request_parser *p){
    enum request_state next;

    switch(p->state) {
        case request_requestLine:
            next = requestLine(c,p);
            break;
        case request_headers:
            next = headers(c,p);
            break;
        case request_body:
            next = body(c,p);
            break;
        case request_done:
            next = done(c,p);
            break;
        default:
            next = request_error;
            break;
    }
    return p->state = next;
}

void
request_parser_close(struct request_parser *p){
    if(p != NULL){
        header_list_destroy(p->headerList);
    }

}


char *
request_state_string(enum request_state state){
    char * resp;
    switch(state) {
        case request_requestLine:
            resp = "Request Line";
            break;
        case request_headers:
            resp = "Headers";
            break;
        case request_body:
            resp = "Body";
            break;
        case request_done:
            resp = "Done";
            break;
        case request_error:
            resp = "Error";
            break;
        default:
            resp = "Unkown";
            break;
    }
    return resp;
}



enum request_state
request_parser_consume(struct request_parser *p, char * b,int len){
    enum request_state st = p->state;
    int i;
    for(i =0 ; i<len ; i++){
        st = request_parser_feed(b[i],p);
    }

    return st;
}
