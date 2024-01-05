#include "qd_mathlib.h"

_QD_BEGIN


size_t abs(D_State* l) {

    size_t len = l->v_pos;

    D_OBJ& v = l->vars.front();

    //暂时
    std::cout << std::abs(v.var.iv) << std::endl;

    l->vars.pop_front();
    //其他函数出栈
    while (--len)
    {
        l->vars.pop_front();
    }


    return 0;
}

MATH_LIB::MATH_LIB() {

}

MATH_LIB::~MATH_LIB() {

}

void MATH_LIB::load_lib() {
    funs["abs"]  = abs;
}


_QD_END

