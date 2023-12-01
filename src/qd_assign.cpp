#include "qd_lex.h"
#include "qd_assign.h"

_QD_BEGIN


void D_VAR::init(){
    memset(&this->var,0,sizeof(this->var));
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
}

void D_VAR::clear(){
    if (  VE_STR == this->type ||
         VE_USER == this->type
          ){
        free(this->var.chv);
    }
    this->type = VE_VOID;
}

void D_VAR::operator=(const D_VAR& dv){
    clear();
    if (VE_STR == dv.type ||
        VE_USER == dv.type) {
        *this = dv.var.chv;
    }
    else{
        this->var = dv.var;
    }
    this->type = dv.type;
}

void D_VAR::operator=(const D_OBJ& dv){
    clear();
    if ( VE_ARRAY == this->type || 
         VE_UNION == this->type ) {
    }
    else if ( VE_STR == dv.type ||
         VE_USER == dv.type ) {
        *this = dv.var.chv;
    }
    else {
        this->var = dv.var;
        this->type = dv.type;
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
    this->var.chv = (char*)malloc( strlen(v) + 1 );
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
        printf("dvar type is string or void or null \n");
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


D_UNION::D_UNION(){

}

D_UNION::D_UNION(const D_UNION& arr){
    *this = arr;
}

D_UNION::D_UNION(const D_OBJ& obj){
    *this = obj;
}

D_UNION::~D_UNION(){
}

void D_UNION::operator=(const D_UNION& arr){
    this->larr.clear();
    for (std::vector<D_VAR>::const_iterator
     iter =  arr.larr.begin(); iter != arr.larr.end() ;
     iter ++ ) {
        this->larr.push_back(*iter);
    }
}

void D_UNION::operator=(const D_OBJ& obj){
    if ( VE_UNION == obj.type ) {
        for (std::vector<D_VAR>::iterator iter = obj.uni->larr.begin() ;
            iter != obj.uni->larr.end() ; iter ++ ) {
            this->larr.push_back(*iter);
        }
    }
}


void D_OBJ::init(){
    memset(&this->var,0,sizeof(this->var));
    this->type = VE_VOID;
}

void D_OBJ::clear(){
    if ( VE_STR == this->type ||
         VE_USER == this->type
          ){
        free(this->var.chv);
    }
    else if ( VE_UNION == this->type ) {
        if (this->uni != nullptr){
            delete this->uni;
        }
    }
    this->type = VE_VOID;
}

D_OBJ::D_OBJ()  {
    init();
}

D_OBJ::~D_OBJ(){
    clear();
}

D_OBJ::D_OBJ(const D_VAR& var){
    init();
    *this = var;
}

D_OBJ::D_OBJ(const D_OBJ& obj) {
    init();
    *this = obj;
}

D_OBJ::D_OBJ(const D_UNION& arr){
    init();
    *this = arr;
}

D_OBJ::D_OBJ(const bool& b){
    init();
    *this = b;
}

D_OBJ::D_OBJ(const int& v){
    init();
    *this = v;
}

D_OBJ::D_OBJ(const unsigned int& u){
    init();
    *this = u;
}

D_OBJ::D_OBJ(const double& v){
    init();
    *this = v;
}

D_OBJ::D_OBJ(const char* v){
    init();
    *this = v;
}

void D_OBJ::operator=(const D_VAR& dv){
    clear();
    if ( VE_STR == dv.type ||
         VE_USER == dv.type ) {
        *this = dv.var.chv;
    }
    else {
        this->var = dv.var;
    }
    this->type = dv.type;
}

void D_OBJ::operator=(const D_OBJ& ob) {
    clear();
    switch (ob.type)
    {
    case VE_STR:
    case VE_USER:
        *this = ob.var.chv;
        break;
    case VE_UNION:
        *this = ob.uni;
    default:
        this->var = ob.var;
        break;
    }
    this->type = ob.type;
}

void D_OBJ::operator=(const D_UNION& arr){
    clear();
    this->uni = new D_UNION(arr);
    this->type = VE_UNION;
}

void D_OBJ::operator=(const bool& b){
    clear();
    this->type = VE_BOOL;
    this->var.bv = b;
}

void D_OBJ::operator=(const int& v){
    clear();
    this->type = VE_INT;
    this->var.iv = v;
}

void D_OBJ::operator=(const unsigned int& u){
    clear();
    this->type = VE_INT;
    this->var.uiv = u;
}

void D_OBJ::operator=(const double& v){
    clear();
    this->type = VE_FLT;
    this->var.dv = v;
}

void D_OBJ::operator=(const char* v){
    clear();
    this->type = VE_STR;
    this->var.chv = (char*)malloc( strlen(v) + 1 );

    strcpy(this->var.chv,v);
}


_QD_END