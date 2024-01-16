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

size_t D_VM::size_call() {
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
        return EV_NULL;
    }

    global->f = fun;

    push_call(global);

    return EV_OK;
}

size_t D_VM::init_lib(D_LIB* l) {
    if (l == nullptr) {
        return EV_NULL;
    }
    this->lib = l;
    if (this->lib == nullptr) {
        return EV_NULL;
    }

    //变量加载

    return EV_OK;
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
        return EV_SYS;
    }

    return EV_OK;
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

size_t D_VM::assing_variable(const Instruction& inc,const D_OBJ& var,CallInfo* const info) {
    if ( VA_DEFAULT == inc.lpos ) {
        default_assign(inc.left.var.chv,var,info);
    }
    else if ( VA_LOCAL == inc.lpos ) {
        local_assign(inc.left.var.chv,var,info);
    }
    else if ( VA_GLOBAL == inc.lpos ) {
        global_assign(inc.left.var.chv,var);
    }
    return EV_OK;
}

size_t D_VM::assing_variable(const Instruction& inc,const D_VAR& var,CallInfo* const info) {
    if ( VA_DEFAULT == inc.lpos ) {
        default_assign(inc.left.var.chv,var,info);
    }
    else if ( VA_LOCAL == inc.lpos ) {
        local_assign(inc.left.var.chv,var,info);
    }
    else if ( VA_GLOBAL == inc.lpos ) {
        global_assign(inc.left.var.chv,var);
    }
    return EV_OK;
}


