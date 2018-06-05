#include "adminStm.h"

#include "adminStates/authenticateReadState.h"
#include "adminStates/authenticateWriteState.h"
#include "adminStates/adminReadState.h"
#include "adminStates/adminWriteState.h"

const struct state_definition adminStateHandlers[] = {
    {
        .state = AUTHENTICATE_READ,
        .on_read_ready = authenticateRead
    },{
        .state = AUTHENTICATE_WRITE,
        .on_write_ready = authenticateWrite
    },{
        .state = ADMIN_READ,
        .on_read_ready = adminRead
    },{
        .state = ADMIN_WRITE,
        .on_write_ready = adminWrite
    },{
        .state = ADMIN_DONE
    },{
        .state = ADMIN_ERROR
        //write
    },{
        .state = ADMIN_FATAL_ERROR
    }
};

struct state_definition * getAdminStates() {
    return (struct state_definition *) adminStateHandlers;
}