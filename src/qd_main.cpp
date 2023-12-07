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
    "  quit     quit  the  interactive  mode \n"
    "  help     view  args helping  documents \n"
    "  codes    show  the  number  of  codes \n"
    "  args     list  the  every single  arg  info\n"
    "  curline  current  soruce code  line  \n"
    "<-------------------------------------------------->\n"
;

QDMAIN::QDMAIN(){
    logger = Logger::getInstance();

    parser = nullptr;
    parser = new DParser();

    vm = nullptr;
    vm = new D_VM();
}

QDMAIN::~QDMAIN(){
    logger->release();

    if ( parser != nullptr ) {
        delete parser;
        parser = nullptr;
    }

    if ( vm != nullptr ) {
        delete vm;
        vm = nullptr;
    }
}

int QDMAIN::qd_main(int argc, char **argv){
    
    // 解析参数
    // for (int i = 1 ;i < argc ; i ++ ) {
    //     if ( !strcmp(argv[i],"-h")) {
    //         //如果是帮助参数直接退出
    //         std::cout << helpings << std::endl;
    //         break;
    //     }
    //     else if ( !strcmp(argv[i],"-i") ) {
    //         interactive_mode();
    //     }
    //     else {
            //文件
            // logger->setLogTime(false);
            // logger->setLogPattern("%Y-%M------------");
            logger->setLogPattern("");
            logger->setLogLevel(3);

            parser->read_file(argv[1]);

            //这里把funhead传入给虚拟机
            vm->fun = parser->env_stack_top()->cur;
            vm->execute();

            logger->setLogLevel(0);
        // }
    // }
    return 0;
}

void QDMAIN::interactive_mode(){
//     std::cout << " Welcome to QD 1.0 !!!" << std::endl;
// //交互模式
//     std::string line;

//     for(;;){
//         std::cout << ">>> ";
//         std::getline(std::cin,line);

//         if (line == "help") {
//             std::cout << helpsargs << std::endl;
//         }
//         else if (line == "codes") {
//             std::cout << parser.bytes_code_line() << std::endl;
//         }
//         else if (line == "args") {
//             // parser.print_variables();
//         }
//         else if (line == "curline") {
//             std::cout << parser.source_code_row() << std::endl;
//         }
//         else if (line == "jump"){
//             std::getline(std::cin,line);
//             parser.global.code_pos = atoi(line.c_str());
//             parser.analyse_code(parser.global.code_pos,parser.global);
//         }
//         else if ( parser.global.proto->lv.find(line) != parser.global.proto->lv.end() ) {
//             // parser.print_variable(line);
//         }
//         else if (line == "quit") {
//             break;
//         }
//         //设置日志等级，这里稍后进行更改
//         else if (line == "level"){
//             std::cout << "please input log level " << std::endl;
//             std::getline(std::cin,line);
//             if ( line == "1" ) {
//                 logger->setLogLevel(1);
//             }
//             else if (line == "2"){
//                 logger->setLogLevel(2);
//             }
//             else if (line == "3"){
//                 logger->setLogLevel(3);
//             } 
//             else if (line == "4"){
//                 logger->setLogLevel(4);
//             }
//             else {
//                 std::cout << "nothing happened " << std::endl;
//             }
//         }
//         else {
//             line.push_back('\n');
//             parser.read_line(line.c_str());
//             //判断是否有变量接收
//             // if ( OC_ASSIGN != parser.global.codes.back().type ) {
//             //     std::cout << parser.global.codes.back().left.var.iv << std::endl;
//             // }
//         }
    
//     }
}


_QD_END