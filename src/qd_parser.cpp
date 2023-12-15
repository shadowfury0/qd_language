#include "qd_parser.h"
#include "qd_funclib.h"

_QD_BEGIN


DParser::DParser(){
    logger = Logger::getInstance();

    // env = nullptr;
    // env = new D_ENV();
    ls = nullptr;
    ls = new LexState();


    //初始化环境变量
    D_ENV* env = new D_ENV();
    this->env.push_back(env);
}

DParser::~DParser(){
    if ( ls != nullptr ) {
        delete ls;
        ls = nullptr;
    }
    //不进行释放内存，只是暂时检测用的

    // std::cout << " env stack size is " << this->env.size() << std::endl;
    this->env_clear();
}

void DParser::env_clear() {
    for ( auto i : this->env ) {
        if (i != nullptr) {
            delete i;
        }
        i = nullptr;
    }
    //清空所有元素
    this->env.clear();
}

size_t DParser::parseX_next(){

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
    if (this->findX_next()) {
        return ERR_END;
    }

    //这里开始全局解析
    if (parse_Func(*this->env_stack_head()->cur)) {
        return ERR_END;
    }

    logger->debug("global bytes code total is ",this->env_stack_head()->cur->codes.size());
    return 0;
}

size_t DParser::parse_Func(FunHead& fun){
    logger->debug(" <------------------ start --------------------> ");
    
    for(;;){
        //因为有if fun while等情况
        if ( T_END == ls->t.token ) {
            if(this->findX_next()) {
                return ERR_END;
            }
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
            this->findX_next();
            break;
        }
        case T_UDATA:
        case T_GLOBAL:
        case T_LOCAL:
        {
            if(statement(fun)) {
                logger->error("user statement error");
                return ERR_END;
            }
            break;
        }
        case T_JUMP:{
            //跳转指令
            if(jump_expr(fun)) {
                logger->error("jump error");
                return ERR_END;
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
                return ERR_END;
            }
            break;
        }
        case T_RETURN: {
            Instruction ret;
            ret.curpos = fun.codes.size();
            ret.left = QD_KYW_RET;
            this->findX_next();

            if ( T_END != ls->t.token ) {
                if( assign_expr(ret,fun) ){
                    logger->error(ls->_row,":",ls->_col," return assign expression error");
                    return ERR_END;
                }
            }
            ret.type = OC_RET;
            fun.codes.push_back(ret);

            // Instruction ret;
            // ret.curpos = fun.codes.size();
            // ret.type = OC_RET;
            // fun.codes.push_back(ret);
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
        default:{
            this->ls->_row ++ ;
            logger->info("do nothing  ");
            break;
        }
        }

    }

    
    logger->debug(" <------------------ end --------------------> ");
    
    return 0;
}

