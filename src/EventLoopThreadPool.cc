/*
 * @Description: 
 * @Author: hahagiraffe
 * @Date: 2019-08-12 20:56:00
 */
#include "EventLoop.h"
#include "EventLoopThread.h"
#include "EventLoopThreadPool.h"
#include <assert.h>

EventLoopThreadPool::EventLoopThreadPool(int numthread,EventLoop* baseloop)
    :baseloop_(baseloop),
    numthread_(numthread),
    next_(0),
    start_(false)
{
    assert(numthread>0);
    LOG_INFO<<"EventLoopThreadPool constructor";
}

EventLoop* EventLoopThreadPool::getNextLoop(){
    baseloop_->assertInLoopThread();
    start_=true;
    EventLoop* loop=baseloop_;
    if(!loops_.empty()){
        loop=loops_[next_];
        next_=(next_+1)%numthread_;
    }
    return loop;
}

void EventLoopThreadPool::start(){
    baseloop_->assertInLoopThread();
    start_=true;
    for(int i=0;i<numthread_;i++){
        std::shared_ptr<EventLoopThread> tmp(new EventLoopThread());
        threads_.push_back(tmp);
        loops_.push_back(tmp->startLoop());
    }
}