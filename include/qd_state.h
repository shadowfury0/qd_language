#ifndef __QD_D_STATE_H__
#define __QD_D_STATE_H__

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

// rets 返回值传入空值
#define D_STA_PUS_NUL \
D_OBJ obj;\
obj.type = VE_NULL;\
l->rets.push_back(obj);

//清空,vars的len个变量
#define D_STA_CLEAN_(len)\
while (len--)\
{l->vars.pop_front();}


_QD_END


#endif
