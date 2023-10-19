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
    void statement();

    //检查token类型是否正确
    int variable_check(int ltype,int pretype);

    //前缀表达式
    unsigned int prefix_expr(int type,int pretype);
    //后缀表达式判断情况
    unsigned int suffix_expr(int type,int pretype);
    //运算,返回-1说明只有一个值
    unsigned int expr_stat(int type,int pretype);

    //解析指令集
    void analyse_code();

    //打印变量
    void print_variable(const std::string& name);
    //打印所有
    void print_variables();
    //当前代码第几行
    unsigned int source_code_line();
    //返回当前解析代码行数
    unsigned int bytes_code_line();
    //返回当前全局变量个数
    unsigned int variable_count();

// private:
    unsigned int code_pos = 0;
    unsigned int FIN_END = UINT32_MAX;
    LexState ls;
    FunState global;
};


_QD_END


#endif