#include "qd_mathlib.h"
#include <math.h>
#include <ctime>

_QD_BEGIN

// 数学函数库通用代码
#define MATH_FUN_(f)\
do{\
size_t len = l->v_pos;\
D_OBJ& v = l->vars.front();\
if ( VE_INT == v.type ) {l->rets.push_back(f(v.var.iv));}\
else if ( VE_FLT == v.type ) {l->rets.push_back(f(v.var.dv));}\
else {return 1;}\
D_STA_CLEAN_(len)\
return 0;\
} while (0)

#define MATH_FUN2_(f)\
do{\
size_t len = l->v_pos;\
if ( l->vars.size() < 2 ) {return 1;}\
D_OBJ v1 = l->vars.front();\
l->vars.pop_front();--len;\
D_OBJ v2 = l->vars.front();\
l->vars.pop_front();--len;\
if ( VE_INT == v1.type && VE_INT == v2.type ) {l->rets.push_back(f(v1.var.iv,v2.var.iv));}\
else if ( VE_FLT == v1.type && VE_INT == v2.type ) {l->rets.push_back(f(v1.var.dv,v2.var.iv));}\
else if ( VE_INT == v1.type && VE_FLT == v2.type ) {l->rets.push_back(f(v1.var.iv,v2.var.dv));}\
else if ( VE_FLT == v1.type && VE_FLT == v2.type ) {l->rets.push_back(f(v1.var.dv,v2.var.dv));}\
else {return 1;}\
D_STA_CLEAN_(len)\
return 0;\
} while (0)



size_t abs(D_State* l) {
    MATH_FUN_(std::abs);
}

size_t ceil(D_State* l) {
    MATH_FUN_(std::ceil);
}

size_t floor(D_State* l) {
    MATH_FUN_(std::floor);
}

size_t round(D_State* l) {
    MATH_FUN_(std::round);
}
// 可能会小于0
size_t log(D_State* l) {
    MATH_FUN_(std::log);
}

size_t log2(D_State* l) {
    MATH_FUN_(std::log2);
}

// 如果结果的大小太大而无法用返回类型的值表示，则该函数将返回带有正确符号的HUGE_VAL，并且会发生溢出范围错误。
size_t exp(D_State* l) {
    MATH_FUN_(std::exp);
}

// 可能存在小于0的值
size_t sqrt(D_State* l) {
    MATH_FUN_(std::sqrt);
}

size_t cbrt(D_State* l) {
    MATH_FUN_(std::cbrt);
}

size_t pow(D_State* l) {
    MATH_FUN2_(std::pow);
}

/**
    随机性,但目前使用的标准库的随机函数
    整数随机数，但目前也就那样可能以后会改，因为不人性化,目前还是用c语言默认的伪随机来生成吧
    而且用时间生成随机数，其实有缺陷所以到时候再想办法
    
    @param 如果传入参数大于等于1则表示在[0，X)范围生成数据 
*/
size_t random(D_State* l) {
    size_t len = l->v_pos;
    srand(time(0));

    int mod = INT32_MAX;

    if ( l->v_pos > 0 ) {
        len--;
        D_OBJ& obj = l->vars.front();
        if ( VE_INT != obj.type ) {
            return 1;
        }
        //数学错误
        if ( 0 == obj.var.iv) {
            return 1;
        }
        mod = obj.var.iv;
        l->vars.pop_front();
    }

    D_STA_CLEAN_(len)


    //XORshift算法
    //随机数种子
    static int n = 0;
    int r = rand() + n++;
    r ^= r << 13;
    r ^= r >> 17;
    r ^= r << 5;
    r = r % mod;

    l->rets.push_back( r ) ;
    return 0;
}

size_t sin(D_State* l) {
    MATH_FUN_(std::sin);
}

size_t cos(D_State* l) {
    MATH_FUN_(std::cos);
}

/**
 * @brief  如果为90度的倍数或者90度，可以进行计算，但是值不准确只能接近无穷大
 *         到时候在这里进行超出阈值的判断
 */
size_t tan(D_State* l) {
    MATH_FUN_(std::tan);
}

/**
 * @brief  asin()函数  [-1，1]范围内
 */
size_t asin(D_State* l) {
    MATH_FUN_(std::asin);
}

/**
 * @brief  acos()函数  [-1，1]范围内
 */
size_t acos(D_State* l) {
    MATH_FUN_(std::acos);
}

/**
 * @brief 跟tan性质一样
 */
size_t atan(D_State* l) {
    MATH_FUN_(std::atan);
}

size_t sinh(D_State* l) {
    MATH_FUN_(std::sinh);
}

size_t cosh(D_State* l) {
    MATH_FUN_(std::cosh);
}

size_t tanh(D_State* l) {
    MATH_FUN_(std::tanh);
}

size_t asinh(D_State* l) {
    MATH_FUN_(std::asinh);
}

size_t acosh(D_State* l) {
    MATH_FUN_(std::acosh);
}

size_t atanh(D_State* l) {
    MATH_FUN_(std::atanh);
}

MATH_LIB::MATH_LIB() {

}

MATH_LIB::~MATH_LIB() {

}

void MATH_LIB::load_lib() {
    funs["abs"]  =  abs;
    funs["fabs"] =  abs;
    funs["ceil"] =  ceil;
    funs["floor"]=  floor;
    funs["sqrt"] =  sqrt;
    funs["cbrt"] =  cbrt;
    funs["round"]=  round;
    funs["log"]  =  log;
    funs["log2"] =  log2;
    funs["exp"]  =  exp;
    funs["pow"]  =  pow;

    //三角函数
    funs["sin"]  =  sin;
    funs["cos"]  =  cos;
    funs["tan"]  =  tan;
    funs["asin"] =  asin;
    funs["acos"] =  acos;
    funs["atan"] =  atan;

    //双曲函数
    funs["sinh"]  =  sinh;
    funs["cosh"]  =  cosh;
    funs["tanh"]  =  tanh;
    funs["asinh"] =  asinh;
    funs["acosh"] =  acosh;
    funs["atanh"] =  atanh;

    //随机数
    funs["rand"]=  random;
}


_QD_END

