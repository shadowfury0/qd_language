#include "qd_env.h"


_QD_BEGIN

D_ENV::D_ENV(){
    init();
    cur = new FunHead();
    // std::cout << "_____functoin value address_____" << &*cur << std::endl;
    // std::cout << "+++++++++++++++++++++++++++++" << std::endl;
}

D_ENV::D_ENV(const D_ENV& func){
    init();
    prev = func.prev;
    this->lv = func.lv;
    this->anonymous = func.anonymous;
    this->code_pos = func.code_pos;

    
    cur = new FunHead(*func.cur);
    // std::cout << "+++++++++++++++++++++++++++++" << std::endl;

}

D_ENV::~D_ENV(){
    // if (cur != nullptr){
    //     delete cur;
    //     cur = nullptr;
    // }
}

void D_ENV::init(){
    cur = nullptr;
    prev = nullptr;
    anonymous = false;
    this->code_pos = 0;
}


_QD_END