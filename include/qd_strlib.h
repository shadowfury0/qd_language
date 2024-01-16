#ifndef __QD_STR_LIB_H__
#define __QD_STR_LIB_H__

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
