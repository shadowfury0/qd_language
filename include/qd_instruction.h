#ifndef __QD_INSTRUCTION_H_
#define __QD_INSTRUCTION_H_

#include "qd_header.h"
#include "qd_assign.h"

_QD_BEGIN

//优先级
static const int priority[][2] = {
    {1,1},      // =
    {10,10},    // +
    {10,10},    // -
    {11,11},    // *
    {12,11},    // %
    {12,11},    // /
    // {12,11},    // //
};

//前缀运算符
enum PreCode{
    OC_NIL,               //无
    OC_MINUS,             //-
};

//与上面优先级对齐
enum OprCode {
    OC_ASSIGN,            //赋值
    /* arithmetic   operators */
    OC_ADD,               //加
    OC_SUB,               //减
    OC_MUL,               //乘
    OC_MOD,               //模
    // OC_POW,               //幂
    OC_DIV,               //除  double
    // OC_IDIV,              //整除
    // /* bitwise operators */
    // OC_BAND,              //与
    // OC_BOR,               //或
    // OC_BXOR,              //非
    /* string operator */
    OC_CONCAT,            //拼接
    /* arrays operators */
    OC_ARR_VAL,            //数组值
    OC_ARR_ASSIGN,         //数组赋值
    OC_ARR_LASSIGN,        //数组列表赋值
    OC_ARR_PASSIGN,        //数组部分赋值
    OC_ARR_ACESS,          //数组访问
    OC_ARR_LIST,           //数组区间访问
    /* logical operators */
    OC_GT,                // >
    OC_GE,                // >=
    OC_LT,                // <
    OC_LE,                // <=
    OC_DEQ,               // ==
    OC_NEQ,               // !=
    OC_AND,               //并 &
    OC_OR,                //或 |
//-------------------------------------------
    /* function  operator */
    OC_IF,                // IF
    OC_FOR,               // FOR 
    OC_JMP,               //跳转指令
    OC_CALL,              //调用函数
    OC_RET,               //返回
    // null
    OC_NULL,              //无
};

//操作码
struct Instruction{
    unsigned char pretype;       //前缀类型 左值是否为负
    unsigned char sufftype;      //后缀类型，右值是否为负
    short type ;         //类型
    short ltype;         //上一个参数类型
    //指令行数
    unsigned int curpos;
    unsigned int lpos; //左子树 指令偏移位置
    unsigned int rpos; //右子树 指令偏移位置

    D_VAR res;          //结果返回值
    D_VAR left;         //指令操作完成 可以当作返回值赋值
    D_VAR right;

    Instruction();
    Instruction(const Instruction& i);
    ~Instruction();

    void operator=(const Instruction& i);
};


_QD_END


#endif