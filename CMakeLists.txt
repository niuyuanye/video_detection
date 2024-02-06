cmake_minimum_required(VERSION 3.0)
project(demo)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/./bin)  # 可执行文件的输出目录
include_directories(
    "/usr/local/include"
    "/usr/local/include/eigen3"
    "/dev/shm/opencv/opencv3420/include"
    "/dev/shm/opencv/opencv3420/include/opencv/"
    "/dev/shm/opencv/opencv3420/include/opencv2/"
    # "/usr/local/opencv452/include/opencv4"
    "./minIni"   
    "./log4cplus/libx64/log4cplus/include"
    "./HK/include"
    "./lib_json/include"
    "/usr/local/cuda-11/include"
    "/usr/local/cuda-11/targets/x86_64-linux/include"
    # "/dev/shm/TensorRT-8.4.1.5/targets/x86_64-linux-gnu/include"
    # "/dev/shm/TensorRT-8.4.1.5/samples/common"
    # "/dev/shm/TensorRT-8.4.3.1/targets/x86_64-linux-gnu/include"
    # "/dev/shm/TensorRT-8.4.3.1/samples/common"
    "/dev/shm/TensorRT-8.6.1.6/targets/x86_64-linux-gnu/include"
    "/dev/shm/TensorRT-8.6.1.6/samples/common"
    "./src/yolov5_v7"
    "/usr/local/ffmpeg4.4/include"
    )
    # include_directories(/usr/local/opencv452/include/opencv4)
    # link_directories(/usr/local/opencv452/lib/)
    # "./log4cplus" # "./tensorRT"

link_directories(
    "/usr/local/lib"
    "/dev/shm/opencv/opencv3420/lib"
    # "/usr/local/opencv452/lib"
    "./bin"
    "./log4cplus/libx64/log4cplus/lib"
    "./HK/libx64"
    "./lib_json/libx64"
    "/usr/local/cuda-11/lib64"
    "/usr/local/cuda-11/targets/x86_64-linux/lib"
    # "/dev/shm/TensorRT-8.4.1.5/targets/x86_64-linux-gnu/lib"
    # "/dev/shm/TensorRT-8.4.3.1/targets/x86_64-linux-gnu/lib"
    "/dev/shm/TensorRT-8.6.1.6/targets/x86_64-linux-gnu/lib"
    "/usr/local/ffmpeg4.4/lib"    
    )

# file(GLOB SRCC "src/*.c")
# ADD_LIBRARY(ini SHARED ${SRCC})
# cuda_add_library(myplugins SHARED ${PROJECT_SOURCE_DIR}/yololayer.cu)
# target_link_libraries(myplugins nvinfer cudart)


# file(GLOB SRC "src/*.cpp" "src/*.c")
# file(GLOB SRC "src/*.cpp" "src/yolov8/*.cpp" "src/yolov5_v6/*.cpp" "src/objdetection/*.cpp")
# file(GLOB SRC "src/*.cpp" "src/yolov8/*.cpp" "src/yolov5_v7/*.cpp" "src/objdetection/*.cpp")
# file(GLOB SRC "src/*.cpp" "src/yolov8/*.cpp"  "src/yolov8seg/*.cpp" "src/yolov5_v7/*.cpp" "src/objdetection/*.cpp")
file(GLOB SRC "src/*.cpp" "src/yolov8/*.cpp"  "src/yolov8-seg/*.cpp" "src/yolov8-pose/*.cpp" "src/yolov5_v7/*.cpp" "src/DepthAnything/*.cpp" "src/objdetection/*.cpp")

file(GLOB SRC_log "/dev/shm/TensorRT-8.6.1.6/samples/common/logger.cpp")
message(${SRC})

# {TensorRT_ROOT}/samples/common/logger.cpp 
# file(GLOB CPPS 
#   ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
#   ${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cu
#   ${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cpp
#   ${TensorRT_ROOT}/samples/common/logger.cpp 
#   )

add_executable(demo ${SRC} ${SRC_log} )
# add_executable(demo ${SRC} ${SRC_yolov5_v6} ${SRC_yolov5_v6s} ${SRC_yolov8} ${SRC_yolov8s})
# add_executable(demo ${SRC} ${SRC_yolov5_v6} ${SRC_yolov8} ${SRC_log} )
target_link_libraries(demo pthread)
target_link_libraries(demo jsoncpp)
target_link_libraries(demo log4cplus)
target_link_libraries(demo loglib)
target_link_libraries(demo minIni)
target_link_libraries(demo opencv_world)
target_link_libraries(demo hcnetsdk )
target_link_libraries(demo PlayCtrl )
target_link_libraries(demo avformat)
target_link_libraries(demo avcodec)
target_link_libraries(demo avutil)
target_link_libraries(demo swresample)
target_link_libraries(demo swscale)
target_link_libraries(demo nvinfer)
target_link_libraries(demo cudart)
# target_link_libraries(demo myplugins)
target_link_libraries(demo mypluginsv57)
target_link_libraries(demo yolov8)



set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE "Release" )

