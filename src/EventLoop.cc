#include "EventLoop.h"
#include "Channel.h"
#include "Epoll.h"
#include "./Log/Logging.h"
#include <assert.h>
#include <stdlib.h>
using namespace haha_giraffe;
EventLoop* t_loopInThisThread = 0;
EventLoop::EventLoop():
                    loop_(false),
                    threadId_(static_cast<pid_t>(pthread_self())),
                    poller_(new Epoll(this)),
                    quit_(false)
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

const int kPollTimes = 10000;
void EventLoop::loop(){
    //printf("EventLoop::loop thread id is %f\n",pthread_self());
    LOG_INFO<<"EventLoop::loop thread id is "<<(long)pthread_self();
    assert(!loop_);
    assertInLoopThread();
    loop_ = true;
    quit_ = false;
    while (!quit_)//one loop per thread指的就是这个loop
    {
        activeChannels_.clear();
        //pollreturntime = poller_->poll(kPollTimes,&activeChannels_);//这里调用poll看有无发生事件，如果有则放到了activechannels_
        poller_->poll(kPollTimes,&activeChannels_);//先不用Timestamp
        for(ChannelList::iterator it=activeChannels_.begin();it!=activeChannels_.end();++it){
            (*it)->handleEvent();//进行处理事件
        }
        //doPendingFunctors();//io线程（占用loop的线程）在这里把pendingFunctors_中的回调函数事件都调用起来，
                            //也就是唤醒io线程从poll调用中返回，然后来处理事件
    }
    loop_ = false;
}

EventLoop* EventLoop::getEventLoopOfCurrentThread(){
    return t_loopInThisThread;
}

void EventLoop::updateChannel(Channel* channel){
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel){
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->removeChannel(channel);
}

void EventLoop::quit(){
    quit_ = true;
}