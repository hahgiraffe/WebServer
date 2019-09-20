/*
 * @Description: Channel相当于fd的通道,每一个EventLoop对应一组channel
 * @Author: haha_giraffe
 * @Date: 2019-07-05 17:49:47
 */
#ifndef CHANNEL_H
#define CHANNEL_H
#include "noncopyable.h"
#include <functional>
#include <memory>
using namespace std;
class EventLoop;
class HttpParse;
class Channel : noncopyable{
public:
    typedef function<void()> Callback;
    Channel(EventLoop* loop,int fd);
    Channel(EventLoop* loop);
    ~Channel();
    void handleEvent(); //根据revents的值调用不同的用户调用
    void setReadCallback(const Callback& cb){
        readcallback_=cb;
    }
    void setWriteCallback(const Callback& cb){
        writecallback_=cb;
    }
    void setErrorCallback(const Callback& cb){
        errorcallback_=cb;
    }

    void setConnCallback(const Callback& cb){
        conncallback_=cb;
    }
    int getfd() const{ return fd_; }
    void setfd(int fd) { fd_=fd;}
    void setparser(std::shared_ptr<HttpParse> parse){
        parse_=parse;
    }
    std::shared_ptr<HttpParse> getparser(){
        std::shared_ptr<HttpParse> result(parse_.lock());
        return result;
    }
    // void handleRead();
    // void handleWrite();
    // void handleError();
    // void handleConn();
    int set_events(int ev) { events_=ev; }
    void set_revents(int revt) { revents_=revt; }
    int& get_events() { return events_;}
    EventLoop* ownerLoop() const { return loop_; }
    //如果lastevent和event不相同则返回false
    bool updatelastevent(){
        bool res=(lastevent_==events_);
        lastevent_=events_;
        return res;
    }
    int getlastevent(){
        return lastevent_;
    }
private:
    // int parse_URI();
    // int parse_Header();
    // int analysisRequest();

    EventLoop* loop_;//每个channel对应一个EventLoop
    int fd_;//每个channel对应一个fd
    int events_; //fd关心的io事件
    int revents_;//fd目前活动的事件
    int lastevent_; //比较和events_区别

    std::weak_ptr<HttpParse> parse_;
    
    Callback readcallback_;
    Callback writecallback_;
    Callback errorcallback_;
    Callback conncallback_;
};


#endif