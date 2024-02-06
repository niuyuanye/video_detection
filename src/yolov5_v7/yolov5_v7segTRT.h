/*
 * @Author: nyy imniuyuanye@sina.com
 * @Date: 2023-03-14 14:56:13
 * @LastEditors: nyy imniuyuanye@sina.com
 * @LastEditTime: 2023-03-14 15:10:20
 * @FilePath: /video_detection/src/yolov5_v7/yolov5_v7segTRT.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef YOLOV5_V5SEGTRT_H
#define YOLOV5_V5SEGTRT_H

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
class Yolov5_v7segTRT{
public:
    int InitialSeg(std::string engine_name,double NMS_THRESH, double CONF_THRESH);
    cv::Mat  DetectImg(cv::Mat img, std::vector<std::vector<Detection>> result);
    void Release();
private:
    // void doInference(IExecutionContext& context, cudaStream_t& stream, void **buffers, float* input, float* output, int batchSize);
    float* gpu_buffers[3];
	float* cpu_output_buffer1 = nullptr;
    float* cpu_output_buffer2 = nullptr;
    // IRuntime* runtime = nullptr;
    // ICudaEngine* engine = nullptr;
    // IExecutionContext* context = nullptr;
    // cudaStream_t stream;


    float kNmsThresh = 0.45f;
    float kConfThresh = 0.5f;

};
#endif //YOLOV5_V5SEGTRT_H