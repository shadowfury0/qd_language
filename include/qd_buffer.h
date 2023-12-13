#ifndef __QD_D_BUFFER_H__
#define __QD_D_BUFFER_H__

#include "qd_header.h"

_QD_BEGIN

/*
    一次读取的块大小
*/

struct DBuffer
{
    // unsigned int pos;   /* bytes still unread */
    unsigned int pos;
    unsigned int size;
    char* buffer;

    DBuffer();
    ~DBuffer();

    void alloc_buffer(const char* buf,unsigned int size);
    char get_ch();
    void offset_buff(int n);
};





_QD_END


#endif


