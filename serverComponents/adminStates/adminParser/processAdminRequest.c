//
// Created by Florencia Cavallin on 10/6/18.
//
#include <stdio.h>
#include "processAdminRequest.h"

uint32_t getttl(){
    return 5;
}

uint8_t setTimeOut(char * data){
    return 1;
}

uint32_t getbz(){
    return 20;
}

uint8_t setBufferSize(char * data){
    return 1;
}

enum admin_error_code createResponse(buffer * buff,enum admin_error_code code, uint8_t len) {
    size_t i = 0;
    buffer_reset(buff);
    uint8_t  * msg = buffer_write_ptr(buff,&i);

    size_t aux = len+REQUEST_DATA;
    if( aux > i){
        return ADMIN_REQ_ERR;
    }
    i = 0;
    msg[i++] = code;
    msg[i++] = len;

    buffer_write_adv(buff,REQUEST_DATA);
    return code;

}

enum admin_error_code getTimeOut(buffer * buff){
    enum admin_error_code responseStatus = ADMIN_NO_ERROR;
    uint32_t ttl = htonl(getttl());
    size_t responseLen = SIZE_INTEGER;
    size_t aux = 0;
    enum admin_error_code ans = createResponse(buff,responseStatus,responseLen);

    if(ans == ADMIN_REQ_ERR){
        return ans;
    }

    uint8_t * ptr = buffer_write_ptr(buff,&aux);

    if(aux < responseLen){
        return ADMIN_REQ_ERR;
    }

    memcpy(ptr, &ttl, SIZE_INTEGER);

    buffer_write_adv(buff,responseLen);


    ptr = buffer_read_ptr(buff,&aux);

    return responseStatus;
}

enum admin_error_code getBufferSize(buffer * buff){
    enum admin_error_code responseStatus = ADMIN_NO_ERROR;
    uint32_t size = htonl(getbz());
    size_t responseLen = SIZE_INTEGER;
    size_t aux = 0;
    enum admin_error_code ans = createResponse(buff,responseStatus,responseLen);

    if(ans == ADMIN_REQ_ERR){
        return ans;
    }

    uint8_t * ptr = buffer_write_ptr(buff,&aux);

    if(aux < responseLen){
        return ADMIN_REQ_ERR;
    }

    memcpy(ptr, &size, SIZE_INTEGER);

    buffer_write_adv(buff,responseLen);


    ptr = buffer_read_ptr(buff,&aux);

    return responseStatus;
}

enum admin_error_code getTransformation(buffer * buff){
    enum admin_error_code responseStatus = ADMIN_NO_ERROR;
    char * t = getCurrentTransformation();
    size_t responseLen = strlen(t);
    size_t aux = 0;
    enum admin_error_code ans = createResponse(buff,responseStatus,responseLen);

    if(ans == ADMIN_REQ_ERR){
        return ans;
    }

    uint8_t * ptr = buffer_write_ptr(buff,&aux);

    if(aux < responseLen){
        return ADMIN_REQ_ERR;
    }
    memcpy(ptr,t,responseLen);
    ptr[responseLen] = 0;
    buffer_write_adv(buff,responseLen);


    ptr = buffer_read_ptr(buff,&aux);

    return responseStatus;

}

enum admin_error_code getMetrics(buffer * buff){

    enum admin_error_code responseStatus = ADMIN_NO_ERROR;
    size_t aux = 0;
    enum admin_error_code ans = createResponse(buff,responseStatus,QUANTITY_METRICS);

     if(ans == ADMIN_REQ_ERR){
        return ans;
    }

    uint8_t * ptr = buffer_write_ptr(buff,&aux);

     if(aux < QUANTITY_METRICS*SIZE_INTEGER){
        return ADMIN_REQ_ERR;
    }

    int i = 0;

    uint32_t gets = htonl(getAmountOfGet());
    memcpy(ptr+i, &gets, sizeof(gets));
    i+=sizeof(gets);

    uint32_t posts = htonl(getAmountOfPost());
    memcpy(ptr+i, &posts, sizeof(posts));
    i+=sizeof(posts);

