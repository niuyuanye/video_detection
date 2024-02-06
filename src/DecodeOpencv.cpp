/*** 
 * @Author: nyy
 * @Date: 2023-03-01 22:56:59
 * @LastEditTime: 2023-03-02 00:48:50
 * @LastEditors: nyy
 * @Description:
 *  使用opencv读取文件夹图片、视频、usb摄像头；
 * 设计不支持多对象调用，此方法只用于测试；
 * 生产环境图像获取使用ffmpeg、HK-SDK、DH-SDK；
 * @FilePath: /video_detection/src/DecodeOpencv.cpp
 * @可以输入预定的版权声明、个性签名、空行等
 */



#include "DecodeOpencv.h"
#include "../log4cplus/mylogger.h"
#include "globale.h"

DecodeOpencv::DecodeOpencv(bool stop = false) : stop_(stop){
    std::cout << "DecodeOpencv Constructor" << std::endl;
}
DecodeOpencv::~DecodeOpencv(){
    std::cout << "DecodeOpencv Destructor" << std::endl;
}

void DecodeOpencv::Stop(){
    this->stop_ = true;
}

 int DecodeOpencv::iniOpencv(){

    pthread_mutex_init(&this->lock_p,NULL);
    std::string inifile = "./properties.ini";
    long time_Peroid_tmp = ini_getl("opencv", "time_Peroid", 300, inifile.c_str()); 
    this->time_Peroid = time_Peroid_tmp;
    LOG_INFO("@@@@@@@@@@@@@@@ time_Peroid:"+std::to_string(time_Peroid));  

    long Type_tmp = ini_getl("opencv", "Type", 1, inifile.c_str()); 
    LOG_INFO("@@@@@@@@@@@@@@@ Type_tmp:"+std::to_string(Type_tmp));  
    this->Type = (int)Type_tmp;
    

    this->width = ini_getl("FY", "width", 640, inifile.c_str());   
    LOG_INFO("@@@@@@@@@@@@@@@ width:"+std::to_string(width)); 

    this->height = ini_getl("FY", "height", 480, inifile.c_str());   
    LOG_INFO("@@@@@@@@@@@@@@@ height:"+std::to_string(height)); 


    LOG_INFO("@@@@@@@@@@@@@@@ Type:"+std::to_string(Type));  
    char video_path_tmp[100];
    long n = ini_gets("opencv", "video_path", "error.mp4", video_path_tmp, sizearray(video_path_tmp), const_cast<char *>(inifile.c_str()));
    if (n >= 0) LOG_INFO("@@@@@@@@@@@@@@@ video_path"<<":"<<video_path_tmp);
    this->video_path = video_path_tmp;


    char img_dir_tmp[100];
    n = ini_gets("opencv", "img_dir", "/dev/shm/video_detection/data", img_dir_tmp, sizearray(img_dir_tmp), const_cast<char *>(inifile.c_str()));
    if (n >= 0) LOG_INFO("@@@@@@@@@@@@@@@ img_dir"<<":"<<img_dir_tmp);
    this->img_dir = img_dir_tmp;

    LOG_INFO("opencv 初始化成功！");



    return 1;
 }
void DecodeOpencv::Run() // 虚函数的重写可以改变成员函数的权限
{   


    cv::VideoCapture capture;    
    cv::Mat frame;

    if(this->Type==1){
        capture.open(this->video_path);
        // capture.open(1);
        while (capture.isOpened())
        {
            auto start = std::chrono::system_clock::now();    
            capture.read(frame);
            cv::resize(frame,frame,cv::Size(width,height));
            EventMessage(frame,"path_img","current_time",1);

            auto end = std::chrono::system_clock::now();
            double time_out = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();//ms 

            double time_sleep = this->time_Peroid - time_out;//默认周期 300ms
            // double time_sleep = 40 - time_out;//默认周期 300ms
            if(time_sleep > 0){
                usleep(time_sleep*1000);
            }
        }  
    }

    if(this->Type==2){
        capture.open(0);
        while (capture.isOpened())
        {
            auto start = std::chrono::system_clock::now();    
            capture.read(frame);
            cv::resize(frame,frame,cv::Size(width,height));
            EventMessage(frame,"usb0.jpg","current_time",1);

            auto end = std::chrono::system_clock::now();
            double time_out = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();//ms 

            double time_sleep = this->time_Peroid - time_out;//默认周期 300ms
            if(time_sleep > 0){
                usleep(time_sleep*1000);
            }
        }  
    }


    if(this->Type==3){
        std::vector<std::string> file_names;
        if (read_files_in_dir_nyy(this->img_dir.c_str(), file_names) < 0) {
            // std::cerr << "read_files_in_dir failed." << std::endl;
            LOG_INFO("read_files_in_dir failed::"<<this->img_dir);
            
        }
        for (int f = 0; f < (int)file_names.size(); f++) {

        // LOG_INFO(file_names[f]);
        auto start = std::chrono::system_clock::now();    
        std::string img_path = this->img_dir+"/"+(file_names[f]);
        LOG_INFO(img_path);
        cv::Mat img = cv::imread(img_path);
        cv::resize(img,img,cv::Size(width,height));
        EventMessage(img, file_names[f],"current_time",1);

        auto end = std::chrono::system_clock::now();
        double time_out = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();//ms 

        double time_sleep = this->time_Peroid - time_out;//默认周期 300ms
        if(time_sleep > 0){
            usleep(time_sleep*1000);
        }

        }
    }
    // while (!stop_)
    // {      
    //     if(time_sleep > 0){
            
    //         usleep(time_sleep*1000);
    //     }

    //     // sleep(1);
    //     // usleep(50);
    // }
}

void DecodeOpencv::EventMessage(cv::Mat img,std::string img_path,std::string image_time,int camid)//event 发送数据
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