#include "qd_parser.h"
#include "qd_baselib.h"


_QD_BEGIN

#define FIND_NEXT \
if ( this->findX_next() ) \
{\
return ERR_END;\
}


DParser::DParser() {
    logger = Logger::getInstance();


    ls = nullptr;
    ls = new LexState();

    lib = nullptr;

    //初始化环境变量
    D_ENV* env = new D_ENV();
    this->env.push_back(env);

}

DParser::~DParser() {
    if ( ls != nullptr ) {
        delete ls;
        ls = nullptr;
    }
    //不进行释放内存，只是暂时检测用的

    // std::cout << " env stack size is " << this->env.size() << std::endl;
    this->env_clear();
}

void DParser::env_clear() {
    for ( D_ENV* i : this->env ) {
        if (i != nullptr) {
            delete i;
            i = nullptr;
        }
    }
    //清空所有元素
    this->env.clear();
}

size_t DParser::parseX_next() {

    if ( T_BLANK != ls->t.token ) {
        this->ls->prevhead.token = this->ls->lookahead.token;
        this->ls->lookahead.token = this->ls->t.token;
    }

    if ( T_ERR == (this->ls->t.token = ls->llex()) ) {
        logger->error("lexical error");
        return ERR_END;
    }
    // logger->debug("debug token info : ",ls->t.token,":",ls->lookahead.token,":",ls->prevhead.token);
    
    return 0;
}

size_t DParser::parse() {

    //第一个token
    FIND_NEXT

    //这里开始全局解析
    if (parse_Func(*this->env_stack_head()->cur)) {
        return ERR_END;
    }

    logger->debug("global bytes code total is ",this->env_stack_head()->cur->codes.size());
    return 0;
}

size_t DParser::parse_Func(FunHead& fun) {
    logger->debug(" <------------------ start --------------------> ");

    bool err_flag = false;
    //暂时
    bool haser = false;

    for(;;){
        //因为有if fun while等情况
        //同时存在多个换行情况
        while ( T_END == ls->t.token ) {
            FIND_NEXT
        }
        
        if( T_EOF == ls->t.token ) {
            break;//结束
        }
        else if ( T_PASS == ls->t.token ) {
            //函数体到达结尾
            break;
        }
        
        //判断是哪个表达式
        switch (ls->t.token)
        {
        case T_COMMENT:{
            logger->info("comment line ");
            FIND_NEXT
            break;
        }
        case T_UDATA:
        case T_GLOBAL:
        case T_LOCAL:
        {
            if(statement(fun)) {
                logger->error("user statement error");
                err_flag = true;
            }
            break;
        }
        case T_JUMP:{
            //跳转指令
            if(jump_expr(fun)) {
                logger->error("jump error");
                err_flag = true;
            }
            break;
        }
        case T_FOR:{
            if ( for_expr(fun) ) {
                logger->error(ls->_row,":",ls->_col," for statement error");
                return ERR_END;
            }
            break;
        }
        case T_WHILE:{
            if ( while_expr(fun) ) {
                logger->error(ls->_row,":",ls->_col," while statement error");
                return ERR_END;
            }
            break;
        }
        case T_FUNC:{
            if ( function_expr(fun) ) {
                logger->error(ls->_row,":",ls->_col," function statement error");
                //错误处理
                
                //清空lfuns的元素，以及传入虚拟机函数定义的字节码
                fun.lfuns.pop_back();
                fun.codes.pop_back();
                fun.codes.pop_back();
                return ERR_END;
            }
            break;
        }
        case T_RETURN: {
            Instruction ret;
            ret.curpos = fun.codes.size();
            ret.left = QD_KYW_RET;
            FIND_NEXT

            if ( T_END != ls->t.token ) {
                if( assign_expr(ret,fun) ){
                    logger->error(ls->_row,":",ls->_col," return assign expression error");
                    err_flag = true;
                }
                else {
                    ret.type = OC_RET;
                    fun.codes.push_back(ret);
                }
            }

            break;
        }
        case T_IF:{
            if( if_expr(fun) ){
                logger->error(ls->_row,":",ls->_col," if statement error");
                return ERR_END;
            }
            break;
        }
        case T_ELIF:{
            logger->error(ls->_row,":",ls->_col," only elif statement error");
            return ERR_END;
        }
        case T_ELSE:{
            logger->error(ls->_row,":",ls->_col," only else statement error");
            return ERR_END;
        }
        case T_PERIOD:{
            logger->error("can not use period single ");
            err_flag = true;
            break;
        }
        default:{
            logger->info("do nothing  ");
            break;
        }
        }

        //解析到末尾的是文件终止符
        if ( T_EOF == ls->t.token ) {
            break;
        }
        else if (err_flag) {
            haser = true;
            if (skip_to_end()) {
                return ERR_END;
            }
            FIND_NEXT
            err_flag = false;
        }
    }

    //有错误
    if (haser) {
        logger->error("parse function has error");
        return ERR_END;
    }
    
    logger->debug(" <------------------ end --------------------> ");
    
    return 0;
}

size_t DParser::skip_blank() {
    if ( T_BLANK == ls->t.token )
    {
        if(this->parseX_next()) {
            return ERR_END;
        }
    }
    return 0;
}

size_t DParser::skip_line() {
    if ( T_END == ls->t.token )
    {
        if (this->parseX_next()) {
            return ERR_END;
        }
    }
    return 0;
}

size_t DParser::findX_next() {
    if (this->parseX_next()) {
        return ERR_END;
    }
    this->skip_blank();
    return 0;
}

size_t DParser::skip_to_end() {
    while ( T_END != ls->t.token )
    {
        FIND_NEXT
    }
    
    return 0;
}

size_t DParser::parse_Opr(Instruction& inc,short type) {
    //判断操作符
    switch (type)
    {
    case T_PLUS:
        return inc.type = OC_ADD;
    case T_MINUS:
        return inc.type = OC_SUB;
    case T_MUL:
        return inc.type = OC_MUL;
    case T_DIV:
        return inc.type = OC_DIV;
    case T_MOD:
        return inc.type = OC_MOD;
    // case T_DIV:
    //     inc.type = OC_IDIV;
    //     break;
    case T_AMPERSAND:
        return inc.type = OC_AND;
    case T_VERTICAL_BAR:
        return inc.type = OC_OR;
    case T_DEQ :
        return inc.type = OC_DEQ;
    case T_NEQ:
        return inc.type = OC_NEQ;
    case T_LE:
        return inc.type = OC_LE;
        break;
    case T_LN:
        return inc.type = OC_LT;
    case T_GE:
        return inc.type = OC_GE;
    case T_GN:
        return inc.type = OC_GT;
    default:
        return inc.type = OC_NULL;
    }
}

