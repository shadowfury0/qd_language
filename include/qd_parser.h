#ifndef __QD_PARSER_H__
#define __QD_PARSER_H__


#include "qd_instruction.h"
#include "qd_env.h"
#include "qd_lex.h"
#include "qd_state.h"
#include "qd_lib.h"

_QD_BEGIN


struct DParser{
    
    DParser();
    ~DParser();

    //读取文件
    size_t read_file(const char* file);
    //读取一行
    size_t read_line(const char* line,size_t len);

    size_t parse();
    //解析函数
    size_t parse_Func(FunHead& fun);

    //一个个解析token
    size_t parseX_next();
    //判断操作符,这个inc先保留万一有用呢
    size_t parse_Opr(Instruction& inc,short type);
    //跳过空白token
    size_t skip_blank();
    //跳过；或换行
    size_t skip_line();
    //下一个非空token
    size_t findX_next();
    //跳转到下一个语句
    size_t skip_to_end();
    //符号反转,目前是负数
    size_t symbol_reversal(Instruction& inc);

    /**
     * @brief 基本表达式
     * @return ERR_END 正常错误(暂时) 
     */
    size_t statement(FunHead& fun);
    //赋值表达式
    size_t assign_expr(Instruction& inc,FunHead& fun);
    //运算表达式
    size_t simple_expr(FunHead& fun);

    //跳转表达式
    size_t jump_expr(FunHead& fun);
    //数组元素赋值
    size_t array_element_expr(const std::string& name,FunHead& fun);
    //联合赋值表达式
    size_t union_expr(const std::string& name,FunHead& fun);
    //数组赋值表达式
    size_t array_expr(const std::string& name,FunHead& fun);
    //数组运算表达式
    size_t array_opr(Instruction& inc,FunHead& fun);
    //if表达式,返回值0运行正确
    size_t if_expr(FunHead& func);
    //elif表达式,
    size_t elif_expr(FunHead& func);
    //else表达式,
    size_t else_expr(FunHead& func);
    //for表达式
    size_t for_expr(FunHead& func);
    //while表达式
    size_t while_expr(FunHead& func);
    //函数表达式
    size_t function_expr(FunHead& func);
    //调用表达式
    size_t call_expr(std::string name,FunHead& fun);
    // 库表达式 , 当前函数token为 句号.
    size_t lib_expr(FunHead& fun);

    //列表访问表达式
    size_t list_access_expr(const std::string& name,FunHead& func);

    D_UNION union_access(int start, int  end,const D_UNION& arr);

    //检查用户变量是否存在
    D_VAR* variable_check(const std::string& name,D_ENV* fun);

    FunHead* find_function(const std::string& name,D_ENV* fun);

    D_ENV* env_stack_top();
    //上一级环境
    D_ENV* last_env(D_ENV* info);
    D_ENV* env_stack_head();
    //初始化输入流
    void init_io(Dio* const io);
    size_t io_size();

    //清空 Env
    void env_clear();
    //加载库,返回0加载成功
    size_t init_lib(D_LIB* lib);
    
    Logger* logger;
    LexState* ls;
    // D_State* state;

    std::vector<D_ENV*> env;
    D_LIB* lib;
};


_QD_END


#endif