/*** 
 * @Author: nyy
 * @Date: 2023-03-01 21:04:34
 * @LastEditTime: 2023-03-01 22:40:06
 * @LastEditors: nyy
 * @Description: 
 * @FilePath: /video_detection/src/objdetection/fyApplyYolov5.h
 * @可以输入预定的版权声明、个性签名、空行等
 */


#ifndef PROAPPLY_H
#define PROAPPLY_H
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
class YOLOV5 : public yolo::YOLO
{
public:
	YOLOV5(const utils::InitParameter& param);
	~YOLOV5();
};


// 槽对象类3
class fyApplyYolov5 : public ObjDetection  , public Object
{
public:
    fyApplyYolov5(bool stop);
    ~fyApplyYolov5();
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
    std::shared_ptr<YOLOV5> yolo;
    std::queue<std::shared_ptr<Image_vedio>> g_imageQueue;
    // int g_bufsize = 100;
    // int target_id = 1;
    // int is_show = 0;

public:
    void eventFunc(cv::Mat img, std::string img_path,std::string image_time,int camid);
};
}


#endif //PROAPPLY_H