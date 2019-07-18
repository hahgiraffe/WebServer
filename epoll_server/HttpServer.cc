#include "HttpServer.h"
#include "Epoll.h"
#include "../src/Log/Logging.h"
#include <iostream>
using namespace haha_giraffe;
HttpServer::HttpServer(int port,std::string addr)
        :sockfd_(0),
        epoll_(new Epoll()),
        addr_(port),
        Isrunning(false)
{   sockfd_=::socket(AF_INET,SOCK_STREAM,0);
    LOG_INFO<<"sockfd is "<<sockfd_;
    if(sockfd_<0){
         LOG_ERROR<<"HttpServer::HttpServer() socket() error";
    }
}

HttpServer::~HttpServer(){
    ::close(sockfd_);
}

void HttpServer::bind(){
    sockaddr_in addr=addr_.getaddress();
    if(::bind(sockfd_,(struct sockaddr*)&addr,sizeof(addr))==-1){
        LOG_ERROR<<"HttpServer::bind() error";
    }
    LOG_INFO<<"bind success";
}
    
void HttpServer::listen(){
    if(::listen(sockfd_,LISTENQUEEN)==-1){
        LOG_ERROR<<"HttpServer::listen() error";
    }
    LOG_INFO<<"listen success";
}
    
void HttpServer::handle_accept(){
    struct sockaddr_in clientaddr;
    socklen_t clientlen;
    //LOG_INFO<<"before accept";
    int clientfd=::accept(sockfd_,(sockaddr*)&clientaddr,&clientlen);
    //Address client(clientaddr);
    if(clientfd<0){
        LOG_ERROR<<"HttpServer::accept() error";
    }
    //在epoll上注册新的clientfd读事件
    struct epoll_event ev;
    ev.data.fd=clientfd;
    ev.events=EPOLLIN;
    epoll_->Epoll_ctl(EPOLL_CTL_ADD,clientfd,ev);
    //clientmap[clientfd]=client;这里是不行的，Address应该定义为noncopyable
    LOG_INFO<<"accept success";
}
    
void HttpServer::start(){
    bind();
    listen();
    struct epoll_event ev;
    ev.data.fd=sockfd_;
    ev.events=EPOLLIN;
    epoll_->Epoll_ctl(EPOLL_CTL_ADD,sockfd_,ev);
    Isrunning=true;
    while(Isrunning){
        struct epoll_event events[EPOLLEVENTS];//这个是触发事件组成的数组
        //LOG_INFO<<"asdasdsad";
        int eventnum=epoll_->Epoll_wait(events);
        //LOG_INFO<<"get "<<eventnum<<" events";
        handle_event(eventnum,events);
    }
}

void HttpServer::handle_event(int eventnum,struct epoll_event* events){
    for(int i=0;i<eventnum;i++){
        int eventfd=events[i].data.fd;
        if( (eventfd==sockfd_) && (events[i].events & EPOLLIN)){
            //新连接
            LOG_INFO<<"a new connection";
            handle_accept();
        }
        else if(events[i].events & EPOLLIN){
            //可读事件
            LOG_INFO<<"readable task";
            handle_read(eventfd);
        }else{ 
            //可写事件
            LOG_INFO<<"writable task";
            handle_write(eventfd);
        }
    }
}

void HttpServer::handle_read(int eventfd){
    char buf[BUFFESIZE];
    int readbyte=::read(eventfd,buf,sizeof(buf));
    if(readbyte==-1){
        LOG_ERROR<<"read error";
        //挡出错了以后要在epollfd中删除监听此fd的读事件
        struct epoll_event event;
        event.events=EPOLLIN;
        event.data.fd=eventfd;
        epoll_->Epoll_ctl(EPOLL_CTL_DEL,eventfd,event);
        ::close(eventfd);
    }
    else if(readbyte==0){
        LOG_INFO<<"client close";
        struct epoll_event event;
        event.events=EPOLLIN;
        event.data.fd=eventfd;
        epoll_->Epoll_ctl(EPOLL_CTL_DEL,eventfd,event);
        ::close(eventfd);
    }
    else{
        std::cout<<buf;
        //将其监听读事件更改为监听写事件
        struct epoll_event event;
        event.data.fd=eventfd;
        event.events=EPOLLOUT;
        epoll_->Epoll_ctl(EPOLL_CTL_MOD,eventfd,event);
    }
}

void HttpServer::handle_write(int eventfd){
    char* buf2="123456";
    int writebyte=::write(eventfd,buf2,strlen(buf2));
    if(writebyte<0){
        LOG_ERROR<<"write error";
        struct epoll_event event;
        event.events=EPOLLOUT;
        event.data.fd=eventfd;
        epoll_->Epoll_ctl(EPOLL_CTL_DEL,eventfd,event);
        ::close(eventfd);
    }
    LOG_INFO<<"write message "<<buf2;
    //又更改为读事件
    struct epoll_event event;
    event.data.fd=eventfd;
    event.events=EPOLLIN;
    epoll_->Epoll_ctl(EPOLL_CTL_MOD,eventfd,event);
}