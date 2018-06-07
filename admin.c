//
// Created by Segundo Fariña on 7/6/18.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_ERROR=0xFF
enum auth_response{
    LOGGED_IN=0x00,
    LOGIN_FAILED=0x01
};
enum server_response{
    OK=0x00,
    ERR=0x01,
    NOT_SUPPORTED=0x02
};
enum client_request{
    SET_TRANSFORMATION=0x00,
    ADD_MEDIA_TYPE=0x01,
    REMOVE_MEDIA_TYPE=0x02,
    SET_BUFFER_SIZE=0x03,
    SET_TIMEOUT=0x04,
    GET_METRICS=0x05,
    GET_TRANSFORMATION=0x06,
    GET_MEDIA_TYPES=0x07,
    GET_BUFFER_SIZE=0x08,
    GET_TIMEOUT=0x09
};

void
authenticate(int proxy){
    bool loggedIn =false;
    char buffer[0xFF] = {0};
    int c=0,i=0;
    while(!loggedIn){
        printf("Please insert admin password\n");
        while((c = tolower(getchar())) != '\n' && i < 0xFF) {
            buffer[i++] = c;
        }

        //send(proxy,buffer,i,0);

        //recv(proxy, &c, 1, 0);
        c =LOGGED_IN;
        if(c ==LOGGED_IN){
            printf("Login succesful\n");
            loggedIn =true;
        }else if(c == LOGIN_FAILED){
            printf("Incorrect password please try again\n");
        }else {
            printf("Server error\n");
            exit(1);
        }


    }
}

void sendRequest(int proxy, uint8_t method,uint8_t len, uint8_t * data){

}

void help(){
    printf("These are the available commands:\n");
    printf("- settarnsf <string>\n");
    printf("         sets the transformation to the one passed by parameter\n");
    printf("- addmediatype <string>\n");
    printf("          \n");
    printf("- rmmediatype <string>\n");
    printf("          \n");
    printf("- setbuffsize <string>\n");
    printf("          sets buffer size for future connections\n");
    printf("- settimeout <number>\n");
    printf("          sets the timeout to the one specified\n");
    printf("- getmetrics \n");
    printf("          shows metrics\n");
    printf("- gettransf \n");
    printf("          shows current transformation\n");
    printf("- getbuffsize \n");
    printf("          shows current buffersize\n");
    printf("- gettimeout \n");
    printf("          shows current timeout\n");
    printf("- exit \n");
    printf("          exits the admin panel\n");


}

void
setTransformation(int proxy){
    printf("Insert your new transformation:\n");
    char buffer[0xFF] = {0};
    int c =0,i = 0;
    printf("Admin:$ ");
    while((c = tolower(getchar())) != '\n' && i < 0xFF) {
        buffer[i++] = c;
    }
    buffer[i]=0;
    printf("Transformation will be set to:\n %s\n Is that ok? [Y/N]\n");
    c = tolower(getchar());
    if (c == 'y'){
        sendRequest(proxy,SET_TRANSFORMATION,i+1,buffer);
    }else{
        printf("Opertaion canceled\n");
    }
}

void
shell(int proxy){


    authenticate(proxy);

    while(true){
        char buffer[0xFF] = {0};
        int c =0,i = 0;

        printf("Admin:$ ");
        while((c = tolower(getchar())) != '\n' && i < 0xFF) {
            buffer[i++] = c;
        }
        buffer[i]=0;
//        if(buffer[0]==0){
//            printf("\n");
//
//        } else
        if(strncmp("help",buffer,i)==0){
            help();
        }else if(strncmp("settarnsf",buffer,i)==0){
            setTransformation(proxy);
        }else if(strncmp("addmediatype",buffer,i)==0){

        }else if(strncmp("rmmediatype",buffer,i)==0){

        }else if(strncmp("setbuffsize",buffer,i)==0){

        }else if(strncmp("settimeout",buffer,i)==0){

        }else if(strncmp("getmetrics",buffer,i)==0){
            sendRequest(proxy,GET_METRICS,0,0);
        }else if(strncmp("gettransf",buffer,i)==0){
            sendRequest(proxy,GET_TRANSFORMATION,0,0);
        }else if(strncmp("getbuffsize",buffer,i)==0){
            sendRequest(proxy,GET_BUFFER_SIZE,0,0);
        }else if(strncmp("gettimeout",buffer,i)==0){
            sendRequest(proxy,GET_TIMEOUT,0,0);
        }else if(strncmp("exit",buffer,i)==0){
            break;
        }
        else{
            printf("Command not found enter 'help' to see available commands\n");
        }

    }

}

int
connectToProxy(char * host, int port,char ** errMsg){

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);

    const int proxy = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
    if(proxy < 0) {
        *errMsg = "Unable to create socket";
        return -1;
    }

    setsockopt(proxy, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));

    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0) {
        *errMsg = "Invalid address/ Address not supported";
        return -1;
    }

    if (connect(proxy, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        *errMsg = "Connection Failed ";
        return -1;
    }


    return proxy;
}


int
main(int argc, char ** argv)
{


    int port;

   if(argc == 3) {
        char *end     = 0;
        const long sl = strtol(argv[2], &end, 10);

        if (end == argv[2]|| '\0' != *end
            || ((LONG_MIN == sl || LONG_MAX == sl) && ERANGE == errno)
            || sl < 0 || sl > USHRT_MAX) {
            fprintf(stderr, "port should be an integer: %s\n", argv[2]);
            return 1;
        }
        port = sl;
    } else {
       fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
       return 1;
   }




    char *errMsg;

    int proxy=connectToProxy(argv[1],port,&errMsg);
    if(proxy<0){
        printf("%s\nCould not connect to proxy\n",errMsg);
        return 1;
    }

    shell(proxy);
}
