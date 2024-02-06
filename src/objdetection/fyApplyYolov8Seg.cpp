/*** 
 * @Author: nyy
 * @Date: 2023-03-01 21:04:34
 * @LastEditTime: 2024-01-31 14:19:29
 * @LastEditors: nyy
 * @Description: 
 * @FilePath: /irs-orin/src/objdetection/fyApplyYolov8Seg.cpp
 * @可以输入预定的版权声明、个性签名、空行等
 */

#include "fyApplyYolov8Seg.h"
// #include "../../loglib/mylogger.h"
#include "../../log4cplus/mylogger.h"
namespace objdetection{
fyApplyYolov8Seg::fyApplyYolov8Seg(bool stop = false) : stop_(stop){
    LOG_INFO("\n fyApplyYolov8Seg start \n ");
    pthread_mutex_init(&this->lock_p,NULL);
}
fyApplyYolov8Seg::~fyApplyYolov8Seg(){
    LOG_INFO("\n fyApplyYolov8 end \n ");
}

void fyApplyYolov8Seg::Stop(){
    this->stop_ = true;    
}

/** FY yolov8 模型初始化
 * @description: 
 * @return {*}
 */
int fyApplyYolov8Seg::iniModel(){
    
    std::string inifile = "./properties.ini";
    long captureHK_Peroid = ini_getl("FY-SEG", "time_Peroid", 300, inifile.c_str()); 
    this->captureHK_Peroid = captureHK_Peroid;
    LOG_INFO("@@@@@@@@@@@@@@@ ProApply_Peroid:"+std::to_string(captureHK_Peroid));


    long g_bufsize = ini_getl("FY", "g_bufsize", 100, inifile.c_str());   
    LOG_INFO("@@@@@@@@@@@@@@@ g_bufsize:"+std::to_string(g_bufsize)); 

    float NMS_THRESH = ini_getf("FY", "NMS_THRESH", 0.2, inifile.c_str());  
    LOG_INFO("@@@@@@@@@@@@@@@ NMS_THRESH:"+std::to_string(NMS_THRESH)); 

    float CONF_THRESH = ini_getf("FY", "CONF_THRESH", 0.2, inifile.c_str());   
    LOG_INFO("@@@@@@@@@@@@@@@ CONF_THRESH:"+std::to_string(CONF_THRESH)); 


    this->is_show = ini_getl("switch", "is_show", 0, inifile.c_str());  
    LOG_INFO("@@@@@@@@@@@@@@@ is_show:"+std::to_string(is_show));  
    this->is_save = ini_getl("switc", "is_save", 0, inifile.c_str());  
    LOG_INFO("@@@@@@@@@@@@@@@ is_save:"+std::to_string(is_save)); 

    char engineModel[100];
    long n = ini_gets("FY", "yolov8model_seg", "../weigths/yolov8s.trt", engineModel, sizearray(engineModel), const_cast<char *>(inifile.c_str()));
    if (n >= 0) LOG_INFO("enginename"<<":"<<engineModel);
    std::string model_path = engineModel;  

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
	param.batch_size = batch_size;

	// param.src_h = 640;
	// param.src_w = 640;

    // param.src_h = 1920;
    // param.src_w = 1080;

    param.src_h = height;
    param.src_w = width;
    param.conf_thresh = CONF_THRESH;
	param.iou_thresh = NMS_THRESH;

    std::cout<<"total_batches:"<<total_batches<<"\n";
	std::cout<<"param.batch_size:"<<param.batch_size<<"\n";
	// std::cout<<"param.src_h:"<<param.src_h<<"\n";

    param.is_show = is_show;
    param.is_save = is_save;

    // setRenderWindow(param);

    yoloseg = std::make_shared<YOLOv8Seg>(param);
    LOG_INFO("model_path："<<model_path);    

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
	if (!yoloseg->init(trt_file))
	{
		// sample::gLogError << "initEngine() ocur errors!" << std::endl;
        LOG_INFO("initEngine() ocur errors!");
		return -1;
	}
	yoloseg->check();

    /************************************************************************************************
	* recycle
	*************************************************************************************************/

    return 1;
}

int fyApplyYolov8Seg::yolov8SegDect(cv::Mat image){
    cv::Mat img = image.clone();
    if (img.empty()) {
        LOG_INFO("========================= img.empty()========================"); 
        return -1;
    }

    std::vector<cv::Mat> imgs_batch;
    imgs_batch.reserve(param.batch_size);        
    // img = cv::imread(image_path);
    // cv::resize(img, img, cv::Size(640, 640), 0, 0, CV_INTER_LINEAR);//改成大小都为50,50的
    imgs_batch.emplace_back(img.clone());
    // task(yolo, param, imgs_batch, delay_time, batchi, is_show, is_save); 
    yoloseg->copy(imgs_batch);
    utils::DeviceTimer d_t1; yoloseg->preprocess(imgs_batch);  float t1 = d_t1.getUsedTime();
    utils::DeviceTimer d_t2; yoloseg->infer();				  float t2 = d_t2.getUsedTime();
    utils::DeviceTimer d_t3; yoloseg->postprocess(imgs_batch); float t3 = d_t3.getUsedTime();

    // sample::gLogInfo << 
	// 	//"copy time = " << t0 / param.batch_size << "; "
	// 	"preprocess time = " << t1 / param.batch_size << "; "
	// 	"infer time = " << t2 / param.batch_size << "; "
	// 	"postprocess time = " << t3 / param.batch_size << std::endl;


    int delay_time = 1;
    auto start = std::chrono::system_clock::now(); 
    yoloseg->showAndSave(param.class_names, delay_time, imgs_batch);



    // auto start_put = std::chrono::system_clock::now();  



    auto end = std::chrono::system_clock::now();    
    double time_out = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();//ms         
    // LOG_INFO("======showAndSave::"<<time_out);

    yoloseg->reset(); 
    imgs_batch.clear();


}

/** 主函数 tensorRT yolov5 检测
 * @description: 
 * @return {*}
 */
void fyApplyYolov8Seg::Run() // 虚函数的重写可以改变成员函数的权限
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
        std::string img_path = video_data->imagePath;
        yolov8SegDect(img);   
        auto end = std::chrono::system_clock::now();
        double time_out = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();//ms 
        
