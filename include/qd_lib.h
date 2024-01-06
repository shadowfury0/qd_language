#ifndef _QD_LIB_H_
#define _QD_LIB_H_

#include "qd_header.h"
#include "qd_state.h"

_QD_BEGIN


// 基类库模块

struct _LIB
{
    _LIB();
    ~_LIB();

    //库初始化
    virtual void load_lib() = 0;
    
    /**
     * @brief 查找库函数名称是否存在
     * @param name 
     */
    bool is_fun(const std::string& name);

    //内部函数
    std::map<std::string,qd_inner_fun> funs;
};

/*
    总模块，负责查询各模块的调用总信息
*/

struct D_LIB
{
    D_LIB();
    ~D_LIB();

    /**
     * @brief 初始化库
     * @return 0 is correct
    */
    size_t init_libs();
    /**
        @brief 查找库函数名是否存在
        @param k 库名
        @param n 函数名
        @return true is exist
    */
    bool is_fun(const std::string& k,const std::string& n);

    //库名，以及库子函数名
    std::map<std::string,_LIB*> l;
};



_QD_END


#endif

