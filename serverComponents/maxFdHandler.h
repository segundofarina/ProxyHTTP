#ifndef __MAX_FD_HANDLER_H_
#define __MAX_FD_HANDLER_H_

#include <sys/socket.h>

void keepSpareFd();

void handleAcceptEmfile(int fd, struct sockaddr * clientAddr, socklen_t  * clientAddrLen);

#endif
