#include "qd_parser.h"


_QD_BEGIN


DParser::DParser(){
    logger = Logger::getInstance();
}

DParser::~DParser(){
    
}

unsigned int DParser::parseX_next(){
    if ( ls.t.token != T_BLANK ) {
        this->ls.lookahead.token = this->ls.t.token;
    }
    if ( T_ERR == (this->ls.t.token = ls.llex()) ) {
        logger->error("lexical error");
        return ERR_END;
    }
    
    return 0;
}

void DParser::parse(const char* str){
    ls.alloc_buff(str);

    //这里开始全局解析
    unsigned int err = parse_Func(global);

    //错误的检测
    if (!err) {
        //只用解析一次就好了
        err = analyse_code(global.code_pos,global);
        if (err) {
            logger->error("anaylsing code error ");
        }
    }
    else {
        logger->error("compiling code error ");
    }

    ls.free_buff();
    
}

unsigned int DParser::parse_Func(FunState& fun){
    for(;;){
        this->findX_next();
        if( T_EOF == ls.t.token ) {
            logger->debug(" <------------------ end --------------------> ");
            break;//结束
        }
        else if ( T_PASS == ls.t.token ) {
            //函数体到达结尾
            break;
        }
        
        //判断是哪个表达式
        switch (ls.t.token)
        {
        case T_COMMENT:{
            logger->info(" comment line ");
            break;
        }
        case T_UDATA:
        case T_GLOBAL:{
            if(statement(fun.code_pos,fun)) {
                logger->error("statement error");
                return ERR_END;
            }
            break;
        }
        // case T_JUMP:{
        //     //跳转指令
        //     jump_expr();
        //     break;
        // }
        case T_IF:{
            if( if_expr(fun) ){
                logger->error("if statement error");
                return ERR_END;
            }
            //函数应该结束
            break;
        }
        case T_FOR:{
            if ( for_expr(fun) ) {
                logger->error("for statement error");
                //错误处理
                return ERR_END;
            }
            break;
        }
        case T_FUNC:{
            if ( function_expr(fun) ) {
                logger->error("function statement error");
                //错误处理
                return ERR_END;
            }
            break;
        }
        case T_RETURN:{
            break;
        }
        default:
            logger->info("do nothing  ");
            break;
        }  
    }
    return 0;
}

unsigned int DParser::skip_blank(){
    if ( T_BLANK == ls.t.token )
    {
        this->parseX_next();
    }
    return 0;
}

unsigned int DParser::skip_line() {
    if ( T_END == ls.t.token )
    {
        this->parseX_next();
    }
    return 0;
}

unsigned int DParser::findX_next(){
    this->parseX_next();
    this->skip_blank();
    return 0;
}

int DParser::parse_PreCode(short type){
    switch (type)
    {
    case T_MINUS:
        return OC_MINUS;
    default:
        return OC_NIL;
    }
}

void DParser::parse_Opr(Instruction& inc,short type){
    //判断操作符
    switch (type)
    {
    case T_PLUS:
        inc.type = OC_ADD;
        break;
    case T_MINUS:
        inc.type = OC_SUB;
        break;
    case T_MUL:
        inc.type = OC_MUL;
        break;
    case T_DIV:
        inc.type = OC_DIV;
        break;
    case T_MOD:
        inc.type = OC_MOD;
        break;
    // case T_DIV:
    //     inc.type = OC_IDIV;
    //     break;
    case T_AMPERSAND:
        inc.type = OC_AND;
        break;
    case T_VERTICAL_BAR:
        inc.type = OC_OR;
        break;
    case T_DEQ :
        inc.type = OC_DEQ;
        break;
    case T_NEQ:
        inc.type = OC_NEQ;
        break;
    case T_LE:
        inc.type = OC_LE;
        break;
    case T_LN:
        inc.type = OC_LT;
        break;
    case T_GE:
        inc.type = OC_GE;
        break;
    case T_GN:
        inc.type = OC_GT;
        break;
    default:
        inc.type = OC_NULL;
        break;
    }
}

unsigned int DParser::statement(unsigned int& pos,FunState& fun){
    Instruction inc;

    //全局变量
    if ( T_GLOBAL == ls.t.token ) {
        inc.res = true;
        findX_next();
    }

    if ( T_UDATA != ls.t.token ) {
        logger->error(ls._row,":",ls._col," data error");
        return ERR_END;
    }

    //记录左值
    inc.left = ls.dvar.var.chv;

    findX_next();
    //下一个如果是函数调用的话
    if ( T_LPARENTH == ls.t.token ) {
        inc.type = OC_CALL;
        //这个返回值要进行更改一下
        inc.rpos = 0;
        D_VAR tmpvar = variable_check(inc.left.var.chv,fun,inc.rpos);
        if ( VE_VOID == tmpvar.type ) {
            logger->error(ls._row,":",ls._col," function name undefine ");
            return ERR_END;
        }
        inc.lpos = tmpvar.var.iv;

        inc.curpos = fun.codes.size();
        inc.res = 0;
        fun.codes.push_back(inc);
        findX_next();
        //这里需要修改
        if (T_RPARENTH == ls.t.token ) {
            return 0;
        }
    }
    //数组下标赋值
    else if ( T_LBRACKET == ls.t.token ) {
        inc.type = OC_ARR_PASSIGN;

        if(array_element_expr(inc,fun)){
            logger->error(ls._row,":",ls._col," array element assign error");
            return ERR_END;
        }
        return 0;
    }
    //因为是赋值表达式所以判断=
    else if ( T_EQ != ls.t.token ) {
        logger->error(ls._row,":",ls._col," unkonwn expression error ");
        return ERR_END;
    }
    
    inc.type = OC_ASSIGN;
    findX_next();

    //数组
    if ( T_LBRACE == ls.t.token ) {
        if(array_expr(inc.left.var.chv,fun)){
            logger->error(ls._row,":",ls._col," array parse error");
            return ERR_END;
        }
        return 0;
    }
    

    if(assign_expr(inc,fun)){
        logger->error(ls._row,":",ls._col," assign expression error");
        return ERR_END;
    }

    return 0;//正常退出
}

