/*
 * @Author: nyy imniuyuanye@sina.com
 * @Date: 2023-02-06 10:48:30
 * @LastEditors: nyy imniuyuanye@sina.com
 * @LastEditTime: 2023-03-01 16:31:35
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

extern "C" {
/* Include ffmpeg header file */
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>

#include <libavutil/imgutils.h>
}



#include<opencv2/core/core.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>

#include "threadPtr.h"

// #include "ProcessApply.h"
#include "funobject.h"

// #include "./objdetection/ProApplyYolov8.h"
#include "./objdetection/objdetection.h"
class ffDecodecpu : public ThreadPtr ,public Object
{
public:
    ffDecodecpu(bool stop);
    ~ffDecodecpu();
    void Stop();
    int init();
    int iniffDecodecpu(std::string ip, std::string name,std::string pass,int camera_id);
    void release(); //just using for init failure.

private:        // 应该让线程回调，不应该自己调用，因此需要将其设置为private
    void Run(); // 虚函数的重写可以改变成员函数的权限    
    cv::Mat avframeToCvmat(const AVFrame * frame,AVPixelFormat pixFormat);
private:
    pthread_mutex_t lock_p;
    bool stop_ = true;
    

    std::string ip = "127.0.0.1";
    std::string name = "admin";
    std::string pass = "sw123456";
    int camera_id = 1;
    int captureHK_Peroid = 300;

private:
    std::string url = "oo";
    int camid = -1;
    AVFormatContext *ifmt_ctx = NULL;
    AVDictionary* avdic = NULL;
    int video_index = -1;
    AVCodecContext * codec_ctx = NULL;
    AVCodec *codec = NULL;

    uint8_t *buffer = NULL;
    int numBytes = -1;
    AVFrame *rawframe = NULL;
    AVFrame *bgrframe = NULL;
    struct SwsContext *img_cvrt_ctx = NULL;

    cv::Mat curFrame;
    std::string curFrameBase64;
    int width = 960;
    int height = 720;
  
    
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

// private:
//     objdetection::ProApplyYolov8* m_recver;
//     std::function<void(objdetection::ProApplyYolov8*,cv::Mat, std::string,std::string,int)>  m_callFun;
//     // std::function<void(ProcessApply*,int)>  m_callFun;
// public:
//     void connectMessage(objdetection::ProApplyYolov8* recver , std::function<void(objdetection::ProApplyYolov8*,cv::Mat, std::string, std::string,int )> slot)
//     // void connectMessage(ProcessApply* recver, std::function<void(ProcessApply*,int )> slot)
//     {
//         m_recver = recver;
//         m_callFun = slot;
//     }
//     void EventMessage(cv::Mat img,std::string img_path,std::string image_time,int camid);
    
};
