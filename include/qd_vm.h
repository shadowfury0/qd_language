#ifndef __QD_VM_H__
#define __QD_VM_H__

#include "qd_log.h"
#include "qd_call.h"

_QD_BEGIN


struct D_VM
{
    D_VM();
    D_VM(const D_VM& vm) ;
    ~D_VM();

    void init();

    void push_call(CallInfo* in);
    void pop_call();
    _qd_uint size_call();
    
    CallInfo* cur_fun();
    CallInfo* head_fun();
    
    unsigned int execute();
    
    //解析指令集
    unsigned int analyse_code(unsigned int& i,CallInfo* info);
    //解析表达式指令集
    unsigned int analyse_expr(Instruction& inc,CallInfo* fun);

    FunHead* find_function(const std::string& name,CallInfo* info);
    //上一级返回
    CallInfo* last_return(CallInfo* info);
    //上一级函数
    CallInfo* last_function(CallInfo* info);
    D_VAR* find_variable(const std::string& name,CallInfo* info);
    //打印所有变量
    void print_variables(const CallInfo& call);

    Logger* logger;

    CallStack* st;
    //原始函数，负责让其他的栈进行拷贝运行
    FunHead* fun;
};


_QD_END


#endif
