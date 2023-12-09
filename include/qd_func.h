#ifndef __QD_FUNC_H__
#define __QD_FUNC_H__

#include "qd_lex.h"
#include "qd_instruction.h"

_QD_BEGIN


//函数头  80 b 字节已经对齐
struct FunHead {
    //if elif for while
    // bool  anonymous;                  //是否为匿名函数
    // std::string name;
    // unsigned int maxstacksize;            // 变量参数个数
    
    std::vector<std::string> args;
    //匿名内部函数，如if  for
    std::vector<FunHead*>  lfuns;
    std::vector<Instruction>  codes;      //指令集

    FunHead();
    FunHead(const FunHead& head);
    ~FunHead();

    void clear();
    //可能潜在参数小，不要紧
    unsigned int args_size();
};

//函数整体部分



_QD_END


#endif