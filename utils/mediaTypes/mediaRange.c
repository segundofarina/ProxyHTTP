//
// Created by Florencia Cavallin on 27/5/18.
//


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <memory.h>
#include <ctype.h>


bool shouldTransform(char const * * mediaRange, int size , char const * mediaType);

bool isContained(char const *mt1, char const *mt2);

//int main(int argc, char * argv[]) {
//    char ** mediaRange = (char *[]){"lala/*"};
//    char * mediaType = "audio/aac";
//    printf("%s",shouldTransform(mediaRange,1,mediaType)?"True":"False");
//}



bool isContained(char const *mt1, char const *mt2) {
    if(strchr (mt2,'/') == NULL){
        return false;
    }
    if(strcmp(mt1,"*/*") == 0){
        return true;
    }
    int i = strchr (mt1,'/')-mt1;
    if(strncmp(mt1,mt2,i) == 0){
        if(mt1[i+1] == '*' || strcmp(mt1+i+1,mt2+i+1) == 0){
            return true;
        }
        return false;
    }
    return false;
}

bool shouldTransform(char const * * mediaRange, int size , char const * mediaType){
    for(int i = 0; i< size ; i ++){
        if(isContained(mediaRange[i],mediaType)){
            return true;
        }
    }
    return false;
}


