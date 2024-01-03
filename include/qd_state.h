#ifndef _QD_STATE_H_
#define _QD_STATE_H_

#include "qd_obj.h"

_QD_BEGIN

/*
    负责语言内部函数调用传参
*/

struct Lib_State
{
    Lib_State();
    ~Lib_State();
    
    /* 内置函数参数变量 */
    std::vector<D_VAR> vars;
};

/*
    定义内部函数调用指针
*/
typedef size_t (*qd_inner_fun) (Lib_State *L);


_QD_END


#endif
