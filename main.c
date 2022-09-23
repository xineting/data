#include "hello.h"

int main(){
    int (*pAddFuncVar)(int, int);
    pAddFuncVar = int_add;
    functioninhello4(pAddFuncVar);
    return 1;
}
