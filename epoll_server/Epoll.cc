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

int Epoll::wait(struct epoll_event* events){
    int eventnum=::epoll_wait(epoll_fd,events,EPOLLMAXEVENTS,TIMEOUT);
    return eventnum;
}


void Epoll::epoll_add(int fd,struct epoll_event& event){
    ::epoll_ctl(epoll_fd,EPOLL_CTL_ADD,fd,&event);
    return;
}
    
void Epoll::epoll_mod(int fd,struct epoll_event& event){
    ::epoll_ctl(epoll_fd,EPOLL_CTL_MOD,fd,&event);
    return;
}
    
void Epoll::epoll_delete(int fd,struct epoll_event& event){
    ::epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,&event);
    return;
}

// void Epoll::Epoll_ctl(int option,int fd,struct epoll_event& event){
//     ::epoll_ctl(epoll_fd,option,fd,&event);
    
//     return;
// }
