
#ifndef YOLOV5_V7TRT_H
#define YOLOV5_V7TRT_H

#include <iostream>
#include <chrono>
#include <cmath>
#include <string>
#include <future>
#include <memory>
#include <opencv2/opencv.hpp>
#include "types.h"

/////////////////////////////////////////////////////////////////////////////////////////
// 封装接口类    
class Yolov5_v7TRT{
public:
    int InitialDetcet(std::string engine_name,double NMS_THRESH, double CONF_THRESH);
    cv::Mat  DetectImg(cv::Mat img, std::vector<std::vector<Detection>> result);
    void Release();
private:
    std::unordered_map<int, std::string> labels_map;
    float* gpu_buffers[2];
    float* cpu_output_buffer = nullptr;
    


    float kNmsThresh = 0.5f;
    float kConfThresh = 0.5f;

};
#endif 