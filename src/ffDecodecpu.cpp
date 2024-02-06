/*
 * @Author: nyy imniuyuanye@sina.com
 * @Date: 2023-02-20 09:08:22
 * @LastEditors: nyy imniuyuanye@sina.com
 * @LastEditTime: 2023-03-28 09:42:43
 * @FilePath: /video_detection/src/ffDecodecpu.cpp
 * @Description: 完成ffmpeg 软解码、I帧解码 及回调发送数据
   这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include "ffDecodecpu.h"
#include "../log4cplus/mylogger.h"
// #include "../log4cplus/logger.h"
#include "globale.h"

ffDecodecpu::ffDecodecpu(bool stop = false) : stop_(stop){
    LOG_INFO("\n ffDecodecpu start \n ");
}
ffDecodecpu::~ffDecodecpu(){
    LOG_INFO("\n ffDecodecpu end \n ");
    this->release();
    pthread_mutex_destroy(&this->lock_p); //destroy mutex
}

void ffDecodecpu::Stop(){
    this->stop_ = true;
}
/**ffmpeg 初始化核心函数
 * @description: 
 * @return {*}
 */
int ffDecodecpu::init()
{
  std::cout << "[INFO]: Opening Stream: " << this->url << std::endl;

  char option_key[] = "rtsp_transport";
  char option_value[] = "tcp";
  av_dict_set(&this->avdic, option_key, option_value, 0);
  char option_key2[] = "stimeout";
  char option_value2[] = "5000000";
  av_dict_set(&this->avdic, option_key2, option_value2, 0);

  int ret = -1;

  // avcodec_register_all();
  //获得格式
	AVInputFormat* iformat = av_find_input_format("video4linux2"); 
	//打开设备
	AVFormatContext* inFmtCtx = NULL;
  //  AVFormatContext *inFmtCtx = avformat_alloc_context();
	//const char* device_name = "hw:0,0";//这个是音频
	const char* device_name = "/dev/video0";
  AVDictionary* options = NULL;
 
	//视频就需要做options了
	av_dict_set(&options, "video_size", "640*480", 0);
	av_dict_set(&options, "framerate", "25", 0);
  // this->url = "rtsp://admin:sw123456@192.168.110.221:554";
  LOG_INFO("message 00");  
  
  // if ((ret = avformat_open_input(&this->ifmt_ctx, this->url.c_str(), 0, &this->avdic)) < 0) {
  if ((ret = avformat_open_input(&this->ifmt_ctx, this->url.c_str(), iformat, &this->avdic)) < 0) {
  // if ((ret = avformat_open_input(&inFmtCtx, "rtsp://admin:sw123456@192.168.110.221:554", 0, &this->avdic)) < 0) {
  //   if ((ret = avformat_open_input(&this->ifmt_ctx, this->url.c_str(),0,NULL)) < 0) { 
  // if ((ret = avformat_open_input(&this->ifmt_ctx, "fire2.mp4",0,NULL)) < 0) { 
  // if ((ret = avformat_open_input(&this->ifmt_ctx, "fire2.mp4", iformat, &this->avdic)) < 0) {
  // if ((ret = avformat_open_input(&this->ifmt_ctx, "person.mp4",0,NULL)) < 0) { 
  // if ((ret = avformat_open_input(&this->ifmt_ctx, "person.mp4", iformat, &this->avdic)) < 0) {
  // if ((ret = avformat_open_input(&this->ifmt_ctx, "/dev/video0", 0, &this->avdic)) < 0) { 
    // std::cerr << "[Error]: nyy Couldn't Open Input File." << std::endl;
    LOG_INFO("[Error]: nyy Couldn't Open Input File.");  
    this->release();
    return ret;
  }
  

  if ((ret = avformat_find_stream_info(this->ifmt_ctx, 0)) < 0) {
    LOG_INFO("[Error]: Couldn't Find Stream Information.");  
    this->release();
    return ret;
  }
  av_dump_format(this->ifmt_ctx, 0, this->url.c_str(), 0);

  for (int i = 0; i < this->ifmt_ctx->nb_streams; i++) {
    if (this->ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
      this->video_index = i;
      LOG_INFO("Stream "<< i << " Get VideoStream. " << this->url);  
      // std::cout << "[INFO]: Stream " << i << " Get VideoStream. " << this->url << std::endl;
      break;
    }
  }

  AVCodecParameters *codec_par = this->ifmt_ctx->streams[video_index]->codecpar;
  this->codec = avcodec_find_decoder(codec_par->codec_id);
  if (this->codec == NULL) {
    // std::cerr << "[Error]: Unsupported Codec!" << std::endl;
    LOG_INFO("[Error]: Couldn't Find Stream Information."); 
    this->release();
    return -1;
  }
  this->codec_ctx = avcodec_alloc_context3(this->codec);
  if (this->codec_ctx == NULL) {
    // std::cerr << "[Error]: Allocate AVCodecContext Failed!" << std::endl;
    LOG_INFO("[Error]: Allocate AVCodecContext Failed!"); 
    this->release();
    return -1;
  }
  if (avcodec_parameters_to_context(this->codec_ctx, codec_par) < 0) {
    // std::cerr << "[Error]: Fill AVCodecContext Failed!" << std::endl;
    LOG_INFO("[Error]: Fill AVCodecContext Failed!"); 
    this->release();
    return -1;
  }
  if (avcodec_open2(this->codec_ctx, this->codec, nullptr) < 0) {
    // std::cerr << "[Error]: Couldn't Open Codec!" << std::endl;
    LOG_INFO("[Error]: Couldn't Open Codec!"); 
    this->release();
    return -1;
  }

  this->numBytes = av_image_get_buffer_size(AV_PIX_FMT_BGR24, codec_ctx->width, codec_ctx->height, 1);
  if (this->numBytes < 0) {
    // std::cerr << "[Error]: Couldn't Get the Buffer Size." << std::endl;
    LOG_INFO("[Error]: Couldn't Get the Buffer Size."); 
    this->release();
    return -1;
  }
  this->buffer = (uint8_t *)av_malloc(numBytes);
  if (this->buffer == NULL) {
    // std::cerr << "[Error]: Memory Block Cannot Be Allocated!" << std::endl;
    LOG_INFO("[Error]: Memory Block Cannot Be Allocated!"); 
    this->release();
    return -1;
  }

  this->rawframe = av_frame_alloc();
  this->bgrframe = av_frame_alloc();
  if (this->rawframe == NULL || this->bgrframe == NULL) {
    // std::cerr << "[Error]: Frame Memory Block Cannot Be Allocated!" << std::endl;
    LOG_INFO("[Error]: Frame Memory Block Cannot Be Allocated!"); 
    this->release();
    return -1;
  }
  // av_image_fill_arrays(this->bgrframe->data, this->bgrframe->linesize, this->buffer, AV_PIX_FMT_BGR24, this->codec_ctx->width, this->codec_ctx->height, 1);
  av_image_fill_arrays(this->bgrframe->data, this->bgrframe->linesize, this->buffer, AV_PIX_FMT_BGR24, 1920, 1080, 1);

  // std::cout << "[INFO]: Open Stream Successed. " << this->url << std::endl;
  LOG_INFO(" Open Stream Successed. " << this->url); 
  return 0;
} 
/**ffmpeg 资源释放
 * @description: 
 * @return {*}
 */
