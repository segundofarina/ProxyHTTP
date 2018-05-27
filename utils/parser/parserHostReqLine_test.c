#include <stdio.h>
#include <string.h>
#include <stdlib.h>

hostData requestTarget_marshall(char * buffer, char * result, uint16_t len);

int main(void) {
	char * test1 = "http://itba.edu.ar/foo?bar";
	char * test2 = "itba.edu.ar/./hello///";
	char * test2a = "http://itba.edu.ar/./hello/../";
	char * test3 = "example.com:9090/hola";
	char * test4 = "http://itba.edu.ar/foo?barhttp://itba";
	char * test5 = "example.com/.../hola";
	char * valid;

	valid = parseReqTarget(test1);
	if(valid != NULL) {
		printf("1: %s\n", valid);
	}
	free(valid);
	// printf("%s\n", valid);
	valid = parseReqTarget(test2);
	if(valid != NULL) {
		printf("2: %s\n", valid);
	}
	free(valid);
	// printf("%s\n", valid);
	valid = parseReqTarget(test2a);
	if(valid != NULL) {
		printf("2a: %s\n", valid);
	}
	free(valid);
	// printf("%s\n", valid);
	valid = parseReqTarget(test3);
	if(valid != NULL) {
		printf("3: %s\n", valid);
	}
	free(valid);
	// printf("%s\n", valid);
	valid = parseReqTarget(test4);
	printf("4: ");
	if(valid == NULL) {
		printf("NULL\n");
	}
	free(valid);
	// printf("%s\n", valid);
	valid = parseReqTarget(test5);
	printf("5: ");
	if(valid == NULL) {
		printf("NULL");
	}
	free(valid);
	// printf("%s\n", valid);
	printf("\n");
}
