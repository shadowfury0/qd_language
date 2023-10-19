#include "qd_lex.h"
#include "qd_assign.h"

_QD_BEGIN

void D_VAR::init(){
    memset(&this->var,0,sizeof(this->var));
    // clear();
    this->type = VE_VOID;
}

D_VAR::D_VAR(){
    init();
}

D_VAR::D_VAR(const D_VAR& dv){
    init();
    *this = dv;
}

D_VAR::D_VAR(const bool& b){
    init();
    *this = b;
}

D_VAR::D_VAR(const int& v){
    init();
    *this = v;
}
D_VAR::D_VAR(const double& v){
    init();
    *this = v;
}
D_VAR::D_VAR(const char* v){
    init();
    *this = v;
}

D_VAR::~D_VAR(){
    clear();
    this->type = VE_VOID;//这个必须在后
}

void D_VAR::clear(){
    if ( this->type == VE_STR 
        || this->type == VE_USER  
          ){
        free(this->var.chv);
    }
    //清0可能暂时不用
    // memset(&this->var,0,sizeof(this->var));
}

void D_VAR::operator=(const D_VAR& dv){
    if (VE_STR == dv.type ||
        VE_USER == dv.type) {
        *this = dv.var.chv;
        this->type = dv.type;
    }else{
        this->type = dv.type;
        this->var = dv.var;
    }
}

void D_VAR::operator=(const bool& b){
    clear();
    this->type = VE_BOOL;
    this->var.bv = b;
}

void D_VAR::operator=(const int& v){
    clear();
    this->type = VE_INT;
    this->var.iv = v;
}

void D_VAR::operator=(const double& v){
    clear();
    this->type = VE_FLT;
    this->var.dv = v;
}

void D_VAR::operator=(const char* v){
    clear();
    this->type = VE_STR;
    this->var.chv = (char*)malloc( strlen(v) + 1);
    strcpy(this->var.chv,v);
}

D_VAR D_VAR::operator-(){
    D_VAR tmp;
    switch (this->type)
    {
    case VE_BOOL:
        tmp = !this->var.bv;
        break;
    case VE_INT:
        tmp = -this->var.iv;
        break;
    case VE_FLT:
        tmp = -this->var.dv;
        break;
    default:
        printf("dvar type is string\n");
        break;
    }
    return tmp;
}


_QD_END