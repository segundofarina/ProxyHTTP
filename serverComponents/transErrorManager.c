#include <string.h>
#include "transErrorManager.h"

char transformationErr[TRANS_ERR_MAX_LEN + 1] = {0};

int setTransformationErrorFile(const char * str) {
    int errSize = strlen(str);
    if(errSize > TRANS_ERR_MAX_LEN) {
        return 0;
    }
    memcpy(transformationErr, str, errSize);
    transformationErr[errSize] = 0;
    return 1;
}

char * getTransformationErrorFile() {
    if(transformationErr[0] == 0) {
        int errSize = strlen("/dev/null");
        memcpy(transformationErr, "/dev/null", errSize);
        transformationErr[errSize] = 0;
    }

    return transformationErr;
}