#ifndef _TRANSFORMATION_MANAGER_H_
#define _TRANSFORMATION_MANAGER_H_

enum MediaType {
    MT_NONE=0xFF,                        // default value for error
    MT_TEXT_ALL=0x00,                    // text/*
    MT_TEXT_PLAIN=0x01,                  // text/plain
    MT_TEXT_HTML=0x02,                   // text/html
    MT_TEXT_CSS=0x03,                    // text/css
    MT_TEXT_JAVASCRIPT=0x04,             // text/javascript
    MT_TEXT_MARKDOWN=0x05,               // text/markdown
    MT_TEXT_XML=0x06,                    // text/xml
    MT_IMAGE_ALL=0x07,                   // image/*
    MT_IMAGE_GIF=0x08,                   // image/gif 
    MT_IMAGE_JPEG=0x09,                  // image/jpeg
    MT_IMAGE_PNG=0x0A,                   // image/png
    MT_IMAGE_TIFF=0x0B,                  // image/tiff
    MT_APPLICATION_ALL=0x0C,             // application/*
    MT_APPLICATION_JSON=0x0D,            // application/json
    MT_APPLICATION_JAVASCRIPT=0x0E       // application/javascript
};

struct mediaTypesNode {
    enum MediaType mediaType;
    struct mediaTypesNode * next;
};

int transformationManagerInit();

int hasTransformation();

char * getCurrentTransformation();

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
