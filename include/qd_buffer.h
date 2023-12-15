#ifndef __QD_D_BUFFER_H__
#define __QD_D_BUFFER_H__

#include "qd_header.h"

_QD_BEGIN

/*
    一次读取的块大小
*/

struct DBuffer
{
    DBuffer();
    ~DBuffer();

    void alloc_buffer(const char* buf,size_t size);
    char get_ch();
    void offset_buff(int n);


    // size_t pos;   /* bytes still unread */
    size_t pos;
    size_t size;
    char* buffer;
};





_QD_END


#endif


