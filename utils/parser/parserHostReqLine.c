//
//  Created by Nicolas Paganini on 5/27/18
//

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
	int i = 0, j = 0, k = 0, ipv6Found = 0, httpFound = 0, l = 0;
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
				if(k<4 && fqdn[i] == http[k]) {
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
//				    printf("DOUBLE_DOT ERROR\n");
					return ERROR;
				}
				break;
			case SLASH:
				if(fqdn[i] == '/' && !httpFound) {
					httpFound = 1;
					state = HOST;
					for (l = 0; l <= j; l++) {
						result[l] = 0;
					}
					j = -1;
				} else if(fqdn[i] == '/' && httpFound) {
					return ERROR;
                } else {
					return ans;
				}
				break;
			case HOST:
				if(fqdn[i] == '/') {
					if(i > 0) {
						if(fqdn[i-1] == '/' && httpFound) {
							ans = ERROR;
						}
					}
					if((ans == IPV6 || ans == IPV6_PORT) && !ipv6Found) {
						ans = ERROR;
					}
					return ans;
				}
				if(fqdn[i] != ':') {
					switch(ans) {
						case DOMAIN_HOST:
						    if(fqdn[i] == '[') {
						        ans = IPV6;
						        j = -1;
						    }
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
//								j = -1;
							}
							break;
						default:
//						    printf("HOST ERROR");
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
//						    printf("IPV6 ERROR\n");
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
		if(j >= 0 && j < resultLen && !ipv6Found) {
			result[j] = fqdn[i];
		}
		i++;
		j++;
	}

	// Now the port
    if(state == PORT && result[0] == 0) {
        return ERROR;
	}
	while(fqdn[i] != 0 && state == PORT) {
		if(isdigit(fqdn[i])) {
			portAux = portAux*10 + fqdn[i] - '0';
		} else if(fqdn[i] == '/') {
			state = SLASH;
		}
		i++;
	}
	*port = (uint16_t) portAux;

	if((ans == IPV6 || ans == IPV6_PORT) && !ipv6Found) {
		return ERROR;
	}

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
	int i;
    hostData ret;
	*port = 80;

	for(i = 0; i < resultLen; i++) {
	    result[i] = 0;
	}

	ret = processHost(buffer, result, resultLen, port);

//	if(result[0] == 0) {
//	    ret = EMPTY;
//	}
	if(*port == 0) {
		*port = 80;
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

int fillRequestData_marshall(hostData addressType, char * host, uint16_t * port, struct requestData * rdStruct) {
	int i;
	switch(addressType) {
		case ERROR:
			return 0;
			break;
		case EMPTY:
			if(*port != (uint16_t)DEFAULT_HTTP_PORT) {
				rdStruct->destPort = (uint16_t)(*port);
			}
			rdStruct->destAddrType = DOMAIN;
			break;
		case DOMAIN_HOST:
			rdStruct->destAddrType = DOMAIN;
			for(i = 0; host[i] != 0; i++) {
				rdStruct->destAddr->fqdn[i] = host[i];
			}
			rdStruct->destPort = (uint16_t)DEFAULT_HTTP_PORT;
			break;
		case DOMAIN_HOST_PORT:
			rdStruct->destAddrType = DOMAIN;
			for(i = 0; host[i] != 0; i++) {
				rdStruct->destAddr->fqdn[i] = host[i];
			}
			rdStruct->destPort = (uint16_t)(*port);
			break;
		case IPV4:
			rdStruct->destAddrType = IPv4;
			// ip passage to sockaddr_in
			rdStruct->destPort = (uint16_t)DEFAULT_HTTP_PORT;
			break;
		case IPV4_PORT:
			rdStruct->destAddrType = IPv4;
			//
			rdStruct->destPort = (uint16_t)(*port);
			break;
		case IPV6:
			rdStruct->destAddrType = IPv6;
			//
			rdStruct->destPort = (uint16_t)DEFAULT_HTTP_PORT;
			break;
		case IPV6_PORT:
			rdStruct->destAddrType = IPv6;
			//
			rdStruct->destPort = (uint16_t)(*port);
			break;
	}
	return 1;
}