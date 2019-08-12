/*
 * @Description: 
 * @Author: hahagiraffe
 * @Date: 2019-08-12 15:03:47
 */
#include "Channel.h"
#include "EventLoop.h"
//#include <poll.h>
#include <sys/epoll.h>
const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop* loop,int fd)
                                    :loop_(loop),
                                    fd_(fd),
                                    events_(0),
                                    revents_(0),
                                    index_(-1)
{

}

void Channel::handleEvent(){
    if(revents_ & EPOLLERR) {
       if(errorcallback_) errorcallback_();
    }
    if(revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)){
        if(readcallback_) readcallback_(this->fd());
    }
    if(revents_ & EPOLLOUT){
        if(writecallback_) writecallback_();
    }
}

void Channel::update(){
    loop_->updateChannel(this);
}