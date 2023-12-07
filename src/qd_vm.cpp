#include "qd_vm.h"
#include "qd_varlib.h"

_QD_BEGIN


D_VM::D_VM() {
    this->init();

    st = nullptr;
    st = new CallStack();
}

D_VM::D_VM(const D_VM& vm) {
    this->init();

    fun = new FunHead(*vm.fun);
    st = new CallStack(*vm.st);
}

void D_VM::init() {
    fun = nullptr;
    st = nullptr;

    logger = Logger::getInstance();
}

D_VM::~D_VM() {
    //清除parser 传递函数
    if ( fun != nullptr ) {
        fun->clear();
        delete fun;
        fun = nullptr;
    }

    if ( st != nullptr ) {
        delete st;
        st = nullptr;
    }

}

void D_VM::push_call(CallInfo* in) {
    this->st->cs.push_back(in);
}

void D_VM::pop_call() {
    this->st->cs.pop_back();
}

_qd_uint D_VM::size_call() {
    return this->st->cs.size();
}

CallInfo* D_VM::cur_fun() {
    return this->st->top();
}

CallInfo* D_VM::head_fun() {
    return this->st->cs.front();
}

unsigned int D_VM::execute() {
    CallInfo* call = new CallInfo();
    push_call(call);
    //全局函数,全局函数还是拷贝一份免得污染
    call->f = new FunHead(*fun);  //这里到时候改，接受的是之前parser的函数信息

    analyse_code(cur_fun()->pos,cur_fun());

    return 0;
}

unsigned int D_VM::analyse_code(unsigned int& i,CallInfo* info){
    unsigned int clen = info->f->codes.size();

    //目前是为了防止栈溢出简单的处理
    if ( st->cs.size() > QD_STACK_MAX ) {
        logger->error("stack overflow error ");
        return ERR_END;
    }

    logger->debug("<------  function  analyse  start   ------>");
    // logger->debug("function is anonymous ? ",info->f->anonymous);

    logger->debug("function value address is ",&*info->f);
    logger->debug("before stack size is ",this->st->cs.size());


    for (;i < clen;i++) {
        Instruction& inc = info->f->codes[i];
        //打印指令
        // logger->error(inc);
        switch (inc.type)
        {
            case OC_CALL:
            {
                logger->debug("<-----  analyse  call  ----->");
                logger->debug("cur line is :  ",inc.curpos);
                logger->debug("left pos is :  ",inc.lpos);
                logger->debug("function name is ",inc.left.var.chv);
                
                CallInfo* call = new CallInfo();
                call->f = new FunHead( *find_function(inc.left.var.chv,info)->lfuns[inc.lpos] );
                // call->f = new FunHead(*info->f->lfuns[inc.lpos]);
                if (!call->f){
                    logger->error("function error");
                    return ERR_END;
                }
                this->push_call(call);
                if(analyse_code(cur_fun()->pos,cur_fun())){
                    return ERR_END;
                }
                //结束 释放
                delete this->cur_fun();
                this->pop_call();

                break;
            }
            case OC_RET: 
            {
                logger->debug("<-----  analyse  ret  ----->");
                logger->debug("cur line is :  ",inc.curpos);
                //出栈
                
                CallInfo* call = last_return(info);

                if (!call) {
                    i = clen;
                    continue;
                }
                //保存上一级函数返回值变量,匿名函数不赋值直接跳出
                if ( FIN_END != inc.rpos && call ) {
                    call->v(inc.left.var.chv) = info->f->codes[inc.rpos].right;
                }
                //当前代码行数终止
                i = clen;
                //存储return变量
                // logger->error(size_call());
                break;
            }
            case OC_IF:
            {
                logger->debug("<-----  analyse  if  ----->");
                logger->debug("cur line is :  ",inc.curpos);
                logger->debug("left pos is :  ",inc.lpos);
                logger->debug("right pos is :  ",inc.rpos);
                
                CallInfo* call = new CallInfo();
                call->anonymous = true;
                call->f = new FunHead(*cur_fun()->f->lfuns[inc.lpos]);

                if (!call->f){
                    logger->error("function error");
                    return ERR_END;
                }
                
                Instruction& tmp = info->f->codes[inc.rpos];
                if (tmp.right.var.bv) {
                    this->push_call(call);
                    if(analyse_code(cur_fun()->pos,cur_fun())){
                        return ERR_END;
                    }
                    delete this->cur_fun();
                    this->pop_call();
                }
                
                break;
            }
            case OC_ASSIGN:
            {
                logger->debug("<-----  analyse  assign  ----->");
                logger->debug("cur line is :  ",inc.curpos);
                logger->debug("left pos is :  ",inc.lpos);
                logger->debug("right pos is :  ",inc.rpos);
                logger->debug("left  value is :  ",inc.left.var.chv);
                logger->debug("operator code is :  ",(int)inc.type);
                if ( FIN_END != inc.rpos ) {
                    //作用域判断
                    if ( FIN_END == inc.lpos ) {
                        logger->error("variable scope error");
                        return ERR_END;
                    }
                    else if ( VA_DEFAULT == inc.lpos ) {
                        info->v(inc.left.var.chv) = info->f->codes[inc.rpos].right;
                    }
                    else if ( VA_LOCAL == inc.lpos ) {
                        last_function(info)->v(inc.left.var.chv) = info->f->codes[inc.rpos].right;
                    }
                    else if ( VA_GLOBAL == inc.lpos ) {
                        this->head_fun()->v(inc.left.var.chv) = info->f->codes[inc.rpos].right;
                    }
                }
                // logger->error(inc.left.var.chv," -> ",info->v(inc.left.var.chv));
                break;
            }
            default:
            {
                if(analyse_expr(inc,info)) {
                    logger->error("analyse  expresion  error  in  analyse  code");
                    return ERR_END;
                }
                break;
            }
        }
    }
    logger->debug("<------  function  analyse  end   ------>");
    logger->debug("after stack size is ",this->st->cs.size());

    logger->error("---------------> ",&*info->f);
    print_variables(*info);

    return 0;
}

