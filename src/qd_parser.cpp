#include "qd_parser.h"


_QD_BEGIN


DParser::DParser(){
    logger = Logger::getInstance();
}

DParser::~DParser(){
    logger->release();
}

void DParser::parseX_next(){
    if ( ls.t.token != T_BLANK ) {
        this->ls.lookahead.token = this->ls.t.token;
    }
    this->ls.t.token = ls.llex();
}

void DParser::parse(const char* str){
    ls.alloc_buff(str);

    //这里开始全局解析
    parse_Func(global);

    //只用解析一次就好了
    analyse_code(global,global.code_pos);

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
        case T_COMMENT:
            logger->info(" comment line ");
            break;
        case T_UDATA:
            if(statement(fun,fun.code_pos)) {
                logger->error("statement error");
                return ERR_END;
            }
            break;
        case T_JUMP:
        //跳转指令
            jump_expr();
            break;
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
        default:
            logger->info("do nothing  ");
            break;
        }
        
        // analyse_code(fun,fun.code_pos);

        // for (int i = 0; i < fun.proto->lfuns.size() ; i ++ ) {
        //     analyse_code(*fun.proto->lfuns[i],fun.proto->lfuns[i]->code_pos);
        // }
    }

    return 0;
}

void DParser::skip_blank(){
    if ( T_BLANK == ls.t.token )
    {
        this->parseX_next();
    }
}

void DParser::skip_line() {
    if ( T_END == ls.t.token )
    {
        this->parseX_next();
    }
}

void DParser::findX_next(){
    this->parseX_next();
    this->skip_blank();
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
    case T_DDIV:
        inc.type = OC_IDIV;
        break;
    default:
        inc.type = OC_NULL;
        break;
    }
}

unsigned int DParser::statement(FunState& fun,unsigned int& pos){
    Instruction inc;

    if ( T_UDATA != ls.t.token ) {
        logger->error(ls._row,":",ls._col," data error");
        return ERR_END;
    }
    //记录左值
    inc.left = ls.dvar.var.chv;

    findX_next();
    //因为是赋值表达式所以判断=
    if ( T_EQ != ls.t.token ){
        logger->error(ls._row,":",ls._col," unkonwn expression error ");
        return ERR_END;
    }
    inc.type = OC_ASSIGN;

    findX_next();

    //在这里做接收数据类型的判断吧
    //直接在exprstat做类型筛选太过于麻烦，所有在这里做方便一点
    //如果这里的数据类型是udata那么就要重新判断
    short assigntype;

    if (VE_USER == ls.dvar.type) {
        if ( T_UDATA == ls.t.token && !this->variable_check(fun)) {
            logger->error(ls._row,":",ls._col,"user data undefine");
            return ERR_END;
        }
        assigntype = find_variable(ls.dvar.var.chv,fun).type;
    }
    else {
        assigntype = ls.dvar.type;
    }

    switch (assigntype)
    {
    case VE_FLT:
    case VE_STR: {
        Instruction tmp;
        tmp.res = ls.dvar;
        tmp.curpos = fun.codes.size();
        fun.codes.push_back(tmp);
        findX_next();
        inc.rpos = tmp.curpos;
        break;
    }
    case VE_BOOL:{
        if ( (inc.rpos = logic_expr(fun.codes) ) < 0) {
            logger->debug("logic expression error ");
            return ERR_END;
        }
        //传一个默认值,好判断变量种类
        fun.proto->lv[inc.left.var.chv] = false;
        break;
    }
    case VE_INT:{
        inc.rpos = this->prefix_expr(fun,0);
        if ( ERR_END == inc.rpos ) {
            return ERR_END;
        }
        fun.proto->lv[inc.left.var.chv] = 0;
        break;
    }
    default:
        logger->error("userdata type error");
        return ERR_END;
    }
    //先把user_data添加进行进行判断

    if ( T_END != ls.t.token ){
        logger->error(ls._row,":",ls._col," error end");
        return ERR_END;
    }

    inc.curpos = fun.codes.size();
    fun.codes.push_back(inc);


    return 0;//正常退出
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
    if ( ls.dvar.var.bv < 0) {
        logger->warn(" jump out of range ");
        findX_next();
        return;
    }
    
    //加入指令
    Instruction inc;
    inc.type = OC_JMP;
    inc.res = ls.dvar.var.iv; 
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
    inc.rpos = logic_expr(func.codes);

    inc.curpos = func.codes.size();
    //记录跳转的函数是第几个
    inc.lpos = func.proto->lfuns.size(); 
    inc.type = OC_JMP;      //把类型定义为跳转类型


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
    //判断该元素是否迭代
    if ( T_UDATA == ret ) {
        //用户变量另作处理
        
    }
    
    if ( T_INT == ret ) {

    }

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
    
    //pass
    findX_next();
    
    return 0;//正常退出
}

