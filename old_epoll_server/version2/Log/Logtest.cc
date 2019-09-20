#include "Logging.h"
#include "AsyncLog.h"
using namespace haha_giraffe;

AsyncLogger *logptr=NULL;

void asyncoutput(const char *msg,int len){
    logptr->Append(msg,len);//前台进程传递日志
}

int main(int argc,char *argv[]){
    //前面这里默认是标准输出
    LOG_DEBUG<<"hello";
    LOG_WARN<<"hello";
    for(int i=0;i<10;i++){
        LOG_INFO<<"ASYNC LOG";
        struct timespec ts = { 0, 500*1000*1000 };
        nanosleep(&ts, NULL);
    }
    //这里设置SetOutput则会输出到日志文件中
    AsyncLogger asynclog("asynclog",500*1000*1000);    
    asynclog.Start();//这里先让后台线程运行
    logptr=&asynclog;
    Logger::SetOutput(asyncoutput);
    for(int i=0;i<10;i++){
        LOG_INFO<<"ASYNC LOG";
        struct timespec ts = { 0, 500*1000*1000 };
        nanosleep(&ts, NULL);
    }
    LOG_ERROR<<"good morning";
    
}