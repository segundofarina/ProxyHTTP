#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parserHostReqLine.h"

int main(void) {
	char * test1 = "http://itba.edu.ar/foo?bar";
	char * test2 = "itba.edu.ar/./hello///";
	char * test2a = "http://itba.edu.ar/./hello/../";
	char * test3 = "example.com:9090/hola";
	char * test4 = "http://itba.edu.ar/foo?barhttp://itba";
	char * test5 = "example.com/.../hola";
	char * test6 = "127.0.0.1:8080";
	char * test7 = "[0:0:0:0]:8080";
	char * test8 = "[:::1]:80";
	int port = 0;
	hostData valid;
	char result[0xFF] = {0};

	valid = requestTarget_marshall(test1, result, 0xFF, &port);
	if(valid != NULL) {
		printf("State 1: %d\n", valid);
	}
	// printf("%s\n", valid);
	valid = requestTarget_marshall(test2, result, 0xFF, &port);
	if(valid != NULL) {
		printf("State 2: %d\n", valid);
	}
	// printf("%s\n", valid);
	valid = requestTarget_marshall(test2a, result, 0xFF, &port);
	if(valid != NULL) {
		printf("State 2a: %d\n", valid);
	}
	// printf("%s\n", valid);
	valid = requestTarget_marshall(test3, result, 0xFF, &port);
	if(valid != NULL) {
		printf("State 3: %d\n", valid);
	}
	valid = requestTarget_marshall(test4, result, 0xFF, &port);
	if(valid != NULL) {
		printf("State 4: %d\n", valid);
	}
	valid = requestTarget_marshall(test5, result, 0xFF, &port);
	if(valid != NULL) {
		printf("State 5: %d\n", valid);
	}
	valid = requestTarget_marshall(test6, result, 0xFF, &port);
	if(valid != NULL) {
		printf("State 6: %d\n", valid);
	}
	valid = requestTarget_marshall(test7, result, 0xFF, &port);
	if(valid != NULL) {
		printf("State 7: %d\n", valid);
	}
	valid = requestTarget_marshall(test8, result, 0xFF, &port);
	if(valid != NULL) {
		printf("State 8: %d\n", valid);
	}

	printf("\n");
}
