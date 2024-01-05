#ifndef _QD_STATE_H_
#define _QD_STATE_H_

#include "qd_obj.h"

_QD_BEGIN

/*
    负责语言内部函数调用传参
*/

struct D_State
{
    D_State();
    D_State(const D_State& s);
    ~D_State();
    
    // 当前函数调用参数个数
    size_t v_pos;
    // 内置函数参数变量 
    std::list<D_OBJ> vars;
    // 内置函数返回值
    std::list<D_OBJ> rets;
};

/*
    定义内部函数调用指针
*/
typedef size_t (*qd_inner_fun) (D_State *L);


_QD_END


#endif