size_t DParser::symbol_reversal(Instruction& inc) {
    if ( OC_MINUS == inc.restype ) {
        inc.restype = OC_NIL;
    }
    else {
        inc.restype = OC_MINUS;
    }
    return 0;
}

size_t DParser::statement(FunHead& fun){
    Instruction inc;

    //全局变量
    if ( T_GLOBAL == ls->t.token ) {
        inc.lpos = VA_GLOBAL;
        FIND_NEXT
    }
    else if ( T_LOCAL == ls->t.token ) {
        inc.lpos = VA_LOCAL;
        FIND_NEXT
    }
    else {
        inc.lpos = VA_DEFAULT;
    }

    if ( T_UDATA != ls->t.token ) {
        logger->error(ls->_row,":",ls->_col," data error");
        return ERR_END;
    }


    //判断是否为用户变量且，这个用户变量是否为库函数名
    D_VAR* libvar = variable_check(ls->dvar.var.chv,this->env_stack_head());
    //记录左值
    inc.left = ls->dvar;
    FIND_NEXT
    
    if (libvar) {
        //当前tok 为函数调用
        if ( VE_LIB == libvar->type && T_EQ != ls->t.token ) {
            if(lib_expr(fun)) {
                logger->error("lib expression error");
                return ERR_END;
            }
            return 0;            
        }
        //如果不是函数调用而是普通赋值
    }

    
    //下一个如果是函数调用的话
    if ( T_LPARENTH == ls->t.token ) {
        if(call_expr(inc.left.var.chv,fun)) {
            logger->error(ls->_row,":",ls->_col," call  error");
            return ERR_END;
        }
        FIND_NEXT
        if ( T_END != ls->t.token ) {
            logger->error(ls->_row,":",ls->_col," function call end is not correct");
            return ERR_END;
        }
        return 0;
    }

    //数组下标赋值
    else if ( T_LBRACKET == ls->t.token ) {
        if(array_element_expr(inc.left.var.chv,fun)){
            logger->error("array subscript is assing error");
            return ERR_END;
        }

        return 0;
    }
    //因为是赋值表达式所以判断=
    else if ( T_EQ != ls->t.token ) {
        logger->error(ls->_row,":",ls->_col," unkonwn expression error ");
        return ERR_END;
    }

    FIND_NEXT

    //数组
    if ( T_LBRACKET == ls->t.token ) {
        if(array_expr(inc.left.var.chv,fun)){
            logger->error(ls->_row,":",ls->_col," array parse error");
            return ERR_END;
        }
        //局部
        this->env_stack_top()->lv[inc.left.var.chv] = "";
        this->env_stack_top()->lv[inc.left.var.chv].type = VE_UNION;
    }
    else {
        if ( assign_expr(inc,fun) ) {
            logger->error("assign expression error");
            return ERR_END;
        }
        fun.codes.push_back(inc);

        if ( VA_DEFAULT == inc.lpos ) {
            this->env_stack_top()->lv[inc.left.var.chv] = 0;
            this->env_stack_top()->lv[inc.left.var.chv].type = VE_NULL;
        }
        else if ( VA_LOCAL == inc.lpos ) {
            last_env(this->env_stack_top())->lv[inc.left.var.chv] = 0;
            last_env(this->env_stack_top())->lv[inc.left.var.chv].type = VE_NULL;
        }
        else if ( VA_GLOBAL == inc.lpos ) {
            this->env_stack_head()->lv[inc.left.var.chv] = 0;
            this->env_stack_head()->lv[inc.left.var.chv].type = VE_NULL;
        }
        else {
            logger->error("assign scope is error");
            return ERR_END;
        }
    }

    
    return 0;//正常退出
}

size_t DParser::assign_expr(Instruction& inc,FunHead& fun){
    inc.type = OC_ASSIGN;

    short assigntype;
    if ( T_MINUS == ls->t.token  
    || T_LPARENTH == ls->t.token) {
        //do nothing
    }
    else if ( T_UDATA == ls->t.token ) {
        D_VAR* tmpobj = variable_check(ls->dvar.var.chv,this->env_stack_top());
        if (!tmpobj) {
            logger->error(ls->_row,":",ls->_col," prefix user data undefine");
            return ERR_END;
        }
        assigntype = tmpobj->type;
    }
    else if ( ls->is_variable(ls->t.token) ) {
        assigntype = ls->dvar.type;
    }
    else {
        //如果是最后位置
        if ( T_END == ls->t.token ) {
            logger->error(ls->_row - 1,":",ls->_end," assign expression error in end position ");
        }
        else {
            logger->error(ls->_row,":",ls->_col," variable type is error ");
        }
        return ERR_END;
    }

    switch (assigntype)
    {
    case VE_STR: {
        Instruction tmp;
        tmp.left = ls->dvar;
        tmp.curpos = fun.codes.size();
        fun.codes.push_back(tmp);
        FIND_NEXT
        inc.rpos = tmp.curpos;
        break;
    }
    default: {
        inc.rpos = this->simple_expr(fun);

        if ( ERR_END == inc.rpos ) {
            logger->error("basic expression error");
            return ERR_END;
        }
        break;
    }
    }
    
    
    inc.curpos = fun.codes.size();
    
    return 0;
}

size_t DParser::jump_expr(FunHead& fun){
    logger->debug("jumping to ");
    FIND_NEXT
    //不是int类型
    if ( T_INT != ls->t.token ) {
        logger->error("jumping num  type is error");
        FIND_NEXT
        return ERR_END;
    }
    //超出范围
    if ( ls->dvar.var.iv < 0) {
        logger->warn(" jump less than zero ");
        FIND_NEXT
        return ERR_END;
    }
    
    FIND_NEXT
    //加入指令
    Instruction inc;
    inc.type = OC_JMP;
    inc.lpos = ls->dvar.var.iv - 1; 
    inc.curpos = fun.codes.size();
    fun.codes.push_back(inc);
    FIND_NEXT

    return 0;
}

