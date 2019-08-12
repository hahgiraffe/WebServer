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

class EventLoop;
class EventLoopThread{
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* startLoop();
private:
    EventLoop* loop_;


};
#endif