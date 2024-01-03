#ifndef _QD_LIB_H_
#define _QD_LIB_H_

#include "qd_header.h"
#include "qd_state.h"

_QD_BEGIN


/*
    总模块，负责查询各模块的调用总信息
*/

struct D_LIB
{
    D_LIB();
    ~D_LIB();

    //库初始化
    virtual void load_lib(){};

    //模块名
    std::string name;
    //内部函数
    std::map<std::string,qd_inner_fun> funs;
};


_QD_END


#endif