size_t DParser::if_expr(FunHead& func){
    
    if ( T_PASS == ls->t.token ) {
        return 0;
    }
    Instruction inc;

    FIND_NEXT
    //判断跳转函数是否执行
    inc.rpos = simple_expr(func);

    inc.curpos = func.codes.size();
    //记录跳转的函数是第几个
    inc.lpos = (int)this->env_stack_top()->cur->lfuns.size();
    inc.type = OC_IF;      //调用

    func.codes.push_back(inc);

    //判断结束符号是否正确
    if (T_COLON != ls->t.token) {
        logger->error(ls->_row,":",ls->_col," if condition judgment error");
        return ERR_END;
    }
    
    FIND_NEXT//这里通常都是 line 换行符

    D_ENV* e = new D_ENV();
    e->anonymous = true;
    e->prev = env_stack_top();
    env_stack_top()->cur->lfuns.push_back(e->cur);
    this->env.push_back(e);

    if(parse_Func(*e->cur)) {
        return ERR_END;
    }

    delete env_stack_top();
    this->env.pop_back();
    
    // if 直接结束
    if ( T_PASS != ls->t.token) {
        //pass
        logger->error(ls->_row,":",ls->_col," missing pass end in if statement");
        return ERR_END;
    }
    FIND_NEXT
    //end
    if ( T_END != ls->t.token ) {
        logger->error(ls->_row,":",ls->_col," missing  end in elif statement");
        return ERR_END;
    }
    FIND_NEXT
    
    //出现elif
    if ( T_ELIF == ls->t.token ) {
        //解析elif 查找到最后的跳转位置挨个遍历
        if (elif_expr(func)) {
            logger->error(ls->_row,":",ls->_col," elif expression parse error");
            return ERR_END;
        }        
    }
    
    //else语句
    if ( T_ELSE == ls->t.token ) {
        if (else_expr(func)) {
            logger->error(ls->_row,":",ls->_col," else expression parse error");
            return ERR_END;
        }
    }

    //这里最后加一个是为了判断if 的终止位置而已 把 ifstate变成false
    Instruction endif;
    endif.type = OC_END;
    endif.curpos = func.codes.size();
    func.codes.push_back(endif);

    return 0;
}

size_t DParser::elif_expr(FunHead& func){

    while ( T_ELIF == ls->t.token ) {
        Instruction inc;
        FIND_NEXT
        inc.rpos = simple_expr(func);

        inc.curpos = func.codes.size();
        inc.lpos = (int)this->env_stack_top()->cur->lfuns.size();
        inc.type = OC_IF;      //调用
        func.codes.push_back(inc);

        //判断结束符号是否正确
        if (T_COLON != ls->t.token) {
            logger->error(ls->_row,":",ls->_col," elif condition judgment error");
            return ERR_END;
        }
        FIND_NEXT
        
        D_ENV* e = new D_ENV();
        e->anonymous = true;
        e->prev = env_stack_top();
        env_stack_top()->cur->lfuns.push_back(e->cur);
        this->env.push_back(e);

        if(parse_Func(*e->cur)) {
            return ERR_END;
        }

        delete env_stack_top();
        this->env.pop_back();

        if ( T_PASS != ls->t.token ) {
            logger->error(ls->_row,":",ls->_col," missing pass end in elif statement");
            return ERR_END;
        }
        FIND_NEXT
        //end
        if ( T_END != ls->t.token ) {
            logger->error(ls->_row,":",ls->_col," missing end in elif statement");
            return ERR_END;
        }
        FIND_NEXT
    }

    return 0;
}

size_t DParser::else_expr(FunHead& func){
    Instruction istrue;
    istrue.curpos = func.codes.size();
    istrue.type = OC_NULL;
    istrue.left = true;
    func.codes.push_back(istrue);

    Instruction inc;
    inc.rpos = istrue.curpos;

    inc.curpos = func.codes.size();
    inc.lpos = (int)this->env_stack_top()->cur->lfuns.size();
    inc.type = OC_IF;      //调用
    func.codes.push_back(inc);

    FIND_NEXT
    //判断结束符号是否正确
    if (T_COLON != ls->t.token) {
        logger->error(ls->_row,":",ls->_col," else condition judgment error");
        return ERR_END;
    }
    FIND_NEXT
    
    D_ENV* e = new D_ENV();
    e->anonymous = true;
    e->prev = env_stack_top();
    env_stack_top()->cur->lfuns.push_back(e->cur);
    this->env.push_back(e);

    if(parse_Func(*e->cur)) {
        return ERR_END;
    }

    delete env_stack_top();
    this->env.pop_back();

    if ( T_PASS != ls->t.token ) {
        logger->error(ls->_row,":",ls->_col," missing pass end in else statement");
        return ERR_END;
    }
    FIND_NEXT
    //end
    if ( T_END != ls->t.token ) {
        logger->error(ls->_row,":",ls->_col," missing end in else statement");
        return ERR_END;
    }
    FIND_NEXT

    return 0;
}

