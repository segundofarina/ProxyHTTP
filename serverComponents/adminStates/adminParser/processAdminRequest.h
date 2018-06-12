//
// Created by Florencia Cavallin on 10/6/18.
//

#ifndef PC_2018_04_PROCESSADMINREQUEST_H
#define PC_2018_04_PROCESSADMINREQUEST_H
#include <stdint.h>
#include "../../../utils/buffer/buffer.h"
#include <memory.h>
#include "../adminReadState.h"
#include "../adminErrorState.h"
#include "../../transformationManager.h"
#include "../../metrics.h"
#include <arpa/inet.h>

#define MAX_DATA 256
#define QUANTITY_METRICS 5
#define QUANTITY_MEDIATYPES 15
#define SIZE_CHAR sizeof(uint8_t)
#define SIZE_INTEGER SIZE_CHAR*4

enum client_request{
    SET_TRANSFORMATION=0x00,
    ADD_MEDIA_TYPE=0x01,
    REMOVE_MEDIA_TYPE=0x02,
    SET_BUFFER_SIZE=0x03,
    SET_TIMEOUT=0x04,
    GET_METRICS=0x05,
    GET_TRANSFORMATION=0x06,
    GET_MEDIA_TYPES=0x07,
    GET_BUFFER_SIZE=0x08,
    GET_TIMEOUT=0x09
};

enum admin_error_code processAdminRequest(buffer * buff);

#endif //PC_2018_04_PROCESSADMINREQUEST_H
