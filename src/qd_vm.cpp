#include "qd_vm.h"
#include "qd_varlib.h"

_QD_BEGIN


D_VM::D_VM() {
    this->init();
    global = new CallInfo();
    st = new CallStack();
}

D_VM::D_VM(const D_VM& vm) {
    this->init();

    // fun = new FunHead(*vm.fun);
    global = new CallInfo(*vm.global);
    st = new CallStack(*vm.st);
}

void D_VM::init() {

    st = nullptr;
    global = nullptr;
    lib = nullptr;

    logger = Logger::getInstance();

}

D_VM::~D_VM() {
    //清除parser 传递函数

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

void D_VM::reserve_global() {
    while ( !this->st->cs.empty() ) {
        if (this->st->top() == this->global) break;
        delete this->st->top();
        this->st->cs.pop_back();
    }
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

size_t D_VM::init_fun(FunHead* fun) {
    if ( fun == nullptr ) {
        logger->error("function is null");
        return 1;
    }

    global->f = fun;

    push_call(global);

    return 0;
}

size_t D_VM::init_lib(D_LIB* l) {
    if (l == nullptr) {
        return 1;
    }
    this->lib = l;
    if (this->lib == nullptr) {
        return 1;
    }

    //变量加载

    return 0;
}

size_t D_VM::execute() {
    return this->execute(0);
}

size_t D_VM::execute(size_t i) {
    // CallInfo* call = new CallInfo();
    //全局函数,全局函数还是拷贝一份免得污染
    // call->f = new FunHead(*fun);  //这里到时候改，接受的是之前parser的函数信息
    global->pos = i;

    if(analyse_code(cur_fun()->pos,cur_fun())){
        return ERR_END;
    }

    return 0;
}

void D_VM::default_assign(const std::string& name,const D_VAR& var,CallInfo* const info) {
    info->v(name) = var;
}

void D_VM::default_assign(const std::string& name,const D_OBJ& var,CallInfo* const info) {
    info->v(name) = var;
}

void D_VM::local_assign(const std::string& name,const D_VAR& var,CallInfo* const info) {
    last_var(info)->v(name) = var;
}

void D_VM::local_assign(const std::string& name,const D_OBJ& var,CallInfo* const info) {
    last_var(info)->v(name) = var;
}

void D_VM::global_assign(const std::string& name,const D_VAR& var) {
    this->head_fun()->v(name) = var;
}

void D_VM::global_assign(const std::string& name,const D_OBJ& var) {
    this->head_fun()->v(name) = var;
}

size_t D_VM::analyse_code(size_t& i,CallInfo* info){
    size_t clen = info->f->codes.size();

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
            case OC_LIB:
            {
                if (analyse_lib_expr(inc,this->cur_fun())) {
                    logger->error("analyse lib expression error");
                    return ERR_END;
                }
                break;
            }
            case OC_JMP:
            {
                logger->debug("<-----  analyse  jump  ----->");
                logger->debug("cur line is :  ",inc.curpos);
                logger->debug("left pos is :  ",inc.lpos);
                logger->debug("right pos is :  ",inc.rpos);
                logger->debug("operator code is :  ",(int)inc.type);

                //判断是否跳转
                if ( FIN_END != inc.rpos ) {
                    Instruction& jump = info->f->codes[inc.rpos];
                    if ( !jump.right.var.bv ) {
                        i = inc.lpos;
                    }
                }
                // 单纯跳转指令
                else{
                    i = inc.lpos;
                }
                break;
            }
            case OC_WHILE:
            {
                logger->debug("<-----  analyse  while  ----->");
                logger->debug("cur line is :  ",inc.curpos);
                logger->debug("left pos is :  ",inc.lpos);
                
                CallInfo* call = new CallInfo();
                call->f = new FunHead(*cur_fun()->f->lfuns[inc.lpos]);

                this->push_call(call);
                if(analyse_code(cur_fun()->pos,cur_fun())){
                    return ERR_END;
                }
                //结束 释放
                delete this->cur_fun();
                this->pop_call();

                break;
            }
            case OC_ARG:
            {
                logger->debug("<-----  analyse  args  ----->");
                logger->debug("cur line is :  ",inc.curpos);
                logger->debug("left pos is :  ",inc.lpos);
                logger->debug("right pos is :  ",inc.rpos);
                logger->debug("left value is ->  ",inc.left);
                logger->debug("right value is ->  ",inc.right);

                break;
            }
            case OC_CALL:
            {
                logger->debug("<-----  analyse  call  ----->");
                logger->debug("cur line is :  ",inc.curpos);
                logger->debug("left pos is :  ",inc.lpos);
                logger->debug("right pos is :  ",inc.rpos);
                logger->debug("function name is ",inc.left.var.chv);
                
                CallInfo* call = new CallInfo();
                call->f = new FunHead( *find_function(inc.left.var.chv)->lfuns[inc.lpos] );
                // call->f = new FunHead(*info->f->lfuns[inc.lpos]);
                if (!call->f){
                    logger->error("function error");
                    return ERR_END;
                }
                this->push_call(call);
                
                //输入参数
                if(this->input_args(inc,info,call)){
                    logger->error("args input error");
                    return ERR_END;
                }

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
                
                CallInfo* call = last_function(info);

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

                Instruction& tmp = info->f->codes[inc.rpos];
                if (tmp.right.var.bv && !info->ifstate) {
                    info->ifstate = true;
                    this->push_call(call);
                    if(analyse_code(cur_fun()->pos,cur_fun())){
                        return ERR_END;
                    }
                    delete this->cur_fun();
                    this->pop_call();
                }
                
                break;
            }
            case OC_END:
            {
                info->ifstate = false;
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
                
                if (analyse_assign(inc,info)){
                    logger->error("analyse code error");
                    return ERR_END;
                }
                break;
            }
            case OC_ARR_IAS:
            {
                if (analyse_array_index_assign(inc,*info->f)) {
                    logger->error("error in array index assign function");
                    return ERR_END;
                }
                break;
            }
            case OC_ARR_VAL:
            {
                logger->debug("<-----  analyse array assign  ----->");
                logger->debug("cur line is :  ",inc.curpos);
                logger->debug("left  value is ->  ",inc.left);
                logger->debug("right  value is ->  ",inc.right);
                logger->debug("operator code is :  ",(int)inc.type);
                //判断数组是否定义
                D_OBJ* tmp = find_variable(inc.left.var.chv);
                if (!tmp) {
                    D_UNION du;
                    du.larr.push_back(inc.right);
                    info->v(inc.left.var.chv) = du;
                }
                else {
                    tmp->push(inc.right);
                }
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

    // print_variables(info);

    return 0;
}

size_t D_VM::analyse_expr(Instruction& inc,CallInfo* info) {
    logger->debug("<-----  analyse expression  ----->");
    D_VAR tleft;
    D_VAR tright;
    D_VAR& tres = inc.right;
    D_OBJ* array = nullptr;

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

    
    if ( VE_USER == tleft.type ) {
        D_OBJ* tmp = find_variable(tleft.var.chv);
        if(!tmp){
            logger->error("left var name not found");
            return ERR_END;
        }
        //不为数组类型才赋值
        tleft = *tmp;            
    }

    if ( VE_USER == tright.type ) {
        D_OBJ* tmp = find_variable(tright.var.chv);
        if(!tmp){
            logger->error("right var name not found");
            return ERR_END;
        }
        //不为数组类型才赋值
        tright = *tmp;
    }


    //如果只有一个值
    if ( VE_VOID == tleft.type ) {
        if ( VE_UNION == inc.left.type ) {
            array = find_variable(inc.left.var.chv);
            if(!array){
                logger->error("union var name not found");
                return ERR_END;
            }
        }
        else if ( VE_USER == inc.left.type ) {
            array = find_variable(inc.left.var.chv);
            if(!array){
                logger->error("single var name not found");
                return ERR_END;
            }
            tleft = *array;
        }
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
        if ( 0 == tright.var.iv || 0.0 == tright.var.dv ) {
            logger->error("dividend is not allow 0 or 0.0");
            return ERR_END;
        }
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
    //数组下标访问
    case OC_ARR_ACE:
    {
        //超出数组范围进行判断
        Instruction& acess = info->f->codes[inc.rpos];
        size_t pos = acess.right.var.iv;
        if (!array) {
            return ERR_END;
        }
        size_t len = array->uni->larr.size() - 1;
        
        //超出范围
        if ( pos < 0 || pos > len ) {
            logger->error("array access out of range");
            return ERR_END;
        }
        tres = array->uni->larr[pos];
        
        break;
    }
    case OC_NULL: {
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

size_t D_VM::analyse_assign(Instruction& inc,CallInfo* info) {
    if ( FIN_END == inc.rpos ) {
        logger->error("missing rpos to assign");
        return ERR_END;
    }

    Instruction& ass = info->f->codes[inc.rpos]; 

    //在这里辨别，数组，普通变量
    //作用域判断
    if ( FIN_END == inc.lpos ) {
        logger->error("variable scope error");
        return ERR_END;
    }
    
    //数组
    if ( VE_UNION == ass.right.type ) {
        default_assign(inc.left.var.chv,*find_variable(ass.right.var.chv),info);
        logger->error(inc.left.var.chv," -> ",info->v(inc.left.var.chv));
    }
    else {
        if ( VA_DEFAULT == inc.lpos ) {
            default_assign(inc.left.var.chv,ass.right,info);
        }
        else if ( VA_LOCAL == inc.lpos ) {
            local_assign(inc.left.var.chv,ass.right,info);
        }
        else if ( VA_GLOBAL == inc.lpos ) {
            global_assign(inc.left.var.chv,ass.right);
        }
        logger->debug(inc.left.var.chv," -> ",ass.right);
    }
    
    
    return 0;
}

size_t D_VM::analyse_array_index_assign(Instruction& inc,FunHead& fun) {
    if ( FIN_END == inc.rpos ) {
        logger->error("error in array index assign");
        return ERR_END;
    }
    Instruction& value = fun.codes[inc.rpos];
    
    size_t index = 0;
    
    D_OBJ* array =  find_variable(inc.left.var.chv);

    if (!array) {
        logger->error("union is not exist");
        return ERR_END;
    }

    size_t len = array->uni->larr.size() - 1;

    if ( VE_USER == inc.right.type ) {
        D_OBJ* tmp = find_variable(inc.right.var.chv);
        if (!tmp) {
            logger->error("index variable not exist");
            return ERR_END;
        }
        index = tmp->var.iv;
    }
    else if ( VE_INT == inc.right.type ){
        index = inc.right.var.iv;
    }
    else {
        logger->error("error type in union index");
        return ERR_END;
    }

    if ( index < 0 || index > len ) {
        logger->error("index out of range");
        return ERR_END;
    }

    array->uni->larr[index] = value.right;
    return 0;
}

size_t D_VM::analyse_lib_expr(Instruction& inc,CallInfo* fun) {
    logger->debug("<-----  analyse  lib  ----->");

    size_t len = inc.lpos;
    fun->f->set_state_pos(len);

    auto start = fun->f->state->vars.begin();

    while (len)
    {
        if ( VE_USER == start->type ) {
            //parser 阶段已经检查过变量是否存在
            *start = *find_variable(start->var.chv);
        }
        start ++;
        len --;
    }

    //开始参数更新操作
    //内部函数调用
    (this->lib->l[inc.left.var.chv]->funs[inc.right.var.chv])(fun->f->state);
    return 0;
}

size_t D_VM::input_args(const Instruction& inc,CallInfo* cur,CallInfo* push) {
    //空参
    if (inc.rpos == FIN_END){
        return 0;
    }
    //push func
    Instruction tmp = cur->f->codes[inc.rpos];
    size_t len = push->f->args_size();
    size_t i = len - 1;
    for (; i >= 0 ; i--) {
        //查找变量值
        if ( VE_USER == tmp.right.type ) {
            D_OBJ* t = find_variable(tmp.right.var.chv);
            if(!t){
                logger->error("function args name not found");
                return ERR_END;
            }
            tmp.right = *t;
        }

        push->v(push->f->args[i]) = tmp.right;
        if (tmp.rpos == FIN_END) break;
        tmp = cur->f->codes[tmp.rpos];
    }

    if (i) {
        return ERR_END;
    }

    return 0;
}

FunHead* D_VM::find_function(const std::string& name ) {
    int i = this->st->cs.size() - 1;
    
    if ( i < 0 ) return nullptr;
    CallInfo* tmpin = this->st->cs[i];

    while ( tmpin )
    {
        if ( tmpin->sv.find(name) != tmpin->sv.end() ) {
            return tmpin->f;
        }
        --i;
        if ( i < 0 ) return nullptr;
        tmpin =  this->st->cs[i];
    }

    return nullptr;
}

D_OBJ* D_VM::find_variable(const std::string& name) {
    //这个必须是int类型
    int i = this->st->cs.size() - 1;
    if ( i < 0 ) return nullptr;
    CallInfo* tmpin = this->st->cs[i];

    while ( tmpin )
    {
        if ( tmpin->sv.find(name) != tmpin->sv.end() ) {
            return &tmpin->sv[name];
        }
        --i;
        if ( i < 0 ) return nullptr;
        tmpin =  this->st->cs[i];
    }

    return nullptr;
}

CallInfo* D_VM::last_function(CallInfo* info) { 
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
    }

    return nullptr;
}

CallInfo* D_VM::last_var(CallInfo* info) {
    int i = this->st->cs.size() - 1;
    if ( i <= 0 ) return this->head_fun();
    CallInfo* tmpin = this->st->cs[i];

    for (;;)
    {
        i --;
        if ( i < 0 ) return this->head_fun();
        tmpin = this->st->cs[i];
        if (!tmpin->anonymous) break;
    }
    //返回全局函数
    return tmpin;
}

void D_VM::print_variables(const CallInfo* call) {
    
    for (
        std::map<std::string, D_OBJ>::const_iterator 
        iter = call->sv.begin();
        iter != call->sv.end() ; iter ++ ) {
        std::cout << iter->first << " -> " << iter->second << std::endl;
    }

}


_QD_END
