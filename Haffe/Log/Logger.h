/*
 * @Description: LogEvent Logger LogEventWrap
 * @Author: hahagiraffe
 * @Date: 2019-08-21 22:48:20
 */

#ifndef HAFFE_LOGGER_H
#define HAFFE_LOGGER_H

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <iostream>
#include <stdarg.h>
#include <map>
#include "../base/Util.h"
#include "../base/Thread.h"
#include "../base/Thread.h"
#include "LogAppender.h"

namespace haffe{

#define LOG_LEVEL(logger,level) \
    if(logger->getlevel() <= level) \
     LogEventWrap(std::shared_ptr<LogEvent> (new haffe::LogEvent(logger,level,__FILE__,__LINE__, \
     0,haffe::GetThreadID(),haffe::GetCoroutineID(),time(0),haffe::Thread::GetName()))).getSS()

#define Haffe_LOG_DEBUG(logger) LOG_LEVEL(logger,haffe::Loglevel::LOG_DEBUG)
#define Haffe_LOG_INFO(logger) LOG_LEVEL(logger,LOG_INFO)
#define Haffe_LOG_WARN(logger) LOG_LEVEL(logger,LOG_WARN)
#define Haffe_LOG_ERROR(logger) LOG_LEVEL(logger,LOG_ERROR)
#define Haffe_LOG_FATAL(logger) LOG_LEVEL(logger,LOG_FATAL)

#define Haffe_LOG_ROOT haffe::LoggerMgr::GetInstance()->getroot()

class LogAppender; //这里提前声明
class Logger;
class LogFormatter;

//LOG类型

std::string LevelToString(Loglevel level);
Loglevel StringToLevel(std::string);

//表示log的内容
class LogEvent{
public:
    LogEvent(std::shared_ptr<Logger> logger,Loglevel level,const char* filename,int32_t line,uint32_t elapse,uint32_t threadid,
    uint32_t coroutineid, uint64_t time,const std::string& threadname):
        filename_(filename),
        line_(line),
        elapse_(elapse),
        time_(time),
        pthreadid_(threadid),
        coroutineid_(coroutineid),
        pthreadname_(threadname),
        level_(level),
        logger_(logger)
    { }

    const char* getfilename() const{
        return filename_;
    }
    int32_t getline() const{
        return line_;
    }
    uint32_t getelapse() const{
        return elapse_;
    }
    uint32_t gettime() const{
        return time_;
    }
    uint32_t getpthreadid() const{
        return pthreadid_;
    }
    uint32_t getcoroutineid() const{
        return coroutineid_;
    }
    std::string getpthreadname() const {
        return pthreadname_;
    }
    std::stringstream& getss() {
        return ss_;
    }
    std::string getcontent() const {
        return ss_.str();
    }
    std::shared_ptr<Logger> getlogger() const{
        return  logger_;
    }
    Loglevel  getlevel() const{
        return level_;
    }
    void format(const char* format,...);
    void format(const char* format,va_list al);
private:
    const char* filename_; //文件名
    int32_t line_; //行号
    uint32_t elapse_; //从启动到现在耗时
    uint32_t time_; //当前时间
    uint32_t pthreadid_; //线程id
    uint32_t coroutineid_; //协程id
    std::string pthreadname_;//线程名称
    std::stringstream ss_;//日志内容流
    std::shared_ptr<Logger> logger_;//日志器指针
    Loglevel level_;//日志等级
};


//日志类
class Logger : public std::enable_shared_from_this<Logger>{
public:
    explicit Logger(const std::string& name="root");
    void log(Loglevel level,std::shared_ptr<LogEvent> event);

    inline void debug(std::shared_ptr<LogEvent> event){
        log(Loglevel::LOG_DEBUG,event);
    }
    inline void info(std::shared_ptr<LogEvent> event){
        log(Loglevel::LOG_INFO,event);
    }
    inline void warn(std::shared_ptr<LogEvent> event){
        log(Loglevel::LOG_WARN,event);
    }
    inline void error(std::shared_ptr<LogEvent> event){
        log(Loglevel::LOG_ERROR,event);
    }
    inline void fatal(std::shared_ptr<LogEvent> event){
        log(Loglevel::LOG_FATAL,event);
    }

    void addAppender(std::shared_ptr<LogAppender> appender);
    void delAppender(std::shared_ptr<LogAppender> appender);
    void clearAppender();

    void setname(const std::string& name){
        name_=name;
    }
    const std::string& getname(){
        return name_;
    }

    void setFormatter(std::shared_ptr<LogFormatter> formatter){
        Mutex::Lock lock(mutex_);
        formatter_=formatter;
    }
    std::shared_ptr<LogFormatter> getFormatter(){
        Mutex::Lock lock(mutex_);
        return formatter_;
    }
    inline void setlevel(Loglevel level){
        level_=level;
    }
    inline Loglevel  getlevel() const{
        return level_;
    }

    void setroot(std::shared_ptr<Logger> logger){
        logger_=logger;
    }

private:
    std::string name_; //名称
    Loglevel level_; //级别
    Mutex mutex_;//互斥量
    std::list<std::shared_ptr<LogAppender>> appenders_; //输出地点的集合
    std::shared_ptr<LogFormatter> formatter_;
    std::shared_ptr<Logger> logger_;
};

//日志包装类，主要是让日志使用更加方便
class LogEventWrap{
public:
    explicit LogEventWrap(std::shared_ptr<LogEvent> event):
            event_(event)
    {
    }
    ~LogEventWrap(){
        //std::cout<<"into ~LogEventWrap"<<std::endl;
        event_->getlogger()->log(event_->getlevel(),event_);
        //std::cout<<"outof ~LogEventWrap"<<std::endl;
    }
    std::stringstream& getSS() const {
        return event_->getss();
    }
    std::shared_ptr<LogEvent> getevent(){
        return event_;
    }
private:
    std::shared_ptr<LogEvent> event_;
};

class LogManager{
public:
    LogManager();
    std::shared_ptr<Logger> getroot() { return root_;}
    std::shared_ptr<Logger> getLoggers(const std::string& name);
 
private:
    Mutex mutex_;
    std::map<std::string,std::shared_ptr<Logger>> loggers_;
    std::shared_ptr<Logger> root_;
};

typedef Singleton<LogManager> LoggerMgr;

}
#endif //HAFFE_LOGGER_H
