#ifndef __QD_MATH_LIB_H__
#define __QD_MATH_LIB_H__

#include "qd_lib.h"


_QD_BEGIN

struct MATH_LIB : _LIB
{
    MATH_LIB();
    //目前是浅拷贝
    ~MATH_LIB();

    void load_lib() override;
};

_QD_END


#endif

