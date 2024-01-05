#ifndef __QD_MAIN_F_H__
#define __QD_MAIN_F_H__

#include "qd_parser.h"
#include "qd_vm.h"
#include "qd_bitset.h"

//程序运行模式目前只有交互式
_QD_BEGIN


class QDMAIN{
public:
    QDMAIN();
    ~QDMAIN();
    //函数主窗口
    size_t qd_main(int argc, char **argv);

    void parse_args(char* str);

    bool is_backslash(const std::string& s);
    size_t read_more(std::string& str);

    size_t interactive_mode();
    size_t script_mode();
    
private:
    Logger* logger;

    Dio* io;
    D_VM* vm;
    DParser* parser;
    D_State* state;

    D_LIB* lib;

    BITSET b;
};


_QD_END


#endif