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

    unsigned int init_fun(FunHead* fun);
    CallInfo* cur_fun();
    CallInfo* head_fun();
    
    unsigned int execute();
    //从某个位置开始执行
    unsigned int execute(unsigned int i);

    //目前没有错误
    void default_assign(const std::string& name,const D_VAR& var,CallInfo* const info);
    void default_assign(const std::string& name,const D_OBJ& var,CallInfo* const info);
    void local_assign(const std::string& name,const D_VAR& var,CallInfo* const info);
    void local_assign(const std::string& name,const D_OBJ& var,CallInfo* const info);
    void global_assign(const std::string& name,const D_VAR& var);
    void global_assign(const std::string& name,const D_OBJ& var);
    
    //解析指令集
    unsigned int analyse_code(unsigned int& i,CallInfo* info);
    //解析表达式指令集
    unsigned int analyse_expr(Instruction& inc,CallInfo* fun);
    //赋值解析
    unsigned int analyse_assign(Instruction& inc,CallInfo* fun);
    //数组下标赋值
    unsigned int analyse_array_index_assign(Instruction& inc,FunHead& fun);


    //第一个call是当前指令所在位置，第二个是变量输入函数位置,函数参数输入,性能会有点差
    unsigned int input_args(const Instruction& inc,CallInfo* cur,CallInfo* push);
    //查找函数
    FunHead* find_function(const std::string& name);
    //上一级返回
    CallInfo* last_return(CallInfo* info);
    //上一级函数
    CallInfo* last_function(CallInfo* info);
    D_OBJ* find_variable(const std::string& name);
    //打印所有变量
    void print_variables(const CallInfo* call);

    Logger* logger;

    CallStack* st;
    //原始函数，负责让其他的栈进行拷贝运行
    FunHead* fun;
};


_QD_END


#endif