unsigned int DParser::logic_expr(std::vector<Instruction>& codes){
    Instruction inc;
    
    inc.curpos = codes.size();
    inc.type = OC_OR;
    inc.left = ls.dvar;

    for(;;) {
        findX_next();

        if (T_ERR == ls.t.token || T_EOF == ls.t.token) {
            logger->error(ls._row,":",ls._col," logic expression error ");
            return ERR_END;
        }
        else if ( T_COLON == ls.t.token || T_END == ls.t.token) {
            //如果是第一次进入循环
            if (VE_VOID == inc.right.type) {
                inc.res = ls.dvar;
                inc.curpos = codes.size();
                codes.push_back(inc);
            }
            break;
        }

        if ( T_AMPERSAND == ls.t.token  ) {
            inc.type = OC_AND;
        }
        else if ( T_VERTICAL_BAR == ls.t.token){
            inc.type = OC_OR;
        }
        // else if ( T_DEQ == ls.t.token || T_NEQ == ls.t.token ) {
        //     Instruction dinc;
        //     if ( T_DEQ == ls.t.token )  dinc.type = OC_LEQ;
        //     else dinc.type = OC_LNEQ;
        //     dinc.left = inc.left;
        //     findX_next();
        //     dinc.right = ls.dvar;
        //     dinc.curpos = codes.size();
        //     codes.push_back(inc);
        //     inc.lpos = dinc.curpos;
        //     continue;
        // }
        else {
            logger->error(ls._row,":",ls._col," logical symbol error  ");
            return ERR_END;
        }

        findX_next();
        inc.right = ls.dvar;

        inc.curpos = codes.size();
        codes.push_back(inc);
        inc.lpos = codes.size() - 1;
    }

    return codes.size() - 1;
}

