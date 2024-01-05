#include "qd_lib.h"
#include "qd_baselib.h"
#include "qd_mathlib.h"

_QD_BEGIN

_LIB::_LIB() {

}

_LIB::~_LIB() {
    
}

bool _LIB::is_fun(const std::string& name) {
    if (this->funs.find(name) != this->funs.end()) return true;
    return false;
}

D_LIB::D_LIB() {

}
    
D_LIB::~D_LIB() {
    for ( auto i : this->l ) {
        if (i.second != nullptr) {
            delete i.second;
            i.second = nullptr;
        } 
    }
}

bool D_LIB::is_fun(const std::string& k,const std::string& n) {
    if ( this->l.find(k) == this->l.end() ) {
        return false;
    }

    return this->l[k]->is_fun(n);
}


size_t D_LIB::init_libs() {

    //函数库调用
    BASE_LIB* base_lib = new BASE_LIB();
    base_lib->load_lib();
    this->l["sys"] = base_lib;

    MATH_LIB* math_lib = new MATH_LIB();
    math_lib->load_lib();
    this->l["math"] = math_lib;
    
    return 0;
}


_QD_END
