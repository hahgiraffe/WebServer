/*
 * @Description: EPOLL
 * @Author: haha_giraffe
 * @Date: 2019-07-09 12:10:05
 */
#ifndef EPOLL_H
#define EPOLL_H
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include "noncopyable.h"
class Channel;
class Epoll : noncopyable{
public:
    typedef std::vector<std::shared_ptr<Channel>> VectorChannel;
    Epoll();
    ~Epoll();
    void epoll_add(std::shared_ptr<Channel> channel);
    void epoll_mod(std::shared_ptr<Channel> channel);
    void epoll_del(std::shared_ptr<Channel> channel);
    VectorChannel poll();
    VectorChannel getEvent(int eventsnum);
    //void add_timer(std::shared_ptr<Channel> requestdata,int timeout);
    int getepollfd(){
        return epollfd_;
    }
    void handleExpired();
private:
    typedef std::vector<struct epoll_event> EventList;
    static const int MAXFDS=10000;
    const int EPOLLTIMEOUT=1000;
    int epollfd_;
    EventList events_;//vector<epoll_event>监听的事件
    std::shared_ptr<Channel> fdtochannel[MAXFDS];
    //std::shared_ptr<HttpData> fdtohttp[MAXFDS];
    //TimerManager timerManager_;
};
#endif