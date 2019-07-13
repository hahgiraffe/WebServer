#include "Channel.h"
#include "EventLoop.h"
#include <poll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop,int fd)
                                    :loop_(loop),
                                    fd_(fd),
                                    events_(0),
                                    revents_(0),
                                    index_(-1)
{

}

void Channel::handleEvent(){
    if(revents_ & POLLNVAL){
        printf("Channel::handle_event() POLLNVAL");
    }

    if(revents_ & (POLLERR | POLLNVAL)){
       if(errorcallback_) errorcallback_();
    }
    
    if(revents_ & (POLLIN | POLLPRI | POLLRDHUP)){
        if(readcallback_) readcallback_();
    }
    if(revents_ & POLLOUT){
        if(writecallback_) writecallback_();
    }
}

void Channel::update(){
    loop_->updateChannel(this);
}