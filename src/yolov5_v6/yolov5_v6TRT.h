/*
 * @Author: nyy imniuyuanye@sina.com
 * @Date: 2023-02-07 09:15:02
 * @LastEditors: nyy imniuyuanye@sina.com
 * @LastEditTime: 2023-02-07 11:06:45
 * @FilePath: /gb_detection/src/yolov5_v6TRT.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef YOLOV5_V5TRT_H
#define YOLOV5_V5TRT_H

#include <iostream>
#include <chrono>
#include <cmath>
#include <string>
#include <future>
#include <memory>
#include <opencv2/opencv.hpp>



#include "yololayer.h"

// #include <log4cplus/log4cplus.h>
// #include "logger.h"
/////////////////////////////////////////////////////////////////////////////////////////
// 封装接口类    
class Yolov5_v6TRT{
public:
    int InitialDetcet(std::string engine_name,double NMS_THRESH, double CONF_THRESH);
    std::vector<std::vector<Yolo::Detection>> DetectImg(cv::Mat img, std::vector<std::vector<Yolo::Detection>> result);
    void Close();
private:
    // void doInference(IExecutionContext& context, cudaStream_t& stream, void **buffers, float* input, float* output, int batchSize);
    void* buffers[2];
private:
    const char* INPUT_BLOB_NAME = "data";
    const char* OUTPUT_BLOB_NAME = "prob"; 

    int inputIndex ;
    int outputIndex;

    uint8_t* img_host = nullptr;
    uint8_t* img_device = nullptr;

    // IRuntime* runtime = nullptr;
    // ICudaEngine* engine = nullptr;
    // IExecutionContext* context = nullptr;
    // cudaStream_t stream;

    double NMS_THRESH =0.4;
    double CONF_THRESH =0.5;

};
#endif //YOLOV5_V5TRT_H