void ffDecodecpu::release()
{
  if (this->img_cvrt_ctx != NULL) {
    sws_freeContext(this->img_cvrt_ctx);
    this->img_cvrt_ctx = NULL;
  }
  if (this->buffer != NULL) {
    av_free(this->buffer);
    this->buffer = NULL;
  }
  if (this->rawframe != NULL) {
    av_frame_free(&this->rawframe);
    this->rawframe = NULL;
  }
  if (this->bgrframe != NULL) {
    av_frame_free(&this->bgrframe);
    this->bgrframe = NULL;
  }
  if (this->codec_ctx != NULL) {
    avcodec_free_context(&this->codec_ctx);
    this->codec_ctx = NULL;
  }
  if (this->avdic != NULL) {
    av_dict_free(&this->avdic);
    this->avdic = NULL;
  }
  if (this->ifmt_ctx != NULL) {
    avformat_close_input(&this->ifmt_ctx);
    this->ifmt_ctx = NULL;
  }
  // std::cout << "[INFO]: releasing... " << this->url << std::endl;
  LOG_INFO("releasing... "<< this->url);  
}

/** ffmpeg 解码 trsp 视频流 初始化 包括trsp url的拼接
 * @description: 
 * @param {string} ip
 * @param {string} name
 * @param {string} pass
 * @param {int} camera_id
 * @return {*}
 */
