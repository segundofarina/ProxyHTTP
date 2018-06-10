#ifndef _TRANSFORMATION_MANAGER_H_
#define _TRANSFORMATION_MANAGER_H_

enum MediaType {
    MT_NONE,                        // default value for error
    MT_TEXT_ALL,                    // text/*
    MT_TEXT_PLAIN,                  // text/plain
    MT_TEXT_HTML,                   // text/html
    MT_TEXT_CSS,                    // text/css
    MT_TEXT_JAVASCRIPT,             // text/javascript
    MT_TEXT_MARKDOWN,               // text/markdown
    MT_TEXT_XML,                    // text/xml
    MT_IMAGE_ALL,                   // image/*
    MT_IMAGE_GIF,                   // image/gif 
    MT_IMAGE_JPEG,                  // image/jpeg
    MT_IMAGE_PNG,                   // image/png
    MT_IMAGE_TIFF,                  // image/tiff
    MT_APPLICATION_ALL,             // application/*
    MT_APPLICATION_JSON,            // application/json
    MT_APPLICATION_JAVASCRIPT       // application/javascript
};

struct mediaTypesNode {
    enum MediaType mediaType;
    struct mediaTypesNode * next;
};

int transformationManagerInit();

int hasTransformation();

char * getTransformation();

/* cmd has to be null terminated */
int addTransformation(const char * cmd);

void removeTransformation();

/* Returns a copy, it must be free */
struct mediaTypesNode * getMediaTypesList();

void freeMediaTypeList(struct mediaTypesNode * list);

int addMediaType(enum MediaType mediaType);

int removeMediaType(enum MediaType mediaType);

void transformationManagerDestroy();



enum MediaType strToMediaType(const char * str);

int hasMediaTypeInList(const struct mediaTypesNode * list, const enum MediaType mediaType);

#endif