unsigned int D_VM::analyse_expr(Instruction& inc,CallInfo* info) {
    logger->debug("<-----  analyse expression  ----->");
    D_VAR tleft;
    D_VAR tright ;
    D_VAR& tres = inc.right;

    logger->debug("cur line is :  ",inc.curpos);
    logger->debug("left pos is :  ",inc.lpos);
    logger->debug("right pos is :  ",inc.rpos);
    logger->debug("operator code is :  ",(int)inc.type);

    if ( FIN_END != inc.lpos ) {
        tleft = info->f->codes[inc.lpos].right;
    }

    if ( FIN_END != inc.rpos ) {
        tright = info->f->codes[inc.rpos].right;
    }

    //暂时不支持用户变量查找
    if ( VE_USER == tleft.type ) {
        D_VAR* tmp = find_variable(tleft.var.chv,info);
        if(!tmp){
            logger->error("left var name not found");
            return ERR_END;
        }
        tleft = *tmp;
    }

    if ( VE_USER == tright.type ) {
        D_VAR* tmp = find_variable(tright.var.chv,info);
        if(!tmp){
            logger->error("right var name not found");
            return ERR_END;
        }
        tright = *tmp;
    }

    //如果只有一个值
    if ( VE_VOID == tleft.type && VE_USER == inc.left.type ) {
        D_VAR* tmp = find_variable(inc.left.var.chv,info);
        if(!tmp){
            logger->error("single var name not found");
            return ERR_END;
        }
        tleft = *tmp;
    }

    logger->debug("left type is  : ",(int)tleft.type);
    logger->debug("right type is  : ",(int)tright.type);
    //解析指令
    switch (inc.type)
    {
    case OC_ADD:{
        if(D_VAR_ADD(tres,tleft,tright)){
            logger->error("error  in  add  expression");
            return ERR_END;
        }
        break;
    }
    case OC_SUB:{
        if(D_VAR_SUB(tres,tleft,tright)) {
            logger->error("error  in  sub  expression");
            return ERR_END;
        }
        break;
    }
    case OC_MUL:{
        if(D_VAR_MUL(tres,tleft,tright)){
            logger->error("error  in  multiply  expression");
            return ERR_END;
        }
        break;
    }
    case OC_MOD:{
        if(D_VAR_MOD(tres,tleft,tright)){
            logger->error("error  in  mod  expression");
            return ERR_END;
        }
        break;
    }
    case OC_DIV:{
        if(D_VAR_DIV(tres,tleft,tright)){
            logger->error("error  in  divide  expression");
            return ERR_END;
        }
        break;
    }
    case OC_AND: {
        if(D_VAR_AND(tres,tleft,tright)){
            logger->error("error  in  and  expression");
            return ERR_END;
        }
        break;
    }
    case OC_OR:{
        if(D_VAR_OR(tres,tleft,tright)){
            logger->error("error  in  or  expression");
            return ERR_END;
        }
        break;
    }
    case OC_DEQ:{
        if(D_VAR_EQ(tres,tleft,tright)){
            logger->error("error  in  equal  expression");
            return ERR_END;
        }
        break;
    }
    case OC_NEQ:{
        if(D_VAR_NE(tres,tleft,tright)){
            logger->error("error  in  not  equal  expression");
            return ERR_END;
        }
        break;
    }
    case OC_LT:{
        if(D_VAR_LT(tres,tleft,tright)){
            logger->error("error  in  less  than  expression");
            return ERR_END;
        }
        break;
    }
    case OC_LE:{
        if(D_VAR_LE(tres,tleft,tright)){
            logger->error("error  in  less  equal  expression");
            return ERR_END;
        }
        break;
    }
    case OC_GT:{
        if(D_VAR_GT(tres,tleft,tright)){
            logger->error("error  in  greater  than  expression");
            return ERR_END;
        }
        break;
    }
    case OC_GE:{
        if(D_VAR_GE(tres,tleft,tright)){
            logger->error("error  in  greater  equal  expression");
            return ERR_END;
        }
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
        // logger->error(ls._row,":",ls._col,"code parse error ");
        // break;
        return ERR_END;
    }
    }

    //结果数据为负数
    if ( OC_MINUS == inc.restype ) {
        tres = -tres;
    }

    logger->debug("result   type   ",(int)tres.type);
    logger->debug("result   value  ",tres);

    return 0;
}

