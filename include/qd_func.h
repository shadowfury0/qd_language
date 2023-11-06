#ifndef __QD_FUNC_H__
#define __QD_FUNC_H__

#include "qd_lex.h"
#include "qd_instruction.h"

_QD_BEGIN

class FunState;
//函数头
struct FunHead{
    unsigned int start;                  //起始指令行
    unsigned int end;                    //结束指令行

    std::vector<FunState*>  lfuns;        //函数内函数
    std::map<std::string,D_VAR> lv;       //局部变量

    FunHead();
    ~FunHead();
};

//函数整体部分
struct FunState
{
    unsigned int code_pos;              //解析当前行

    FunHead* proto;                     //当前函数头信息
    FunState* prev;                     //上一级函数  

    // std::vector<D_VAR> rets;              //返回集合
    //这个词法解析完成后就可以添加操作码了
    std::vector<Instruction>  codes;    //指令集

    FunState();
    ~FunState();

    void init();
};


_QD_END


#endif