/*
 * @Author: nyy imniuyuanye@sina.com
 * @Date: 2023-03-01 13:03:54
 * @LastEditors: nyy imniuyuanye@sina.com
 * @LastEditTime: 2023-03-01 17:23:29
 * @FilePath: /video_detection/src/objdetection/objdetection_manager.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include "objdetection_manager.h"
namespace objdetection{
    ObjDetection_Manager::ObjDetection_Manager(){};

    ObjDetection_Manager::~ObjDetection_Manager(){};

    int ObjDetection_Manager::Ini(const ObjDectionType& type){
        // detections_[Type_WXYOLOV56_DETECTION] = CreateDetection(Type_WXYOLOV56_DETECTION);
        // detections_[Type_FYYOLOV5_DETECTION] = CreateDetection(Type_FYYOLOV5_DETECTION);
        // detections_[Type_FYYOLOV8_DETECTION] = CreateDetection(Type_FYYOLOV8_DETECTION);  

        detections_[type] = CreateDetection(type);    
        return 1;
    }




    std::unique_ptr<objdetection::ObjDetection> ObjDetection_Manager::CreateDetection(
        const ObjDectionType& type) 
    {
        std::unique_ptr<objdetection::ObjDetection> detection_ptr(nullptr);
        switch (type) {
        case Type_FYYOLOV5_DETECTION: {
            detection_ptr.reset(new objdetection::fyApplyYolov5(false));
            break;
        }
        case Type_FYYOLOV7_DETECTION: {
            detection_ptr.reset(new objdetection::fyApplyYolov7(false));
            break;
        }
        case Type_FYYOLOV8_DETECTION: {
            detection_ptr.reset(new objdetection::fyApplyYolov8(false));
            break;
        }
        case Type_FKYOLOV8_SEG_DETECTION: {
            detection_ptr.reset(new objdetection::fyApplyYolov8Seg(false));
            break;
        }
        case Type_FKYOLOV8_POSE_DETECTION:{
            detection_ptr.reset(new objdetection::fyApplyYolov8Pose(false));
            break;
        }
        case Type_WXYOLOV56_DETECTION: {
            detection_ptr.reset(new objdetection::wxApplyYolov57seg(false));
            break;
        }
        case Type_WXYOLOV57_DETECTION: {
            detection_ptr.reset(new objdetection::wxApplyYolov57(false));
            break;
        }
        case Type_WXYOLOV5_SEG_DETECTION: {
            detection_ptr.reset(new objdetection::wxApplyYolov57seg(false));
            break;
        }
        case Type_DEPTH_ANYTHING: {
            detection_ptr.reset(new objdetection::ApplyDepthAnything(false));
            break;
        }
        default: {
            break;
        }
        } 
        return detection_ptr;
    }

    objdetection::ObjDetection* ObjDetection_Manager::GetDetection(const ObjDectionType& type){
        auto it = detections_.find(type);
        return it != detections_.end() ? it->second.get() : nullptr;
    }
}