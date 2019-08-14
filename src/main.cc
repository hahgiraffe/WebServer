/*
 * @Description: 
 * @Author: hahagiraffe
 * @Date: 2019-08-12 20:38:02
 */
//测试EventLoop是否满足一个线程中只有一个的要求
#include <thread>
#include "EventLoop.h"
#include "Channel.h"
#include "Server.h"
#include "./Log/Logging.h"
#include <sys/timerfd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace haha_giraffe;
EventLoop* loopptr;
// test0
// void threadfunc(){
//     //printf("sub thread id is %d\n",pthread_self());
//     LOG_INFO<<"sub thread id is"<<(long)pthread_self();
//     loopptr->loop();
// }

// int main(){
//     //printf("main thread id is %d\n",pthread_self());
//     LOG_INFO<<"sub thread id is"<<(long)pthread_self();
//     EventLoop loop;
//     loopptr = &loop;
//     std::thread t(threadfunc);
//     t.join();
//     loop.loop();
// }

// test1 EPOLL
// void timeout(){
//     LOG_INFO<<"TIMEOUT!";
//     loopptr->quit();
// }

// int main(){
//     EventLoop loop;
//     loopptr=&loop;
//     int timerfd = ::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);//先获取一个timerfd
//     Channel channel(&loop,timerfd);
//     channel.setReadCallback(timeout);
//     channel.enableReading();//这个调用Channel::update() -> EventLoop::updateChannel() ->Epoll::updateChannel()

//     struct itimerspec howlong;
//     bzero(&howlong,sizeof(howlong));
//     howlong.it_value.tv_sec=5;//定时5秒
//     ::timerfd_settime(timerfd,0,&howlong,NULL);
//     loop.loop();
//     ::close(timerfd);
// }

// test2 Acceptor
// void Connectorfunction(int clientfd,const Address& peeraddr){
//     char host[1024];
//     const sockaddr_in addr=peeraddr.getaddress();
//     ::inet_ntop(AF_INET, &addr.sin_addr, host, sizeof host);
//     const int tempport =peeraddr.getaddress().sin_port;
//     LOG_INFO<<"Clinet fd: "<<clientfd<<" peerAddress is "<<host<<" peerport is "<<ntohs(tempport);
//     ::write(clientfd,"gooood\n",10);
    
//     //::close(clientfd);
// }

// int main(){
//     EventLoop loop;
//     //loopptr=&loop;
//     Address address(1234);
//     Acceptor acceptor(&loop,address);
//     acceptor.setNewConnectionCallback(Connectorfunction);
//     acceptor.listen();
//     loop.loop();
// }

int main(){
    EventLoop loop;
    Server httpserver(&loop,3,1234);
    httpserver.start();
    loop.loop();
}