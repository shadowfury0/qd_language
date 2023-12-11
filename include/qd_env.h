#ifndef __QD_ENVIRONMENT_H__
#define __QD_ENVIRONMENT_H__

#include "qd_header.h"
#include "qd_func.h"

_QD_BEGIN


struct D_ENV
{
    bool anonymous;
    unsigned int code_pos;
    D_ENV* prev;                       //上一级环境  
    FunHead* cur;                      //当前函数头信息

    std::map<std::string,D_VAR> lv;       //局部变量  debug 检测
    // std::map<std::string,D_OBJ> lv;       //局部变量  debug 检测

    D_ENV();
    D_ENV(const D_ENV& func);
    ~D_ENV();

    void init();
};


_QD_END


#endif