unsigned int DParser::assign_expr(Instruction& inc,FunState& fun){
    short assigntype;
    if ( T_MINUS == ls.t.token ) {
        //do nothing
    }
    else if ( VE_USER == ls.dvar.type ) {
        assigntype = variable_check(ls.dvar.var.chv,fun).type;
        //判断是否为数组，
        if ( array_check(ls.dvar.var.chv) ) {
            if ( list_access_expr(inc.left.var.chv,fun) ) {
                logger->error("list access expression error");
                return ERR_END;
            }
            return 0;
        }
        else {
            logger->error(ls._row,":",ls._col," prefix user data undefine");
            return ERR_END;
        }
    }
    else {
        assigntype = ls.dvar.type;
    }
    
    switch (assigntype)
    {
    case VE_STR: {
        Instruction tmp;
        tmp.left = ls.dvar;
        tmp.curpos = fun.codes.size();
        fun.codes.push_back(tmp);
        this->findX_next();
        inc.rpos = tmp.curpos;
        break;
    }
    case VE_FUNC:{
        //判断返回值是否为空
        break;
    } 
    default:{
        inc.rpos = this->prefix_expr(0,fun);
        if ( ERR_END == inc.rpos ) {
            logger->error("basic expression error");
            return ERR_END;
        }
        fun.proto->lv[inc.left.var.chv] = 0;
        fun.proto->lv[inc.left.var.chv].type = VE_NULL;

        break;
    }
    }
    //先把user_data添加进行进行判断

    if ( T_END != ls.t.token ){
        logger->error(ls._row,":",ls._col," error end");
        return ERR_END;
    }

    inc.curpos = fun.codes.size();
    fun.codes.push_back(inc);
    return 0;
}

void DParser::jump_expr(){
    logger->debug("jumping to ");
    findX_next();
    //不是int类型
    if ( T_INT != ls.t.token ) {
        logger->error("jumping num  type is error");
        findX_next();
        return;
    }
    //超出范围
    if ( ls.dvar.var.iv < 0) {
        logger->warn(" jump out of range ");
        findX_next();
        return;
    }
    
    findX_next();
    //加入指令
    Instruction inc;
    inc.type = OC_JMP;
    inc.lpos = ls.dvar.var.iv; 
    inc.res = true;//跳转
    inc.curpos = bytes_code_line();
    global.codes.push_back(inc);
}

unsigned int DParser::if_expr(FunState& func){
    if ( T_PASS == ls.t.token ) {
        return 0;
    }
    Instruction inc;

    findX_next();
    //判断跳转函数是否执行
    inc.rpos = prefix_expr(0,func);

    inc.curpos = func.codes.size();
    //记录跳转的函数是第几个
    inc.lpos = func.proto->lfuns.size(); 
    inc.type = OC_IF;      //调用

    func.codes.push_back(inc);

    //判断结束符号是否正确
    if (T_COLON != ls.t.token) {
        logger->error("if condition judgment error");
        return ERR_END;
    }
    
    findX_next();//这里通常都是 line 换行符


    FunState* ifstate = new FunState();
    ifstate->prev = &func;
    parse_Func(*ifstate);
    func.proto->lfuns.push_back(ifstate);
    
    if ( T_PASS != ls.t.token) {
        logger->error("missing pass end in if statement");
        return ERR_END;
    }
    //pass
    findX_next();

    return 0;   //正常退出
}

unsigned int DParser::for_expr(FunState& func){
    if ( T_PASS == ls.t.token ) {
        return 0;
    }
    
    Instruction inc;
    findX_next();
    
    int ret = ls.t.token;

    D_VAR iter = ls.dvar;
    //判断该元素是否迭代
    if ( T_UDATA == ret && VE_USER == ls.dvar.type) {
        unsigned int tmptype = variable_check(ls.dvar.var.chv,func).var.iv;
        //用户变量另作处理
        if ( tmptype == VE_VOID ) {
            logger->error(ls._row,":",ls._col," user data undefine");
            return ERR_END;
        }
        else {
            //判断迭代类型
            iter = tmptype;
        }
    }
    
    Instruction jump;
    jump.type = OC_JMP;
    jump.lpos = 0;

    //迭代数据类型判断
    if ( T_INT == ret ) {
        //第一次进入函数已经调用过一次了
        int jumloop = iter.var.iv - 1;
        jump.res = true;
        //判断for语句是否要执行
        inc.res = jumloop < 0 ? 0 : jumloop;
    }

    inc.curpos = func.codes.size();
    //记录跳转的函数是第几个
    inc.lpos = func.proto->lfuns.size(); 
    inc.type = OC_FOR;      //调用

    func.codes.push_back(inc);

    findX_next();

    //判断结束符号是否正确
    if (T_COLON != ls.t.token) {
        logger->error("for condition judgment error");
        return ERR_END;
    }
    //line
    findX_next();

    FunState* forstate = new FunState();
    forstate->prev = &func;
    parse_Func(*forstate);
    func.proto->lfuns.push_back(forstate);
    func.proto->lfuns[inc.lpos]->codes.push_back(jump);
    
    if ( T_PASS != ls.t.token) {
        logger->error("missing pass end in for statement");
        return ERR_END;
    }
    //pass
    findX_next();
    
    return 0;//正常退出
}

unsigned int DParser::array_element_expr(Instruction& inc,FunState& fun){
    if ( !array_check(inc.left.var.chv) ){
        logger->error(ls._row,":",ls._col," array is not exist");
        return ERR_END;
    }
    //[
    this->findX_next();
    //数字
    inc.lpos = ls.dvar.var.iv;
    this->findX_next();
    //]
    if ( T_RBRACKET != ls.t.token ) {
        logger->error(ls._row,":",ls._col," right bracket missing ");
        return ERR_END;
    }
    this->findX_next();
    if ( T_EQ != ls.t.token ){
        logger->error(ls._row,":",ls._col," unkonwn expression error ");
        return ERR_END;
    }
    this->findX_next();

    //rpos,left
    if( assign_expr(inc,fun) ){
        logger->error(ls._row,":",ls._col," assign expression error");
        return ERR_END;
    }

    return 0;
}

