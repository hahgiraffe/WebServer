/*
 * @Description: EPOLL
 * @Author: haha_giraffe
 * @Date: 2019-07-09 12:10:05
 */
#ifndef EPOLL_H
#define EPOLL_H
#include <vector>
#include <map>
#include "EventLoop.h"
#include "noncopyable.h"
class Channel;
class Epoll : noncopyable{
public:
    typedef std::vector<Channel*> ChannelList;
    Epoll(EventLoop* loop);
    ~Epoll();
    void poll(int timeoutMs,ChannelList* activeChannel);//先不返回Timestamp
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    void assertInLoopThread() { ownerLoop_->assertInLoopThread(); }
private:
    static const int kInitEventsize = 20;
    void fillActiveChannels(int numEvents,ChannelList* activeChannels) const;
    void update(int operation, Channel* channel);
    typedef std::vector<struct epoll_event> EventList;
    typedef std::map<int,Channel*> ChannelMap;
    EventLoop* ownerLoop_;
    int epollfd_;
    EventList events_;//vector<epoll_event>监听的事件
    ChannelMap channels_;//map<fd,fd所对应的channel*>
};
#endif