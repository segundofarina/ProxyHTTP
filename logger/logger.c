#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "logger.h"

#include "../utils/buffer/buffer.h"

#define LOGGER_FD 1

struct Logger {
    enum logger_level level;
    int loggerFd;

    uint8_t rawBuff_a[2048];
    buffer buff;

    fd_selector selector;
};

void loggerExec(struct selector_key * key);

static const struct fd_handler loggerHandlers = {
	.handle_read = NULL,
    .handle_write = loggerExec,
    .handle_close = NULL,
    .handle_block = NULL
};

static struct Logger logger;

void loggerInit(const enum logger_level level, fd_selector selector) {
    logger.level = level;
    logger.loggerFd = LOGGER_FD;
    buffer_init(&logger.buff, N(logger.rawBuff_a), logger.rawBuff_a);

    logger.selector = selector;
    selector_register(logger.selector, logger.loggerFd, &loggerHandlers, OP_NOOP, 0);
}

void loggerWrite(const enum logger_level level, const char * msg) {
    uint8_t * ptr;
    size_t count;
    int n;
    
    if(level > logger.level) {
        return;
    }

    if(logger.loggerFd == -1) {
        return;
    }

    ptr = buffer_write_ptr(&logger.buff, &count);
    n = strlen( strncpy((char *)ptr, msg, count) );
    buffer_write_adv(&logger.buff, n);

    if(buffer_can_read(&logger.buff)) {
        /* Set write interest */
        selector_set_interest(logger.selector, logger.loggerFd, OP_WRITE);
    }
}

int isLoggerFd(const int fd) {
    return logger.loggerFd == fd;
}

void loggerExec(struct selector_key * key) {
    uint8_t * ptr;
    size_t count;
    int n;

    if(logger.loggerFd == -1) {
        return;
    }
    
    ptr = buffer_read_ptr(&logger.buff, &count);
    n = write(key->fd, ptr, count);
    if(n <= 0) {
        // error
        selector_unregister_fd(key->s, logger.loggerFd);
        logger.loggerFd = -1;

        return;
    }
    buffer_read_adv(&logger.buff, n);

    fd_interest interest = OP_NOOP;
    if(buffer_can_read(&logger.buff)) {
        interest = OP_WRITE;
    }

    selector_set_interest(key->s, logger.loggerFd, interest);
}
