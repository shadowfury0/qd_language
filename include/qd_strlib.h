#ifndef _QD_STR_LIB_H_
#define _QD_STR_LIB_H_

#include "qd_lib.h"

_QD_BEGIN


struct STR_LIB : _LIB
{
    STR_LIB();
    ~STR_LIB();

    void load_lib() override;

};


_QD_END


#endif
