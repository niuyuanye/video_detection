
#ifndef WXAPPLYYOLOV57_H
#define WXAPPLYYOLOV57_H
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

#include "../yolov5_v7/yolov5_v7TRT.h"



namespace objdetection{
    class wxApplyYolov57 : public ObjDetection ,public Object
    {
    public:
        wxApplyYolov57(bool stop);
        ~wxApplyYolov57();
        void Stop();
        int iniModel();

    private:        // 应该让线程回调，不应该自己调用，因此需要将其设置为private
        void Run(); // 虚函数的重写可以改变成员函数的权限
        cv::Rect Get_rect(cv::Mat& img, float bbox[4]);  
    private:
        pthread_mutex_t lock_p;
        bool stop_ = true;
        // int time_Peroid = 200;
        

        std::shared_ptr<Yolov5_v7TRT> yolo;
        std::queue<std::shared_ptr<Image_vedio>> g_imageQueue;

    public:
        void eventFunc(cv::Mat img, std::string img_path,std::string image_time,int camid);
    };
}
#endif 