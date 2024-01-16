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
        logger->idup = true;
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

void Logger::setLogId(bool b) {
    this->idup = b;
}

void Logger::fprint_curtime(){
    if (!this->timeup){
        return;
    }
    curtime = std::chrono::system_clock::now();
    time_t in_time_t = std::chrono::system_clock::to_time_t(curtime);
#if defined(__GNUC__) && (__GNUC__ > 5)
    std::cout << std::put_time(std::localtime(&in_time_t), this->pattern.c_str());
#endif
}

void Logger::fprint_id() {
    if (!this->idup){
        return;
    }
    std::cout << " <id " << this->lid << "> ";
}



_QD_END