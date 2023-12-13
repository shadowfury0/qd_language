#ifndef __QD_PARSER_H__
#define __QD_PARSER_H__


#include "qd_instruction.h"
#include "qd_env.h"
#include "qd_lex.h"

_QD_BEGIN


struct DParser{
    
    DParser();
    ~DParser();

    //读取文件
    unsigned int read_file(const char* file);
    //读取一行
    unsigned int read_line(const char* line,unsigned int len);

    unsigned int parse();
    //解析函数
    unsigned int parse_Func(FunHead& fun);

    //一个个解析token
    unsigned int parseX_next();
    //判断操作符前缀
    int parse_PreCode(short type);
    //判断操作符,这个inc先保留万一有用呢
    unsigned int parse_Opr(Instruction& inc,short type);
    //跳过空白token
    unsigned int skip_blank();
    //跳过；或换行
    unsigned int skip_line();
    //下一个非空token
    unsigned int findX_next();
    //符号反转,目前是负数
    unsigned int symbol_reversal(Instruction& inc);

    //基本表达式
    unsigned int statement(FunHead& fun);
    //赋值表达式
    unsigned int assign_expr(Instruction& inc,FunHead& fun);
    //运算表达式
    unsigned int simple_expr(FunHead& fun);

    //跳转表达式
    unsigned int jump_expr(FunHead& fun);
    //数组元素赋值
    unsigned int array_element_expr(const std::string& name,FunHead& fun);
    //数组表达式
    unsigned int array_expr(const std::string& name,FunHead& fun);
    //if表达式,返回值0运行正确
    unsigned int if_expr(FunHead& func);
    //elif表达式,
    unsigned int elif_expr(FunHead& func);
    //else表达式,
    unsigned int else_expr(FunHead& func);
    //for表达式
    unsigned int for_expr(FunHead& func);
    //while表达式
    unsigned int while_expr(FunHead& func);
    //函数表达式
    unsigned int function_expr(FunHead& func);
    //调用表达式
    unsigned int call_expr(std::string name,FunHead& fun);
    //列表访问表达式
    unsigned int list_access_expr(const std::string& name,FunHead& func);

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

    //清空 Env
    void env_clear();
    
    Logger* logger;
    LexState* ls;

    std::vector<D_ENV*> env;
};


_QD_END


#endif