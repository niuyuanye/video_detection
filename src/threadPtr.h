#ifndef THREADPTR_H
#define THREADPTR_H

#include <iomanip>
#include <iostream>
#include <memory>
#include <chrono>
#include <cmath>
#include <unistd.h>
#include <string>
#include <string.h>
#include <vector>

#include <pthread.h>

class ThreadPtr
{
public:
    ThreadPtr();
    virtual ~ThreadPtr();
     // virtual void Start();
    void Start(); // Start和Join不必是虚的
    //  virtual void Join();
    void Join();
   // static void *myThread_routine(void *arg); // static函数不隐含this指针 也应该是private

    void SetAutoDelete(bool arg);
    // static int a; // 静态成员变量，类内声明，类外初始化

private:
    virtual void Run() = 0;
    // 如果调用，必须对其进行实现
    // 纯虚函数，基类不需要实现
    // 如果Run不是private的，可以直接调用t.Run，运行结果一样，但是含义不一样
    static void *ThreadPtr_routine(void *arg); 
    pthread_t thread_Id_;
    bool autoDelete_; // 增加一个属性，是否自动销毁该对象

    // int myThread::a = 0;
};
// 





// class myRealThread : public ThreadPtr   
// {
// public:
//     myRealThread(bool stop = false) : stop_(stop)
//     {
//         std::cout << "myRealThread Constructor" << std::endl;
//     }
//     ~myRealThread()
//     {
//         std::cout << "myRealThread Destructor" << std::endl;
//     }
//     void Stop(){
//         this->stop_ = true;
//     }

// private:       // 应该让线程回调，不应该自己调用，因此需要将其设置为private
//     void Run() // 虚函数的重写可以改变成员函数的权限
//     {
//         while (!stop_)
//         {
//             std::cout << "this is a test" << std::endl;
//             sleep(1); // 睡眠1s
//         }
//     }
// private:
//     pthread_mutex_t lock_p;
//     bool stop_ = true;
// };


#endif // THREADPTR_H