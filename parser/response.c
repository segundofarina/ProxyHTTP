//
// Created by Segundo Fariña on 31/5/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>


#include "headerGroup.h"
#include "multi_parser.h"
#include "response.h"
#include "statusLine.h"
#include "parser_errorCodes.h"


/* Taken from RFC 7230 section 3.3
 *  "The presence of a message body in a response depends on both the
   request method to which it is responding and the response status code
   (Section 3.1.2).  Responses to the HEAD request method (Section 4.3.2
   of [RFC7231]) never include a message body because the associated
   response header fields (e.g., Transfer-Encoding, Content-Length,
   etc.), if present, indicate only what their values would have been if
   the request method had been GET (Section 4.3.1 of [RFC7231]). 2xx
   (Successful) responses to a CONNECT request method (Section 4.3.6 of
   [RFC7231]) switch to tunnel mode instead of having a message body.
   All 1xx (Informational), 204 (No Content), and 304 (Not Modified)
   responses do not include a message body.  All other responses do
   include a message body, although the body might be of zero length."
   */

enum header_name {
    HEADER_CONT_LEN,
    HEADER_TRANSF_ENC,
    HEADER_NOT_INTERESTED,
    HEADER_CONT_ENCONDING,
    HEADER_CONNECTION,
    HEADER_MEDIA_TYPE

};

char **headerNamesResponse = (char *[]) {"Content-Length", "Transfer-Encoding", "Content-Encoding", "Connection","Media-Type"};
int typesResponse[] = {HEADER_CONT_LEN, HEADER_TRANSF_ENC, HEADER_CONT_ENCONDING, HEADER_CONNECTION,HEADER_MEDIA_TYPE};
enum header_name ignoeredResponse[] = {HEADER_CONT_LEN, HEADER_CONNECTION,HEADER_TRANSF_ENC};
#define HEADERS_AMOUNT 5
#define HEADER_IGNORED 3

extern char *
getMediaType(struct response_parser * p){
    return getHeaderValue(p->headerList,HEADER_MEDIA_TYPE);
}




extern bool
isIgnored(uint32_t name) {
    for (int i = 0; i < HEADER_IGNORED; i++) {
        if ((enum header_name)name == ignoeredResponse[i]) {
            return true;
        }
    }
    return false;
}

bool
expectsBody(int statusCode, enum request_method method){
    if(method == METHOD_HEAD){
        return false;
    }else if(statusCode>= 100 && statusCode<200){
        return false;
    }else if (statusCode == 204 || statusCode== 304){
        return false;
    }

    return true;
}



enum body_type
getTransfEncodingResponse(char * value){

    struct multi_parser p;
    enum transf_types {
        transf_chunk,
        transf_notIntrested,

    };
    char **transfNames = (char *[]) {"Chunked"};
    int types[] = {body_type_chunked};
    int len = 1;
    int i;

    for (i = 0; value[i] != 0 && value[i] == ' '; i++);

    multi_parser_init(&p, body_type_identity, transfNames, types, len);

    enum body_type result = multi_parser_consume(value + i, &p);

    multi_parser_close(&p);

    return result;
}


int parseIntResponse(const char * b){
    int num = -1;
    int i = 0;

    if (b == NULL) {
        return -1;
    }

    while (b[i] != 0) {
        if (isdigit(b[i])) {
            if (num == -1) {
                num = 0;
            }
            num = num * 10 + b[i] - '0';
        } else if (b[i] != ' ') {
            return -1;
        }
        i++;
    }

    return num;

}

enum body_type
getBodyTypeResponse(struct header_list * list){
    if (list == NULL) {
        return body_type_error;
    }
    if (list->name == HEADER_TRANSF_ENC) {
        return getTransfEncodingResponse(list->value);
    } else if (list->name == HEADER_CONT_ENCONDING) {
        return getTransfEncodingResponse(list->value);
    }
    return getBodyTypeResponse(list->next);
}

/* Searches in header_list for header Content-Length
 * returns -1 if it's not present or it's value is in an incorrect format
 */
int
getContentLengthResponse(struct header_list *list){
    if(list == NULL){
        return -1;
    } else if(list->name == HEADER_CONT_LEN){
        return parseIntResponse(list->value);
    }
    return getContentLengthResponse(list->next);
}


static enum response_state
statusLine(const uint8_t c, struct response_parser *p) {
    enum response_state next;
    enum statusLine_state state = statusLine_parser_feed(c, p->statusLineParser);
    switch (state) {
        case sl_end:

            statusLine_parser_close(p->statusLineParser);
            free(p->statusLineParser);
            p->statusLineParser=NULL;
            headerGroup_parser_init(p->headerParser,HEADER_NOT_INTERESTED,headerNamesResponse,typesResponse,HEADERS_AMOUNT);
            next = response_headers;
            break;
        default:
            next = response_statusLine;
            break;
    }
    return next;
}

