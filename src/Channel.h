/*
 * @Description: Channel相当于fd的通道,每一个EventLoop对应一个channel
 * @Author: haha_giraffe
 * @Date: 2019-07-05 17:49:47
 */
#ifndef CHANNEL
#define CHANNEL

class EventLoop;
class Channel{
public:


private:
    void update();

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_;//每个channel对应一个EventLoop
    const int fd_;//每个channel对应一个fd
    int events_; //fd关心的io事件
    int revents_;//目前活动的事件
    int index_;
};


#endif