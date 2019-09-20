/*
 * @Description: 
 * @Author: hahagiraffe
 * @Date: 2019-08-13 21:06:17
 */
#include "Epoll.h"
#include "Log/Logging.h"
#include "Channel.h"
#include "HttpParse.h"
#include <sys/epoll.h>
#include <error.h>
#include <assert.h>
#include <unistd.h>
using namespace haha_giraffe;
typedef std::vector<std::shared_ptr<Channel>> VectorChannel;
const int MAXEVENTS = 1024;
Epoll::Epoll()
    :epollfd_(epoll_create1(EPOLL_CLOEXEC)),
    events_(MAXEVENTS)
{
    assert(epollfd_>=0);
}

Epoll::~Epoll(){

}
    
void Epoll::epoll_add(std::shared_ptr<Channel> channel){
    int fd=channel->getfd();
    /* TODO TIMER */
    fdtohttp[fd]=channel->getparser();
    struct epoll_event event;
    event.data.fd=fd;
    event.events=channel->get_events();
    channel->updatelastevent();
    fdtochannel[fd]=channel;
    if(epoll_ctl(epollfd_,EPOLL_CTL_ADD,fd,&event)<0){
        LOG_ERROR<<"epoll_add error";
        fdtochannel[fd].reset();
    }
}
    
void Epoll::epoll_mod(std::shared_ptr<Channel> channel){
    int fd= channel->getfd();
    //如果channel关心的event没有改变则不需要mod
    if(!channel->updatelastevent()){
        struct epoll_event event;
        event.data.fd=fd;
        event.events=channel->get_events();
        if(epoll_ctl(epollfd_,EPOLL_CTL_MOD,fd,&event)<0){
            LOG_ERROR<<"epoll_mod error";
            fdtochannel[fd].reset();
        }
    }
}
    
void Epoll::epoll_del(std::shared_ptr<Channel> channel){
    int fd= channel->getfd();
    struct epoll_event event;
    event.data.fd=fd;
    event.events=channel->getlastevent();
    if(epoll_ctl(epollfd_,EPOLL_CTL_DEL,fd,&event)<0){
        LOG_ERROR<<"epoll_del error";
    }
    fdtochannel[fd].reset();
    fdtohttp[fd].reset();
}

VectorChannel Epoll::poll(){
    while (true)
    {
        int event_count=epoll_wait(epollfd_,&*events_.begin(),events_.size(),EPOLLTIMEOUT);
        if(event_count<0){
            LOG_ERROR<<"eventcount<0";
        }
        //LOG_INFO<<"num "<<event_count<<" event happend ";
        VectorChannel activechannel=getEvent(event_count);
        if(activechannel.size()>0){
            return activechannel;
        }
    }
    
}
    
VectorChannel Epoll::getEvent(int eventsnum){
    VectorChannel activechannel;
    for(int i=0;i<eventsnum;i++){
        int fd=events_[i].data.fd;
        std::shared_ptr<Channel> channel=fdtochannel[fd];
        if(channel){
            channel->set_revents(events_[i].events);
            channel->set_events(0);
            activechannel.push_back(channel);
        }
        else{
            LOG_ERROR<<"get channel error";
        }
    }
    return activechannel;
}

void Epoll::handleExpired(){

}
