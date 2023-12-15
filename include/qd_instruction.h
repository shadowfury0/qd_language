#ifndef __QD_INSTRUCTION_H_
#define __QD_INSTRUCTION_H_

#include "qd_header.h"
#include "qd_obj.h"

_QD_BEGIN

//这个好像可以弃之了
//优先级
// static const int priority[][2] = {
//     {0,0},      //null
//     {1,1},      // =
//     {10,10},    // +
//     {10,10},    // -
//     {11,11},    // *
//     {11,10},    // %
//     {11,10},    // /
//     {9,9},      // >
//     {9,9},      // >=
//     {9,9},      // <
//     {9,9},      // <=
//     {9,9},      // ==
//     {9,9},      // !=
//     {8,8},      //并 &
//     {8,8},      //或 |
// };

static const int priority[] = {
    0,      //null
    1,      // =
    10,     // +
    10,     // -
    11,     // *
    11,     // %
    11,     // /
    9,      // >
    9,      // >=
    9,      // <
    9,      // <=
    9,      // ==
    9,      // !=
    8,      //并 &
    8,      //或 |
};

//前缀运算符
enum PreCode{
    OC_NIL,               //无
    OC_MINUS,             //-
};

//与上面优先级对齐
enum OprCode {
    // null
    OC_NULL,              //无

    OC_ASSIGN,            //赋值
    /* arithmetic   operators */
    OC_ADD,               //加
    OC_SUB,               //减
    OC_MUL,               //乘
    OC_MOD,               //模
    // OC_POW,               //幂
    OC_DIV,               //除  double
    /* logical operators */
    OC_GT,                // >
    OC_GE,                // >=
    OC_LT,                // <
    OC_LE,                // <=
    OC_DEQ,               // ==
    OC_NEQ,               // !=
    OC_AND,               //并 &
    OC_OR,                //或 |
    // OC_IDIV,              //整除
    /* bitwise operators */
    // OC_BAND,              //与
    // OC_BOR,               //或
    // OC_BXOR,              //非
    /* string operator */
    OC_CONCAT,            //拼接
    /* arrays operators */
    OC_ARR_VAL,            //数组值
    OC_ARR_ACE,            //数组下标访问   a[0]
    OC_ARR_IAS,            //数组赋值      a = [1,2,3,4]
    // OC_ARR_LASSIGN,        //数组列表赋值  a = a[i:j]
    // OC_ARR_PASSIGN,        //数组部分赋值  a[i] = 0
    // OC_ARR_ACESS,          //数组访问      
    // OC_ARR_LIST,           //数组区间访问
//-------------------------------------------
    /* function  operator */
    OC_END,               // 函数终止状态，虚拟机清空状态，目前只有if用到
    OC_IF,                // IF
    OC_FOR,               // FOR 
    OC_WHILE,             // WHILE
    OC_JMP,               //跳转指令
    OC_CALL,              //调用函数
    OC_RET,               //返回
    OC_ARG,               // 函数参数
};

//操作码
struct Instruction{
    unsigned char restype;       //负数类型
    unsigned char type ;         //类型
    //指令行数
    size_t curpos;
    size_t lpos; //左子树 指令偏移位置
    size_t rpos; //右子树 指令偏移位置

    D_VAR left;         //指令操作完成 可以当作返回值赋值
    D_VAR right;        //结果返回值

    Instruction();
    Instruction(const Instruction& i);
    ~Instruction();

    void operator=(const Instruction& i);

    friend std::ostream& operator<<(std::ostream& os, const Instruction& p);
};




_QD_END


#endif