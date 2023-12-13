#ifndef __QD_MAIN_F_H__
#define __QD_MAIN_F_H__

#include "qd_parser.h"
#include "qd_vm.h"

//程序运行模式目前只有交互式
_QD_BEGIN


class QDMAIN{
public:
    QDMAIN();
    ~QDMAIN();
    //函数主窗口
    int qd_main(int argc, char **argv);

    void interactive_mode();
    
private:
    Dio* io;
    D_VM* vm;
    DParser* parser;
    Logger* logger;
};


_QD_END


#endif