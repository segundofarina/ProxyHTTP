//
// Created by Florencia Cavallin on 10/6/18.
//

#include "processAdminRequest.h"

int createResponse(buffer * buff,enum admin_error_code code, uint8_t len) {
    int i = 0;
    buffer_reset(buff);
    uint8_t  * msg = buffer_write_ptr(buff,&i);

    if((len+REQUEST_DATA) > i){
        return 0;
    }
    msg[i++] = code;
    msg[i++] = len;

    buffer_write_adv(buff,REQUEST_DATA);
    return 1;

}

int getTransformation(buffer * buff){
    enum admin_error_code responseStatus = ADMIN_NO_ERROR;
    char * t = getCurrentTransformation();
    uint8_t responseLen = strlen(t);
    uint8_t aux = 0;
    int ans = createResponse(buff,responseStatus,responseLen);

    if(ans == 0){
        return 0;
    }

    uint8_t * ptr = buffer_write_ptr(buff,&aux);

    if(aux < responseLen){
        return 0;
    }
    memcpy(ptr,t,aux);
    buffer_write_adv(buff,responseLen);

    return aux;

}

int getMetrics(buffer * buff){

    enum admin_error_code responseStatus = ADMIN_NO_ERROR;
    uint8_t responseLen = QUANTITY_METRICS;
    uint8_t aux = 0;
    int ans = createResponse(buff,responseStatus,responseLen);

    if(ans == 0){
        return 0;
    }

    uint8_t * ptr = buffer_write_ptr(buff,&aux);

    if(aux < responseLen){
        return 0;
    }

    int i = 0;

    uint32_t gets = htonl(getAmountOfGet());
    memcpy(ptr+i, &gets, SIZE_INTEGER);
    i+=SIZE_INTEGER;

    uint32_t posts = htonl(getAmountOfPost());
    memcpy(ptr+i, &posts, SIZE_INTEGER);
    i+=SIZE_INTEGER;

    uint32_t heads = htonl(getAmountOfHead());
    memcpy(ptr+i, &heads, SIZE_INTEGER);
    i+=SIZE_INTEGER;

    uint32_t clients = htonl(getActiveClients());
    memcpy(ptr+i, &clients, SIZE_INTEGER);
    i+=SIZE_INTEGER;

    uint32_t historicClients = htonl(getHistoricClients());
    memcpy(ptr+i, &historicClients, SIZE_INTEGER);
    i+=SIZE_INTEGER;

    buffer_write_adv(buff,i);

    return i;
}

int getMediaTypes(buffer * buff){

    enum admin_error_code responseStatus = ADMIN_NO_ERROR;
    uint8_t responseLen = QUANTITY_MEDIATYPES;
    uint8_t aux = 0;
    int ans = createResponse(buff,responseStatus,responseLen);

    if(ans == 0){
        return 0;
    }

    uint8_t * ptr = buffer_write_ptr(buff,&aux);

    if(aux < responseLen){
        return 0;
    }

    int i = 0;
    struct mediaTypesNode * node = getMediaTypesList();
    while(node != NULL){
        ptr[i++] = node->mediaType;
        node = node->next;
    }
    return i;
}

enum admin_error_code processAdminRequest(buffer * buff) {
    int read = 0;
    uint8_t * ptr = buffer_read_ptr(buff,&read);

    if(read < REQUEST_DATA){
        return ADMIN_REQ_ERR;
    }

    uint8_t method = ptr[REQUEST_METHOD];
    uint8_t len = ptr[REQUEST_LENGTH_DATA];
    uint8_t data[MAX_DATA+1] = {0};
    memcpy(data,ptr+REQUEST_DATA,len);
    buffer_read_adv(buff,REQUEST_DATA+len);

    enum admin_error_code responseStatus;
    uint8_t responseLen = 0;

    int ans;

    switch(method) {
        case SET_TRANSFORMATION:
            data[len] = 0;
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
            //HACER
            break;
        case SET_TIMEOUT:
            //HACER
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
            //HACER
            break;
        case GET_TIMEOUT:
            //HACER
            break;
        default:
            responseStatus = ADMIN_NOT_SUPPORTED_ERROR;
            ans = createResponse(buff,responseStatus,responseLen);
    }

   if(ans==0){
        return  ADMIN_NO_ERROR;
    }else{
       return ADMIN_INTERNAL_SERVER_ERR;
    }

}








