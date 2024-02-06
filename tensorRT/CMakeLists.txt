cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_BUILD_TYPE "Release")




# cuda
PROJECT(yolov8 VERSION 1.0.0 LANGUAGES C CXX CUDA)
message(*********************************************************)
message(STATUS CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR})
message(*********************************************************)

# include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/common.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/common.cmake)

message(*********************************************************)
message(STATUS TensorRT_ROOT = ${TensorRT_ROOT})
message(*********************************************************)

message(STATUS ${ALL_LIBS})
file(GLOB CPPS 
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/yolov8/*.cu
  ${CMAKE_CURRENT_SOURCE_DIR}/yolov8-seg/*.cu
  ${CMAKE_CURRENT_SOURCE_DIR}/yolov8-pose/*.cu
  ${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cu
  ${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cpp
  ${TensorRT_ROOT}/samples/common/logger.cpp 
  )
# list(REMOVE_ITEM CPPS app_yolov5.cpp)
message(*********************************************************)
message(STATUS CPPS = ${CPPS})
message(*********************************************************)

list (LENGTH CPPS length)
message(*********************************************************)
message(STATUS ***length*** = ${length}) 
message(*********************************************************)

# find_package(OpenCV REQUIRED)

set(OpenCV_INCLUDE_DIRS /dev/shm/opencv/opencv3420/include)
set(OpenCV_LIBRARIES /dev/shm/opencv/opencv3420/lib)

include_directories(${INCLUDE_DRIS} ${OpenCV_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})

message(*********************************************************)
message(STATUS ***INCLUDE_DRIS*** = ${INCLUDE_DRIS})
message(*********************************************************)

message(*********************************************************)
message(STATUS ***OpenCV_INCLUDE_DIRS*** = ${OpenCV_INCLUDE_DIRS})
message(*********************************************************)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)  # 可执行文件的输出目录
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)


add_library(${PROJECT_NAME} SHARED ${CPPS})
target_link_libraries(${PROJECT_NAME} ${ALL_LIBS} ${OpenCV_LIBRARIES})

# set_property(TARGET ${PROJECT_NAME} PROPERTY CUDA_ARCHITECTURES 50 61 72 75 86)
# target_compile_options(${PROJECT_NAME} PUBLIC 
#    $<$<COMPILE_LANGUAGE:CUDA>:--default-stream per-thread -lineinfo; --use_fast_math --disable-warnings>)
  
# add_executable(demo app_yolov5.cpp)

# # NVCC
# # target_link_libraries(detect ${PROJECT_NAME} ${ALL_LIBS} ${OpenCV_LIBRARIES} libgflags_nothreads.a)
# target_link_libraries(demo ${PROJECT_NAME} ${ALL_LIBS} ${OpenCV_LIBRARIES} )
