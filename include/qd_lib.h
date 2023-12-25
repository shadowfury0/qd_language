#ifndef _QD_LIB_H_
#define _QD_LIB_H_

#include "qd_header.h"
#include "qd_func.h"

_QD_BEGIN


/*
    总模块，负责查询各模块的调用总信息
*/

struct BASE_LIB
{
    //定义函数指针
    typedef int (*lua_CFunction) (void *v);

    BASE_LIB();
    //目前是浅拷贝
    BASE_LIB(const BASE_LIB& d);
    ~BASE_LIB();

    std::string name;

    //模块内部变量
    std::map<std::string,D_OBJ> v;


};


_QD_END


#endif

