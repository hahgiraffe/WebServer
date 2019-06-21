#include "EventLoop.h"

EventLoop::EventLoop():
                    loop_(false),
                    threadId_(static_cast<pid_t>(pthread_self()))
{
    
}
    
EventLoop::~EventLoop(){

}
    
void EventLoop::loop(){

}