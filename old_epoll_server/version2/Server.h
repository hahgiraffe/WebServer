/*
 * @Description: 
 * @Author: hahagiraffe
 * @Date: 2019-08-13 17:03:02
 */
#ifndef SERVER_H
#define SERVER_H
#include <functional>
#include <memory>
class EventLoop;
class EventLoopThreadPool;
class Channel;
class Channel;
class Server{
public:
    Server(EventLoop* loop,int numthread,int port);
    ~Server();
    void start();
    void handlenewconn();
    void handlethisconn();
private:
    int numthread_;
    int port_;
    std::unique_ptr<EventLoopThreadPool> pool_; //独一无二不可复制
    int listenfd_;
    std::shared_ptr<Channel> listenchannel_;
    EventLoop* loop_;
    bool state_;
    const int MAXFDS=10000;
    
};

#endif