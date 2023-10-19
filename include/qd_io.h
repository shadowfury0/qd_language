#ifndef __QD_IO_H__
#define __QD_IO_H__

#include "qd_header.h"
/*
    数据流类，输入输出数据的流总类
*/

_QD_BEGIN

struct Dio {
    char* str;		//当前流数据
//   lua_Reader reader;	/* reader function */
    // void *data;			//额外数据，暂时没用
//   lua_State *L;			/* Lua state (for reader) */
    unsigned int buffsize;		//未读数据位置
    unsigned int off;

    Dio();
    ~Dio();

    void init();
    void alloc_str(const char* ch);
    void alloc_str(const char* ch,unsigned int b);
    void free_str();

    void reset_off();
    //移动到某个位置
    void move_off(unsigned int n);
    //偏移
    void offset_off(int n);
    const char get_ch();
};


_QD_END

#endif