#include "admin.h"
#include "utils/mediaTypes/mediatypes.c"

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

    uint8_t password[0xFF] = {0};
    int i = 1;
    int c = 0;

    printf("Please insert admin password\n");
    while((c = getchar()) != '\n' && i < 0xFF) {
        password[i++] = c;
    }
    password[0] = i-1;
    send(proxy,password,i,0);
    recv(proxy, &c, 1, 0);

    if(c == LOGGED_IN){
        printf("Login successful\n");
    }else {
        printf("Server error\n");
        exit(1);
    }
}

void
sendRequest(int proxy, uint8_t method,uint8_t len, uint8_t * data ){
    int i = 0;
    uint8_t msg[0xFF] = {0};
    msg[i++] = method;
    msg[i++] = len;
    memcpy(msg+i,data,len);
    send(proxy,msg,len+i,0);
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
    printf("- addmediatypes <string>, <string>, <string>, <string>, ...\n");
    printf("          \n");
    printf("- rmmediatype <string>\n");
    printf("          \n");
    printf("-rmmediatypes <string>, <string>, <string>, <string>, ...\n");
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
    sendRequest(proxy,SET_TRANSFORMATION,strlen((char *)parameter),parameter);
    uint8_t response[MAX_SERVER_RESPONSE] = {0};
    receiveResponse(proxy,3,response);
    switch(response[0]){
        case OK:
            printf("Transformation has been set to: %s\n",parameter);
            break;
        default:
            printf("Something went wrong and the transformation has not been set to: %s\n",parameter);
    }
}

void
addMediaType(int proxy, uint8_t * parameter){
    uint8_t mt[2] = {0};
    mt[0] = strToMediaType((char *)parameter);
    if(mt[0] == MT_NONE){
        printf("%s is an invalid media type\n",parameter);
        return;
    }
    sendRequest(proxy,ADD_MEDIA_TYPE,1,mt);
    uint8_t response[MAX_SERVER_RESPONSE] = {0};
    receiveResponse(proxy,3,response);
    switch(response[0]){
        case OK:
            printf("The media type %s has been added\n",parameter);
            break;
        default:
            printf("Something went wrong and the media type %s has not been added\n",parameter);
    }
}

void
addMediaTypes(int proxy, uint8_t * parameter){

    uint8_t mt[QUANTITY_MEDIATYPES] = {0};
    uint8_t auxmt[MT_MAX_LONG] = {0};
    int i = 0, j = 0, h = 0;
    while(parameter[i] != 0){
        if(parameter[i] == ','){
            memset(auxmt,0,MT_MAX_LONG);
            memcpy(auxmt,parameter+j,i-j);
            mt[h] =strToMediaType((char *)(auxmt));
            h++;
            j = i+1;
        }
        i++;
    }

    if(parameter[i] == 0){
        memset(auxmt,0,MT_MAX_LONG);
        memcpy(auxmt,parameter+j,i-j);
        mt[h] =strToMediaType((char *)(auxmt));
        h++;
    }
    sendRequest(proxy,ADD_MEDIA_TYPE,h,mt);
    uint8_t response[MAX_SERVER_RESPONSE] = {0};
    receiveResponse(proxy,3,response);
    switch(response[0]){
        case OK:
            printf("The media types typed have been added\n");
            break;
        default:
            printf("Something went wrong and the media types typed have not been added\n");
    }
}


void
removeMediaType(int proxy, uint8_t * parameter){
    uint8_t mt[2] = {0};
    mt[0] = strToMediaType((char *)parameter);
    if(mt[0] == MT_NONE){
        printf("%s is an invalid media type\n",parameter);
        return;
    }
    sendRequest(proxy,REMOVE_MEDIA_TYPE,1,mt);
    uint8_t response[MAX_SERVER_RESPONSE] = {0};
    receiveResponse(proxy,3,response);
    switch(response[0]){
        case OK:
            printf("The media type %s has been removed\n",parameter);
            break;
        default:
            printf("The media type %s has not been added\n",parameter);
    }
}

void
removeMediaTypes(int proxy, uint8_t * parameter){

    uint8_t mt[QUANTITY_MEDIATYPES] = {0};
    uint8_t auxmt[MT_MAX_LONG] = {0};
    int i = 0, j = 0, h = 0;
    while(parameter[i] != 0){
        if(parameter[i] == ','){
            memset(auxmt,0,MT_MAX_LONG);
            memcpy(auxmt,parameter+j,i-j);
            mt[h] =strToMediaType((char *)(auxmt));
            h++;
            j = i+1;
        }
        i++;
    }

    if(parameter[i] == 0){
        memset(auxmt,0,MT_MAX_LONG);
        memcpy(auxmt,parameter+j,i-j);
        mt[h] =strToMediaType((char *)(auxmt));
        h++;
    }
    sendRequest(proxy,REMOVE_MEDIA_TYPE,h,mt);
    uint8_t response[MAX_SERVER_RESPONSE] = {0};
    receiveResponse(proxy,3,response);
    switch(response[0]){
        case OK:
            printf("The media types typed have been removed\n");
            break;
        default:
            printf("Something went wrong and the media types typed have not been removed\n");
    }
}

