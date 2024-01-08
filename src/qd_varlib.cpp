#include "qd_varlib.h"


_QD_BEGIN


int _dvar_add(D_VAR& result,const D_VAR& left,const D_VAR& right){
    if ( VE_FLT == left.type && VE_FLT == right.type ) {
        result = left.var.dv + right.var.dv;
    }
    else if ( VE_FLT == left.type && VE_INT == right.type ) {
        result = left.var.dv + right.var.iv;
    }
    else if ( VE_INT == left.type && VE_FLT == right.type ) {
        result = left.var.iv + right.var.dv;
    }
    else if ( VE_INT == left.type && VE_INT == right.type ) {
        result = left.var.iv + right.var.iv;
    }
    else {
        return 1;
    }

    return 0;
}

int _dvar_sub(D_VAR& result,const D_VAR& left,const D_VAR& right){

    if ( VE_FLT == left.type && VE_FLT == right.type ) {
        result = left.var.dv - right.var.dv;
    }
    else if ( VE_FLT == left.type && VE_INT == right.type ) {
        result = left.var.dv - right.var.iv;
    }
    else if ( VE_INT == left.type && VE_FLT == right.type ) {
        result = left.var.iv - right.var.dv;
    }
    else if ( VE_INT == left.type && VE_INT == right.type ) {
        result = left.var.iv - right.var.iv;
    }
    else {
        return 1;
    }
    return 0;
}

int _dvar_mul(D_VAR& result,const D_VAR& left,const D_VAR& right){

    if ( VE_FLT == left.type && VE_FLT == right.type ) {
        result = left.var.dv * right.var.dv;
    }
    else if ( VE_FLT == left.type && VE_INT == right.type ) {
        result = left.var.dv * right.var.iv;
    }
    else if ( VE_INT == left.type && VE_FLT == right.type ) {
        result = left.var.iv * right.var.dv;
    }
    else if ( VE_INT == left.type && VE_INT == right.type ) {
        result = left.var.iv * right.var.iv;
    }
    else {
        return 1;
    }
    return 0;
}
// right 不允许为0
int _dvar_mod(D_VAR& result,const D_VAR& left,const D_VAR& right) {
    if ( VE_INT == left.type && VE_INT == right.type ) {
        result = left.var.iv % right.var.iv;
    }
    else {
        return 1;
    }
    return 0;
}

int _dvar_div(D_VAR& result,const D_VAR& left,const D_VAR& right){
    if ( VE_FLT == left.type && VE_FLT == right.type ) {
        result = left.var.dv / right.var.dv;
    }
    else if ( VE_FLT == left.type && VE_INT == right.type ) {
        result = left.var.dv / right.var.iv;
    }
    else if ( VE_INT == left.type && VE_FLT == right.type ) {
        result = left.var.iv / right.var.dv;
    }
    else if ( VE_INT == left.type && VE_INT == right.type ) {
        result = left.var.iv / right.var.iv;
    }
    else {
        return 1;
    }
    return 0;
}

int _dvar_and(D_VAR& result,const D_VAR& left,const D_VAR& right) {
    if ( VE_BOOL != left.type || VE_BOOL != right.type ) {
        return 1;
    }
    result = right.var.bv && left.var.bv;
    return 0;
}

int _dvar_or(D_VAR& result,const D_VAR& left,const D_VAR& right){
    if ( VE_BOOL != left.type || VE_BOOL != right.type ) {
        return 1;
    }
    result = right.var.bv || left.var.bv;
    return 0;
}

int _dvar_eq(D_VAR& result,const D_VAR& left,const D_VAR& right){
    unsigned int ltype = left.type;
    unsigned int rtype = right.type;

    if ( ltype != rtype ) {
        return 1;
    }

    switch (ltype)
    {
    case VE_BOOL:
        result = left.var.bv == right.var.bv;
        break;
    case VE_INT:
        result = left.var.iv == right.var.iv;
        break;
    case VE_FLT:
        result = left.var.dv == right.var.dv;
        break;
    default:
        return 1;
    }
    return 0;
}

int _dvar_ne(D_VAR& result,const D_VAR& left,const D_VAR& right){
    unsigned int ltype = left.type;
    unsigned int rtype = right.type;

    if ( ltype != rtype ) {
        return 1;
    }

    switch (ltype)
    {
    case VE_BOOL:
        result = left.var.bv != right.var.bv;
        break;
    case VE_INT:
        result = left.var.iv != right.var.iv;
        break;
    case VE_FLT:
        result = left.var.dv != right.var.dv;
        break;
    default:
        return 1;
    }

    return 0;
}

int _dvar_lt(D_VAR& result,const D_VAR& left,const D_VAR& right){
    unsigned int ltype = left.type;
    unsigned int rtype = right.type;

    if ( ltype != rtype ) {
        return 1;
    }

    switch (ltype)
    {
    case VE_INT:
        result = left.var.iv < right.var.iv;
        break;
    case VE_FLT:
        result = left.var.dv < right.var.dv;
        break;
    default:
        return 1;
    }

    return 0;
}

int _dvar_le(D_VAR& result,const D_VAR& left,const D_VAR& right){
    unsigned int ltype = left.type;
    unsigned int rtype = right.type;

    if ( ltype != rtype ) {
        return 1;
    }

    switch (ltype)
    {
    case VE_INT:
        result = left.var.iv <= right.var.iv;
        break;
    case VE_FLT:
        result = left.var.dv <= right.var.dv;
        break;
    default:
        return 1;
    }

    return 0;
}

int _dvar_gt(D_VAR& result,const D_VAR& left,const D_VAR& right){
    unsigned int ltype = left.type;
    unsigned int rtype = right.type;

    if ( ltype != rtype ) {
        return 1;
    }

    switch (ltype)
    {
    case VE_INT:
        result = left.var.iv > right.var.iv;
        break;
    case VE_FLT:
        result = left.var.dv > right.var.dv;
        break;
    default:
        return 1;
    }

    return 0;
}

int _dvar_ge(D_VAR& result,const D_VAR& left,const D_VAR& right){
    unsigned int ltype = left.type;
    unsigned int rtype = right.type;

    if ( ltype != rtype ) {
        return 1;
    }

    switch (ltype)
    {
    case VE_INT:
        result = left.var.iv >= right.var.iv;
        break;
    case VE_FLT:
        result = left.var.dv >= right.var.dv;
        break;
    default:
        return 1;
    }
    return 0;
}




_QD_END