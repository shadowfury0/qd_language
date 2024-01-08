#ifndef _QD_IO_LIB_H_
#define _QD_IO_LIB_H_

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