        double time_sleep = this->captureHK_Peroid - time_out;//默认周期 300ms
        // LOG_INFO("yolov8-seg time:"<< time_out);
        // start =  end;
        // LOG_INFO("captureHK_Peroid:"<< captureHK_Peroid);
        // LOG_INFO("time_sleep:"<< time_sleep);



        if(time_sleep > 0){            
            // usleep(time_sleep*1000);
        }       
    }
}


/** 设置yolov8初始化参数
 * @description: 
 * @param {InitParameter&} initParameters
 * @return {*}
 */
void fyApplyYolov8Seg::setParameters(utils::InitParameter& initParameters)
{
    initParameters.class_names = utils::dataSets::coco80;
	//initParameters.class_names = utils::dataSets::voc20;
	initParameters.num_class = 80; // for coco
	//initParameters.num_class = 20; // for voc2012
	initParameters.batch_size = 8;
	initParameters.dst_h = 640;
	initParameters.dst_w = 640;
	initParameters.input_output_names = { "images",  "output0" };
	initParameters.conf_thresh = 0.25f;
	initParameters.iou_thresh = 0.7f;
	initParameters.save_path = "D:/Data/1/";
}


/** 回调函数
 * @description: 
 * @param {Mat} img
 * @param {string} img_path
 * @param {string} image_time
 * @param {int} camid
 * @return {*}
 */
void fyApplyYolov8Seg::eventFunc(cv::Mat img, std::string img_path,std::string image_time,int camid)
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


}
