#include "qd_io.h"

_QD_BEGIN
Dio::Dio(){
    init();
}

Dio::~Dio(){
    free_str();
}

void Dio::init(){
    this->buffsize = 0;
    this->off = 0;
    this->str = nullptr;
}

void Dio::alloc_str(const char* ch){
    this->alloc_str(ch,strlen(ch));
}

void Dio::alloc_str(const char* ch,unsigned int n){
    free_str();
    str = (char*)malloc(n+1);
    this->buffsize = n;
    this->off = 0;
    strcpy(this->str,ch);
}

void Dio::free_str(){
    if (this->str != nullptr){
        free(this->str);
        this->str = nullptr;
    }
}

void Dio::reset_off(){
    this->off = 0;
}

void Dio::move_off(unsigned int n){
    this->off = n;
}

void Dio::offset_off(int n){
    this->off += n;
    if (this->off < 0 ){
        this->off = 0;
    }else if(this->off > this->buffsize){
        //'\0'字符
        this->off = this->buffsize;
    }
}

const char Dio::get_ch(){
    return this->str[this->off];
}


_QD_END