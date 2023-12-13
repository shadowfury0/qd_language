#ifndef __QD_DIO_H__
#define __QD_DIO_H__

#include "qd_buffer.h"

_QD_BEGIN


struct Dio {

    // unsigned int n; //位置
    unsigned int index; //当前流位置
    // unsigned int buffsize;  //当前缓冲大小

    std::vector<DBuffer*> buffs;

    Dio();
    ~Dio();

    // void alloc_buff(const char* ch);
    void alloc_buff(const char* ch,unsigned int n);
    void clear();

    //偏移
    void offset_buff(int n);
    char get_ch();

    //头缓冲
    DBuffer* cur();
};


_QD_END


#endif