unsigned int DParser::function_expr(FunState& func){
    if ( T_PASS == ls.t.token ) {
        return 0;
    }
    
    findX_next();

    if ( T_UDATA != ls.t.token ) {
        logger->error("error function name ");
        return ERR_END;
    }
    std::string funcname = ls.dvar.var.chv;

    //预定义函数名称，可以使用递归
    func.proto->lv[funcname] = 0;
    func.proto->lv[funcname].type = VE_NULL;

    parseX_next();
    if ( T_LPARENTH != ls.t.token ) {   
        return ERR_END;
    }
    findX_next();
    
    
    for (;;)
    {
        if ( T_RPARENTH == ls.t.token ) {
            findX_next();
            break;    
        }
        else if ( T_UDATA != ls.t.token ) {
            return ERR_END;
        }
        //添加变量
        func.proto->lv[ls.dvar.var.chv] = 0;

        findX_next();
        if ( T_RPARENTH == ls.t.token ) {
            findX_next();
            break;    
        }
        else if ( T_COMMA != ls.t.token ) {
            logger->error("missing comma in function");
            return ERR_END;
        }
        findX_next();
    }
    
    if ( T_COLON != ls.t.token ) {
        logger->error("error ending function statement ");
        return ERR_END;
    }

    findX_next(); // end
    //添加当前函数进入到function里面去
    FunState* function = new FunState();
    function->prev = &func;
    parse_Func(*function);

    //添加return命令
    Instruction inc;
    inc.type = OC_RET;
    function->codes.push_back(inc);

    func.proto->lv[funcname] = (unsigned int)func.proto->lfuns.size();
    func.proto->lv[funcname].type = VE_FUNC;
    func.proto->lfuns.push_back(function);

    if ( T_PASS != ls.t.token ) {
        logger->error("missing pass end in function statement");
        return ERR_END;
    }
    return 0;
}

unsigned int DParser::array_expr(const std::string& name,FunState& fun){
    unsigned int lastpos = FIN_END;
    Instruction assign;
    for (;;) {
        Instruction inc;

        this->findX_next();
        if ( T_EOF == ls.t.token ) {
            logger->error("parser end error");
            return ERR_END;
        }
        else if ( T_END == ls.t.token ) {
            logger->error(ls._row,":",ls._col," missing right brace ");
            return ERR_END;        
        }
        
        inc.left = ls.dvar;
        inc.rpos = lastpos;
        inc.type = OC_ARR_VAL;
        lastpos = inc.curpos = fun.codes.size();
        fun.codes.push_back(inc);
        
        this->findX_next();

        if ( T_RBRACE == ls.t.token ) {
            //正常退出
            break;
        }
        else if ( T_COMMA != ls.t.token ) {
            logger->error(ls._row,":",ls._col," missing comma in array ");
            return ERR_END;
        }

    }
    
    assign.left = name.c_str();
    assign.type = OC_ARR_ASSIGN;
    assign.rpos = lastpos;
    assign.curpos = fun.codes.size();
    fun.codes.push_back(assign);

    this->arrays[name] = {};
    return 0;
}

unsigned int DParser::list_access_expr(const std::string& name,FunState& func){
    this->findX_next();
    std::string varname = ls.dvar.var.chv;
    if ( T_END == ls.t.token ) {
        logger->error("array not allow copy assign now");
        // print_array(name);
        return ERR_END;
    }
    //列表访问中间不允许空格
    else  if ( T_LBRACKET != ls.t.token ) {
        logger->error(ls._row,":",ls._col," list access expression error");
        return ERR_END;
    }

    this->findX_next();

    int ret = parse_PreCode(ls.t.token);
    if (OC_NIL != ret){
        findX_next();
    }
    
    D_VAR left = 0;
    if ( T_INT == ls.t.token ) {
        left = ret == OC_MINUS ?  -ls.dvar.var.iv :  ls.dvar.var.iv ;
        this->findX_next();
    }
    else if ( T_UDATA == ls.t.token ) {
        left = ls.dvar;
        this->findX_next();
    }

    //如果只有一个负数符号
    if ( ( OC_MINUS == ret && T_INT == ls.lookahead.token ) 
      || ( T_UDATA != ls.lookahead.token && T_INT != ls.lookahead.token ) ) {
        logger->error(ls._row,":",ls._col," missing int value with minus symbol");
        return ERR_END;
    }
    
    //如果访问一个下标
    if ( T_RBRACKET == ls.t.token ) {
        
        Instruction inc;
        inc.type = OC_ARR_ACESS;
        inc.left = varname.c_str();
        inc.right = left;
        inc.curpos = func.codes.size();
        func.codes.push_back(inc);
        
        Instruction assign;
        assign.left = name.c_str();
        assign.type = OC_ASSIGN;
        assign.rpos = inc.curpos;
        assign.curpos = func.codes.size();
        func.codes.push_back(assign);

        this->findX_next();

        return 0;
    }
    else if ( T_COLON != ls.t.token ) {
        logger->error(ls._row,":",ls._col," is not colon symbol");
        return ERR_END;
    }
    
    this->findX_next();

    ret = parse_PreCode(ls.t.token);
    if (OC_NIL != ret){
        findX_next();
    }
    
    D_VAR right = QD_INT_32_MAX;

    if ( T_INT == ls.t.token ) {
        right = ret == OC_MINUS ?  -ls.dvar.var.iv :  ls.dvar.var.iv;
        this->findX_next();
    }
    else if ( T_UDATA == ls.t.token ) {
        right = ls.dvar;
        this->findX_next();
    }

    if ( ( OC_MINUS == ret && T_INT != ls.lookahead.token ) 
    || ( T_UDATA != ls.lookahead.token && T_INT != ls.lookahead.token ) ) {
        logger->error(ls._row,":",ls._col," missing int value with minus symbol");
        return ERR_END;
    }

    if ( T_RBRACKET != ls.t.token ) {
        logger->error(ls._row,":",ls._col," missing right brace symbol");
        return ERR_END;
    }

    Instruction alist;
    alist.type = OC_ARR_LIST;
    alist.left = left;
    alist.right = right;
    alist.res = varname.c_str();
    alist.curpos = func.codes.size();
    func.codes.push_back(alist);

    Instruction assign;
    assign.type = OC_ARR_LASSIGN;
    assign.left = name.c_str();
    assign.rpos = alist.curpos;
    assign.curpos = func.codes.size();
    func.codes.push_back(assign);

    this->findX_next();
    this->arrays[name] = {};

    return 0;
}