FunHead* D_VM::find_function(const std::string& name,CallInfo* info ) {
    CallInfo* tmpin;
    // CallInfo* tmpin = info;
    unsigned int i = this->st->cs.size();
    while ( tmpin && i-- )
    {
        tmpin = this->st->cs[i];
        if ( tmpin->sv.find(name) != tmpin->sv.end() ) {
            return tmpin->f;
        }
    }
    return nullptr;
}

CallInfo* D_VM::last_return(CallInfo* info) { 
    int i = this->st->cs.size() - 1;
    if ( i <= 0 ) return nullptr;
    CallInfo* tmpin = this->st->cs[i];

    //如果不是匿名函数
    if (!tmpin->anonymous) {
        --i;
        if ( i < 0 ) return nullptr;
        tmpin->pos = tmpin->f->codes.size();
        tmpin =  this->st->cs[i];
    }
    //如果是匿名函数
    else {
        while ( tmpin )
        {
            if (!tmpin->anonymous) {
                break;
            }
            --i;
            if ( i < 0 ) return nullptr;
            tmpin->pos = tmpin->f->codes.size();
            tmpin = this->st->cs[i];
        }

        //再找上一个函数
        --i;
        if ( i < 0 ) return nullptr;

        tmpin->pos = tmpin->f->codes.size();
        tmpin =  this->st->cs[i];

    }

    while ( tmpin )
    {
        if (!tmpin->anonymous) {
            return tmpin;
        }
        --i;
        if ( i < 0 ) return nullptr;
        tmpin = this->st->cs[i];
        // tmpin->pos = tmpin->f->codes.size();
    }

    //返回全局函数
    return nullptr;
}

CallInfo* D_VM::last_function(CallInfo* info) {
    unsigned int i = this->st->cs.size() - 1;
    if ( i < 0 ) return nullptr;
    CallInfo* tmpin = tmpin = this->st->cs[i];

    //匿名函数查找
    while (tmpin)
    {
        if (!tmpin->anonymous) return tmpin; 
        --i;
        if ( i < 0 ) return nullptr;
        tmpin =  this->st->cs[i];
    }

    return nullptr;
}

D_VAR* D_VM::find_variable(const std::string& name,CallInfo* info) {
    CallInfo* tmpin ;
    unsigned int i = this->st->cs.size();
    
    while ( tmpin && i-- )
    {
        tmpin = this->st->cs[i];
        if ( tmpin->sv.find(name) != tmpin->sv.end() ) {
            return &tmpin->sv[name];
        }
    }

    return nullptr;
}

void D_VM::print_variables(const CallInfo& call) {
    for ( auto i : call.sv ) {
        logger->error(i.first," -> ",i.second);
    }
}


_QD_END
