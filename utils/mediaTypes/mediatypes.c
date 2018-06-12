#include "mediatypes.h"
#include <string.h>

enum MediaType strToMediaType(const char * str) {

    if(strcmp("text/*", str) == 0) {
        return MT_TEXT_ALL;
    }else if(strcmp("text/plain", str) == 0){
        return MT_TEXT_PLAIN;
    }else if(strcmp("text/html", str) == 0){
        return  MT_TEXT_HTML;
    }else if(strcmp("text/css", str) == 0){
        return MT_TEXT_CSS;
    }else if(strcmp("text/javascript", str) == 0){
        return MT_TEXT_JAVASCRIPT;
    }else if(strcmp("text/markdown", str) == 0){
        return MT_TEXT_MARKDOWN;
    }else if(strcmp("text/xml", str) == 0){
        return MT_TEXT_XML;
    }else if(strcmp("image/*", str) == 0){
        return MT_IMAGE_ALL;
    }else if(strcmp("image/gif", str) == 0){
        return MT_IMAGE_GIF;
    }else if(strcmp("image/jpeg", str) == 0){
        return MT_IMAGE_JPEG;
    }else if(strcmp("image/png", str) == 0){
        return MT_IMAGE_PNG;
    }else if(strcmp("image/tiff", str) == 0){
        return MT_IMAGE_TIFF;
    }else if(strcmp("application/*", str) == 0){
        return MT_APPLICATION_ALL;
    }else if(strcmp("application/json", str) == 0){
        return MT_APPLICATION_JSON;
    }else if(strcmp("application/javascript", str) == 0){
        return MT_APPLICATION_JAVASCRIPT;
    }

    return MT_NONE;
}