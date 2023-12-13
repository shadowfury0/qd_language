#include "qd_io.h"

_QD_BEGIN


Dio::Dio() {
    this->index = 0;
    // std::cout << "__________________\n";
}

Dio::~Dio(){
    this->index = 0;

    this->clear();
    // std::cout << "++++++++++++++\n";
}

void Dio::offset_buff(int n){
    unsigned int len = this->cur()->size;

    if ( n < len || n > 0 ) {
        this->cur()->offset_buff(n);
    }
    
}

char Dio::get_ch(){
    //如果等于最后一个缓冲区长度，index加一
    // std::cout << this->cur()->pos << " : " << this->cur()->size << std::endl;
    if ( this->cur()->pos >= this->cur()->size ) {
        this->index ++;
    }

    //超出范围则为0
    if (this->index < this->buffs.size() ) {
        DBuffer* buf = this->cur();

        if ( !buf ) {
            return 0;
        }
        return buf->get_ch();
    }
    return 0;
}

// void Dio::alloc_buff(const char* ch){
//     this->alloc_buff(ch,strlen(ch));
// }

void Dio::alloc_buff(const char* ch,unsigned int n){

    DBuffer* b = nullptr;
    b = new DBuffer();
    b->alloc_buffer(ch,n);

    this->buffs.push_back(b);
}

void Dio::clear(){

    for (std::vector<DBuffer *>::iterator i = this->buffs.begin();
        i != this->buffs.end() ; i ++) {
        delete *i;
        *i = nullptr;
    }
    this->buffs.clear();
}

DBuffer* Dio::cur() {
    if ( index > this->buffs.size() - 1 && index < 0) {
        return nullptr;
    }
    return this->buffs[index];
}


_QD_END
