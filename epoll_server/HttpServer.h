/*
 * @Description: HttpServer用于实现基础操作
 * @Author: hahagiraffe
 * @Date: 2019-07-18 11:45:07
 */
#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include "Address.h"
#include <memory>
#include <string>
#include <map>
class Epoll;
class HttpServer{
public:
    HttpServer(int port,std::string addr="127.0.0.1");
    ~HttpServer();
    void start();
private:

    void setnonblock(int fd);

    void handle_accept();
    void bind();
    void listen();
    void handle_event(int eventnum,struct epoll_event*);
    void handle_read(int fd);
    void handle_write(int fd);
    int sockfd_;
    std::shared_ptr<Epoll> epoll_;
    Address addr_;
    bool Isrunning;
    std::map<int,Address> clientmap;

    const int LISTENQUEEN=10;
    const int EPOLLEVENTS=100;
    const int BUFFESIZE=1000;
};
#endif