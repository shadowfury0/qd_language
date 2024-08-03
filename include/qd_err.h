#ifndef __QD_ERR0R_H__
#define __QD_ERR0R_H__

/*
    错误分类
*/

enum ERROR_TYPE {
    ET_OK = 0,
    ET_IO,         //文件io错误
    ET_PARSER,     //parser阶段错误
    ET_VM,         //vm阶段错误
    ET_LIB,        //本地库错误
};

//本地库错误
enum ERR_LIB {
    EL_OK  = 0,         //默认无错误
    EL_SYS = 1,         //系统错误
    EL_TYPE,            //类型错误
    EL_ARG_NUM,         //参数个数错误
    EL_ACE_OUT,         //访问越界
    EL_MATH,            //数学错误
};

//解析器错误
enum ERR_PARSE {
    EP_OK = 0,
    EP_SYS = 1,         //系统默认错误
    EP_NULL,            //空
    EP_OU_RANG,         //超出范围
    EP_FUN,             //函数错误
    EP_RET,             //返回值错误
    EP_TYPE,            //类型错误
    EP_LIB,             //本地库错误
    EP_OP,              //符号错误
    EP_IO,              //io错误
};

enum ERR_VM {
    EV_OK  = 0,
    EV_SYS = 1,         //默认错误
    EV_NULL,            //空
    EV_STK_OVER,        //调用栈溢出
    EV_LIB,             //本地调用错误
    EV_ACC_OUT,         //访问错误
    EV_TYPE,            //类型错误
    EV_MATH,            //数学错误
};


#endif

