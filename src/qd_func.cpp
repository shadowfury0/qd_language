#include "qd_func.h"

_QD_BEGIN


FunHead::FunHead(){
}

FunHead::FunHead(const FunHead& head){
    this->lv = head.lv;

    for (unsigned int iter = 0 ; iter < head.lfuns.size() ; iter++) {
        FunState* funstate = nullptr;
        funstate = new FunState(*head.lfuns[iter]);
        this->lfuns.push_back(funstate);
    }
}

FunHead::~FunHead(){
    //析构所有子函数的堆内存空间
    for ( std::vector<FunState *>::iterator
    iter = lfuns.begin() ; iter != lfuns.end() ; iter++)
    {
        if (*iter != nullptr) {
            delete *iter;
            *iter = nullptr;
        }
    }

}

FunState::FunState(){
    init();
    proto = new FunHead();
    // std::cout << "+++++++++++++++++++++++++++++" << std::endl;
}

FunState::FunState(const FunState& func){
    init();
    prev = func.prev;
    this->codes = func.codes;
    
    proto = new FunHead(*func.proto);
    // std::cout << "+++++++++++++++++++++++++++++" << std::endl;

}

FunState::~FunState(){
    if (proto != nullptr){
        delete proto;
    }
    // std::cout << "____________________ " << std::endl;
}

void FunState::init(){
    proto = nullptr;
    prev = nullptr;
    this->code_pos = 0;
}



_QD_END