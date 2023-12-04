#include "qd_func.h"

_QD_BEGIN


FunHead::FunHead(){
}

FunHead::FunHead(const FunHead& head){
    //这个变量做一下更改，如果为VE_NULL 不进行拷贝

    this->lv = head.lv;

    for (unsigned int iter = 0 ; iter < head.lfuns.size() ; iter++) {
        FunState* funstate = nullptr;
        funstate = new FunState(*head.lfuns[iter]);
        this->lfuns.push_back(funstate);
    }
}

FunHead::~FunHead(){
    if ( !lfuns.empty()) {
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

}

FunState::FunState(){
    init();
    proto = new FunHead();
    // std::cout << "+++++++++++++++++++++++++++++" << std::endl;
}

FunState::FunState(const FunState& func){
    init();
    prev = func.prev;
    this->anonymous = func.anonymous;
    this->code_pos = func.code_pos;

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
    this->anonymous = false;
}



_QD_END