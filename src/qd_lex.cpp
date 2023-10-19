#include "qd_lex.h"

_QD_BEGIN

Token::Token(){
    init();
}

void Token::init(){
    this->token = T_NULL;
}

LexState::LexState(){
    init();
}

LexState::~LexState(){
    // if (io != NULL) {
    //     delete io;
    // }
    if ( buff != nullptr){
        delete buff;
    }
}

void LexState::init(){
    this->cur = 0;
    this->line_number = 0;
    this->lastline = 0;

    // t.init();
    // lookahead.init();

    buff = nullptr;
    buff = new Dbuffer();
    buff->init();

}

void LexState::next(){
    //查找下一个字符 
    this->buff->offset_buff(1);
    this->cur = this->buff->get_ch();
}
void LexState::prev(){
    this->buff->offset_buff(-1);
    this->cur = this->buff->get_ch();
}


/*
** =======================================================
** 词法解析
** =======================================================
*/

int LexState::llex(){
    switch (this->cur)
    {
    case '\0':
        return T_EOF;//文件结束会打印两次？？
    case ';':{
        printf("end \n");
        this->remove_line();
        return T_END;
    }
    /* line breaks */
    case '\n': case '\r':{
        printf("line \n");
        this->remove_line();
        ++line_number;
        return T_END;
    }
     /* spaces */
    case ' ': case '\f': case '\t': case '\v':{
        printf("blank ");
        this->remove_blank();
        return T_BLANK;
    }  
    //注释
    case '#':{
        read_comment();
        printf("comment \n");
        return T_COMMENT;
    }
    case '"': case '\'':{
        printf("string ");
        return  read_string(this->cur);
    }
    case '.': {  /* '.', '..', '...', or number */
        this->next();
        if (T_INT == lookahead.token){
            double tmp = (double)dvar.var.iv;
            read_decimal();//读取小数部分            
            tmp += dvar.var.dv;
            dvar = tmp;
            printf("decimal ");
            return T_DECIMAL;
        }else{
            //错误处理
            return T_NULL;
        }
    }
    case '=': {
        this->next();
        if (check1_next('=')) return T_DEQ;  /* '==' */
        else {
            printf("equal ");
            return T_EQ;
        }
    }
    case '+':{
        this->next();
        printf("plus ");
        return T_PLUS;
    }
    case '-':{
        this->next();
        printf("minus ");
        return T_MINUS;
    }
    case '*':{
        this->next();
        printf("multiply ");
        return T_MUL;
    }
    case '%':{
        this->next();
        printf("mod  ");
        return T_MOD;
    }
    case '/':{
        this->next();
        if (check1_next('/')) {
            printf("divide ");
            return T_DDIV;  /* '//' */
        } else {
            printf("dobule divide ");
            return T_DIV;
        }
    }
    case '(':{
        //这里可能稍后还会进行更改
        int pnum = 0;
        //多个(嵌套
        for(;;) {
            pnum++;
            this->next();
            this->remove_blank();
            if (this->cur != '(') break;
        }
        
        // if (this->cur == '-') this->next();//如果有负数
        bool neg = false; 
        if ( '-' == this->cur ) {
            this->next();
            this->remove_blank();
            neg = true;
        }

        if ( isalnum(this->cur) ) {
            //还有一个问题就是小数没有进行判断是个麻烦
            int num = read_numeral();
            this->remove_blank();
            if (neg) dvar = -dvar.var.iv;
            if ( this->cur == ')') {
                printf("int ");
                //去除对应数量的(
                for (;;) {
                    pnum--;
                    this->next();
                    this->remove_blank();
                    if (this->cur != ')') break; 
                }
                
                return pnum == 0 ? num : T_ERR;
            }
        }
        //如果是正常表达式 ( 1 + 2 )
        //如果不是单个整型 
        this->prev();
        this->prev_blank();
        this->prev_number();
        this->prev_blank();
        if (this->cur == '-') this->prev(),this->prev_blank();//如果有负数

        while(--pnum) {
            this->prev_blank();
            this->prev();
        }
        printf(" ( ");
        
        this->next();
        return T_LPARENTH;        
    }
    case ')':{
        this->next();
        printf(" ) ");
        return T_RPARENTH;
    }
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
    {
        printf("int ");
        return read_numeral();
    }
    default:
        //解析字母
        if (this->islalpha(this->cur)){
            std::string data;
            do{
                data.push_back(this->cur);
                next();
            }while( this->islalpha(this->cur) || this->isdigit(this->cur));
            //判断关键字
            int keyw = is_keyword(data.c_str());
            if ( -1 !=  keyw ) {
                printf("keyword ");
                // dvar = data.c_str();
                if ( T_TRUE == keyw ) {
                    dvar = true;
                }else if ( T_FALSE == keyw ){
                    dvar = false;
                }
                return keyw;
            } else {
                dvar = data.c_str();//用户变量
                printf("user_data ");
                dvar.type = VE_USER;
                return T_UDATA;
            }
        } 
        return T_ERR;
    }
}

