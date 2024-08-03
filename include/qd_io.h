#ifndef __QD_DIO_H__
#define __QD_DIO_H__

#include "qd_buffer.h"

_QD_BEGIN


struct Dio {

    Dio();
    ~Dio();

    // void alloc_buff(const char* ch);
    void alloc_buff(const char* ch,size_t n);
    void clear();
    void clean_back();

    //偏移
    void offset_buff(int n);
    char get_ch();

    //头缓冲
    DBuffer* cur();
    
    size_t index; //当前流位置

    std::vector<DBuffer*> buffs;
};


_QD_END


#endif