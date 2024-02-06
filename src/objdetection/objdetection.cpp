/*
 * @Author: nyy imniuyuanye@sina.com
 * @Date: 2023-02-27 15:14:11
 * @LastEditors: nyy imniuyuanye@sina.com
 * @LastEditTime: 2023-02-27 16:14:08
 * @FilePath: /video_detection/src/yolodetection/objdetection.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include <unistd.h>
#include "objdetection.h"

#include "../../log4cplus/mylogger.h"

namespace objdetection{
// int ThreadPtr::a = 0;
ObjDetection::ObjDetection() : autoDelete_(false) // 默认是false
{
    std::cout << "ObjDetection ThreadPtr Constructor" << std::endl;
}
ObjDetection::~ObjDetection()
{
    std::cout << "ObjDetection ThreadPtr Destructor" << std::endl;
}

void ObjDetection::Start()
{
    pthread_create(&thread_Id_, nullptr, ThreadPtr_routine, this); // 需要连接线程库
    pthread_detach(this->thread_Id_);
    // 该函数调用myThread_routine()
    // 此处的第四个参数是派生类的指针，也是第三个参数-myThread_routine函数的参数
    // Run是普通的成员函数，隐含的第一个参数是myThread*，也就是this指针，调用的时候是thiscall约定
}

void ObjDetection::Join()
{
    pthread_join(thread_Id_, nullptr);    
}

void *ObjDetection::ThreadPtr_routine(void *arg) // 类的静态函数
{
    // a++; // 类的静态成员函数可以直接访问类的静态成员变量

    // 应当在此处调用Run方法
    // 但是静态成员函数，不能调用非静态的成员函数，因为没有this指针
    // 基类指针，指向派生类对象
    ObjDetection *thread = static_cast<ObjDetection *>(arg); // arg是this指针，作为参数传递进来，将派生类指针转换为基类指针
    // static_cast将子类指针转为父类指针是安全的，但是父类指针转为子类是不安全的，因为没有动态类型检查
    thread->Run(); // 利用虚函数实现多态，基类指针指向派生类对象    
    LOG_INFO("线程结束！！！ ");
    
    // 回调Run方法，是派生类的Run方法
    // 静态成员函数不能访问非静态的成员（包括函数和变量），因此必须使用指针访问具体的对象成员
    if (thread->autoDelete_) // 在线程结束后，自动销毁线程对象，保证线程与线程对象的生命周期一致
    {
        LOG_INFO("线程结束 delete thread success ！！！");
        delete thread;
        thread = nullptr;
    }
    return nullptr;
}

void ObjDetection::SetAutoDelete(bool arg)
{
    autoDelete_ = arg;
}

}