size_t DParser::for_expr(FunHead& func){
//     DPARSER_NEXT

//     Instruction forinc;
//     forinc.type = OC_FOR;
//     forinc.curpos = func.codes.size();
//     forinc.lpos = func.lfuns.size();
//     func.codes.push_back(forinc);

//     FunHead* forstate = new FunHead();
//     forstate->anonymous = true;
//     // forstate->prev = &func;

//     //初始化值
//     Instruction first;
//     first.left = 1;
//     first.type = OC_NULL;
//     first.curpos = forstate->codes.size();
//     forstate->codes.push_back(first);

//     Instruction assign;
//     assign.curpos = forstate->codes.size();

//     if ( T_UDATA != ls->t.token ) {
//         logger->error(ls->_row,":",ls->_col," for initialization type is not correct");
//         return ERR_END;
//     }
//     std::string varname = ls->dvar.var.chv;
//     assign.left = ls->dvar;//迭代计数器
//     assign.rpos = first.curpos;
//     assign.type = OC_ASSIGN;
//     forstate->codes.push_back(assign);
//     env->lv[ls->dvar.var.chv] = 0;
//     env->lv[ls->dvar.var.chv].type = VE_NULL;
//     DPARSER_NEXT

    
//     if (T_IN != ls->t.token) {
//         logger->error(ls->_row,":",ls->_col," for condition missing in symbol");
//         return ERR_END;
//     }
//     DPARSER_NEXT

//     Instruction second;
//     if ( T_UDATA != ls->t.token && T_INT != ls->t.token ) {
//         logger->error(ls->_row,":",ls->_col," for iterator type is not correct");
//         return ERR_END;
//     }
//     second.left = ls->dvar;//这个值到时候可以改改
//     second.type = OC_NULL;
//     second.curpos = forstate->codes.size();
//     forstate->codes.push_back(second);
//     //迭代类型变量
//     assign.left = "for";
//     assign.left.type = VE_USER;
//     assign.curpos = forstate->codes.size();
//     assign.rpos = second.curpos;
//     env->lv["for"] = 0;
//     env->lv["for"].type = VE_NULL;
//     forstate->codes.push_back(assign);

//     DPARSER_NEXT
//     size_t forstart = forstate->codes.size();

//     //判断结束符号是否正确
//     if (T_COLON != ls->t.token) {
//         logger->error(ls->_row,":",ls->_col," for condition judgment error");
//         return ERR_END;
//     }
//     //line
//     DPARSER_NEXT

//     //解析内部for函数
//     parse_Func(*forstate);
//     if ( T_PASS != ls->t.token) {
//         logger->error("missing pass end in for statement");
//         return ERR_END;
//     }
//     //pass
//     DPARSER_NEXT

//     //自增操作 区间
// //--------------------------------------------------------------------------------------- 
//     Instruction incre;
//     incre.type = OC_NULL;
//     incre.left = 1;
//     incre.curpos = forstate->codes.size();
//     forstate->codes.push_back(incre);
    
//     Instruction add;
//     add.type = OC_ADD;
//     add.left = varname.c_str();
//     add.left.type = VE_USER;
//     add.rpos = incre.curpos;
//     add.curpos = forstate->codes.size();
//     forstate->codes.push_back(add);

//     assign.left = varname.c_str();
//     assign.left.type = VE_USER;
//     assign.rpos = add.curpos;
//     assign.curpos = forstate->codes.size();
//     forstate->codes.push_back(assign);
    
// //---------------------------------------------------------------------------------------    

//     //比较大小是否跳转
//     Instruction cmp;
//     cmp.curpos = forstate->codes.size();
//     cmp.type = OC_GT;
//     cmp.left = varname.c_str();
//     cmp.left.type = VE_USER;
//     cmp.rpos = second.curpos;
//     forstate->codes.push_back(cmp);
//     Instruction jmp;
//     jmp.lpos = forstart - 1;
//     jmp.curpos = forstate->codes.size();
//     jmp.type = OC_JMP;
//     jmp.rpos = cmp.curpos;
//     forstate->codes.push_back(jmp);

//     func.lfuns.push_back(forstate);
    
    return 0;//正常退出
}

size_t DParser::while_expr(FunHead& func) {
    if ( T_PASS == ls->t.token ) {
        return 0;
    }
    FIND_NEXT

    Instruction inc;
    inc.curpos = func.codes.size();
    //记录跳转的函数是第几个
    inc.lpos = (int)this->env_stack_top()->cur->lfuns.size();
    inc.type = OC_WHILE;
    func.codes.push_back(inc);


    D_ENV* e = new D_ENV();
    e->anonymous = true;
    e->prev = env_stack_top();
    env_stack_top()->cur->lfuns.push_back(e->cur);
    this->env.push_back(e);
    

    Instruction jmp;
    jmp.type = OC_JMP;
    jmp.curpos = e->cur->codes.size();
    jmp.lpos = FIN_END; // 跳转到最后位置
    jmp.rpos = simple_expr(*e->cur);
    e->cur->codes.push_back(jmp);

    if ( T_COLON != ls->t.token ) {
        logger->error(ls->_row,":",ls->_col," while condition judgment error");
        return ERR_END;
    }
    FIND_NEXT

    if(parse_Func(*e->cur)) {
        return ERR_END;
    }

    //第二次跳转到头部
    jmp.curpos = e->cur->codes.size();
    //-1是跳转到头部 0
    jmp.lpos = -1; // 跳转到最后位置
    jmp.rpos = FIN_END;
    e->cur->codes.push_back(jmp);


    delete env_stack_top();
    this->env.pop_back();

    // if 直接结束
    if ( T_PASS != ls->t.token) {
        //pass
        logger->error(ls->_row,":",ls->_col," missing pass end in while statement");
        return ERR_END;
    }
    FIND_NEXT
    //end
    if ( T_END != ls->t.token ) {
        logger->error(ls->_row,":",ls->_col,"end error in while statement");
        return ERR_END;
    }
    FIND_NEXT

    return 0;
}

size_t DParser::array_element_expr(const std::string& name,FunHead& fun){
    Instruction inc;
    inc.left = name.c_str();
    D_VAR* tmpobj = variable_check(inc.left.var.chv,this->env_stack_top());
    
    if ( !tmpobj ){
        logger->error(ls->_row,":",ls->_col," array or union is not exist");
        return ERR_END;
    }
    //[
    FIND_NEXT
    //用户变量或数字
    inc.right = ls->dvar;
    FIND_NEXT
    //]
    if ( T_RBRACKET != ls->t.token ) {
        logger->error(ls->_row,":",ls->_col," right bracket missing ");
        return ERR_END;
    }
    FIND_NEXT
    if ( T_EQ != ls->t.token ){
        logger->error(ls->_row,":",ls->_col," unkonwn expression error ");
        return ERR_END;
    }
    FIND_NEXT

    if( assign_expr(inc,fun) ) {
        logger->error(ls->_row,":",ls->_col," array assign expression error");
        return ERR_END;
    }
    inc.type = OC_ARR_IAS;

    inc.curpos = fun.codes.size();
    fun.codes.push_back(inc);
    
    return 0;
}

