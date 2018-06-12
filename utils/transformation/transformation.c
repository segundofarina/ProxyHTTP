//
// Created by Florencia Cavallin on 9/6/18.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "transformation.h"

char hexadecimal(int i) {
    char ans = 0;
    if(i>=0 && i<10){
        ans = i + '0';
    }else if(i>=10 && i<16){
        ans = i - 10 + 'A';
    }
    return ans;
}

int cantBytes(double num){
    if(num == 0){
        return 1;
    }
    int ans = 0;
    while(num>=1){
        num = num/16;
        ans++;
    }
    return ans;
}

void toHexa(char * buffer,int num, int bytes){
    int i = bytes-1;
    while(i>=0){
        buffer[i] = hexadecimal(num%16);
        num = num/16;
        i--;
    }
    buffer[bytes] = 0;
}

void chunkBody(char * buffer, int * bLen, char * ans, int * aLen){

    int i = *bLen;
    int chunksize = cantBytes(i);

    while( (4 + i + chunksize) > *aLen && i >= 0){
        i--;
        chunksize = cantBytes(i);
    }

    if(i < 0){
        (*bLen) = 0;
        (*aLen) = 0;
        return;
    }

    toHexa(ans,i,chunksize);
    int h = strlen(ans);
    ans[h++] = '\r';
    ans[h++] = '\n';

    for(int j = 0; j < i ; j ++ ){
        ans[h++] = buffer[j];
    }

    ans[h++] = '\r';
    ans[h++] = '\n';

    (*bLen) = i;
    (*aLen) = h;

}
