#include <stdlib.h>
#include <string.h>

#include "transformationManager.h"

#define MAX_CMD 255

struct Transformation {
    char cmd[MAX_CMD + 1];
    struct mediaTypesNode * firstNode;
};

static struct Transformation * transformation = NULL;

int transformationManagerInit() {
    transformation = malloc(sizeof(*transformation));
    if(transformation == NULL) {
        return 0;
    }

    transformation->cmd[0] = 0;
    transformation->firstNode = NULL;

    return 1;
}

int hasTransformation() {
    return transformation->cmd[0] != 0;
}

char * getTransformation() {
    return transformation->cmd;
}

int addTransformation(const char * cmd) {
    int n = strlen(cmd);
    if(n > MAX_CMD) {
        return 0;
    }

    memcpy(transformation->cmd, cmd, n+1);

    return 1;
}

void removeTransformation() {
    transformation->cmd[0] = 0;
}

static struct mediaTypesNode * copyList(const struct mediaTypesNode * node) {
    struct mediaTypesNode * newNode;
    
    if(node == NULL) {
        return NULL;
    }

    newNode = malloc(sizeof(*newNode));
    
    if(newNode == NULL) {
        return NULL; //should handle better
    }

    newNode->mediaType = node->mediaType;
    newNode->next = copyList(node->next);
    return newNode;
}

/* Returns a copy, it must be free */
struct mediaTypesNode * getMediaTypesList() {
    return copyList(transformation->firstNode);
}

void freeMediaTypeList(struct mediaTypesNode * list) {
    struct mediaTypesNode * current = list;
    struct mediaTypesNode * next = NULL;
    while(current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
}

int addMediaType(enum MediaType mediaType) {
    if(hasMediaTypeInList(transformation->firstNode, mediaType)) {
        return 1;
    }

    struct mediaTypesNode * node = malloc(sizeof(*node));
    if(node == NULL) {
        return 0;
    }

    node->mediaType = mediaType;
    node->next = transformation->firstNode;
    transformation->firstNode = node;

    return 1;
}

int removeMediaType(enum MediaType mediaType) {
    struct mediaTypesNode * current = transformation->firstNode;
    struct mediaTypesNode * prev = NULL;

    while(current != NULL) {
        if(current->mediaType == mediaType) {

            if(prev == NULL) {
                transformation->firstNode = current->next;
            } else {
                prev->next = current->next;
            }

            free(current);
            
            return 1;
        }
        prev = current;
        current = current->next;
    }

    return 0;
}

void transformationManagerDestroy() {
    if(transformation == NULL) {
        return;
    }
    freeMediaTypeList(transformation->firstNode);
    free(transformation);
}


int hasMediaTypeInList(const struct mediaTypesNode * list, const enum MediaType mediaType) {
    const struct mediaTypesNode * current = list;
    while(current != NULL) {
        if(current->mediaType == mediaType) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

enum MediaType strToMediaType(const char * str) {
    if(strcmp("text/plain", str) == 0) {
        return MT_TEXT_PLAIN;
    }

    if(strcmp("text/html", str) == 0) {
        return MT_TEXT_HTML;
    }

    if(strcmp("text/css", str) == 0) {
        return MT_TEXT_CSS;
    }

    if(strcmp("text/javascript", str) == 0) {
        return MT_TEXT_JAVASCRIPT;
    }

    if(strcmp("text/markdown", str) == 0) {
        return MT_TEXT_MARKDOWN;
    }

    if(strcmp("text/xml", str) == 0) {
        return MT_TEXT_XML;
    }

    if(strcmp("image/*", str) == 0) {
        return MT_IMAGE_ALL;
    }

    if(strcmp("image/gif", str) == 0) {
        return MT_IMAGE_GIF;
    }

    if(strcmp("image/jpeg", str) == 0) {
        return MT_IMAGE_JPEG;
    }

    if(strcmp("image/png", str) == 0) {
        return MT_IMAGE_PNG;
    }

    if(strcmp("image/tiff", str) == 0) {
        return MT_IMAGE_TIFF;
    }

    if(strcmp("application/*", str) == 0) {
        return MT_APPLICATION_ALL;
    }

    if(strcmp("application/json", str) == 0) {
        return MT_APPLICATION_JSON;
    }

    if(strcmp("application/javascript", str) == 0) {
        return MT_APPLICATION_JAVASCRIPT;
    }

    return MT_NONE;
}
