#include "Epoll.h"
#include "Log/Logging.h"
#include "Channel.h"
#include <sys/epoll.h>
#include <poll.h>
#include <error.h>
#include <assert.h>
#include <unistd.h>
using namespace haha_giraffe;

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

Epoll::Epoll(EventLoop* loop)
                            :ownerLoop_(loop),
                            epollfd_(epoll_create1(EPOLL_CLOEXEC)),
                            events_(kInitEventsize)
{
    if (epollfd_ < 0)
    {
        LOG_ERROR << "EPoller::EPoller";
    }
}
    
Epoll::~Epoll(){
    ::close(epollfd_);
}

void Epoll::poll(int timeoutMs,ChannelList* activeChannel){
    int numEvents = ::epoll_wait(epollfd_, &*events_.begin(),static_cast<int>(events_.size()),timeoutMs);
    if(numEvents>0){
        LOG_INFO<<numEvents<<" events happended";
        fillActiveChannels(numEvents,activeChannel);//把有events的放到channel中
        if(events_.size() == numEvents){
            //这里指当前所有fd都有事件发生，即扩大数组范围
            events_.resize(events_.size()*2);
        }
    }
    else if (numEvents == 0)
    {
        LOG_INFO << " nothing happended";
    }
    else
    {
        LOG_INFO << "error EPoller::poll()";
    }
    return ;
}
    
void Epoll::updateChannel(Channel* channel){
    assertInLoopThread();
    LOG_INFO << "fd = " << channel->fd() << " events = " << channel->events();
    const int index = channel->index();
    if (index == kNew || index == kDeleted)
    {
        // a new one, add with EPOLL_CTL_ADD
        int fd = channel->fd();
        if (index == kNew)
        {
            assert(channels_.find(fd) == channels_.end());
            channels_[fd] = channel;
        }
        else // index == kDeleted
        {
        assert(channels_.find(fd) != channels_.end());
        assert(channels_[fd] == channel);
        }
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else
    {
        // update existing one with EPOLL_CTL_MOD/DEL
        int fd = channel->fd();
        (void)fd;
        assert(channels_.find(fd) != channels_.end());
        assert(channels_[fd] == channel);
        assert(index == kAdded);
        if (channel->isNoneEvent())
        {
        update(EPOLL_CTL_DEL, channel);
        channel->set_index(kDeleted);
        }
        else
        {
        update(EPOLL_CTL_MOD, channel);
        }
    }
}
    
void Epoll::removeChannel(Channel* channel){
    assertInLoopThread();
    int fd = channel->fd();
    LOG_INFO << "fd = " << fd;
    assert(channels_.find(fd) != channels_.end());
    assert(channels_[fd] == channel);
    assert(channel->isNoneEvent());
    int index = channel->index();
    assert(index == kAdded || index == kDeleted);
    size_t n = channels_.erase(fd);
    (void)n;
    assert(n == 1);
    if (index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}
    
void Epoll::fillActiveChannels(int numEvents,ChannelList* activeChannels) const{
    for(int i=0;i<numEvents;i++){
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
#ifndef NDEBUG
        int fd = channel->fd();
        ChannelMap::const_iterator it=channels_.find(fd);
        assert(it!=channels_.end());
        assert(it->second == channel);
#endif
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

//更改channel中fd监听的事件
void Epoll::update(int operation, Channel* channel){
    struct epoll_event event;
    bzero(&event, sizeof event);
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0)
    {
        if (operation == EPOLL_CTL_DEL)
        {
        LOG_ERROR << "epoll_ctl op=" << operation << " fd=" << fd;
        }
        else
        {
        LOG_ERROR << "epoll_ctl op=" << operation << " fd=" << fd;
        }
    }
}