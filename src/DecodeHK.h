/*
 * @Author: nyy imniuyuanye@sina.com
 * @Date: 2023-02-06 10:48:30
 * @LastEditors: nyy imniuyuanye@sina.com
 * @LastEditTime: 2023-03-28 10:51:30
 * @FilePath: /gb_detection/src/captureHK.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <stdio.h>
#include <unistd.h>
#include <chrono>
#include <errno.h>
#include "HCNetSDK.h"

#include <PlayM4.h>
#include <plaympeg4.h>
#include <iostream>

#include<opencv2/core/core.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>

#include "threadPtr.h"
// #include "ProcessApply.h"
#include "funobject.h"

#include "./objdetection/objdetection.h"

class DecodeHK : public ThreadPtr ,public Object
{
public:
    DecodeHK(bool stop);
    ~DecodeHK();
    void Stop();
    int iniDecodeHK(std::string ip, std::string name,std::string pass,int camera_ch);

private:        // 应该让线程回调，不应该自己调用，因此需要将其设置为private
    void Run(); // 虚函数的重写可以改变成员函数的权限    
    static void CALLBACK g_RealDataCallBack_V30(LONG lPlayHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* pUser);
    //数据解码回调函数，
    // void CALLBACK DecCBFun(long nPort, char* pBuf, long nSize, FRAME_INFO* pFrameInfo, long nUser, long nReserved2)
    static void CALLBACK DecCBFun(LONG nPort, char *pBuf, LONG nSize, FRAME_INFO *pFrameInfo, void* pUser, LONG nReserved2);
    static void EventMsg(cv::Mat img,std::string img_path,std::string image_time,int camid,void* pUser); 

        
private:
    pthread_mutex_t lock_p;
    bool stop_ = true;
    

    std::string ip = "127.0.0.1";
    std::string name = "admin";
    std::string pass = "sw123456";
    int camera_ch = 1;
    int captureHK_Peroid = 300;
    NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = {0};
    int lUserID =-1;
    int nPort = -1;
    std::chrono::system_clock::time_point start;

    int width = 960;
    int height = 720;


// private:
//     ProcessApply* m_recver;
//     std::function<void(ProcessApply*,cv::Mat, std::string,std::string,int)>  m_callFun;
// public:
//     void funMessage(ProcessApply* recver, std::function<void(ProcessApply*,cv::Mat, std::string, std::string,int )> slot)
//     {
//         m_recver = recver;
//         m_callFun = slot;
//     }
//     void EventMessage(cv::Mat img,std::string img_path,std::string image_time,int camid)
//     {
//         // m_recver->m_sender = std::bind(&DecodeHK::self, this);
//         if(m_callFun.operator bool()){
//             m_callFun(m_recver,img, img_path, image_time, camid);
//             // m_recver->m_sender = NULL;
//         }else{
//             std::cout<<"fs callable object not set, not callable"<<std::endl;
//         }        
//     }

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
