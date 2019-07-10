#include "EventLoop.h"
#include "Channel.h"
#include "./Log/Logging.h"
#include <assert.h>
#include <stdlib.h>
using namespace haha_giraffe;
EventLoop* t_loopInThisThread = 0;
EventLoop::EventLoop():
                    loop_(false),
                    threadId_(static_cast<pid_t>(pthread_self()))
{
    if(t_loopInThisThread){
        //printf("another Thread has include this eventloop %d\n",threadId_);
        LOG_ERROR<<"another Thread has include this eventloop";
    }
    else{
        t_loopInThisThread = this;
    }
}
    
EventLoop::~EventLoop(){
    assert(!loop_);
}
    
void EventLoop::abortNotInLoopThread(){
    //printf("EventLoop::abortNotInLoopThread()");
    LOG_ERROR<<"EventLoop::abortNotInLoopThread()";
    exit(1);
}

void EventLoop::loop(){
    //printf("EventLoop::loop thread id is %f\n",pthread_self());
    LOG_INFO<<"EventLoop::loop thread id is "<<(long)pthread_self();
    assert(!loop_);
    assertInLoopThread();
    loop_ = true;
    //poll
    loop_ = false;
}

EventLoop* EventLoop::getEventLoopOfCurrentThread(){
    return t_loopInThisThread;
}

void EventLoop::updateChannel(Channel* channel){
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    //poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel){
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    //poller_->removeChannel(channel);
}