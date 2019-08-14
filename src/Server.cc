/*
 * @Description: 
 * @Author: hahagiraffe
 * @Date: 2019-08-13 17:22:02
 */
#include "Server.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "Log/Logging.h"
#include "Channel.h"
#include "./base/Util.h"
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace haha_giraffe;

Server::Server(EventLoop* loop,int numthread,int port)
    :numthread_(numthread),
    port_(port),
    pool_(new EventLoopThreadPool(numthread,loop)),
    listenfd_(createlistenfd(port)),
    listenchannel_(new Channel(loop,listenfd_)),
    loop_(loop),
    state_(false)
{
    setlistenfdnonblock(listenfd_);
}

Server::~Server(){

}

void Server::start(){
    pool_->start();
    listenchannel_->set_events(EPOLLIN | EPOLLET);
    listenchannel_->setReadCallback(std::bind(&Server::handlenewconn,this));
    listenchannel_->setConnCallback(std::bind(&Server::handlethisconn,this));
    //
    loop_->addToPoller(listenchannel_);
    state_=true;
}