unsigned int DParser::expr_stat(int ltype,FunState& fun){
    if ( 
       T_INT != ls.t.token && T_UDATA != ls.t.token 
    && T_DECIMAL != ls.t.token && T_MINUS != ls.t.token 
    && T_TRUE != ls.t.token && T_FALSE != ls.t.token ) {
        logger->error(ls._row,":",ls._col," express_stat type error  ");
        return ERR_END;
    }
    else if ( 
       T_END == ls.t.token 
    || T_LPARENTH == ls.t.token 
    || T_COLON == ls.t.token ) {
        return FIN_END;
    }

    //如果用户变量定义
    if ( T_UDATA == ls.t.token && this->variable_check(ls.dvar.var.chv,fun).var.iv == VE_VOID) {
        logger->error(ls._row,":",ls._col," user data undefine");
        return ERR_END;
    }

    Instruction inc;
    inc.ltype = ltype;

    int ret = parse_PreCode(ls.t.token);
    if (OC_NIL != ret){
        findX_next();
    }
    inc.pretype = ret;

    inc.left = ls.dvar;
    findX_next();
    //判断运算符
    parse_Opr(inc,ls.t.token);

    if ( T_END == ls.t.token 
    || priority[inc.type][1] < priority[inc.ltype][0] 
    || T_COLON == ls.t.token ) {
        inc.res = inc.left;
        inc.type = OC_NULL;
        inc.curpos = fun.codes.size();
        fun.codes.push_back(inc);
        return inc.curpos;
    }
    findX_next();


    ret =  expr_stat(inc.type,fun);
    if (ERR_END == ret) {
        return ERR_END;
    }

    inc.rpos = ret;    

    inc.curpos = fun.codes.size();
    fun.codes.push_back(inc);

    return inc.curpos;
}

unsigned int DParser::prefix_expr(int type,FunState& fun){
    unsigned int ret = expr_stat(type,fun);

    if ( T_END == ls.t.token || T_COLON == ls.t.token ) {
        return ret;
    }

    for (;;) {
        if (ERR_END == ret || T_EOF == ls.t.token) {
            logger->error("prefix expression error ");
            return ERR_END;
        }
        Instruction inc;

        inc.lpos = ret;
        parse_Opr(inc,ls.t.token);
        findX_next();
        
        ret = expr_stat(inc.type,fun);
        
        if ( ERR_END == ret) {
            logger->error("prefix expression expr_stat error");
            return ERR_END;
        }

        inc.rpos = ret;
        inc.curpos = fun.codes.size();
        fun.codes.push_back(inc);
        
        ret = inc.curpos;

        if ( T_END == ls.t.token || T_COLON == ls.t.token ) {
            return ret;
        }
        //表达式解析到最后
        // if ( T_END ==  ls.t.token || ls.t.token == T_RPARENTH || ls.t.token == T_LPARENTH ){
        //     break;
        // }
    }  

    return ERR_END;
}

unsigned int DParser::suffix_expr(int type,int pretype){ 
    //  unsigned int est = FIN_END;
    // int pretmp;
    // Instruction inc;
    // inc.ltype = type;

    // switch (ls.t.token)
    // {
    // case T_LPARENTH:{//括号在前
    //     inc.pretype = pretype;

    //     findX_next();
    //     pretmp = parse_PreCode(ls.t.token);
    //     if (OC_NIL != pretmp){
    //         findX_next();
    //     }

    //     inc.lpos = this->suffix_expr(0,pretmp);
        
    //     //只有一个
    //     if ( FIN_END == inc.lpos ) {
    //         inc.left = ls.dvar;
    //     }

    //     if ( T_RPARENTH == ls.t.token ) {
    //         findX_next();
    //         //已经结束
    //         if ( T_END == ls.t.token ) {
    //             //如果是一个表达式
    //             return inc.lpos;
    //         } 
            
    //         //嵌套
    //         if (T_RPARENTH == ls.t.token) {
    //             //如果是)返回出去
    //             return inc.lpos;
    //         }
            
    //         parse_Opr(inc,ls.t.token);
            
    //         //运算符优先级小
    //         if ( priority[inc.ltype][0] > priority[inc.type][1] ) {
    //             return inc.lpos;
    //         }
    //         findX_next();

    //         pretmp = parse_PreCode(ls.t.token);
    //         if (OC_NIL != pretmp){
    //             findX_next();
    //         }
    //         //紧挨着下一个()
    //         //例如 () * ()
    //         if (T_LPARENTH == ls.t.token) {
    //             inc.rpos = suffix_expr(inc.type,pretmp);
    //             inc.right = ls.dvar;
    //             //判断最后一个是不是()
    //             if ( T_RPARENTH == ls.lookahead.token) inc.sufftype = pretmp;
                
    //             inc.curpos = global.codes.size();
    //             global.codes.push_back(inc);
    //             return inc.curpos;
    //         }

    //         //-----------------------------------------------这里需要更改
    //         //临时变量
    //         // inc.rpos = expr_stat(inc.type,pretmp);

    //         if (FIN_END == inc.rpos ) {
    //             inc.right = ls.dvar;
    //             inc.sufftype = pretmp;
    //         }
    //         inc.curpos = global.codes.size();
    //         global.codes.push_back(inc);

    //         //------------------上面expr
    //         inc.lpos = inc.curpos;
    //         parse_Opr(inc,ls.t.token);
    //         inc.pretype = inc.sufftype = OC_NIL;//需要清空

    //         if ( OC_NULL == inc.type ) {
    //             return  inc.curpos;
    //         }
    //         findX_next();

    //         pretmp = parse_PreCode(ls.t.token);
    //         if (OC_NIL != pretmp){
    //             findX_next();
    //         }

    //         inc.rpos = this->suffix_expr(0,pretmp);
    //         if (FIN_END == inc.rpos ) {
    //             inc.right = ls.dvar;
    //             inc.sufftype = pretmp;
    //         }            

    //         inc.curpos = global.codes.size();
    //         global.codes.push_back(inc);
    //         return inc.curpos;
    //     } 
    //     else {
    //         //错误处理
    //     }

    //     return FIN_END;
    // }
    // default:{//括号在后
    //     // est = prefix_expr(type,pretype);
    //     if ( T_LPARENTH == ls.t.token ) {
    //         parse_Opr(inc,ls.lookahead.token);
    //         //这里判断优先级
    //         if ( priority[global.codes[est].ltype][0] > priority[inc.type][1]  ) {
    //             global.codes.pop_back();
    //             inc.rpos = this->suffix_expr(inc.type,pretmp);
    //             inc.lpos = est - 1;
    //             inc.curpos = global.codes.size();
    //             global.codes.push_back(inc);
    //             return inc.curpos;
    //         }else{
    //             //错误处理
    //         }
    //     }
    //     else {
    //         //错误处理
    //     }
    //     return est; 
    // }
    // }
}

