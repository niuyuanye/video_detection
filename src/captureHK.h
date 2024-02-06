/*
 * @Author: nyy imniuyuanye@sina.com
 * @Date: 2023-02-06 10:48:30
 * @LastEditors: nyy imniuyuanye@sina.com
 * @LastEditTime: 2023-03-01 17:42:48
 * @FilePath: /gb_detection/src/captureHK.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <iostream>
#include <memory>
#include <chrono>
#include <cmath>
#include <unistd.h>
#include <string>
#include <vector>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/in.h>

#include <errno.h>
#include "HCNetSDK.h"

#include <PlayM4.h>
#include <plaympeg4.h>
#include <iostream>


#include<opencv2/core/core.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>

#include "threadPtr.h"
#include "funobject.h"

#include "./objdetection/objdetection.h"
class captureHK : public ThreadPtr ,public Object
{
public:
    captureHK(bool stop);
    ~captureHK();
    void Stop();
    int iniCaptureHK(std::string ip, std::string name,std::string pass,int camera_ch);

private:        // 应该让线程回调，不应该自己调用，因此需要将其设置为private
    void Run(); // 虚函数的重写可以改变成员函数的权限    
private:
    pthread_mutex_t lock_p;
    bool stop_ = true;    

    std::string ip = "127.0.0.1";
    std::string name = "admin";
    std::string pass = "sw123456";
    int camera_ch = 1;
    int captureHK_Peroid = 300;
    NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = {0};
    int lUserID;
    

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
