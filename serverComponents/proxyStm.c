#include "proxyStm.h"

#include "proxyStates/requestState.h"
#include "proxyStates/responseState.h"

const struct state_definition proxyStatesHandlers[] = {
    {
        .state = REQUEST,
		.on_read_ready = requestRead, 
		.on_write_ready = requestWrite,
		.on_block_ready = requestBlockReady,
        .on_arrival = requestArrival,
        .on_departure = requestDeparture
    },{
        .state = RESPONSE,
        .on_read_ready = responseRead,
        .on_write_ready = responseWrite
    },{
        .state = DONE,
    },{
        .state = ERROR,
    }
};

struct state_definition * getProxyStates() {
	return (struct state_definition *) proxyStatesHandlers;
}