unsigned int DParser::expr_stat(FunState& fun,int ltype){
    //不为int类型数据直接跳出
    if ( T_INT != ls.t.token && T_UDATA != ls.t.token) {
        logger->error(ls._row,":",ls._col," express_stat type error  ");
        return ERR_END;
    }
    else if ( T_END == ls.t.token || T_LPARENTH == ls.t.token) {
        return FIN_END;
    }

    //如果用户变量定义
    if ( T_UDATA == ls.t.token && !this->variable_check(fun)) {
        logger->error(ls._row,":",ls._col,"user data undefine");
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

    // logger->debug(inc.ltype);
    if ( T_END == ls.t.token || priority[inc.type][1] < priority[inc.ltype][0]) {
        inc.res = inc.left;
        inc.curpos = fun.codes.size();
        fun.codes.push_back(inc);
        return inc.curpos;
    }
    findX_next();


    ret =  expr_stat(fun,inc.type);
    if (ERR_END == ret) {
        return ERR_END;
    }

    inc.rpos = ret;    

    inc.curpos = fun.codes.size();
    fun.codes.push_back(inc);

    return inc.curpos;
}

unsigned int DParser::prefix_expr(FunState& fun,int type){
    unsigned int ret = expr_stat(fun,type);

    if ( T_END == ls.t.token ) {
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
        
        ret = expr_stat(fun,inc.type);
        
        if ( ERR_END == ret) {
            logger->error("prefix expression expr_stat error");
            return ERR_END;
        }

        inc.rpos = ret;
        inc.curpos = fun.codes.size();
        fun.codes.push_back(inc);
        
        ret = inc.curpos;
        
        if ( T_END == ls.t.token ) {
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

bool DParser::variable_check(const FunState& fun){
    if ( fun.proto->lv.find(ls.dvar.var.chv) != fun.proto->lv.end() ) {
        return true;
    }

    FunState* tmpfunc = fun.prev;

    while ( tmpfunc )
    {
        if (tmpfunc->proto->lv.find(ls.dvar.var.chv) != tmpfunc->proto->lv.end() ) {
            return true;
        }
        tmpfunc = tmpfunc->prev;
    }

    return false;
}

D_VAR DParser::find_variable(const std::string& name,const FunState& fun){
    D_VAR dvar;
    if ( fun.proto->lv.find(name) != fun.proto->lv.end() ) {
        dvar = fun.proto->lv[name];
        return dvar;
    }
    
    FunState* tmpfunc = fun.prev;

    while ( tmpfunc )
    {
        if (tmpfunc->proto->lv.find(name) != tmpfunc->proto->lv.end() ) {
            dvar = tmpfunc->proto->lv[name];
            return dvar;
        }
        tmpfunc = tmpfunc->prev;
    }

    return dvar;
}

void DParser::analyse_jump(Instruction& inc,unsigned int& cpos){
    
}

void DParser::analyse_logic(Instruction& inc,std::vector<Instruction>& codes,std::map<std::string,D_VAR>& variables){
    logger->debug("<-----  analyse  logic   ----->");

    logger->debug("cur line is :  ",inc.curpos);
    logger->debug("left pos is :  ",inc.lpos);
    logger->debug("right pos is :  ",inc.rpos);
    logger->debug("operator code is :  ",inc.type);
    logger->debug("left    value  ",inc.left.var.bv);
    logger->debug("right   value  ",inc.right.var.bv);
    //只有一个值，或者是已经有结果
    if ( VE_BOOL == inc.res.type){
        logger->debug("result   value  ",inc.res.var.bv);
        return;
    }
    D_VAR tmpleft;
    D_VAR tmpright;
    //判断是否为用户变量
    if (VE_USER == inc.left.type) {
        tmpleft = inc.left;
        if ( variables.find(inc.left.var.chv) == global.proto->lv.end() ) {
            //错误处理
            logger->error("analyse logic  userdata not define ");
            inc.left.clear();
            inc.left.type = VE_VOID;
            inc.res = false;
            return;
        }
        else{
            inc.left = variables[inc.left.var.chv];
        }
    }
    if (VE_USER == inc.right.type) {
        tmpright = inc.right;
        if ( variables.find(inc.right.var.chv) == variables.end() ) {
            //错误处理
            logger->error("analyse logic  userdata not define ");
            inc.right.clear();
            inc.right.type = VE_VOID;
            inc.res = false;
            return;
        }
        else{
            inc.right = variables[inc.right.var.chv]; 
        }
    }

    if (FIN_END != inc.lpos) {
        inc.left = codes[inc.lpos].res.var.bv;
    }


    if (OC_AND == inc.type) {
        inc.res = inc.right.var.bv & inc.left.var.bv;
    }
    else if (OC_OR == inc.type) {
        inc.res = inc.right.var.bv | inc.left.var.bv;
    }
    inc.res.type = VE_BOOL;
    logger->debug("result   value  ",inc.res.var.bv);

    //复原
    if ( VE_USER == tmpleft.type  ) {
        inc.left = tmpleft;
    }
    if ( VE_USER == tmpright.type ) {
        inc.right = tmpright;
    }
}

void DParser::analyse_assign(Instruction& inc,std::vector<Instruction>& codes,std::map<std::string,D_VAR>& variables){
    logger->debug("<-----  analyse  assign  ----->");

    logger->debug("cur line is :  ",inc.curpos);
    logger->debug("left pos is :  ",inc.lpos);
    logger->debug("right pos is :  ",inc.rpos);
    logger->debug("left  value is :  ",inc.left.var.chv);
    logger->debug("operator code is :  ",inc.type);
    
    if ( FIN_END ==  inc.rpos ) {
        logger->error("assigment right expression is null");
        return;
    }
    Instruction& tmp = codes[inc.rpos];

    switch (tmp.res.type)
    {
    case VE_BOOL:
        variables[inc.left.var.chv] = tmp.res.var.bv;
        break;
    case VE_INT:
        variables[inc.left.var.chv] = OC_MINUS == inc.sufftype ? -tmp.res.var.iv : tmp.res.var.iv;
        break;
    case VE_STR:
        variables[inc.left.var.chv] = tmp.res.var.chv;
        break;
    default:
        logger->error("assign nothing happend");
        return;
    }
    
    print_variable(inc.left.var.chv,variables); 
}

void DParser::analyse_expr(Instruction& inc,FunState& fun){
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
        tleft = find_variable(tleft.var.chv,fun);
    }
    //这个好像用不到？？
    if (VE_USER == inc.right.type ) {
        tmpright = tright;
        tright = find_variable(tright.var.chv,fun);
    }
    
    //如果为负数
    if (OC_MINUS == inc.pretype){
        tleft = -tleft;
    }
    if (OC_MINUS == inc.sufftype){
        tright = -tright;
    }
    
    if (VE_VOID != inc.res.type) {
        inc.res = tleft;
        logger->debug("result   value  ",tres.var.iv);
        return;
    }
    
    //解析指令
    switch (inc.type)
    {
    case OC_ADD:
    case OC_SUB:
    {
        if ( FIN_END != inc.rpos){
            Instruction& tmp = fun.codes[inc.rpos];
            tright = tmp.res;
        }
        if ( FIN_END != inc.lpos) {
            Instruction& tmp = fun.codes[inc.lpos];
            tleft = tmp.res;
        }
        
        if ( (OC_ADD == inc.type && OC_SUB != inc.ltype ) || 
                OC_SUB == inc.type && OC_SUB == inc.ltype ) {
            tres = tleft.var.iv + tright.var.iv;
        }else{
            tres = tleft.var.iv - tright.var.iv;
        }
        break;
    }
    case OC_MUL:{
        if ( FIN_END != inc.rpos ) {
            Instruction& rtmp = fun.codes[inc.rpos];
            if ( FIN_END == rtmp.lpos ) {
                tright = OC_MINUS ==  inc.sufftype ? -rtmp.res : rtmp.res;
            }
        } 
        if ( FIN_END == inc.lpos ) {
            tres = tleft.var.iv * tright.var.iv;
        }
        else {
            //左边有数据未计算完成
            Instruction& ltmp = fun.codes[inc.lpos];
            if ( FIN_END == ltmp.lpos ) {
                tleft = OC_MINUS == inc.pretype ? -ltmp.res : ltmp.res;
                if ( FIN_END == ltmp.rpos ) {
                    tres = tleft.var.iv * tright.var.iv;
                }
            }
        }
        break;
    }
    case OC_MOD:{
        if ( FIN_END != inc.rpos ) {
            Instruction& rtmp = fun.codes[inc.rpos];
            if ( FIN_END == rtmp.lpos ) {
                tright = OC_MINUS ==  inc.sufftype ? -rtmp.res : rtmp.res;
            }
        }
        if ( FIN_END == inc.lpos ) {
            tres = tleft.var.iv % tright.var.iv;
        }
        else
        {
            Instruction& ltmp = fun.codes[inc.lpos];
            if ( FIN_END == ltmp.lpos ) {
                tleft = ltmp.left;
                if ( FIN_END == ltmp.rpos ) {
                    tres = tleft.var.iv % tright.var.iv;
                }
            }
        }
        break;
    }
    case OC_IDIV:{
        if ( FIN_END != inc.rpos ) {
            Instruction& rtmp = fun.codes[inc.rpos];
            if ( FIN_END == rtmp.lpos ) {
                tright = OC_MINUS ==  inc.sufftype ? -rtmp.res : rtmp.res;
            }else{
                //错误
            }
        } 
        if ( FIN_END == inc.lpos ) {
            tres = tleft.var.iv / tright.var.iv;
        }
        else
        {
            //左边有数据未计算完成
            Instruction& ltmp = fun.codes[inc.lpos];
            if ( FIN_END == ltmp.lpos ) {
                tleft = ltmp.left;
                if (  FIN_END == ltmp.rpos ) {
                    tres = tleft.var.iv / tright.var.iv;
                }
            }
        }
        break;
    }
    default:
        logger->error(ls._row,":",ls._col,"code parse error ");
        break;
    }

    if ( VE_USER == tmpleft.type  ) {
        tleft = tmpleft;
    }
    if ( VE_USER == tmpright.type ) {
        tright = tmpright;
    }

    logger->debug("result   value  ",tres.var.iv);
}

void DParser::analyse_code(FunState& fun,unsigned int& pos){
    unsigned int clen = fun.codes.size();
    unsigned int i = pos;
    logger->debug("<------  function  analyse  start   ------>");
    for (;i < clen;i++){
        Instruction& inc = fun.codes[i];
        //在这里判断解析类型
        switch (inc.type)
        {
        case OC_JMP:{
                logger->debug("analyse  jump    !!!!!!!!! ");
                logger->debug("cur line is :  ",inc.curpos);
                logger->debug("left pos is :  ",inc.lpos);
                logger->debug("right pos is :  ",inc.rpos);
                logger->debug("result  value  is :  ",inc.res.var.iv);
                logger->debug("operator code is :  ",inc.type);
                // //只有跳转语句
                // if ( VE_VOID != inc.type )
                // {
                //     // i = inc.res.var.iv;
                // }
                //if
                if (FIN_END != inc.rpos) {
                    if ( fun.codes[inc.rpos].res.var.bv &&  FIN_END != inc.lpos ) {
                        //应该去执行子函数
                        FunState& jumpfun = *fun.proto->lfuns[inc.lpos];
                        analyse_code(jumpfun,jumpfun.code_pos);
                    }
                }

            }
            break;
        case OC_AND:
        case OC_OR:
            //暂时
            analyse_logic(inc,fun.codes,fun.proto->lv);
            break;
        case OC_ASSIGN:
            analyse_assign(inc,fun.codes,fun.proto->lv);
            break;
        default:
            //赋值了一些常量
            analyse_expr(inc,fun);
            break;
        }
    }
    logger->debug(" <------  function  analyse  end   ------>");
    //为了交互模式暂时这么使用而已
    pos = clen;
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