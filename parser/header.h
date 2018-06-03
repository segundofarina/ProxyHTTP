//
// Created by Segundo Fariña on 28/5/18.
//

#ifndef PC_2018_04_HEADER_H
#define PC_2018_04_HEADER_H


#include <stdint.h>


//https://tools.ietf.org/html/rfc2616#section-4.2

/*
 * message-header    = field-name ":" [ field-value ]
 *      field-name   = token
 *      field-value  = *( field-content | LWS )
 *     field-content = <the OCTETs making up the field-value
 *                      and consisting of either *TEXT or combinations
 *                      of token, separators, and quoted-string>
 */

// esto hay que sacarlo y ponerlo en request.c
//enum header_name{
//    HEADER_HOST,
//    HEADER_CONT_LEN,
//    HEADER_NOT_INTERESTED
//};

enum header_state{
    header_name,
    header_value,
    header_crlf,
    header_end,
    header_error,
};

struct header_parser{
    struct multi_parser * nameParser;

    enum header_state state;

    uint32_t name;
    char * value;
    int len;
    int value_max_len;
};




extern void
header_parser_init (struct header_parser* p,const int notMatch, char ** strings, const int * matches,const int len);


extern void
header_parser_close(struct header_parser* p);

enum header_state
header_parser_feed(const uint8_t c,struct header_parser* p);

char *
header_state_string(enum header_state st);

void
header_parser_reset    (struct header_parser *p);

#endif //PC_2018_04_HEADER_H
