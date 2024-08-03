#ifndef __QD_IO_LIB_H__
#define __QD_IO_LIB_H__

#include "qd_lib.h"

_QD_BEGIN


struct IO_LIB : _LIB
{
    IO_LIB();
    ~IO_LIB();

    void load_lib() override;

};


_QD_END


#endif
