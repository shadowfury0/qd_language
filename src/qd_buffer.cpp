#include "qd_buffer.h"

_QD_BEGIN

DBuffer::DBuffer() {
    this->buffer = nullptr;
    this->size = 0;
    this->pos = 0;
}

DBuffer::~DBuffer() {
    if (this->buffer != nullptr) {
        free(this->buffer);
        this->buffer = nullptr;
    }
    this->size = 0;
    this->pos = 0;
}

void DBuffer::alloc_buffer(const char* buf,unsigned int size) {
    this->buffer = (char*)malloc( size + 1 );
    this->size = size;
    strcpy(this->buffer,buf);
}

char DBuffer::get_ch() {
    return this->buffer[pos];
}

void DBuffer::offset_buff(int n) {
    this->pos += n;
    if (this->pos < 0 ){
        this->pos = 0;
    }else if(this->pos > this->size){
        //'\0'字符
        this->pos = this->size;
    }
}

_QD_END
