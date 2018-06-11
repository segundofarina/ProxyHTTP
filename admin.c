#include "admin.h"

long
validateLong(uint8_t * num){

    char * number = (char *) num;
    char *end     = 0;
    const long sl = strtol(number, &end, 10);

    if (end == number|| '\0' != *end
        || ((LONG_MIN == sl || LONG_MAX == sl) && ERANGE == errno)
        || sl < 0 || sl > USHRT_MAX) {
        return -1;
    }

    return sl;
}

void
authenticate(int proxy){

    bool loggedIn        = false;
    uint8_t buffer[0xFF] = {0};
    uint8_t i            = 1;
    int c                = 0;

    while(!loggedIn){
        printf("Please insert admin password\n");
        while((c = tolower(getchar())) != '\n' && i < 0xFF) {
            buffer[i++] = c;
        }
        buffer[0] = i-1;
        send(proxy,buffer,i,0);
        recv(proxy, &c, 1, 0);
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

void
sendRequest(int proxy, uint8_t method,uint8_t len, uint8_t * data ){

    uint8_t * msg = malloc(sizeof(uint8_t)*(len+2));
    msg[0] = method;
    msg[1] = len;
    memcpy(msg+2,data,len);
    send(proxy,msg,len,0);
}

void
receiveResponse(int proxy,uint8_t len, uint8_t * data ){
    recv(proxy,data,len,0);
}

void
help(){
    printf("These are the available commands:\n");
    printf("- settransf <string>\n");
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
    printf("- getmediatypes \n");
    printf("          shows current mediatypes\n");
    printf("- exit \n");
    printf("          exits the admin panel\n");


}

void
setTransformation(int proxy, uint8_t * parameter){
    if(validateLong(parameter) == -1){
        printf("'%s' is an invalid parameter for this command",parameter);
        return;
    }
    printf("Transformation will be set to: %s\n",parameter);
    sendRequest(proxy,SET_TRANSFORMATION,strlen((char *)parameter),parameter);

}

void
addMediaType(int proxy, uint8_t * parameter){
    printf("The media type %s will be added\n",parameter);
    sendRequest(proxy,ADD_MEDIA_TYPE,strlen((char *)parameter),parameter);
}
void
removeMediaType(int proxy, uint8_t * parameter){
    printf("The media type %s will be removed\n",parameter);
    sendRequest(proxy,REMOVE_MEDIA_TYPE,strlen((char *)parameter),parameter);
}

void
setBufferSize(int proxy, uint8_t * parameter){
    if(validateLong(parameter) == -1){
        printf("'%s' is not a valid parameter for this command\n",parameter);
        return;
    }
    printf("The buffer size will be set to %s\n",parameter);
    sendRequest(proxy,SET_BUFFER_SIZE,strlen((char *)parameter),parameter);
}

void
setTimeOut(int proxy, uint8_t * parameter){
    if(validateLong(parameter) == -1){
        printf("'%s' is not a valid parameter for this command\n",parameter);
        return;
    }
    printf("The time out will be set to %s\n",parameter);
    sendRequest(proxy,SET_TIMEOUT,strlen((char *)parameter),parameter);
}

void
printTimeOut(uint8_t * data, uint8_t len){
    if(len == 1) {
        printf("The current time out is: %i\n", data[0]);
    }
}

void
printBufferSize(uint8_t * data, uint8_t len){
    if(len == 1) {
        printf("The current buffer size is: %i\n", data[0]);
    }
}

void
printMediaTypes(uint8_t * data, uint8_t len){
    printf("Media types supported:\n");
    int i = 0;
    while(i<len){
        printf("%i) ",i);
        switch(data[i++]){
            case TEXT_ALL: printf("text/*\n");
                break;
            case TEXT_PLAIN: printf("text/plain\n");
                break;
            case TEXT_HTML: printf("text/html\n");
                break;
            case TEXT_CSS: printf("text/cass\n");
                break;
            case TEXT_JAVASCRIPT: printf("text/javascript\n");
                break;
            case TEXT_MARKDOWN: printf("text/markdown\n");
                break;
            case TEXT_XML: printf("text/xml\n");
                break;
            case IMAGE_ALL: printf("image/*\n");
                break;
            case IMAGE_GIF: printf("image/gif\n");
                break;
            case IMAGE_JPEG: printf("image/jpeg\n");
                break;
            case IMAGE_PNG: printf("image/png\n");
                break;
            case IMAGE_TIFF: printf("image/tiff\n");
                break;
            case APPLICATION_ALL: printf("application/*\n");
                break;
            case APPLICATION_JSON: printf("application/json\n");
                break;
            case APPLICATION_JAVASCRIPT: printf("application/javascript\n");
                break;
        }
    }
}

void
printTransformation(uint8_t * data, uint8_t len){
    printf("The current transformation is: ");
    int i = 0;
    while(i<len){
        printf("%c",data[i++]);
    }
    printf("\n");
}

void
printMetrics(uint8_t * data, uint8_t len){
    int i = 0;
    printf("Amount of GET's: %i\n",data[i++]);
    printf("Amount of POST's's: %i\n",data[i++]);
    printf("Amount of HEAD's: %i\n",data[i++]);
    printf("Active clients: %i\n",data[i++]);
    printf("Historic connections: %i\n",data[i]);
}

void
getProperties(int proxy,enum client_request command){

    sendRequest(proxy,command,0,0);
    uint8_t data[MAX_SERVER_RESPONSE] = {0};
    receiveResponse(proxy,(uint8_t)MAX_SERVER_RESPONSE,data);

    int i          = 0;
    uint8_t status = data[i++];
    uint8_t len    = data[i++];

    if(status == ERR){
        printf("An error has ocurred. Please try again\n");
    }else if(status == NOT_SUPPORTED){
        printf("The command entered is not supported by the proxy\n");
    }else if(status == OK){
        switch(command){
            case GET_METRICS: printMetrics(data+i,len);
                break;
            case GET_TRANSFORMATION: printTransformation(data+i,len);
                break;
            case GET_MEDIA_TYPES: printMediaTypes(data+i,len);
                break;
            case GET_BUFFER_SIZE: printBufferSize(data+i,len);
                break;
            case GET_TIMEOUT: printTimeOut(data+i,len);
                break;
            default: return;
        }
    }
}

void
shell(int proxy){


    authenticate(proxy);

    char command[0xFF];
    uint8_t parameter[0xFF];
    bool param;
    int c;
    int ci;
    int pi;

    while(true){

        memset(command,0,0xFF);
        memset(parameter,0,0xFF);
        param = false;
        ci    = 0;
        pi    = 0;

        printf("Admin:$ ");

        do{
            c = getchar();
        }while(c == ' ');

        while(c != '\n' && ci < 0xFF && pi < 0xFF) {
            if(c == ' '){
                if(!param) {
                    param = true;
                }
            }else if(!param){
                command[ci++] = c;
            }else if(param){
                parameter[pi++] = c;
            }
            c = tolower(getchar());

        }
        command[ci]   = 0;
        parameter[pi] = 0;

        if(strcmp("help",command)==0){
            help();
        }else if(strcmp("settransf",command)==0){
            setTransformation(proxy,parameter);
        }else if(strcmp("addmediatype",command)==0){
            addMediaType(proxy,parameter);
        }else if(strcmp("rmmediatype",command)==0){
            removeMediaType(proxy,parameter);
        }else if(strcmp("setbuffsize",command)==0){
            setBufferSize(proxy,parameter);
        }else if(strcmp("settimeout",command)==0) {
            setTimeOut(proxy,parameter);
        }else if(strcmp("getmetrics",command)==0){
            getProperties(proxy,GET_METRICS);
        }else if(strcmp("gettransf",command)==0){
            getProperties(proxy,GET_TRANSFORMATION);
        }else if(strcmp("getbuffsize",command)==0){
            getProperties(proxy,GET_BUFFER_SIZE);
        }else if(strcmp("gettimeout",command)==0) {
            getProperties(proxy,GET_TIMEOUT);
        }else if(strcmp("getmediatypes",command)==0){
            getProperties(proxy,GET_MEDIA_TYPES);
        }else if(strcmp("exit",command)==0){
            break;
        }else{
            printf("Command not found enter 'help' to see available commands\n");
        }
    }

}

int
connectToProxy(char * host, int port,char ** errMsg){

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);

    const int proxy = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
        port = validateLong((uint8_t *)argv[2]);

        if(port == -1){
            fprintf(stderr, "port should be an integer: %s\n", argv[2]);
        }
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
