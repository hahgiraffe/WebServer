/*
 * @Description: 
 * @Author: hahagiraffe
 * @Date: 2019-08-13 18:53:15
 */
#include "Util.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
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

int readn(int fd,void* buf,int size){

}

int writen(int fd,void* buf,int size){

}