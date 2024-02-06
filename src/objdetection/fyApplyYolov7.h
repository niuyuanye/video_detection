/*** 
 * @Author: nyy
 * @Date: 2023-03-01 21:36:05
 * @LastEditTime: 2023-03-01 22:39:55
 * @LastEditors: nyy
 * @Description: 
 * @FilePath: /video_detection/src/objdetection/fyApplyYolov7.h
 * @可以输入预定的版权声明、个性签名、空行等
 */

#ifndef FYAPPLYYOLOV7_H
#define FYAPPLYYOLOV7_H
#include <iomanip>
#include <iostream>
#include <memory>
#include <chrono>
#include <cmath>
#include <unistd.h>
#include <string>
#include <string.h>
#include <vector>
#include <queue>

#include <pthread.h>

#include <functional>
#include "json/json.h"

#include<opencv2/core/core.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>


#include "../globale.h"
#include "objdetection.h"
#include "../funobject.h"

#include"../../tensorRT/utils/yolo.h"

namespace objdetection{
class YOLOV7 : public yolo::YOLO
{
    public:
        YOLOV7(const utils::InitParameter& param);
        ~YOLOV7();
};


class fyApplyYolov7 : public ObjDetection  , public Object
{
public:
    fyApplyYolov7(bool stop);
    ~fyApplyYolov7();
    void Stop();
    int iniModel();

public:        // 应该让线程回调，不应该自己调用，因此需要将其设置为private
    void Run(); // 虚函数的重写可以改变成员函数的权限  
    void setParameters(utils::InitParameter& initParameters);
private:
    pthread_mutex_t lock_p;
    bool stop_ = true;
    int captureHK_Peroid = 200;

    
    utils::InitParameter param;
    std::shared_ptr<YOLOV7> yolo;
    std::queue<std::shared_ptr<Image_vedio>> g_imageQueue;

public:
    void eventFunc(cv::Mat img, std::string img_path,std::string image_time,int camid);
};
}


#endif 