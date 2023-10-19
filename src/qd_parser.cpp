#include "qd_parser.h"

_QD_BEGIN


DParser::DParser(){
}

DParser::~DParser(){

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
    for(;;){
        this->parseX_next();
        if ( T_ERR == ls.t.token) {
            printf("\nlexcial error\n");
            break;
        } 
        if( T_EOF == ls.t.token ) {
            printf(" <------------------ end --------------------> \n");
            break;//结束
        }else{
            // printf("  %d\n",ls.t.token);
            //判断是哪个表达式
            switch (ls.t.token)
            {
            case T_UDATA:
                statement();
                break;
            default:
                int pretmp = parse_PreCode(ls.t.token);
                if (OC_NIL != pretmp){
                    findX_next();
                }
                this->suffix_expr(0,pretmp);
                break;
            }
            ++global.curline;
        }
    }
    analyse_code();

    ls.free_buff();
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

void DParser::statement(){
    Instruction inc;

    this->skip_blank();
    if ( T_UDATA != ls.t.token ) {
        printf("data error\n");
        return;
    }
    //记录左值
    inc.left = ls.dvar.var.chv;

    findX_next();
    //因为是赋值表达式所以判断=
    if ( T_EQ != ls.t.token ){
        return;
    }
    inc.type = OC_ASSIGN;

    findX_next();

    //判断前缀
    int pretmp = parse_PreCode(ls.t.token);
    if (OC_NIL != pretmp){
        findX_next();
    }

    //记录右值
    inc.rpos = this->suffix_expr(inc.type,pretmp);

    if (FIN_END == inc.rpos ) {
        inc.right = ls.dvar;
        inc.rfin = true;
        inc.sufftype = pretmp;
    }

    //只有两个指令 并且不是括号表达式
    if (1 == global.codes.size() && ls.lookahead.token == T_RPARENTH) inc.sufftype = pretmp;


    if ( T_END != ls.t.token ){
        printf("error end\n");
        return;
    }

    inc.curpos = global.codes.size();
    global.codes.push_back(inc);
}

//暂时用不到
int DParser::variable_check(int ltype,int pretype){
    // switch (ls.dvar.type)
    // {
    // case VE_STR:case VE_INT:case VE_FLT:
    //     return suffix_expr(ltype,pretype);
    // case VE_USER:
    //     // global.proto->lv[tmp] = global.proto->lv[ls.dvar.var.chv];
    //     global.proto->lv[ls.dvar.var.chv];
    //     return 0;
    // default://结束
    //     return -1;
    // }
}

unsigned int DParser::expr_stat(int ltype,int pretype){
    if ( T_END == ls.t.token || T_LPARENTH == ls.t.token) {
        return FIN_END;
    }

    Instruction inc;
    if (OC_MINUS == pretype){
        inc.left = -ls.dvar;
    }
    else{
        inc.left = ls.dvar;
    }

    findX_next();

    if ( ls.t.token == T_END  || ls.t.token == T_RPARENTH ){
        return FIN_END;
    }

    inc.ltype = ltype;//记录上一个操作符
    // 解析运算符    
    parse_Opr(inc,ls.t.token);

    if (inc.type == OC_NULL) {
        return FIN_END;
    }

    //这里判断优先级
    if ( priority[inc.ltype][0] > priority[inc.type][1] ) {
        return FIN_END;
    }

    // inc.pretype = pretype;
    findX_next();
    
    //判断前缀
    int pretmp = parse_PreCode(ls.t.token);
    if (OC_NIL != pretmp){
        findX_next();
    }

    if (T_LPARENTH == ls.t.token) {
        inc.rpos = this->suffix_expr(inc.type,pretmp);
    }
    else {
        inc.rpos = this->expr_stat(inc.type,pretmp);
    }
    //这里做下一个类型判断检查

    if ( FIN_END == inc.rpos ) {
        inc.right = ls.dvar;
        inc.sufftype = pretmp;
        inc.rfin = true;
    }
    
    //判断最后一个()负数
    if ( T_RPARENTH == ls.lookahead.token ) {
        inc.sufftype = pretmp;
    }

    inc.curpos = global.codes.size();
    global.codes.push_back(inc);

    return inc.curpos;
}

unsigned int DParser::prefix_expr(int type,int pretype){
    this->skip_blank();
    //需要一个临时变量判断程序运行结束
    unsigned int lastpos =  expr_stat(type,pretype);

    if ( FIN_END == lastpos ) {
        //只有一个值
        return FIN_END;
    }

    for(;;)
    {
        this->skip_blank();
        if ( T_EOF == ls.t.token ){
            //解析错误
            printf("parser error\n");
            break;
        }
        //表达式解析到最后
        if ( T_END ==  ls.t.token || ls.t.token == T_RPARENTH || ls.t.token == T_LPARENTH ){
            break;
        }
        Instruction inc;
        inc.lpos = lastpos;

        parse_Opr(inc,ls.t.token);
        findX_next();

        // 判断前缀
        int pretmp = parse_PreCode(ls.t.token);
        if (OC_NIL != pretmp){
            findX_next();
        }
        
        inc.rpos = expr_stat(inc.type,pretmp);

        if (FIN_END == inc.rpos ) {
            inc.right = ls.dvar;
            inc.sufftype = pretmp;
            inc.rfin = true;
        }

        //插入
        inc.curpos = lastpos = global.codes.size();
        global.codes.push_back(inc);
    }
    
    return lastpos;
}

unsigned int DParser::suffix_expr(int type,int pretype){ 
    unsigned int est = FIN_END;
    int pretmp;
    Instruction inc;
    inc.ltype = type;

    switch (ls.t.token)
    {
    case T_LPARENTH:{//括号在前
        inc.pretype = pretype;

        findX_next();
        pretmp = parse_PreCode(ls.t.token);
        if (OC_NIL != pretmp){
            findX_next();
        }

        inc.lpos = this->suffix_expr(0,pretmp);
        
        //只有一个
        if ( FIN_END == inc.lpos ) {
            inc.left = ls.dvar;
        }

        if ( T_RPARENTH == ls.t.token ) {
            findX_next();
            //已经结束
            if ( T_END == ls.t.token ) {
                //如果是一个表达式
                return inc.lpos;
            } 
            
            //嵌套
            if (T_RPARENTH == ls.t.token) {
                //如果是)返回出去
                return inc.lpos;
            }
            
            parse_Opr(inc,ls.t.token);
            
            //运算符优先级小
            if ( priority[inc.ltype][0] > priority[inc.type][1] ) {
                return inc.lpos;
            }
            findX_next();

            pretmp = parse_PreCode(ls.t.token);
            if (OC_NIL != pretmp){
                findX_next();
            }
            //紧挨着下一个()
            //例如 () * ()
            if (T_LPARENTH == ls.t.token) {
                inc.rpos = suffix_expr(0,pretmp);
                inc.right = ls.dvar;
                inc.rfin = false;
                //判断最后一个是不是()
                if ( T_RPARENTH == ls.lookahead.token) inc.sufftype = pretmp;
                
                inc.curpos = global.codes.size();
                global.codes.push_back(inc);
                return inc.curpos;
            }

            //-----------------------------------------------这里需要更改
            //临时变量
            inc.rpos = expr_stat(inc.type,pretmp);

            if (FIN_END == inc.rpos ) {
                inc.right = ls.dvar;
                inc.sufftype = pretmp;
                inc.rfin = true;
            }
            inc.curpos = global.codes.size();
            global.codes.push_back(inc);

            //------------------上面expr
            inc.rfin = false;
            inc.lfin = false;
            inc.lpos = inc.curpos;
            parse_Opr(inc,ls.t.token);
            inc.pretype = inc.sufftype = OC_NIL;//需要清空

            if ( OC_NULL == inc.type ) {
                return  inc.curpos;
            }
            findX_next();

            pretmp = parse_PreCode(ls.t.token);
            if (OC_NIL != pretmp){
                findX_next();
            }

            inc.rpos = this->suffix_expr(0,pretmp);
            if (FIN_END == inc.rpos ) {
                inc.right = ls.dvar;
                inc.sufftype = pretmp;
                inc.rfin = true;
            }            

            inc.curpos = global.codes.size();
            global.codes.push_back(inc);
            return inc.curpos;
        } 
        else {
            //错误处理
        }

        return FIN_END;
    }
    default:{//括号在后
        est = prefix_expr(type,pretype);
        if ( T_LPARENTH == ls.t.token ) {
            parse_Opr(inc,ls.lookahead.token);
            //这里判断优先级
            if ( priority[global.codes[est].ltype][0] > priority[inc.type][1]  ) {
                global.codes.pop_back();
                inc.rpos = this->suffix_expr(0,pretmp);
                inc.lpos = est - 1;
                inc.curpos = global.codes.size();
                global.codes.push_back(inc);
                return inc.curpos;
            }else{
                //错误处理
            }
        }
        else {
            //错误处理
        }
        return est; 
    }
    }
}

void DParser::print_variable(const std::string& name){
    std::cout << name ;
    const D_VAR& dv = global.proto->lv[name.c_str()];
    switch (dv.type)
    {
    case VE_INT:
        std::cout << "  : " << dv.var.iv << "  |  type : int"  << std::endl;
        break;
    case VE_FLT:
        std::cout << "  : " << dv.var.dv << "  |  type : double" << std::endl;
        break;
    case VE_BOOL:
        std::cout << "  : " << dv.var.bv << "  |  type : bool" << std::endl;
        break;
    case VE_STR:
    case VE_USER:
        std::cout << "  : " << dv.var.chv << "  |  type : string"   << std::endl;
        break;
    default:
        std::cout << "  variables  no  found " << std::endl;
        return;
    }
}

void DParser::print_variables(){
    //打印全局变量
    for (auto i: global.proto->lv){
        print_variable(i.first);
    }
}

unsigned int DParser::source_code_line(){
    return global.curline;
}

unsigned int DParser::bytes_code_line(){
    return global.codes.size();
}

unsigned int DParser::variable_count(){
    return global.proto->lv.size();
}

void DParser::analyse_code(){
    int clen = global.codes.size();
    for (int i = code_pos; i < clen ; i++ ) {
        Instruction& inc = global.codes[i];
        
        D_VAR& tleft =  inc.left;
        D_VAR& tright = inc.right;

        printf("cur line is :  %d\n",inc.curpos);
        printf("left pos is :  %d\n",inc.lpos);
        printf("right pos is :  %d\n",inc.rpos);
        printf("operator code is :  %d\n",inc.type);
        printf("last code is :  %d\n",inc.ltype);
        printf("left    value  %d\n",inc.left.var.iv);
        printf("right   value  %d\n",inc.right.var.iv);
        printf("before finish  %d %d\n",inc.lfin,inc.rfin);

        printf("pretype  is  %d  <---->  sufftype  is  %d\n",inc.pretype,inc.sufftype);
        //判断用户变量
        if (inc.left.type == VE_USER) {
            //判断是否为用户变量
            if ( global.proto->lv.find(tleft.var.chv) == global.proto->lv.end() ) {
                //错误处理
                printf("userdata not define \n");
                tleft.type = VE_VOID;
            }
            else{
                tleft = global.proto->lv[tleft.var.chv]; 
            }
        }
        if (inc.right.type == VE_USER) {
            if ( global.proto->lv.find(tright.var.chv) == global.proto->lv.end() ) {
                //错误处理
                printf("userdata not define \n");
                tright = 0;//清零
            }
            else{
                tright = global.proto->lv[tright.var.chv];
            }
        }

        //如果为负数
        if (inc.lfin){
            if (OC_MINUS == inc.pretype){
                tleft = -tleft;
            }
        }
        if (inc.rfin){
            if (OC_MINUS == inc.sufftype){
                tright = -tright;
            }
        } 

        //解析指令
        switch (inc.type)
        {
        case OC_ASSIGN:{
            if (inc.rfin){
                global.proto->lv[tleft.var.chv] = tright;
            }
            else{
                Instruction& tmp = global.codes[inc.rpos];
                global.proto->lv[tleft.var.chv] = OC_MINUS == inc.sufftype ? -tmp.left.var.iv : tmp.left.var.iv;
            }
            //如果为空则表示数据有问题，并删除
            if (VE_VOID == tleft.type) {
                global.proto->lv.erase(tleft.var.chv);
            }
            else {
                printf("variable  - < > - type is  %d\n",tright.type);
            }
            break;
        }
        case OC_ADD:
        case OC_SUB:
        {
            //计算右边数据
            if (!inc.rfin){
                Instruction& rtmp = global.codes[inc.rpos];
                if (rtmp.lfin) {
                    tright = OC_MINUS ==  inc.sufftype ? -rtmp.left : rtmp.left;
                }else{
                    //错误
                }
                inc.rfin = true;
            }else{
                //错误
            }
            //判断左子树是否有子树
            if (FIN_END == inc.lpos){
                if ( (inc.type == OC_ADD && inc.ltype != OC_SUB) ||
                        (inc.type == OC_SUB && inc.ltype == OC_SUB) ) {
                    tleft = tleft.var.iv + tright.var.iv;
                }else{
                    tleft = tleft.var.iv - tright.var.iv;
                }
            }else{
                //左边有数据未计算完成
                Instruction& ltmp = global.codes[inc.lpos];
                if (ltmp.lfin) {
                    tleft = OC_MINUS == inc.pretype  ? -ltmp.left : ltmp.left;
                    if (inc.rfin) {
                        //减法运算
                        if ( (inc.type == OC_ADD && inc.ltype != OC_SUB) ||
                        (inc.type == OC_SUB && inc.ltype == OC_SUB) ) {
                            tleft = tleft.var.iv + tright.var.iv;
                        }
                        else {
                            tleft = tleft.var.iv - tright.var.iv;
                        }

                    }else{
                        //错误
                    }
                }else{
                    //错误
                }
            }
            inc.lfin = true;    //计算完成

            if (OC_MINUS == inc.sufftype){
                inc.pretype = OC_MINUS;
            }
            break;
        }
        case OC_MUL:{
            if (!inc.rfin) {
                Instruction& rtmp = global.codes[inc.rpos];
                if (rtmp.lfin) {
                    tright = OC_MINUS ==  inc.sufftype ? -rtmp.left : rtmp.left;
                }else{
                    //错误
                }
                inc.rfin = true;
            }else {
                //错误
            }     
            if (FIN_END == inc.lpos) {
                tleft = tleft.var.iv * tright.var.iv;
            }
            else {
                //左边有数据未计算完成
                Instruction& ltmp = global.codes[inc.lpos];
                if (ltmp.lfin) {
                    tleft = OC_MINUS == inc.pretype ? -ltmp.left : ltmp.left;
                    if (inc.rfin) {
                        tleft = tleft.var.iv * tright.var.iv;
                    }else{
                        //错误
                    }
                }else{
                    //错误
                }
            }
            inc.lfin = true;    //计算完成
            break;
        }
        case OC_MOD:{
            if (!inc.rfin) {
                Instruction& rtmp = global.codes[inc.rpos];
                if (rtmp.lfin) {
                    tright = OC_MINUS ==  inc.sufftype ? -rtmp.left : rtmp.left;
                }else{
                    //错误
                }
                inc.rfin = true;
            }else {
                //错误
            }  

            if (FIN_END == inc.lpos) {
                tleft = tleft.var.iv % tright.var.iv;
            }
            else
            {
                Instruction& ltmp = global.codes[inc.lpos];
                if (ltmp.lfin) {
                    tleft = ltmp.left;
                    if (inc.rfin) {
                        tleft = tleft.var.iv % tright.var.iv;
                    }else{
                        //错误
                    }
                }else{
                    //错误
                }
            }
            inc.lfin = true;    //计算完成
            break;
        }
        case OC_IDIV:{
            if (!inc.rfin) {
                Instruction& rtmp = global.codes[inc.rpos];
                if (rtmp.lfin) {
                    tright = OC_MINUS ==  inc.sufftype ? -rtmp.left : rtmp.left;
                }else{
                    //错误
                }
                inc.rfin = true;
            }else {
                //错误
            }  

            if (FIN_END == inc.lpos) {
                tleft = tleft.var.iv / tright.var.iv;
            }
            else
            {
                //左边有数据未计算完成
                Instruction& ltmp = global.codes[inc.lpos];
                if (ltmp.lfin) {
                    tleft = ltmp.left;
                    if (inc.rfin) {
                        tleft = tleft.var.iv / tright.var.iv;
                    }else{
                        //错误
                    }
                }else{
                    //错误
                }
            }
            inc.lfin = true;    //计算完成
            break;
        }
        default:
            printf("code parse error \n");
            // return;
            break;
        }
                
        printf("after  finish  %d %d\n",inc.lfin,inc.rfin);
        printf("result is -----> %d\n",tleft.var.iv);
        printf("---------------------------------------------------------\n");
    }
    
    code_pos = clen;
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