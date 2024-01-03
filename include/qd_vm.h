#ifndef __QD_VM_H__
#define __QD_VM_H__

#include "qd_log.h"
#include "qd_call.h"
#include "qd_state.h"
#include "qd_lib.h"

_QD_BEGIN


struct D_VM
{
    D_VM();
    D_VM(const D_VM& vm) ;
    ~D_VM();

    void init();

    void push_call(CallInfo* in);
    void pop_call();
    //保留全局栈信息
    void reserve_global();

    _qd_uint size_call();

    size_t init_fun(FunHead* fun);
    size_t init_state(Lib_State* l);
    size_t init_lib(std::vector<D_LIB*>* l);

    CallInfo* cur_fun();
    CallInfo* head_fun();
    
    size_t execute();
    //从某个位置开始执行
    size_t execute(size_t i);

    //目前没有错误
    void default_assign(const std::string& name,const D_VAR& var,CallInfo* const info);
    void default_assign(const std::string& name,const D_OBJ& var,CallInfo* const info);
    void local_assign(const std::string& name,const D_VAR& var,CallInfo* const info);
    void local_assign(const std::string& name,const D_OBJ& var,CallInfo* const info);
    void global_assign(const std::string& name,const D_VAR& var);
    void global_assign(const std::string& name,const D_OBJ& var);
    
    //解析指令集
    size_t analyse_code(size_t& i,CallInfo* info);
    //解析表达式指令集
    size_t analyse_expr(Instruction& inc,CallInfo* fun);
    //赋值解析
    size_t analyse_assign(Instruction& inc,CallInfo* fun);
    //数组下标赋值
    size_t analyse_array_index_assign(Instruction& inc,FunHead& fun);


    //第一个call是当前指令所在位置，第二个是变量输入函数位置,函数参数输入,性能会有点差
    size_t input_args(const Instruction& inc,CallInfo* cur,CallInfo* push);
    //查找函数
    FunHead* find_function(const std::string& name);
    //上一级，非匿名函数位置
    CallInfo* last_function(CallInfo* info);
    //上一级变量位置，(local位置查找)
    CallInfo* last_var(CallInfo* info);
    
    D_OBJ* find_variable(const std::string& name);
    //打印所有变量
    void print_variables(const CallInfo* call);

    Logger* logger;
    Lib_State* state;

    CallStack* st;
    
    //全局栈信息，目的是为了交互模式保留上下文
    CallInfo* global;

    std::vector<D_LIB*>* lib;
};


_QD_END


#endif
