/*
 * @Description: Channel相当于fd的通道,每一个EventLoop对应一个channel
 * @Author: haha_giraffe
 * @Date: 2019-07-05 17:49:47
 */
#ifndef CHANNEL
#define CHANNEL
#include "noncopyable.h"
#include <functional>
using namespace std;
class EventLoop;
class Channel : noncopyable{
public:
    typedef function<void()> EventCallback;
    Channel(EventLoop* loop,int fd);
    void handleEvent(); //根据revents的值调用不同的用户调用
    void setReadCallback(const EventCallback& cb){
        readcallback_=cb;
    }
    void setWriteCallback(const EventCallback& cb){
        writecallback_=cb;
    }
    void setErrorCallback(const EventCallback& cb){
        errorcallback_=cb;
    }

    int fd() const{ return fd_; }
    int events() const { return events_; }
    void set_revents(int revt) { revents_=revt; }
    bool isNoneEvent() const { return events_ == kNoneEvent; }

    void enableReading() {events_ |= kReadEvent; update(); }
    void enableWriting() {events_ |= kWriteEvent; update(); }
    void disableWriting() {events_ &= ~kWriteEvent; update(); }
    void diableAll() {events_ = kNoneEvent; update(); }

    int index() const { return index_; }
    void set_index(int index) { index_ = index; }

    EventLoop* ownerLoop() const { return loop_; }
private:
    void update();//用于更改完events后的更新操作

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_;//每个channel对应一个EventLoop
    const int fd_;//每个channel对应一个fd
    int events_; //fd关心的io事件
    int revents_;//目前活动的事件
    int index_; //kAdded（已经监听）,kNew（新建）,kDeleted（已经被删除）,用于体现当前fd的状态

    EventCallback readcallback_;
    EventCallback writecallback_;
    EventCallback errorcallback_;
};


#endif