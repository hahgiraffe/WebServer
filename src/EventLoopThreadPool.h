/*
 * @Description: 
 * @Author: hahagiraffe
 * @Date: 2019-08-12 20:46:35
 */
#ifndef EVENTLOOPTHREADPOOL_H
#define EVENTLOOPTHREADPOOL_H
#include <vector>
#include <memory>
#include "Log/Logging.h"
using namespace haha_giraffe;

class EventLoop;
class EventLoopThread;
class EventLoopThreadPool{
public:
    EventLoopThreadPool(int numthread,EventLoop* baseloop);
    ~EventLoopThreadPool(){
        LOG_INFO<<"EventLoopThreadPool destructor";
    }
    EventLoop* getNextLoop();
    void start();
private:
    EventLoop* baseloop_;
    int numthread_;
    int next_;
    bool start_;
    std::vector<std::shared_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop*> loops_;
};

#endif
