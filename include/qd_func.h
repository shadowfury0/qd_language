#ifndef __QD_FUNC_H__
#define __QD_FUNC_H__

#include "qd_lex.h"
#include "qd_instruction.h"

_QD_BEGIN


//函数头
struct FunHead {

    FunHead();
    FunHead(const FunHead& head);
    ~FunHead();

    void clear();
    //可能潜在参数小，不要紧
    size_t args_size();

    
    std::vector<std::string> args;
    //匿名内部函数，如if  for
    std::vector<FunHead*>  lfuns;
    std::vector<Instruction>  codes;      //指令集
};

//函数整体部分



_QD_END


#endif