bool LexState::check1_next(int c)
{
    if (this->cur == c) {
        this->next();
        return true;
    }
    else return false;
}

bool LexState::check2_next (const char* two){
    //判断第二个字符
    if (two[2] == '\0'){
        //错误处理
        return false;
    }
    if (this->cur == two[0]){
        this->next();
        if (this->cur == two[1]) return true;
        else this->prev();
    }
    return false;
}

bool LexState::isdigit(unsigned int c){
    //'0' - '9'
    if (c >= '0' && c <= '9'){
        return true;
    }
    return false;    
}

bool LexState::islalpha(unsigned int a){
    // 'a' -'z'
    if ( (a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z') ) {
        return true;
    }
    return false;
}

bool LexState::isblank(unsigned int b){
    if ( b == ' ' || b == '\f' || b == '\t' || b == '\v' ) {
        return true;
    }
    return false;
}

bool LexState::isline(unsigned int l){
    if ( '\n' == l || '\r' == l ) {
        return true;
    }
    return false;
}

int LexState::read_numeral(){
    //第一个数字
    std::string tmp;
    int first = this->cur;

    for(;;){
        if (this->isdigit(this->cur) ){
            tmp.push_back(this->cur);
            this->next();
        }
        else break;
    }
    dvar = atoi(tmp.c_str());//这里式字符还需要转换
    dvar.type = VE_INT;
    return T_INT;
}

void LexState::read_decimal(){
    //第一个数字
    std::string tmp = "0.";
    for(;;){
        if (this->isdigit(this->cur)){
            tmp.push_back(this->cur);
            this->next();
        }
        else break;
    }
    dvar = atof(tmp.c_str());//这里式字符还需要转换
    dvar.type = VE_FLT;
}

int LexState::read_string(unsigned int c){
    std::string tmp;//暂时
    //"
    this->next();
    while (this->cur != c) {
        switch (this->cur) {
            case '\0':
                printf("errror");
                return T_ERR;
            case '\n':case '\r':
                printf("string error");
                return T_STRING;
            case '\\'://转义字符
                this->next();
                int ch;
                switch (this->cur)
                {
                case 'a': ch = '\a'; goto push_ch;
                case 'b': ch = '\b'; goto push_ch;
                case 'f': ch = '\f'; goto push_ch;
                case 't': ch = '\t'; goto push_ch;
                case 'n': ch = '\n'; goto push_ch;
                case 'r': ch = '\r'; goto push_ch;
                case 'v': ch = '\v'; goto push_ch;
                case '\n': case '\r':
                    //这里要往前移一下
                    inclinenumber(); this->prev(); break;
                default:
                    ch = this->cur;
                push_ch:
                    tmp.push_back(ch);
                    break;
                }
                this->next();
                break;
            default:
                tmp.push_back(this->cur);
                this->next();
                break;
        }
    }
    //"
    this->next();
    dvar = tmp.c_str();
    return  T_STRING;
}

void LexState::read_comment(){
    while ( !isline(this->cur) )
    {
        this->next();
    }
    //保险
    remove_line();
}

/*
** increment line number and skips newline sequence (any of
** \n, \r, \n\r, or \r\n)
*/
void LexState::inclinenumber(){
    int old = this->cur;
    if ( isnewline() ){
        this->next();
        // \n\n 或 \r\r不算
        if ( isnewline() && this->cur != old ) {
            this->next();
        }
    }
}

bool LexState::isnewline(){
    return this->cur == '\r' || this->cur == '\n';
}

void LexState::prev_number(){
    while (isalnum(this->cur))
    {
        this->prev();
    }
}

void LexState::prev_blank(){
    while (isblank(this->cur))
    {
        this->prev();
    }
}

void LexState::remove_blank(){
    while (isblank(this->cur))
    {
        this->next();
    }
}

void LexState::remove_line(){
    while ( ';' == this->cur || isnewline() )
    {
        this->next();
    }
}

/** =======================================================*/


void LexState::alloc_buff(const char* buff){
    this->buff->alloc_buff(buff);
    this->cur = this->buff->get_ch();
}

void LexState::free_buff(){
    this->buff->free_buff();
    this->cur = 0;
}


_QD_END