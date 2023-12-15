#ifndef __QD_LOG_H__
#define __QD_LOG_H__

#include "qd_header.h"

#include <chrono>
#include <iomanip>
#include <ctime>
#include <time.h>

_QD_BEGIN


class Logger{
public:
    //设置日志级别
    void setLogLevel(unsigned char);
    //设置是否打印时间属性
    void setLogTime(bool b);
    //设置日志日期类型
    void setLogPattern(const char* pattern);
    //日志标记打印
    void setLogId(bool b);
    
    //各种日志级别开始打印
    template<class... T>
    void debug(const T&... t);
    
    template<class... T>
    void info(const T&... t);

    template<class... T>
    void warn(const T&... t);

    template<class... T>
    void error(const T&... t);

    template<class... T>
    void fatal(const T&... t);

    static Logger* getInstance();
    static void release();
private:
    Logger(){}
    ~Logger(){}
    
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    template<class... Args>
    void log(unsigned char level ,const Args&... t);

    //打印当前时间
    void fprint_curtime();
    void fprint_id();

    template<class T>
    void print(const T& t);
    template<class T,class... Args>
    void print(const T& t,const Args&... args);

private:

    enum LOG_LEVEL{
        ALL = 0,
        DEBUG,
        INFO,
        WARN,
        ERR,
        FATAL,
        OFF,//关闭所有
    };
    bool idup;          //打印索引？
    bool timeup;         //打印时间？
    unsigned char llevel;//默认打印全部
    size_t lid;    //第几个日志
    std::string pattern; //格式化类型
    std::chrono::system_clock::time_point curtime;
public:
    static Logger* logger;
};


template<class T>
void Logger::print(const T& t){
    std::cout << t;
}

template<class T,class... Args>
void Logger::print(const T& t,const Args&... args){
    std::cout << t ;
    print(args...);
}

//-----------------LOG TYPE PRINT ---------------------------
template<class... Args>
void Logger::log(unsigned char level ,const Args&... t){
    if (level >= llevel) {
        lid++;
        switch (level)
        {
        case DEBUG:
            std::cout << "(- debug -) " ;
            break;
        case INFO:
            std::cout << "(- info  -) " ;
            break;
        case WARN:
            std::cout << "(- warn  -) " ;
            break;
        case ERR:
            std::cout << "(- err   -) " ;
            break;
        case FATAL:
            std::cout << "(- fatal -) " ;
            break;
        }

        fprint_id();
        fprint_curtime();
        print(t...);
        std::cout << std::endl;
    }
}

template<class... T>
void Logger::debug(const T&... t){
    this->log(LOG_LEVEL::DEBUG,t...);
}

template<class... T>
void Logger::info(const T&... t){
    this->log(LOG_LEVEL::INFO,t...);
}

template<class... T>
void Logger::warn(const T&... t){
    this->log(LOG_LEVEL::WARN,t...);
}

template<class... T>
void Logger::error(const T&... t){
    this->log(LOG_LEVEL::ERR,t...);
}

template<class... T>
void Logger::fatal(const T&... t){
    this->log(LOG_LEVEL::FATAL,t...);
}


_QD_END



#endif