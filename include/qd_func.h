#ifndef __QD_FUNC_H__
#define __QD_FUNC_H__

#include "qd_lex.h"
#include "qd_instruction.h"

_QD_BEGIN

class FunState;

//函数头  80 b 字节已经对齐
struct FunHead{
    unsigned int start;                  //起始指令行
    unsigned int end;                    //结束指令行

    //匿名内部函数，如if  for
    std::vector<FunState*>  lfuns;        
    std::map<std::string,D_OBJ> lv;       //局部变量

    FunHead();
    FunHead(const FunHead& head);
    ~FunHead();
};

//函数整体部分
struct FunState
{
    //if elif for while
    bool  anonymous;                    //是否为匿名函数
    unsigned int code_pos;              //解析当前行

    FunHead* proto;                     //当前函数头信息
    FunState* prev;                     //上一级函数  

    //这个词法解析完成后就可以添加操作码了
    std::vector<Instruction>  codes;    //指令集

    FunState();
    FunState(const FunState& func);
    ~FunState();

    void init();
};


_QD_END


#endif