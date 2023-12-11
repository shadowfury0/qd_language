#ifndef __QD_LEX_H__
#define __QD_LEX_H__

#include "qd_buffer.h"
#include "qd_io.h"
#include "qd_obj.h"


_QD_BEGIN

//定义关键字


static const char* const identity[] = {
    QD_KYW_IF,QD_KYW_ELIF,QD_KYW_ELSE,QD_KYW_TRUE,QD_KYW_FALSE,
    QD_KYW_JUMP,QD_KYW_PASS,QD_KYW_FOR,QD_KYW_WHILE,
    QD_KYW_GLOBAL,QD_KYW_LOCAL,QD_KYW_FUNC,QD_KYW_RET,QD_KYW_IN,
};

//全局函数判断第几个关键词
static inline int is_keyword(const char* c){
    int i = -1;
    int len = sizeof(identity) /sizeof(char*);
    for (i = 0; i <= len; i++)
    {
        if (i == len ){
            i = -1;
            break;
        }
        if(!strcmp(identity[i],c)) break;
    }
    return i; //词法与关键词位置对齐
}
//根据位置判断
static inline int is_keyword(int i){
    int len = sizeof(identity) /sizeof(char*);
    return i >= 0 && i < len ? i : -1;
}


enum TOK_TYPE {
    T_ERR = -2,
    T_EOF = -1,
/*   keyword  */
    T_IF,                   //if
    T_ELIF,                 //else if
    T_ELSE,                 //else
    T_TRUE,                 //true
    T_FALSE,                //false
    T_JUMP,                 //jump
    T_PASS,                 //pass
    T_FOR,                  //for
    T_WHILE,                //while
    T_GLOBAL,               //global
    T_LOCAL,                //local
    T_FUNC,                 //func
    T_RETURN,               //return
    T_IN,                   //in
/*   terminal    */
    T_NULL,                 //空值
    T_BLANK,                //空白
    T_END,                  //结束
    T_COLON,                //冒号
    T_COMMENT,              //注释
    T_COMMA,                //逗号
/*   variable    */
    T_INT,                  //整数
    T_DECIMAL,              //小数
    T_STRING,               //字符串
    T_UDATA,                //用户变量
/*   symbols    */
    T_EQ,                   //=
    T_DEQ,                  //==
    T_EXCLAMATION,          //!
    T_NEQ,                  //!=
    T_GN,                   // >
    T_GE,                   // >=
    T_LN,                   // <
    T_LE,                   // <=
    T_PLUS,                 //+
    T_MINUS,                //-
    T_MUL,                  //*
    T_MOD,                  //%
    T_DIV,                  // /
    T_AMPERSAND,            //&
    T_VERTICAL_BAR,         //|   
/*    others    */
    // T_DDIV,                 // //
    T_LPARENTH,             // (
    T_RPARENTH,             // )
    T_LBRACKET,             // [
    T_RBRACKET,             // ]
    T_LBRACE,               // {
    T_RBRACE,               // }
/*    auxiliary    */
    T_XMINUS,               // -
};


struct Token {
    short token; //词法解析的类别
    /* semantics information */

    Token();
    void init();
};

struct LexState
{
    unsigned char cur;   //当前token类型
    unsigned int _row;  //当前行
    unsigned int _col;         //当前列
    unsigned int lastline; //上一个token行数

    Token t;  //当前token类型
    Token lookahead; //前一个token类型
    Token prevhead;  // 上上一个token类型
    D_VAR dvar;//当前值记录

    Dbuffer* buff;//当前token字符

    Logger* logger;

    LexState();
    ~LexState();

    void init();
    //下一个字符
    void next();
    void prev();
    
    //解析每个字符
    int llex();

    //比较下一个字符是否为c字符
    bool check1_next (int c);
    //比较下两个字符是否是two
    bool check2_next (const char* two);

    bool isdigit(unsigned int c);
    bool islalpha(unsigned int c);
    bool isblank(unsigned int c);
    bool isline(unsigned int c);

    bool isnewline();
    //解析TOKEN
    int  read_numeral();
    void read_decimal();
    int  read_string(unsigned int c);
    void read_comment();
    
    void inclinenumber();

    //往前走找到不是空白符停止
    void prev_blank();
    //往前推数字
    void prev_number();
    //跳过空白符号
    void remove_blank();
    //跳过结束符
    void remove_line();

    //判断是否为关键字
    bool is_keyw(unsigned int tok);
    //判断是否为运算符
    bool is_operator(unsigned int tok);
    //判断是否为用户数据类型
    bool is_variable(unsigned int tok);

    void alloc_buff(const char* buff);
    void free_buff();
    // void alloc_io(const char* io);
};


_QD_END


#endif