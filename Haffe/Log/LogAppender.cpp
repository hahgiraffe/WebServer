/*
 * @Description: 
 * @Author: hahagiraffe
 * @Date: 2019-08-28 16:42:56
 */
#include "LogFormatter.h"
#include "LogAppender.h"
#include "Logger.h"
#include <iostream>
namespace haffe {

    FileAppender::FileAppender(const std::string& filename)
        :filename_(filename)
    {
        reopen();
    }

    void FileAppender::log(std::shared_ptr<Logger> logger,Loglevel level, std::shared_ptr<LogEvent> event){
        if(level>=level_){
            Mutex::Lock lock(mutex_);
            if(!formatter_->format(filestream_,logger,level,event)){
                std::cout<<"error"<<std::endl;
            }
        }
    }

    bool FileAppender::reopen(){
        if(filestream_){
            filestream_.close();
        }
        filestream_.open(filename_);
        return !!filestream_;
    }

    void StdoutAppender::log(std::shared_ptr<Logger> logger,Loglevel level,std::shared_ptr<LogEvent> event){
        //std::cout<<"into stdoutappender\n";
        if(level>=level_){
            Mutex::Lock lock(mutex_);
            formatter_->format(std::cout,logger,level,event);
        }
    }
}