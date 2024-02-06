#ifndef GLOBALE_H
#define GLOBALE_H
#include <iostream>
#include <string>
#include <vector>

#include <ctime>
#include <chrono>
#include <sstream>

#include "myutils.h"
#include "minIni.h"

#include<opencv2/core/core.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>

// static int status_global;
extern int status_global ;
#define sizearray(a)  (sizeof(a) / sizeof((a)[0]))

#pragma pack(1)
struct cameraInfo
{
    std::string  str_ip   ="127.0.0.1";
    std::string  str_name ="admin";
    std::string  str_pass ="sw123456";
};
typedef struct Image_vedio {
  int camid = -1;
  int hight=0;
  int width=0;
  cv::Mat image;
  std::string imageBase64;
  std::string imagePath;
  std::string imageTime;
} Image_vedio;


struct PackageHeader
{
    uint16_t u16_start = 0x66AA;
    uint8_t  u8_banben =10;
    uint16_t u16_PackageSize=2;
    uint16_t u16_TrainType = 143;
    uint32_t u32_TrainNum = 25;
    uint8_t  u8_type=1;
    uint16_t u16_len=1;
};

struct PackageRegister
{
    uint8_t  u8_banben =1;
    uint16_t u16_PackageSize=1;
    uint8_t  u8_year =22;
    uint8_t  u8_mon =11;
    uint8_t  u8_day =10;
    uint8_t  u8_hour =11;
    uint8_t  u8_min =10;
    uint8_t  u8_sec =10;
    uint8_t  u8_shbeiType=2;// 0x01：STO  0x02：TODS  0x03：SQD
    uint8_t  u8_baika=1;// 0x01：STO  0x02：TODS  0x03：SQD
    uint32_t  u8_Reserve1 = 0;
    uint16_t  u8_Reserve2 = 0;
    // uint8_t  u8_Reserve[6] ;
};

struct PackageRegisterAck
{
    uint8_t  u8_Version =1;
    uint16_t u16_PackageSize;
    uint8_t  u8_year =22;
    uint8_t  u8_mon =11;
    uint8_t  u8_day =10;
    uint8_t  u8_hour =11;
    uint8_t  u8_min =10;
    uint8_t  u8_sec =10;
    uint8_t  u8_DeviceType=2;// 0x01：STO  0x02：TODS  0x03：SQD
    uint8_t  u8_isJM =0;
    uint8_t     char_FTPIP[64];
    uint8_t     char_FTPuser[32];
    uint8_t     char_FTPpass[32];
    uint8_t     u8_Reserve[8];
};

struct PackageRealTime
{
    uint8_t  u8_Version = 1;
    uint8_t  u8_Device = 2;
    uint8_t  u8_year =22;
    uint8_t  u8_mon =11;
    uint8_t  u8_day =10;
    uint8_t  u8_hour =11;
    uint8_t  u8_min =10;
    uint8_t  u8_sec =10;
    uint64_t  u8_Reserve=0;
    // uint8_t  u8_Reserve[8];
};
struct PackageSelfCheck
{
    uint8_t  u8_Version = 1;
    uint8_t  u8_Device = 2;
    uint8_t  u8_year =22;
    uint8_t  u8_mon =11;
    uint8_t  u8_day =10;
    uint8_t  u8_hour =11;
    uint8_t  u8_min =10;
    uint8_t  u8_sec =10;
    uint8_t  u8_Cam =0;
    uint8_t  u8_LD =0;
    uint8_t  u8_TF03 =0;
    uint8_t  u8_lightsource =0;
    uint8_t  u8_Reserve[4];
};
struct PackageVersion 
{
    uint8_t  u8_Version = 1;
    uint8_t  u8_Device = 2;
    uint8_t  u8_year =22;
    uint8_t  u8_mon =11;
    uint8_t  u8_day =10;
    uint8_t  u8_hour =11;
    uint8_t  u8_min =10;
    uint8_t  u8_sec =10;
    uint8_t  u8_BanKaAll =5;

    uint8_t  u8_mcsIdx =1;
    uint32_t u32_mcs = 0x08010101;
    uint16_t u16_mcs_day04:5,u16_mcs_mon58:4,u16_mcs_year915:7;

    uint8_t  u8_LDIdx =2;
    uint32_t u32_LD = 0x08010101;
    uint16_t u16_LD_day04:5,u16_LD_mon58:4,u16_LD_year915:7;

    uint8_t  u8_IRS1Idx =3;
    uint32_t u32_IRS1 = 0x08010101;
    uint16_t u16_IRS1_day04:5,u16_IRS1_mon58:4,u16_IRS1_year915:7;

    uint8_t  u8_IRS2Idx =4;
    uint32_t u32_IRS2 = 0x08010101;
    uint16_t u16_IRS2_day04:5,u16_IRS2_mon58:4,u16_IRS2_year915:7;

    uint8_t  u8_IRS3Idx =5;
    uint32_t u32_IRS3 = 0x08010101;
    uint16_t u16_IRS3_day04:5,u16_IRS3_mon58:4,u16_IRS3_year915:7;
    // uint8_t  u8_Reserve[6];
    uint32_t  u8_Reserve1 = 0;
    uint16_t  u8_Reserve2 = 0;

};

struct PackageHeartBeatHeader
{
    uint8_t  u8_Version = 1;
    uint16_t u16_Xuhao= 2;
    uint8_t  u8_year =22;
    uint8_t  u8_mon =11;
    uint8_t  u8_day =10;
    uint8_t  u8_hour =11;
    uint8_t  u8_min =10;
    uint8_t  u8_sec =10;
    uint8_t  u8_bakaAll=1;
};
struct PackageHeartBeatData
{
    uint8_t  u8_DeviceType=2;// 0x01：STO  0x02：TODS  0x03：SQD
    uint8_t  u8_BanKaType=2;// 
    uint8_t  u8_VersionData[4];  //要升级的版本固件信息
    uint8_t  u8_FilePathData[50];
    uint8_t  u8_FileNameData[64];
    uint32_t u32_FileSize= 0;//文件大小 Byte
    uint8_t  u8_FileMD5Data[32];
};

