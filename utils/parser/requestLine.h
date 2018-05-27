
#ifndef PC_2018_04_METHOD_H
#define PC_2018_04_METHOD_H

#include <stdint.h>

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
    struct method_parser * methodparser;

    char fqdn [0xFF];

    u_int16_t index;


};

/** inicializa el parser */
void
requestLine_parser_init (struct requestLine_parser *p);

/** entrega un byte al parser. retorna true si se llego al final  */
enum requestLine_state
requestLine_parser_feed (struct requestLine_parser *p, const uint8_t c);

/**
 * por cada elemento del buffer llama a `request_parser_feed' hasta que
 * el parseo se encuentra completo o se requieren mas bytes.
 *
 * @param errored parametro de salida. si es diferente de NULL se deja dicho
 *   si el parsing se debió a una condición de error
 */
enum requestLine_state
requestLine_parser_consume(buffer *b, struct requestLine_parser *p, bool *errored);

/**
 * Permite distinguir a quien usa socks_hello_parser_feed si debe seguir
 * enviando caracters o no.
 *
 * En caso de haber terminado permite tambien saber si se debe a un error
 */
bool
requestLine_is_done(const enum requestLine_parser st, bool *errored);

void
requestLine_close(struct request_parser *p);


#endif //PC_2018_04_METHOD_H
