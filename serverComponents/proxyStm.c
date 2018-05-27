#include "proxyStm.h"

static const struct state_definition proxyStatesHandlers[] = {
    {
        .state            = OBTAIN_ORIGIN,
        /*.on_arrival       = hello_read_init,
        .on_departure     = hello_read_close,
        .on_read_ready    = hello_read,*/
    },{
        .state            = SOLVE_DOMAIN,
        //.on_block_ready   = request_resolv_done,
    },{
        .state            = CONNECT,
        //.on_arrival       = request_connecting_init,
        //.on_write_ready   = request_connecting,
    }, {
        .state            = DONE,
    },{
        .state            = ERROR,
    }
};

struct state_definition * getProxyStates() {
	return proxyStatesHandlers;
}