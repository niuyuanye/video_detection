/*
 * @Author: nyy imniuyuanye@sina.com
 * @Date: 2023-02-06 10:48:30
 * @LastEditors: nyy imniuyuanye@sina.com
 * @LastEditTime: 2023-03-01 17:48:00
 * @FilePath: /gb_detection/src/captureHK.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include "captureHK.h"
#include "../log4cplus/mylogger.h"
// #include "../log4cplus/logger.h"
#include "globale.h"

captureHK::captureHK(bool stop = false) : stop_(stop){
    std::cout << "captureHK Constructor" << std::endl;
}
captureHK::~captureHK(){
    std::cout << "captureHK Destructor" << std::endl;
}

void captureHK::Stop(){
    this->stop_ = true;
}

int captureHK::iniCaptureHK(std::string ip, std::string name,std::string pass,int camera_ch){
    pthread_mutex_init(&this->lock_p,NULL);
    std::string inifile = "./properties.ini";
    long captureHK_Peroid = ini_getl("switch", "captureHK_Peroid", 300, inifile.c_str()); 
    this->captureHK_Peroid = captureHK_Peroid;
    LOG_INFO("@@@@@@@@@@@@@@@ captureHK_Peroid:"+std::to_string(captureHK_Peroid));  


    NET_DVR_Init();
    //login
    NET_DVR_USER_LOGIN_INFO struLoginInfo = {0};
    // NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = {0};
    struLoginInfo.bUseAsynLogin = false;

    this->ip=ip;
    this->name=name;
    this->pass=pass;
    this->camera_ch=camera_ch;

    struLoginInfo.wPort = 8000;
    memcpy(struLoginInfo.sDeviceAddress, const_cast<char *>(ip.c_str()), NET_DVR_DEV_ADDRESS_MAX_LEN);
    memcpy(struLoginInfo.sUserName, const_cast<char *>(name.c_str()), NAME_LEN);
    memcpy(struLoginInfo.sPassword, const_cast<char *>(pass.c_str()), NAME_LEN);

    lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);

    if (lUserID < 0)
    {
        // printf("pyd1---Login error, %d\n", NET_DVR_GetLastError());
        LOG_INFO("pyd1---Login error, :"<<NET_DVR_GetLastError()); 
        return -1;
    }
    LOG_INFO("NET_DVR_Login_V40 success!!!");
    return 1;
}



void captureHK::Run() // 虚函数的重写可以改变成员函数的权限
{    
    while (!stop_)
    {       
        auto start = std::chrono::system_clock::now();       
        NET_DVR_JPEGPARA strPicPara = {0};
        strPicPara.wPicQuality = 2;
        strPicPara.wPicSize = 0;
        int iRet;        
        std::string path_img = "./camera"+std::to_string(camera_ch)+".jpg";
        // std::string path_img = std::to_string(camera_ch)+".jpg";

        iRet = NET_DVR_CaptureJPEGPicture(lUserID, struDeviceInfoV40.struDeviceV30.byStartChan, &strPicPara, const_cast<char *>(path_img.c_str()));
        if (!iRet)
        {
            // printf("pyd1---NET_DVR_CaptureJPEGPicture error, %d\n", NET_DVR_GetLastError());
            LOG_INFO("pyd1---NET_DVR_CaptureJPEGPicture error:"<<NET_DVR_GetLastError()); 
            // return HPR_ERROR;
            continue;
        }
        
        std::string current_time = GetCurrentTimeStamp(1);
        cv::Mat image = cv::imread(path_img);
        cv::resize(image,image,cv::Size(960,720));
        EventMessage(image,path_img,current_time,camera_ch);
        
        auto end = std::chrono::system_clock::now();
        double time_out = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();//ms 
        
        double time_sleep = this->captureHK_Peroid - time_out;//默认周期 300ms
        // start =  end;
        // LOG_INFO("captureHK_Peroid:"<< captureHK_Peroid);
        // LOG_INFO("time_sleep:"<< time_sleep);
        // LOG_INFO("time_out:"<<time_out); 

        // if(camera_ch==2){
        //     // LOG_INFO("current_time:"<<current_time);
        //     LOG_INFO("time_out:"<<time_out);
        // }
        if(time_sleep > 0){
            
            usleep(time_sleep*1000);
        }
        
        // LOG_INFO("current_time:"<< current_time);
        // LOG_INFO("Capture img time:"<<time_ou); 

        // sleep(1);
        // usleep(50);
    }
}

void captureHK::EventMessage(cv::Mat img,std::string img_path,std::string image_time,int camid)//event 发送数据
{
        // m_recver->m_sender = std::bind(&ProcessApply::self, this);
        // m_callFun(m_recver,img, img_path, image_time, camid);
        // m_recver->m_sender = NULL;

        if(m_callFun.operator bool()){
            // m_callFun(m_recver,img, img_path, image_time, camid);
            // m_recver->m_sender = std::bind(&objdetection::ProApplyYolov8::self, this);
            m_callFun(m_recver,img, img_path, image_time, camid);
            // m_recver->m_sender = NULL;
        }else{
            LOG_INFO("fs callable object not set, not callable");
        }  
}
