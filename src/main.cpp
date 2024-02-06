/*
 * @Author: nyy imniuyuanye@sina.com
 * @Date: 2023-02-06 09:15:54
 * @LastEditors: nyy imniuyuanye@sina.com
 * @LastEditTime: 2023-04-13 19:48:43
 * @FilePath: /gb_detection/src/main.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */



#include <pthread.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>



// #include "../log4cplus/logger.h"
#include "DecodeOpencv.h"
#include "captureHK.h"
#include "DecodeHK.h"
#include "ffDecodecpu.h"

#include "./objdetection/objdetection_manager.h"
// #include "./objdetection/ProApply.h"
// #include "ProApplyYolov8.h"
// #include "./objdetection/ProApplyYolov8.h"

#include "../log4cplus/mylogger.h"


std::string ADDRESS = "192.168.43.182";
int Server_Dis_PORT = 6000;
int Blind_PORT = 5000;

int debug_log =1;


int main() {       
    if(debug_log == 1){
        initLogger(true);   
    }else{
        initLogger(false);
    }
    LOG_INFO("\n \n Software START !!!\n");   

    cameraInfo cam_info;
    std::vector<cameraInfo>cam_info_vect;
    std::vector<std::shared_ptr<captureHK> >cam_captureHks;
    std::vector<std::shared_ptr<DecodeHK> >cam_DecodeHKs;
    std::vector<std::shared_ptr<ffDecodecpu> >cam_ffDecodecpus;
    std::vector<std::shared_ptr<DecodeOpencv> >cam_ffDecodeOpencvs;
    std::shared_ptr<objdetection::ObjDetection_Manager> ObjDetection_Manager_ptr(new objdetection::ObjDetection_Manager());

    std::string inifile = "./properties.ini";
    
    long AIType_tmp = ini_getl("switch", "AIType", 1, inifile.c_str());   
    LOG_INFO("@@@@@@@@@@@@@@@ AIType:"+std::to_string(AIType_tmp));     
    int AIType = AIType_tmp;

    long DecodeType_tmp = ini_getl("switch", "DecodeType", 1, inifile.c_str());   
    LOG_INFO("@@@@@@@@@@@@@@@ DecodeType_tmp:"+std::to_string(DecodeType_tmp)); 
    int DecodeType = DecodeType_tmp;

    long ch_tmp = ini_getl("camera", "ch", 1, inifile.c_str());   
    LOG_INFO("@@@@@@@@@@@@@@@ ch_tmp:"+std::to_string(ch_tmp)); 
    int camera_ch = ch_tmp;


    /* 模型选择加载及初始化 2023.03.28 nyy
    */
    char str_ip[100];char str_name[100];char str_pass[100];
    char section[50];
    objdetection::ObjDetection* objdetection_ptr = nullptr;  
    ObjDetection_Manager_ptr->Ini((ObjDectionType)AIType); 
    objdetection_ptr = ObjDetection_Manager_ptr->GetDetection((ObjDectionType)AIType);  

    int status = objdetection_ptr->iniModel();
    if(status <0){
        LOG_INFO("ProApply 模型初始化失败！");
        return -1;
    }
    else{
        objdetection_ptr->SetAutoDelete(true); // 保证了线程对象与线程的生命周期是一致的
        objdetection_ptr->Start();         
    }
    std::function<void(objdetection::ObjDetection*,cv::Mat, std::string, std::string, int)> slot_event = &objdetection::ObjDetection::eventFunc;
    
    /* 视频解码方式选择及初始化 2023.03.28 nyy
    */
    if(DecodeType == 2 || DecodeType == 3 || DecodeType == 4){
        for(int i =0; i<camera_ch;i++){
            int num_camera = i+1;

            std::string ip = "ip_"+std::to_string(num_camera);         
            long n = ini_gets("camera", ip.c_str(), "192.168.110.220", str_ip, sizearray(str_ip), const_cast<char *>(inifile.c_str()));
            if (n >= 0) LOG_INFO(ip<<":"<<str_ip);
            cam_info.str_ip=str_ip;

            std::string name = "name_"+std::to_string(num_camera);  
            n = ini_gets("camera", name.c_str(), "admin", str_name, sizearray(str_name), const_cast<char *>(inifile.c_str()));
            if (n >= 0) LOG_INFO(name<<":"<<str_name);
            cam_info.str_name=str_name;

            std::string pass = "pass_"+std::to_string(num_camera);
            n = ini_gets("camera", pass.c_str(), "sw123", str_pass, sizearray(str_pass), const_cast<char *>(inifile.c_str()));
            if (n >= 0) LOG_INFO(pass<<":"<<str_pass);
            cam_info.str_pass=str_pass;

            cam_info_vect.push_back(cam_info);

            LOG_INFO("初始化相机初 第"<<num_camera<<"个");

            /* 使用ffmpegCpu 实时解码获取图片*/
            if(DecodeType == 2){
                
                std::shared_ptr<ffDecodecpu>ffDecodecpu_ptr(new ffDecodecpu(false));
                int status = ffDecodecpu_ptr->iniffDecodecpu(str_ip,cam_info.str_name,cam_info.str_pass,num_camera);
                if(status <0){
                    LOG_INFO("相机初始化失败！"<<str_ip);
                    return -1;
                }
                ffDecodecpu_ptr->SetAutoDelete(true); // 保证了线程对象与线程的生命周期是一致的
                ffDecodecpu_ptr->Start();   
                ffDecodecpu_ptr->connectMessage(objdetection_ptr, slot_event);//绑定从 解码图片 发送数据到ProcessApply
                cam_ffDecodecpus.push_back(ffDecodecpu_ptr);  

            }
            /* 使用SDK 实时解码获取图片*/
            if(DecodeType == 3){
                
                std::shared_ptr<DecodeHK> decodeHK_ptr(new DecodeHK(false));
                int status = decodeHK_ptr->iniDecodeHK(str_ip,cam_info.str_name,cam_info.str_pass,num_camera);
                if(status <0){
                    LOG_INFO("相机初始化失败！"<<str_ip);
                    continue;
                }
                decodeHK_ptr->SetAutoDelete(true); // 保证了线程对象与线程的生命周期是一致的
                decodeHK_ptr->Start();   
                decodeHK_ptr->connectMessage(objdetection_ptr, slot_event);//绑定从captureHK 发送数据到ProcessApply
                cam_DecodeHKs.push_back(decodeHK_ptr);
            }


            /* 使用SDK capture获取图片*/
            if(DecodeType == 4){
                std::shared_ptr<captureHK> captureHK_ptr(new captureHK(false));
                int status = captureHK_ptr->iniCaptureHK(str_ip,cam_info.str_name,cam_info.str_pass,i);
                if(status <0){
                    LOG_INFO("相机初始化失败！"<<str_ip);
                    continue;
                }
                captureHK_ptr->SetAutoDelete(true); // 保证了线程对象与线程的生命周期是一致的
                captureHK_ptr->Start();   
                captureHK_ptr->connectMessage(objdetection_ptr, slot_event);//绑定从captureHK 发送数据到ProcessApply
                cam_captureHks.push_back(captureHK_ptr);
            }  
        } 

    }   
   

    if(DecodeType ==1){
        std::shared_ptr<DecodeOpencv>DecodeOpencv_ptr(new DecodeOpencv(false));
        int status_ = DecodeOpencv_ptr->iniOpencv();
        if(status_ <0){
            LOG_INFO("opencv 初始化失败！"<<str_ip);
            return -1;
        }
        DecodeOpencv_ptr->SetAutoDelete(true); // 保证线程对象与线程的生命周期是一致的
        DecodeOpencv_ptr->Start();   
        DecodeOpencv_ptr->connectMessage(objdetection_ptr, slot_event);//绑定从 解码图片 发送数据到ProcessApply
        cam_ffDecodeOpencvs.push_back(DecodeOpencv_ptr);
    }


   
    while(true){
        LOG_INFO("===========================");
        sleep(1);
        // usleep(100000);
    }
    return 1;    
}