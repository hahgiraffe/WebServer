#include "ThreadPool.h"
#include <stdio.h>
ThreadPool::ThreadPool(int threadnum){
    state=true;
    threadnumbers=threadnum;
    createThreads();
}

ThreadPool::~ThreadPool(){
    stop();
    for(std::deque<Task*>::iterator it=taskQueue.begin();it!=taskQueue.end();it++){
        delete *it;
    }
    taskQueue.clear();
}

int ThreadPool::createThreads(){
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&condition,NULL);
    threads=(pthread_t*)malloc(sizeof(pthread_t)*threadnumbers);
    for(int i=0;i<threadnumbers;i++){
        pthread_create(&threads[i],NULL,threadfunc,this);
    }
    return 0;
}
int ThreadPool::addTask(Task* task){
    pthread_mutex_lock(&mutex);
    taskQueue.push_back(task);
    int size=taskQueue.size();
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&condition);//每插入一个任务，即条件变量满足，唤醒一个等待该条件的线程
    return size;
}

void ThreadPool::stop(){
    if(!state){//如果线程池已经关闭则返回
        return;
    }
    state=false;
    pthread_cond_broadcast(&condition);//唤醒所有线程
    for(int i=0;i<threadnumbers;i++){
        pthread_join(threads[i],NULL);//当前线程等待所有线程退出
    }
    free(threads);
    threads=NULL;
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condition);
}


int ThreadPool::size(){
    pthread_mutex_lock(&mutex);
    int size=taskQueue.size();
    pthread_mutex_unlock(&mutex);
    return size;
}

Task* ThreadPool::take(){
    Task* task=NULL;
    while(!task){
        pthread_mutex_lock(&mutex);
        while(taskQueue.empty() && state){
            pthread_cond_wait(&condition,&mutex);//阻塞，等待条件变量为真
        }
        if(!state){//线程池关闭
            pthread_mutex_unlock(&mutex);
            break;
        }
        else if(taskQueue.empty()){
            pthread_mutex_unlock(&mutex);
            break;
        }
        task=taskQueue.front();
        taskQueue.pop_front();
        pthread_mutex_unlock(&mutex);
    }
    return task;

}

void* ThreadPool::threadfunc(void *arg){//每个线程执行的函数
    //printf("111111\n");
    pthread_t pid=pthread_self();
    ThreadPool* pool=static_cast<ThreadPool*> (arg);//把线程池本身当做参数传过来了
    while(pool->state){
        Task *task=pool->take();//取出任务,在任务队列为空的时候线程组阻塞在这
        if(!task){
            printf("thread %lu not take task\n",pid);
            break;
        }
        task->run();//这里是运行自己定义的任务函数,也就是子类的run
    }
    return 0;
}