size_t DParser::function_expr(FunHead& func){

    if ( T_PASS == ls->t.token ) {
        return 0;
    }
    
    FIND_NEXT

    if ( T_UDATA != ls->t.token ) {
        logger->error(ls->_row,":",ls->_col," illegal function name ");
        return ERR_END;
    }

    std::string funcname = ls->dvar.var.chv;

    Instruction inc;
    inc.type = OC_NULL;
    inc.left = (int)func.lfuns.size();
    inc.left.type = VE_FUNC;
    inc.curpos = func.codes.size();
    func.codes.push_back(inc);

    Instruction ass;
    ass.type = OC_ASSIGN;
    ass.lpos = VA_DEFAULT;
    ass.left = funcname.c_str();
    ass.rpos = inc.curpos;
    ass.curpos = func.codes.size();
    func.codes.push_back(ass);


    size_t curfunpos = this->env_stack_top()->cur->lfuns.size();

    if ( curfunpos >= QD_SIZE_MAX ) {
        logger->error("function offset is larger than int32 max");
        return ERR_END;
    }

    this->env_stack_top()->lv[funcname] = (int)curfunpos;
    this->env_stack_top()->lv[funcname].type = VE_FUNC;

    FIND_NEXT

    if ( T_LPARENTH != ls->t.token ) {
        logger->error("missing left  parentheses ");
        this->env_stack_top()->lv.erase(funcname);
        return ERR_END;
    }
    FIND_NEXT


    //添加当前函数进入到function里面去
    D_ENV* e = new D_ENV();
    e->prev = env_stack_top();
    env_stack_top()->cur->lfuns.push_back(e->cur);
    this->env.push_back(e);


    //进入函数前，添加函数参数
    while ( T_UDATA == ls->t.token )
    {
        //存入变量
        if ( VE_USER != ls->dvar.type ) {
            //如果参数类型不是用户变量报错
            logger->error("variable is not user type");
            this->env_stack_top()->lv.erase(funcname);
            return ERR_END;
        }
        //添加参数变量
        e->cur->args.push_back(ls->dvar.var.chv);

        this->env_stack_top()->lv[ls->dvar.var.chv] = 0;
        this->env_stack_top()->lv[ls->dvar.var.chv].type = VE_NULL;

        FIND_NEXT
        //右括号退出
        if ( T_RPARENTH == ls->t.token ) {
            break;
        }
        //判断逗号
        else if ( T_COMMA != ls->t.token ) {
            logger->error("missing comma between variable in function ");
            this->env_stack_top()->lv.erase(funcname);
            return ERR_END;
        }

        FIND_NEXT
    }

    
    if ( T_RPARENTH != ls->t.token ) {
        logger->error("missing rigth  parentheses ");
        this->env_stack_top()->lv.erase(funcname);
        return ERR_END;
    }

    
    FIND_NEXT

    if ( T_COLON != ls->t.token ) {
        logger->error(ls->_row,":",ls->_col," error ending function statement ");
        this->env_stack_top()->lv.erase(funcname);
        return ERR_END;
    }
    
    FIND_NEXT // end
    
    //进入函数解析
    if(parse_Func(*e->cur)) {
        return ERR_END;
    }

    //函数默认必须添加return返回不管有没有
    Instruction rtt;
    rtt.type = OC_RET;
    rtt.left = QD_KYW_RET;
    rtt.curpos = e->cur->codes.size();
    e->cur->codes.push_back(rtt);

    delete env_stack_top();
    this->env.pop_back();


    if ( T_PASS != ls->t.token ) {
        logger->error("missing pass end in function statement");
        this->env_stack_top()->lv.erase(funcname);
        return ERR_END;
    }

    FIND_NEXT
    
    return 0;
}

size_t DParser::array_expr(const std::string& name,FunHead& fun){

    for (;;) {
        Instruction inc;

        FIND_NEXT

        if ( T_EOF == ls->t.token ) {
            logger->error(ls->_row,":",ls->_col," parser end error");
            return ERR_END;
        }
        else if ( T_END == ls->t.token ) {
            logger->error(ls->_row,":",ls->_col," missing right bracket ");
            return ERR_END;        
        }
        //数组名
        inc.left = name.c_str();
        //值
        inc.right = ls->dvar;
        inc.type = OC_ARR_VAL;
        inc.curpos = fun.codes.size();
        fun.codes.push_back(inc);
        
        FIND_NEXT
        if ( T_RBRACKET == ls->t.token ) {
            //正常退出
            break;
        }
        else if ( T_COMMA != ls->t.token ) {
            logger->error(ls->_row,":",ls->_col," missing comma in array or array is null");
            return ERR_END;
        }
    }

    if ( T_RBRACKET != ls->t.token ) {
        logger->error(ls->_row,":",ls->_col," missing right bracket ");
        return ERR_END;
    }


    FIND_NEXT
    return 0;
}

