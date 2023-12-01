#include "qd_buffer.h"

_QD_BEGIN


Dbuffer::Dbuffer(){
    init();
}

void Dbuffer::init(){
    this->n = 0;
    this->buffsize = 0;
    this->buffer = nullptr;
}

Dbuffer::~Dbuffer(){
    this->free_buff();
    this->n = 0;
    this->buffsize = 0;
}

void Dbuffer::reset_buff(){
    this->n = 0;
}

unsigned int Dbuffer::size_buff(){
    return this->buffsize;
}

void Dbuffer::move_buff(unsigned int n){
    this->n = n;
}

void Dbuffer::offset_buff(int n){
    this->n += n;
    if (this->n < 0 ){
        this->n = 0;
    }else if(this->n > this->buffsize){
        //'\0'字符
        this->n = this->buffsize;
    }
}

char Dbuffer::get_ch(){
    return *(this->buffer + n);
}

void Dbuffer::alloc_buff(const char* ch){
    this->alloc_buff(ch,strlen(ch));
}

void Dbuffer::alloc_buff(const char* ch,unsigned int n){
    free_buff();
    this->buffer = (char*)malloc(n + 1);
    this->buffsize = n;
    this->n = 0;
    strcpy(this->buffer,ch);
}

void Dbuffer::free_buff(){
    if (this->buffer != nullptr){
        free(this->buffer);
        this->buffer = nullptr;
    }
}

_QD_END