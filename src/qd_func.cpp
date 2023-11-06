#include "qd_func.h"

_QD_BEGIN


FunHead::FunHead(){
}

FunHead::~FunHead(){
        //析构所有子函数的堆内存空间
    for (auto iter = lfuns.begin() ; iter != lfuns.end() ; iter++)
    {
        if (*iter != nullptr) {
            delete *iter;
            *iter = nullptr;
        }
    }
}

FunState::FunState() : code_pos(0){
    proto = nullptr;
    prev = nullptr;

    init();
}

FunState::~FunState(){
    if (proto != nullptr){
        delete proto;
    }
    
}

void FunState::init(){
    proto = new FunHead();
}



_QD_END