size_t DParser::list_access_expr(const std::string& name,FunHead& func){
    // DPARSER_NEXT
    // std::string varname = ls->dvar.var.chv;
    // if ( T_END == ls->t.token ) {
    //     Instruction ll;
    //     ll.type = OC_NULL;
    //     ll.left = 0;
    //     ll.curpos = func.codes.size();
    //     func.codes.push_back(ll);
        
    //     Instruction rr;
    //     rr.type = OC_NULL;
    //     rr.left = QD_INT_32_MAX;
    //     rr.curpos = func.codes.size();
    //     func.codes.push_back(rr);
        
    //     Instruction inc;
    //     inc.type = OC_ARR_LIST;
    //     inc.lpos = ll.curpos;
    //     inc.rpos = rr.curpos;
    //     inc.left = varname.c_str();

    //     inc.curpos = func.codes.size();
    //     func.codes.push_back(inc);
    //     Instruction ass;
    //     ass.type = OC_ARR_LASSIGN;
    //     ass.left = name.c_str();
    //     ass.rpos = inc.curpos;
    //     ass.curpos = func.codes.size();
    //     func.codes.push_back(ass);
    //     // func.proto->lv[name] = 0;
    //     // func.proto->lv[name].type = VE_UNION;
    //     return 0;
    // }
    // else  if ( T_LBRACKET != ls->t.token ) {
    //     logger->error(ls->_row,":",ls->_col," list access expression error");
    //     return ERR_END;
    // }

    // DPARSER_NEXT

    // int ret = parse_PreCode(ls->t.token);
    // if ( OC_NIL != ret ){
    //     DPARSER_NEXT
    // }
    
    // D_VAR left = 0;
    // if ( T_INT == ls->t.token ) {
    //     left = ls->dvar.var.iv ;
    //     DPARSER_NEXT
    // }
    // else if ( T_UDATA == ls->t.token ) {
    //     left = ls->dvar;
    //     DPARSER_NEXT
    // }

    // //如果只有一个负数符号
    // if ( T_COLON != ls->t.token && T_MINUS == ls->lookahead.token ) {
    //     logger->error(ls->_row,":",ls->_col," missing int value with minus symbol");
    //     return ERR_END;
    // }
    
    // //如果访问一个下标
    // if ( T_RBRACKET == ls->t.token ) {
    //     Instruction inc;
    //     inc.type = OC_ARR_ACESS;
    //     inc.left = varname.c_str();
    //     inc.right = left;
    //     inc.curpos = func.codes.size();
    //     func.codes.push_back(inc);
        
    //     Instruction assign;
    //     assign.left = name.c_str();
    //     assign.type = OC_ASSIGN;
    //     assign.rpos = inc.curpos;
    //     assign.curpos = func.codes.size();
    //     func.codes.push_back(assign);

    //     DPARSER_NEXT

    //     if ( T_END != ls->t.token ) {
    //         logger->error(ls->_row,":",ls->_col," error ending in array assign");
    //         return ERR_END;
    //     }

    //     return 0;
    // }
    // else if ( T_COLON != ls->t.token ) {
    //     logger->error(ls->_row,":",ls->_col," is not colon symbol");
    //     return ERR_END;
    // }
    
    // DPARSER_NEXT

    // ret = parse_PreCode(ls->t.token);
    // if (OC_NIL != ret){
    //     DPARSER_NEXT
    // }
    
    // D_VAR right = QD_INT_32_MAX;

    // if ( T_INT == ls->t.token ) {
    //     right = ret == OC_MINUS ?  -ls->dvar.var.iv :  ls->dvar.var.iv;
    //     DPARSER_NEXT
    // }
    // else if ( T_UDATA == ls->t.token ) {
    //     right = ls->dvar;
    //     DPARSER_NEXT
    // }

    // if ( ( OC_MINUS == ret && T_INT != ls->lookahead.token ) 
    // || ( T_UDATA != ls->lookahead.token && T_INT != ls->lookahead.token ) ) {
    //     logger->error(ls->_row,":",ls->_col," missing int value with minus symbol");
    //     return ERR_END;
    // }

    // if ( T_RBRACKET != ls->t.token ) {
    //     logger->error(ls->_row,":",ls->_col," missing right brace symbol");
    //     return ERR_END;
    // }

    // Instruction ll;
    // ll.type = OC_NULL;
    // ll.left = left;
    // ll.curpos = func.codes.size();
    // func.codes.push_back(ll);
    
    // Instruction rr;
    // rr.type = OC_NULL;
    // rr.left = right;
    // rr.curpos = func.codes.size();
    // func.codes.push_back(rr);

    // Instruction alist;
    // alist.type = OC_ARR_LIST;
    // alist.lpos = ll.curpos;
    // alist.rpos = rr.curpos;
    // alist.left = varname.c_str();
    // alist.curpos = func.codes.size();
    // func.codes.push_back(alist);

    // Instruction assign;
    // assign.type = OC_ARR_LASSIGN;
    // assign.left = name.c_str();
    // assign.rpos = alist.curpos;
    // assign.curpos = func.codes.size();
    // func.codes.push_back(assign);

    // DPARSER_NEXT

    // func.proto->lv[name] = 0;
    // func.proto->lv[name].type = VE_UNION;

    return 0;
}

