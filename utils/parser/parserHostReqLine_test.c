//
//  Created by Nicolas Paganini on 5/27/18
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parserHostReqLine.h"

int main(void) {

    int i, errCode;
	// char * test1 = "http://itba.edu.ar/foo?bar";
    char * test1 = "10.0.0.2";
	char * test2 = "itba.edu.ar/./hello///";
	char * test2a = "http://itba.edu.ar/./hello/../";
	char * test3 = "example.com:9090/hola";
	char * test4 = "http://itba.edu.ar/foo?barhttp://itba";
	char * test5 = "example.com/.../hola";
	char * test6 = "127.0.0.1:8080";
	char * test7 = "[0:0:0:0]:8080/pepito";
	char * test7a = "http://[0:0:0:0]:8080/pepito";
	char * test8 = "[:::1]:80";
	char * test9 = "http://www.google.com:8080/";
	char * test10 = "/";
	char * test11 = "http://:1080/";    // Should fail?
	//  Begin RFC examples' tests.
	char * test12 = "http://www.example.org/where?q=now";
	char * test13 = "http://www.example.org/pub/WWW/TheProject.html";
	char * test14 = "www.example.com:80";
	char * test15 = "http://www.example.org:8001";
    char * test16 = "goog";
    // Begin tests that should fail.
    char * test17 = "http:///pepito";
    char * test18 = "[:8080/";
    char * test19 = "[:::0]::8080/";
    char * test20 = "google.com::";
    char * test21 = "192.37.188.2";
    char * test22 = "192.37.188.2.2";
	int port = 80;
	hostData valid;
	char result[0xFF];

	printf("Hello!\n");
	for(i = 0; i < 0xFF; i++) {
	    result[i] = 0;
	}

	valid = requestTarget_marshall(test1, result, (uint16_t)0xFF, (uint16_t*)&port);
	printf("State 1: ");
    switch(valid) {
        //  ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT
        case ERROR:
            printf("Error in test 1.\n");
            break;
        case EMPTY:
            printf("EMPTY\n");
            break;
        case DOMAIN_HOST:
            printf("DOMAIN_HOST\n");
            break;
        case DOMAIN_HOST_PORT:
            printf("DOMAIN_HOST_PORT\n");
            break;
        case IPV4:
            printf("IPV4\n");
            break;
        case IPV4_PORT:
            printf("IPV4_PORT\n");
            break;
        case IPV6:
            printf("IPV6\n");
            break;
        case IPV6_PORT:
            printf("IPV6_PORT\n");
            break;
    }
    printf("Host: %s\tPort: %d\n\n", result, port);
    for(i = 0; i < 0xFF; i++) {
        result[i] = 0;
    }
    // errCode = fillRequestData_marshall(hostData addressType, char * host, uint16_t port, struct requestData * rdStruct);
    // if(errCode == 1) {
    //     printf("Host ");
    // }

	valid = requestTarget_marshall(test2, result, (uint16_t)0xFF, (uint16_t*)&port);
	printf("State 2: ");
    switch(valid) {
        //  ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT
        case ERROR:
            printf("Error in test 2.\n");
            break;
        case EMPTY:
            printf("EMPTY\n");
            break;
        case DOMAIN_HOST:
            printf("DOMAIN_HOST\n");
            break;
        case DOMAIN_HOST_PORT:
            printf("DOMAIN_HOST_PORT\n");
            break;
        case IPV4:
            printf("IPV4\n");
            break;
        case IPV4_PORT:
            printf("IPV4_PORT\n");
            break;
        case IPV6:
            printf("IPV6\n");
            break;
        case IPV6_PORT:
            printf("IPV6_PORT\n");
            break;
    }
    printf("Host: %s\tPort: %d\n\n", result, port);
    for(i = 0; i < 0xFF; i++) {
        result[i] = 0;
    }

	valid = requestTarget_marshall(test2a, result, (uint16_t)0xFF, (uint16_t*)&port);
    printf("State 2a: ");
    switch(valid) {
        //  ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT
        case ERROR:
            printf("Error in test 2a.\n");
            break;
        case EMPTY:
            printf("EMPTY\n");
            break;
        case DOMAIN_HOST:
            printf("DOMAIN_HOST\n");
            break;
        case DOMAIN_HOST_PORT:
            printf("DOMAIN_HOST_PORT\n");
            break;
        case IPV4:
            printf("IPV4\n");
            break;
        case IPV4_PORT:
            printf("IPV4_PORT\n");
            break;
        case IPV6:
            printf("IPV6\n");
            break;
        case IPV6_PORT:
            printf("IPV6_PORT\n");
            break;
    }
    printf("Host: %s\tPort: %d\n\n", result, port);
    for(i = 0; i < 0xFF; i++) {
        result[i] = 0;
    }

	valid = requestTarget_marshall(test3, result, (uint16_t)0xFF, (uint16_t*)&port);
    printf("State 3: ");
    switch(valid) {
        //  ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT
        case ERROR:
            printf("Error in test 3.\n");
            break;
        case EMPTY:
            printf("EMPTY\n");
            break;
        case DOMAIN_HOST:
            printf("DOMAIN_HOST\n");
            break;
        case DOMAIN_HOST_PORT:
            printf("DOMAIN_HOST_PORT\n");
            break;
        case IPV4:
            printf("IPV4\n");
            break;
        case IPV4_PORT:
            printf("IPV4_PORT\n");
            break;
        case IPV6:
            printf("IPV6\n");
            break;
        case IPV6_PORT:
            printf("IPV6_PORT\n");
            break;
    }
    printf("Host: %s\tPort: %d\n\n", result, port);
    for(i = 0; i < 0xFF; i++) {
        result[i] = 0;
    }

	valid = requestTarget_marshall(test4, result, (uint16_t)0xFF, (uint16_t*)&port);
    printf("State 4: ");
    switch(valid) {
        //  ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT
        case ERROR:
            printf("Error in test 4.\n");
            break;
        case EMPTY:
            printf("EMPTY\n");
            break;
        case DOMAIN_HOST:
            printf("DOMAIN_HOST\n");
            break;
        case DOMAIN_HOST_PORT:
            printf("DOMAIN_HOST_PORT\n");
            break;
        case IPV4:
            printf("IPV4\n");
            break;
        case IPV4_PORT:
            printf("IPV4_PORT\n");
            break;
        case IPV6:
            printf("IPV6\n");
            break;
        case IPV6_PORT:
            printf("IPV6_PORT\n");
            break;
    }
    printf("Host: %s\tPort: %d\n\n", result, port);
    for(i = 0; i < 0xFF; i++) {
        result[i] = 0;
    }

	valid = requestTarget_marshall(test5, result, (uint16_t)0xFF, (uint16_t*)&port);
    printf("State 5: ");
    switch(valid) {
        //  ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT
        case ERROR:
            printf("Error in test 5.\n");
            break;
        case EMPTY:
            printf("EMPTY\n");
            break;
        case DOMAIN_HOST:
            printf("DOMAIN_HOST\n");
            break;
        case DOMAIN_HOST_PORT:
            printf("DOMAIN_HOST_PORT\n");
            break;
        case IPV4:
            printf("IPV4\n");
            break;
        case IPV4_PORT:
            printf("IPV4_PORT\n");
            break;
        case IPV6:
            printf("IPV6\n");
            break;
        case IPV6_PORT:
            printf("IPV6_PORT\n");
            break;
    }
    printf("Host: %s\tPort: %d\n\n", result, port);
    for(i = 0; i < 0xFF; i++) {
        result[i] = 0;
    }

	valid = requestTarget_marshall(test6, result, (uint16_t)0xFF, (uint16_t*)&port);
    printf("State 6: ");
    switch(valid) {
        //  ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT
        case ERROR:
            printf("Error in test 6.\n");
            break;
        case EMPTY:
            printf("EMPTY\n");
            break;
        case DOMAIN_HOST:
            printf("DOMAIN_HOST\n");
            break;
        case DOMAIN_HOST_PORT:
            printf("DOMAIN_HOST_PORT\n");
            break;
        case IPV4:
            printf("IPV4\n");
            break;
        case IPV4_PORT:
            printf("IPV4_PORT\n");
            break;
        case IPV6:
            printf("IPV6\n");
            break;
        case IPV6_PORT:
            printf("IPV6_PORT\n");
            break;
    }
    printf("Host: %s\tPort: %d\n\n", result, port);
    for(i = 0; i < 0xFF; i++) {
        result[i] = 0;
    }

	valid = requestTarget_marshall(test7, result, (uint16_t)0xFF, (uint16_t*)&port);
    printf("State 7: ");
    switch(valid) {
        //  ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT
        case ERROR:
            printf("Error in test 7.\n");
            break;
        case EMPTY:
            printf("EMPTY\n");
            break;
        case DOMAIN_HOST:
            printf("DOMAIN_HOST\n");
            break;
        case DOMAIN_HOST_PORT:
            printf("DOMAIN_HOST_PORT\n");
            break;
        case IPV4:
            printf("IPV4\n");
            break;
        case IPV4_PORT:
            printf("IPV4_PORT\n");
            break;
        case IPV6:
            printf("IPV6\n");
            break;
        case IPV6_PORT:
            printf("IPV6_PORT\n");
            break;
    }
    printf("Host: %s\tPort: %d\n\n", result, port);
    for(i = 0; i < 0xFF; i++) {
        result[i] = 0;
    }

	valid = requestTarget_marshall(test7a, result, (uint16_t)0xFF, (uint16_t*)&port);
    printf("State 7a: ");
    switch(valid) {
        //  ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT
        case ERROR:
            printf("Error in test 7a.\n");
            break;
        case EMPTY:
            printf("EMPTY\n");
            break;
        case DOMAIN_HOST:
            printf("DOMAIN_HOST\n");
            break;
        case DOMAIN_HOST_PORT:
            printf("DOMAIN_HOST_PORT\n");
            break;
        case IPV4:
            printf("IPV4\n");
            break;
        case IPV4_PORT:
            printf("IPV4_PORT\n");
            break;
        case IPV6:
            printf("IPV6\n");
            break;
        case IPV6_PORT:
            printf("IPV6_PORT\n");
            break;
    }
    printf("Host: %s\tPort: %d\n\n", result, port);
    for(i = 0; i < 0xFF; i++) {
        result[i] = 0;
    }

	valid = requestTarget_marshall(test8, result, (uint16_t)0xFF, (uint16_t*)&port);
    printf("State 8: ");
    switch(valid) {
        //  ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT
        case ERROR:
            printf("Error in test 8.\n");
            break;
        case EMPTY:
            printf("EMPTY\n");
            break;
        case DOMAIN_HOST:
            printf("DOMAIN_HOST\n");
            break;
        case DOMAIN_HOST_PORT:
            printf("DOMAIN_HOST_PORT\n");
            break;
        case IPV4:
            printf("IPV4\n");
            break;
        case IPV4_PORT:
            printf("IPV4_PORT\n");
            break;
        case IPV6:
            printf("IPV6\n");
            break;
        case IPV6_PORT:
            printf("IPV6_PORT\n");
            break;
    }
    printf("Host: %s\tPort: %d\n\n", result, port);
    for(i = 0; i < 0xFF; i++) {
        result[i] = 0;
    }

    valid = requestTarget_marshall(test9, result, (uint16_t)0xFF, (uint16_t*)&port);
    printf("State 9: ");
    switch(valid) {
        //  ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT
        case ERROR:
            printf("Error in test 9.\n");
            break;
        case EMPTY:
            printf("EMPTY\n");
            break;
        case DOMAIN_HOST:
            printf("DOMAIN_HOST\n");
            break;
        case DOMAIN_HOST_PORT:
            printf("DOMAIN_HOST_PORT\n");
            break;
        case IPV4:
            printf("IPV4\n");
            break;
        case IPV4_PORT:
            printf("IPV4_PORT\n");
            break;
        case IPV6:
            printf("IPV6\n");
            break;
        case IPV6_PORT:
            printf("IPV6_PORT\n");
            break;
    }
    printf("Host: %s\tPort: %d\n\n", result, port);
    for(i = 0; i < 0xFF; i++) {
        result[i] = 0;
    }

    valid = requestTarget_marshall(test10, result, (uint16_t)0xFF, (uint16_t*)&port);
    printf("State 10: ");
    switch(valid) {
        //  ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT
        case ERROR:
            printf("Error in test 10.\n");
            break;
        case EMPTY:
            printf("EMPTY\n");
            break;
        case DOMAIN_HOST:
            printf("DOMAIN_HOST\n");
            break;
        case DOMAIN_HOST_PORT:
            printf("DOMAIN_HOST_PORT\n");
            break;
        case IPV4:
            printf("IPV4\n");
            break;
        case IPV4_PORT:
            printf("IPV4_PORT\n");
            break;
        case IPV6:
            printf("IPV6\n");
            break;
        case IPV6_PORT:
            printf("IPV6_PORT\n");
            break;
    }
    printf("Host: %s\tPort: %d\n\n", result, port);
    for(i = 0; i < 0xFF; i++) {
        result[i] = 0;
    }

    valid = requestTarget_marshall(test11, result, (uint16_t)0xFF, (uint16_t*)&port);
    printf("State 11: ");
    switch(valid) {
        //  ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT
        case ERROR:
            printf("Error in test 11.\n");
            break;
        case EMPTY:
            printf("EMPTY\n");
            break;
        case DOMAIN_HOST:
            printf("DOMAIN_HOST\n");
            break;
        case DOMAIN_HOST_PORT:
            printf("DOMAIN_HOST_PORT\n");
            break;
        case IPV4:
            printf("IPV4\n");
            break;
        case IPV4_PORT:
            printf("IPV4_PORT\n");
            break;
        case IPV6:
            printf("IPV6\n");
            break;
        case IPV6_PORT:
            printf("IPV6_PORT\n");
            break;
    }
    printf("Host: %s\tPort: %d\n\n", result, port);
    for(i = 0; i < 0xFF; i++) {
        result[i] = 0;
    }

    valid = requestTarget_marshall(test12, result, (uint16_t)0xFF, (uint16_t*)&port);
    printf("State 12: ");
    switch(valid) {
        //  ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT
        case ERROR:
            printf("Error in test 12.\n");
            break;
        case EMPTY:
            printf("EMPTY\n");
            break;
        case DOMAIN_HOST:
            printf("DOMAIN_HOST\n");
            break;
        case DOMAIN_HOST_PORT:
            printf("DOMAIN_HOST_PORT\n");
            break;
        case IPV4:
            printf("IPV4\n");
            break;
        case IPV4_PORT:
            printf("IPV4_PORT\n");
            break;
        case IPV6:
            printf("IPV6\n");
            break;
        case IPV6_PORT:
            printf("IPV6_PORT\n");
            break;
    }
    printf("Host: %s\tPort: %d\n\n", result, port);
    for(i = 0; i < 0xFF; i++) {
        result[i] = 0;
    }

    valid = requestTarget_marshall(test13, result, (uint16_t)0xFF, (uint16_t*)&port);
    printf("State 13: ");
    switch(valid) {
        //  ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT
        case ERROR:
            printf("Error in test 13.\n");
            break;
        case EMPTY:
            printf("EMPTY\n");
            break;
        case DOMAIN_HOST:
            printf("DOMAIN_HOST\n");
            break;
        case DOMAIN_HOST_PORT:
            printf("DOMAIN_HOST_PORT\n");
            break;
        case IPV4:
            printf("IPV4\n");
            break;
        case IPV4_PORT:
            printf("IPV4_PORT\n");
            break;
        case IPV6:
            printf("IPV6\n");
            break;
        case IPV6_PORT:
            printf("IPV6_PORT\n");
            break;
    }
    printf("Host: %s\tPort: %d\n\n", result, port);
    for(i = 0; i < 0xFF; i++) {
        result[i] = 0;
    }

    valid = requestTarget_marshall(test14, result, (uint16_t)0xFF, (uint16_t*)&port);
    printf("State 14: ");
    switch(valid) {
        //  ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT
        case ERROR:
            printf("Error in test 14.\n");
            break;
        case EMPTY:
            printf("EMPTY\n");
            break;
        case DOMAIN_HOST:
            printf("DOMAIN_HOST\n");
            break;
        case DOMAIN_HOST_PORT:
            printf("DOMAIN_HOST_PORT\n");
            break;
        case IPV4:
            printf("IPV4\n");
            break;
        case IPV4_PORT:
            printf("IPV4_PORT\n");
            break;
        case IPV6:
            printf("IPV6\n");
            break;
        case IPV6_PORT:
            printf("IPV6_PORT\n");
            break;
    }
    printf("Host: %s\tPort: %d\n\n", result, port);
    for(i = 0; i < 0xFF; i++) {
        result[i] = 0;
    }

    valid = requestTarget_marshall(test15, result, (uint16_t)0xFF, (uint16_t*)&port);
    printf("State 15: ");
    switch(valid) {
        //  ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT
        case ERROR:
            printf("Error in test 15.\n");
            break;
        case EMPTY:
            printf("EMPTY\n");
            break;
        case DOMAIN_HOST:
            printf("DOMAIN_HOST\n");
            break;
        case DOMAIN_HOST_PORT:
            printf("DOMAIN_HOST_PORT\n");
            break;
        case IPV4:
            printf("IPV4\n");
            break;
        case IPV4_PORT:
            printf("IPV4_PORT\n");
            break;
        case IPV6:
            printf("IPV6\n");
            break;
        case IPV6_PORT:
            printf("IPV6_PORT\n");
            break;
    }
    printf("Host: %s\tPort: %d\n\n", result, port);
    for(i = 0; i < 0xFF; i++) {
        result[i] = 0;
    }

    valid = requestTarget_marshall(test16, result, (uint16_t)0xFF, (uint16_t*)&port);
    printf("State 16: ");
    switch(valid) {
        //  ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT
        case ERROR:
            printf("Error in test 16.\n");
            break;
        case EMPTY:
            printf("EMPTY\n");
            break;
        case DOMAIN_HOST:
            printf("DOMAIN_HOST\n");
            break;
        case DOMAIN_HOST_PORT:
            printf("DOMAIN_HOST_PORT\n");
            break;
        case IPV4:
            printf("IPV4\n");
            break;
        case IPV4_PORT:
            printf("IPV4_PORT\n");
            break;
        case IPV6:
            printf("IPV6\n");
            break;
        case IPV6_PORT:
            printf("IPV6_PORT\n");
            break;
    }
    printf("Host: %s\tPort: %d\n\n", result, port);
    for(i = 0; i < 0xFF; i++) {
        result[i] = 0;
    }

    valid = requestTarget_marshall(test17, result, (uint16_t)0xFF, (uint16_t*)&port);
    printf("State 17: ");
    switch(valid) {
        //  ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT
        case ERROR:
            printf("Error in test 17.\n");
            break;
        case EMPTY:
            printf("EMPTY\n");
            break;
        case DOMAIN_HOST:
            printf("DOMAIN_HOST\n");
            break;
        case DOMAIN_HOST_PORT:
            printf("DOMAIN_HOST_PORT\n");
            break;
        case IPV4:
            printf("IPV4\n");
            break;
        case IPV4_PORT:
            printf("IPV4_PORT\n");
            break;
        case IPV6:
            printf("IPV6\n");
            break;
        case IPV6_PORT:
            printf("IPV6_PORT\n");
            break;
    }
    printf("Host: %s\tPort: %d\n\n", result, port);
    for(i = 0; i < 0xFF; i++) {
        result[i] = 0;
    }

    valid = requestTarget_marshall(test18, result, (uint16_t)0xFF, (uint16_t*)&port);
    printf("State 18: ");
    switch(valid) {
        //  ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT
        case ERROR:
            printf("Error in test 18.\n");
            break;
        case EMPTY:
            printf("EMPTY\n");
            break;
        case DOMAIN_HOST:
            printf("DOMAIN_HOST\n");
            break;
        case DOMAIN_HOST_PORT:
            printf("DOMAIN_HOST_PORT\n");
            break;
        case IPV4:
            printf("IPV4\n");
            break;
        case IPV4_PORT:
            printf("IPV4_PORT\n");
            break;
        case IPV6:
            printf("IPV6\n");
            break;
        case IPV6_PORT:
            printf("IPV6_PORT\n");
            break;
    }
    printf("Host: %s\tPort: %d\n\n", result, port);
    for(i = 0; i < 0xFF; i++) {
        result[i] = 0;
    }

    valid = requestTarget_marshall(test19, result, (uint16_t)0xFF, (uint16_t*)&port);
    printf("State 19: ");
    switch(valid) {
        //  ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT
        case ERROR:
            printf("Error in test 19.\n");
            break;
        case EMPTY:
            printf("EMPTY\n");
            break;
        case DOMAIN_HOST:
            printf("DOMAIN_HOST\n");
            break;
        case DOMAIN_HOST_PORT:
            printf("DOMAIN_HOST_PORT\n");
            break;
        case IPV4:
            printf("IPV4\n");
            break;
        case IPV4_PORT:
            printf("IPV4_PORT\n");
            break;
        case IPV6:
            printf("IPV6\n");
            break;
        case IPV6_PORT:
            printf("IPV6_PORT\n");
            break;
    }
    printf("Host: %s\tPort: %d\n\n", result, port);
    for(i = 0; i < 0xFF; i++) {
        result[i] = 0;
    }

    valid = requestTarget_marshall(test20, result, (uint16_t)0xFF, (uint16_t*)&port);
    printf("State 20: ");
    switch(valid) {
        //  ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT
        case ERROR:
            printf("Error in test 20.\n");
            break;
        case EMPTY:
            printf("EMPTY\n");
            break;
        case DOMAIN_HOST:
            printf("DOMAIN_HOST\n");
            break;
        case DOMAIN_HOST_PORT:
            printf("DOMAIN_HOST_PORT\n");
            break;
        case IPV4:
            printf("IPV4\n");
            break;
        case IPV4_PORT:
            printf("IPV4_PORT\n");
            break;
        case IPV6:
            printf("IPV6\n");
            break;
        case IPV6_PORT:
            printf("IPV6_PORT\n");
            break;
    }
    printf("Host: %s\tPort: %d\n\n", result, port);
    for(i = 0; i < 0xFF; i++) {
        result[i] = 0;
    }

    valid = requestTarget_marshall(test21, result, (uint16_t)0xFF, (uint16_t*)&port);
    printf("State 21: ");
    switch(valid) {
        //  ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT
        case ERROR:
            printf("Error in test 21.\n");
            break;
        case EMPTY:
            printf("EMPTY\n");
            break;
        case DOMAIN_HOST:
            printf("DOMAIN_HOST\n");
            break;
        case DOMAIN_HOST_PORT:
            printf("DOMAIN_HOST_PORT\n");
            break;
        case IPV4:
            printf("IPV4\n");
            break;
        case IPV4_PORT:
            printf("IPV4_PORT\n");
            break;
        case IPV6:
            printf("IPV6\n");
            break;
        case IPV6_PORT:
            printf("IPV6_PORT\n");
            break;
    }
    printf("Host: %s\tPort: %d\n\n", result, port);
    for(i = 0; i < 0xFF; i++) {
        result[i] = 0;
    }
    valid = requestTarget_marshall(test22, result, (uint16_t)0xFF, (uint16_t*)&port);
    printf("State 22: ");
    switch(valid) {
        //  ERROR = 0, EMPTY, DOMAIN_HOST, DOMAIN_HOST_PORT, IPV4, IPV4_PORT, IPV6, IPV6_PORT
        case ERROR:
            printf("Error in test 22.\n");
            break;
        case EMPTY:
            printf("EMPTY\n");
            break;
        case DOMAIN_HOST:
            printf("DOMAIN_HOST\n");
            break;
        case DOMAIN_HOST_PORT:
            printf("DOMAIN_HOST_PORT\n");
            break;
        case IPV4:
            printf("IPV4\n");
            break;
        case IPV4_PORT:
            printf("IPV4_PORT\n");
            break;
        case IPV6:
            printf("IPV6\n");
            break;
        case IPV6_PORT:
            printf("IPV6_PORT\n");
            break;
    }
    printf("Host: %s\tPort: %d\n\n", result, port);
    for(i = 0; i < 0xFF; i++) {
        result[i] = 0;
    }

	printf("\nBye!\n");
	return 0;
}
