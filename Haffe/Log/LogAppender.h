/*
 * @Description: LogAppender日志输出方法（stdio流，文件流等）
 * @Author: hahagiraffe
 * @Date: 2019-08-28 16:26:31
 */
#ifndef HAFFE_LOGAPPENDER_H
#define HAFFE_LOGAPPENDER_H

#include <fstream>
#include <memory>
#include "../base/Util.h"
#include "../base/Thread.h"

namespace haffe{

class LogFormatter;
class Logger;
class LogEvent;

class LogAppender{
    friend class Logger;
public:
    void setFormatter(std::shared_ptr<LogFormatter> formatter){
        Mutex::Lock lock(mutex_);
        formatter_=formatter;
        if(formatter_){
            hasFormattter= true;
        }
        else{
            hasFormattter= false;
        }
    }
    std::shared_ptr<LogFormatter> getFormatter() {
        Mutex::Lock lock(mutex_);
        return formatter_;
    }
    Loglevel getlevel() const{
        return level_;
    }
    void setlevel(Loglevel level){
        level_=level;
    }
    virtual void log(std::shared_ptr<Logger> logger,Loglevel level,std::shared_ptr<LogEvent> event)=0; //纯虚函数
    virtual ~LogAppender(){ } //析构函数定义为虚函数

protected:
    Loglevel level_=LOG_DEBUG;//默认类型
    std::shared_ptr<LogFormatter> formatter_;
    bool hasFormattter= false;
    Mutex mutex_;
};

class FileAppender : public LogAppender{
public:
    FileAppender(const std::string& filename);
    void log(std::shared_ptr<Logger> logger,Loglevel level,std::shared_ptr<LogEvent> event) override;

    bool reopen();
private:
    std::string filename_;
    std::ofstream filestream_;
    uint64_t lasttime_=0;
};

class StdoutAppender : public LogAppender{
public:
    void log(std::shared_ptr<Logger> logger,Loglevel level,std::shared_ptr<LogEvent> event) override;

};

}
#endif //HAFFE_LOGAPPENDER_H