size_t DParser::simple_expr(FunHead& fun){
    if ( T_END == ls->t.token || T_COLON == ls->t.token ) {
        logger->error("missing expression after equal");
        return ERR_END;
    }
    
    std::vector<size_t> values; // 存放计算值位置
    //存放运算符
    std::vector<int> ops; //存放符号运算token

    int negative = 0;
    // () 个数
    size_t path = 0;
    //函数标识
    bool funflag = false;

    while ( T_END != ls->t.token && T_COLON != ls->t.token ) {
        int tok = ls->t.token;
        
        if ( T_EOF == tok ) {
            logger->error("simple expression end error");
            return ERR_END;
        }
        else if ( T_EQ == tok ) {
            logger->error("equal operator is not allow in simple expression");
            return ERR_END;
        }
        else if ( ls->is_variable(tok) ) {
            //用户变量判断是否存在
            D_VAR* tmpobj = nullptr;

            Instruction inc;
            if ( T_UDATA == tok ) {
                tmpobj = variable_check(ls->dvar.var.chv,this->env_stack_top());
                if ( !tmpobj ) {
                    logger->error("variable is undefine ");
                    return ERR_END;
                }
            }

            inc.left = ls->dvar;

            if (tmpobj) {
                //函数
                if ( VE_FUNC == tmpobj->type ) {
                    FIND_NEXT
                    //函数调用                
                    if(call_expr(inc.left.var.chv,fun)) {
                        logger->error(ls->_row,":",ls->_col," call  error");
                        return ERR_END;
                    }
                    inc.left = QD_KYW_RET;
                    inc.left.type = VE_USER;

                    funflag = true;
                }
                else if ( VE_LIB == tmpobj->type ) {
                    FIND_NEXT

                    if (lib_expr(fun)) {
                        logger->error("lib parse error in simple expression ");
                        return ERR_END;
                    }
                    inc.left = QD_KYW_RET;
                    inc.left.type = VE_USER;
                    funflag = true;
                }
                //数组
                else if ( VE_UNION == tmpobj->type ) {
                    inc.left.type = VE_UNION;

                    FIND_NEXT
                    //判断是否为[
                    if ( T_LBRACKET != ls->t.token ) {
                        inc.type = OC_NULL;
                        inc.curpos = fun.codes.size();
                        //暂时不需要处理负数前缀
                        // if (negative % 2 != 0) {
                        //     symbol_reversal(inc); // 处理负数前缀
                        //     --negative;
                        // }
                        fun.codes.push_back(inc);
                        values.push_back(inc.curpos);
                        continue;
                    }
                    FIND_NEXT
                    if ( T_UDATA != ls->t.token && T_INT != ls->t.token ) {
                        logger->error("array index type error");
                        return ERR_END;
                    }
                    
                    Instruction arr;
                    arr.type = OC_NULL;
                    arr.left = ls->dvar;
                    arr.curpos = fun.codes.size();
                    fun.codes.push_back(arr);
                    inc.rpos = arr.curpos;
                    inc.type = OC_ARR_ACE;
                    FIND_NEXT
                    if ( T_RBRACKET != ls->t.token ) {
                        logger->error("right bracket error");
                        return ERR_END;
                    }
                }
                else {
                    inc.type = OC_NULL;
                }
            }
            
            inc.curpos = fun.codes.size();

            if (negative % 2 != 0) {
                symbol_reversal(inc); // 处理负数前缀
                --negative;
            }
            fun.codes.push_back(inc);
            values.push_back(inc.curpos);

            //是否为库函数调用
            if ( tmpobj ) 
                if ( VE_LIB == tmpobj->type )
                    continue;

        } else if ( T_MINUS == ls->t.token ) {
            if ( ls->lookahead.token != T_RPARENTH 
            && !ls->is_variable(ls->lookahead.token) ){
                // 负数前缀检测
                ++negative;
            }
            else {
            Instruction tmp;
                while (!ops.empty() && 
            priority[parse_Opr(tmp,ops.back())] >= priority[parse_Opr(tmp,tok)] ) {
                    size_t val2 = values.back();
                    values.pop_back();
                    size_t val1 = values.back();
                    values.pop_back();
                    int op = ops.back();
                    ops.pop_back();
                    Instruction inc;
                    inc.lpos = val1;
                    inc.rpos = val2;
                    parse_Opr(inc,op);
                    inc.curpos = fun.codes.size();
                    values.push_back(inc.curpos);
                    fun.codes.push_back(inc);
                }
                ops.push_back(tok);
            }
        } else if ( T_LPARENTH == tok && !ls->is_variable(ls->lookahead.token) ) {
            if ( T_MINUS == ls->lookahead.token 
            && ( ls->is_operator(ls->prevhead.token) 
            ||   T_LPARENTH == ls->prevhead.token ) ) {
                --negative;
                ops.push_back(T_XMINUS);
            }
            ops.push_back(tok);
            path ++;
        } else if ( T_RPARENTH == tok && 
            (
                (
                T_LPARENTH == ls->lookahead.token &&
                ls->is_variable(ls->prevhead.token) 
                )
                ||
                T_RPARENTH == ls->lookahead.token
                ||
                T_RBRACKET == ls->lookahead.token
                ||
                ls->is_variable(ls->lookahead.token)
            )
        ) {
            path --;
            if ( path && ls->lookahead.token == T_RPARENTH ) {
                if (funflag) {
                    logger->error("extra function right parentheses");
                    return ERR_END;
                }
                funflag = false;
            }

            while ( ops.back() != T_LPARENTH && !ls->is_operator(ls->lookahead.token) ) {
                size_t val2 = values.back();
                values.pop_back();
                size_t val1 = values.back();
                values.pop_back();
                int op = ops.back();
                ops.pop_back();
                Instruction inc;
                inc.lpos = val1;
                inc.rpos = val2;
                parse_Opr(inc,op);
                inc.curpos = fun.codes.size();
                values.push_back(inc.curpos);
                fun.codes.push_back(inc);
            }
            ops.pop_back();

            if (!ops.empty() && ops.back() == T_XMINUS ) {
                symbol_reversal(fun.codes[values.back()]);
                ops.pop_back();
            }
        } else if ( ls->is_operator(tok) && 
        ( ls->is_variable(ls->lookahead.token) 
        || ls->lookahead.token == T_RPARENTH 
        || ls->lookahead.token == T_RBRACKET ) ) {
            Instruction tmp;
            while (!ops.empty() && 
            priority[parse_Opr(tmp,ops.back())] >= priority[parse_Opr(tmp,tok)] ) {
                size_t val2 = values.back();
                values.pop_back();
                size_t val1 = values.back();
                values.pop_back();
                int op = ops.back();
                ops.pop_back();
                Instruction inc;
                inc.lpos = val1;
                inc.rpos = val2;
                parse_Opr(inc,op);
                inc.curpos = fun.codes.size();
                values.push_back(inc.curpos);
                fun.codes.push_back(inc);
            }
            ops.push_back(tok);
        }
        else {
            logger->error(ls->_row,":",ls->_col," simple expression error");
            return ERR_END;
        }
        FIND_NEXT
    }


    if (path) {
        logger->error("mismatched left and right parentheses");
        return ERR_END;
    }
    else if ( ls->is_operator(ls->lookahead.token ) ) {
        logger->error("expression endup with operator ");
        return ERR_END;
    }


    while (!ops.empty()) {
        size_t val2 = values.back();
        values.pop_back();
        size_t val1 = values.back();
        values.pop_back();
        int op = ops.back();
        ops.pop_back();
        Instruction inc;
        inc.lpos = val1;
        inc.rpos = val2;
        parse_Opr(inc,op);
        inc.curpos = fun.codes.size();
        values.push_back(inc.curpos);
        fun.codes.push_back(inc);
    }

    return values.back();
}

size_t DParser::call_expr(std::string name,FunHead& fun){
    Instruction inc;
    inc.left = name.c_str();
    inc.type = OC_CALL;
    //这个返回值要进行更改一下
    D_VAR* tmpobj = variable_check(name,this->env_stack_top());
    if ( !tmpobj ) {
        logger->error(ls->_row,":",ls->_col," function name undefine ");
        return ERR_END;
    }

    inc.lpos = tmpobj->var.iv;
    FIND_NEXT

    //参数判断
    size_t stacksize = 0;
    size_t argpos = FIN_END;
    while ( ls->is_variable(ls->t.token) || T_MINUS == ls->t.token )
    {
        Instruction arg;
        arg.type = OC_ARG;

        if ( T_MINUS == ls->t.token ) {
            FIND_NEXT
            if ( T_INT != ls->t.token && T_DECIMAL != ls->t.token && T_UDATA != ls->t.token ) {
                logger->error("function negative is incorrect");
                return ERR_END;
            }
            arg.restype = OC_MINUS;
        }

        arg.right = ls->dvar;
        
        arg.rpos = argpos;
        argpos = arg.curpos = fun.codes.size();
        fun.codes.push_back(arg);

        FIND_NEXT
        stacksize++;
        
        if ( T_RPARENTH == ls->t.token  ) {
            break;
        }
        else if ( T_COMMA != ls->t.token ) {
            logger->error("missing comma between variable in call function ");
            return ERR_END;
        }

        FIND_NEXT

    }

    FunHead* cur = find_function(name,this->env_stack_top());
    //查找当前参数不匹配
    if ( stacksize != cur->lfuns[tmpobj->var.iv]->args_size() ) {
        logger->error("function args number error ");
        return ERR_END;
    }
    

    if ( T_RPARENTH != ls->t.token ) {
        logger->error(ls->_row,":",ls->_col," function call missing right parentheses");
        return ERR_END;
    }
    // DPARSER_NEXT
    //最后添加
    inc.rpos = argpos;
    inc.curpos = fun.codes.size();
    fun.codes.push_back(inc);

    return 0;
}

