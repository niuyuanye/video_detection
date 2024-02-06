/*** 
 * @Author: nyy
 * @Date: 2023-03-01 21:04:34
 * @LastEditTime: 2023-03-01 22:40:40
 * @LastEditors: nyy
 * @Description: 
 * @FilePath: /video_detection/src/objdetection/fyApplyYolov8.cpp
 * @可以输入预定的版权声明、个性签名、空行等
 */

#include "ApplyDepthAnything.h"
#include "../../log4cplus/mylogger.h"
namespace objdetection{
/**
 * @brief Setting up Tensorrt logger
*/
class Logger : public nvinfer1::ILogger
{
    void log(Severity severity, const char* msg) noexcept override
    {
        // Only output logs with severity greater than warning
        if (severity <= Severity::kWARNING)
            std::cout << msg << std::endl;
    }
}logger;




ApplyDepthAnything::ApplyDepthAnything(bool stop = false) : stop_(stop){
    LOG_INFO("\n ApplyDepthAnything start \n ");
    pthread_mutex_init(&this->lock_p,NULL);
}
ApplyDepthAnything::~ApplyDepthAnything(){
    LOG_INFO("\n ApplyDepthAnything end \n ");
}

void ApplyDepthAnything::Stop(){
    this->stop_ = true;    
}

/** FY yolov8 模型初始化
 * @description: 
 * @return {*}
 */
int ApplyDepthAnything::iniModel(){
    
    std::string inifile = "./properties.ini";
    long captureHK_Peroid = ini_getl("FY", "time_Peroid", 300, inifile.c_str()); 
    this->captureHK_Peroid = captureHK_Peroid;
    LOG_INFO("@@@@@@@@@@@@@@@ ProApply_Peroid:"+std::to_string(captureHK_Peroid));


    long g_bufsize = ini_getl("FY", "g_bufsize", 100, inifile.c_str());   
    LOG_INFO("@@@@@@@@@@@@@@@ g_bufsize:"+std::to_string(g_bufsize)); 

    float NMS_THRESH = ini_getf("FY", "NMS_THRESH", 0.2, inifile.c_str());  
    LOG_INFO("@@@@@@@@@@@@@@@ NMS_THRESH:"+std::to_string(NMS_THRESH)); 

    float CONF_THRESH = ini_getf("FY", "CONF_THRESH", 0.2, inifile.c_str());   
    LOG_INFO("@@@@@@@@@@@@@@@ CONF_THRESH:"+std::to_string(CONF_THRESH));   
    


    this->target_id = ini_getl("switch", "target_id", 1, inifile.c_str());  
    LOG_INFO("@@@@@@@@@@@@@@@ target_id:"+std::to_string(target_id)); 

    this->is_show = ini_getl("switch", "is_show", 0, inifile.c_str());  
    LOG_INFO("@@@@@@@@@@@@@@@ is_show:"+std::to_string(is_show));  
    this->is_save = ini_getl("switch", "is_save", 0, inifile.c_str());  
    LOG_INFO("@@@@@@@@@@@@@@@ is_save:"+std::to_string(is_save)); 

    char engineModel[100];
    long n = ini_gets("Depth", "model_vit", "../weigths/yolov8s.trt", engineModel, sizearray(engineModel), const_cast<char *>(inifile.c_str()));
    if (n >= 0) LOG_INFO("enginename"<<":"<<engineModel);
    std::string model_path = engineModel;  

    long width = ini_getl("FY", "width", 640, inifile.c_str());   
    LOG_INFO("@@@@@@@@@@@@@@@ width:"+std::to_string(width)); 

    long height = ini_getl("FY", "height", 480, inifile.c_str());   
    LOG_INFO("@@@@@@@@@@@@@@@ height:"+std::to_string(height)); 

	/************************************************************************************************
	* init
	*************************************************************************************************/	
	
    // DepthAnything depth_model(engine_file_path, logger);
    depth_model = std::make_shared<DepthAnything>(model_path, logger);

  
    /************************************************************************************************
	* recycle
	*************************************************************************************************/

    return 1;


}

/** 主函数 tensorRT yolov5 检测
 * @description: 
 * @return {*}
 */
void ApplyDepthAnything::Run() // 虚函数的重写可以改变成员函数的权限
{    
    bool is_show = true;
	bool is_save = true;
    int delay_time = 1;
    int batchi = 0;
    while (!stop_)
    {       
        auto start = std::chrono::system_clock::now();       

        pthread_mutex_lock(&lock_p); //lock
        bool flag=g_imageQueue.empty();
        pthread_mutex_unlock(&lock_p); //unlock
        if (flag) {
            usleep(10*1000);
            continue;
        }        
        pthread_mutex_lock(&lock_p); //lock
        std::shared_ptr<Image_vedio> video_data = g_imageQueue.front();
        g_imageQueue.pop();   
        pthread_mutex_unlock(&lock_p); //unlock



        cv::Mat img = video_data->image.clone();
        if (img.empty()) {
            LOG_INFO("========================= img.empty()========================"); 
            continue;
        }
        int camid=video_data->camid;
        std::string img_path = video_data->imagePath;


        cv::Mat im0,im1;
        img.copyTo(im0);
        // Inference depth model
        im1 = depth_model->predict(im0);


        if(this->is_show){
            if(camid==target_id){
                cv::imshow("Depth-Anything", im1);
                cv::waitKey(1);

                cv::imshow("src", img);
                cv::waitKey(1);
            }  
        }
        if(this->is_save){
            if(camid==target_id){
                cv::imwrite("../result/"+img_path, im1);
            }
        }  

        auto end = std::chrono::system_clock::now();
        double time_out = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();//ms 
        
        double time_sleep = this->captureHK_Peroid - time_out;//默认周期 300ms
        // start =  end;
        // LOG_INFO("captureHK_Peroid:"<< captureHK_Peroid);
        // LOG_INFO("time_sleep:"<< time_sleep);

        if(camid==target_id){
            LOG_INFO("Depth-Anything time:"<<time_out<<" ms "<<camid); 
            // LOG_INFO("time_out:"<<time_out);
        }

        if(time_sleep > 0){            
            // usleep(time_sleep*1000);
        }       
    }
}

/** 回调函数
 * @description: 
 * @param {Mat} img
 * @param {string} img_path
 * @param {string} image_time
 * @param {int} camid
 * @return {*}
 */
void ApplyDepthAnything::eventFunc(cv::Mat img, std::string img_path,std::string image_time,int camid)
{
    auto start = std::chrono::system_clock::now();  

    cv::Mat image = img; 
    // img.copyTo(image);
    // LOG_INFO("apply 01");      

    std::shared_ptr<Image_vedio> result(new Image_vedio());
    result->camid = camid;
    result->image = image;
    result->imagePath = img_path;
    result->hight= image.rows;
    result->width= image.cols;
    result->imageTime= image_time;

    pthread_mutex_lock(&lock_p); //lock
    if (g_imageQueue.size() >= 100) {
        LOG_INFO("g_bufsize  is Overflow : "<< g_imageQueue.size());
        g_imageQueue.pop();
        g_imageQueue.push(result);
    } else {
        g_imageQueue.push(result);
    }

    pthread_mutex_unlock(&lock_p); //unlock

    std::string current_time = GetCurrentTimeStamp(1);
    // LOG_INFO("current_time:"<< current_time);

    // cv::Mat image = cv::imread(img_path);
    // cv::resize(image,image,cv::Size(640,480)); 

    // std::string path_result = "../result/"+std::to_string(camid)+"/"+current_time+".jpg";
    // cv::imwrite(path_result, img);

    // if(camid == 1){
    //     LOG_INFO("image_time:"<<image_time);
    // }
    
}



}
