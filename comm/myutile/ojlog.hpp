#pragma once
#include "ojnocopy.hpp"
#include <iostream>
#include <string>
#include "ojtimestamp.hpp"

namespace mylog_ns {

// 日志级别枚举
enum MyLogLevels
{
    MYINFO,  // 普通信息  
    MYDEBUG, // 调试信息
    MYERROR, // 错误信息
    MYFATAL, // 致命信息
};

// 日志类
class MyLogger : public OJnocopy
{
public:
    static MyLogger& GetInstance()
    {
        static MyLogger log;
        return log;
    }

    ~MyLogger() {}

    // 设置日志级别
    void SetLevel(int level)
    {
        logLevel_ = level;
    }

    // 写日志
    void WriteLog(const std::string& message)
    {
        switch (logLevel_)
        {
        case MYINFO:
            std::cout << "[INFO] ";
            break;
        case MYERROR:
            std::cout << "[ERROR] ";
            break;
        case MYDEBUG:
            std::cout << "[DEBUG] ";
            break;
        case MYFATAL:
            std::cout << "[FATAL] ";
            break;
        default:
            break;
        }
        std::string time = OJTimestamp::now().TimestamptoString();
        std::cout << "[" << time.c_str() << "]:" << message.c_str() << std::endl;
    }

private:
    MyLogger() : logLevel_(MYINFO) {}

private:
    int logLevel_;
};

} // namespace mylog_ns

// 宏定义：日志宏，方便调用
#define MYLOG_INFO(Logmsgformat, ...) \
    do { \
        mylog_ns::MyLogger& logger = mylog_ns::MyLogger::GetInstance(); \
        logger.SetLevel(mylog_ns::MYINFO); \
        char buf[1024]; \
        snprintf(buf, 1024, "[%s:%d] " Logmsgformat, __FILE__, __LINE__, ##__VA_ARGS__); \
        logger.WriteLog(buf); \
    } while (0);

#define MYLOG_FATAL(Logmsgformat, ...) \
    do { \
        mylog_ns::MyLogger& logger = mylog_ns::MyLogger::GetInstance(); \
        logger.SetLevel(mylog_ns::MYFATAL); \
        char buf[1024]; \
        snprintf(buf, 1024, "[%s:%d] " Logmsgformat, __FILE__, __LINE__, ##__VA_ARGS__); \
        logger.WriteLog(buf); \
        exit(1); \
    } while (0);

#ifdef DEBUGMOD
#define MYLOG_DEBUG(Logmsgformat, ...) \
    do { \
        mylog_ns::MyLogger& logger = mylog_ns::MyLogger::GetInstance(); \
        logger.SetLevel(mylog_ns::MYDEBUG); \
        char buf[1024]; \
        snprintf(buf, 1024, "[%s:%d] " Logmsgformat, __FILE__, __LINE__, ##__VA_ARGS__); \
        logger.WriteLog(buf); \
    } while (0);
#else
#define MYLOG_DEBUG(Logmsgformat, ...)
#endif

#define MYLOG_ERROR(Logmsgformat, ...) \
    do { \
        mylog_ns::MyLogger& logger = mylog_ns::MyLogger::GetInstance(); \
        logger.SetLevel(mylog_ns::MYERROR); \
        char buf[1024]; \
        snprintf(buf, 1024, "[%s:%d] " Logmsgformat, __FILE__, __LINE__, ##__VA_ARGS__); \
        logger.WriteLog(buf); \
    } while (0);
