/*
 * @Author: nyy imniuyuanye@sina.com
 * @Date: 2023-02-06 10:48:30
 * @LastEditors: nyy imniuyuanye@sina.com
 * @LastEditTime: 2023-02-10 15:59:30
 * @FilePath: /gb_detection/src/captureHK.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include "ffDecodecpu.h"

#include "logger.h"
#include "globale.h"

ffDecodecpu::ffDecodecpu(bool stop = false) : stop_(stop){
    std::cout << "ffDecodecpu Constructor" << std::endl;
}
ffDecodecpu::~ffDecodecpu(){
    std::cout << "ffDecodecpu Destructor" << std::endl;
    this->release();
    pthread_mutex_destroy(&this->lock); //destroy mutex
}

void ffDecodecpu::Stop(){
    this->stop_ = true;
}

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
	av_dict_set(&options, "framerate", "30", 0);
  
  // if ((ret = avformat_open_input(&this->ifmt_ctx, this->url.c_str(), 0, &this->avdic)) < 0) {
  // if ((ret = avformat_open_input(&inFmtCtx, "/dev/video0", iformat, &options)) < 0) {
//   if ((ret = avformat_open_input(&this->ifmt_ctx, this->url.c_str(),0,NULL)) < 0) { 
  if ((ret = avformat_open_input(&this->ifmt_ctx, "/dev/video0", iformat, &this->avdic)) < 0) { 
    std::cerr << "[Error]: nyy Couldn't Open Input File." << std::endl;
    this->release();
    return ret;
  }

  if ((ret = avformat_find_stream_info(this->ifmt_ctx, 0)) < 0) {
    std::cerr << "[Error]: Couldn't Find Stream Information." << std::endl;
    this->release();
    return ret;
  }
  av_dump_format(this->ifmt_ctx, 0, this->url.c_str(), 0);

  for (int i = 0; i < this->ifmt_ctx->nb_streams; i++) {
    if (this->ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
      this->video_index = i;
      std::cout << "[INFO]: Stream " << i << " Get VideoStream. " << this->url << std::endl;
      break;
    }
  }

  AVCodecParameters *codec_par = this->ifmt_ctx->streams[video_index]->codecpar;
  this->codec = avcodec_find_decoder(codec_par->codec_id);
  if (this->codec == NULL) {
    std::cerr << "[Error]: Unsupported Codec!" << std::endl;
    this->release();
    return -1;
  }
  this->codec_ctx = avcodec_alloc_context3(this->codec);
  if (this->codec_ctx == NULL) {
    std::cerr << "[Error]: Allocate AVCodecContext Failed!" << std::endl;
    this->release();
    return -1;
  }
  if (avcodec_parameters_to_context(this->codec_ctx, codec_par) < 0) {
    std::cerr << "[Error]: Fill AVCodecContext Failed!" << std::endl;
    this->release();
    return -1;
  }
  if (avcodec_open2(this->codec_ctx, this->codec, nullptr) < 0) {
    std::cerr << "[Error]: Couldn't Open Codec!" << std::endl;
    this->release();
    return -1;
  }

  this->numBytes = av_image_get_buffer_size(AV_PIX_FMT_BGR24, codec_ctx->width, codec_ctx->height, 1);
  if (this->numBytes < 0) {
    std::cerr << "[Error]: Couldn't Get the Buffer Size." << std::endl;
    this->release();
    return -1;
  }
  this->buffer = (uint8_t *)av_malloc(numBytes);
  if (this->buffer == NULL) {
    std::cerr << "[Error]: Memory Block Cannot Be Allocated!" << std::endl;
    this->release();
    return -1;
  }

  this->rawframe = av_frame_alloc();
  this->bgrframe = av_frame_alloc();
  if (this->rawframe == NULL || this->bgrframe == NULL) {
    std::cerr << "[Error]: Frame Memory Block Cannot Be Allocated!" << std::endl;
    this->release();
    return -1;
  }
  // av_image_fill_arrays(this->bgrframe->data, this->bgrframe->linesize, this->buffer, AV_PIX_FMT_BGR24, this->codec_ctx->width, this->codec_ctx->height, 1);
  av_image_fill_arrays(this->bgrframe->data, this->bgrframe->linesize, this->buffer, AV_PIX_FMT_BGR24, 1920, 1080, 1);

  std::cout << "[INFO]: Open Stream Successed. " << this->url << std::endl;
  return 0;
}

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
  std::cout << "[INFO]: releasing... " << this->url << std::endl;
}


int ffDecodecpu::iniffDecodecpu(std::string ip, std::string name,std::string pass,int camera_id){
    // pthread_mutex_init(&this->lock_p,NULL);
    pthread_mutex_init(&this->lock,NULL);
    std::string inifile = "/dev/shm/gb_detection/bin/properties.ini";
    long captureHK_Peroid = ini_getl("switch", "ffDecodecpu_Peroid", 300, inifile.c_str()); 
    this->captureHK_Peroid = captureHK_Peroid;
    LOG_INFO("@@@@@@@@@@@@@@@ ffDecodecpu_Peroid:"+std::to_string(captureHK_Peroid));  

    this->ip=ip;
    this->name=name;
    this->pass=pass;
    this->camera_id=camera_id;

    int ffdecode_len = init();
    if (ffdecode_len < 0)
    {
        LOG_INFO("ffDecodeCpu  status: "<<ffdecode_len); 
        return -1;
    }
    LOG_INFO("ffDecodeCpu   success!!!"<<url);
    return 1;


}



void ffDecodecpu::Run() // 虚函数的重写可以改变成员函数的权限
{
    AVPacket pkt;
    int keyval = 0, ret = 0;
    int rccount = 3;
    
    while (!stop_)
    {       
        auto start = std::chrono::system_clock::now();  

        cv::Mat image;
        ret = av_read_frame(this->ifmt_ctx, &pkt);
        //断线重联机制
        if (ret < 0) {
            std::cerr << "[Error]: Read Frame Error or End Of File." << std::endl;
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

                pthread_mutex_lock(&this->lock); //lock
                frame.copyTo(this->curFrame);
                // ffmpeg中转为base64 
                // this->curFrameBase64=Mat2Base64(this->curFrame, "jpg");  
                pthread_mutex_unlock(&this->lock);//unlock
                
                }
            }
            
        }
        av_packet_unref(&pkt);
            
        std::string current_time = GetCurrentTimeStamp(1);
        EventMessage(curFrame,"vvv",current_time,camera_id);

        auto end = std::chrono::system_clock::now();
        double time_out = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();//ms 
        
        double time_sleep = this->captureHK_Peroid - time_out;//默认周期 300ms
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

