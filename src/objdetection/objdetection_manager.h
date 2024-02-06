/*
 * @Author: nyy imniuyuanye@sina.com
 * @Date: 2023-02-06 10:48:30
 * @LastEditors: nyy imniuyuanye@sina.com
 * @LastEditTime: 2023-03-01 17:24:15
 * @FilePath: /gb_detection/src/ProcessApply.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef OBJDETECTION_MANAGER_H
#define OBJDETECTION_MANAGER_H
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

#include "../globale.h"
#include "objdetection.h"


#include "fyApplyYolov5.h"
#include "fyApplyYolov7.h"
#include "fyApplyYolov8.h"
#include "fyApplyYolov8Seg.h"
#include "fyApplyYolov8Pose.h"


// #include "wxApplyYolov56.h"
#include "wxApplyYolov57seg.h"
#include "wxApplyYolov57.h"


#include "ApplyDepthAnything.h"

namespace objdetection{
    class ObjDetection_Manager
    {
    public:
        ObjDetection_Manager();
        ~ObjDetection_Manager();
        int Ini(const ObjDectionType& type);

    public:        
        objdetection::ObjDetection* GetDetection(const ObjDectionType& type);
        std::unique_ptr<objdetection::ObjDetection> CreateDetection(const ObjDectionType& type);
    private:
        std::map<ObjDectionType, std::unique_ptr<objdetection::ObjDetection>> detections_;
        
    };
}
#endif 