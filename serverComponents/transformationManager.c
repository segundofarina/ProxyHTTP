#include <stdlib.h>

#include "transformationManager.h"

struct Transformation {
    char cmd[256];
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
    freeMediaTypeList(transformation->firstNode);
    free(transformation);
}

