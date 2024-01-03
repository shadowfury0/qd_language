#include "qd_baselib.h"

_QD_BEGIN

/*
    基本库函数
*/

size_t print(Lib_State* l) {
    std::cout << l->vars.back();
    l->vars.pop_back();
}

size_t type(Lib_State* l) {

}

BASE_LIB::BASE_LIB() {
    name = "sys";
}

BASE_LIB::~BASE_LIB() {

}

void BASE_LIB::load_lib() {
    funs["print"] = print;
    funs["type"] = type;
}


_QD_END

