#include "qd_main.h"
#include "qd_version.h"

_QD_BEGIN

const char* const helpings  = 
    "usage: %s [options] [script [args]]\n"
    "Available options are:\n"
    "  -v  version       show version information\n"
    "  -h  help          viewing  help  documents\n"
    "  -i  interactive   enter  interactive mode after executing 'script'\n"
;

const char* const helpsargs = 
    "<-------------------------------------------------->\n"
    "Available  args are:\n"
    "  quit     quit  the  interactive  mode \n"
    "  help     view  args helping  documents \n"
    "  codes    show  the  number  of  bytes  codes \n"
    "  args     list  the  every  single  variable  info\n"
    "<-------------------------------------------------->\n"
;

#define has_error	0	/* bad option */
#define has_v	    1	/* bad option */
#define has_i		2	/* -i */
#define has_        3   /* max */

QDMAIN::QDMAIN(){
    logger = Logger::getInstance();

    parser = nullptr;
    parser = new DParser();

    vm = nullptr;
    vm = new D_VM();

    io = nullptr;
    io = new Dio();

    this->b.new_bitset(has_);
    parser->init_io(this->io);
}

QDMAIN::~QDMAIN(){

    if ( parser != nullptr ) {
        delete parser;
        parser = nullptr;
    }

    if ( vm != nullptr ) {
        delete vm;
        vm = nullptr;
    }

    if ( io != nullptr ) {
        delete io;
        io = nullptr;
    }

    logger->release();
}

void QDMAIN::parse_args(char* str) {
    bool handle = false;
    if ('-' == str[0]) {
        handle = true;        
    }
    if (!handle) {
        this->parser->read_file(str);
        return;
    }
    switch (str[1])
    {
    case 'v':
    {
        if (str[2] != '\0')
            this->b.add_bit(has_error);
        else
            this->b.add_bit(has_v);
        break;
    }
    case 'i':
    {
        if (str[2] != '\0')
            this->b.add_bit(has_error);
        else
            this->b.add_bit(has_i);
        break;
    }
    default:
        this->b.add_bit(has_error);
        break;
    }
}

size_t QDMAIN::qd_main(int argc, char **argv) {
    
#if defined(__GNUC__) && (__GNUC__ < 5)
    logger->warn("g++ version is less than 5.0.0 ");
#endif

    logger->setLogLevel(3);
    logger->setLogId(false);
    logger->setLogTime(false);

    // 解析参数
    size_t i = 0;
    if (argc < 1) {
        return 1;
    }
    else if ( 1 == argc ){
        return 0;
    }

    for ( i = 1 ; i < argc ; i++) {
        parse_args(argv[i]);
    }
    logger->error(this->b);


    if(b.test(has_error)) {
        std::cout << helpings << std::endl;
        return 1;
    }

    if (b.test(has_v)) {
        std::cout << QD_VERSION << std::endl;
    }

    if (b.test(has_i)) {
        this->interactive_mode();
    }
    else {
        this->script_mode();
    }


    logger->setLogLevel(0);

    return 0;
}

size_t QDMAIN::interactive_mode() {
    //提前解析一些文件输入

    std::cout << " Welcome to QD 1.0 !!!" << std::endl;
//交互模式
    std::string line;
    size_t pos = 0;
    // this->vm->init_fun(this->parser->env_stack_top()->cur);
    
    if(this->script_mode()){
        logger->error("interactive mode parser error");
        return ERR_END;
    }

    pos = this->vm->global->f->codes.size();

    for(;;){
        std::cout << ">>> ";
        std::getline(std::cin,line);

        if ( "help" == line  ) {
            std::cout << helpsargs << std::endl;
        }
        else if ( "quit" == line ) {
            break;
        }
        else if ( "codes" == line ) {
            std::cout << pos << std::endl;
        }
        else if ( "args" == line ) {
            this->vm->print_variables(this->vm->global);
        }
        else {
            //这里判断是否为全局变量
            D_OBJ* obj = this->vm->find_variable(line);

            if (obj) {
                //打印变量信息
                std::cout << *obj << std::endl;
            }
            else {
                size_t start = this->parser->io_size();

                if (is_backslash(line)) {
                    this->read_more(line);
                }

                line.push_back('\n');

                this->parser->read_line(line.c_str(),line.size());

                //错误处理
                size_t end = this->parser->io_size();

                if (this->parser->parse()) {
                    for (size_t i = start ; i < end ; i++ ) {
                        this->io->clean_back();
                    }
                    continue;
                }

                this->vm->execute(pos);
                pos = this->vm->global->f->codes.size();
                this->vm->reserve_global();
            }
        
        }
    }
}

size_t QDMAIN::script_mode() {

    if(this->parser->parse()) {
        this->io->clean_back();
        logger->error("script mode parser error");
        return ERR_END;
    }

    //这里把funhead传入给虚拟机
    vm->init_fun(parser->env_stack_top()->cur);
    vm->execute();
    
    // logger->error(parser->env.size()," : ",vm->size_call() );

    return 0;
}

bool QDMAIN::is_backslash(const std::string& s) {
    if (s.back() == '\\') {
        return true;
    }
    return false;
}

size_t QDMAIN::read_more(std::string& str) {
    
    // str.pop_back();
    str.back() = '\n';

    std::string line;
    std::cout << ": ";
    std::getline(std::cin,line);

    while (is_backslash(line))
    {
        line.back() = '\n';
        str.append(line);

        std::cout << ": ";
        std::getline(std::cin,line);
    }
    str.append(line);

    return 0;
}

_QD_END

