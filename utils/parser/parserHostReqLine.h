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
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define DEFAULT_HTTP_PORT (80)

typedef enum {ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT} hostData;

//	As per RFC:7230
//	request-line = method SP request-target SP HTTP-version CRLF
typedef enum {METHOD, SP1, REQ_TAR, SP2, HTTPV, CRLF} reqLineState;

typedef enum {START, HTTP, SLASH, HOST, DOUBLE_DOT, PORT} validHostState;

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
 * Note: a request-line with a request-target that does not contain the host
 * qualifies as EMPTY and is a successful request.
 *
 *
 * Returns zero on success or non-zero for the error code:
 * (as taken from http://man7.org/linux/man-pages/man3/getaddrinfo.3.html)
 *
 * EAI_ADDRFAMILY: The specified network host does not have any network addresses in the requested address family.
 * EAI_AGAIN: The name server returned a temporary failure indication.  Try again later.
 * EAI_BADFLAGS: hints.ai_flags contains invalid flags; or, hints.ai_flags included AI_CANONNAME and name was NULL.
 * EAI_FAIL: The name server returned a permanent failure indication.
 * EAI_FAMILY: The requested address family is not supported.
 * EAI_MEMORY: Out of memory.
 * EAI_NODATA: The specified network host exists, but does not have any network addresses defined.
 * EAI_NONAME: The node or service is not known; or both node and service are NULL; or AI_NUMERICSERV was specified
 *			in hints.ai_flags and service was not a numeric port-number string.
 * EAI_SERVICE: The requested service is not available for the requested socket type.  It may be available through
 *			another socket type. For example, this error could occur if service was "shell" (a service available
 *			only on stream sockets), and either hints.ai_protocol was IPPROTO_UDP, or hints.ai_socktype was SOCK_DGRAM;
 *			or the error could occur if service was not NULL, and hints.ai_socktype was SOCK_RAW (a socket type that
 *			does not support the concept of services).
 * EAI_SOCKTYPE: The requested socket type is not supported.  This could occur, for example, if hints.ai_socktype
 *			and hints.ai_protocol are inconsistent (e.g., SOCK_DGRAM and IPPROTO_TCP, respectively).
 * EAI_SYSTEM: Other system error, check errno for details.
**/
int fillRequestData_marshall(hostData addressType, char * host, uint16_t * port, struct requestData * rdStruct);

#endif