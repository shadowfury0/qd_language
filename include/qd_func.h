#ifndef __QD_FUNC_H__
#define __QD_FUNC_H__

#include "qd_lex.h"
#include "qd_instruction.h"

_QD_BEGIN

//函数头
struct FunHead{
    unsigned char stacksize;             //栈参数大小
    unsigned int max_line;              //总行数

    std::vector<FunHead*>  lfuns;        //函数内函数
    std::map<std::string,D_VAR> lv;            //局部变量

    FunHead();
    ~FunHead();
};

//函数整体部分
struct FunState
{
    unsigned int lastline;              //之前执行行
    unsigned int curline;               //当前执行行
    unsigned int rows;                  //函数总行数
    FunHead* proto;                     //当前函数头信息
    FunState* prev;                     //上一级函数  

    std::vector<D_VAR> rets;              //返回集合
    //这个词法解析完成后就可以添加操作码了
    std::deque<Instruction>  codes;    //指令集

    FunState();
    ~FunState();

    void init();
};


_QD_END

#endif