//
// Created by Florencia Cavallin on 31/5/18.
//


#include <printf.h>

int parseInt(const char * b);

int main(void){

    printf("%i\n",parseInt("     34"));
    printf("%i\n",parseInt("     345678"));
    printf("%i\n",parseInt("     4"));
    printf("%i\n",parseInt("     34   "));
    printf("%i\n",parseInt("  ;   34"));
    printf("%i\n",parseInt("  ;  calita 34"));
    printf("%i\n",parseInt("4"));
    printf("%i\n",parseInt("florci"));


}

int parseInt(const char * b){
    int num = -1;
    int i = 0;

    if(b == NULL){
        return -1;
    }

    while(b[i] != 0){
        if(isdigit(b[i])){
            if(num == -1){
                num = 0;
            }
            num = num*10 + b[i]-'0';
        }else if(b[i] != ' '){
            return -1;
        }
        i++;
    }

    return num;

}