void DParser::print_variable(const std::string& name,std::map<std::string,D_VAR>& variables){
    std::cout << name ;
    const D_VAR& dv = variables[name.c_str()];
    switch (dv.type)
    {
    case VE_INT:
        std::cout << "  " << dv.var.iv << " |  type  int   "   << std::endl;
        break;
    case VE_FLT:
        std::cout << "  " << dv.var.dv << " |  type  double    "   << std::endl;
        break;
    case VE_BOOL:
        std::cout << "  " << dv.var.bv << " |  type  bool    "   << std::endl;
        break;
    case VE_STR:
    case VE_USER:
        std::cout <<"  " <<  dv.var.chv << " |  type  string    "   << std::endl;
        break;
    default:
        std::cout << "  variables  no  found " << std::endl;
        return;
    }
}

void DParser::print_variables(std::map<std::string,D_VAR> lv){
    //打印全局变量
    for (auto i: lv){
        print_variable(i.first,lv);
    }
}

void DParser::print_array(const std::string& name){
    std::vector<D_VAR>& vec = this->arrays[name].larr;
    std::cout <<  name << "  |  ";
    for (std::vector<D_VAR>::iterator ater = vec.begin() ; ater != vec.end() ; ater ++) {
        switch (ater->type)
        {
        case VE_INT:
            std::cout << ater->var.iv << " ";
            break;
        case VE_FLT:
            std::cout << ater->var.dv << " ";
            break;
        case VE_STR:
            std::cout << ater->var.chv << " ";
            break;
        case VE_BOOL:
            std::cout << ater->var.bv << " ";
            break;
        }
    }
    std::cout << std::endl;
}

void DParser::print_arrays(){
    for (std::map<std::string, D_ARRAY>::iterator
         iter = this->arrays.begin() ; 
         iter != this->arrays.end(); iter++) {
        
        print_array(iter->first);
        std::cout << std::endl;
    }
}

unsigned int DParser::source_code_row(){
    return ls._row;
}

unsigned int DParser::source_code_col(){
    return ls._col;
}

unsigned int DParser::bytes_code_line(){
    return global.codes.size();
}

unsigned int DParser::variable_count(){
    return global.proto->lv.size();
}

bool DParser::array_check(const std::string& name){
    if ( arrays.find(name) != arrays.end() ) {
        return true;
    }
    else {
        return false;
    }
}

D_VAR DParser::variable_check(const std::string& name,const FunState& fun){
    if ( fun.proto->lv.find(name) != fun.proto->lv.end() ) {
        // if ( VE_NULL != fun.proto->lv[name].type ) {
            return fun.proto->lv[name];
        // }
    }

    FunState* tmpfunc = fun.prev;

    while ( tmpfunc )
    {
        if (tmpfunc->proto->lv.find(name) != tmpfunc->proto->lv.end() ) {
            // if ( VE_NULL != fun.proto->lv[name].type ) {
                return tmpfunc->proto->lv[name];
            // }
        }
        tmpfunc = tmpfunc->prev;
    }
    return D_VAR();
}

D_VAR DParser::variable_check(const std::string& name,const FunState& fun,unsigned int& recurse){
    D_VAR tmp;
    if ( fun.proto->lv.find(name) != fun.proto->lv.end() ) {
        return fun.proto->lv[name];
    }

    FunState* tmpfunc = fun.prev;
    unsigned int cur = recurse;
    while ( tmpfunc )
    {
        cur++;
        if ( tmpfunc->proto->lv.find(name) != tmpfunc->proto->lv.end() ) {
            recurse = cur;
            return tmpfunc->proto->lv[name];
        }
        tmpfunc = tmpfunc->prev;
    }
    recurse = cur;
    return D_VAR();
}

D_ARRAY DParser::list_access(int start, int end,const D_ARRAY& arr){
    D_ARRAY tmp;
    int i ;
    int len = arr.larr.size();
    if ( start < 0 ) {
        start = len + start + 1;
    }
    if ( end < 0 ) {
        end = len + end + 1;
    }
    else if ( end == QD_INT_32_MAX ) {
        end = len;
    }

    for ( i = start ; i < len && i < end; i++ ) {
        tmp.larr.push_back(arr.larr[i]);
    }
    return tmp; 
}

D_VAR DParser::analyse_assign(Instruction& inc,std::vector<Instruction>& codes){
    logger->debug("<-----  analyse  assign  ----->");

    logger->debug("cur line is :  ",inc.curpos);
    logger->debug("left pos is :  ",inc.lpos);
    logger->debug("right pos is :  ",inc.rpos);
    logger->debug("left  value is :  ",inc.left.var.chv);
    logger->debug("operator code is :  ",inc.type);
    
    if ( FIN_END ==  inc.rpos ) {
        logger->error("assigment right expression is null");
        return ERR_END;
    }

    Instruction& tmp = codes[inc.rpos];

    switch (tmp.res.type)
    {
    case VE_BOOL:
        return tmp.res.var.bv;
    case VE_INT:
        return OC_MINUS == inc.sufftype ? -tmp.res.var.iv : tmp.res.var.iv;
    case VE_STR:
        return tmp.res.var.chv;
    case VE_FLT:
        return OC_MINUS == inc.sufftype ? -tmp.res.var.dv : tmp.res.var.dv;
    default:
        logger->error("assign nothing happend");
        return D_VAR();
    }
}

