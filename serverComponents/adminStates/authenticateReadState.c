#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "authenticateReadState.h"

#include "../admin-structure.h"
#include "../adminStm.h"
#include "../adminActiveHandlers.h"
#include "../../utils/buffer/buffer.h"
#include "../../parser/request.h"
#include "adminErrorState.h"
#include "adminReadState.h"

int createResponse(buffer * buff,enum auth_response code) {
    size_t i = 0;
    buffer_reset(buff);
    uint8_t  * msg = buffer_write_ptr(buff,&i);

    if(i == 0){
        return 0;
    }
    msg[0] = code;
    buffer_write_adv(buff,1);
    return 1;

}

enum auth_response processAuthentication(buffer * buff) {
    size_t i = 0;
    uint8_t * ptr = buffer_read_ptr(buff,&i);
    char pwd[256] = {0};
    size_t len = ptr[AUTH_LENGHT_PWD];
    memcpy(pwd,ptr+AUTH_PWD,len);
    buffer_read_adv(buff,len);
    enum auth_response ans;

    if(i < len || (strncmp(pwd, PASSWORD,len) != 0)){
        ans = LOGIN_FAILED;
    }else{
        ans = LOGGED_IN;
    }
    int aux = createResponse(buff,ans);
    if(aux == 0){
        return LOGIN_FAILED;
    }
    return LOGGED_IN;
}

unsigned authenticateRead(struct selector_key * key) {
    struct AdminConn * conn = DATA_TO_ADMIN(key);
    uint8_t *ptr;
    size_t count;
    ssize_t n;

    ptr = buffer_write_ptr(&conn->buff, &count);
    n = recv(conn->adminFd, ptr, count, 0);
    if(n <= 0) {
        /* Admin closed connection */
        return ADMIN_FATAL_ERROR;
    }
    buffer_write_adv(&conn->buff, n);

    enum auth_response error = processAuthentication(&conn->buff);

    if(error == LOGGED_IN) {
        if(selector_set_interest_key(key, OP_WRITE) != SELECTOR_SUCCESS) {
            return ADMIN_FATAL_ERROR;
        }
        return AUTHENTICATE_WRITE;
    }
    return adminSetError(key, error);
}
