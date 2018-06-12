//
// Created by Segundo Fariña on 31/5/18.
//

#ifndef PC_2018_04_STATUSLINE_H
#define PC_2018_04_STATUSLINE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

enum statusLine_state {
    sl_version,
    sl_code,
    sl_extra,
    sl_clrf,
    sl_end,
    sl_error
};


struct statusLine_parser {
    enum statusLine_state state;
    int code;
};

/** inicializa el parser */
void
statusLine_parser_init (struct statusLine_parser *p);

/** entrega un byte al parser. retorna true si se llego al final  */
enum statusLine_state
statusLine_parser_feed ( const uint8_t c,struct statusLine_parser *p);


void
statusLine_parser_close(struct statusLine_parser *p);

char *
statusLine_state_string(const enum statusLine_state st);


#endif //PC_2018_04_STATUSLINE_H
