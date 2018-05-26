#ifndef PC_2018_04_HTTPREQUEST_H
#define PC_2018_04_HTTPREQUEST_H

#include <stdint.h>
#include <stdbool.h>

#include <sys/socket.h>

#include "../buffer/buffer.h"

enum request_state {
    request_line,
    headers,
    body,
    done,
    error
};

struct request_parser {
    struct socks_addr * origin_dest;
    enum request_state state;
};

struct socks_addr {
    char fqdn[0xff];
    struct sockaddr_in  ipv4;
    struct sockaddr_in6 ipv6;
};

void request_init(struct request_parser *p);

enum request_state request_feed (struct request_parser *p, const uint8_t c);

enum request_state request_consume(struct request_parser *p, buffer * b);

enum request_state requestLine(struct request_parser *p, const uint8_t c);

enum request_state requestHeaders(struct request_parser *p, const uint8_t c);

enum request_state requestBody(struct request_parser *p, const uint8_t c);

enum request_state requestDone(struct request_parser *p, const uint8_t c);

#endif //PC_2018_04_HTTPREQUEST_H