size_t D_VM::analyse_code(size_t& i,CallInfo* info){
    size_t clen = info->f->codes.size();

    //目前是为了防止栈溢出简单的处理
    if ( st->cs.size() > QD_STACK_MAX ) {
        logger->error("stack overflow error ");
        return EV_SYS;
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
                    return EV_SYS;
                }
                break;
            }
            case OC_LIBV:
            {
                //判断是否为用户变量
                if ( VE_USER ==  inc.left.type )
                {
                    D_OBJ* tmp = find_variable(inc.left.var.chv);

                    if (!tmp) {
                        logger->error("variable not found in lib function");
                        return EV_SYS;
                    }
                    
                    //判断是否符合条件
                    if ( OC_MINUS == inc.restype ) {
                        if ( VE_INT == tmp->type || VE_FLT == tmp->type || VE_BOOL == tmp->type ) {
                            info->f->state->vars.push_back(-(*tmp));
                        }
                        else {
                            logger->error(*tmp," has no negative to use ");
                            info->f->state->vars.push_back(*tmp);
                        }
                    }
                    else {
                        info->f->state->vars.push_back(*tmp);
                    }
                }
                else {
                    if ( OC_MINUS == inc.restype ) {
                        info->f->state->vars.push_back(-inc.left);
                    }
                    else {
                        info->f->state->vars.push_back(inc.left);
                    }
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
            case OC_VLOOP:
            {
                logger->debug("<-----  analyse  loop  value  ----->");
                logger->debug("cur line is :  ",inc.curpos);
                logger->debug("left pos is :  ",inc.lpos);
                logger->debug("right pos is :  ",inc.rpos);
                logger->debug("left  value is :  ",inc.left.var.chv);
                logger->debug("operator code is :  ",(int)inc.type);

                D_OBJ* var = find_variable(inc.right.var.chv);
                //总长度
                size_t step = 0;

                if ( !var ) {
                    logger->error("var is not found in OC_VLOOP ");
                    return EV_SYS;
                }
                else if ( VE_INT == var->type ) {
                    inc.lpos++;
                    step = var->var.iv;
                    //这个值小于int最大值
                    if ( inc.lpos >= QD_INT32_MAX ) {
                        logger->error("for loop index is out of int32 maximum range");
                        return EV_SYS;
                    }
                    D_VAR in = (int)inc.lpos;
                    default_assign(inc.left.var.chv,in,info);
                }
                else if ( VE_ARRAY == var->type ) {
                    step = var->size();
                    D_PRO pro = var->arr->arr[inc.lpos];

                    D_VAR ar;
                    if ( VE_STR == var->at ) {
                        ar = pro.chv;
                    }
                    else
                    {
                        ar.var = pro;
                    }
                    ar.type = var->at;

                    default_assign(inc.left.var.chv,ar,info);
                    inc.lpos++;
                }
                else if ( VE_UNION == var->type ) {
                    step = var->size();
                    default_assign(inc.left.var.chv,var->uni->un[inc.lpos],info);
                    inc.lpos++;
                }
                else {
                    logger->error("var type is error in OC_VLOOP ");
                    return EV_SYS;
                }

                //跳出循环
                if ( inc.lpos > step ) {
                    info->pos = info->f->codes.size();
                }

                break;
            }
            case OC_LOOP:
            {
                logger->debug("<-----  analyse  loop  ----->");
                logger->debug("cur line is :  ",inc.curpos);
                logger->debug("left pos is :  ",inc.lpos);
                
                CallInfo* call = new CallInfo();
                call->anonymous = true;
                call->f = new FunHead(*cur_fun()->f->lfuns[inc.lpos]);

                this->push_call(call);
                if(analyse_code(cur_fun()->pos,cur_fun())){
                    return EV_SYS;
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
                // logger->error(inc);
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

                if (!call->f){
                    logger->error("function error");
                    return EV_SYS;
                }
                
                //输入参数
                if(this->input_args(inc,info,call)){
                    logger->error("args input error");
                    return EV_SYS;
                }
                //这个必须放在输入参数后面,因为input_args 是从栈顶开始查找变量
                this->push_call(call);

                if(analyse_code(cur_fun()->pos,cur_fun())){
                    return EV_SYS;
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
                
                CallInfo* call = last_return();

                
                //如果为全局函数的话,直接执行后续步骤
                if (!call) {
                    continue;
                }
                if ( FIN_END == inc.rpos ) {
                    D_OBJ tmpobj;
                    tmpobj.type = VE_NULL;
                    call->v(inc.left.var.chv) = tmpobj;
                }
                //保存上一级函数返回值变量,匿名函数不赋值直接跳出
                else  {
                    call->v(inc.left.var.chv) = info->f->codes[inc.rpos].right;
                }

                break;
            }
            case OC_BRK:
            {
                logger->debug("<-----  analyse  break  ----->");
                logger->debug("cur line is :  ",inc.curpos);

                last_break();
                
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
                        return EV_SYS;
                    }
                    delete this->cur_fun();
                    this->pop_call();
                }
                else {
                    //要析构
                    delete call;
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
                    return EV_SYS;
                }
                break;
            }
            //数组下标访问
            case OC_ARR_GET:
            {
                //超出数组范围进行判断
                Instruction& acess = info->f->codes[inc.rpos];
                size_t pos;
                size_t len;

                //查找索引位置

                D_OBJ* arr = find_variable(inc.left.var.chv);
                if (!arr) {
                    logger->error("arr get error arr or union not found ");
                    return EV_SYS;
                }
                else {
                    pos =  acess.right.var.iv;
                }

                //判断类型
                if ( VE_UNION == arr->type ) {
                    len = arr->uni->un.size();
                }
                else {
                    len = arr->arr->arr.size();
                }
                
                //超出范围
                if ( pos < 0 || pos > len  - 1 ) {
                    logger->error("array access out of range");
                    return EV_SYS;
                }

                //赋值
                if ( VE_ARRAY == arr->type ) {
                    if ( VE_STR == arr->at ) {
                        inc.right = arr->arr->arr[pos].chv;
                    }
                    else
                    {
                        inc.right.var = arr->arr->arr[pos];
                    }
                    inc.right.type = arr->at;
                }
                else {
                    inc.right = arr->uni->un[pos];
                }
                
                break;
            }
            case OC_ARR_SET:
            {
                if (analyse_array_index_assign(inc,*info->f)) {
                    logger->error("error in array index assign function");
                    return EV_SYS;
                }
                break;
            }
            case OC_ARR_NEW:
            {
                logger->debug("<-----  analyse array new  ----->");
                logger->debug("cur line is :  ",inc.curpos);
                logger->debug("left  value is ->  ",inc.left);
                logger->debug("right  value is ->  ",inc.right);
                logger->debug("operator code is :  ",(int)inc.type);

                D_OBJ* array = find_variable(inc.left.var.chv);

                //判断数组是否定义
                if (array) {
                    array->clear();
                }
                //union
                if (1 == inc.rpos) {
                    D_UNION du;
                    assing_variable(inc,du,info);
                }else if (!inc.rpos) {
                    D_ARRAY ar;
                    assing_variable(inc,ar,info);
                }
                else {
                    logger->error("is not array or union type");
                    return EV_SYS;
                }
                break;
            }
            default:
            {
                if(analyse_expr(inc,info)) {
                    logger->error("analyse  expresion  error  in  analyse  code");
                    return EV_SYS;
                }
                break;
            }
        }
    }
    
    logger->debug("<------  function  analyse  end   ------>");
    logger->debug("after stack size is ",this->st->cs.size());

    // print_variables(info);

    return EV_OK;
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
            return EV_SYS;
        }
        //不为数组类型才赋值
        tleft = *tmp;            
    }

    if ( VE_USER == tright.type ) {
        D_OBJ* tmp = find_variable(tright.var.chv);
        if(!tmp){
            logger->error("right var name not found");
            return EV_SYS;
        }
        //不为数组类型才赋值
        tright = *tmp;
    }

    logger->debug("left type is :  ",(short)inc.left.type);
    logger->debug("right type is :  ",(short)inc.right.type);

    //如果只有一个值
    if ( VE_VOID == tleft.type ) {
        if ( VE_UNION == inc.left.type ) {
            array = find_variable(inc.left.var.chv);
            if(!array){
                logger->error("union var name not found");
                return EV_SYS;
            }
        }
        else if ( VE_USER == inc.left.type ) {
            array = find_variable(inc.left.var.chv);
            if(!array){
                logger->error("var is not exist ");
                return EV_SYS;
            }
            else if ( VE_NULL == array->type ) {
                logger->error("maybe function return is null");
                return EV_SYS;
            }
            tleft = *array;
        }
    }
    
    
    //解析指令
    switch (inc.type)
    {
    case OC_ADD:{
        if(D_VAR_ADD(tres,tleft,tright)){
            logger->error("error  in  add  expression");
            return EV_SYS;
        }
        break;
    }
    case OC_SUB:{
        if(D_VAR_SUB(tres,tleft,tright)) {
            logger->error("error  in  sub  expression");
            return EV_SYS;
        }
        break;
    }
    case OC_MUL:{
        if(D_VAR_MUL(tres,tleft,tright)){
            logger->error("error  in  multiply  expression");
            return EV_SYS;
        }
        break;
    }
    case OC_MOD:{
        if ( 0 == tright.var.iv ) {
            logger->error("mod is not allow 0 ");
            return EV_SYS;
        }
        if(D_VAR_MOD(tres,tleft,tright)){
            logger->error("error  in  mod  expression");
            return EV_SYS;
        }
        break;
    }
    case OC_DIV:{
        // if ( 0 == tright.var.iv || 0.0 == tright.var.dv ) {
        //     logger->error("dividend is not allow 0 or 0.0");
        //     return EV_SYS;
        // }
        if(D_VAR_DIV(tres,tleft,tright)){
            logger->error("error  in  divide  expression");
            return EV_SYS;
        }
        break;
    }
    case OC_AND: {
        if(D_VAR_AND(tres,tleft,tright)){
            logger->error("error  in  and  expression");
            return EV_SYS;
        }
        break;
    }
    case OC_OR:{
        if(D_VAR_OR(tres,tleft,tright)){
            logger->error("error  in  or  expression");
            return EV_SYS;
        }
        break;
    }
    case OC_DEQ:{
        if(D_VAR_EQ(tres,tleft,tright)){
            logger->error("error  in  equal  expression");
            return EV_SYS;
        }
        break;
    }
    case OC_NEQ:{
        if(D_VAR_NE(tres,tleft,tright)){
            logger->error("error  in  not  equal  expression");
            return EV_SYS;
        }
        break;
    }
    case OC_LT:{
        if(D_VAR_LT(tres,tleft,tright)){
            logger->error("error  in  less  than  expression");
            return EV_SYS;
        }
        break;
    }
    case OC_LE:{
        if(D_VAR_LE(tres,tleft,tright)){
            logger->error("error  in  less  equal  expression");
            return EV_SYS;
        }
        break;
    }
    case OC_GT:{
        if(D_VAR_GT(tres,tleft,tright)){
            logger->error("error  in  greater  than  expression");
            return EV_SYS;
        }
        break;
    }
    case OC_GE:{
        if(D_VAR_GE(tres,tleft,tright)){
            logger->error("error  in  greater  equal  expression");
            return EV_SYS;
        }
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
        // break;
        return EV_SYS;
    }
    }

    //结果数据为负数
    if ( OC_MINUS == inc.restype ) {
        tres = -tres;
    }


    logger->debug("result   type   ",(int)tres.type);
    logger->debug("result   value  ",tres);

    return EV_OK;
}

