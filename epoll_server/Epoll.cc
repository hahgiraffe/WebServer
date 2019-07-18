#include "Epoll.h"
#include "../src/Log/Logging.h"
#include <error.h>
using namespace haha_giraffe;
Epoll::Epoll()
    :epoll_fd(0)
{
    epoll_fd=epoll_create(FDSIZE);
    if(epoll_fd<0){
        LOG_ERROR<<"Epoll() epoll_create() error ,errornumber is "<<errno;
    }
    LOG_INFO<<"epollfd is "<<epoll_fd;
}

int Epoll::Epoll_wait(struct epoll_event* events){
    int eventnum=::epoll_wait(epoll_fd,events,EPOLLMAXEVENTS,-1);
    return eventnum;
}

void Epoll::Epoll_ctl(int option,int fd,struct epoll_event& event){
    ::epoll_ctl(epoll_fd,option,fd,&event);
    return;
}