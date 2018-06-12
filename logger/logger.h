#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "../utils/selector/selector.h"

enum logger_level {
    PRODUCTION = 0,
    DEBUG = 1
};

/* Init the logger library before using it, specify the logging level */
void loggerInit(const enum logger_level level, fd_selector selector);


/* Log msg */
void loggerWrite(const enum logger_level level, const char * msg);



int isLoggerFd(const int fd);

void loggerExec();

#endif