int ffDecodecpu::iniffDecodecpu(std::string ip, std::string name,std::string pass,int camera_id){
    // pthread_mutex_init(&this->lock_p,NULL);
    pthread_mutex_init(&this->lock_p,NULL);
    std::string inifile = "./properties.ini";
    long captureHK_Peroid = ini_getl("switch", "captureHK_Peroid", 300, inifile.c_str()); 
    this->captureHK_Peroid = captureHK_Peroid;
    LOG_INFO("@@@@@@@@@@@@@@@ ffDecodecpu_Peroid:"+std::to_string(captureHK_Peroid));  

    this->width = ini_getl("FY", "width", 640, inifile.c_str());   
    LOG_INFO("@@@@@@@@@@@@@@@ width:"+std::to_string(width)); 

    this->height = ini_getl("FY", "height", 480, inifile.c_str());   
    LOG_INFO("@@@@@@@@@@@@@@@ height:"+std::to_string(height)); 



    this->ip=ip;
    this->name=name;
    this->pass=pass;
    this->camera_id=camera_id;
    std::string url_tmp = "rtsp://"+name+":"+pass+"@"+ ip+":554";//admin:sw123456@192.168.110.221:554";
    // std::string url_tmp = "rtsp://"+name+":"+pass+"@"+ ip+":554/ch1/main/av_stream";//admin:sw123456@192.168.110.221:554";
    this->url=url_tmp;

    // this->url="/dev/shm/person.mp4";

    int ffdecode_len = init();
    if (ffdecode_len < 0)
    {
        LOG_INFO("ffDecodeCpu  status: "<<ffdecode_len); 
        return -1;
    }
    LOG_INFO("ffDecodeCpu   success!!!"<<url);
    return 1;
}
/** ffmpeg 解码数据转换为 opencv Mat 图片
 * @description: 
 * @param {AVFrame *} frame
 * @param {AVPixelFormat} pixFormat
 * @return {*}
 */
