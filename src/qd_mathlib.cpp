#include "qd_mathlib.h"


#include <math.h>

_QD_BEGIN


D_OBJ  MATH_LIB::power(const D_OBJ& n,const D_OBJ& m) {
    return std::pow(n.var.dv,m.var.dv);
}



_QD_END

