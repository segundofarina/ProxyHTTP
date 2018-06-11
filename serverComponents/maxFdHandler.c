#include "maxFdHandler.h"

#include <unistd.h>
#include <fcntl.h>

static int spareFd = -1;

/* Keep spare fd to avoid problems when max fd amount reached */
void keepSpareFd() {
	if(spareFd == -1) {
		spareFd = open("/dev/null", O_RDONLY);
	}
}

void handleAcceptEmfile(int fd, struct sockaddr * clientAddr, socklen_t  * clientAddrLen) {
	/* Close spareFd, to get new fd to accept the connection */
	close(spareFd);
	spareFd = accept(fd, clientAddr, clientAddrLen);

	/* Close accepted connection */
	close(spareFd);
	spareFd = -1;   
	
	/* Open again spareFd for future errors */
	keepSpareFd();
}