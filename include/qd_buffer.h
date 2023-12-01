#ifndef __QD_BUFFER_H__
#define __QD_BUFFER_H__

#include "qd_header.h"

_QD_BEGIN

//16
struct Dbuffer {
    char *buffer;
    unsigned int n;//位置
    unsigned int buffsize;//大小

    Dbuffer();
    ~Dbuffer();

    void init();
    unsigned int size_buff();

    void alloc_buff(const char* ch);
    void alloc_buff(const char* ch,unsigned int n);
    void free_buff();

    void reset_buff();
    //移动到某个位置
    void move_buff(unsigned int n);
    //偏移
    void offset_buff(int n);
    char get_ch();
};

_QD_END

#endif