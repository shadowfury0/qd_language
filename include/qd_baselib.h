#ifndef __QD_BASE_LIB_H__
#define __QD_BASE_LIB_H__

#include "qd_lib.h"

_QD_BEGIN


struct BASE_LIB : _LIB
{
    BASE_LIB();
    //目前是浅拷贝
    ~BASE_LIB();

    void load_lib() override;
};


_QD_END


#endif
