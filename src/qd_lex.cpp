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
    
}

void LexState::init(){
    logger = Logger::getInstance();

    this->cur = 0;
    
    this->_row = 1;
    this->_col = 0;
    this->_end = 0;

    this->lastline = 0;
    

    io = nullptr;
    // io = new Dio();
}

void LexState::next(){
    //查找下一个字符 
    this->io->offset_buff(1);
    this->cur = this->io->get_ch();
    
    _col ++ ;
}

void LexState::prev(){
    this->io->offset_buff(-1);
    this->cur = this->io->get_ch();
    _col --;
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
        // printf("end \n");
        this->next();

        return T_END;
    }
    case ':':{
        // printf(": ");
        this->next();
        return T_COLON;
    }
    /* line breaks */
    case '\n': case '\r':{
        // printf("line \n");

        if(this->remove_line()) {
            return T_ERR;
        }

        this->_end = this->_col - 1;
        _col = 0;
        ++_row;
        // inclinenumber();
        return T_END;
    }
     /* spaces */
    case ' ': case '\f': case '\t': case '\v':{
        // printf("blank ");
        this->remove_blank();
        return T_BLANK;
    }  
    //注释
    case '#':{
        if(read_comment()){
            return T_ERR;
        }
        ++_row;
        _col = 0;
        // printf("comment \n");
        return T_COMMENT;
    }
    case ',':{
        // printf(", ");
        this->next();
        return T_COMMA;
    }
    case '"': case '\'':{
        // printf("string ");
        return  read_string(this->cur);
    }
    case '.': {  /* '.', '..', '...', or number */
        this->next();
        // printf(". ");
        //暂时不需要
        return T_PERIOD;
    }
    case '=': {
        this->next();
        if (check1_next('=')) {
            // printf("== ");
            return T_DEQ;  /* '==' */
        }
        else {
            // printf("= ");
            return T_EQ;
        }
    }
    case '>':{
        this->next();
        if (check1_next('=')) {
            // printf(">= ");
            return T_GE;  /* '>=' */
        }
        else {
            // printf("> ");
            return T_GN;
        }
    }
    case '<':{
        this->next();
        if (check1_next('=')) {
            // printf("<= ");
            return T_LE;  /* '<=' */
        }
        else {
            // printf("< ");
            return T_LN;
        }
    }
    case '!':{
        this->next();
        if (check1_next('=')) {
            // printf(" != ");
            return T_NEQ;  /* '!=' */
        }
        else {
            // printf(" ! ");
            return T_EXCLAMATION;
        }
    }
    case '+':{
        this->next();
        // printf("+ ");
        return T_PLUS;
    }
    case '-':{
        this->next();
        // printf("- ");
        return T_MINUS;
    }
    case '*':{
        this->next();
        // printf("* ");
        return T_MUL;
    }
    case '%':{
        this->next();
        // printf("% ");
        return T_MOD;
    }
    case '/':{
        this->next();
        // printf("/ ");
        return T_DIV;
    }
    case '(':{
        // printf(" ( ");
        this->next();
        return T_LPARENTH;        
    }
    case ')':{
        this->next();
        // printf(" ) ");
        return T_RPARENTH;
    }
    case '&':{
        // printf("& ");
        this->next();
        return T_AMPERSAND;
    }
    case '|':{
        // printf("| ");
        this->next();
        return T_VERTICAL_BAR;
    }
    case '{':{
        // printf("{ ");
        this->next();
        return T_LBRACE;
    }
    case '}':{
        // printf("} ");
        this->next();
        return T_RBRACE;
    }
    case '[':{
        // printf("[ ");
        this->next();
        return T_LBRACKET;
    }
    case ']':{
        // printf("] ");
        this->next();
        return T_RBRACKET;
    }
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
    {
        read_numeral();
        if ( '.' == this->cur) {
            this->next();
            //判断错误类型
            if ( !isalnum(this->cur) ) {
                logger->error("period back is not number");
                return T_ERR;
            }
            double tmp = (double)dvar.var.iv;
            read_decimal();//读取小数部分            
            tmp += dvar.var.dv;
            dvar = tmp;
            // printf("decimal ");
            return T_DECIMAL;
        }
        // printf("int ");
        return T_INT;
    }
    default:{
    //解析字母
        if (this->islalpha(this->cur) || '_' == this->cur){
            std::string data;
            do{
                data.push_back(this->cur);
                next();
            }while( this->islalpha(this->cur) || this->isdigit(this->cur) || '_' == this->cur );
            //判断关键字
            int keyw = is_keyword(data.c_str());
            if ( -1 !=  keyw ) {
                // printf("keyword ");
                // dvar = data.c_str();
                if ( T_TRUE == keyw ) {
                    dvar = true;
                }else if ( T_FALSE == keyw ){
                    dvar = false;
                }
                return keyw;
            } else {
                dvar = data.c_str();//用户变量
                // printf("user_data ");
                dvar.type = VE_USER;
                return T_UDATA;
            }
        } 
        return T_ERR;
    }
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

bool LexState::isdigit(size_t c){
    //'0' - '9'
    if (c >= '0' && c <= '9'){
        return true;
    }
    return false;    
}

bool LexState::islalpha(size_t a){
    // 'a' -'z'
    if ( (a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z') ) {
        return true;
    }
    return false;
}

bool LexState::isblank(size_t b){
    if ( b == ' ' || b == '\f' || b == '\t' || b == '\v' ) {
        return true;
    }
    return false;
}

bool LexState::isline(size_t l){
    if ( '\n' == l || '\r' == l) {
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

int LexState::read_string(size_t c){
    std::string tmp;//暂时
    //"
    this->next();
    while (this->cur != c) {
        switch (this->cur) {
            case '\0':
                logger->error("errror");
                return T_ERR;
            case '\n':case '\r':
                logger->error("string error");
                return T_ERR;
            case '\\'://转义字符
                // ++_row;
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
                    inclinenumber();
                    //^M  CRLF
                    // if ( '\^\M' == this->cur ) this->prev();
                    if ( QD_CRLF == this->cur ) this->prev();
                    //LF
                    this->prev();
                    break;
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

bool LexState::read_comment(){
    while ( !isline(this->cur) )
    {
        this->next();
    }

    return remove_line();
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
    while (isdigit(this->cur))
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

bool LexState::remove_line() {
    unsigned char old = this->cur;
    this->next();
    if ( isnewline() && this->cur != old ) {
        this->next();
    }

    //如果行数超出最大值，进行报错
    if ( this->_row > QD_SIZE_MAX ) {
        logger->error("codes has too many lines");
        return true;
    }
    
    return false;
}

bool LexState::is_keyw(size_t tok) {
    if ( tok >= T_IF && tok <= T_IN ) {
        return true;
    }
    return false;
}

bool LexState::is_operator(size_t tok){
    if ( tok >= T_EQ && tok <= T_VERTICAL_BAR ){
        return true;
    }
    return false;
}

bool LexState::is_variable(size_t tok){
    if ( (tok >= T_INT && tok <= T_UDATA ) || T_TRUE == tok || T_FALSE == tok ){
        return true;
    }
    return false;
}

/** =======================================================*/


void LexState::alloc_buff(const char* buff,size_t len){
    this->io->alloc_buff(buff,len);
    this->cur = this->io->get_ch();
}

void LexState::alloc_io(Dio* const io) {
    this->io = io;
}


_QD_END