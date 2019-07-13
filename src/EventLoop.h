/*
 * @Description: 每个线程一个事件循环
 * @Author: haha_giraffe
 * @Date: 2019-06-21 16:07:14
 */
#ifndef EVENTLOOP
#define EVENTLOOP
#include <pthread.h>
#include <memory>
#include <vector>
class Channel;
class Epoll;
class EventLoop{

public:
    EventLoop();
    ~EventLoop();
    void loop();
    void assertInLoopThread(){
        if(!isInLoopThread()){
            abortNotInLoopThread();
        }
    }
    EventLoop* getEventLoopOfCurrentThread();
    bool isInLoopThread() const { return threadId_ ==static_cast<pid_t>(pthread_self());}
    
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    void quit();
private:

    typedef std::vector<Channel*> ChannelList;
    void abortNotInLoopThread();
    bool loop_;
    const pid_t threadId_;
    std::unique_ptr<Epoll> poller_;
    bool quit_;
    ChannelList activeChannels_;
};

#endif //EVENTLOOP