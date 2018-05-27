#include <stdlib.h>
#include <ctype.h>

typedef enum {ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT} hostData;

//	As per RFC:7230
//	request-line = method SP request-target SP HTTP-version CRLF
typedef enum {METHOD, SP1, REQ_TAR, SP2, HTTPV, CRLF} reqLineState;

typedef enum {START, HTTP, SLASH, HOST, DOUBLE_DOT, PORT} validHostState;

/**
 *	|	Valid hosts:				|			Host		|  port
 *	-----------------------------------------------------------------
 *	|	/ 							|			EMPTY		|	80
 *	|	/background.png				|			EMPTY		|	80
 *	|	/test.html?query=alibaba	|			EMPTY		|	80
 *	|	/anypage.html				|			EMPTY		|	80
 *	|	http://google.com/docs		|		google.com		|	80
 *	|	google.com:1080				|		google.com		|  1080
 *	|	127.0.0.1					|		127.0.0.1		|	80
 *	|	127.0.0.1:1080				|		127.0.0.1		|  1080
 *	|	http://[AAAA]:8080			|		AAAA host 		|  8080
 *	=================================================================
 *	|	Invalid hosts: ERROR		|			Host		|  port
 *	-----------------------------------------------------------------
 *	|	:1080						|			EMPTY		|  1080
*/
hostData processHost(char * fqdn, char * result, uint16_t resultLen, uint16_t * port) {
	int i = 0, j = 0;
	hostData ans = EMPTY;
	*port = (uint16_t) 80;
	int port = 0;
	validHostState state = HTTP;
	while(fqdn[i] != 0 && j < resultLen) {
		switch(state) {
			case START:
				switch(tolower(fqdn[i])) {
					case 'h':
						state = HTTP;
						result[j++] = fqdn[i];
						break;
					case '/':
						return ans;
						break;
					default:
						if(isdigit(fqdn[i])) {
							state = HOST;
						}
						result[j++] = fqdn[i];
				}
				break;
			default:
				return ERROR;
		}
		i++;
	}
	return ;
}

/**
 *	The return value is as follows:
 *	ERROR: Code 400 (bad request)
 *	EMPTY: host not found, only request-target present
 *	DOMAIN_HOST: host found as domain name, must be resolved
 *	DOMAIN_HOST_PORT: host found as domain name with port
 *	IPV4: host found as IPv4
 *	IPV4_PORT: host found as IPv4 with port
 *	IPV6: host found as IPv6
 *	IPV6_PORT: host found as IPv6 with port
 */
hostData requestTarget_marshall(char * buffer, char * result, uint16_t resultLen, uint16_t * port) {
	hostData ret = EMPTY;
	reqLineState state = METHOD;
	char fqdn[0xFF] = {0};
	char c = 0;
	int i = 0, j = 0;

	while((c = (char)buffer[i]) != 0) {
		switch(state) {
			case METHOD:
				if(c == ' ') {
					state = SP1;
				}
				break;
			case SP1:
				if(c == ' ') {
					return ERROR;
				} else {
					fqdn[j] = c;
					j++;
					state = REQ_TAR;
				}
			case REQ_TAR:
				if(c == ' ') {
					state = SP2;
					ret = processHost(fqdn, result, resultLen, port);
				} else {
					fqdn[j] = c;
					j++;
				}
			case HTTPV:
				if(c == '\r') {
					state = CRLF;
				}
			case CRLF:
				if(c != '\n') {
					return ERROR;
				}
			default:
				return ERROR;
		}
	}

	return ret;
}