    uint32_t heads = htonl(getAmountOfHead());
    memcpy(ptr+i, &heads,  sizeof(heads));
    i+=sizeof(heads);

    uint32_t clients = htonl(getActiveClients());
    memcpy(ptr+i, &clients,  sizeof(clients));
    i+=sizeof(clients);

    uint32_t historicClients = htonl(getHistoricClients());
    memcpy(ptr+i, &historicClients, sizeof(historicClients));
    i+=sizeof(historicClients);

    buffer_write_adv(buff,i);

    return responseStatus;
}

enum admin_error_code getMediaTypes(buffer * buff){

    enum admin_error_code responseStatus = ADMIN_NO_ERROR;
    size_t i = 0;
    uint8_t * ptr = buffer_write_ptr(buff,&i);

    i = 2;
    struct mediaTypesNode * node = getMediaTypesList();
    while(node != NULL){
        ptr[i++] = node->mediaType;
        node = node->next;
    }

    size_t responseLen = i-2;
    
    int ans = createResponse(buff,responseStatus,responseLen);

    if(ans == ADMIN_REQ_ERR){
        return ans;
    }
    
    buffer_write_adv(buff,responseLen);

    return responseStatus;
}

enum admin_error_code processAdminRequest(buffer * buff) {
    size_t read = 0;
    uint8_t * ptr = buffer_read_ptr(buff,&read);

    if(read < REQUEST_DATA){
        return ADMIN_REQ_ERR;
    }

    uint8_t method = ptr[REQUEST_METHOD];

    size_t len = ptr[REQUEST_LENGTH_DATA];

    char data[MAX_DATA+1] = {0};
    memcpy(data,ptr+REQUEST_DATA,len);
    buffer_read_adv(buff,REQUEST_DATA+len);

    enum admin_error_code responseStatus;
    size_t responseLen = 0;

    enum admin_error_code ans;

    switch(method) {
        case SET_TRANSFORMATION:
            if(addTransformation(data) == 0){
                responseStatus = ADMIN_REQ_ERR;
            }else{
                responseStatus = ADMIN_NO_ERROR;
            }
            ans = createResponse(buff,responseStatus,responseLen);
            break;
        case ADD_MEDIA_TYPE:
            if(addMediaType(data[0]) == 0){
                responseStatus = ADMIN_REQ_ERR;
            }else{
                responseStatus = ADMIN_NO_ERROR;
            }
            ans = createResponse(buff,responseStatus,responseLen);
            break;
        case REMOVE_MEDIA_TYPE:
            if(removeMediaType(data[0]) == 0){
                responseStatus = ADMIN_REQ_ERR;
            }else{
                responseStatus = ADMIN_NO_ERROR;
            }
            ans = createResponse(buff,responseStatus,responseLen);
            break;
        case SET_BUFFER_SIZE:
            if(setBufferSize(data) == 0){
                responseStatus = ADMIN_REQ_ERR;
            }else{
                responseStatus = ADMIN_NO_ERROR;
            }
            ans = createResponse(buff,responseStatus,responseLen);
            break;
        case SET_TIMEOUT:
            if(setTimeOut(data) == 0){
                responseStatus = ADMIN_REQ_ERR;
            }else{
                responseStatus = ADMIN_NO_ERROR;
            }
            ans = createResponse(buff,responseStatus,responseLen);
            break;
        case GET_METRICS:
            ans = getMetrics(buff);
            break;
        case GET_TRANSFORMATION:
            ans = getTransformation(buff);
            break;
        case GET_MEDIA_TYPES:
            ans = getMediaTypes(buff);
            break;
        case GET_BUFFER_SIZE:
            ans = getBufferSize(buff);
            break;
        case GET_TIMEOUT:
            ans = getTimeOut(buff);
            break;
        default:
            responseStatus = ADMIN_NOT_SUPPORTED_ERROR;
            ans = createResponse(buff,responseStatus,responseLen);
    }

    switch(ans){
        case ADMIN_NO_ERROR:
            return ans;
        default:
            return ADMIN_INTERNAL_SERVER_ERR;
    }
}








