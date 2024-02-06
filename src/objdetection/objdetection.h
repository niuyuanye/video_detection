/*
 * @Author: nyy imniuyuanye@sina.com
 * @Date: 2023-02-27 15:14:11
 * @LastEditors: nyy imniuyuanye@sina.com
 * @LastEditTime: 2023-03-02 15:56:07
 * @FilePath: /video_detection/src/objdetection/objdetection.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#ifndef OBJDETECTION_H
#define OBJDETECTION_H

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

#include<opencv2/core/core.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
namespace objdetection{
class ObjDetection
{
public:
    ObjDetection() ;
    virtual ~ObjDetection() ;
    // virtual bool ObjDetection() = 0;
    // virtual void Start();
    void Start(); // Start和Join不必是虚的
    //  virtual void Join();
    void Join();
    // static void *myThread_routine(void *arg); // static函数不隐含this指针 也应该是private
    void SetAutoDelete(bool arg);
    // static int a; // 静态成员变量，类内声明，类外初始化

public:
    virtual void Run() = 0;
    virtual int iniModel() = 0;
    virtual void eventFunc(cv::Mat img, std::string img_path,std::string image_time,int camid) = 0;
    // 如果调用，必须对其进行实现
    // 纯虚函数，基类不需要实现
    // 如果Run不是private的，可以直接调用t.Run，运行结果一样，但是含义不一样
    static void *ThreadPtr_routine(void *arg); 
    pthread_t thread_Id_;
    bool autoDelete_; // 增加一个属性，是否自动销毁该对象
    // int myThread::a = 0;
protected:
    int g_bufsize = 100;
    int target_id = 1;
    int is_show = 0;
    int is_save = 0;
    int time_Peroid = 200;
};
}
#endif // OBJDETECTION_H