unsigned int DParser::analyse_expr(Instruction& inc,FunState& fun){
    logger->debug("<-----  analyse expression  ----->");
    D_VAR& tleft =  inc.left;
    D_VAR& tright = inc.right;
    D_VAR& tres = inc.res;
    D_VAR tmpleft;
    D_VAR tmpright;

    logger->debug("cur line is :  ",inc.curpos);
    logger->debug("left pos is :  ",inc.lpos);
    logger->debug("right pos is :  ",inc.rpos);
    logger->debug("operator code is :  ",inc.type);
    logger->debug("last code is :  ",inc.ltype);
    logger->debug("left    value  ",inc.left.var.iv);
    logger->debug("right   value  ",inc.right.var.iv);
    logger->debug((int)inc.pretype,(int)inc.sufftype);


    // 判断用户变量
    if (VE_USER == inc.left.type ) {
        tmpleft = tleft;
        tleft = variable_check(tleft.var.chv,fun);
    }
    if (VE_USER == inc.right.type ) {
        tmpright = tright;
        tright = variable_check(tright.var.chv,fun);
    }
    
    if ( FIN_END != inc.lpos) {
        Instruction& tmp = fun.codes[inc.lpos];
        tleft = tmp.res;
    }
    if ( FIN_END != inc.rpos){
        Instruction& tmp = fun.codes[inc.rpos];
        tright = tmp.res;
    }

    //如果为负数
    if (OC_MINUS == inc.pretype){
        tleft = -tleft;
    }
    if (OC_MINUS == inc.sufftype){
        tright = -tright;
    }

    //解析指令
    switch (inc.type)
    {
    case OC_ADD:
    case OC_SUB:{
        //这里进行数据类别的判断,例如整型,浮点数的转换
        if ( (OC_ADD == inc.type && OC_SUB != inc.ltype ) || 
                OC_SUB == inc.type && OC_SUB == inc.ltype ) {
            result_add(tres,tleft,tright);
        }else{
            result_sub(tres,tleft,tright);
        }
        break;
    }
    case OC_MUL:{
        if(result_mul(tres,tleft,tright)){
            logger->error("error  in  multiply  expression");
            return ERR_END;
        }
        break;
    }
    case OC_MOD:{
        if(result_mod(tres,tleft,tright)){
            logger->error("error  in  mod  expression");
            return ERR_END;
        }
        break;
    }
    case OC_DIV:{
        if(result_div(tres,tleft,tright)){
            logger->error("error  in  divide  expression");
            return ERR_END;
        }
        break;
    }
    case OC_AND: {
        if(result_and(tres,tleft,tright)){
            logger->error("error  in  and  expression");
            return ERR_END;
        }
        break;
    }
    case OC_OR:{
        if(result_or(tres,tleft,tright)){
            logger->error("error  in  or  expression");
            return ERR_END;
        }
        break;
    }
    case OC_DEQ:{
        if(result_eq(tres,tleft,tright)){
            logger->error("error  in  equal  expression");
            return ERR_END;
        }
        break;
    }
    case OC_NEQ:{
        if(result_ne(tres,tleft,tright)){
            logger->error("error  in  not  equal  expression");
            return ERR_END;
        }
        break;
    }
    case OC_LT:{
        if(result_lt(tres,tleft,tright)){
            logger->error("error  in  less  than  expression");
            return ERR_END;
        }
        break;
    }
    case OC_LE:{
        if(result_le(tres,tleft,tright)){
            logger->error("error  in  less  equal  expression");
            return ERR_END;
        }
        break;
    }
    case OC_GT:{
        if(result_gt(tres,tleft,tright)){
            logger->error("error  in  greater  than  expression");
            return ERR_END;
        }
        break;
    }
    case OC_GE:{
        if(result_ge(tres,tleft,tright)){
            logger->error("error  in  greater  equal  expression");
            return ERR_END;
        }
        break;
    }
    case OC_ARR_ACESS:{
        if ( tright.var.iv < 0 || tright.var.iv >= this->arrays[tleft.var.chv].larr.size() ) {
            logger->error("variable access out of range");
            return ERR_END;
        }
        inc.res = this->arrays[tleft.var.chv].larr[tright.var.iv];
        break;
    }
    case OC_NULL:{
        inc.res = inc.left;
        break; 
    }
    default:{
        logger->error(ls._row,":",ls._col,"code parse error ");
        break;
    }
    }

    if ( VE_USER == tmpleft.type  ) {
        tleft = tmpleft;
    }
    if ( VE_USER == tmpright.type ) {
        tright = tmpright;
    }

    logger->debug("result   value  ",tres.var.dv);
    logger->debug("result   value  ",tres.var.iv);
    logger->debug("result   value  ",tres.var.bv);

    return 0;
}

unsigned int DParser::analyse_array(Instruction& inc,const FunState& fun){
    std::string name = inc.left.var.chv;
    unsigned int pos = inc.rpos;

    if ( FIN_END == pos ) {
        //do nothing
        return 0;
    }    
    D_ARRAY array;
    Instruction tmp = fun.codes[inc.rpos];
    array.larr.push_back(tmp.left);
    while ( FIN_END != pos) {
        tmp = fun.codes[tmp.rpos];
        pos = tmp.rpos;
        array.larr.push_back(tmp.left);
    }
    //反转了一下数组,有点麻烦暂时先保留这个条件。
    std::reverse(array.larr.begin(),array.larr.end());
    this->arrays[name] = array;
    print_array(name);
    return 0;
}

