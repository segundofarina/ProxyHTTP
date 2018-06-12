//
//  Created by Nicolas Paganini on 5/27/18
//

// #include <stdio.h>
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
	int i = 0, j = 0, k = 0, ipv6Found = 0, httpFound = 0, l = 0, ipv4DotCounter = 0;
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
//				    printf("DOUBLE_DOT ERROR_hostData\n");
					return ERROR_hostData;
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
					return ERROR_hostData;
                } else {
					return ans;
				}
				break;
			case HOST:
				if(fqdn[i] == '/') {
					if(i > 0) {
						if(fqdn[i-1] == '/' && httpFound) {
							ans = ERROR_hostData;
						}
					}
					if((ans == IPV6 || ans == IPV6_PORT) && !ipv6Found) {
						ans = ERROR_hostData;
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
							if(!isdigit(fqdn[i]) && fqdn[i] != '.') {
								ans = DOMAIN_HOST;
							} else if(fqdn[i] == '.') {
								ipv4DotCounter++;
								if(ipv4DotCounter == 4) {
									return ERROR_hostData;
								}
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
//						    printf("HOST ERROR_hostData");
							return ERROR_hostData;
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
//						    printf("IPV6 ERROR_hostData\n");
							ans = ERROR_hostData;
						}
					} else {
						if(ipv6Found) {
							state = DOUBLE_DOT;
							ans = IPV6_PORT;
						}
					}
				}
				break;
			default:
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
        return ERROR_hostData;
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
		return ERROR_hostData;
	}

	return j >= resultLen? ERROR_hostData : ans;
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

	i = 0;
	while(i < resultLen && buffer[i] == ' ') {
		i++;
	}

	ret = processHost(buffer+i, result, resultLen, port);

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
//					return ERROR_hostData;
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
//                    return ERROR_hostData;
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
//					return ERROR_hostData;
//				break;
//		}
//		i++;
//	}
}

/*
 *	Fills the requestData struct for the request.
 *	Uses function inet_pton to turn the char* host into sockaddr_in[6] as required.
 */
int fillRequestData_marshall(hostData addressType, char * host, uint16_t port, struct requestData * rdStruct) {
	int i, errCode = -1;

	switch(addressType) {
		case ERROR_hostData:
			return errCode;
			break;
		case EMPTY:
			if(port != (uint16_t)DEFAULT_HTTP_PORT) {
				rdStruct->destPort = port;
			}
			rdStruct->destAddrType = DOMAIN;
			errCode = 1;
			break;
		case DOMAIN_HOST:
			rdStruct->destAddrType = DOMAIN;
			for(i = 0; host[i] != 0; i++) {
				rdStruct->destAddr.fqdn[i] = host[i];
			}
			rdStruct->destPort = (uint16_t)DEFAULT_HTTP_PORT;
			errCode = 1;
			break;
		case DOMAIN_HOST_PORT:
			rdStruct->destAddrType = DOMAIN;
			for(i = 0; host[i] != 0; i++) {
				rdStruct->destAddr.fqdn[i] = host[i];
			}
			rdStruct->destPort = port;
			errCode = 1;
			break;
		case IPV4:
			rdStruct->destAddrType = IPv4;
			rdStruct->destPort = (uint16_t)DEFAULT_HTTP_PORT;
			break;
		case IPV4_PORT:
			rdStruct->destAddrType = IPv4;
			rdStruct->destPort = port;
			break;
		case IPV6:
			rdStruct->destAddrType = IPv6;
			rdStruct->destPort = (uint16_t)DEFAULT_HTTP_PORT;
			break;
		case IPV6_PORT:
			rdStruct->destAddrType = IPv6;
			rdStruct->destPort = port;
			break;
	}

	if(addressType == IPV4 || addressType == IPV4_PORT) {
		rdStruct->destAddr.ipv4.sin_family = AF_INET;
		rdStruct->destAddr.ipv4.sin_port = htons(port);
		errCode = inet_pton(AF_INET, host, &(rdStruct->destAddr.ipv4.sin_addr.s_addr));
	}
	if(addressType == IPV6 || addressType == IPV6_PORT) {
		rdStruct->destAddr.ipv6.sin6_family = AF_INET6;
		rdStruct->destAddr.ipv6.sin6_port = htons(port);
		rdStruct->destAddr.ipv6.sin6_flowinfo = (uint32_t) 0;	// IPv6 flow information
		errCode = inet_pton(AF_INET, host, &(rdStruct->destAddr.ipv6.sin6_addr.s6_addr));
		rdStruct->destAddr.ipv6.sin6_scope_id = (uint32_t) 0;
	}

	rdStruct->destPort = htons(rdStruct->destPort);
	return errCode;
}
