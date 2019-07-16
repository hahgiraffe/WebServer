#include "Socket.h"
#include "Address.h"
#include "Log/Logging.h"
#include <sys/socket.h>
#include <unistd.h>
using namespace haha_giraffe;
const int MAXLEN=128;

Socket::~Socket(){
    if(::close(sockfd_)<0){
        LOG_ERROR<<"Socket::~Socket";
    }
}
//SO_REUSEADDR允许同一个端口上启动同一个服务器的多个实例（或者允许单个进程绑定同一个端口到多个套接字），每个实例绑定不同的本地ip即可
void Socket::setReuseAddr(bool check){
    int option = (check =true ? 1 : 0);
    if(::setsockopt(sockfd_,SOL_SOCKET,SO_REUSEADDR,&option,sizeof(option))<0){
        LOG_ERROR<<"Socket::setReuseAddr";
    }
}
    
void Socket::bindAddress(const Address& addr){
    const sockaddr_in tempaddr=addr.getaddress();
    if(::bind(sockfd_,static_cast<const sockaddr*>(static_cast<const void*>(&tempaddr)),sizeof(addr)) <0){
        LOG_ERROR<<"Socket::bindAddress";
    }
}
    
void Socket::listen(){
    if(::listen(sockfd_,MAXLEN)<0){
        LOG_ERROR<<"Socket::listen()";
    }
}
    
int Socket::accept(const Address& addr){
    socklen_t len=sizeof(addr);
    const sockaddr_in tempaddr = addr.getaddress();
    int clientfd=::accept(sockfd_,static_cast<sockaddr*>(static_cast<void*>(const_cast<sockaddr_in*>(&tempaddr))),&len);
    if(clientfd<0){
        LOG_ERROR<<"Socket::accept()";
    }
    return clientfd;
}

void Socket::shutdownWrite(){

}
    
void Socket::setTcpNoDelay(bool on){

}