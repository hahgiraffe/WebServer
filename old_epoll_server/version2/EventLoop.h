/*
 * @Description: 每个线程一个事件循环
 * @Author: haha_giraffe
 * @Date: 2019-06-21 16:07:14
 */
#ifndef EVENTLOOP_H
#define EVENTLOOP_H
#include <pthread.h>
#include <memory>
#include <vector>
#include <mutex>
#include <functional>
#include "noncopyable.h"
class Channel;
class Epoll;
class EventLoop : noncopyable{
public:
    typedef std::function<void()> Functor;
    EventLoop();
    ~EventLoop();
    void loop();
    void assertInLoopThread(){
        if(!isInLoopThread()){
            abortNotInLoopThread();
        }
    }
    EventLoop* getEventLoopOfCurrentThread();//返回this指针
    bool isInLoopThread() const { return threadId_ ==static_cast<pid_t>(pthread_self());}
    void quit();

    void runInLoop(Functor&& cb);
    void queueInLoop(Functor&& cb);

    //void shutdown(std::shared_ptr<Channel> channel);
    void removeFromPoller(std::shared_ptr<Channel> channel);
    void updatePoller(std::shared_ptr<Channel> channel);
    void addToPoller(std::shared_ptr<Channel> channel);

    long getthreadid(){
        return threadId_;
    }
private:
    void abortNotInLoopThread();
    void wakeup();
    void handleRead();
    void handleConn();
    void doPendingFunctor();
    bool loop_;
    const pid_t threadId_;
    std::shared_ptr<Epoll> poller_;
    bool quit_;
    int wakeupfd_;
    //注意这里wakeupfd的作用不是监听外部来的连接或者数据，而是用于监听自己事件
    bool eventHandling_;
    std::vector<Functor> pendingFunctors_;
    bool callingPendingFunctors_;
    std::shared_ptr<Channel> pwakeupChannel_;
    std::mutex mutex_;
   
};

#endif //EVENTLOOP