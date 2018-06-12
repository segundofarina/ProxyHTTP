#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "transformationFork.h"

#include "transformationManager.h"
#include "transErrorManager.h"

#define READ 0
#define WRITE 1

void runChildCode(const int readPipe[2], const int writePipe[2], const char * cmd);
void configureChildPipes(const int readPipe[2], const int writePipe[2]);

int forkTransformation(int * readTransformFd, int * writeTransformFd/* , transformation */) {
    /* read is from child to parent, write is from parent to child */
    int readPipe[2], writePipe[2];
    int pid;

    if(pipe(readPipe) == -1) {
        return -1;
    }
    if(pipe(writePipe) == -1) {
        return -1;
    }

    pid = fork();
    if(pid < 0) {
        return -1;
    }

    if(pid == 0) {
        runChildCode(readPipe, writePipe, getTransformation());
        exit(0);
    }

    /* Parent code */

    /* Close unused pipes */
    close(writePipe[READ]);
    close(readPipe[WRITE]);

    *readTransformFd = readPipe[READ];
    *writeTransformFd = writePipe[WRITE];
    
    return pid;
}

void runChildCode(const int readPipe[2], const int writePipe[2], const char * cmd) {
    int error = 0, errorFd = -1;
    configureChildPipes(readPipe, writePipe);

    errorFd = open(getTransformationErrorFile(), O_WRONLY | O_CREAT | O_APPEND);
    if(errorFd != -1) {
        dup2(errorFd, STDERR_FILENO);
        close(errorFd);
    } else {
        close(STDERR_FILENO);
    }

	if (execl("/bin/sh", "sh", "-c", cmd, (char *) 0) == -1){
        //error
        
        close(readPipe[WRITE]);
	    close(writePipe[READ]);
        
        error = 1;
    }
    exit(error);
}

void configureChildPipes(const int readPipe[2], const int writePipe[2]) {
    close(writePipe[WRITE]);
    close(readPipe[READ]);

    /* Close old stdin, stdout */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);

    /* Set pipes as new stdin, stdout */
    dup2(writePipe[READ], STDIN_FILENO);
    dup2(readPipe[WRITE], STDOUT_FILENO);

	close(readPipe[WRITE]);
	close(writePipe[READ]);
}