unsigned int DParser::analyse_code(unsigned int& pos,FunState& fun){
    unsigned int clen = fun.codes.size();
    unsigned int i = pos;
    logger->debug("<------  function  analyse  start   ------>");
    
    FunState* function = nullptr;

    for (;i < clen;i++){
        Instruction& inc = fun.codes[i];
        //在这里判断解析类型
        logger->debug("operator code is :  ",inc.type);

        switch (inc.type)
        {
        case OC_CALL:{
            logger->debug("analyse  call    !!!!!!!!! ");
            logger->debug("cur line is :  ",inc.curpos);
            logger->debug("left pos is :  ",inc.lpos);
            logger->debug("right pos is :  ",inc.rpos);
            logger->debug("result  value  is :  ",inc.res.var.iv);
            logger->debug("stack deep size is ",this->funstack.size());
            
            //function
            //第几层函数的调用
            FunState* curfun = &fun;
            unsigned int recur = inc.rpos;
            while (recur) {
                curfun = curfun->prev;
                recur --;
            }
            //分配内存空间
            function =  new FunState(*curfun->proto->lfuns[inc.lpos]);
            function->proto->lv.clear();
            function->code_pos = 0;
            this->funstack.push_back(*function);
            analyse_code(function->code_pos,*function);
            //释放内存
            delete function;
            break;
        }
        case OC_JMP:{
            logger->debug("analyse  jump    !!!!!!!!! ");
            logger->debug("cur line is :  ",inc.curpos);
            logger->debug("left pos is :  ",inc.lpos);
            logger->debug("right pos is :  ",inc.rpos);
            logger->debug("result value is :  ",inc.res.var.bv);
            logger->debug("operator code is :  ",inc.type);

            //判断是否跳转
            if ( FIN_END != inc.rpos ) {
                if (!inc.rpos) {
                    //保存下一次跳转的条件
                    continue;//次数为0
                }
                --inc.rpos;
            }

            if (inc.res.var.bv) {
                if (FIN_END != inc.lpos) {
                    i = inc.lpos - 1;
                }
            }
            break;
        }      
        case OC_ASSIGN:{
            D_VAR result ;
            if (inc.res.var.bv) {
                result = analyse_assign(inc,fun.codes);
                if ( VE_VOID == result.type ) {
                    return ERR_END;
                }
                global.proto->lv[inc.left.var.chv] = result;
                print_variable(inc.left.var.chv,global.proto->lv);
            }
            else {
                result = analyse_assign(inc,fun.codes);
                if ( VE_VOID == result.type ) {
                    return ERR_END;
                }
                fun.proto->lv[inc.left.var.chv] = result;
                print_variable(inc.left.var.chv,fun.proto->lv);
            }
            break;
        }
        case OC_RET:{
            logger->debug("analyse  return    !!!!!!!!! ");
            logger->debug("stack deep size is ",this->funstack.size());
            if ( this->funstack.size() ) {
                // 出栈
                this->funstack.pop_back();
            }
            logger->debug("<------  function  analyse  end   ------>");
            pos = clen;
            return 0;
        }
        case OC_IF:{
            logger->debug("analyse  if    !!!!!!!!! ");
            logger->debug("cur line is :  ",inc.curpos);
            logger->debug("left pos is :  ",inc.lpos);
            logger->debug("right pos is :  ",inc.rpos);
            logger->debug("result  value  is :  ",inc.res.var.iv);
            //if
            if ( FIN_END != inc.rpos && FIN_END != inc.lpos ) {
                if ( fun.codes[inc.rpos].res.var.bv ) {
                    FunState& jumpfun = *fun.proto->lfuns[inc.lpos];
                    //一定要清空所有变量防止有变量信息记录
                    jumpfun.proto->lv.clear();
                    jumpfun.code_pos = 0;  
                    analyse_code(jumpfun.code_pos,jumpfun);
                }
                else {
                    logger->debug("if  function  dont  jump  in");
                }
            }
            else  {
                logger->error("if  expression  analyse  error");
                return ERR_END;
            }
            break;
        }
        case OC_FOR:{
            logger->debug("analyse  for    !!!!!!!!! ");
            logger->debug("cur line is :  ",inc.curpos);
            logger->debug("left pos is :  ",inc.lpos);
            logger->debug("right pos is :  ",inc.rpos);
            logger->debug("result  value  is :  ",inc.res.var.iv);
            //for 
            if ( FIN_END != inc.lpos ) {
                if ( inc.res.var.iv ) {
                    FunState& jumpfun =  *fun.proto->lfuns[inc.lpos];
                    //一定要清空所有变量防止有变量信息记录
                    jumpfun.proto->lv.clear();
                    jumpfun.codes.back().rpos = inc.res.var.iv;
                    //重置偏移量为0
                    jumpfun.code_pos = 0;   
                    //最后一个指令是jump所以
                    analyse_code(jumpfun.code_pos,jumpfun);
                }
                else {
                    logger->debug("for  function  dont  jump  in");
                }
            }
            else {
                logger->error("for  expression  analyse  error");
                return ERR_END;
            }
            break;
        }
        case OC_ARR_ASSIGN:{
            if (analyse_array(inc,fun)){
                logger->error("arrray assign error");
                return ERR_END;
            }
            break;
        }
        case OC_ARR_LASSIGN:{
            Instruction& arr = fun.codes[inc.rpos];

            D_VAR lval;
            D_VAR rval;
            if ( VE_USER == arr.left.type ) {
                lval = variable_check(arr.left.var.chv,fun);
                if ( VE_INT !=  lval.type ) {
                    logger->error("error in array is not int type");
                    return ERR_END;
                }
            }
            else {
                lval = arr.left.var.iv;
            }
            
            if ( VE_USER == arr.right.type ) {
                rval = variable_check(arr.right.var.chv,fun);
                if ( VE_INT !=  rval.type ) {
                    logger->error("error in array is not int type");
                    return ERR_END;
                }
            }
            else{
                rval = arr.right.var.iv;
            }
            
            D_ARRAY tmparr = list_access(
                        lval.var.iv,rval.var.iv,
                        this->arrays[arr.res.var.chv]);
            this->arrays[inc.left.var.chv].larr.clear();
            this->arrays[inc.left.var.chv] = tmparr;

            print_array(inc.left.var.chv);
            break;
        }
        case OC_ARR_PASSIGN:{
            this->arrays[inc.left.var.chv].larr[inc.lpos] = analyse_assign(inc,global.codes);
            break;
        }
        case OC_ARR_LIST:
        case OC_ARR_VAL:{
            //跳过，
            break;
        }
        default:{
            //赋值了一些常量
            if(analyse_expr(inc,fun)) {
                logger->error("analyse  expresion  error  in  analyse  code");
                return ERR_END;
            }
            break;
        }
        }
    }

    logger->debug("<------  function  analyse  end   ------>");
    pos = clen;

    return 0;
}

unsigned int DParser::result_add(D_VAR& result,const D_VAR& left,const D_VAR& right){

    if ( VE_FLT == left.type && VE_FLT == right.type ) {
        result = left.var.dv + right.var.dv;
    }
    else if ( VE_FLT == left.type && VE_INT == right.type ) {
        result = left.var.dv + right.var.iv;
    }
    else if ( VE_INT == left.type && VE_FLT == right.type ) {
        result = left.var.iv + right.var.dv;
    }
    else if ( VE_INT == left.type && VE_INT == right.type ) {
        result = left.var.iv + right.var.iv;
    }
    else {
        logger->error("these  type  can  not  compare  in  add  expression ");
        return ERR_END;
    }

    return 0;
}

unsigned int DParser::result_sub(D_VAR& result,const D_VAR& left,const D_VAR& right){

    if ( VE_FLT == left.type && VE_FLT == right.type ) {
        result = left.var.dv - right.var.dv;
    }
    else if ( VE_FLT == left.type && VE_INT == right.type ) {
        result = left.var.dv - right.var.iv;
    }
    else if ( VE_INT == left.type && VE_FLT == right.type ) {
        result = left.var.iv - right.var.dv;
    }
    else if ( VE_INT == left.type && VE_INT == right.type ) {
        result = left.var.iv - right.var.iv;
    }
    else {
        logger->error("these  type  can  not  compare  in  sub  expression ");
        return ERR_END;
    }
    return 0;
}

unsigned int DParser::result_mul(D_VAR& result,const D_VAR& left,const D_VAR& right){

    if ( VE_FLT == left.type && VE_FLT == right.type ) {
        result = left.var.dv * right.var.dv;
    }
    else if ( VE_FLT == left.type && VE_INT == right.type ) {
        result = left.var.dv * right.var.iv;
    }
    else if ( VE_INT == left.type && VE_FLT == right.type ) {
        result = left.var.iv * right.var.dv;
    }
    else if ( VE_INT == left.type && VE_INT == right.type ) {
        result = left.var.iv * right.var.iv;
    }
    else {
        logger->error("these  type  can  not  compare  in  mul  expression ");
        return ERR_END;
    }
    return 0;
}

