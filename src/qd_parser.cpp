#include "qd_parser.h"


_QD_BEGIN


DParser::DParser(){
    logger = Logger::getInstance();

    global = nullptr;
    global = new FunState();
}

DParser::~DParser(){
    if ( global != nullptr ) {
        delete global;
        global = nullptr;
    }
    
    if ( !this->funstack.empty() ) {
        for ( FunState* iter = this->funstack.back() ; 
        iter != this->funstack.front() ; -- iter ) {
            if (iter != nullptr){
                delete iter;
                iter = nullptr;
            }
            this->funstack.pop_back();
        }
    }
    // if ( !this->funstack.empty() ) {
    //     for (unsigned int i = this->funstack.size() - 1; i > 0 ; i -- ) {
    //         FunState* iter = this->funstack[i];
    //         // if ( iter != nullptr ) {
    //             std::cout << &iter << std::endl;
    //             delete iter;
    //             iter = nullptr;
    //         // }
    //         this->funstack.pop_back();
    //     }
    // }
    
    
}

unsigned int DParser::parseX_next(){

    if ( T_BLANK != ls.t.token ) {
        this->ls.prevhead.token = this->ls.lookahead.token;
        this->ls.lookahead.token = this->ls.t.token;
    }

    if ( T_ERR == (this->ls.t.token = ls.llex()) ) {
        logger->error("lexical error");
        return ERR_END;
    }
    // logger->debug("debug token info : ",ls.t.token,":",ls.lookahead.token,":",ls.prevhead.token);
    
    return 0;
}

