/*
 * @Author: nyy imniuyuanye@sina.com
 * @Date: 2023-02-06 10:48:30
 * @LastEditors: nyy imniuyuanye@sina.com
 * @LastEditTime: 2023-03-28 10:55:04
 * @FilePath: /gb_detection/src/captureHK.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include "DecodeHK.h"

// #include "logger.h"
#include "../log4cplus/mylogger.h"
// #include "../log4cplus/logger.h"
#include "globale.h"

DecodeHK::DecodeHK(bool stop = false) : stop_(stop){
    std::cout << "DecodeHK Constructor" << std::endl;
}
DecodeHK::~DecodeHK(){
    std::cout << "DecodeHK Destructor" << std::endl;
}

void DecodeHK::Stop(){
    this->stop_ = true;
}

void CALLBACK DecodeHK::g_RealDataCallBack_V30(LONG lPlayHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* pUser)
{
    DecodeHK *thiz = static_cast<DecodeHK *>(pUser);

    int nPort = -1;
    DWORD dRet;
    BOOL inData =false;
    switch (dwDataType)
    {
        case NET_DVR_SYSHEAD:           //系统头
        if (!PlayM4_GetPort(&nPort))  //获取播放库未使用的通道号
        {
            break;
        }
        thiz->nPort=nPort;
        if (dwBufSize > 0) {
            if (!PlayM4_SetStreamOpenMode(nPort, STREAME_REALTIME)) {
            dRet = PlayM4_GetLastError(nPort);
            break;
            }
            if (!PlayM4_OpenStream(nPort, pBuffer, dwBufSize, 1024 * 1024)) {
            dRet = PlayM4_GetLastError(nPort);
            break;
            }
            //  //设置解码回调函数 只解码不显示
            //  if (!PlayM4_SetDecCallBack(nPort, captureHK::DecCBFun)) {
            //     dRet = PlayM4_GetLastError(nPort);
            //     break;
            //  }
            if (!PlayM4_SetDecCallBackMend(nPort, DecCBFun, pUser))
            {
            dRet = PlayM4_GetLastError(nPort);
            break;
            }
    
            // //设置解码回调函数 解码且显示
            // if (!PlayM4_SetDecCallBackEx(nPort, DecCBFun, NULL, NULL))
            // {
            // dRet = PlayM4_GetLastError(nPort);
            // break;
            // }
            
            //打开视频解码
            if (!PlayM4_Play(nPort, NULL))
            {
            dRet = PlayM4_GetLastError(nPort);
            break;
            }
            // //打开音频解码, 需要码流是复合流
            //  if (!PlayM4_PlaySound(nPort)) {
            //    dRet = PlayM4_GetLastError(nPort);
            //    break;
            //  }
        }
        break;
        case NET_DVR_STREAMDATA:  //码流数据
        if (dwBufSize > 0 && thiz->nPort != -1) {
            inData = PlayM4_InputData(thiz->nPort, pBuffer, dwBufSize);
            while (!inData) {
            sleep(10);
                inData = PlayM4_InputData(nPort, pBuffer, dwBufSize);
                LOG_INFO("PlayM4_InputData failed 1111"); 
            }
        }
        break;
        default:
        if (dwBufSize > 0 && thiz->nPort != -1) {
            inData = PlayM4_InputData(thiz->nPort, pBuffer, dwBufSize);
            while (!inData)
            {
                sleep(100);
                inData = PlayM4_InputData(nPort, pBuffer, dwBufSize);
                // std::cout << "PlayM4_InputData failed 22222" << std::endl;
                LOG_INFO("PlayM4_InputData failed 22222"); 
                break;
            }
        }
        break;
    }

}
void DecodeHK::EventMsg(cv::Mat img,std::string img_path,std::string image_time,int camid, void* pUser) { 
    DecodeHK *thiz = static_cast<DecodeHK *>(pUser);
    thiz->EventMessage(img,"path_img","current_time",camid);     
}



void CALLBACK DecodeHK::DecCBFun(LONG nPort, char *pBuf, LONG nSize, FRAME_INFO *pFrameInfo, void* pUser, LONG nReserved2)
{
    DecodeHK *thiz = static_cast<DecodeHK *>(pUser);
    // 
    // cv::Mat g_BGRImage;
    if (pFrameInfo->nType == T_YV12)
    {
        // if (g_BGRImage.empty())
        // {
        //     g_BGRImage.create(pFrameInfo->nHeight, pFrameInfo->nWidth, CV_8UC3);
        // }
        // LOG_INFO("pFrameInfo->nHeight:"<<pFrameInfo->nHeight<< " thiz->camera_ch:"<<thiz->camera_ch<< " nSize"<<nSize);
        cv::Mat g_BGRImage = cv::Mat(pFrameInfo->nHeight, pFrameInfo->nWidth, CV_8UC3, cv::Scalar(0));
        
        // if(thiz->camera_ch==1){
        //     LOG_INFO("g_BGRImage:"<<pFrameInfo->nHeight);
        //     // std::cout<<"DecodeHK_Peroid:"<<thiz->captureHK_Peroid<<"\n";            
        // }
        cv::Mat YUVImage(pFrameInfo->nHeight + pFrameInfo->nHeight / 2, pFrameInfo->nWidth, CV_8UC1, (unsigned char*)pBuf);

        cv::cvtColor(YUVImage, g_BGRImage, cv::COLOR_YUV2BGR_YV12);

        if (g_BGRImage.empty()) {
            LOG_INFO("========================= g_BGRImage.empty()========================"); 
        }
        else{
            
            // cv::resize(g_BGRImage,g_BGRImage,cv::Size(640,480));
            // cv::resize(g_BGRImage,g_BGRImage,cv::Size(960,640));      
            cv::resize(g_BGRImage,g_BGRImage,cv::Size(thiz->width,thiz->height));        
            // std::cout<<"T_YV12=========\n";
            // cv::imshow("RGBImage1", g_BGRImage);
            // cv::waitKey(5);
            
            // if(thiz->camera_ch==1){
            //     // std::cout<<"T_YV12=========\n";
            //     cv::imshow("RGBImage1", g_BGRImage);
            //     cv::waitKey(5);
            //     // std::cout<<"DecodeHK_Peroid:"<<thiz->captureHK_Peroid<<"\n";            
            // }

            
            auto end = std::chrono::system_clock::now();
            double time_out = std::chrono::duration_cast<std::chrono::milliseconds>(end - thiz->start).count();//ms 
            if(time_out>(thiz->captureHK_Peroid)){// 此处设计可以减少视频数量，但解码依旧是25帧 2024.2.2.nyy
                thiz->start= end;             
                // thiz->EventMessage(g_BGRImage,"path_img","current_time",thiz->camera_ch);
                thiz->EventMsg(g_BGRImage,"path_img","current_time",thiz->camera_ch,pUser);
            } 

            // int camera_ch = thiz->camera_ch;
            // thiz->EventMessage(g_BGRImage,"path_img","current_time",camera_ch);    
        }
        // YUVImage.~Mat();
    }
}
int DecodeHK::iniDecodeHK(std::string ip, std::string name,std::string pass,int camera_ch){
    NET_DVR_Init();
    
    
    pthread_mutex_init(&this->lock_p,NULL);
    std::string inifile = "./properties.ini";
    long captureHK_Peroid = ini_getl("switch", "decode_Peroid", 300, inifile.c_str()); 
    this->captureHK_Peroid = captureHK_Peroid;
    LOG_INFO("@@@@@@@@@@@@@@@ captureHK_Peroid:"+std::to_string(captureHK_Peroid));  

    this->width = ini_getl("FY", "width", 640, inifile.c_str());   
    LOG_INFO("@@@@@@@@@@@@@@@ width:"+std::to_string(width)); 

    this->height = ini_getl("FY", "height", 480, inifile.c_str());   
    LOG_INFO("@@@@@@@@@@@@@@@ height:"+std::to_string(height)); 

    this->ip   = ip;
    this->name = name;
    this->pass = pass;
    this->camera_ch = camera_ch;
    return 1;
}
void DecodeHK::Run() // 虚函数的重写可以改变成员函数的权限
{
    start = std::chrono::system_clock::now();
    // LOG_INFO("start:"<<start);  
    NET_DVR_DEVICEINFO_V30 struDeviceInfo;
    int iPort = 8000;
    int iChannel = 1;
    int iUserID = NET_DVR_Login_V30(const_cast<char *>(this->ip.c_str()), iPort, const_cast<char *>(this->name.c_str()), const_cast<char *>(this->pass.c_str()), &struDeviceInfo);

    if(iUserID >= 0)
    {
            // NET_DVR_CLIENTINFO ClientInfo = {0};
            // ClientInfo.lChannel     = iChannel;  //channel NO.
            // ClientInfo.lLinkMode    = 0;
            // ClientInfo.sMultiCastIP = NULL;
            // int iRealPlayHandle = NET_DVR_RealPlay_V30(iUserID, &ClientInfo, PsDataCallBack, NULL, 0);

        NET_DVR_PREVIEWINFO struPreviewInfo = {0};
        struPreviewInfo.lChannel =iChannel;
        struPreviewInfo.dwStreamType = 0;
        struPreviewInfo.dwLinkMode = 0;
        struPreviewInfo.bBlocked = 1;
        struPreviewInfo.bPassbackRecord  = 1;
        // int iRealPlayHandle = NET_DVR_RealPlay_V40(iUserID, &struPreviewInfo, PsDataCallBack, NULL);
        int iRealPlayHandle = NET_DVR_RealPlay_V40(iUserID, &struPreviewInfo, DecodeHK::g_RealDataCallBack_V30, this);
        // int iRealPlayHandle = NET_DVR_RealPlay_V40(iUserID, &struPreviewInfo, NULL, NULL);
        if(iRealPlayHandle >= 0)
        {
            // printf("[GetStream]---RealPlay %s:%d success, \n", ip, iChannel, NET_DVR_GetLastError());
            LOG_INFO("NET_DVR_RealPlay_V40 success ip:"<<ip<<" camera_ch:"<<camera_ch<<" error = "<<NET_DVR_GetLastError()); 
        }
        else
        {
            // printf("[GetStream]---RealPlay %s:%d failed, error = %d\n", ip, iChannel, NET_DVR_GetLastError());
            LOG_INFO("NET_DVR_RealPlay_V40 failed ip:"<<ip<<" camera_ch:"<<camera_ch<<" error = "<<NET_DVR_GetLastError()); 
            stop_ = true; 
        }
        // NET_DVR_SetStandardDataCallBack(iRealPlayHandle, g_StdDataCallBack1, 0);
    }
    else
    {
        // printf("[GetStream]---Login %s failed, error = %d\n", ip, NET_DVR_GetLastError());
        LOG_INFO("NET_DVR_Login_V30 failed ip:"<<ip<<" camera_ch:"<<camera_ch<<" error = "<<NET_DVR_GetLastError()); 
        stop_ = true;  
    }

    // std::function<void(int,int)> functional = std::bind(&DecodeHK::classMemberFun, this, std::placeholders::_1, std::placeholders::_2); 

    std::function<void(cv::Mat ,std::string ,std::string ,int ,void*)>functional = DecodeHK::EventMsg;
    //直接在回调函数中无法使用继承的EventMessage方法 所以通过二次std::function  2023.03.28 nyy

    while (!stop_)
    {       
        // auto start = std::chrono::system_clock::now();  

        // sleep(1);   
        // 

        // auto end = std::chrono::system_clock::now();
        // double time_out = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();//ms 
        
        // double time_sleep = this->captureHK_Peroid - time_out;//默认周期 300ms   
    
        // if(time_sleep > 0){
            
        //     usleep(time_sleep*1000);
        // }    

        sleep(2);
        // usleep(50);
    }

    LOG_INFO("线程结束 ip:"<<ip<<" camera_ch:"<<camera_ch); 
    
}
void DecodeHK::EventMessage(cv::Mat img,std::string img_path,std::string image_time,int camid)//event 发送数据
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


