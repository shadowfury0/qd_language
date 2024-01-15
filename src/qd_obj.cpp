#include "qd_obj.h"

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

// D_VAR::D_VAR(const size_t& u){
//     init();
//     *this = u;
// }

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
    if (VE_VOID == this->type ) {
        return;
    }
    if ( VE_STR == this->type ||
         VE_USER == this->type ||
         VE_ARRAY == this->type ||
         VE_UNION == this->type 
          ){
        delete[] this->var.chv;
        this->var.chv = nullptr;
    }
    this->type = VE_VOID;
    memset(&this->var,'\0',sizeof(this->var));
}

void D_VAR::operator=(const D_VAR& dv){
    clear();
    if (VE_STR == dv.type ||
        VE_USER == dv.type || 
        VE_ARRAY == dv.type || 
        VE_UNION == dv.type ) {
        *this = dv.var.chv;
    }
    else{
        this->var = dv.var;
    }
    this->type = dv.type;
}

void D_VAR::operator=(const D_OBJ& dv){
    clear();
    if ( 
        VE_ARRAY == this->type || 
         VE_UNION == dv.type ) {
        //do nothing 默认给个初值
        *this = "";
    }
    else if ( 
        VE_STR == dv.type ||
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

// void D_VAR::operator=(const size_t& u){
//     clear();
//     this->type = VE_INT;
//     this->var.iv = u;
// }

void D_VAR::operator=(const double& v){
    clear();
    this->type = VE_FLT;
    this->var.dv = v;
}

void D_VAR::operator=(const char* v){
    clear();
    this->type = VE_STR;
    this->var.chv = new char[strlen(v) + 1 ];
    this->var.chv[strlen(v)] = '\0';
    strcpy(this->var.chv,v);
}

void D_VAR::alloc_str(const char* v,size_t len) {
    clear();
    this->type = VE_STR;
    this->var.chv = new char[strlen(v) + 1 ];
    this->var.chv[strlen(v)] = '\0';
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
    case VE_LIB:
        return *this == dv.var.iv;
    case VE_BOOL:
        return *this == dv.var.bv;
    case VE_FLT:
        return *this == dv.var.dv;
    case VE_STR:
    case VE_USER:
    case VE_UNION:
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

// bool D_VAR::operator==(const size_t& uiv){
//     return this->var.iv == uiv;
// }

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
    case VE_LIB:
        return *this != dv.var.iv;
    case VE_BOOL:
        return *this != dv.var.bv;
    case VE_FLT:
        return *this != dv.var.dv;
    case VE_STR:
    case VE_USER:
    case VE_UNION:
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

// bool D_VAR::operator!=(const size_t& uiv){
//     return this->var.iv == uiv;
// }

bool D_VAR::operator!=(const double& bv){
    return this->var.bv != bv;
}

bool D_VAR::operator!=(const char* chv){
    return 0 != strcmp(this->var.chv,chv);
}

std::ostream& operator<<(std::ostream& os, const D_VAR& p){
    switch (p.type)
    {
    case VE_VOID:
        os << " null ";
        break;
    case VE_BOOL:
        os << " bool : " << p.var.bv ;
        break;
    case VE_INT:
        os << " int : " << p.var.iv ;
        break;
    case VE_FLT:
        os << " double : " << p.var.dv ;
        break;
    case VE_STR:
        os << " string : " << p.var.chv ;
        break;
    case VE_USER:
        os << " user : " << p.var.chv ;
        break;
    case VE_FUNC:
        os << " function : " << p.var.iv;
        break;
    case VE_ARRAY:
        os << " function : " << p.var.iv; 
        break;
    case VE_UNION:
        os << " union " << p.var.chv;
        break;
    case VE_LIB:
        os << " lib ";
        break;
    default:
        os << " error type ";
        break;
    }
    return os << " ";
}

D_ARRAY::D_ARRAY() {

}

D_ARRAY::D_ARRAY(const D_ARRAY& ar) {
    this->arr = ar.arr;
}

D_ARRAY::D_ARRAY(const D_OBJ& obj) {
    *this = obj;
}

D_ARRAY::~D_ARRAY() {

}

void D_ARRAY::operator=(const D_ARRAY& ar){
    this->arr = ar.arr;
}

void D_ARRAY::operator=(const D_OBJ& obj) {
    if ( VE_ARRAY == obj.type ) {
        *this = *obj.arr;
    }
}

D_UNION::D_UNION(){

}

D_UNION::D_UNION(const D_UNION& un){
    *this = un;
}

D_UNION::D_UNION(const D_OBJ& obj){
    *this = obj;
}

D_UNION::~D_UNION(){
}

void D_UNION::operator=(const D_UNION& un){
    this->un = un.un;
}

void D_UNION::operator=(const D_OBJ& obj){
    if ( VE_UNION == obj.type ) {
        *this = *obj.uni;
    }
}

std::ostream& operator<<(std::ostream& os, const D_UNION& u) {
    size_t i = 0;
    for (;i < u.un.size();i++ ) {
        os << u.un[i] << " ";
    }
    return os;
}

void D_OBJ::init(){
    memset(&this->var,0,sizeof(this->var));
    this->type = VE_VOID;
    this->at = VE_VOID;
}

void D_OBJ::clear(){
    if (VE_VOID == this->type ) {
        return;
    }
    if ( VE_STR == this->type ||
         VE_USER == this->type
          ){
        delete[] this->var.chv;
        this->var.chv = nullptr;
    }
    else if ( VE_UNION == this->type ) {
        if ( this->uni != nullptr){
            delete this->uni;
            this->uni = nullptr;
        }
    }
    else if ( VE_ARRAY == this->type ) {
        //如果是字符串
        if ( VE_STR == this->at) {
            for (D_PRO& i : this->arr->arr ) {
                delete[] i.chv;
                i.chv = nullptr;
            }
        }
        if ( this->arr != nullptr ){
            delete this->arr;
            this->arr = nullptr;
        }
    }
    this->type = VE_VOID;
    memset(&this->var,'\0',sizeof(this->var));
}

void D_OBJ::push_back(const D_VAR& var) {
    if ( VE_UNION == this->type ) 
        this->uni->un.push_back(var);
    else if ( VE_ARRAY == this->type ) {
        D_PRO pro;
        if ( VE_STR == this->at ) {
            size_t len = strlen(var.var.chv);
            pro.chv = new char[len + 1];
            pro.chv[len] = '\0';
            strncpy(pro.chv,var.var.chv,len);
        }
        else {
            memcpy(&pro,&var,sizeof(D_PRO));
        }
        this->arr->arr.push_back(pro);
    } 
}

size_t D_OBJ::size() {
    if ( VE_UNION == this->type ) 
        return this->uni->un.size();
    else if ( VE_ARRAY == this->type )
        return this->arr->arr.size();
    else
        return 0;    
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

D_OBJ::D_OBJ(const D_UNION& un){
    init();
    *this = un;
}

D_OBJ::D_OBJ(const D_ARRAY& arr){
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

// D_OBJ::D_OBJ(const size_t& u){
//     init();
//     *this = u;
// }

D_OBJ::D_OBJ(const double& v){
    init();
    *this = v;
}

D_OBJ::D_OBJ(const char* v){
    init();
    *this = v;
}

void D_OBJ::operator=(const D_VAR& dv){
    if ( VE_STR == dv.type ||
         VE_USER == dv.type ) {
        *this = dv.var.chv;
        this->type = dv.type;
    }
    else if ( VE_UNION == dv.type || VE_UNION == dv.type ) {
        clear();
        //do nothing
        std::cout << " var can not assign obj" << std::endl; 
    }
    else {
        this->var = dv.var;
        this->type = dv.type;
    }
}

void D_OBJ::operator=(const D_OBJ& ob) {
    clear();
    switch (ob.type)
    {
    case VE_STR:
    case VE_USER:
        *this = ob.var.chv;
        break;
    case VE_ARRAY:
        *this = *ob.arr;
        this->at = ob.at;
        break;
    case VE_UNION:
        *this = *ob.uni;
        break;
    default:
        this->var = ob.var;
        break;
    }
    this->type = ob.type;
}

void D_OBJ::operator=(const D_UNION& un){
    clear();
    this->uni = new D_UNION(un);
    this->type = VE_UNION;
}

void D_OBJ::operator=(const D_ARRAY& ar){
    clear();
    this->arr = new D_ARRAY(ar);
    this->type = VE_ARRAY;
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

// void D_OBJ::operator=(const size_t& u){
//     clear();
//     this->type = VE_INT;
//     this->var.iv = u;
// }

void D_OBJ::operator=(const double& v){
    clear();
    this->type = VE_FLT;
    this->var.dv = v;
}

void D_OBJ::operator=(const char* v){
    clear();
    this->type = VE_STR;
    this->var.chv = new char[ strlen(v) + 1 ];
    this->var.chv[strlen(v)] = '\0';
    strcpy(this->var.chv,v);
}

void D_OBJ::alloc_str(const char* v,size_t len) {
    clear();
    this->type = VE_STR;
    this->var.chv = new char[strlen(v) + 1 ];
    this->var.chv[strlen(v)] = '\0';
    strcpy(this->var.chv,v);
}

D_OBJ D_OBJ::operator-() {
    D_OBJ tmp;
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
        printf("dobj type is string or void or null \n");
        break;
    }
    return tmp;
}

std::ostream& operator<<(std::ostream& os, const D_OBJ& p) {
    switch (p.type)
    {
    case VE_VOID:
        os << " null ";
        break;
    case VE_BOOL:
        os << " bool : " << p.var.bv ;
        break;
    case VE_INT:
        os << " int : " << p.var.iv ;
        break;
    case VE_FLT:
        os << " double : " << p.var.dv ;
        break;
    case VE_STR:
        os << " string : " << p.var.chv ;
        break;
    case VE_USER:
        os << " user : " << p.var.chv ;
        break;
    case VE_FUNC:
        os << " function : " << p.var.iv; 
        break;
    case VE_ARRAY:
        //单独打印
        os << " array :  ";
        if ( VE_BOOL == p.at ) {
            for (D_PRO& i : p.arr->arr) {
                os << i.bv << " ";
            }
        }
        else if ( VE_FLT == p.at ) {
            for (D_PRO& i : p.arr->arr) {
                os << i.dv << " ";
            }
        }
        else if ( VE_STR == p.at ) {
            for (D_PRO& i : p.arr->arr) {
                os << *i.chv << " ";
            }
        }
        else {
            for (D_PRO& i : p.arr->arr) {
                os << i.iv << " ";
            }
        }
        break;
    case VE_UNION:
        os << " union " << *p.uni;
        break;
    default:
        os << " error type ";
        break;
    }
    return os << " ";
}



_QD_END