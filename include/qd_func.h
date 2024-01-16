#ifndef __QD_FUNC_H__
#define __QD_FUNC_H__

#include "qd_lex.h"
#include "qd_instruction.h"
#include "qd_state.h"

_QD_BEGIN


//函数头
struct FunHead {

    FunHead();
    FunHead(const FunHead& head);
    ~FunHead();

    void clear();
    size_t args_size();

    void state_push_var(const D_VAR& var);
    void set_state_pos(const size_t& i);
    size_t state_var_size();
    size_t state_var_pos();

    //内部库状态
    D_State* state;

    //函数参数,这个可以不需要？？
    std::vector<std::string> args;
    //匿名内部函数，如if  for
    std::vector<FunHead*>  lfuns;
    std::vector<Instruction>  codes;      //指令集

};

//函数整体部分



_QD_END


#endif