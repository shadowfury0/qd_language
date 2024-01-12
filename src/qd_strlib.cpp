#include "qd_strlib.h"
#include "qd_log.h"


_QD_BEGIN

//获取字符串,str 为 std::string 类型
#define STR_FUN_(str)\
do{\
if ( VE_STR != l->vars.front().type ) {return 1;}\
str = l->vars.front().var.chv;--len;\
l->vars.pop_front();\
}while (0)


size_t _str_len(D_State* l) {
    size_t len = l->v_pos;
    if ( len < 1 ) {
        D_STA_PUS_NUL
        return 0;
    }

    D_OBJ& str = l->vars.front();

    // args is not string type
    if ( VE_STR != str.type ) {
        l->rets.push_back(0);
    }
    else
    {
        //潜在溢出
        int s = (int)strlen(str.var.chv);
        l->rets.push_back(s);
    }

    while (len--)
    {
        l->vars.pop_front();
    }
    
    return 0;
}

/**
 * 截取字符串准备跟c++一样 
 *  访问字符串，从下标0开始，如果大于字符串长度则报错 
 *  第一个参数为字符串，第二个截取起始位置，第三个参数为截取长度(可以没有)
 *  但是索引位置超出会报错，截取长度不会超出不会报错
 */
size_t _str_sub(D_State* l) {
    size_t len = l->v_pos;

    if ( len < 2 ) {
        return 1;
    }

    std::string str;
    //第一个参数
    STR_FUN_(str);


    //第二个参数
    D_OBJ var1 = l->vars.front();
    if ( VE_INT != var1.type || ( var1.var.iv < 0 && var1.var.iv > str.size() - 1 ) ) {
        return 1;
    }
    --len;
    l->vars.pop_front();

    //如果有第三个参数
    if (len > 0) {
        //第三个参数
        D_OBJ var2 = l->vars.front();
        if ( VE_INT != var2.type || var2.var.iv < 0 ) {
            return 1;
        }
        --len;
        l->vars.pop_front();

        l->rets.push_back(str.substr(var1.var.iv,var2.var.iv).c_str());
    }
    else {
        l->rets.push_back(str.substr(var1.var.iv).c_str());
    }
    
    D_STA_CLEAN_(len)

    return 0;
}

size_t _str_cmp(D_State* l) {
    size_t len = l->v_pos;
    if ( len < 2 ) {
        return 1;
    }

    std::string str1;
    std::string str2;
    //第一个参数
    STR_FUN_(str1);
    //第二个参数
    STR_FUN_(str2);

    if(str1 == str2) {
        l->rets.push_back(true);
    }
    else {
        l->rets.push_back(false);
    }

    while (len--)
    {
        l->vars.pop_front();
    }

    return 0;
}

size_t _str_upper(D_State* l) {
    size_t len = l->v_pos;
    if ( len < 1 ) {
        return 1;
    }

    std::string str;
    STR_FUN_(str);
    
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return std::toupper(c); });

    l->rets.push_back(str.c_str());

    return 0;
}

size_t _str_lower(D_State* l) {
    size_t len = l->v_pos;
    if ( len < 1 ) {
        return 1;
    }

    std::string str;
    STR_FUN_(str);
    
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return std::tolower(c); });

    l->rets.push_back(str.c_str());
    return 0;
}

STR_LIB::STR_LIB() {

}

STR_LIB::~STR_LIB() {
    
}

void STR_LIB::load_lib() {
    funs["len"]    =   _str_len;
    funs["substr"] =   _str_sub;
    funs["cmp"]    =   _str_cmp;

    funs["upper"]  =   _str_upper;
    funs["lower"] =    _str_lower;
}


_QD_END

