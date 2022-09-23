#include "hello2.h"

typedef struct structname2{
    int a;
    int b;
}typedefname2;

enum enumname{
    enum1,
    enum2
};

void functioninhello1(typedefname*,typedefname2 param2);

void functioninhello2(typedefname* param1,typedefname2 param2){
}

void functioninhello3(struct structname2 param1,enum enumname param2){

}

int int_add(int a,int b){
    return a+b;
}

int functioninhello4 (int (*MyOperator)(int, int)) {
    return MyOperator(2, 1); // 类似java的回调函数
}