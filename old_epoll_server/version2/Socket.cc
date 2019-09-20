/*
 * @Description: 
 * @Author: hahagiraffe
 * @Date: 2019-08-15 16:48:58
 */
#include "Socket.h"
#include "./Log/Logging.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
using namespace haha_giraffe;

//创建监听文件描述符,绑定地址并开启监听
int createlistenfd(int port){
    int listenfd;
    if((listenfd=socket(AF_INET,SOCK_STREAM,0))==-1){
        return -1;
    }
    //在socket和bind之间消除地址重用错误
    int reuse=1;
    if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse))==-1){
        return -1;
    }
    struct sockaddr_in address;
    bzero(&address,sizeof(address));
    address.sin_family=AF_INET;
    address.sin_addr.s_addr=htonl(INADDR_ANY);
    address.sin_port=htons(port);
    if(bind(listenfd,(struct sockaddr*)&address,sizeof(address))==-1){
        return -1;
    }
    if(listen(listenfd,1024)==-1){
        return -1;
    }
    return listenfd;
}

//设置描述符为非阻塞
void setlistenfdnonblock(int fd){
    int flag=fcntl(fd,F_GETFL,0);
    if(flag==-1){
        exit(1);
    }
    flag |= O_NONBLOCK;
    if(fcntl(fd,F_SETFL,flag)==-1){
        exit(1);
    }
}

void setnodelay(int fd){
    int enable = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void*)&enable, sizeof(enable));
}
int readn(int fd,void* buf,int size){
    size_t nleft = size;
    ssize_t nread = 0;
    ssize_t readSum = 0;
    char *ptr = (char*)buf;
    while (nleft > 0)
    {
        if ((nread = read(fd, ptr, nleft)) < 0)
        {
            if (errno == EINTR)
                nread = 0;
            else if (errno == EAGAIN)
            {
                return readSum;
            }
            else
            {
                return -1;
            }  
        }
        else if (nread == 0)
            break;
        readSum += nread;
        nleft -= nread;
        ptr += nread;
    }
    return readSum;
}

int writen(int fd,void* buf,int size){
    size_t nleft = size;
    ssize_t nwritten = 0;
    ssize_t writeSum = 0;
    char *ptr = (char*)buf;
    while (nleft > 0)
    {
        if ((nwritten = write(fd, ptr, nleft)) <= 0)
        {
            if (nwritten < 0)
            {
                if (errno == EINTR)
                {
                    nwritten = 0;
                    continue;
                }
                else if (errno == EAGAIN)
                {
                    return writeSum;
                }
                else
                    return -1;
            }
        }
        writeSum += nwritten;
        nleft -= nwritten;
        ptr += nwritten;
    }
    return writeSum;
}

ssize_t readn(int fd, std::string &inBuffer, bool &zero)
{
    ssize_t nread = 0;
    ssize_t readSum = 0;
    while (true)
    {
        char buff[MAX_BUFF];
        if ((nread = read(fd, buff, MAX_BUFF)) < 0)
        {
            if (errno == EINTR)
                continue;
            else if (errno == EAGAIN)
            {
                return readSum;
            }  
            else
            {
                perror("read error");
                return -1;
            }
        }
        else if (nread == 0)
        {
            //printf("redsum = %d\n", readSum);
            zero = true;
            break;
        }
        //printf("before inBuffer.size() = %d\n", inBuffer.size());
        //printf("nread = %d\n", nread);
        readSum += nread;
        //buff += nread;
        inBuffer += std::string(buff, buff + nread);
        //printf("after inBuffer.size() = %d\n", inBuffer.size());
    }
    return readSum;
}

int writen(int fd,string& buf,int size){
    size_t nleft = size;
    ssize_t nwritten = 0;
    ssize_t writeSum = 0;
    const char *ptr = buf.c_str();
    while (nleft > 0)
    {
        if ((nwritten = write(fd, ptr, nleft)) <= 0)
        {
            if (nwritten < 0)
            {
                if (errno == EINTR)
                {
                    nwritten = 0;
                    continue;
                }
                else if (errno == EAGAIN)
                    break;
                else
                    return -1;
            }
        }
        writeSum += nwritten;
        nleft -= nwritten;
        ptr += nwritten;
    }
    if (writeSum == static_cast<int>(buf.size()))
        buf.clear();
    else
        buf = buf.substr(writeSum);
    return writeSum;
}

void handle_for_sigpipe()
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if(sigaction(SIGPIPE, &sa, NULL))
        return;
}