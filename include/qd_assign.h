#ifndef __QD_VARIABLE_H__
#define __QD_VARIABLE_H__

#include "qd_header.h"
/*
    记录全局变量等信息
*/

_QD_BEGIN

/**
 * 变量或者表达式类型
*/
enum VETYPE{
    VE_VOID = -1,           //非数据变量
    VE_NULL,                //空值
    VE_BOOL,                //bool
    VE_INT,                 //整型
    VE_FLT,                 //浮点数
    VE_STR,                 //字符串
    VE_ARRAY,               //数组(不不同类型数据数组)
    VE_USER,                //用户变量
    //？？？？这个是啥变量
    VE_DELAY,               //延缓变量
};


/*
** ==================================================================
** Original Data
** ===================================================================
*/

//16
struct D_VAR{
    short type;
    union d_assign
    {
        bool bv;
        int iv;
        double dv;
        char* chv;
    } var;

    D_VAR();
    D_VAR(const D_VAR& dv);
    D_VAR(const bool& v);
    D_VAR(const int& v);
    D_VAR(const double& v);
    D_VAR(const char* v);
    ~D_VAR();

    void init();
    void clear();

    void operator=(const D_VAR& dv);
    void operator=(const bool& b);
    void operator=(const int& v);
    void operator=(const double& v);
    void operator=(const char* v);

    D_VAR operator-();
};

/*
** ==================================================================
** Array
** ===================================================================
*/

struct D_ARRY{
    int type = VE_ARRAY;    //数组类型
    std::vector<D_VAR> arry;
};



_QD_END

#endif