#ifndef _TRANSFORMATION_MANAGER_H_
#define _TRANSFORMATION_MANAGER_H_

#include "../utils/mediaTypes/mediatypes.h"

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

int hasMediaTypeInList(const struct mediaTypesNode * list, const enum MediaType mediaType);

#endif
