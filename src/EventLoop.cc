#include "EventLoop.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
EventLoop* t_loopInThisThread = 0;
EventLoop::EventLoop():
                    loop_(false),
                    threadId_(static_cast<pid_t>(pthread_self()))
{
    if(t_loopInThisThread){
        printf("another Thread has include this eventloop %d\n",threadId_);
    }
    else{
        t_loopInThisThread = this;
    }
}
    
EventLoop::~EventLoop(){
    assert(!loop_);
}
    
void EventLoop::abortNotInLoopThread(){
    printf("EventLoop::abortNotInLoopThread()");
    exit(1);
}

void EventLoop::loop(){
    printf("EventLoop::loop thread id is %d\n",pthread_self());
    assert(!loop_);
    assertInLoopThread();
    loop_ = true;
    //poll
    loop_ = false;
}

EventLoop* EventLoop::getEventLoopOfCurrentThread(){
    return t_loopInThisThread;
}