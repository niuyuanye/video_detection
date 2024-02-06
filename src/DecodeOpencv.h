/*** 
 * @Author: nyy
 * @Date: 2023-03-01 22:56:48
 * @LastEditTime: 2023-03-02 00:24:55
 * @LastEditors: nyy
 * @Description: 
 * @FilePath: /video_detection/src/DecodeOpencv.h
 * @可以输入预定的版权声明、个性签名、空行等
 */

#include <iostream>
#include <memory>
#include <chrono>
#include <cmath>
#include <unistd.h>
#include <string>
#include <vector>

#include <iostream>


#include<opencv2/core/core.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>

#include "threadPtr.h"
#include "funobject.h"

#include "./objdetection/objdetection.h"
class DecodeOpencv : public ThreadPtr ,public Object
{
public:
    DecodeOpencv(bool stop);
    ~DecodeOpencv();
    void Stop();
    int iniOpencv();


private:        // 应该让线程回调，不应该自己调用，因此需要将其设置为private
    void Run(); // 虚函数的重写可以改变成员函数的权限    
private:
    pthread_mutex_t lock_p;
    bool stop_ = true;    

    std::string ip = "127.0.0.1";
    std::string name = "admin";
    std::string pass = "sw123456";
    int width = 960;
    int height = 720;
    
    int camera_ch = 1;
    int time_Peroid = 300;
    std::string video_path="error.mp4";
    std::string img_dir = "errorpath";
    int Type = 1;
    

private:
    objdetection::ObjDetection* m_recver;
    std::function<void(objdetection::ObjDetection*,cv::Mat, std::string,std::string,int)>  m_callFun;
public:
    void connectMessage(objdetection::ObjDetection* recver, std::function<void(objdetection::ObjDetection*,cv::Mat, std::string, std::string,int )> slot)
    {
        m_recver = recver;
        m_callFun = slot;
    }
    void EventMessage(cv::Mat img,std::string img_path,std::string image_time,int camid);

};
