/*
 * @Description: 每个线程一个事件循环
 * @Author: haha_giraffe
 * @Date: 2019-06-21 16:07:14
 */
#ifndef EVENTLOOP
#define EVENTLOOP
#include <pthread.h>

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
    
private:
    void abortNotInLoopThread();
    bool loop_;
    const pid_t threadId_;

};

#endif //EVENTLOOP