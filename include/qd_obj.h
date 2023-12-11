#ifndef __QD_VARIABLE_H__
#define __QD_VARIABLE_H__

#include "qd_header.h"
#include "qd_log.h"


_QD_BEGIN

/**
 * 变量或者表达式类型
*/

//词法解析阶段,暂存的数据类型。
enum VE_TYPE {
    VE_VOID,                //未初始化值
    VE_NULL,                //空值
    VE_BOOL,                //bool
    VE_INT,                 //整型
    VE_FLT,                 //浮点数
    VE_STR,                 //字符串
    VE_USER,                //用户变量
    VE_FUNC,                //函数
    VE_DELAY,               //滞后变量(用于函数)
//------------------------------------------
    VE_UNION,               //联合体
};

enum VA_SCOPE {
    VA_DEFAULT,
    VA_LOCAL,
    VA_GLOBAL,
};

/*
** ===================================================================
** Original Data
** ===================================================================
*/
struct D_OBJ;

union D_PRO
{
    _qd_b bv;
    _qd_int iv;
    _qd_uint uiv;
    _qd_doub dv;
    _qd_char* chv;
};

#define QD_VAR_FIELDS D_PRO var; unsigned char type;

//16
struct D_VAR{
    QD_VAR_FIELDS

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
    void operator=(const D_OBJ& dv);
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

    friend std::ostream& operator<<(std::ostream& os, const D_VAR& p);
};


/*
** ==================================================================
** Union
** ===================================================================
*/

//24
struct D_UNION {
    //默认类型
    std::vector<D_VAR> larr;

    D_UNION();
    D_UNION(const D_UNION& arr);
    D_UNION(const D_OBJ& arr);
    ~D_UNION();

    void operator=(const D_UNION& arr);
    void operator=(const D_OBJ& obj);

    friend std::ostream& operator<<(std::ostream& os, const D_UNION& p);
};

/*
** ==================================================================
** Object Type 
** ===================================================================
*/

// 32
struct D_OBJ
{
    short type;
    union 
    {
        D_PRO var;
        D_UNION* uni;
    };

    void init();
    void clear();
    void push(const D_VAR& var);

    D_OBJ();
    ~D_OBJ();

    D_OBJ(const D_VAR& var);
    D_OBJ(const D_UNION& arr);
    D_OBJ(const D_OBJ& obj);
    D_OBJ(const bool& b);
    D_OBJ(const int& v);
    D_OBJ(const unsigned int& u);
    D_OBJ(const double& v);
    D_OBJ(const char* v);
    
    void operator=(const D_VAR& dv);
    void operator=(const D_OBJ& dv);
    void operator=(const D_UNION& arr);
    void operator=(const bool& b);
    void operator=(const int& v);
    void operator=(const unsigned int& u);
    void operator=(const double& v);
    void operator=(const char* v);

    friend std::ostream& operator<<(std::ostream& os, const D_OBJ& p);
};



_QD_END


#endif