size_t D_VM::analyse_assign(Instruction& inc,CallInfo* info) {
    if ( FIN_END == inc.rpos ) {
        logger->error("missing rpos to assign");
        return EV_SYS;
    }

    Instruction& ass = info->f->codes[inc.rpos]; 
    //在这里辨别，数组，普通变量
    //作用域判断
    if ( FIN_END == inc.lpos ) {
        logger->error("variable scope error");
        return EV_SYS;
    }
    

    //数组
    if ( VE_UNION == ass.right.type || VE_ARRAY == ass.right.type ) {
        //因为可能会出现重名的情况,如果重名可能会赋值有问题
        assing_variable(inc,*find_variable(ass.right.var.chv),info);
    }
    else {
        assing_variable(inc,ass.right,info);
    }
    
    
    return EV_OK;
}

size_t D_VM::analyse_array_index_assign(Instruction& inc,FunHead& fun) {
    logger->debug("<-----  analyse array set  ----->");
    logger->debug("cur line is :  ",inc.curpos);
    logger->debug("left  value is ->  ",inc.left);
    logger->debug("right  value is ->  ",inc.right);
    logger->debug("left pos is :  ",inc.lpos);
    logger->debug("right pos is :  ",inc.rpos);
    logger->debug("operator code is :  ",(int)inc.type);

    D_OBJ* array = nullptr;
    if ( FIN_END == inc.rpos ) {
        array = find_variable(inc.left.var.chv);
        if(!array) {
            logger->error("array or union not found ");
            return EV_SYS;
        }
        else if ( VE_ARRAY == array->type && VE_VOID == array->at )
        {
            array->at = inc.right.type;
        }

        array->push_back(inc.right);
    }
    else {
        array = find_variable(inc.left.var.chv);
        if(!array) {
            logger->error("array or union not found ");
            return EV_SYS;
        }

        //数组下标
        int pos;
        if ( VE_INT == inc.right.type ) {
            //超出范围
            pos = inc.right.var.iv;
        }
        else {
            D_OBJ* var = find_variable(inc.right.var.chv);
            if (!var) {
                logger->error("array or union index variable is not exist");
                return EV_SYS;
            }
            else if ( VE_INT != var->type ) {
                logger->error("array index type is error");
                return EV_SYS;
            }
            else {
                pos = var->var.iv;
            }
        }
        
        if ( pos < 0 || pos > array->size() - 1 ) {
            logger->error("array index out of range");
            return EV_SYS;
        }

        D_VAR& var = fun.codes[inc.rpos].right;
        //这里判断数组类别
        if ( VE_ARRAY == array->type ) {
            D_PRO pro;
            if (var.type != array->at ) {
                logger->error("can not assign this type into array");
                return EV_SYS;
            }
            //如果是字符串
            else if ( VE_STR == array->at ) {
                //有些繁琐
                size_t len = strlen(var.var.chv);
                pro.chv = new char[len + 1];
                strncpy(pro.chv,var.var.chv,len);
            }
            else {
                memcpy(&pro,&var,sizeof(D_PRO));
            }

            array->arr->arr[pos] = pro;
        }
        else if ( VE_UNION == array->type ) {
            array->uni->un[pos] = var;
        }
    }

    return EV_OK;
}

