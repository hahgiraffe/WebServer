/*
 * @Description: 
 * @Author: hahagiraffe
 * @Date: 2019-08-12 15:03:47
 */
#include "Channel.h"
#include "EventLoop.h"
//#include <poll.h>
#include <sys/epoll.h>

Channel::Channel(EventLoop* loop,int fd)
                                    :loop_(loop),
                                    fd_(fd),
                                    events_(0),
                                    revents_(0),
                                    lastevent_(0)
{

}

Channel::Channel(EventLoop* loop)
                                :loop_(loop),
                                events_(0),
                                revents_(0),
                                lastevent_(0)
{

}

Channel::~Channel(){

}

void Channel::handleEvent(){
    events_=0;
    if(revents_ & EPOLLERR) {
       if(errorcallback_) errorcallback_();
    }
    if(revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)){
        if(readcallback_) readcallback_();
    }
    if(revents_ & EPOLLOUT){
        if(writecallback_) writecallback_();
    }
    if(conncallback_) conncallback_();
}

// int Channel::parse_URI(){

// }
// int Channel::parse_Header(){

// }
// int Channel::analysisRequest(){
    
// }