void
setBufferSize(int proxy, uint8_t * parameter){
    if(validateLong(parameter) == -1){
        printf("'%s' is not a valid parameter for this command\n",parameter);
        return;
    }
    sendRequest(proxy,SET_TIMEOUT,SIZE_INTEGER,parameter);

    uint8_t response[MAX_SERVER_RESPONSE] = {0};
    receiveResponse(proxy,3,response);
    switch(response[0]){
        case OK:
            printf("The buffer size has been set to %s\n",parameter);
            break;
        default:
            printf("Something went wrong and the buffer size has not been set to %s\n",parameter);
    }
}

void
setTimeOut(int proxy, uint8_t * parameter){
    if(validateLong(parameter) == -1){
        printf("'%s' is not a valid parameter for this command\n",parameter);
        return;
    }
    sendRequest(proxy,SET_TIMEOUT,SIZE_INTEGER,parameter);

    uint8_t response[MAX_SERVER_RESPONSE] = {0};
    receiveResponse(proxy,3,response);
    switch(response[0]){
        case OK:
            printf("The time out has been set to %s\n",parameter);
            break;
        default:
            printf("Something went wrong and the time out has not been set to %s\n",parameter);
    }
}

void
printTimeOut(uint8_t * data, uint8_t len){
        uint32_t ttl;
        memcpy(&ttl, data, SIZE_INTEGER);
        ttl = ntohl(ttl);
        printf("The current time out is: %i\n", ttl);
}

void
printBufferSize(uint8_t * data, uint8_t len){
        uint32_t bz;
        memcpy(&bz, data, SIZE_INTEGER);
        bz = ntohl(bz);
        printf("The current buffer size is: %i\n", bz);
}

void
printMediaTypes(uint8_t * data, uint8_t len){
    if(len == 0){
        printf("There are not media types added\n");
        return;
    }
    printf("The media types added are:\n");
    int i = 0;
    while(i<len){
        printf("%i) ",i);
        switch(data[i++]){
            case MT_TEXT_ALL: printf("text/*\n");
                break;
            case MT_TEXT_PLAIN: printf("text/plain\n");
                break;
            case MT_TEXT_HTML: printf("text/html\n");
                break;
            case MT_TEXT_CSS: printf("text/cass\n");
                break;
            case MT_TEXT_JAVASCRIPT: printf("text/javascript\n");
                break;
            case MT_TEXT_MARKDOWN: printf("text/markdown\n");
                break;
            case MT_TEXT_XML: printf("text/xml\n");
                break;
            case MT_IMAGE_ALL: printf("image/*\n");
                break;
            case MT_IMAGE_GIF: printf("image/gif\n");
                break;
            case MT_IMAGE_JPEG: printf("image/jpeg\n");
                break;
            case MT_IMAGE_PNG: printf("image/png\n");
                break;
            case MT_IMAGE_TIFF: printf("image/tiff\n");
                break;
            case MT_APPLICATION_ALL: printf("application/*\n");
                break;
            case MT_APPLICATION_JSON: printf("application/json\n");
                break;
            case MT_APPLICATION_JAVASCRIPT: printf("application/javascript\n");
                break;
        }
    }
}

void
printTransformation(uint8_t * data, uint8_t len){
    if(len == 0){
        printf("There is no current transformation yet\n");
        return;
    }
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

    uint32_t gets;
    memcpy(&gets, data+i, SIZE_INTEGER);
    gets = ntohl(gets);
    
    i+=SIZE_INTEGER;


    uint32_t posts;
    memcpy(&posts, data+i, SIZE_INTEGER);
    posts = ntohl(posts);
    
    i+=SIZE_INTEGER;

    uint32_t heads;
    memcpy(&heads, data+i, SIZE_INTEGER);
    heads = ntohl(heads);
    
    i+=SIZE_INTEGER;

    uint32_t clients;
     memcpy(&clients, data+i, SIZE_INTEGER);
    clients = ntohl(clients);
   
    i+=SIZE_INTEGER;

    uint32_t historicConnections;
    memcpy(&historicConnections, data+i, SIZE_INTEGER);
    historicConnections = ntohl(historicConnections);

    printf("Amount of GET's: %d\n",gets);
    printf("Amount of POST's's: %d\n",posts);
    printf("Amount of HEAD's: %d`\n",heads);
    printf("Active clients: %d\n",clients);
    printf("Historic connections: %d\n",historicConnections);
}

void
getProperties(int proxy,enum client_request command){

    sendRequest(proxy,command,0,0);
    uint8_t data[MAX_SERVER_RESPONSE] = {0};
    receiveResponse(proxy,MAX_SERVER_RESPONSE,data);

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
            }
            else if(!param){
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
        }else if(strcmp("rmtransf",command)==0){
            setTransformation(proxy,parameter);
        }else if(strcmp("addmediatype",command)==0){
            printf("%s\n", parameter);
            addMediaType(proxy,parameter);
        }else if(strcmp("addmediatypes",command)==0){
            addMediaTypes(proxy,parameter);
        }else if(strcmp("rmmediatype",command)==0){
            removeMediaType(proxy,parameter);
        }else if(strcmp("rmmediatypes",command)==0){
            removeMediaTypes(proxy,parameter);
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
main(int argc, char ** argv){
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

