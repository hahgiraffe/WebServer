/*
 * @Description: 
 * @Author: hahagiraffe
 * @Date: 2019-08-14 15:14:17
 */
#include "EventLoop.h"
#include "Channel.h"
#include "Epoll.h"
#include "./Log/Logging.h"
#include "base/Util.h"
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
using namespace haha_giraffe;
EventLoop* t_loopInThisThread = 0;
int createEventfd(){
    int evfd=eventfd(0,EFD_NONBLOCK|EFD_CLOEXEC);
    if(evfd<0){
        LOG_ERROR<<"fail eventfd";
    }
    return evfd;
}

EventLoop::EventLoop():
                    loop_(false),
                    threadId_(static_cast<pid_t>(pthread_self())),
                    poller_(new Epoll()),
                    quit_(false),
                    wakeupfd_(createEventfd()),
                    eventHandling_(false),
                    callingPendingFunctors_(false),
                    pwakeupChannel_(new Channel(this,wakeupfd_))
{
    if(t_loopInThisThread){
        //printf("another Thread has include this eventloop %d\n",threadId_);
        LOG_ERROR<<"another Thread has include this eventloop";
    }
    else{
        t_loopInThisThread = this;
    }
    pwakeupChannel_->set_events(EPOLLIN| EPOLLET);
    pwakeupChannel_->setReadCallback(bind(EventLoop::handleRead,this));
    pwakeupChannel_->setConnCallback(bind(EventLoop::handleConn,this));
    poller_->epoll_add(pwakeupChannel_);
}
    
EventLoop::~EventLoop(){
    assert(!loop_);
    close(wakeupfd_);
    t_loopInThisThread = NULL;
}
    
void EventLoop::abortNotInLoopThread(){
    //printf("EventLoop::abortNotInLoopThread()");
    LOG_ERROR<<"EventLoop::abortNotInLoopThread()";
    exit(1);
}

const int kPollTimes = 10000;
void EventLoop::loop(){
   assert(!loop_);
   assertInLoopThread();
   loop_=true;
   quit_=false;
   std::vector<std::shared_ptr<Channel>> activechannel;
   while (!quit_)
   {
       activechannel.clear();
       activechannel=poller_->poll();
       eventHandling_=true;
       for(int i=0;i<activechannel.size();i++){
           activechannel[i]->handleEvent();
       }
       eventHandling_=false;
       doPendingFunctor();
       poller_->handleExpired();
   }
   
}

EventLoop* EventLoop::getEventLoopOfCurrentThread(){
    return t_loopInThisThread;
}

void EventLoop::quit(){
    quit_ = true;
    if(!isInLoopThread()){
        wakeup();
    }
}

void EventLoop::runInLoop(Functor&& cb){
    if(isInLoopThread()){
        cb();
    }
    else{
        queueInLoop(std::move(cb));
    }
}
    
void EventLoop::queueInLoop(Functor&& cb){
    {
        std::lock_guard<mutex> lock(mutex_);
        pendingFunctors_.emplace_back(std::move(cb));//相当于移动的push_back()
    }
    if(!isInLoopThread() || callingPendingFunctors_){
        wakeup();
    }
}

void EventLoop::wakeup(){
    uint64_t one = 1;
    ssize_t n = writen(wakeupfd_, (char*)(&one), sizeof(one));
    if (n != sizeof one)
    {
        LOG_INFO<< "EventLoop::wakeup() writes " << n << " bytes instead of 8";
    }
}

void EventLoop::handleRead(){
    uint64_t one = 1;
    ssize_t n = readn(wakeupfd_, &one, sizeof(one));
    if (n != sizeof(one))
    {
        LOG_INFO << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
    }
    //pwakeupChannel_->setEvents(EPOLLIN | EPOLLET | EPOLLONESHOT);
    pwakeupChannel_->set_events(EPOLLIN | EPOLLET);
}

void EventLoop::handleConn(){
    updatePoller(pwakeupChannel_);
}

void EventLoop::doPendingFunctor(){
    std::vector<Functor> functors;
    callingPendingFunctors_=true;
    {
        std::lock_guard<mutex> lock(mutex_);
        functors.swap(pendingFunctors_);
    }
    for(int i=0;i<functors.size();i++){
        functors[i]();
    }
    callingPendingFunctors_=false;
}