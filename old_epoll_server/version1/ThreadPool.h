/*
 * @Description: 线程池的实现
 * @Author: hahagiraffe
 * @Date: 2019-08-03 15:41:57
 */
#ifndef THREAD_POLL_H
#define THREAD_POLL_H
#include <deque>
#include <string>
#include <pthread.h>
#include <stdlib.h>
#include <functional>
//task任务类，是一个抽象类，每一个job都是继承他
class Task
{
public:
    Task(std::function<void()> arg=NULL,const std::string taskname="")
        :_arg(arg),taskname(taskname){ }
    virtual ~Task(){ }
    void setarg(std::function<void()> arg){
        _arg=arg;
    }
    virtual int run()=0;//纯虚函数
protected:
    std::function<void()> _arg;
    std::string taskname;
};

//线程池类
class ThreadPool
{
public:
    ThreadPool(int threadnum=10);
    ~ThreadPool();
public:
    int addTask(Task *task);
    void stop();
    int size();
    Task* take();
    int createThreads();//创建线程函数
    static void* threadfunc(void *threaddata);//线程执行函数

private:
    //拷贝构造和拷贝赋值运算符都定义为private为了不允许编译器默认生成
    //只能在类内使用
    ThreadPool(const ThreadPool&);
    ThreadPool& operator=(const ThreadPool&);
private:
    volatile bool state;//保存线程池状态
    int threadnumbers;
    pthread_t* threads;
    pthread_mutex_t mutex;//互斥量
    pthread_cond_t condition;//条件变量
    std::deque<Task*> taskQueue;//任务队列

};

#endif