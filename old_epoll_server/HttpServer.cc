#include "HttpServer.h"
#include "../src/Log/Logging.h"
#include "IOutil.h"
#include "ThreadPool.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
using namespace haha_giraffe;

class Mytask:public Task{
public:
    Mytask(){}
    virtual int run(){
        printf("thread[%lu]\n",pthread_self());
        _arg();
        //sleep(1);  
        return 0;  
    }
};

HttpServer::HttpServer(int port,std::string addr)
        :sockfd_(0),
        epoll_(new Epoll()),
        addr_(port),
        Isrunning(false),
        threadpool_(new ThreadPool())
{   sockfd_=::socket(AF_INET,SOCK_STREAM,0);
    LOG_INFO<<"sockfd is "<<sockfd_;
    if(sockfd_<0){
         LOG_ERROR<<"HttpServer::HttpServer() socket() error";
    }
    //这里设置sockfd_非阻塞，长连接，端口重用
    setnonblock(sockfd_);
    int value=1;
    if(setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &value, sizeof(int))==-1){
        LOG_ERROR<<"HttpServer() KEEPALIVE error";
    }
    if(setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(int))==-1){
        LOG_ERROR<<"HttpServer() SO_REUSEADDR error";
    }
}

HttpServer::~HttpServer(){
    ::close(sockfd_);
    threadpool_->stop();
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

//外部调用，开启服务器
void HttpServer::start(){
    bind();
    listen();
    struct epoll_event ev;
    ev.data.fd=sockfd_;
    ev.events=EPOLLIN;
    epoll_->epoll_add(sockfd_,ev);
    Isrunning=true;
    while(Isrunning){
        struct epoll_event events[EPOLLEVENTS];//这个是触发事件组成的数组
        //LOG_INFO<<"asdasdsad";
        int eventnum=epoll_->wait(events);
        //LOG_INFO<<"get "<<eventnum<<" events";
        handle_event(eventnum,events);
    }
}

//分发事件
void HttpServer::handle_event(int eventnum,struct epoll_event* events){
    for(int i=0;i<eventnum;i++){
        int eventfd=events[i].data.fd;
        if( (eventfd==sockfd_) && (events[i].events & EPOLLIN)){
            //新连接
            LOG_INFO<<"a new connection";
            //Mytask taskobj;
            //这里主要成员函数不能转换为void*
            //taskobj.setarg(std::bind(&HttpServer::handle_accept,this));
            //threadpool_->addTask(&taskobj);
            handle_accept();
            //handleConn_(events[i].events);
        }
        else if(events[i].events & EPOLLIN){
            //可读事件
            LOG_INFO<<"readable task ,readable fd is "<<events[i].data.fd;
            //handle_read(eventfd);
            //Mytask taskobj;
            //taskobj.setarg(std::bind(&HttpServer::handleRead_,this));
            handleRead_(eventfd);
        }else{ 
            //可写事件
            LOG_INFO<<"writable task ，writable fd is "<<events[i].data.fd;
            //handle_write(eventfd);
            handleWrite_(eventfd);
        }
    }
}

//新连接，接收事件
void HttpServer::handle_accept(){
    //因为sockfd_设置为非阻塞，accept这里就要变成while
    struct sockaddr_in clientaddr;
    socklen_t clientlen;
    int clientfd;
    while((clientfd=::accept(sockfd_,(sockaddr*)&clientaddr,&clientlen))>0){
        //当大于0，非阻塞的sockfd_才accept成功,通过clientfd服务器与客户端通信
        setnonblock(clientfd);//将新连接得到的fd设置为非阻塞
        //在epoll上注册新的clientfd读事件
        struct epoll_event ev;
        ev.data.fd=clientfd;
        //LOG_INFO<<"before "<<(int)ev.events;
        //handleConn_(&ev.events);
        //LOG_INFO<<"after "<<(long)ev.events;
        ev.events=EPOLLIN | EPOLLET;
        epoll_->epoll_add(clientfd,ev);
        //clientmap[clientfd]=client;这里是不行的，Address应该定义为noncopyable
        LOG_INFO<<"accept success，clientfd is "<<clientfd;
    }
    //Address client(clientaddr);
    if(clientfd==-1){
        if (errno != EAGAIN && errno != ECONNABORTED && errno != EPROTO && errno != EINTR){
            LOG_ERROR<<"HttpServer::accept() error";
        }
    }
    
}

//处理读事件
// void HttpServer::handle_read(int eventfd){
//     //这里非阻塞要while(read)
//     //char buf[BUFFESIZE];
//     //int readbyte;
//     //int allsize=0;
//     // while(readbyte=::read(eventfd,buf+allsize,BUFFESIZE-1)>0){
//     //     allsize+=readbyte;
//     // }
//     // if(readbyte==-1 && errno!=EAGAIN){
//     //     LOG_ERROR<<"read error";
//     //     //挡出错了以后要在epollfd中删除监听此fd的读事件
//     //     struct epoll_event event;
//     //     event.events=EPOLLIN;
//     //     event.data.fd=eventfd;
//     //     epoll_->epoll_delete(eventfd,event);
//     //     ::close(eventfd);
//     // }
//     bool isreadable=false;
//     std::string buffer;
//     if(readbufferfunc(eventfd,buffer,isreadable)==-1){
//         exit(1);
//     }
//     std::cout<<"---------------"<<std::endl;
//     std::cout<<buffer;
//     std::cout<<"---------------"<<std::endl;
//     /*将其监听读事件更改为监听写事件
//     这里要理解一下，在ET模式下，当epoll注册了EPOLLOUT事件时，
//     即刻就使得epoll_wait返回，因为当fd的输出缓冲区为空就会返回EPOLLOUT事件
//     ET模式下返回一次，LT模式下多次返回
//     当发送大数据的时候，一次性发不完，即发送的数据超过缓冲区大小，
//     ET循环发完才行，LT会返回EPOLLOUT
//     */
//     struct epoll_event event;
//     event.data.fd=eventfd;
//     event.events=EPOLLOUT;
//     epoll_->epoll_mod(eventfd,event);
// }

// //处理写事件,非阻塞写事件while(write)
// void HttpServer::handle_write(int eventfd){
//     char buf2[BUFFESIZE];
//     snprintf(buf2,sizeof(buf2),"HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\nHello World\n", 12);
//     int datasize=strlen(buf2);//strlen(buf2)为11 sizeof(buf2)为BUFFERSIZE
//     // int writebyte,n=datasize;
//     // while(n>0){
//     //     writebyte=::write(eventfd,buf2+datasize-n,n);
//     //     if(writebyte<0){
//     //         LOG_ERROR<<"write error";
//     //         break;
//     //     }
//     //     n-=writebyte;
//     // }
//     // std::cout<<buf2;
//     std::string buffer(buf2);
//     writebufferfunc(eventfd,buffer,datasize);
//     //又更改为读事件
//     struct epoll_event event;
//     event.data.fd=eventfd;
//     event.events=EPOLLIN;
//     epoll_->epoll_mod(eventfd,event);
// }

//将fd设置为nonblock
void HttpServer::setnonblock(int fd){
    int options=fcntl(fd,F_GETFL);
    if(options<0){
        LOG_ERROR<<"setnonblock F_GETFL error";
    }
    options=options|O_NONBLOCK;
    if(fcntl(fd,F_SETFL,options)<0){
        LOG_ERROR<<"setnonblock F_SETFL error";
    }
}