unsigned int DParser::result_mod(D_VAR& result,const D_VAR& left,const D_VAR& right){
    if ( VE_INT == left.type && VE_INT == right.type ) {
        result = left.var.iv % right.var.iv;
    }
    else {
        logger->error("these  type  can  not  compare  in  mod  expression ");
        return ERR_END;
    }
    return 0;
}

unsigned int DParser::result_div(D_VAR& result,const D_VAR& left,const D_VAR& right){

    if ( VE_FLT == left.type && VE_FLT == right.type ) {
        result = left.var.dv / right.var.dv;
    }
    else if ( VE_FLT == left.type && VE_INT == right.type ) {
        result = left.var.dv / right.var.iv;
    }
    else if ( VE_INT == left.type && VE_FLT == right.type ) {
        result = left.var.iv / right.var.dv;
    }
    else if ( VE_INT == left.type && VE_INT == right.type ) {
        result = left.var.iv / right.var.iv;
    }
    else {
        logger->error("these  type  can  not  compare  in  div  expression ");
        return ERR_END;
    }
    return 0;
}
    
unsigned int DParser::result_and(D_VAR& result,const D_VAR& left,const D_VAR& right){
    if ( VE_BOOL != left.type || VE_BOOL != right.type ) {
        logger->error("left  or  right is not bool type in  and  expression");
        return ERR_END;
    }
    result = right.var.bv && left.var.bv;
    return 0;
}

unsigned int DParser::result_or(D_VAR& result,const D_VAR& left,const D_VAR& right){
    if ( VE_BOOL != left.type || VE_BOOL != right.type ) {
        logger->error("left  or  right is not bool type in  or  expression");
        return ERR_END;
    }
    result = right.var.bv || left.var.bv;
    return 0;
}

unsigned int DParser::result_eq(D_VAR& result,const D_VAR& left,const D_VAR& right){
    unsigned int ltype = left.type;
    unsigned int rtype = right.type;

    if ( ltype != rtype ) {
        logger->error(" left  right  type  is  not  equals");
        return ERR_END;
    }

    switch (ltype)
    {
    case VE_BOOL:
        result = left.var.bv == right.var.bv;
        break;
    case VE_INT:
        result = left.var.iv == right.var.iv;
        break;
    case VE_FLT:
        result = left.var.dv == right.var.dv;
        break;
    default:
        logger->error("no  such  type to equal expression ");
        return ERR_END;
    }

    return 0;
}

unsigned int DParser::result_ne(D_VAR& result,const D_VAR& left,const D_VAR& right){
    unsigned int ltype = left.type;
    unsigned int rtype = right.type;

    if ( ltype != rtype ) {
        logger->error("left  right  type  is  not  equals");
        return ERR_END;
    }

    switch (ltype)
    {
    case VE_BOOL:
        result = left.var.bv != right.var.bv;
        break;
    case VE_INT:
        result = left.var.iv != right.var.iv;
        break;
    case VE_FLT:
        result = left.var.dv != right.var.dv;
        break;
    default:
        logger->error("no  such  type to not equal expression ");
        return ERR_END;
    }

    return 0;
}

unsigned int DParser::result_lt(D_VAR& result,const D_VAR& left,const D_VAR& right){
    unsigned int ltype = left.type;
    unsigned int rtype = right.type;

    if ( ltype != rtype ) {
        logger->error("left  right  type  is  not  equals");
        return ERR_END;
    }

    switch (ltype)
    {
    case VE_INT:
        result = left.var.iv < right.var.iv;
        break;
    case VE_FLT:
        result = left.var.dv < right.var.dv;
        break;
    default:
        logger->error("these  type  can  not  compare  in  less  than  expression ");
        return ERR_END;
    }

    return 0;
}

unsigned int DParser::result_le(D_VAR& result,const D_VAR& left,const D_VAR& right){
    unsigned int ltype = left.type;
    unsigned int rtype = right.type;

    if ( ltype != rtype ) {
        logger->error("left  right  type  is  not  equals");
        return ERR_END;
    }

    switch (ltype)
    {
    case VE_INT:
        result = left.var.iv <= right.var.iv;
        break;
    case VE_FLT:
        result = left.var.dv <= right.var.dv;
        break;
    default:
        logger->error("these  type  can  not  compare  in  less  equal  expression ");
        return ERR_END;
    }

    return 0;
}

unsigned int DParser::result_gt(D_VAR& result,const D_VAR& left,const D_VAR& right){
    unsigned int ltype = left.type;
    unsigned int rtype = right.type;

    if ( ltype != rtype ) {
        logger->error("left  right  type  is  not  equals");
        return ERR_END;
    }

    switch (ltype)
    {
    case VE_INT:
        result = left.var.iv > right.var.iv;
        break;
    case VE_FLT:
        result = left.var.dv > right.var.dv;
        break;
    default:
        logger->error("these  type  can  not  compare  in  greater  than  expression ");
        return ERR_END;
    }

    return 0;
}

unsigned int DParser::result_ge(D_VAR& result,const D_VAR& left,const D_VAR& right){
    unsigned int ltype = left.type;
    unsigned int rtype = right.type;

    if ( ltype != rtype ) {
        logger->error("left  right  type  is  not  equals");
        return ERR_END;
    }

    switch (ltype)
    {
    case VE_INT:
        result = left.var.iv >= right.var.iv;
        break;
    case VE_FLT:
        result = left.var.dv >= right.var.dv;
        break;
    default:
        logger->error("these  type  can  not  compare  in  greater  equal  expression ");
        return ERR_END;
    }

    return 0;
}

void DParser::read_file(const char* file){
    FILE *fp;
    char buffer[1024];
    memset(buffer,0,sizeof(buffer));

    // 打开文件
    fp = fopen(file, "r");
    if ( fp == NULL ) {
        std::cout << " file open error " << std::endl;
        fclose(fp);
        return;
    }

    // while (fgets(buffer,sizeof(buffer),fp))
    // {
    //     // 输出文件内容
    //     this->parse(buffer);
    // }
    //目前一次性读
    int st = fread(buffer,sizeof(char),sizeof(buffer),fp);
    this->parse(buffer);

    // 关闭文件
    fclose(fp);
}

void DParser::read_line(const char* line){
    this->parse(line);
}



_QD_END