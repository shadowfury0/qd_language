#include "qd_baselib.h"

_QD_BEGIN

/*
    基本库函数
*/

size_t print(D_State* l) {
    size_t len = l->pos;

    while ( len && l->vars.size() )
    {
        D_VAR& v = l->vars.front();
        switch (v.type)
        {
        case VE_BOOL:
            std::cout <<  v.var.bv ;
            break;
        case VE_INT:
            std::cout << v.var.iv ;
            break;
        case VE_FLT:
            std::cout << v.var.dv ;
            break;
        case VE_STR:
            std::cout << v.var.chv ;
            break;
        default:
            std::cout << " null ";
            break;
        }
        l->vars.pop_front();
        len -- ;
    }
    std::cout << std::endl;
}

//只判断第一个数据类型
size_t type(D_State* l) {
    size_t len = l->pos;

    D_VAR& v = l->vars.front();
    switch (v.type)
    {
    case VE_BOOL:
        std::cout <<  "bool" ;
        break;
    case VE_INT:
        std::cout << "int" ;
        break;
    case VE_FLT:
        std::cout << "double" ;
        break;
    case VE_STR:
        std::cout << "string" ;
        break;
    default:
        std::cout << " null ";
        break;
    }
    l->vars.pop_front();
    std::cout << std::endl;

    //其他函数出栈
    while (--len)
    {
        l->vars.pop_front();
    }
}

BASE_LIB::BASE_LIB() {

}

BASE_LIB::~BASE_LIB() {

}

void BASE_LIB::load_lib() {
    funs["print"] = print;
    funs["type"] = type;
}



_QD_END

