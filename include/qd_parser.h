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
    unsigned int statement(unsigned int& pos,FunState& fun);
    //赋值表达式
    unsigned int assign_expr(Instruction& inc,FunState& fun);
    //运算表达式
    unsigned int simple_expr(FunState& fun);

    //跳转表达式
    unsigned int jump_expr(FunState& fun);
    //数组元素赋值
    unsigned int array_element_expr(Instruction& inc,FunState& fun);
    //数组表达式
    unsigned int union_expr(bool global,const std::string& name,FunState& fun);
    //if表达式,返回值0运行正确
    unsigned int if_expr(FunState& func);
    //elif表达式,
    unsigned int elif_expr(FunState& func);
    //else表达式,
    unsigned int else_expr(FunState& func);
    //for表达式
    unsigned int for_expr(FunState& func);
    //while表达式
    unsigned int while_expr(FunState& func);
    //函数表达式
    unsigned int function_expr(FunState& func);
    //调用表达式
    unsigned int call_expr(std::string name,FunState& fun);
    //列表访问表达式
    unsigned int list_access_expr(const std::string& name,FunState& func);


    //解析指令集
    unsigned int analyse_code(unsigned int& pos,FunState& fun);
    //解析表达式指令集
    unsigned int analyse_expr(Instruction& inc,FunState& fun);
    //解析函数
    unsigned int analyse_func(Instruction& inc);
    //解析数组
    unsigned int analyse_array(Instruction& inc,const FunState& fun);
    //解析赋值表达式
    D_OBJ analyse_assign(Instruction& inc,FunState& fun);
    //联合体访问
    D_UNION union_access(int start, int  end,const D_UNION& arr);

    //返回计算数据类型
    unsigned int result_add(D_VAR& result,const D_VAR& left,const D_VAR& right);
    unsigned int result_sub(D_VAR& result,const D_VAR& left,const D_VAR& right);
    //浮点数不能整除则变为除法
    unsigned int result_mod(D_VAR& result,const D_VAR& left,const D_VAR& right);
    unsigned int result_mul(D_VAR& result,const D_VAR& left,const D_VAR& right);
    unsigned int result_div(D_VAR& result,const D_VAR& left,const D_VAR& right);
    unsigned int result_and(D_VAR& result,const D_VAR& left,const D_VAR& right);
    unsigned int result_or(D_VAR& result,const D_VAR& left,const D_VAR& right);
    unsigned int result_eq(D_VAR& result,const D_VAR& left,const D_VAR& right);
    unsigned int result_ne(D_VAR& result,const D_VAR& left,const D_VAR& right);
    unsigned int result_lt(D_VAR& result,const D_VAR& left,const D_VAR& right);
    unsigned int result_le(D_VAR& result,const D_VAR& left,const D_VAR& right);
    unsigned int result_gt(D_VAR& result,const D_VAR& left,const D_VAR& right);
    unsigned int result_ge(D_VAR& result,const D_VAR& left,const D_VAR& right);

    //打印单个数组
    void print_union(const std::string& name,std::map<std::string,D_OBJ>& variables);
    //打印数组
    void print_unions(std::map<std::string,D_OBJ>& variables);
    //打印变量
    void print_variable(const std::string& name,std::map<std::string,D_OBJ>& variables);
    //打印所有
    void print_variables(std::map<std::string,D_OBJ> lv);
    //当前代码第几行
    unsigned int source_code_row();
    //当前代码第几列
    unsigned int source_code_col();
    //返回当前解析代码行数
    unsigned int bytes_code_line();
    //返回当前全局变量个数
    unsigned int variable_count();


    //检查用户变量是否存在
    D_OBJ* variable_check(const std::string& name,const FunState& fun);
    //查找函数在哪一个位置，哪一层函
    FunState* function_check(const std::string& name,FunState* fun);

    FunState* function_stack_top();

    FunState* global;

    Logger* logger;

    LexState ls;

    std::vector<FunState*> funstack;
};


_QD_END


#endif