/*** 
 * @Author: nyy
 * @Date: 2023-03-01 21:36:05
 * @LastEditTime: 2023-03-01 22:30:51
 * @LastEditors: nyy
 * @Description: 
 * @FilePath: /video_detection/src/objdetection/fyApplyYolov7.cpp
 * @可以输入预定的版权声明、个性签名、空行等
 */

#include "fyApplyYolov7.h"
#include "../../log4cplus/mylogger.h"
// #include "../log4cplus/logger.h"
namespace objdetection{
YOLOV7::YOLOV7(const utils::InitParameter& param) :yolo::YOLO(param)
{
}
YOLOV7::~YOLOV7()
{
}

fyApplyYolov7::fyApplyYolov7(bool stop = false) : stop_(stop){
    LOG_INFO("\n fyApplyYolov7 start \n ");
    pthread_mutex_init(&this->lock_p,NULL);
}
fyApplyYolov7::~fyApplyYolov7(){
    LOG_INFO("\n fyApplyYolov7 end \n ");
}
void fyApplyYolov7::Stop(){
    this->stop_ = true;    
}

/** FY yolov7 模型初始化
 * @description: 
 * @return {*}
 */
int fyApplyYolov7::iniModel(){
    
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
    long n = ini_gets("FY", "yolov5model", "../weigths/yolov5s.trt", engineModel, sizearray(engineModel), const_cast<char *>(inifile.c_str()));
    if (n >= 0) LOG_INFO("enginename"<<":"<<engineModel);
    std::string model_path = engineModel;

    char yolov5_version_tmp[100];
    n = ini_gets("FY", "yolov5_version", "v560", yolov5_version_tmp, sizearray(yolov5_version_tmp), const_cast<char *>(inifile.c_str()));
    if (n >= 0) LOG_INFO("yolov7_version_tmp"<<":"<<yolov5_version_tmp);
    std::string yolov5_version = yolov5_version_tmp;    

    long width = ini_getl("FY", "width", 640, inifile.c_str());   
    LOG_INFO("@@@@@@@@@@@@@@@ width:"+std::to_string(width)); 

    long height = ini_getl("FY", "height", 480, inifile.c_str());   
    LOG_INFO("@@@@@@@@@@@@@@@ height:"+std::to_string(height)); 

	/************************************************************************************************
	* init
	*************************************************************************************************/
	// parameters	
	setParameters(param);
	// update params from command line parser
	int size = 640;
	int batch_size = 1;
    param.dst_h = param.dst_w = size;

	int total_batches = 1;
	int delay_time = 0;
	param.batch_size = 1;
    // param.src_h = 1920;
    // param.src_w = 1080;
    param.src_h = height;
    param.src_w = width;
    param.conf_thresh = CONF_THRESH;
	param.iou_thresh = NMS_THRESH;

    std::cout<<"total_batches:"<<total_batches<<"\n";
	std::cout<<"param.batch_size:"<<param.batch_size<<"\n";
	std::cout<<"param.src_h:"<<param.src_h<<"\n";
	
	// YOLOV5 yolo(param);
    yolo = std::make_shared<YOLOV7>(param);

	// read model
	// std::vector<unsigned char> trt_file = utils::loadModel(model_path);
    std::vector<unsigned char> trt_file = utils::loadModel(model_path);
	if (trt_file.empty())
	{
		// sample::gLogError << "trt_file is empty!" << std::endl;
        LOG_INFO("trt_file is empty!");
		return -1;
	}
	// init model
	if (!yolo->init(trt_file))
	{
		// sample::gLogError << "initEngine() ocur errors!" << std::endl;
        LOG_INFO("initEngine() ocur errors!");
		return -1;
	}
	yolo->check();

    /************************************************************************************************
	* recycle
	*************************************************************************************************/

    return 1;


}

/** 主函数 tensorRT yolov7 检测
 * @description: 
 * @return {*}
 */
void fyApplyYolov7::Run() // 虚函数的重写可以改变成员函数的权限
{    
    // bool is_show = true;
	// bool is_save = true;
    // int delay_time = 1;
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


        std::vector<cv::Mat> imgs_batch;
	    imgs_batch.reserve(param.batch_size);        
        // img = cv::imread(image_path);
        // cv::resize(img, img, cv::Size(640, 640), 0, 0, CV_INTER_LINEAR);//改成大小都为50,50的
        imgs_batch.emplace_back(img.clone());
        
        utils::DeviceTimer d_t1; yolo->preprocess(imgs_batch);  float t1 = d_t1.getUsedTime();
        utils::DeviceTimer d_t2; yolo->infer();				  float t2 = d_t2.getUsedTime();
        utils::DeviceTimer d_t3; yolo->postprocess(imgs_batch); float t3 = d_t3.getUsedTime();

        const std::vector<std::vector<utils::Box>>& objectss = yolo->getObjectss();
        const std::vector<std::string>& classNames = param.class_names;


        for (size_t bi = 0; bi < imgs_batch.size(); bi++)
        {
            if (!objectss.empty())
            {
                for (auto& box : objectss[bi])
                {				
                    cv::rectangle(imgs_batch[bi], cv::Point(box.left, box.top), cv::Point(box.right, box.bottom), cv::Scalar(0, 255, 255), 2, cv::LINE_AA);
                    cv::putText(imgs_batch[bi], cv::format("%.4f", box.confidence), cv::Point(box.left, box.top - 3), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 255), 1, cv::LINE_AA);
                    cv::putText(imgs_batch[bi], classNames[box.label], cv::Point(box.left, box.top + 12), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 255), 1, cv::LINE_AA);
                    if (!box.land_marks.empty()) // for facial landmarks
                    {
                        for (auto& pt:box.land_marks)
                        {
                            cv::circle(imgs_batch[bi], pt, 1, cv::Scalar(255, 255, 255), 1, cv::LINE_AA, 0);
                        }
                    }
                }
            }		
            cv::Mat img = imgs_batch[bi];
            if(this->is_show){
                if(camid==target_id){
                cv::imshow("FYyolov7IMG", img);
                cv::waitKey(1);
            }  
            }
            if(this->is_save){
                if(camid==target_id){
                cv::imwrite("../result/"+img_path, img);
            }  
            }
          
	    }    
        yolo->reset(); 
        imgs_batch.clear(); 

        auto end = std::chrono::system_clock::now();
        double time_out = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();//ms 
        
        double time_sleep = this->captureHK_Peroid - time_out;//默认周期 300ms
        // start =  end;
        // LOG_INFO("captureHK_Peroid:"<< captureHK_Peroid);
        // LOG_INFO("time_sleep:"<< time_sleep);

        if(camid==target_id){
            LOG_INFO("FY yolov7 time:"<<time_out<<" ms "<<camid); 
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
void fyApplyYolov7::eventFunc(cv::Mat img, std::string img_path,std::string image_time,int camid)
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
    if (g_imageQueue.size() >= this->g_bufsize) {
        // std::cout << "g_bufsize  is Overflow ! "<< g_imageQueue.size() <<std::endl;
        LOG_INFO("g_bufsize  is Overflow : "<< g_imageQueue.size());
        g_imageQueue.pop();
        g_imageQueue.push(result);
    } else {
        g_imageQueue.push(result);
    }

    pthread_mutex_unlock(&lock_p); //unlock

    // std::string current_time = GetCurrentTimeStamp(1);

    
}

/** 设置yolov7初始化参数
 * @description: 
 * @param {InitParameter&} initParameters
 * @return {*}
 */
void fyApplyYolov7::setParameters(utils::InitParameter& initParameters)
{
	initParameters.class_names = utils::dataSets::coco80;
	initParameters.num_class = 80; // for coco

	initParameters.batch_size = 8;
	initParameters.dst_h = 640;
	initParameters.dst_w = 640;

	/*initParameters.dst_h = 1280;
	initParameters.dst_w = 1280;*/

	initParameters.input_output_names = { "images",  "output" };

	initParameters.conf_thresh = 0.25f;
	initParameters.iou_thresh = 0.45f;

	//initParameters.conf_thresh = 0.1f;
	//initParameters.iou_thresh = 0.2f;
	initParameters.save_path = "";
}

}
