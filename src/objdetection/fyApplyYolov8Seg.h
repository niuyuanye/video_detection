/*** 
 * @Author: nyy
 * @Date: 2023-03-01 21:04:34
 * @LastEditTime: 2024-01-23 16:17:29
 * @LastEditors: nyy
 * @Description: 
 * @FilePath: /irs-orin/src/objdetection/fyApplyYolov8Seg.h
 * @可以输入预定的版权声明、个性签名、空行等
 */

#ifndef FYAPPLYYOLOV8SEG_H
#define FYAPPLYYOLOV8SEG_H
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
// #include "../threadPtr.h"
#include "objdetection.h"
#include "../funobject.h"


#include"../../tensorRT/utils/yolo.h"
#include"../yolov8-seg/yolov8_seg.h"


// 槽对象类3
// class ProApplyYolov8 : public ThreadPtr ,public Object
namespace objdetection{
    class fyApplyYolov8Seg : public ObjDetection ,public Object
    {
    public:
        fyApplyYolov8Seg(bool stop);
        ~fyApplyYolov8Seg();
        void Stop();
        int iniModel();
        int yolov8SegDect(cv::Mat image);
        std::vector<cv::Point> GetRailRegionResult(){
            return mask_rail;
        }

    public:        // 应该让线程回调，不应该自己调用，因此需要将其设置为private
        void Run(); // 虚函数的重写可以改变成员函数的权限  
        void setParameters(utils::InitParameter& initParameters);
    private:
        pthread_mutex_t lock_p;
        bool stop_ = true;
        int captureHK_Peroid = 200;

        
        utils::InitParameter param;
        std::shared_ptr<YOLOv8Seg> yoloseg;
        std::vector<cv::Point> mask_rail;
        std::queue<std::shared_ptr<Image_vedio>> g_imageQueue;
        
        int g_bufsize = 100;
        int target_id = 1;
        int is_show = 0;
        int is_save = 0;
        int time_Peroid = 200;
    public:
        void eventFunc(cv::Mat img, std::string img_path,std::string image_time,int camid);

    };
}
#endif //FYAPPLYYOLOV8SEG_H