size_t D_VM::analyse_lib_expr(Instruction& inc,CallInfo* fun) {
    logger->debug("<-----  analyse  lib  ----->");

    FunHead& s = *fun->f;

    s.set_state_pos(inc.lpos);

    if ( s.state->v_pos > s.state->vars.size() ) {
        logger->error("v_pos is larger than args size");
        return EV_SYS;
    }

    size_t ret = ( this->lib->l[inc.left.var.chv]->funs[inc.right.var.chv] )( s.state );
    
    //内部函数调用
    if ( ret ) {
        logger->error("system call error");
        return EV_SYS;
    }

    if (s.state->rets.empty()) {
        logger->error("cur lib returns is empty");
        return EV_SYS;
    }
    //调用完后将变量赋值到ret
    fun->v(QD_KYW_RET) = s.state->rets.front();
    
    //出栈
    s.state->rets.pop_front();

    return EV_OK;
}

size_t D_VM::input_args(const Instruction& inc,CallInfo* cur,CallInfo* push) {
    //空参
    if (inc.rpos == FIN_END){
        return EV_OK;
    }
    
    //push func
    Instruction tmp = cur->f->codes[inc.rpos];
    size_t len = push->f->args_size();
    size_t i = len - 1;
    for (; i >= 0 ; i--) {
        D_OBJ input;
        //查找变量值
        if ( VE_USER == tmp.right.type ) {
            D_OBJ* t = find_variable(tmp.right.var.chv);
            if(!t){
                logger->error("function args name not found");
                return EV_SYS;
            }
            input = *t;
        }
        else {
            input = tmp.right;
        }

        //暂时先不支持输入数组
        if ( VE_UNION == input.type || VE_ARRAY == input.type ) {
            logger->error("is not support array or union now");
            return EV_SYS;
        }

        if ( OC_MINUS == tmp.restype  ) {
            if (  VE_INT == input.type || VE_FLT == input.type || VE_BOOL == input.type ) {
                push->v(push->f->args[i]) = -input;
            }
            else {
                logger->warn(tmp.right," has no negative to use ");
                push->v(push->f->args[i]) = input;
            }
        }
        else {
            push->v(push->f->args[i]) = input;
        }

        if (tmp.rpos == FIN_END) break;
        tmp = cur->f->codes[tmp.rpos];
    }

    if (i) {
        return EV_SYS;
    }

    return EV_OK;
}

