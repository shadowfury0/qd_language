#include "qd_log.h"

_QD_BEGIN

Logger* Logger::logger = nullptr;
// Logger::Logger():lid(0),llevel(0){

// }

// Logger::~Logger(){

// }

Logger* Logger::getInstance(){
    if (logger == nullptr) {
        logger = new Logger();
        logger->llevel = 1;
        logger->lid = 0;
        logger->pattern = "[ %Y-%m-%d %X ] ";
        logger->timeup = true;
        logger->info( " Logger init ");
    }
    return logger;
}
void Logger::release(){
    if (logger != nullptr) {
        logger->info(" Logger release ");
        delete logger;
        logger = nullptr;
    } 
}

void Logger::setLogLevel(unsigned char l){
    this->llevel = l;
}

void Logger::setLogTime(bool b){
    this->timeup = b;
}

void Logger::setLogPattern(const char* pattern){
    this->pattern = pattern;
}

void Logger::fprint_curtime(){
    if (!this->timeup){
        return;
    }
    curtime = std::chrono::system_clock::now();
    time_t in_time_t = std::chrono::system_clock::to_time_t(curtime);
    std::cout << std::put_time(std::localtime(&in_time_t), this->pattern.c_str()) ;
}


_QD_END