struct PackageDownload
{
    uint8_t  u8_Version = 1;
    uint8_t  u8_DeviceType= 2;// 0x01：STO  0x02：TODS  0x03：SQD
    uint16_t u16_Xuhao=1;
    uint32_t u32_ID= 2;
    uint8_t  u8_year =22;
    uint8_t  u8_mon =11;
    uint8_t  u8_day =10;
    uint8_t  u8_hour =11;
    uint8_t  u8_min =10;
    uint8_t  u8_sec =10;
    uint8_t  u8_year_start=22;
    uint8_t  u8_mon_start =11;
    uint8_t  u8_day_start =10;
    uint8_t  u8_hour_start =11;
    uint8_t  u8_min_start =10;
    uint8_t  u8_sec_start =10;
    uint8_t  u8_year_end =22;
    uint8_t  u8_mon_end =11;
    uint8_t  u8_day_end =10;
    uint8_t  u8_hour_end =11;
    uint8_t  u8_min_end =10;
    uint8_t  u8_sec_end =10;
    uint8_t  u8_shbeiType=2;// 0x01：STO  0x02：TODS  0x03：SQD
    uint8_t  u8_FileType=3;  ///0x01，配置文件， 0x02：升级文件 ,3日志文件， 4 图片
    uint8_t  u8_Reserve[8];
};
struct PackageDownloadAck
{
    uint8_t  u8_Version = 1;
    uint8_t  u8_DeviceType= 2;// 0x01：STO  0x02：TODS  0x03：SQD
    uint32_t u32_ID= 2;
    uint8_t  u8_year =22;
    uint8_t  u8_mon =11;
    uint8_t  u8_day =10;
    uint8_t  u8_hour =11;
    uint8_t  u8_min =10;
    uint8_t  u8_sec =10;
    uint8_t  u8_FileType=3;//0x01，配置文件， 0x02：升级文件 ,3日志文件， 4 图片
    uint16_t u16_ACK_Xuhao=1;     //使用发送方的序号
    uint8_t  u8_ACK_Status=1;//返回状态
    uint16_t u16_FileAllNum=1;
    uint16_t u16_FileNum=0;
    uint8_t  u8_FilePathData[50];
    uint8_t  u8_FileNameData[64];
    uint8_t  u8_percentage=0;
    uint32_t u32_Reserve=0;
};
/* u8_ACK_Status
地面超时60s未收到车载如下状态即为异常状态，重新请求：
1:接收到请求命令 
2:取消上传
3:文件准备中，
4:传输中，
5:传输完成
*/
struct PackageUpLoad
{
    uint8_t  u8_Version = 1;
    uint8_t  u8_DeviceType= 2;// 0x01：STO  0x02：TODS  0x03：SQD
    uint16_t u16_Xuhao= 2;
    uint32_t u32_ID= 2;
    uint8_t  u8_year =22;
    uint8_t  u8_mon =11;
    uint8_t  u8_day =10;
    uint8_t  u8_hour =11;
    uint8_t  u8_min =10;
    uint8_t  u8_sec =10;
    uint8_t  u8_FileType=1;  //0x01，配置文件， 无升级文件0x02：升级文件
    uint8_t  u8_FilePathData[50];
    uint8_t  u8_FileNameData[64];
    uint32_t u32_FileSize=2;
    uint8_t  u8_FileMD5Data[32];
    uint32_t u32_Reserve=0;
};
struct PackageUpLoadAck
{
    uint8_t  u8_Version = 1;
    uint8_t  u8_DeviceType= 2;// 0x01：STO  0x02：TODS  0x03：SQD
    uint32_t u32_ID= 2;    //须要使用地面发送的ID
    uint8_t  u8_year =22;
    uint8_t  u8_mon =11;
    uint8_t  u8_day =10;
    uint8_t  u8_hour =11;
    uint8_t  u8_min =10;
    uint8_t  u8_sec =10;
    uint8_t  u8_FileType=1;  ///0x01，配置文件， 0x02：升级文件 ,3日志文件， 4 图片
    uint16_t u16_ACK_Xuhao=1;//使用发送方的序号
    uint8_t  u8_ACK_Status=1;//1 接收到上传命令  2-取消  3-预留 4-传输中 5-传输完成
    uint8_t  u8_percentage=0;
    uint32_t u32_Reserve=0;
};

struct ImgDetectResult
{
    float bbox[4]; //x y w h
    float conf;
    int   class_id;
    float distance; 
};
#pragma pack()



enum ObjDectionType : int {  
  Type_WXYOLOV56_DETECTION = 1,
  Type_WXYOLOV57_DETECTION = 2,
  Type_WXYOLOV7_DETECTION = 3,
  Type_WXYOLOV8_DETECTION = 4,
  Type_WXYOLOV5_SEG_DETECTION = 5,
  Type_WXYOLOV8_SEG_DETECTION = 6,
  Type_FYYOLOV5_DETECTION = 101,
  Type_FYYOLOV6_DETECTION = 102,
  Type_FYYOLOV7_DETECTION = 103,
  Type_FYYOLOV8_DETECTION = 104,
  Type_FKYOLOV8_SEG_DETECTION = 105,
  Type_FKYOLOV8_POSE_DETECTION = 106,
  Type_DEPTH_ANYTHING = 201,
  ObstacleConf_EvaluatorType_TF_PREDICTION_EVALUATOR = 1000
};
#endif //GLOBALE_H

