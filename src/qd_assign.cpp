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

D_VAR::D_VAR(const unsigned int& u){
    init();
    *this = u;
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
    this->type = VE_VOID;
}

void D_VAR::operator=(const D_VAR& dv){
    if (VE_STR == dv.type ||
        VE_USER == dv.type) {
        *this = dv.var.chv;
    }
    else{
        this->var = dv.var;
    }
    this->type = dv.type;
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

void D_VAR::operator=(const unsigned int& u){
    clear();
    this->type = VE_INT;
    this->var.uiv = u;
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


bool D_VAR::operator==(const D_VAR& dv){
    switch (this->type)
    {
    case VE_INT:
    case VE_FUNC:
        return *this == dv.var.iv;
    case VE_BOOL:
        return *this == dv.var.bv;
    case VE_FLT:
        return *this == dv.var.dv;
    case VE_STR:
    case VE_USER:
        return *this == dv.var.chv;
    default:
        return false;
    }
}

bool D_VAR::operator==(const bool& b){
    return this->var.bv == b;
}

bool D_VAR::operator==(const int& iv){
    return this->var.iv == iv;
}

bool D_VAR::operator==(const unsigned int& uiv){
    return this->var.uiv == uiv;
}

bool D_VAR::operator==(const double& bv){
    return this->var.bv == bv;
}

bool D_VAR::operator==(const char* chv){
    return 0 == strcmp(this->var.chv,chv);
}


bool D_VAR::operator!=(const D_VAR& dv){
    switch (this->type)
    {
    case VE_INT:
    case VE_FUNC:
        return *this != dv.var.iv;
    case VE_BOOL:
        return *this != dv.var.bv;
    case VE_FLT:
        return *this != dv.var.dv;
    case VE_STR:
    case VE_USER:
        return *this != dv.var.chv;
    default:
        return false;
    }
}

bool D_VAR::operator!=(const bool& b){
    return this->var.bv != b;
}

bool D_VAR::operator!=(const int& iv){
    return this->var.iv != iv;
}

bool D_VAR::operator!=(const unsigned int& uiv){
    return this->var.uiv == uiv;
}

bool D_VAR::operator!=(const double& bv){
    return this->var.bv != bv;
}

bool D_VAR::operator!=(const char* chv){
    return 0 != strcmp(this->var.chv,chv);
}


D_ARRAY::D_ARRAY(){

}

D_ARRAY::D_ARRAY(const D_ARRAY& arr){
    *this = arr;
}

void D_ARRAY::operator=(const D_ARRAY& arr){
    for (std::vector<D_VAR>::const_iterator
     iter =  arr.larr.begin(); iter != arr.larr.end() ;
     iter ++ ) {
        this->larr.push_back(*iter);
    }
}

D_OBJ::D_OBJ(){

}

D_OBJ::D_OBJ(const D_OBJ& obj){

}

D_OBJ::~D_OBJ(){
    
}

_QD_END