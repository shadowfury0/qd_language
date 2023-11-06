#ifndef __QD_PARSER_H__
#define __QD_PARSER_H__

#include "qd_instruction.h"
#include "qd_func.h"
#include "qd_lex.h"

_QD_BEGIN


class DParser{
public:
    DParser();
    ~DParser();

    //读取文件
    void read_file(const char* file);
    //读取一行
    void read_line(const char* line);

    void parse(const char* str);
    //解析函数
    unsigned int parse_Func(FunState& fun);

    //一个个解析token
    void parseX_next();
    //判断操作符前缀
    int parse_PreCode(short type);
    //判断操作符
    void parse_Opr(Instruction& inc,short type);
    //跳过空白token
    void skip_blank();
    //跳过；或换行
    void skip_line();
    //下一个非空token
    void findX_next();

    //基本表达式
    //返回0正常退出
    unsigned int statement(FunState& fun,unsigned int& pos);

    //前缀表达式
    unsigned int prefix_expr(FunState& fun,int type);
    //后缀表达式判断情况
    unsigned int suffix_expr(int type,int pretype);
    //运算,返回-1说明只有一个值
    unsigned int expr_stat(FunState& fun,int type);
    //跳转表达式
    void jump_expr();
    //if表达式
    unsigned int if_expr(FunState& func);
    //for表达式
    unsigned int for_expr(FunState& func);
    //逻辑表达式
    unsigned int logic_expr(std::vector<Instruction>& codes);


    //解析指令集
    void analyse_code(FunState& fun,unsigned int& pos);
    //解析表达式指令集
    void analyse_expr(Instruction& inc,FunState& fun);
    //解析jump指令
    void analyse_jump(Instruction& inc,unsigned int& cpos);
    //解析函数
    void analyse_func(Instruction& inc);
    //解析逻辑表达式
    void analyse_logic(Instruction& inc,std::vector<Instruction>& codes,std::map<std::string,D_VAR>& variables);
    //解析赋值表达式
    void analyse_assign(Instruction& inc,std::vector<Instruction>& codes,std::map<std::string,D_VAR>& variables);

    //打印变量
    void print_variable(const std::string& name,std::map<std::string,D_VAR>& variables);
    //打印所有
    void print_variables(std::map<std::string,D_VAR> lv);
    //当前代码第几行
    unsigned int source_code_row();
    //当前代码第几列
    unsigned int source_code_col();
    //返回当前解析代码行数
    unsigned int bytes_code_line();
    //返回当前全局变量个数
    unsigned int variable_count();
    //检查用户变量是否存在
    bool variable_check(const FunState& fun);
    //返回查找的值，跟variable_check差不多为了区分而已,这个函数返回值不能使用引用
    D_VAR find_variable(const std::string& name,const FunState& fun);


    LexState ls;
    FunState global;
    
    Logger* logger;
};


_QD_END


#endif