void DParser::parse(const char* str){
    ls.alloc_buff(str);
    //null是函数开始时初始化值
    if ( T_NULL == ls.t.token ) {
        this->findX_next();
    }
    //这里开始全局解析
    unsigned int err = parse_Func(*global);

    //错误的检测
    if (!err) {
        this->funstack.push_back(global);
        //只用解析一次就好了
        err = analyse_code( global->code_pos, *global );

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
        //因为if语句比较特殊不好解决
        if ( T_END == ls.t.token ) {
            this->findX_next();
        }
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
            logger->info("comment line ");
            this->findX_next();
            break;
        }
        case T_UDATA:
        case T_LOCAL:{
            if(statement(fun.code_pos,fun)) {
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
                logger->error(ls._row,":",ls._col," for statement error");
                return ERR_END;
            }
            break;
        }
        case T_WHILE:{
            if ( while_expr(fun) ) {
                logger->error(ls._row,":",ls._col," while statement error");
                return ERR_END;
            }
            break;
        }
        case T_FUNC:{
            if ( function_expr(fun) ) {
                logger->error(ls._row,":",ls._col," function statement error");
                //错误处理
                return ERR_END;
            }
            break;
        }
        case T_RETURN: {
            Instruction inc;
            inc.curpos = fun.codes.size();
            inc.left = QD_KYW_RET;
            this->findX_next();

            if ( T_END != ls.t.token ) {
                if( assign_expr(inc,fun) ){
                    logger->error(ls._row,":",ls._col," return assign expression error");
                    return ERR_END;
                }
            }
            fun.codes.push_back(inc);

            Instruction ret;
            ret.curpos = fun.codes.size();
            ret.type = OC_RET;
            fun.codes.push_back(ret);
            break;
        }
        case T_IF:{
            if( if_expr(fun) ){
                logger->error(ls._row,":",ls._col," if statement error");
                return ERR_END;
            }
            break;
        }
        case T_ELIF:{
            logger->error(ls._row,":",ls._col," only elif statement error");
            return ERR_END;
        }
        case T_ELSE:{
            logger->error(ls._row,":",ls._col," only else statement error");
            return ERR_END;
        }
        default:{
            this->ls._row ++ ;
            logger->info("do nothing  ");
            break;
        }
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

unsigned int DParser::parse_Opr(Instruction& inc,short type){
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

unsigned int DParser::symbol_reversal(Instruction& inc){
    if ( OC_MINUS == inc.restype ) {
        inc.restype = OC_NIL;
    }
    else {
        inc.restype = OC_MINUS;
    }
    return 0;
}

unsigned int DParser::statement(unsigned int& pos,FunState& fun){
    Instruction inc;

    //全局变量
    if ( T_LOCAL == ls.t.token ) {
        inc.right = true;
        findX_next();
    }

    if ( T_UDATA != ls.t.token ) {
        logger->error(ls._row,":",ls._col," data error");
        return ERR_END;
    }

    //记录左值
    inc.left = ls.dvar;
    this->findX_next();

    //暂时不允许空值
    // if ( T_END == ls.t.token ) {
    //     fun.proto->lv[inc.left.var.chv] = 0;
    //     fun.proto->lv[inc.left.var.chv].type = VE_NULL;
    //     return 0;
    // }

    //下一个如果是函数调用的话
    if ( T_LPARENTH == ls.t.token ) {
        if(call_expr(inc.left.var.chv,fun)) {
            logger->error(ls._row,":",ls._col," call  error");
            return ERR_END;
        }
        return 0;
    }
    //数组下标赋值
    else if ( T_LBRACKET == ls.t.token ) {
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

    this->findX_next();

    //数组
    if ( T_LBRACKET == ls.t.token ) {
        if(union_expr(inc.right.var.bv,inc.left.var.chv,fun)){
            logger->error(ls._row,":",ls._col," array parse error");
            return ERR_END;
        }
        return 0;
    }

    if ( assign_expr(inc,fun) ) {
        logger->error(ls._row,":",ls._col," assign expression error");
        return ERR_END;
    }

    fun.codes.push_back(inc);

    return 0;//正常退出
}

unsigned int DParser::assign_expr(Instruction& inc,FunState& fun){
    inc.type = OC_ASSIGN;

    short assigntype;
    if ( T_MINUS == ls.t.token  
    || T_LPARENTH == ls.t.token) {
        //do nothing
    }
    else if ( VE_USER == ls.dvar.type ) {
        D_OBJ* tmpobj = variable_check(ls.dvar.var.chv,fun);
        assigntype = tmpobj->type;

        //判断是否为数组，
        if ( VE_UNION == assigntype ) {
            if ( list_access_expr(inc.left.var.chv,fun) ) {
                logger->error("list access expression error");
                return ERR_END;
            }
            return 0;
        }
        else if ( VE_VOID == assigntype ){
            delete tmpobj;
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
        Instruction tmp;
        tmp.type = OC_NULL;

        D_OBJ* tmpobj = variable_check(ls.dvar.var.chv,fun);
        if ( VE_VOID == tmpobj->type ){
            delete tmpobj;
            logger->error(ls._row,":",ls._col," prefix user data undefine");
            return ERR_END;
        }

        tmp.left = tmpobj->var.iv;
        tmp.left.type = VE_FUNC;
        
        tmp.curpos = fun.codes.size();
        fun.codes.push_back(tmp);
        inc.rpos = tmp.curpos;

        this->findX_next();

        if(call_expr(ls.dvar.var.chv,fun)) {
            logger->error(ls._row,":",ls._col," call  error");
            return ERR_END;
        }

        break;
    } 
    default:{
        inc.rpos = this->simple_expr(fun);
        
        if ( ERR_END == inc.rpos ) {
            logger->error("basic expression error");
            return ERR_END;
        }
        break;
    }
    }

    //先把user_data添加进行进行判断
    if ( T_END != ls.t.token ){
        logger->error(ls._row,":",ls._col," error end");
        return ERR_END;
    }
    
    
    fun.proto->lv[inc.left.var.chv] = 0;
    fun.proto->lv[inc.left.var.chv].type = VE_NULL;
    inc.curpos = fun.codes.size();
    // fun.codes.push_back(inc);
    return 0;
}

unsigned int DParser::jump_expr(FunState& fun){
    logger->debug("jumping to ");
    findX_next();
    //不是int类型
    if ( T_INT != ls.t.token ) {
        logger->error("jumping num  type is error");
        findX_next();
        return ERR_END;
    }
    //超出范围
    if ( ls.dvar.var.iv < 0) {
        logger->warn(" jump out of range ");
        findX_next();
        return ERR_END;
    }
    
    findX_next();
    //加入指令
    Instruction inc;
    inc.type = OC_JMP;
    inc.lpos = ls.dvar.var.iv - 1; 
    inc.curpos = bytes_code_line();
    fun.codes.push_back(inc);

    return 0;
}

unsigned int DParser::if_expr(FunState& func){
    
    if ( T_PASS == ls.t.token ) {
        return 0;
    }
    Instruction inc;

    findX_next();
    //判断跳转函数是否执行
    inc.rpos = simple_expr(func);

    inc.curpos = func.codes.size();
    //记录跳转的函数是第几个
    inc.lpos = func.proto->lfuns.size(); 
    inc.type = OC_IF;      //调用

    func.codes.push_back(inc);

    //判断结束符号是否正确
    if (T_COLON != ls.t.token) {
        logger->error(ls._row,":",ls._col," if condition judgment error");
        return ERR_END;
    }
    
    findX_next();//这里通常都是 line 换行符

    FunState* ifstate = new FunState();
    ifstate->prev = &func;
    ifstate->anonymous = true;
    parse_Func(*ifstate);
    func.proto->lfuns.push_back(ifstate);
    
    // if 直接结束
    if ( T_PASS != ls.t.token) {
        //pass
        logger->error(ls._row,":",ls._col," missing pass end in if statement");
        return ERR_END;
    }
    findX_next();
    //end
    if ( T_END != ls.t.token ) {
        logger->error(ls._row,":",ls._col," missing  end in elif statement");
        return ERR_END;
    }
    findX_next();

    //出现elif
    if ( T_ELIF == ls.t.token ) {
        //解析elif 查找到最后的跳转位置挨个遍历
        if (elif_expr(func)) {
            logger->error(ls._row,":",ls._col," elif expression parse error");
            return ERR_END;
        }        
    }
    
    //else语句
    if ( T_ELSE == ls.t.token ) {
        if (else_expr(func)) {
            logger->error(ls._row,":",ls._col," else expression parse error");
            return ERR_END;
        }
    }

    unsigned int startpos = inc.curpos;
    unsigned int endpos = func.codes.size();//解析都是最后的一个指令肯定是elif或else

    //现在目前是给所有if elif指令赋值
    unsigned int i;
    for (i = startpos; i < endpos ; i++ ) { 
        Instruction& cur = func.codes[i];
        if ( OC_IF == cur.type ) {
            cur.right = endpos;
        } 
    }

    return 0;
}

unsigned int DParser::elif_expr(FunState& func){

    while ( T_ELIF == ls.t.token ) {
        Instruction inc;
        findX_next();
        inc.rpos = simple_expr(func);

        inc.curpos = func.codes.size();
        inc.lpos = func.proto->lfuns.size(); 
        inc.type = OC_IF;      //调用
        func.codes.push_back(inc);

        //判断结束符号是否正确
        if (T_COLON != ls.t.token) {
            logger->error(ls._row,":",ls._col," elif condition judgment error");
            return ERR_END;
        }
        findX_next();
        
        FunState* elifstate = new FunState();
        elifstate->prev = &func;
        elifstate->anonymous = true;
        parse_Func(*elifstate);
        func.proto->lfuns.push_back(elifstate);

        if ( T_PASS != ls.t.token ) {
            logger->error(ls._row,":",ls._col," missing pass end in elif statement");
            return ERR_END;
        }
        findX_next();
        //end
        if ( T_END != ls.t.token ) {
            logger->error(ls._row,":",ls._col," missing end in elif statement");
            return ERR_END;
        }
        findX_next();
    }

    return 0;
}

unsigned int DParser::else_expr(FunState& func){
    Instruction istrue;
    istrue.curpos = func.codes.size();
    istrue.type = OC_NULL;
    istrue.left = true;
    func.codes.push_back(istrue);

    Instruction inc;
    findX_next();
    inc.rpos = istrue.curpos;

    inc.curpos = func.codes.size();
    inc.lpos = func.proto->lfuns.size(); 
    inc.type = OC_IF;      //调用
    func.codes.push_back(inc);

    //判断结束符号是否正确
    if (T_COLON != ls.t.token) {
        logger->error(ls._row,":",ls._col," else condition judgment error");
        return ERR_END;
    }
    findX_next();
    
    FunState* elsestate = new FunState();
    elsestate->prev = &func;
    elsestate->anonymous = true;
    parse_Func(*elsestate);
    func.proto->lfuns.push_back(elsestate);

    if ( T_PASS != ls.t.token ) {
        logger->error(ls._row,":",ls._col," missing pass end in else statement");
        return ERR_END;
    }
    findX_next();
    //end
    if ( T_END != ls.t.token ) {
        logger->error(ls._row,":",ls._col," missing end in else statement");
        return ERR_END;
    }
    findX_next();
    
    
    return 0;
}

unsigned int DParser::for_expr(FunState& func){
    findX_next();

    Instruction forinc;
    forinc.type = OC_FOR;
    forinc.curpos = func.codes.size();
    forinc.lpos = func.proto->lfuns.size();
    func.codes.push_back(forinc);

    FunState* forstate = new FunState();
    forstate->anonymous = true;
    forstate->prev = &func;

    //初始化值
    Instruction first;
    first.left = 1;
    first.type = OC_NULL;
    first.curpos = forstate->codes.size();
    forstate->codes.push_back(first);

    Instruction assign;
    assign.curpos = forstate->codes.size();

    if ( T_UDATA != ls.t.token ) {
        logger->error(ls._row,":",ls._col," for initialization type is not correct");
        return ERR_END;
    }
    std::string varname = ls.dvar.var.chv;
    assign.left = ls.dvar;//迭代计数器
    assign.rpos = first.curpos;
    assign.type = OC_ASSIGN;
    forstate->codes.push_back(assign);
    forstate->proto->lv[ls.dvar.var.chv] = 0;
    forstate->proto->lv[ls.dvar.var.chv].type = VE_NULL;
    findX_next();

    
    if (T_IN != ls.t.token) {
        logger->error(ls._row,":",ls._col," for condition missing in symbol");
        return ERR_END;
    }
    findX_next();

    Instruction second;
    if ( T_UDATA != ls.t.token && T_INT != ls.t.token ) {
        logger->error(ls._row,":",ls._col," for iterator type is not correct");
        return ERR_END;
    }
    second.left = ls.dvar;//这个值到时候可以改改
    second.type = OC_NULL;
    second.curpos = forstate->codes.size();
    forstate->codes.push_back(second);
    //迭代类型变量
    assign.left = "for";
    assign.left.type = VE_USER;
    assign.curpos = forstate->codes.size();
    assign.rpos = second.curpos;
    forstate->proto->lv["for"] = 0;
    forstate->proto->lv["for"].type = VE_NULL;
    forstate->codes.push_back(assign);

    findX_next();
    unsigned int forstart = forstate->codes.size();

    //判断结束符号是否正确
    if (T_COLON != ls.t.token) {
        logger->error(ls._row,":",ls._col," for condition judgment error");
        return ERR_END;
    }
    //line
    findX_next();

    //解析内部for函数
    parse_Func(*forstate);
    if ( T_PASS != ls.t.token) {
        logger->error("missing pass end in for statement");
        return ERR_END;
    }
    //pass
    findX_next();

    //自增操作 区间
//--------------------------------------------------------------------------------------- 
    Instruction incre;
    incre.type = OC_NULL;
    incre.left = 1;
    incre.curpos = forstate->codes.size();
    forstate->codes.push_back(incre);
    
    Instruction add;
    add.type = OC_ADD;
    add.left = varname.c_str();
    add.left.type = VE_USER;
    add.rpos = incre.curpos;
    add.curpos = forstate->codes.size();
    forstate->codes.push_back(add);

    assign.left = varname.c_str();
    assign.left.type = VE_USER;
    assign.rpos = add.curpos;
    assign.curpos = forstate->codes.size();
    forstate->codes.push_back(assign);
    
//---------------------------------------------------------------------------------------    

    //比较大小是否跳转
    Instruction cmp;
    cmp.curpos = forstate->codes.size();
    cmp.type = OC_GT;
    cmp.left = varname.c_str();
    cmp.left.type = VE_USER;
    cmp.rpos = second.curpos;
    forstate->codes.push_back(cmp);
    Instruction jmp;
    jmp.lpos = forstart - 1;
    jmp.curpos = forstate->codes.size();
    jmp.type = OC_JMP;
    jmp.rpos = cmp.curpos;
    forstate->codes.push_back(jmp);

    func.proto->lfuns.push_back(forstate);
    
    return 0;//正常退出
}

unsigned int DParser::while_expr(FunState& func){
    findX_next();

    FunState* whilestate = new FunState();
    whilestate->prev = &func;

    //添加一个jump指令，
    Instruction jump1;
    jump1.type = OC_JMP;
    //判断跳转函数是否执行
    jump1.rpos = simple_expr(*whilestate);

    //判断结束符号是否正确
    if (T_COLON != ls.t.token) {
        logger->error(ls._row,":",ls._col," while condition judgment error");
        return ERR_END;
    }    
    findX_next();//这里通常都是 line 换行符

    Instruction inc;
    inc.curpos = func.codes.size();
    //记录跳转的函数是第几个
    inc.lpos = func.proto->lfuns.size();
    inc.type = OC_WHILE;
    func.codes.push_back(inc);

    jump1.curpos = whilestate->codes.size();
    whilestate->codes.push_back(jump1); // lpos在后面改
    whilestate->anonymous = true;
    parse_Func(*whilestate);


    Instruction jump2;
    jump2.curpos = whilestate->codes.size();
    jump2.lpos = -1; //最开始位置
    jump2.type = OC_JMP;
    whilestate->codes.push_back(jump2);
    //jump1跳转到最后位置
    whilestate->codes[jump1.curpos].lpos = whilestate->codes.size() ;
    
    func.proto->lfuns.push_back(whilestate);

    if ( T_PASS != ls.t.token) {
        //pass
        logger->error(ls._row,":",ls._col," missing pass end in while statement");
        return ERR_END;
    }
    findX_next();
    //end
    if ( T_END != ls.t.token ) {
        logger->error(ls._row,":",ls._col," missing  end in while statement");
        return ERR_END;
    }
    findX_next();


    return 0;
}

unsigned int DParser::array_element_expr(Instruction& inc,FunState& fun){
    D_OBJ* tmpobj = variable_check(inc.left.var.chv,fun);
    if ( VE_VOID == tmpobj->type ){
        delete tmpobj;
        logger->error(ls._row,":",ls._col," array is not exist");
        return ERR_END;
    }
    //[
    this->findX_next();
    //用户变量或数字
    inc.right = ls.dvar;
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


    inc.rpos = simple_expr(fun);

    if( ERR_END == inc.rpos ){
        logger->error(ls._row,":",ls._col," array assign expression error");
        return ERR_END;
    }
    inc.type = OC_ARR_PASSIGN;

    inc.curpos = fun.codes.size();
    fun.codes.push_back(inc);
    return 0;
}

unsigned int DParser::function_expr(FunState& func){

    if ( T_PASS == ls.t.token ) {
        return 0;
    }
    
    findX_next();

    if ( T_UDATA != ls.t.token ) {
        logger->error(ls._row,":",ls._col," illegal function name ");
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
        // func.proto->lv[ls.dvar.var.chv] = 0;
        //暂时没有

        findX_next();
        if ( T_RPARENTH == ls.t.token ) {
            findX_next();
            break;    
        }
        else if ( T_COMMA != ls.t.token ) {
            logger->error(ls._row,":",ls._col," missing comma in function");
            return ERR_END;
        }
        findX_next();
    }
    
    if ( T_COLON != ls.t.token ) {
        logger->error(ls._row,":",ls._col," error ending function statement ");
        return ERR_END;
    }

    findX_next(); // end
    //添加当前函数进入到function里面去
    FunState* function = new FunState();
    function->prev = &func;
    parse_Func(*function);


    func.proto->lv[funcname] = (unsigned int)func.proto->lfuns.size();
    func.proto->lv[funcname].type = VE_FUNC;
    func.proto->lfuns.push_back(function);

    if ( T_PASS != ls.t.token ) {
        logger->error("missing pass end in function statement");
        return ERR_END;
    }
    this->findX_next();
    
    return 0;
}

unsigned int DParser::union_expr(bool global,const std::string& name,FunState& fun){
    unsigned int lastpos = FIN_END;
    Instruction assign;

    for (;;) {
        Instruction inc;

        this->findX_next();

        if ( T_EOF == ls.t.token ) {
            logger->error(ls._row,":",ls._col," parser end error");
            return ERR_END;
        }
        else if ( T_END == ls.t.token ) {
            logger->error(ls._row,":",ls._col," missing right bracket ");
            return ERR_END;        
        }
        
        inc.left = ls.dvar;
        inc.rpos = lastpos;
        inc.type = OC_ARR_VAL;
        lastpos = inc.curpos = fun.codes.size();
        fun.codes.push_back(inc);
        
        this->findX_next();
        if ( T_RBRACKET == ls.t.token ) {
            //正常退出
            break;
        }
        else if ( T_COMMA != ls.t.token ) {
            logger->error(ls._row,":",ls._col," missing comma in array or array is null");
            return ERR_END;
        }
    }
    
    assign.left = name.c_str();
    assign.type = OC_ARR_ASSIGN;
    assign.rpos = lastpos;
    assign.right = global;
    assign.curpos = fun.codes.size();
    fun.codes.push_back(assign);

    fun.proto->lv[name] = 0;
    fun.proto->lv[name].type = VE_UNION;

    if ( T_RBRACKET != ls.t.token ) {
        logger->error(ls._row,":",ls._col," missing right bracket ");
        return ERR_END;
    }
    this->findX_next();
    return 0;
}

unsigned int DParser::list_access_expr(const std::string& name,FunState& func){
    this->findX_next();
    std::string varname = ls.dvar.var.chv;
    if ( T_END == ls.t.token ) {
        Instruction ll;
        ll.type = OC_NULL;
        ll.left = 0;
        ll.curpos = func.codes.size();
        func.codes.push_back(ll);
        
        Instruction rr;
        rr.type = OC_NULL;
        rr.left = QD_INT_32_MAX;
        rr.curpos = func.codes.size();
        func.codes.push_back(rr);
        
        Instruction inc;
        inc.type = OC_ARR_LIST;
        inc.lpos = ll.curpos;
        inc.rpos = rr.curpos;
        inc.left = varname.c_str();

        inc.curpos = func.codes.size();
        func.codes.push_back(inc);
        Instruction ass;
        ass.type = OC_ARR_LASSIGN;
        ass.left = name.c_str();
        ass.rpos = inc.curpos;
        ass.curpos = func.codes.size();
        func.codes.push_back(ass);
        func.proto->lv[name] = 0;
        func.proto->lv[name].type = VE_UNION;
        return 0;
    }
    else  if ( T_LBRACKET != ls.t.token ) {
        logger->error(ls._row,":",ls._col," list access expression error");
        return ERR_END;
    }

    this->findX_next();

    int ret = parse_PreCode(ls.t.token);
    if ( OC_NIL != ret ){
        findX_next();
    }
    
    D_VAR left = 0;
    if ( T_INT == ls.t.token ) {
        left = ls.dvar.var.iv ;
        this->findX_next();
    }
    else if ( T_UDATA == ls.t.token ) {
        left = ls.dvar;
        this->findX_next();
    }

    //如果只有一个负数符号
    if ( T_COLON != ls.t.token && T_MINUS == ls.lookahead.token ) {
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

        if ( T_END != ls.t.token ) {
            logger->error(ls._row,":",ls._col," error ending in array assign");
            return ERR_END;
        }

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

    Instruction ll;
    ll.type = OC_NULL;
    ll.left = left;
    ll.curpos = func.codes.size();
    func.codes.push_back(ll);
    
    Instruction rr;
    rr.type = OC_NULL;
    rr.left = right;
    rr.curpos = func.codes.size();
    func.codes.push_back(rr);

    Instruction alist;
    alist.type = OC_ARR_LIST;
    alist.lpos = ll.curpos;
    alist.rpos = rr.curpos;
    alist.left = varname.c_str();
    alist.curpos = func.codes.size();
    func.codes.push_back(alist);

    Instruction assign;
    assign.type = OC_ARR_LASSIGN;
    assign.left = name.c_str();
    assign.rpos = alist.curpos;
    assign.curpos = func.codes.size();
    func.codes.push_back(assign);

    this->findX_next();

    func.proto->lv[name] = 0;
    func.proto->lv[name].type = VE_UNION;

    return 0;
}

unsigned int DParser::simple_expr(FunState& fun){
    std::vector<unsigned int> values; // 存放计算值位置
    //存放运算符
    std::vector<int> ops; //存放符号运算token

    int negative = 0;

    while ( T_END != ls.t.token && T_COLON != ls.t.token ) {
        int tok = ls.t.token;
        if ( T_EOF == tok ) {
            return ERR_END;
        }
        
        if ( ls.is_variable(tok) ) {
            Instruction inc;
            inc.left = ls.dvar;
            inc.type = OC_NULL;
            inc.curpos = fun.codes.size();

            if (negative % 2 != 0) {
                symbol_reversal(inc); // 处理负数前缀
                --negative;
            }
            fun.codes.push_back(inc);
            values.push_back(inc.curpos);
        } else if ( T_MINUS == ls.t.token ) {
            if ( ls.lookahead.token != T_RPARENTH 
            && !ls.is_variable(ls.lookahead.token) ){
                // 负数前缀检测
                ++negative;
            }
            else {
            Instruction tmp;
                while (!ops.empty() && 
            priority[parse_Opr(tmp,ops.back())] >= priority[parse_Opr(tmp,tok)] ) {
                    unsigned int val2 = values.back();
                    values.pop_back();
                    unsigned int val1 = values.back();
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
        } else if ( T_LPARENTH == tok ) {
            if ( T_MINUS == ls.lookahead.token 
            && ( ls.is_operator(ls.prevhead.token) 
            ||   T_LPARENTH == ls.prevhead.token ) ) {
                --negative;
                ops.push_back(T_XMINUS);
            }
            ops.push_back(tok);
        } else if ( T_RPARENTH == tok ) {
            while ( ops.back() != T_LPARENTH ) {
                unsigned int val2 = values.back();
                values.pop_back();
                unsigned int val1 = values.back();
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
        } else if ( ls.is_operator(tok) ) {
            Instruction tmp;
            while (!ops.empty() && 
            priority[parse_Opr(tmp,ops.back())] >= priority[parse_Opr(tmp,tok)] ) {
                unsigned int val2 = values.back();
                values.pop_back();
                unsigned int val1 = values.back();
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
            logger->error(ls._row,":",ls._col," simple expression error");
            return ERR_END;
        }
        this->findX_next();
    }

    while (!ops.empty()) {
        unsigned int val2 = values.back();
        values.pop_back();
        unsigned int val1 = values.back();
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

unsigned int DParser::call_expr(std::string name,FunState& fun){
    Instruction inc;
    inc.left = name.c_str();
    inc.type = OC_CALL;
    // inc.rpos = 0;
    //这个返回值要进行更改一下
    D_OBJ* tmpobj = variable_check(inc.left.var.chv,fun);
    if ( VE_VOID == tmpobj->type ) {
        delete tmpobj;
        logger->error(ls._row,":",ls._col," function name undefine ");
        return ERR_END;
    }
    inc.lpos = tmpobj->var.iv;

    inc.curpos = fun.codes.size();
    fun.codes.push_back(inc);
    findX_next();

    if ( T_RPARENTH != ls.t.token ) {
        logger->error(ls._row,":",ls._col," function call missing right parentheses");
        return ERR_END;
    }
    this->findX_next();

    if ( T_END != ls.t.token ) {
        logger->error(ls._row,":",ls._col," function call end is not correct");
        return ERR_END;
    }


    return 0;
}

void DParser::print_variable(const std::string& name,std::map<std::string,D_OBJ>& variables){
    std::cout << name ;
    const D_OBJ& dv = variables[name];
    switch (dv.type)
    {
    case VE_NULL:
        std::cout << "  type  is  null"  << std::endl;
        break;
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
    case VE_FUNC:
        std::cout << "  type  is  function  "   << std::endl;
        break;
    default:
        std::cout << "  variables  no  found " << std::endl;
        return;
    }
}

void DParser::print_variables(std::map<std::string,D_OBJ> lv){
    //打印全局变量
    for (auto i: lv){
        print_variable(i.first,lv);
    }
}

void DParser::print_union(const std::string& name,std::map<std::string,D_OBJ>& variables){
    const std::vector<D_VAR>& vec = variables[name].uni->larr;
    std::cout <<  name << "  |  ";
    if (!vec.size()){
        logger->error("vector size is zero");
        return;
    }
    for (std::vector<D_VAR>::const_iterator ater = vec.begin() ; ater != vec.end() ; ater ++) {
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

void DParser::print_unions(std::map<std::string,D_OBJ>& variables){
    //打印全局变量
    for (auto i: variables){
        print_union(i.first,variables);
    }
}

unsigned int DParser::source_code_row(){
    return ls._row;
}

unsigned int DParser::source_code_col(){
    return ls._col;
}

unsigned int DParser::bytes_code_line(){
    return global->codes.size();
}

unsigned int DParser::variable_count(){
    return global->proto->lv.size();
}

D_OBJ* DParser::variable_check(const std::string& name,const FunState& fun){
    if ( fun.proto->lv.find(name) != fun.proto->lv.end() ) {
        //如果是null 必须满足不是编译期的条件
        return &fun.proto->lv[name];
    }

    FunState* tmpfunc = fun.prev;

    while ( tmpfunc )
    {
        if (tmpfunc->proto->lv.find(name) != tmpfunc->proto->lv.end() ) {
            return &tmpfunc->proto->lv[name];
        }
        tmpfunc = tmpfunc->prev;
    }

    D_OBJ* tmp = new D_OBJ(); 
    return tmp;
}

FunState* DParser::function_check(const std::string& name,FunState* fun){
    FunState* tmp = fun;
    if ( tmp->proto->lv.find(name) != tmp->proto->lv.end() ) {
        return tmp;
    }

    while ( tmp )
    {
        if ( tmp->proto->lv.find(name) != tmp->proto->lv.end() ) {
            return tmp;
        }
        tmp = tmp->prev;
    }
    //这里返回函数自身的原因是prev很可能为空
    return fun;
}

FunState* DParser::function_stack_top(){
    return this->funstack.back();
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

D_OBJ DParser::analyse_assign(Instruction& inc,FunState& fun){
    logger->debug("<-----  analyse  assign  ----->");

    logger->debug("cur line is :  ",inc.curpos);
    logger->debug("left pos is :  ",inc.lpos);
    logger->debug("right pos is :  ",inc.rpos);
    logger->debug("left  value is :  ",inc.left.var.chv);
    logger->debug("operator code is :  ",(int)inc.type);
    if ( FIN_END ==  inc.rpos ) {
        logger->error("assigment right expression is null");
        return ERR_END;
    }

    Instruction& tmp = fun.codes[inc.rpos];
    switch (tmp.right.type)
    {
    case VE_BOOL:
        return tmp.right.var.bv;
    case VE_INT:
        return tmp.right.var.iv;
    case VE_STR:
        return tmp.right.var.chv;
    case VE_FLT:
        return tmp.right.var.dv;
    case VE_FUNC: {
        D_OBJ* tmpobj =  variable_check( QD_KYW_RET, fun );
        //这里要改返回值有问题,
        if ( VE_VOID == tmpobj->type ) {
            delete tmpobj;
            logger->error(ls._row,":",ls._col," variable name undefine ");
            return D_OBJ();
        }
        return *tmpobj;
    }
    default:
        logger->error("assign nothing happend");
        return D_OBJ();//这里潜在错误
    }
}

unsigned int DParser::analyse_expr(Instruction& inc,FunState& fun){
    logger->debug("<-----  analyse expression  ----->");
    D_VAR tleft;
    D_VAR tright ;
    D_VAR& tres = inc.right;

    logger->debug("cur line is :  ",inc.curpos);
    logger->debug("left pos is :  ",inc.lpos);
    logger->debug("right pos is :  ",inc.rpos);
    logger->debug("operator code is :  ",(int)inc.type);

    if ( FIN_END != inc.lpos ) {
        tleft = fun.codes[inc.lpos].right;
    }

    if ( FIN_END != inc.rpos ) {
        tright = fun.codes[inc.rpos].right;
    }


    if ( VE_USER == tleft.type  ) {
        D_OBJ* tmpvar = variable_check(tleft.var.chv,fun);
        if ( VE_VOID == tmpvar->type ) {
            delete tmpvar;
            return ERR_END;
        }
        tleft = *tmpvar;
    }

    if ( VE_USER == tright.type ) {
        D_OBJ* tmpvar = variable_check(tright.var.chv,fun);
        if ( VE_VOID == tmpvar->type ) {
            delete tmpvar;
            return ERR_END;
        }
        tright = *tmpvar;
    }

    //如果只有一个值
    if ( VE_VOID == tleft.type && VE_USER == inc.left.type ) {
        D_OBJ* tmpvar = variable_check(inc.left.var.chv,fun);
        if ( VE_VOID == tmpvar->type ) {
            delete tmpvar;
            return ERR_END;
        }
        tleft = *tmpvar;
    }

    logger->debug("left type is  : ",tleft.type);
    logger->debug("right type is  : ",tright.type);
    //解析指令
    switch (inc.type)
    {
    case OC_ADD:{
        if(result_add(tres,tleft,tright)){
            logger->error("error  in  add  expression");
            return ERR_END;
        }
        break;
    }
    case OC_SUB:{
        if(result_sub(tres,tleft,tright)) {
            logger->error("error  in  sub  expression");
            return ERR_END;
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
        D_OBJ* tmpobj = variable_check(tleft.var.chv,fun);
        if ( VE_VOID == tmpobj->type ) {
            delete tmpobj;
            logger->error("variable not exist int array");
            return ERR_END;
        }
        if ( tright.var.iv < 0 || tright.var.iv >= tmpobj->uni->larr.size() ) {
            logger->error("variable access out of range");
            return ERR_END;
        }
        inc.right = tmpobj->uni->larr[tright.var.iv];
        break;
    }
    case OC_NULL:{
        if ( VE_VOID == tleft.type ) {
            tres = inc.left;
        }
        else {
            tres = tleft;
        }
        break; 
    }
    default:{
        logger->error(ls._row,":",ls._col,"code parse error ");
        break;
    }
    }

    //结果数据为负数
    if ( OC_MINUS == inc.restype ) {
        tres = -tres;
    }

    logger->debug("result   type   ",tres.type);
    logger->debug("result   value  ",tres.var.dv);
    logger->debug("result   value  ",tres.var.iv);
    logger->debug("result   value  ",tres.var.bv);

    return 0;
}

unsigned int DParser::analyse_array(Instruction& inc,const FunState& fun){
    logger->debug("<-----  analyse  array assign  ----->");

    logger->debug("cur line is :  ",inc.curpos);
    logger->debug("left pos is :  ",inc.lpos);
    logger->debug("right pos is :  ",inc.rpos);
    logger->debug("left  value is :  ",inc.left.var.chv);
    std::string name = inc.left.var.chv;
    unsigned int pos = inc.rpos;
    if ( FIN_END == pos ) {
        //do nothing
        return 0;
    }    

    D_UNION array;
    Instruction tmp = fun.codes[inc.rpos];
    //检查一下是否为拷贝
    if ( VE_USER == tmp.left.type ) {
        D_OBJ* tmpobj = variable_check(tmp.left.var.chv,fun);
        if ( VE_VOID == tmpobj->type ) {
            delete tmpobj;
            return ERR_END;
        }
        array = tmpobj->uni;
        fun.proto->lv[name] = array;
        return 0;
    }
    array.larr.push_back(tmp.left);
    pos = tmp.rpos;
    while ( FIN_END != pos ) {
        tmp = fun.codes[tmp.rpos];
        pos = tmp.rpos;
        array.larr.push_back(tmp.left);
    }
    //反转了一下数组,有点麻烦暂时先保留这个条件。
    std::reverse(array.larr.begin(),array.larr.end());

    if (inc.right.var.bv) {
        // (*this->funstack.begin())->proto->lv[name] = array;
        global->proto->lv[name] = array;
    }
    else{
        fun.proto->lv[name] = array;
    }
    return 0;
}

unsigned int DParser::analyse_code(unsigned int& i,FunState& fun){
    unsigned int clen = fun.codes.size();
    // unsigned int i = pos;
    logger->debug("<------  function  analyse  start   ------>");
    logger->debug("function is anonymous ? ",fun.anonymous);

    for (;i < clen;i++) {

        Instruction& inc = fun.codes[i];
        //在这里判断解析类型
        logger->debug("operator code is :  ",(int)inc.type);

        switch (inc.type)
        {
        case OC_CALL:{
            logger->debug("analyse  call    !!!!!!!!! ");
            logger->debug("cur line is :  ",inc.curpos);
            logger->debug("left pos is :  ",inc.lpos);
            logger->debug("right pos is :  ",inc.rpos);
            // logger->error("result  value  is :  ",inc.right.var.iv);
            
            FunState* curfun = function_stack_top();
            //检查函数前后地址变化
            // logger->error(&*curfun);
            curfun = function_check(inc.left.var.chv,curfun);
            // logger->error(&*curfun);

            FunState* callfun = new FunState(*curfun->proto->lfuns[inc.lpos]);
            //一定要清空所有变量防止有变量信息记录，对运算操作进行规划，防止有null值
            callfun->proto->lv.clear();
            // callfun->code_pos = 0;
            callfun->prev = function_stack_top();

            this->funstack.push_back(callfun);

            analyse_code(callfun->code_pos,*callfun);

            // // 内存回收
            delete function_stack_top();
            this->funstack.pop_back();
            break;
        }
        case OC_JMP:{
            logger->debug("analyse  jump    !!!!!!!!! ");
            logger->debug("cur line is :  ",inc.curpos);
            logger->debug("left pos is :  ",inc.lpos);
            logger->debug("right pos is :  ",inc.rpos);
            logger->debug("operator code is :  ",(int)inc.type);
            //判断是否跳转
            if ( FIN_END != inc.rpos ) {
                Instruction& jump = fun.codes[inc.rpos];
                //收集到的bool类型好像大于1
                if ( !jump.right.var.bv && true ) {
                    i = inc.lpos;
                }
            }
            //单纯跳转指令
            else{
                i = inc.lpos ;
            }
            break;
        }      
        case OC_ASSIGN:{
            D_OBJ result ;
            if (inc.right.var.bv) {
                result = analyse_assign(inc,fun);
                if ( VE_VOID == result.type ) {
                    return ERR_END;
                }
                //因为global是第一个函数,这里可能到时候要改
                global->proto->lv[inc.left.var.chv] = result;
                print_variable( inc.left.var.chv,global->proto->lv );
            }
            else {
                result = analyse_assign(inc,fun);
                if ( VE_VOID == result.type ) {
                    return ERR_END;
                }
                
                //判断return所在位置
                if ( !strcmp(QD_KYW_RET,inc.left.var.chv) ) {
                    FunState* cur = function_stack_top();
                    bool any = cur->anonymous;
                    //if else while
                    while (any)
                    {
                        cur = cur->prev;
                        any = cur->anonymous;
                    }
                    //如果是全局变量则不执行
                    if ( cur->prev != nullptr ) {
                        cur->prev->proto->lv[QD_KYW_RET] = result;
                        print_variable(QD_KYW_RET,cur->prev->proto->lv);
                    }
                }
                else{
                    fun.proto->lv[inc.left.var.chv] = result;
                    print_variable(inc.left.var.chv,fun.proto->lv);
                }
            }
            break;
        }
        case OC_RET:{
            logger->debug("analyse  return    !!!!!!!!! ");
            logger->debug("cur line is :  ",inc.curpos);

            //这个一定要是最后一个栈空间，因为开辟新空间上下文信息不一样
            //这里有问题检查检查
            FunState* cur = &fun;
            bool any = cur->anonymous;
            cur->code_pos = clen;
            while (any)
            {
                cur = cur->prev;
                any = cur->anonymous;
                cur->code_pos = cur->codes.size();
            }
            
            break;
        }
        case OC_IF:{
            logger->debug("analyse  if    !!!!!!!!! ");
            logger->debug("cur line is :  ",inc.curpos);
            logger->debug("left pos is :  ",inc.lpos);
            logger->debug("right pos is :  ",inc.rpos);
            logger->debug("result  value  is :  ",inc.right.var.iv);
            //if elif else
            if ( fun.codes[inc.rpos].right.var.bv ) {
                if ( FIN_END != inc.rpos && FIN_END != inc.lpos ) {
                    FunState* jumpfun = new FunState(*function_stack_top()->proto->lfuns[inc.lpos]);
                    //一定要清空所有变量防止有变量信息记录
                    jumpfun->proto->lv.clear();
                    // jumpfun->code_pos = 0;
                    jumpfun->prev = function_stack_top();
                    this->funstack.push_back(jumpfun);
                    
                    analyse_code(jumpfun->code_pos,*jumpfun);

                    delete function_stack_top();
                    this->funstack.pop_back();
                }
                else{
                    logger->error("if  expression  analyse  error");
                    return ERR_END;
                }
            }
            else{
                logger->debug("jump next if or elif function");
            }
            break;
        }
        case OC_FOR:{
            logger->debug("analyse  for    !!!!!!!!! ");
            logger->debug("cur line is :  ",inc.curpos);
            logger->debug("left pos is :  ",inc.lpos);
            logger->debug("right pos is :  ",inc.rpos);
            logger->debug("result  value  is :  ",inc.right.var.iv);
            //for 
            if ( FIN_END != inc.lpos ) {
                FunState* jumpfun = new FunState(*function_stack_top()->proto->lfuns[inc.lpos]);
                //一定要清空所有变量防止有变量信息记录
                jumpfun->proto->lv.clear();
                jumpfun->prev = function_stack_top();
                this->funstack.push_back(jumpfun);
                
                analyse_code(jumpfun->code_pos,*jumpfun);

                delete function_stack_top();
                this->funstack.pop_back();
            }
            else {
                logger->error("for  expression  analyse  error");
                return ERR_END;
            }
            break;
        }
        case OC_WHILE:{
            logger->debug("analyse  while    !!!!!!!!! ");
            logger->debug("cur line is :  ",inc.curpos);
            logger->debug("left pos is :  ",inc.lpos);
            //while
            if ( FIN_END != inc.lpos ) {
                FunState* jumpfun = new FunState(*function_stack_top()->proto->lfuns[inc.lpos]);
                //一定要清空所有变量防止有变量信息记录
                jumpfun->proto->lv.clear();
                jumpfun->prev = function_stack_top();
                this->funstack.push_back(jumpfun);
                
                analyse_code(jumpfun->code_pos,*jumpfun);

                delete function_stack_top();
                this->funstack.pop_back();
            }
            else{
                logger->error("while  expression  analyse  error");
                return ERR_END;
            }
            break;
        }
        case OC_ARR_ASSIGN:{
            if (analyse_array(inc,fun)){
                logger->error("arrray assign error");
                return ERR_END;
            }
            if (inc.right.var.bv) {
                // print_union(inc.left.var.chv,(*this->funstack.begin())->proto->lv);
                print_union(inc.left.var.chv,global->proto->lv);
            }
            else {
                print_union(inc.left.var.chv,fun.proto->lv);
            }
            break;
        }
        case OC_ARR_LASSIGN:{
            Instruction& arr = fun.codes[inc.rpos];

            D_OBJ lval;
            D_OBJ rval;
            if ( VE_USER == fun.codes[arr.lpos].right.type ) {
                lval = variable_check(fun.codes[arr.lpos].right.var.chv,fun);
                if ( VE_INT !=  lval.type ) {
                    logger->error("error in array is not int type");
                    return ERR_END;
                }
            }
            else {
                lval = fun.codes[arr.lpos].right.var.iv;
            }
            
            if ( VE_USER == fun.codes[arr.rpos].right.type ) {
                rval = variable_check(fun.codes[arr.rpos].right.var.chv,fun);
                if ( VE_INT !=  rval.type ) {
                    logger->error("error in array is not int type");
                    return ERR_END;
                }
            }
            else{
                rval = fun.codes[arr.rpos].right.var.iv;
            }
            D_OBJ* tmpobj = variable_check(arr.left.var.chv,fun);
            if ( VE_VOID == tmpobj->type ) {
                logger->error("array list assign  variable not exist");
                delete tmpobj;
                return ERR_END;
            }
            D_UNION tmparr = 
            union_access(lval.var.iv,rval.var.iv,*tmpobj->uni);
            if ( !strcmp(arr.left.var.chv,inc.left.var.chv) ) {
               fun.proto->lv[arr.left.var.chv].uni->larr.clear();
            }
            fun.proto->lv[inc.left.var.chv] = tmparr;

            print_union(inc.left.var.chv,fun.proto->lv);
            break;
        }
        case OC_ARR_PASSIGN:{
            D_OBJ* tmpobj = variable_check(inc.left.var.chv,fun);
            if ( VE_VOID == tmpobj->type ) {
                logger->error("array list assign  variable not exist");
                delete tmpobj;
                return ERR_END;
            }
            D_VAR restmp;
            if ( VE_STR == inc.right.type || VE_USER == inc.right.type ) {
                D_OBJ* tmpvar = variable_check(inc.right.var.chv,fun);
                if ( VE_VOID == tmpvar->type ) {
                    logger->error("array position  variable not exist");
                    delete tmpvar;
                    return ERR_END;
                }
                else if ( VE_INT == tmpvar->type ) {
                    restmp = inc.right; 
                    inc.right = tmpvar->var.iv;
                }
                else{
                    logger->error("error varibale type to access array");
                    return ERR_END;
                }
            }
            tmpobj->uni->larr[inc.right.var.iv] = analyse_assign(inc,fun);
            // print_union(inc.left.var.chv,fun.proto->lv);
            if ( VE_STR == restmp.type || VE_USER == restmp.type ) { 
                inc.right = restmp;
            }
            break;
        }
        case OC_ARR_LIST:
        case OC_ARR_VAL:{
            logger->debug("cur line is :  ",inc.curpos);
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
    // print_variables(fun.proto->lv);
    // logger->error("stack  size is ",this->funstack.back()->proto->lv.size());
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
        logger->error("left  right  type  is  not  equals");
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