size_t DParser::skip_blank(){
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

size_t DParser::findX_next(){
    if (this->parseX_next()) {
        return ERR_END;
    }
    this->skip_blank();
    return 0;
}

size_t DParser::parse_Opr(Instruction& inc,short type){
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

size_t DParser::symbol_reversal(Instruction& inc){
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
        findX_next();
    }
    else if ( T_LOCAL == ls->t.token ) {
        inc.lpos = VA_LOCAL;
        findX_next();
    }
    else {
        inc.lpos = VA_DEFAULT;
    }

    if ( T_UDATA != ls->t.token ) {
        logger->error(ls->_row,":",ls->_col," data error");
        return ERR_END;
    }

    //记录左值
    inc.left = ls->dvar;
    this->findX_next();

    //下一个如果是函数调用的话
    if ( T_LPARENTH == ls->t.token ) {
        if(call_expr(inc.left.var.chv,fun)) {
            logger->error(ls->_row,":",ls->_col," call  error");
            return ERR_END;
        }
        this->findX_next();
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

    this->findX_next();

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
            logger->error(ls->_row,":",ls->_col," assign expression error");
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
    else{
        logger->error("variable type is error ");
        return ERR_END;
    }

    switch (assigntype)
    {
    case VE_STR: {
        Instruction tmp;
        tmp.left = ls->dvar;
        tmp.curpos = fun.codes.size();
        fun.codes.push_back(tmp);
        this->findX_next();
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
    findX_next();
    //不是int类型
    if ( T_INT != ls->t.token ) {
        logger->error("jumping num  type is error");
        findX_next();
        return ERR_END;
    }
    //超出范围
    if ( ls->dvar.var.iv < 0) {
        logger->warn(" jump less than zero ");
        findX_next();
        return ERR_END;
    }
    
    findX_next();
    //加入指令
    Instruction inc;
    inc.type = OC_JMP;
    inc.lpos = ls->dvar.var.iv - 1; 
    inc.curpos = fun.codes.size();
    fun.codes.push_back(inc);
    findX_next();

    return 0;
}

size_t DParser::if_expr(FunHead& func){
    
    if ( T_PASS == ls->t.token ) {
        return 0;
    }
    Instruction inc;

    findX_next();
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
    
    findX_next();//这里通常都是 line 换行符

    D_ENV* e = new D_ENV();
    e->anonymous = true;
    e->prev = env_stack_top();
    env_stack_top()->cur->lfuns.push_back(e->cur);
    this->env.push_back(e);

    parse_Func(*e->cur);

    delete env_stack_top();
    this->env.pop_back();
    
    // if 直接结束
    if ( T_PASS != ls->t.token) {
        //pass
        logger->error(ls->_row,":",ls->_col," missing pass end in if statement");
        return ERR_END;
    }
    findX_next();
    //end
    if ( T_END != ls->t.token ) {
        logger->error(ls->_row,":",ls->_col," missing  end in elif statement");
        return ERR_END;
    }
    findX_next();
    
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
        findX_next();
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
        findX_next();
        
        D_ENV* e = new D_ENV();
        e->anonymous = true;
        e->prev = env_stack_top();
        env_stack_top()->cur->lfuns.push_back(e->cur);
        this->env.push_back(e);

        parse_Func(*e->cur);
        delete env_stack_top();
        this->env.pop_back();

        if ( T_PASS != ls->t.token ) {
            logger->error(ls->_row,":",ls->_col," missing pass end in elif statement");
            return ERR_END;
        }
        findX_next();
        //end
        if ( T_END != ls->t.token ) {
            logger->error(ls->_row,":",ls->_col," missing end in elif statement");
            return ERR_END;
        }
        findX_next();
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

    findX_next();
    //判断结束符号是否正确
    if (T_COLON != ls->t.token) {
        logger->error(ls->_row,":",ls->_col," else condition judgment error");
        return ERR_END;
    }
    findX_next();
    
    D_ENV* e = new D_ENV();
    e->anonymous = true;
    e->prev = env_stack_top();
    env_stack_top()->cur->lfuns.push_back(e->cur);
    this->env.push_back(e);

    parse_Func(*e->cur);

    delete env_stack_top();
    this->env.pop_back();

    if ( T_PASS != ls->t.token ) {
        logger->error(ls->_row,":",ls->_col," missing pass end in else statement");
        return ERR_END;
    }
    findX_next();
    //end
    if ( T_END != ls->t.token ) {
        logger->error(ls->_row,":",ls->_col," missing end in else statement");
        return ERR_END;
    }
    findX_next();

    return 0;
}

size_t DParser::for_expr(FunHead& func){
//     findX_next();

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
//     findX_next();

    
//     if (T_IN != ls->t.token) {
//         logger->error(ls->_row,":",ls->_col," for condition missing in symbol");
//         return ERR_END;
//     }
//     findX_next();

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

//     findX_next();
//     size_t forstart = forstate->codes.size();

//     //判断结束符号是否正确
//     if (T_COLON != ls->t.token) {
//         logger->error(ls->_row,":",ls->_col," for condition judgment error");
//         return ERR_END;
//     }
//     //line
//     findX_next();

//     //解析内部for函数
//     parse_Func(*forstate);
//     if ( T_PASS != ls->t.token) {
//         logger->error("missing pass end in for statement");
//         return ERR_END;
//     }
//     //pass
//     findX_next();

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
    findX_next();

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
    findX_next();
    parse_Func(*e->cur);

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
    findX_next();
    //end
    if ( T_END != ls->t.token ) {
        logger->error(ls->_row,":",ls->_col,"end error in while statement");
        return ERR_END;
    }
    findX_next();

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
    this->findX_next();
    //用户变量或数字
    inc.right = ls->dvar;
    this->findX_next();
    //]
    if ( T_RBRACKET != ls->t.token ) {
        logger->error(ls->_row,":",ls->_col," right bracket missing ");
        return ERR_END;
    }
    this->findX_next();
    if ( T_EQ != ls->t.token ){
        logger->error(ls->_row,":",ls->_col," unkonwn expression error ");
        return ERR_END;
    }
    this->findX_next();

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
    
    this->findX_next();

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

    if ( curfunpos >= QD_INT_32_MAX ) {
        logger->error("function offset is larger than int32 max");
        return ERR_END;
    }
    this->env_stack_top()->lv[funcname] = (int)curfunpos;
    this->env_stack_top()->lv[funcname].type = VE_FUNC;

    this->findX_next();

    if ( T_LPARENTH != ls->t.token ) {
        logger->error("missing left  parentheses ");
        return ERR_END;
    }
    this->findX_next();


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
            return ERR_END;
        }
        //添加参数变量
        e->cur->args.push_back(ls->dvar.var.chv);

        this->env_stack_top()->lv[ls->dvar.var.chv] = 0;
        this->env_stack_top()->lv[ls->dvar.var.chv].type = VE_NULL;

        this->findX_next();
        //右括号退出
        if ( T_RPARENTH == ls->t.token ) {
            break;
        }
        //判断逗号
        else if ( T_COMMA != ls->t.token ) {
            logger->error("missing comma between variable in function ");
            return ERR_END;
        }
        this->findX_next();
    }

    
    if ( T_RPARENTH != ls->t.token ) {
        logger->error("missing rigth  parentheses ");
        return ERR_END;
    }

    
    this->findX_next();

    if ( T_COLON != ls->t.token ) {
        logger->error(ls->_row,":",ls->_col," error ending function statement ");
        return ERR_END;
    }
    
    this->findX_next(); // end
    
    //进入函数解析
    parse_Func(*e->cur);

    delete env_stack_top();
    this->env.pop_back();


    if ( T_PASS != ls->t.token ) {
        logger->error("missing pass end in function statement");
        return ERR_END;
    }
    this->findX_next();
    
    return 0;
}

size_t DParser::array_expr(const std::string& name,FunHead& fun){

    for (;;) {
        Instruction inc;

        this->findX_next();

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
        
        this->findX_next();
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


    this->findX_next();
    return 0;
}

size_t DParser::list_access_expr(const std::string& name,FunHead& func){
    // this->findX_next();
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

    // this->findX_next();

    // int ret = parse_PreCode(ls->t.token);
    // if ( OC_NIL != ret ){
    //     findX_next();
    // }
    
    // D_VAR left = 0;
    // if ( T_INT == ls->t.token ) {
    //     left = ls->dvar.var.iv ;
    //     this->findX_next();
    // }
    // else if ( T_UDATA == ls->t.token ) {
    //     left = ls->dvar;
    //     this->findX_next();
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

    //     this->findX_next();

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
    
    // this->findX_next();

    // ret = parse_PreCode(ls->t.token);
    // if (OC_NIL != ret){
    //     findX_next();
    // }
    
    // D_VAR right = QD_INT_32_MAX;

    // if ( T_INT == ls->t.token ) {
    //     right = ret == OC_MINUS ?  -ls->dvar.var.iv :  ls->dvar.var.iv;
    //     this->findX_next();
    // }
    // else if ( T_UDATA == ls->t.token ) {
    //     right = ls->dvar;
    //     this->findX_next();
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

    // this->findX_next();

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

    while ( T_END != ls->t.token && T_COLON != ls->t.token ) {
        int tok = ls->t.token;
        
        if ( T_EOF == tok ) {
            logger->error("simple expression end error");
            return ERR_END;
        }
        if ( ls->is_variable(tok) ) {
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
                //这里判断是函数调用还是普通成员变量,还是数组
                if ( VE_FUNC == tmpobj->type ) {
                    this->findX_next();
                    //函数调用                
                    if(call_expr(inc.left.var.chv,fun)) {
                        logger->error(ls->_row,":",ls->_col," call  error");
                        return ERR_END;
                    }
                    inc.left = QD_KYW_RET;
                    inc.left.type = VE_USER;
                }
                //数组
                else if ( VE_UNION == tmpobj->type ) {
                    inc.left.type = VE_UNION;

                    this->findX_next();
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
                    this->findX_next();
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
                    this->findX_next();
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
        } else if ( T_RPARENTH == tok  ) {
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
            path -- ;
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
        this->findX_next();
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
    this->findX_next();


    //参数判断
    size_t stacksize = 0;
    size_t argpos = FIN_END;
    while ( ls->is_variable(ls->t.token) )
    {
        Instruction arg;
        arg.type = OC_ARG;
        arg.right = ls->dvar;
        arg.rpos = argpos;
        argpos = arg.curpos = fun.codes.size();
        fun.codes.push_back(arg);

        this->findX_next();
        stacksize++;
        if ( T_RPARENTH == ls->t.token ) {
            break;
        }
        else if ( T_COMMA != ls->t.token ) {
            logger->error("missing comma between variable in call function ");
            return ERR_END;
        }
        this->findX_next();
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
    // this->findX_next();
    //最后添加
    inc.rpos = argpos;
    inc.curpos = fun.codes.size();
    fun.codes.push_back(inc);

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
        if (!tmpin->anonymous) return tmpin; 
        --i;
        if ( i < 0 ) return nullptr;
        tmpin = this->env[i];
    }
    return nullptr;
}

D_ENV* DParser::env_stack_head() {
    return this->env.front();
} 

void DParser::init_io(Dio* const io) {
    this->ls->alloc_io(io);
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
    else if ( end == QD_INT_32_MAX ) {
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



_QD_END