FunHead* D_VM::find_function(const std::string& name ) {
    if (this->st->cs.empty()) return nullptr;
    size_t i = this->st->cs.size();
    
    CallInfo* tmpin = nullptr;

     while (i--)
    {
        tmpin =  this->st->cs[i];
        if ( tmpin->sv.find(name) != tmpin->sv.end() ) {
            return tmpin->f;
        }
    }

    return nullptr;
}

D_OBJ* D_VM::find_variable(const std::string& name) {
    if (this->st->cs.empty()) return nullptr;
    //这个必须是int类型
    size_t i = this->st->cs.size();

    CallInfo* tmpin = nullptr;

    while (i--)
    {
        tmpin =  this->st->cs[i];
        if ( tmpin->sv.find(name) != tmpin->sv.end() ) {
            return &tmpin->sv[name];
        }
    }

    return nullptr;

}

void D_VM::last_break() {
    if (!this->st->cs.size()) return;
    size_t i = this->st->cs.size() - 1;
    if (!i) return;
    CallInfo* tmp = this->st->cs[i];
    
    //匿名函数是前提
    while (tmp->anonymous)
    {
        tmp->pos = tmp->f->codes.size();
        --i;
        if (!i) break;
        tmp = this->st->cs[i];
    }
}

CallInfo* D_VM::last_return() {
    if (this->st->cs.empty()) return nullptr;

    //这里必须是有符号
    size_t i = this->st->cs.size() - 1;
    if ( !i ) return nullptr;
    CallInfo* tmpin = this->st->cs[i];

    //如果不是匿名函数
    if (!tmpin->anonymous) {
        tmpin->pos = tmpin->f->codes.size();
    }
    //如果是匿名函数
    else {
        while ( i-- )
        {
            tmpin->pos = tmpin->f->codes.size();
            tmpin = this->st->cs[i];

            if (!tmpin->anonymous) {
                break;
            }
        }

        //再找上一个函数
        if ( !i ) return nullptr;
        //这里放在后面的原因是全局函数不需要返回值
        tmpin->pos = tmpin->f->codes.size();
    }

    while ( i-- )
    {
        tmpin = this->st->cs[i];
        if (!tmpin->anonymous) {
            return tmpin;
        }
    }

    return nullptr;
}

CallInfo* D_VM::last_var(CallInfo* info) {
    if (this->st->cs.empty()) return this->head_fun();
    //这个必须是int类型
    size_t i = this->st->cs.size();

    CallInfo* tmpin = nullptr;

    while (i--)
    {
        tmpin =  this->st->cs[i];
        if (!tmpin->anonymous) {
            return tmpin;
        }
    }

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
