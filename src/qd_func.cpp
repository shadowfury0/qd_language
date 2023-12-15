#include "qd_func.h"

_QD_BEGIN


FunHead::FunHead(){
    // this->anonymous = false;
    // this->maxstacksize = 0;
}

FunHead::FunHead(const FunHead& head){
    // this->anonymous = head.anonymous;
    //这个变量做一下更改，如果为VE_NULL 不进行拷贝
    // this->maxstacksize = head.maxstacksize;
    this->args = head.args;
    this->codes = head.codes;

    for (size_t iter = 0 ; iter < head.lfuns.size() ; iter++) {
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

size_t FunHead::args_size(){
    return args.size();
}


_QD_END