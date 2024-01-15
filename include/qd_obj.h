#ifndef __QD_OBJ_H__
#define __QD_OBJ_H__

#include "qd_header.h"


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
//------------------------------------------
    VE_ARRAY,               //数组
    VE_UNION,               //联合体
//------------------------------------------
    VE_LIB,                 //用于库
};

//作用域
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
    _qd_st  sv;
    _qd_double dv;
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
    D_VAR(const double& v);
    D_VAR(const char* v);
    ~D_VAR();

    void init();
    void clear();

    void operator=(const D_VAR& dv);
    void operator=(const D_OBJ& dv);
    void operator=(const bool& b);
    void operator=(const int& v);
    void operator=(const double& v);
    void operator=(const char* v);
    void alloc_str(const char* v,size_t len);
    
    bool operator==(const D_VAR& dv);
    bool operator==(const bool& b);
    bool operator==(const int& dv);
    bool operator==(const double& dv);
    //返回两个字符串完全相等的值
    bool operator==(const char* dv);

    bool operator!=(const D_VAR& dv);
    bool operator!=(const bool& b);
    bool operator!=(const int& dv);
    bool operator!=(const double& dv);
    //返回两个字符串完全相等的值
    bool operator!=(const char* dv);

    D_VAR operator-();

    friend std::ostream& operator<<(std::ostream& os, const D_VAR& p);
};


/*
** ==================================================================
** Array 
** ===================================================================
*/

struct D_ARRAY {
    std::vector<D_PRO> arr;

    D_ARRAY();
    D_ARRAY(const D_ARRAY& ar);
    D_ARRAY(const D_OBJ& ar);
    ~D_ARRAY();

    void operator=(const D_ARRAY& arr);
    void operator=(const D_OBJ& obj);
};

/*
** ==================================================================
** Union
** ===================================================================
*/

//24
struct D_UNION {
    //默认类型
    std::vector<D_VAR> un;

    D_UNION();
    D_UNION(const D_UNION& un);
    D_UNION(const D_OBJ& un);
    ~D_UNION();

    void operator=(const D_UNION& un);
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
    unsigned char type;
    //数组类型
    unsigned char at;

    union 
    {
        D_PRO var;
        D_ARRAY* arr;
        D_UNION* uni;
    };

    void init();
    void clear();
    void push_back(const D_VAR& var);
    //返回数组类型值
    size_t size();

    D_OBJ();
    ~D_OBJ();

    D_OBJ(const D_VAR& var);
    D_OBJ(const D_UNION& un);
    D_OBJ(const D_ARRAY& arr);
    D_OBJ(const D_OBJ& obj);
    D_OBJ(const bool& b);
    D_OBJ(const int& v);
    D_OBJ(const double& v);
    D_OBJ(const char* v);
    
    void operator=(const D_VAR& dv);
    void operator=(const D_OBJ& dv);
    void operator=(const D_UNION& un);
    void operator=(const D_ARRAY& un);
    void operator=(const bool& b);
    void operator=(const int& v);
    void operator=(const double& v);
    void operator=(const char* v);
    void alloc_str(const char* v,size_t len);

    D_OBJ operator-();

    friend std::ostream& operator<<(std::ostream& os, const D_OBJ& p);
    
};



_QD_END


#endif