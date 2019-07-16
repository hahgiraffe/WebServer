#include "Acceptor.h"
#include "EventLoop.h"
#include "Log/Logging.h"
#include "Address.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
//#include <iostream>
using namespace haha_giraffe;
//创建一个非阻塞socketfd并返回
int createnonblocksocket(){
    int sockfd=socket(AF_INET,SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,IPPROTO_TCP);
    if(sockfd<0){
        LOG_ERROR<<"Accept::createnonblocksocket";
    }
    //std::cout<<"sockfd is "<<sockfd<<std::endl;
    return sockfd;
}

Acceptor::Acceptor(EventLoop* loop,const Address& addr)
                                :loop_(loop),
                                acceptSocket_(createnonblocksocket()),
                                acceptChannel_(loop,acceptSocket_.fd()),
                                Islistening_(false)

{
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.bindAddress(addr);
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead,this));
}

void Acceptor::listen(){
    loop_->assertInLoopThread();
    Islistening_=true;
    acceptSocket_.listen();
    acceptChannel_.enableReading();//update->epoll
}
//回调的时候读
void Acceptor::handleRead(){
    loop_->assertInLoopThread();
    Address peeraddress(0);
    int connfd=acceptSocket_.accept(peeraddress);
    if(connfd>=0){
        newconnectioncallback_(connfd,peeraddress);
    }
    else{
        close(connfd);
    }
}

