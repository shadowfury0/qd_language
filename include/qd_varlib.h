#ifndef __QD_VAR_LIB_H__
#define __QD_VAR_LIB_H__

#include "qd_header.h"
#include "qd_assign.h"

_QD_BEGIN


// 0 correct  1 error 
QD_API int _dvar_add(D_VAR& result,const D_VAR& left,const D_VAR& right);
QD_API int _dvar_sub(D_VAR& result,const D_VAR& left,const D_VAR& right);
QD_API int _dvar_mul(D_VAR& result,const D_VAR& left,const D_VAR& right);
QD_API int _dvar_mod(D_VAR& result,const D_VAR& left,const D_VAR& right);
QD_API int _dvar_div(D_VAR& result,const D_VAR& left,const D_VAR& right);
QD_API int _dvar_and(D_VAR& result,const D_VAR& left,const D_VAR& right);
QD_API int _dvar_or(D_VAR& result,const D_VAR& left,const D_VAR& right);
QD_API int _dvar_eq(D_VAR& result,const D_VAR& left,const D_VAR& right);
QD_API int _dvar_ne(D_VAR& result,const D_VAR& left,const D_VAR& right);
QD_API int _dvar_lt(D_VAR& result,const D_VAR& left,const D_VAR& right);
QD_API int _dvar_le(D_VAR& result,const D_VAR& left,const D_VAR& right);
QD_API int _dvar_gt(D_VAR& result,const D_VAR& left,const D_VAR& right);
QD_API int _dvar_ge(D_VAR& result,const D_VAR& left,const D_VAR& right);


#define D_VAR_ADD _dvar_add
#define D_VAR_SUB _dvar_sub
#define D_VAR_MUL _dvar_mul
#define D_VAR_MOD _dvar_mod
#define D_VAR_DIV _dvar_div
#define D_VAR_AND _dvar_and
#define D_VAR_OR  _dvar_or
#define D_VAR_EQ  _dvar_eq
#define D_VAR_NE  _dvar_ne
#define D_VAR_LT  _dvar_lt
#define D_VAR_LE  _dvar_le
#define D_VAR_GT  _dvar_gt
#define D_VAR_GE  _dvar_ge


_QD_END


#endif