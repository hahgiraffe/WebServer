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
#include "Socket.h"
#include "HttpParse.h"
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
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
    handle_for_sigpipe();
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
    LOG_INFO<<"listenfd epoll_add ,listenfd is"<<listenfd_;
    state_=true;
}

//这个是listenfd监听，当有EPOLLIN事件时候，
void Server::handlenewconn(){
    struct sockaddr_in clientaddr;
    int clientfd;
    memset(&clientaddr,0,sizeof(clientaddr));
    socklen_t clientaddrsize=sizeof(clientaddr);
    //ET模式要这样
    while(clientfd=accept(listenfd_,(sockaddr*)&clientaddr,&clientaddrsize)>0)
    {
        EventLoop *nextloop=pool_->getNextLoop();
        LOG_INFO<<"nextloop pid is "<<(long)nextloop->getthreadid();
        LOG_INFO<<"new connection from "<<inet_ntoa(clientaddr.sin_addr)<<" : "<<ntohs(clientaddr.sin_port);
        if(clientfd>MAXFDS){
            close(clientfd);
            LOG_ERROR<<"fd not enough ";
        }
        setlistenfdnonblock(clientfd);
        setnodelay(clientfd);
        std::shared_ptr<HttpParse> parse(new HttpParse(nextloop,clientfd));
        parse->getchannel()->setparser(parse);
        //注意这里是nextloop，新的监听fd的关联loop
        nextloop->queueInLoop(std::bind(&HttpParse::newEvent,parse));
    }
    listenchannel_->set_events(EPOLLIN|EPOLLET);
}

void Server::handlethisconn(){
    loop_->updatePoller(listenchannel_); 
}