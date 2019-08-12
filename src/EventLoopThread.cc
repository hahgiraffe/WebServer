/*
 * @Description: 
 * @Author: hahagiraffe
 * @Date: 2019-08-12 22:03:52
 */
#include "EventLoop.h"
#include "EventLoopThread.h"
#include <functional>
#include <assert.h>
EventLoopThread::EventLoopThread()
    :loop_(NULL),
    exiting_(false),
    thread_(std::bind(&EventLoopThread::threadfunc,this)),
    mutex_(),
    condition_()
{
    
}

EventLoopThread::~EventLoopThread(){
    exiting_==true;
    if(loop_!=NULL){
        loop_->quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop(){
    
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while(loop_==NULL){
            condition_.wait(lock);
        }
        return loop_;
    }
}

void EventLoopThread::threadfunc(){
    EventLoop loop;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        loop_=&loop;
        condition_.notify_all();
    }
    loop.loop();
    loop_=NULL;
}