size_t DParser::lib_expr(FunHead& fun) {
    Instruction inc;
    inc.left = ls->dvar;

    //如果存在 local global 关键字 报错
    if ( T_GLOBAL == ls->prevhead.token || T_LOCAL == ls->prevhead.token ) {
        logger->error("can not use global or local with lib ");
        return ERR_END;
    }

    //库函数调用步骤
    if ( T_PERIOD != ls->t.token ) {
        logger->error("lib call without period");
        return ERR_END;
    }

    FIND_NEXT
    
    inc.right = ls->dvar;
    //查找调用函数名是否存在
    if ( !this->lib->is_fun(inc.left.var.chv,inc.right.var.chv) ) {
        logger->error("is not exist function name ",ls->dvar.var.chv," in ",inc.left.var.chv);
        return ERR_END;
    }

    //定义库函数调用指令集
    inc.type = OC_LIB;
    
    FIND_NEXT

    if ( T_LPARENTH != ls->t.token ) {
        logger->error("missing left parentheses ");
        return ERR_END;
    }

    FIND_NEXT

    //参数个数
    size_t nums = 0; 
    //参数获取
    while ( ls->is_variable(ls->t.token) || T_MINUS == ls->t.token ) {
        Instruction add;
        add.type = OC_LIBV;
        add.curpos = fun.codes.size();
        if ( T_MINUS == ls->t.token ) {
            FIND_NEXT
            if ( T_INT != ls->t.token && T_DECIMAL != ls->t.token && T_UDATA != ls->t.token ) {
                logger->error("lib function negative is incorrect");
                return ERR_END;
            }
            add.restype = OC_MINUS;
        }

        add.left = ls->dvar;

        fun.codes.push_back(add);
        nums++;
        
        //判断用户变量是否存在
        if ( VE_USER == ls->dvar.type ) {
            D_VAR* tmpvar = variable_check(ls->dvar.var.chv,this->env_stack_top());
            if (!tmpvar) {
                logger->error("lib fun args do not exist this variable");
                return ERR_END;
            }
        }

        FIND_NEXT
        if ( T_RPARENTH == ls->t.token ) {
            break;
        }
        else if ( T_COMMA != ls->t.token ) {
            logger->error("missing comma between lib function call");
            return ERR_END;
        }

        FIND_NEXT
    }

    if ( T_RPARENTH != ls->t.token ) {
        logger->error("missing right parentheses ");
        return ERR_END;
    }

    inc.curpos = fun.codes.size();
    //参数个数
    inc.lpos = nums;

    fun.codes.push_back(inc);
    
    FIND_NEXT

    return 0;
}

D_VAR* DParser::variable_check(const std::string& name,D_ENV* fun) {
    D_ENV* tmpfun = fun;
    while ( tmpfun )
    {
        if ( tmpfun->lv.find(name) != tmpfun->lv.end() ) {
            return &tmpfun->lv[name];
        }
        tmpfun = tmpfun->prev;
    }
    return nullptr;
}

FunHead* DParser::find_function(const std::string& name,D_ENV* fun) {
    D_ENV* tmp = fun;

    while ( tmp )
    {
        if ( tmp->lv.find(name) != tmp->lv.end() ) {
            return tmp->cur;
        }
        tmp =  tmp->prev;
    }

    return nullptr;
}

D_ENV* DParser::env_stack_top() {
    return this->env.back();
}

D_ENV* DParser::last_env(D_ENV* info) {
    size_t i = this->env.size() - 1;
    if ( i < 0 ) return nullptr;
    D_ENV* tmpin =  this->env[i];

    //匿名函数查找
    while (tmpin)
    {
        --i;
        if ( i < 0 ) return nullptr;
        tmpin = this->env[i];
        if (!tmpin->anonymous) return tmpin; 
    }
    return nullptr;
}

D_ENV* DParser::env_stack_head() {
    return this->env.front();
} 

void DParser::init_io(Dio* const io) {
    this->ls->alloc_io(io);
}

size_t DParser::io_size() {
    return this->ls->io->buffs.size();
}

D_UNION DParser::union_access(int start, int end,const D_UNION& arr){
    D_UNION tmp;
    int i ;
    int len = arr.larr.size();
    if ( start < 0 ) {
        start = len + start + 1;
    }

    if ( end < 0 ) {
        end = len + end + 1;
    }
    else if ( end == QD_SIZE_MAX ) {
        end = len;
    }
    else if ( end > len ) {
        logger->warn("array index out of range");
    }

    if ( end < start ) {
        logger->warn("array is null");
    }

    for ( i = start ; i < len && i < end; i++ ) {
        tmp.larr.push_back(arr.larr[i]);
    }
    
    return tmp; 
}

size_t DParser::read_file(const char* file){
    std::ifstream is (file, std::ifstream::binary);
    if (is) {
        struct stat statbuf;
	    stat(file, &statbuf);
        //获取文件大小
        size_t length = statbuf.st_size;
        // get length of file:
        // is.seekg (0, is.end);
        // int length = is.tellg();
        // is.seekg (0, is.beg);
        
        size_t chunk = QD_BUF_BLK_SIZ;

        // 计算需要切片的次数
        unsigned  int numberOfChunks = length / chunk;
        logger->debug("input file size is ",length);

        // 读取每个数据块并进行处理
        for (size_t i = 0; i < numberOfChunks; ++i) {
            char * buffer = new char[chunk + 1];
            is.read(buffer, chunk);
            buffer[chunk] = '\0'; //末尾一定要加
            // std::cout.write(buffer,chunk + 1);
            //分配流空间
            ls->alloc_buff(buffer,chunk);

            delete[] buffer;
        }

        //处理剩余字符串
        int rest = length % chunk;
        char * buffer = new char[ rest + 1 ];
        buffer[rest] = '\0'; //末尾一定要加
        is.read( buffer, rest );
        ls->alloc_buff(buffer,rest);

        delete[] buffer;

        is.close();

        logger->debug("file block size is ",this->ls->io->buffs.size());
    }
    else {
        logger->error("file open error ");
        return ERR_END;
    }
    
    return 0;
}

size_t DParser::read_line(const char* line,size_t len){
    // this->parse(line);
    ls->alloc_buff(line,len);
    return 0;
}

size_t DParser::init_lib(D_LIB* lib) {
    if (lib == nullptr) {
        return 1;
    }
    this->lib = lib;
    if (this->lib == nullptr) {
        return 1;
    }


    //变量加载
    for ( auto& i : this->lib->l ) {
        this->env_stack_head()->lv[i.first] = 0;
        this->env_stack_head()->lv[i.first].type = VE_LIB;
    }

    return 0;
}


_QD_END

