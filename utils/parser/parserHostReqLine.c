#include <stdio.h>
#include "parserHostReqLine.h"

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
	int i = 0, j = 0, k = 0, ipv6Found = 0, l = 0;
	hostData ans = EMPTY;
	*port = (uint16_t) 80;
	int portAux = 0;
	char http[] = "http";
	validHostState state = START;
	while(fqdn[i] != 0 && j < resultLen && state != PORT) {
		switch(state) {
			case START:
				switch(tolower(fqdn[i])) {
					case 'h':
						ans = DOMAIN_HOST;
						state = HTTP;
						k++;
						break;
					case '/':
						return ans;
					case '[':
						ans = IPV6;
						state = HOST;
						j = -1;
						break;
					default:
						state = HOST;
						if(isdigit(fqdn[i])) {
							ans = IPV4;
						} else {
							ans = DOMAIN_HOST;
						}
				}
				break;
			case HTTP:
				if(k == 4) {
					if(fqdn[i] == ':') {
						state = DOUBLE_DOT;
					} else {
						state = HOST;
					}
				}
				if(fqdn[i] == http[k]) {
					k++;
				}
				break;
			case DOUBLE_DOT:
				if(fqdn[i] == '/') {
					state = SLASH;
				} else if(isdigit(fqdn[i])) {
					state = PORT;
					portAux = portAux*10 + fqdn[i] - '0';
					j = -1;
				} else {
				    printf("DOUBLE_DOT ERROR\n");
					return ERROR;
				}
				break;
			case SLASH:
				if(fqdn[i] == '/') {
					j = -1;
					state = HOST;
					for(l = 0; l < resultLen; l++) {
					    result[l] = 0;
					}
				} else {
					return ans;
				}
				break;
			case HOST:
				if(fqdn[i] == '/') {
					return ans;
				}
				if(fqdn[i] != ':') {
					switch(ans) {
						case DOMAIN_HOST:
							break;
						case IPV4:
							if(!isdigit(fqdn[i])) {
								ans = DOMAIN_HOST;
							}
							break;
						case IPV6:
							if(!isdigit(fqdn[i]) && fqdn[i] != ']') {
								ans = DOMAIN_HOST;
							} else if(fqdn[i] == ']') {
								ipv6Found = 1;
								j = -1;
							}
							break;
						default:
						    printf("HOST ERROR");
							return ERROR;
					}
				} else {
					if(ans != IPV6) {
						state = DOUBLE_DOT;
						if(ans == IPV4) {
							ans = IPV4_PORT;
						} else if(ans == DOMAIN_HOST) {
							ans = DOMAIN_HOST_PORT;
							j = -1;
						} else {
						    printf("IPV6 ERROR\n");
							ans = ERROR;
						}
					} else {
						if(ipv6Found) {
							state = DOUBLE_DOT;
							ans = IPV6_PORT;
						}
					}
				}
				break;
		}
		if(j >= 0 && j < resultLen) {
			result[j] = fqdn[i];
		}
		i++;
		j++;
	}

	// Now the port
	while(fqdn[i] != 0 && state == PORT) {
		if(isdigit(fqdn[i])) {
			portAux = portAux*10 + fqdn[i] - '0';
		} else if(fqdn[i] == '/') {
			state = SLASH;
		}
		i++;
	}
	*port = portAux;

	if(j >= resultLen) {
		return ERROR;
	} else {
		return ans;
	}
}

/**
 * @param buffer Must begin with METHOD and end in CRLF (inclusive).
 *
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

	ret = processHost(buffer, result, resultLen, port);

	if(result[0] == 0) {
	    ret = EMPTY;
	}

    return ret;

//    reqLineState state = METHOD;
//    char c = 0;
//    int i = 0, j = 0;
//
//	while((c = buffer[i]) != 0) {
//		switch(state) {
//			case METHOD:
//				if(c == ' ') {
//					state = SP1;
//				}
//				break;
//			case SP1:
//				if(c == ' ') {
//					return ERROR;
//				} else {
//					fqdn[j] = c;
//					j++;
//					state = REQ_TAR;
//				}
//				break;
//			case REQ_TAR:
//				if(c == ' ') {
//					state = SP2;
//					ret = processHost(fqdn, result, resultLen, port);
//				} else {
//					fqdn[j] = c;
//					j++;
//				}
//				break;
//            case SP2:
//                if(c == ' ') {
//                    return ERROR;
//                } else {
//                    fqdn[j] = c;
//                    j++;
//                    state = HTTPV;
//                }
//                break;
//			case HTTPV:
//				if(c == '\r') {
//					state = CRLF;
//				}
//				break;
//			case CRLF:
//				if(c != '\n')
//					return ERROR;
//				break;
//		}
//		i++;
//	}
}