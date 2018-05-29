//
//  Created by Nicolas Paganini on 5/27/18
//

#ifndef PC_2018_04_PARSERHOSTREQLINE_H
#define PC_2018_04_PARSERHOSTREQLINE_H

#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <netinet/in.h>
#include <inttypes.h>

#define DEFAULT_HTTP_PORT (80)

typedef enum {ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT} hostData;

//	As per RFC:7230
//	request-line = method SP request-target SP HTTP-version CRLF
typedef enum {METHOD, SP1, REQ_TAR, SP2, HTTPV, CRLF} reqLineState;

typedef enum {START, HTTP, SLASH, HOST, DOUBLE_DOT, PORT} validHostState;

/* Parte del request parser */
enum addrType {
    IPv4, IPv6, DOMAIN
};

union socks_addr {
    char fqdn[0xff];
    struct sockaddr_in  ipv4;
    struct sockaddr_in6 ipv6;
};

struct requestData {
    enum addrType destAddrType;
    union socks_addr destAddr;
    /** port in network byte order */
    in_port_t destPort;
};
/* end */


/**
 *	|	Valid hosts:				|			Host		|  port
 *	-----------------------------------------------------------------
 *	|	/ 							|			EMPTY		|	80
 *	|	/background.png				|			EMPTY		|	80
 *	|	/test.html?query=alibaba	|			EMPTY		|	80
 *	|	/anypage.html				|		    EMPTY		|	80
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
hostData processHost(char * fqdn, char * result, uint16_t resultLen, uint16_t * port);

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
hostData requestTarget_marshall(char * buffer, char * result, uint16_t resultLen, uint16_t * port);

/**
 * Fills the Request Data struct with the available data. If there's no specified
 * port, the default port for HTTP is port 80.
 * Returns TRUE or FALSE depending on whether the request had ERRORs or not.
 * Note: a request-line with a request-target that does not contain the host
 * qualifies as EMPTY and is a successful request.
**/
int fillRequestData_marshall(hostData addressType, char * host, uint16_t * port, struct requestData * rdStruct);

#endif