#include "qd_func.h"

_QD_BEGIN


FunHead::FunHead(){
    this->stacksize = 0;
    this->max_line = 0;
}

FunHead::~FunHead(){

}

FunState::FunState(){
    proto = NULL;
    prev = NULL;
    this->lastline = 0;
    this->curline = 0;
    this->rows = 0;

    init();
}

FunState::~FunState(){
    if (proto != NULL){
        delete proto;
    }

    if (prev != NULL){
        delete prev;
    }
}

void FunState::init(){
    proto = new FunHead();
}



_QD_END