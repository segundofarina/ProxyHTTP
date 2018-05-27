#include "request.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
// #include <stdio.h>
// #include "parser_utils.h"

// typedef enum {CRLF = 0, NOT_INTERESTED, POSIBLE_HOST, HOST, HOST_DONE, HOST_IGNORE, CRLF2} headerState;

// struct headerParser {
// 	headerState hSt;	// inicializado en CRLF = 0
// 	int i;				// inicializado en 0
// 	int j;				// inicializado en 0
// 	int foundHost;		// inicializado en 0
// };



//////////////////////////// Para agregar a request.h ///////////////////////////////////////////////////////////////
////////// Nota: Adaptar lo que devuelve requestHeaders a la estructura existente ///////////////////////////////////

typedef enum {CRLF = 0, NOT_INTERESTED, POSIBLE_HOST, HOST, HOST_DONE, HOST_IGNORE, CRLF2} HeaderState;

typedef struct {
    HeaderState hSt;    // inicializado en CRLF = 0
    int i;              // inicializado en 0
    int j;              // inicializado en 0
    int foundHost;      // inicializado en 0
} HeaderParser;

HeaderParser * headerParser_init(void);

void headerParser_destroy(HeaderParser *p);

Request_state requestHeaders(struct request_parser *p, const uint8_t c, HeaderParser *hP);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


HeaderParser * headerParser_init(void) {
	HeaderParser *ret = malloc(sizeof(*ret));
	if(ret != NULL) {
		memset(ret, 0, sizeof(*ret));
	}
	return ret;
}

void headerParser_destroy(HeaderParser *p) {
    if(p != NULL) {
        free(p);
    }
}

Request_state requestHeaders(struct request_parser *p, const uint8_t c, HeaderParser *hP) {
	//// const struct parser_definition dHost = parser_utils_strcmpi("HOST");
	//// struct parser *parserHost = parser_init(parser_no_classes(), &dHost);
	// int i = hP->i, j = hP->j, foundHost = hP->foundHost;
	Request_state st = p->state;
	HeaderState makingHost = hP->hSt;
	char host[] = {'h', 'o', 's', 't'};
	int hostLength = 4;
    // while(buffer_can_read(b) && (st != done || st != error)) {
        // const uint8_t c = buffer_read(b);
        // st = request_feed(p, c);
        switch(makingHost) {
        	// printf("%d\n", c);
        	case CRLF:
        		switch((char)c) {
        			case 'h':
        			case 'H':
        				// const struct parser_event * e = parser_feed(parserHost, c);
        				if(hP->i < hostLength && !(hP->foundHost)) {
        					// printf("Let's see...\n");
        					if(host[hP->i] == tolower((char)c)) {
        						makingHost = POSIBLE_HOST;
        						(hP->i)++;
        					}
        				} else {
        					makingHost = NOT_INTERESTED;
        					// printf("Nope.\n");
        				}
        				break;
        			case '\r':
        				makingHost = CRLF2;
        				break;
        			case '\n':
        				hP->i = 0;
        				// printf("New Line.\n");
        				break;
        			case ' ':
        				makingHost = NOT_INTERESTED;
        				// printf("Nope.\n");
        				break;
        			default:
        				makingHost = NOT_INTERESTED;
        				// printf("Nope.\n");
        		}
        		break;
        	case NOT_INTERESTED:
        		if((char)c == '\r') {
        			makingHost = CRLF;
        		}
        		break;
        	case CRLF2:
        		if('\n') {
        			st = done;
        			// printf("Done.\n");
        		}
        	case POSIBLE_HOST:
        		if(hP->i < hostLength) {
        			if(host[hP->i] == tolower((char)c)) {
        				// printf("%d\n", hP->i);
        				(hP->i)++;
        				// printf("%d\n", hP->i);
        				if((hP->i) == hostLength) {
        					makingHost = HOST;
        					// printf("Found!\n");
        				}
        			} else {
        				makingHost = NOT_INTERESTED;
        			}
        		} else {
        			makingHost = NOT_INTERESTED;
        		}
        		break;
        	case HOST:
        		if((char)c == ':') {
        			hP->foundHost = 1;
        			if(p->origin_dest->fqdn[0] != 0) {
        				makingHost = HOST_IGNORE;
        			} else {
        				makingHost = HOST_DONE;
        				// printf("Begin writing...\n");
        			}
        		} else {
        			makingHost = NOT_INTERESTED;
        		}
        		break;
        	case HOST_IGNORE:
        		// Do nothing.
        		// printf("Do nothing.\n");
        		if((char)c == '\r') {
        			makingHost = CRLF;
        		}
        		break;
        	case HOST_DONE:
        		if((char)c == '\r') {
        			makingHost = CRLF;
        		} else if((char)c == ' ') {
        			// Do nothing.
        		} else {
        			// printf("Write char: %c\n", (char)c);
        			p->origin_dest->fqdn[hP->j] = (char)c;
        			(hP->j)++;
        		}
        		break;
        	default:
        		st = error;
        }
    // }
    hP->hSt = makingHost;
    p->state = st;
    //// parser_utils_strcmpi_destroy(dHost);
    return st;
}
