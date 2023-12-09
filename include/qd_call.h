#ifndef __QD_STATE_H__
#define __QD_STATE_H__

#include "qd_header.h"
#include "qd_assign.h"
#include "qd_func.h"

_QD_BEGIN



struct CallInfo 
{
    using Stack_V = D_VAR;

    CallInfo();
    CallInfo(const CallInfo& ca);
    ~CallInfo();

    void init();

    //改变变量的值
    Stack_V& v(const std::string name);

    //匿名 if for else while 
    bool anonymous;
    //上一个条件判断是否为true，例如if elif ，如果为true则不执行
    bool ifstate;
    unsigned int pos;
    FunHead* f;            //当前函数
    // CallInfo* prev;        //上一级变量
    //闭包上级函数
    // std::map<std::string,Stack_V>*  closure;
    std::map<std::string,Stack_V>   sv;
};


struct CallStack
{
    CallStack();
    CallStack(const CallStack& cal);
    ~CallStack();

    void init();
    CallInfo* top();
    
    std::vector<CallInfo*>  cs;   //运行栈
};


_QD_END


#endif