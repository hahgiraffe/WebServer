//测试EventLoop是否满足一个线程中只有一个的要求
#include <thread>
#include "../src/EventLoop.h"
#include <stdio.h>
EventLoop* loopptr;

void threadfunc(){
    printf("sub thread id is %d\n",pthread_self());
    loopptr->loop();
}

int main(){
    printf("main thread id is %d\n",pthread_self());
    EventLoop loop;
    loopptr = &loop;
    std::thread t(threadfunc);
    t.join();
    loop.loop();
}