static enum response_state
headersResponse(const uint8_t c,struct response_parser *p){
    enum response_state next;
    enum headerGroup_state state = headerGroup_parser_feed(c, p->headerParser);
    switch (state) {
        case headerGroup_end:
            p->headerList = p->headerParser->list;
            headerGroup_parser_close(p->headerParser);
            free(p->headerParser);
            p->headerParser=NULL;


            if(expectsBody(p->statusCode,p->method)){
                int type = getBodyTypeResponse(p->headerList);
                int len = getContentLengthResponse(p->headerList);
                if(type == body_type_chunked || len >0){

                    p->bodyParser = malloc(sizeof(struct body_parser));
                    body_parser_init(p->bodyParser, type, len);

                    next = response_body;
                } else if(len ==0){
                    next = response_done;
                }else{
                    p->errorCode = ERROR_BAD_REQUEST;
                    next = response_error;
                }
            }else{
                next =response_done;
            }

            break;
        case headerGroup_error:
            next = response_error;
            break;
        default:
            next = response_headers;
            break;
    }

    return next;
}


static enum response_state
bodyResponse(const uint8_t c,struct response_parser *p) {
    enum response_state next;
    enum body_state state = body_parser_feed(c, p->bodyParser);
    p->shouldKeepLastChar = p->bodyParser->shouldKeep;

    switch (state) {
        case body_end:
            body_parser_close(p->bodyParser);
            free(p->bodyParser);
            p->bodyParser = NULL;
            next = response_done;
            break;
        case body_error:
            next = response_error;
            break;
        default:
            next = response_body;
            break;
    }

    return next;
}

static enum response_state
doneResponse( const uint8_t c,struct response_parser *p) {
    enum response_state next;
    next = response_error;
    return next;
}


extern void
response_parser_init(struct response_parser *p, enum request_method method) {

    p->state = response_statusLine;
    p->statusLineParser = malloc(sizeof(struct statusLine_parser));
    p->headerParser = malloc(sizeof(struct headerGroup_parser));

    p->headerList = NULL;
    p->shouldKeepLastChar = false;
    p->method = method;
    statusLine_parser_init(p->statusLineParser);

}


extern enum response_state
response_parser_feed(const uint8_t c, struct response_parser *p) {
    enum response_state next;

    switch (p->state) {
        case response_statusLine:
            next = statusLine(c, p);
            break;
        case response_headers:
            next = headersResponse(c,p);
            break;
        case response_body:
            next = bodyResponse(c,p);
            break;
        case response_done:
            next = doneResponse(c,p);
            break;
        default:
            next = response_error;
            break;
    }
    return p->state = next;
}

extern void
response_parser_close(struct response_parser *p) {
    if (p != NULL) {
        header_list_destroy(p->headerList);
        p->headerList=NULL;
    }

}


extern char *
response_state_string(enum response_state state) {
    char *resp;
    switch (state) {
        case response_statusLine:
            resp = "Status Line";
            break;
        case response_headers:
            resp = "Headers";
            break;
        case response_body:
            resp = "Body";
            break;
        case response_done:
            resp = "Done";
            break;
        case response_error:
            resp = "Error";
            break;
        default:
            resp = "Unkown";
            break;
    }
    return resp;
}


//extern enum response_state
//response_parser_consume(struct response_parser *p, char *b, int *len, char *writebuff, int *written) {
//    int i, j;
//
//    for (i = 0, j = 0; i < *len; i++) {
//        p->prevState = p->state;
//        response_parser_feed(b[i], p);
//
//
//        if (p->state == response_headers &&
//            (p->headerParser->state == headerGroup_header || p->headerParser->state == headerGroup_init)) {
//
//            enum header_state state = p->headerParser->headerParser->state;
//            switch (state) {
//                case header_name:
//
//                    if (p->headerBufferLen < 20) {
//                        p->headerNameBuffer[p->headerBufferLen++] = b[i];
//                        p->hasBeenDumped=false;
//                    } else {
//                        if (!p->hasBeenDumped) {
//                            memcpy(writebuff + j, p->headerNameBuffer, p->headerBufferLen);
//                            p->hasBeenDumped = true;
//                            j += p->headerBufferLen;
//                            p->headerBufferLen = 0;
//                        }
//
//                        writebuff[j++] = b[i];
//
//                    }
//                    break;
//                case header_value:
//                    if (!isIgnored((enum header_name) p->headerParser->headerParser->nameParser->currentMatch)) {
//                        if (!p->hasBeenDumped) {
//                            memcpy(writebuff + j, p->headerNameBuffer, p->headerBufferLen);
//                            p->hasBeenDumped = true;
//                            j += p->headerBufferLen;
//                        }
//                        writebuff[j++] = b[i];
//                    } else {
//                        p->hasBeenDumped = true;
//                    }
//                    break;
//
//                default:
//                    if (!isIgnored((enum header_name) p->headerParser->headerParser->nameParser->currentMatch)) {
//                        writebuff[j++] = b[i];
//                    }
//                    p->hasBeenDumped = false;
//                    p->headerBufferLen = 0;
//                    break;
//            }
//
//
//        } else if (p->prevState == response_body) {
//
//            if (p->shouldKeepLastChar) {
//                writebuff[j++] = b[i];
//            }
//        } else {
//            writebuff[j++] = b[i];
//        }
//
//
//        if (p->prevState == response_headers && p->state == response_body) {
//            // breaking so the proxy knows the body has begun
//            int addlen= strlen(headersAdd);
//            memcpy(writebuff+j-2,headersAdd, addlen);
//            j+=addlen;
//            i++;
//            break;
//        }
//        if (p->state == response_error) {
//            //printf("[RESPONSE PARSER]The parser state is  response_error\n");
//            i++;
//            break;
//        }
//    }
//
//
//
//    *written = j;
//    *len = i;
//
//
//    return p->state;
//}
