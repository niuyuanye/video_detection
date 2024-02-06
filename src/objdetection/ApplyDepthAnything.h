/*** 
 * @Author: nyy
 * @Date: 2023-03-01 21:04:34
 * @LastEditTime: 2023-03-01 22:40:30
 * @LastEditors: nyy
 * @Description: 
 * @FilePath: /video_detection/src/objdetection/fyApplyYolov8.h
 * @可以输入预定的版权声明、个性签名、空行等
 */

#ifndef APPLYDEPTHANYTHING_H
#define APPLYDEPTHANYTHING_H
#include <iomanip>
#include <iostream>
#include <memory>
#include <chrono>
#include <unistd.h>
#include <string>
#include <string.h>
#include <vector>
#include <queue>
#include <tuple>
#include <algorithm>
#include <cmath>

#include <pthread.h>
#include <functional>
// #include "json/json.h"

#include<opencv2/core/core.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>


#include "../globale.h"
// #include "threadPtr.h"
#include "objdetection.h"
#include "../funobject.h"

#include"../DepthAnything/depth_anything.h"
// #include"yolov8.h"
// #include"../../tensorRT/utils/yolo.h"



// 槽对象类3
// class ProApplyYolov8 : public ThreadPtr ,public Object
namespace objdetection{

    class ApplyDepthAnything : public ObjDetection ,public Object
    {
    public:
        ApplyDepthAnything(bool stop);
        ~ApplyDepthAnything();
        void Stop();
        int iniModel();

    public:        // 应该让线程回调，不应该自己调用，因此需要将其设置为private
        void Run(); // 虚函数的重写可以改变成员函数的权限  
    private:
        pthread_mutex_t lock_p;
        bool stop_ = true;
        int captureHK_Peroid = 200;

        
        std::shared_ptr<DepthAnything> depth_model;
        std::queue<std::shared_ptr<Image_vedio>> g_imageQueue;
        // int g_bufsize = 100;
        // int target_id = 1;
        // int is_show = 0;
    


    public:
        void eventFunc(cv::Mat img, std::string img_path,std::string image_time,int camid);
    };
}
#endif //APPLYDEPTHANYTHING_H