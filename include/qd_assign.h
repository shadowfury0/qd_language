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
enum VE_TYPE{
    VE_VOID = -1,           //非数据变量
    VE_NULL,                //空值
    // VE_DELAY,               //延缓变量,作用是最低级的变量,不是空值而是未赋值的变量
    VE_BOOL,                //bool
    VE_INT,                 //整型
    VE_FLT,                 //浮点数
    VE_STR,                 //字符串
    VE_USER,                //用户变量
    VE_FUNC,                //函数

//------------------------------------------
    VE_ARRAY,               //数组
    VE_UNION,               //联合体
};


/*
** ===================================================================
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
        unsigned int uiv;
        double dv;
        char* chv;
    } var;

    D_VAR();
    D_VAR(const D_VAR& dv);
    D_VAR(const bool& v);
    D_VAR(const int& v);
    D_VAR(const unsigned int& v);
    D_VAR(const double& v);
    D_VAR(const char* v);
    ~D_VAR();

    void init();
    void clear();

    void operator=(const D_VAR& dv);
    void operator=(const bool& b);
    void operator=(const int& v);
    void operator=(const unsigned int& u);
    void operator=(const double& v);
    void operator=(const char* v);
    
    bool operator==(const D_VAR& dv);
    bool operator==(const bool& b);
    bool operator==(const int& dv);
    bool operator==(const unsigned int& dv);
    bool operator==(const double& dv);
    //返回两个字符串完全相等的值
    bool operator==(const char* dv);

    bool operator!=(const D_VAR& dv);
    bool operator!=(const bool& b);
    bool operator!=(const int& dv);
    bool operator!=(const unsigned int& dv);
    bool operator!=(const double& dv);
    //返回两个字符串完全相等的值
    bool operator!=(const char* dv);

    D_VAR operator-();
    
};



/*
** ==================================================================
** Array
** ===================================================================
*/

//24
struct D_ARRAY{
    //默认类型
    std::vector<D_VAR> larr;

    D_ARRAY();
    D_ARRAY(const D_ARRAY& arr);
    void operator=(const D_ARRAY& arr);
};


struct D_OBJ
{
    union 
    {
        D_VAR  var;
        D_ARRAY array;
    };
    D_OBJ();
    D_OBJ(const D_OBJ& obj);
    ~D_OBJ();
};



_QD_END

#endif