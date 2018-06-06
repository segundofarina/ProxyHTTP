
#ifndef PC_2018_04_METHOD_H
#define PC_2018_04_METHOD_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "method.h"

enum requestLine_state {
    rl_method,
    rl_target,
    rl_extra,
    rl_clrf,
    rl_end,
    rl_error
};


struct requestLine_parser {
    enum requestLine_state state;
    struct method_parser * methodParser;

    char uri [0xFF];

    enum request_method method;

    uint16_t len;


};

/** inicializa el parser */
void
requestLine_parser_init (struct requestLine_parser *p);

/** entrega un byte al parser. retorna true si se llego al final  */
enum requestLine_state
requestLine_parser_feed ( const uint8_t c,struct requestLine_parser *p);

/**
 * por cada elemento del buffer llama a `request_parser_feed' hasta que
 * el parseo se encuentra completo o se requieren mas bytes.
 *
 * @param errored parametro de salida. si es diferente de NULL se deja dicho
 *   si el parsing se debió a una condición de error
 */
enum requestLine_state
requestLine_parser_consume(char *buffer,size_t len, struct requestLine_parser *p, bool *errored);

/**
 * Permite distinguir a quien usa socks_hello_parser_feed si debe seguir
 * enviando caracters o no.
 *
 * En caso de haber terminado permite tambien saber si se debe a un error
 */
bool
requestLine_is_done(const enum requestLine_state st, bool *errored);

void
requestLine_parser_close(struct requestLine_parser *p);

char *
requestLine_state_toString(const enum requestLine_state st);


#endif //PC_2018_04_METHOD_H