cv::Mat ffDecodecpu::avframeToCvmat(const AVFrame * frame,AVPixelFormat pixFormat)
{
    // int width = 1920;
    // int height = 1080;
    // int width = 960;
    // int height = 720;
    // int width = 640;
    // int height = 480;
    cv::Mat image(height, width, CV_8UC3);
    int cvLinesizes[1];
    cvLinesizes[0] = image.step1();
    SwsContext* conversion = sws_getContext(frame->width, frame->height, pixFormat , width, height, AVPixelFormat::AV_PIX_FMT_BGR24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
    sws_scale(conversion, frame->data, frame->linesize, 0, frame->height, &image.data, cvLinesizes);
    sws_freeContext(conversion);
    return image;
}

/** 主函数 数据流的ffmpeg解码 及回调函数发送
 * @description: 
 * @return {*}
 */
void ffDecodecpu::Run() // 虚函数的重写可以改变成员函数的权限
{
    AVPacket pkt;
    int keyval = 0, ret = 0;
    int rccount = 10;
 
    LOG_INFO("start run ........ 第"<<camera_id<<"个");
    
    while (!stop_)
    {       
        auto start = std::chrono::system_clock::now();  

        cv::Mat image;
        ret = av_read_frame(this->ifmt_ctx, &pkt);
        //断线重联机制
        if (ret < 0) {
            // std::cerr << "[Error]: Read Frame Error or End Of File." << std::endl;
            LOG_INFO("[Error]: Read Frame Error or End Of File.");
            //timeout or re-connect
            int connected = 0;
            for (int ri = 0; ri < rccount; ri++) {
                this->release();
                if (!(this->init()<0)) { connected = 1; break; }
            }
            if (connected) { continue; } else { break; }
        }



        if (pkt.stream_index == this->video_index) { 
          // JUST RECV KEY FRAME
          if (!(pkt.flags & AV_PKT_FLAG_KEY)) {
              av_packet_unref(&pkt);
              continue;
          }
            ret = avcodec_send_packet(this->codec_ctx, &pkt);
            if (ret < 0) {
                std::cerr << "[Error]: avcodec_send_packet encounter error." << std::endl;
                //timeout or re-connect
                int connected = 0;
                for (int ri = 0; ri < rccount; ri++) {
                this->release();
                if (!(this->init()<0)) { connected = 1; break; }
                }
                if (connected) { continue; } else { break; }
            }

            while (ret >= 0) {
                ret = avcodec_receive_frame(this->codec_ctx, this->rawframe);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                break;
                }
                else if (ret < 0) {
                std::cerr << "[Error]: avcodec_receive_frame encounter error." << std::endl;
                break;
                }
            if (ret >= 0) {
                AVPixelFormat pixFormat;
                switch (this->codec_ctx->pix_fmt) {
                    case AV_PIX_FMT_YUVJ420P:
                    pixFormat = AV_PIX_FMT_YUV420P;
                    break;
                    case AV_PIX_FMT_YUVJ422P:
                    pixFormat = AV_PIX_FMT_YUV422P;
                    break;
                    case AV_PIX_FMT_YUVJ444P:
                    pixFormat = AV_PIX_FMT_YUV444P;
                    break;
                    case AV_PIX_FMT_YUVJ440P:
                    pixFormat = AV_PIX_FMT_YUV440P;
                    break;
                    default:
                    pixFormat = this->codec_ctx->pix_fmt;
                    break;
                }
                cv::Mat frame = avframeToCvmat(rawframe,pixFormat);
                std::string current_time = GetCurrentTimeStamp(1);


                
                // if(camera_id==2){
                //   // LOG_INFO("frame.cols:"<<frame.cols);
                //     cv::imshow("frame", frame);
                //     cv::waitKey(1);
                // }
                // cv::imshow("frame", frame);
                // cv::waitKey(1);

                frame.copyTo(this->curFrame);
                this->EventMessage(this->curFrame,"vvv","current_time",this->camera_id);
                // LOG_INFO("this->camera_id:"<<this->camera_id);
                
                
                }
            }
            
        }
        av_packet_unref(&pkt);

        
            
        

        auto end = std::chrono::system_clock::now();
        double time_out = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();//ms 
        
        double time_sleep = this->captureHK_Peroid - time_out;//默认周期 300ms
        if (curFrame.empty()) {
            LOG_INFO("========================= curFrame.empty()========================"); 
        }

        // cv::Mat img(640, 480, CV_8UC3);      
        // cv::Mat img = cv::Mat::zeros(640, 480, CV_8UC3);      
        // LOG_INFO("img.cols:"<<img.cols<<"第"<<camera_id<<"个");
        // this->EventMessage(img,"vvv","current_time",this->camera_id);
        // usleep(100*1000);
        // LOG_INFO("img.rows:"<<img.rows<<"第"<<camera_id<<"个");


        


        // if(time_sleep > 0){            
        //     usleep(time_sleep*1000);
        // }

        // sleep(1);
        // usleep(30*1000);
    }
}

/** 回调函数 发送数据
 * @description: 
 * @param {Mat} img
 * @param {string} img_path
 * @param {string} image_time
 * @param {int} camid
 * @return {*}
 */
void ffDecodecpu::EventMessage(cv::Mat img,std::string img_path,std::string image_time,int camid)//event 发送数据
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
