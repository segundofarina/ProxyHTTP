#include <stdint.h>
#include <sys/socket.h>

#include "obtainOriginState.h"

#include "../connection-structure.h"
#include "../proxyStm.h"
#include "../../utils/buffer/buffer.h"

unsigned obtainOriginRead(struct selector_key * key) {
	struct Connection * conn = DATA_TO_CONN(key);
	unsigned ret = OBTAIN_ORIGIN;
	uint8_t *ptr;
	size_t count;
	ssize_t  n;

	ptr = buffer_write_ptr(&conn->readBuffer, &count);
	n = recv(key->fd, ptr, count, 0);
	if(n > 0) {
		buffer_write_adv(&conn->readBuffer, n);

		if(selector_set_interest_key(key, OP_WRITE) == SELECTOR_SUCCESS) {
			return ANSWER;
		}
	}
	return ERROR;
}

unsigned writeEcho(struct selector_key * key) {
	struct Connection * conn = DATA_TO_CONN(key);
	unsigned ret = ANSWER;
	uint8_t *ptr;
	size_t count;
	ssize_t  n;

	ptr = buffer_read_ptr(&conn->readBuffer, &count);
	n = send(key->fd, ptr, count, 0);
	if(n > 0) {
		buffer_read_adv(&conn->readBuffer, n);

		if(selector_set_interest_key(key, OP_READ) == SELECTOR_SUCCESS) {
			return OBTAIN_ORIGIN;
		}
	}
	return ERROR;
}



/*

static unsigned
hello_read(struct selector_key *key) {
    struct hello_st *d = &ATTACHMENT(key)->client.hello;
    unsigned  ret      = HELLO_READ;
        bool  error    = false;
     uint8_t *ptr;
      size_t  count;
     ssize_t  n;

    ptr = buffer_write_ptr(d->rb, &count);
    n = recv(key->fd, ptr, count, 0);
    if(n > 0) {
        buffer_write_adv(d->rb, n);
        const enum hello_state st = hello_consume(d->rb, &d->parser, &error);
        if(hello_is_done(st, 0)) {
            if(SELECTOR_SUCCESS == selector_set_interest_key(key, OP_WRITE)) {
                ret = hello_process(d);
            } else {
                ret = ERROR;
            }
        }
    } else {
        ret = ERROR;
    }

    return error ? ERROR : ret;
}


*/