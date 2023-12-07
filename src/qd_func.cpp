#include "qd_func.h"

_QD_BEGIN


FunHead::FunHead(){
    // this->anonymous = false;
}

FunHead::FunHead(const FunHead& head){
    // this->anonymous = head.anonymous;
    //这个变量做一下更改，如果为VE_NULL 不进行拷贝
    this->codes = head.codes;

    for (unsigned int iter = 0 ; iter < head.lfuns.size() ; iter++) {
        FunHead* funstate = nullptr;
        funstate = new FunHead(*head.lfuns[iter]);
        this->lfuns.push_back(funstate);
    }
}

FunHead::~FunHead(){
    //可能会重复释放
    // if ( !lfuns.empty()) {
    //     //析构所有子函数的堆内存空间
    //     for ( std::vector<FunHead *>::iterator
    //     iter = lfuns.begin() ; iter != lfuns.end() ; iter++)
    //     {
    //         if (*iter != nullptr) {
    //             delete *iter;
    //             *iter = nullptr;
    //         }
    //     }
    // }
}

void FunHead::clear() {
    if ( !lfuns.empty()) {
        //析构所有子函数的堆内存空间
        for ( std::vector<FunHead *>::iterator
        iter = lfuns.begin() ; iter != lfuns.end() ; iter++)
        {
            if (*iter != nullptr) {
                (*iter)->clear();
                delete *iter;
                *iter = nullptr;
            }
        }
    }
}


_QD_END