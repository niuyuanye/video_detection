
#include "wxApplyYolov57.h"
#include "../yolov5_v7/utils.h"
#include "../yolov5_v7/preprocess.h"
#include "../yolov5_v7/postprocess.h"

#include "../../log4cplus/mylogger.h"
namespace objdetection{
wxApplyYolov57::wxApplyYolov57(bool stop = false) : stop_(stop){
    LOG_INFO("\n wxApplyYolov56 start \n ");
    pthread_mutex_init(&this->lock_p,NULL);
}
wxApplyYolov57::~wxApplyYolov57(){
    LOG_INFO("\n wxApplyYolov57seg end \n ");
}

void wxApplyYolov57::Stop(){
    this->stop_ = true;    
}

/** WX yolov5.7 模型初始化
 * @description: 
 * @return {*}
 */
int wxApplyYolov57::iniModel(){  
   	/************************************************************************************************
	* init
	*************************************************************************************************/
    std::string inifile = "./properties.ini";
    long time_Peroid = ini_getl("WX", "time_Peroid", 300, inifile.c_str()); 
    this->time_Peroid = time_Peroid;
    LOG_INFO("@@@@@@@@@@@@@@@ time_Peroid:"+std::to_string(time_Peroid));

    long g_bufsize = ini_getl("WX", "g_bufsize", 100, inifile.c_str());   
    LOG_INFO("@@@@@@@@@@@@@@@ g_bufsize:"+std::to_string(g_bufsize)); 
    this->g_bufsize = g_bufsize;

    float NMS_THRESH = ini_getf("WX", "NMS_THRESH", 100, inifile.c_str());  
    LOG_INFO("@@@@@@@@@@@@@@@ NMS_THRESH:"+std::to_string(NMS_THRESH)); 

    float CONF_THRESH = ini_getf("WX", "CONF_THRESH", 100, inifile.c_str());   
    LOG_INFO("@@@@@@@@@@@@@@@ CONF_THRESH:"+std::to_string(CONF_THRESH)); 

    char enginename[100];
    long n = ini_gets("WX", "engine_name", "yolov5m.engine", enginename, sizearray(enginename), const_cast<char *>(inifile.c_str()));
    if (n >= 0) LOG_INFO("enginename"<<":"<<enginename);

    this->target_id = ini_getl("switch", "target_id", 1, inifile.c_str());  
    LOG_INFO("@@@@@@@@@@@@@@@ target_id:"+std::to_string(target_id));  
    this->is_show = ini_getl("switch", "is_show", 0, inifile.c_str());  
    LOG_INFO("@@@@@@@@@@@@@@@ is_show:"+std::to_string(is_show)); 
    this->is_save = ini_getl("switch", "is_save", 0, inifile.c_str());  
    LOG_INFO("@@@@@@@@@@@@@@@ is_save:"+std::to_string(is_save)); 

    /************************************************************************************************
	* recycle
	*************************************************************************************************/

    yolo = std::make_shared<Yolov5_v7TRT>();
    return yolo->InitialDetcet(enginename,NMS_THRESH,CONF_THRESH); 
}

/** 主函数 tensorRT yolov5 检测
 * @description: 
 * @return {*}
 */
void wxApplyYolov57::Run() // 虚函数的重写可以改变成员函数的权限
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

        std::vector<std::vector<Detection>> res_batch(1);
        cv::Mat batch_res_tmp =  this->yolo->DetectImg(img,res_batch);
        
        // auto& res = batch_res_tmp[0];        

        // bool is_target = false;
        // for (size_t j = 0; j < res.size(); j++) {
        //     cv::Rect r = Get_rect(img, res[j].bbox);
        //     cv::rectangle(img, r, cv::Scalar(0x27, 0xC1, 0x36), 2);
        //     std::string lable = std::to_string((int)res[j].class_id) + ";"+std::to_string(res[j].conf);
        //     cv::putText(img, lable, cv::Point(r.x, r.y - 1), cv::FONT_HERSHEY_PLAIN, 1.2, cv::Scalar(0, 0, 0xFF), 2);

        //     if((int)res[j].class_id == 0){
        //         is_target = true;
        //     }
        // }

        auto end = std::chrono::system_clock::now();
        double time_out = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();//ms 
        
        double time_sleep = this->time_Peroid - time_out;//默认周期 300ms
        // LOG_INFO("captureHK_Peroid:"<< captureHK_Peroid);
        // LOG_INFO("time_sleep:"<< time_sleep);
        if(this->is_show){
            if(camid==target_id){
            cv::imshow("WXYolov57", img);
            cv::waitKey(1);
           }
        }
        if(this->is_save){
            if(camid==target_id){
                cv::imwrite("../result/"+img_path, img);
            }  
        }

        if(camid==target_id){
            LOG_INFO("WX yolov57time:"<<time_out<<" ms "<<camid); 
            // LOG_INFO("time_out:"<<time_out);
        }

        if(time_sleep > 0){            
            // usleep(time_sleep*1000);
        }       
    }
    this->yolo->Release();
}
/** 回调函数
 * @description: 
 * @param {Mat} img
 * @param {string} img_path
 * @param {string} image_time
 * @param {int} camid
 * @return {*}
 */
void wxApplyYolov57::eventFunc(cv::Mat img, std::string img_path,std::string image_time,int camid)
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

cv::Rect wxApplyYolov57::Get_rect(cv::Mat& img, float bbox[4]) {
    int INPUT_W = 640,INPUT_H=640;
    int l, r, t, b;
    float r_w = INPUT_W / (img.cols * 1.0);
    float r_h = INPUT_H / (img.rows * 1.0);
    if (r_h > r_w) {
        l = bbox[0] - bbox[2] / 2.f;
        r = bbox[0] + bbox[2] / 2.f;
        t = bbox[1] - bbox[3] / 2.f - (INPUT_H - r_w * img.rows) / 2;
        b = bbox[1] + bbox[3] / 2.f - (INPUT_H - r_w * img.rows) / 2;
        l = l / r_w;
        r = r / r_w;
        t = t / r_w;
        b = b / r_w;
    } else {
        l = bbox[0] - bbox[2] / 2.f - (INPUT_W - r_h * img.cols) / 2;
        r = bbox[0] + bbox[2] / 2.f - (INPUT_W - r_h * img.cols) / 2;
        t = bbox[1] - bbox[3] / 2.f;
        b = bbox[1] + bbox[3] / 2.f;
        l = l / r_h;
        r = r / r_h;
        t = t / r_h;
        b = b / r_h;
    }
    return cv::Rect(l, t, r - l, b - t);
}

}
