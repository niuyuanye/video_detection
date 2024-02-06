/*
 * @Author: nyy imniuyuanye@sina.com
 * @Date: 2023-02-06 10:48:30
 * @LastEditors: nyy imniuyuanye@sina.com
 * @LastEditTime: 2023-02-10 15:49:16
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

#include "../threadPtr.h"
#include "../ProcessApply.h"
#include "../funobject.h"
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
private:
    pthread_mutex_t lock_p;
    pthread_mutex_t lock;
    bool stop_ = true;
    

    std::string ip = "127.0.0.1";
    std::string name = "admin";
    std::string pass = "sw123456";
    int camera_id = 1;
    int captureHK_Peroid = 300;

private:
  std::string url = "12777777777";
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
  
    
private:
    ProcessApply* m_recver;
    std::function<void(ProcessApply*,cv::Mat, std::string,std::string,int)>  m_callFun;
    // std::function<void(ProcessApply*,int)>  m_callFun;
public:
    void connectMessage(ProcessApply* recver, std::function<void(ProcessApply*,cv::Mat, std::string, std::string,int )> slot)
    // void connectMessage(ProcessApply* recver, std::function<void(ProcessApply*,int )> slot)
    {
        m_recver = recver;
        m_callFun = slot;
    }
    void EventMessage(cv::Mat img,std::string img_path,std::string image_time,int camid)
    // void EventMessage(int camid)
    {
        // m_recver->m_sender = std::bind(&captureHK::self, this);
        m_callFun(m_recver,img, img_path, image_time, camid);
        // m_callFun(m_recver,camid);
        // m_recver->m_sender = NULL;
        
        // if(m_callFun.operator bool()){
        //     m_callFun(m_recver,camid);
        //     m_recver->m_sender = NULL;
        // }else{
        //     std::cout<<"fs callable object not set, not callable"<<std::endl;
        // }  
        
    }
};
