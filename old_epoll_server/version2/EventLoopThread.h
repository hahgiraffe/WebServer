/*
 * @Description: 
 * @Author: hahagiraffe
 * @Date: 2019-08-12 21:43:18
 */
#ifndef EVENTLOOPTHREAD_H
#define EVENTLOOPTHREAD_H
#include <thread>
#include <mutex>
#include <condition_variable>
#include "noncopyable.h"
class EventLoop;
class EventLoopThread : noncopyable{
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* startLoop();
private:
    void threadfunc();
    EventLoop* loop_;
    bool exiting_;
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable condition_; 

};
#endif