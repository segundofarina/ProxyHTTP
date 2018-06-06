#include "proxyStm.h"

#include "proxyStates/requestState.h"
#include "proxyStates/responseState.h"
#include "proxyStates/errorState.h"

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
        .on_write_ready = responseWrite,
        .on_arrival = responseArrival,
        .on_departure = responseDeparture
    },{
        .state = DONE
    },{
        .state = ERROR,
        .on_write_ready = errorWrite
    },{
        .state = FATAL_ERROR
    }
};

struct state_definition * getProxyStates() {
	return (struct state_definition *) proxyStatesHandlers;
}