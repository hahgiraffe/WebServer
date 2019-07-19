/*
 * @Description: EPOLL的封装类
 * @Author: hahagiraffe
 * @Date: 2019-07-18 11:46:47
 */
#ifndef EPOLL_H
#define EPOLL_H
#include <unistd.h>
#include <sys/epoll.h>

class Epoll{
public:
    Epoll();
    ~Epoll(){
        ::close(epoll_fd);
    }
    void epoll_add(int fd,struct epoll_event& event);
    void epoll_mod(int fd,struct epoll_event& event);
    void epoll_delete(int fd,struct epoll_event& event);
    //void Epoll_ctl(int option,int fd,struct epoll_event& event);
    int wait(struct epoll_event* event);
    int getfd() { return epoll_fd; }
private:
    int epoll_fd;
    const int FDSIZE=10;
    const int EPOLLMAXEVENTS=100;
    const int TIMEOUT=-1;
};
#endif