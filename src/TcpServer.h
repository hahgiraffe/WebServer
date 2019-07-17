/*
 * @Description:  TcpServer
 * @Author: haha_giraffe
 * @Date: 2019-07-17 19:41:35
 */
#ifndef TCPSERVER_H
#define TCPSERVER_H
#include "noncopyable.h"
#include <string>
#include <memory>
class EventLoop;
class Acceptor;
class Address;
class TcpServer : noncopyable{
public:
    TcpServer(EventLoop* loop,Address& addr,std::string name);
    ~TcpServer();
    void start();
    void setConnectionCallback(const ConnectionCallback& cb){//这里在主函数中就调用了如test8.cc
        connectioncallback_=cb;
    }
    void setMessageCallback(const MessageCallback& cb){//这里在主函数中就调用了
        messagecallback_=cb;
    }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    { writeCompleteCallback_ = cb; }
private:
    EventLoop* loop_;
    const std::string name_;
    std::shared_ptr<Acceptor> acceptor_;
    ConnectionCallback connectioncallback_;
    MessageCallback messagecallback_;
    WriteCompleteCallback writeCompleteCallback_;
    bool start_;
    int nextConnId_;
};

#endif