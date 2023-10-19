#include "qd_main.h"

_QD_BEGIN

inline const char* const helpings  = 
    "usage: %s [options] [script [args]]\n"
    "Available options are:\n"
    "  -h help         viewing help  documents\n"
    "  -i interactive  enter interactive mode after executing 'script'\n"
;

inline const char* const helpsargs = 
    "<-------------------------------------------------->\n"
    "Available  args are:\n"
    "  help   view  args helping  documents \n"
    "  codes  show  the  number  of  codes \n"
    "  args   list  the  every single  arg  info\n"
    "<-------------------------------------------------->\n"
;

int QDMAIN::qd_main(int argc, char **argv){
    // std::cout << argc << std::endl;
    
    // 解析参数
    for (int i = 1 ;i < argc ; i ++ ) {
        if ( !strcmp(argv[i],"-h")) {
            //如果是帮助参数直接退出
            std::cout << helpings << std::endl;
            break;
        }
        else if ( !strcmp(argv[i],"-i") ) {
            interactive_mode();
        }
        else {
            //文件
            parser.read_file(argv[i]);
        }
    }

}

void QDMAIN::interactive_mode(){
    std::cout << " Welcome to QD 1.0 !!!" << std::endl;
//交互模式
    std::string line;

    for(;;){
        std::cout << ">>> ";
        std::getline(std::cin,line);

        if (line == "help") {
            std::cout << helpsargs << std::endl;
        }
        else if (line == "codes") {
            std::cout << parser.bytes_code_line() << std::endl;
        }
        else if (line == "args") {
            parser.print_variables();
        }
        else if (line == "curline") {
            std::cout << parser.source_code_line() << std::endl;
        }
        else if ( parser.global.proto->lv.find(line) != parser.global.proto->lv.end() ) {
            parser.print_variable(line);
        }
        else {
            line.push_back('\n');
            parser.read_line(line.c_str());
            //判断是否有变量接收
            // if ( OC_ASSIGN != parser.global.codes.back().type ) {
            //     std::cout << parser.global.codes.back().left.var.iv << std::endl;
            // }
        }
    }
}

_QD_END