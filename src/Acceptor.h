/*
 * @Description: 将Socket,Channel整合在一起
 * @Author: haha_giraffe
 * @Date: 2019-07-15 19:40:54
 */
#ifndef ACCEPTOR_H
#define ACCEPTOR_H
#include "noncopyable.h"
#include "Channel.h"
#include "Socket.h"
#include <functional>
class EventLoop;
class Address;
class Acceptor : noncopyable{
public:
typedef std::function<void (int,const Address&)> NewConnectionCallback;
    Acceptor(EventLoop* loop,const Address& addr);
    void setNewConnectionCallback(const NewConnectionCallback& cb){
        newconnectioncallback_=cb;
    }
    void listen();
private:
    void handleRead(int );
    EventLoop* loop_;
    Socket acceptSocket_;
    Channel acceptChannel_; //注意这里的acceptSocket_和acceptChannel_的顺序不同
    NewConnectionCallback newconnectioncallback_;
    bool Islistening_;
};

#endif