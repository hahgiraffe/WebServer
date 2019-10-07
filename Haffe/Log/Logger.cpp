/*
 * @Description: 
 * @Author: hahagiraffe
 * @Date: 2019-08-23 16:10:56
 */
#include "Logger.h"
#include <time.h>
#include <cstring>
#include <memory>
#include "LogAppender.h"
#include "LogFormatter.h"


namespace haffe{
    std::string LevelToString(Loglevel level){
        std::string res="UNKOWN";
        switch (level){
            case LOG_DEBUG:
                res="DEBUG";
                return res;
            case LOG_INFO:
                res="INFO";
                return res;
            case LOG_WARN:
                res="WARN";
                return res;
            case LOG_ERROR:
                res="ERROR";
                return res;
            case LOG_FATAL:
                res="FATAL";
                return res;
            default:
                return res;
        }
    }
    Loglevel StringToLevel(std::string str){
        if(std::strcmp(str.c_str(),"DEBUG")==0){
            return LOG_DEBUG;
        }
        else if(std::strcmp(str.c_str(),"INFO")==0){
            return LOG_INFO;
        }
        else if(std::strcmp(str.c_str(),"WARN")==0){
            return LOG_WARN;
        }
        else if(std::strcmp(str.c_str(),"ERROR")==0){
            return LOG_ERROR;
        }
        else if(std::strcmp(str.c_str(),"FATAL")==0){
            return LOG_FATAL;
        }
        else{
            return LOG_UNKOWN;
        }
    }

    void LogEvent::format(const char *fmat, ...) {
        va_list al;
        va_start(al, fmat);
        format(fmat, al);
        va_end(al);
    }
    void LogEvent::format(const char *format, va_list al) {
        char* buf = nullptr;
        int len = vasprintf(&buf, format, al);
        if(len != -1) {
            ss_ << std::string(buf, len);
            free(buf);
        }

    }

    Logger::Logger(const std::string& name)
        :name_(name),
        level_(LOG_DEBUG)
    {
        formatter_.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
    }
    void Logger::log(Loglevel level,std::shared_ptr<LogEvent> event){
        if(level>=level_){
            //std::cout<<"into Logger::log";
            auto self=shared_from_this();
            Mutex::Lock lock(mutex_);
            if(!appenders_.empty()){
                for(auto& it : appenders_){
                    it->log(self,level,event);
                }
            }
            else if (logger_){
                logger_->log(level,event);
            }
        }
    }
    void Logger::addAppender(std::shared_ptr<LogAppender> appender){
        Mutex::Lock lock(mutex_);
        if(!appender->getFormatter()){
            appender->formatter_=formatter_;
        }
        appenders_.push_back(appender);
    }
    void Logger::delAppender(std::shared_ptr<LogAppender> appender) {
        Mutex::Lock lock(mutex_);
        for(auto it=appenders_.begin();it!=appenders_.end();it++){
            if(*it == appender){
                appenders_.erase(it);
                break;
            }
        }
    }
    void Logger::clearAppender(){
        appenders_.clear();
    }

    LogManager::LogManager(){
        root_.reset(new Logger);
        root_->addAppender(std::shared_ptr<LogAppender> (new StdoutAppender()));
        //这里可以把root加上FileAppender,这样就可以打印到指定的文件中
        time_t ti;
        struct tm *newtime;
        ti=time(NULL);
        newtime=localtime(&ti);
        char tmpbuf[128];
        strftime( tmpbuf, 128, "%T_%d_%b_%Y", newtime);
        std::string filename="./rootlog_"+std::string(tmpbuf)+".txt";
        root_->addAppender(std::shared_ptr<LogAppender> (new FileAppender(filename)));
        loggers_[root_->getname()]=root_;

    }

    std::shared_ptr<Logger> LogManager::getLoggers(const std::string& name){
        Mutex::Lock lock(mutex_);
        auto it=loggers_.find(name);
        if(it!=loggers_.end()){
            return it->second;
        }

        std::shared_ptr<Logger> logger(new Logger(name));
        logger->setroot(logger);
        loggers_[name]=logger;
        return logger;
    }

}