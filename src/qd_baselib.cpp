#include "qd_baselib.h"

_QD_BEGIN

/*
    基本库函数
*/

size_t print(D_State* l) {
    size_t len = l->v_pos;

    while ( len && l->vars.size() )
    {
        D_OBJ& v = l->vars.front();
        switch (v.type)
        {
        case VE_BOOL:
            if (v.var.bv) std::cout << "true ";
            else    std::cout << "false ";
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

    //压栈
    D_STA_PUS_NUL

    return 0;
}

//只判断第一个数据类型
size_t type(D_State* l) {
    size_t len = l->v_pos;

    D_OBJ& v = l->vars.front();
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
    std::cout << std::endl;

    //其他函数出栈
    D_STA_CLEAN_(len)

    D_STA_PUS_NUL

    return 0;
}

size_t error(D_State* l) {
    std::cout << "system error !!! " ;
    print(l);
    return 1;
}

size_t assert(D_State* l) {
    size_t len = l->v_pos;

    D_OBJ& v = l->vars.front();

    //判断该数是否为true，如果为true，继续执行下面代码
    bool ass = false;

    switch (v.type)
    {
    case VE_BOOL:
        ass =  v.var.bv == false ? true : false;
        break;
    case VE_INT:
        ass = v.var.iv == 0 ? true : false;
        break;
    default:
        ass = false;
        break;
    }

    D_STA_CLEAN_(len);

    D_STA_PUS_NUL

    return !ass;
}

size_t len(D_State* l) {
    
    size_t len = l->v_pos;

    D_OBJ& v = l->vars.front();
    
    int s = 0;
    if ( VE_UNION == v.type ) {
        //潜在溢出
        s = (int)v.uni->larr.size();
    }

    //其他函数出栈
    D_STA_CLEAN_(len);

    l->rets.push_back(s);

    return 0;
}

BASE_LIB::BASE_LIB() {

}

BASE_LIB::~BASE_LIB() {

}

void BASE_LIB::load_lib() {
    funs["print"]  = print;
    funs["type"]   = type;
    funs["error"]  = error;
    funs["assert"] = assert;
    funs["len"]    = len;
}



_QD_END

