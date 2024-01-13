#ifndef __QD_CALL_INFO_H__
#define __QD_CALL_INFO_H__

#include "qd_header.h"
#include "qd_obj.h"
#include "qd_func.h"

_QD_BEGIN



struct CallInfo 
{

    CallInfo();
    //浅拷贝
    CallInfo(const CallInfo& ca);
    ~CallInfo();

    void init();

    //改变变量的值
    D_OBJ& v(const std::string name);

    //匿名 if for else while 
    bool anonymous;
    //上一个条件判断是否为true，例如if elif ，如果为true则不执行
    bool ifstate;
    size_t pos;
    FunHead* f;               //当前函数

    std::map<std::string,D_OBJ>   sv;
};


struct CallStack
{
    CallStack();
    CallStack(const CallStack& cal);
    ~CallStack();

    void init();
    void clear();
    CallInfo* top();
    
    std::vector<CallInfo*>  cs;   //运行栈
};


_QD_END


#endif